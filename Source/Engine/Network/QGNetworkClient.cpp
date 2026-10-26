
#include <Network/QGNetworkClient.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>

#define QGNETCODE_CONNECT_TOKEN_EXPIRY 30
#define QGNETCODE_CONNECT_TOKEN_TIMEOUT 5

void QGNetworkClient::Initialize() {
    this->QGNetworkSystem::Initialize();
    this->RegisterPacketCallback(QGNetworkPackets::QGPACKET_STARTUP, &HandleStartupPacket);
}

void QGNetworkClient::Connect(const char* address, std::string token) {
    m_time = 0.0;

    static uint8_t private_key[NETCODE_KEY_BYTES] = { 0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea,
                                                  0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4,
                                                  0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
                                                  0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1 };

    struct netcode_client_config_t client_config;
    netcode_default_client_config(&client_config);
    m_client = netcode_client_create("0.0.0.0", &client_config, m_time);

    m_clientID = 0;
    netcode_random_bytes((uint8_t*)&m_clientID, 8);
    printf("client id is %llu\n", m_clientID);

    uint8_t user_data[NETCODE_USER_DATA_BYTES];
    memset(user_data, 0, NETCODE_USER_DATA_BYTES);
    memcpy(user_data, token.c_str(), token.length());

    uint8_t connect_token[NETCODE_CONNECT_TOKEN_BYTES];

    if (netcode_generate_connect_token(1, &address, &address, QGNETCODE_CONNECT_TOKEN_EXPIRY, QGNETCODE_CONNECT_TOKEN_TIMEOUT, m_clientID, QGNETCODE_PROTOCOL_ID, private_key, user_data, connect_token) != NETCODE_OK)
    {
        QGASSERT(false, "Failed to generate connect token.");
        return;
    }

    netcode_client_connect(m_client, connect_token);
}

void QGNetworkClient::Update(float delta) {
    netcode_client_update(m_client, m_time);

    // Loop until there are no more packets to process
    while(true)
    {
        int packet_bytes;
        uint64_t packet_sequence;
        unsigned char* packet = netcode_client_receive_packet(m_client, &packet_bytes, &packet_sequence);
        if (!packet) break;

        // Create a packet to be processed
        QGNetworkPacket qgpacket;
        
        int offset = 0;
        memcpy(&qgpacket.env.id, packet + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(&qgpacket.env.type, packet + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        memcpy(&qgpacket.env.size, packet + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        memcpy(&qgpacket.env.tick, packet + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        //memcpy(&qgpacket.env.client, &m_clientID, sizeof(uint64_t));
        //offset += sizeof(uint64_t);

        memcpy(&qgpacket.env.start, packet + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        memcpy(&qgpacket.env.total, packet + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        memcpy(&qgpacket.env.ack, packet + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        QGASSERT(packet_bytes - offset == qgpacket.env.size, "Packet size mismatch: %d - %d = %d.", packet_bytes, offset, qgpacket.env.size);

        qgpacket.bytes = (unsigned char*)malloc(qgpacket.env.size);
        memcpy(qgpacket.bytes, packet + offset, qgpacket.env.size);

        // Do we have the whole packet?
        bool wholePacket = false;
        if (qgpacket.env.total == qgpacket.env.size) {
            wholePacket = true;
            this->ProcessPacket(&qgpacket);
        }
        else {
            // Or is it in pieces...
            printf("Received packet fragment of ID %llu, range %d to %d of %d.\n", 
                qgpacket.env.id, qgpacket.env.start, qgpacket.env.size, qgpacket.env.total);

            auto it = m_packetBytes.find(qgpacket.env.id);
            if (it == m_packetBytes.end()) {
                m_packetBytes[qgpacket.env.id] = qgpacket.env.size;

                // Create a copy of the packet
                QGNetworkPacket* packetcopy = new QGNetworkPacket();
                packetcopy->env.id = qgpacket.env.id;
                packetcopy->env.ack = qgpacket.env.ack;
                packetcopy->env.client = qgpacket.env.client;
                packetcopy->env.size = qgpacket.env.total;
                packetcopy->env.tick = qgpacket.env.tick;
                packetcopy->env.start = 0;
                packetcopy->env.total = qgpacket.env.total;
                packetcopy->env.type = qgpacket.env.type;

                unsigned char* cpybytes = (unsigned char*)malloc(qgpacket.env.total);
                memcpy(cpybytes, 0, qgpacket.env.total);
                packetcopy->bytes = cpybytes;

                m_packetFragments[qgpacket.env.id] = packetcopy;
            }
            else {
                m_packetBytes[qgpacket.env.id] += qgpacket.env.size;
            }

            // Copy this packet's bytes into the whole
            memcpy(m_packetFragments[qgpacket.env.id]->bytes + qgpacket.env.start, qgpacket.bytes, qgpacket.env.size);

            if (m_packetBytes[qgpacket.env.id] == qgpacket.env.total) {
                // Now we have a whole packet
                this->ProcessPacket(m_packetFragments[qgpacket.env.id]);
                wholePacket = true;

                // Clean up after processing
                delete m_packetFragments[qgpacket.env.id];
                m_packetFragments.erase(qgpacket.env.id);
                m_packetBytes.erase(qgpacket.env.id);
            }
        }

        // Ack back if necessary
        if (wholePacket == true) {
            if (qgpacket.env.ack == 1)
                this->SendAckPacket(qgpacket.env.id);

            // Process as an ack packet?
            if (qgpacket.env.type == QGPACKET_ACK)
                this->HandleAckPacket(&qgpacket);
        }

        (void)packet_sequence;
        netcode_client_free_packet(m_client, packet);
    }

    m_time += delta;

    // Handle any ack packets that need to be re-sent
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
    uint64_t currentTick = timeSystem->Tick();

    // Check the last sync time for clients and send sync where needed
    if (m_lastSyncTick < (currentTick - QGNETWORK_CLIENT_SYNC_TICKS)) {
        this->SendSyncPacket();
        m_lastSyncTick = currentTick;
    }

    // Determine response time from average RTT (plus 20%)
    if (m_avgRTT) {
        int avgRTTTicks = std::ceil(std::ceil(((float)m_avgRTT / 1000.0f) * (float)QG_TICKS_PER_SECOND) * 2.0f);
        auto it = m_packets.begin();
        for (; it != m_packets.end(); it++) {
            if (it->second->env.tick < (currentTick - avgRTTTicks)) {
                // Resend
                printf("Resending packet type %d from tick %llu.\n", it->second->env.type, it->second->env.tick);
                this->Send(it->second->env.type, it->second->bytes, it->second->env.size, it->second->env.ack);

                // Remove any re-sent packets
                it = m_packets.erase(it);
                continue;
            }
            else {
                break;
            }
        }
    }
}

void QGNetworkClient::Send(uint32_t type, unsigned char* data, uint32_t size, bool ack) {
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
    uint64_t tick = timeSystem->Tick();

    m_nextPacketID++;
    for (int s = 0; s < size; s += QG_NETWORK_MAX_PACKET_SIZE) {
        uint32_t fragmentSize = std::min((int)size - s, QG_NETWORK_MAX_PACKET_SIZE);

        int newSize = fragmentSize + sizeof(QGNetworkEnvelope);
        unsigned char* newBytes = (unsigned char*)malloc(newSize);

        int offset = 0;
        memcpy(newBytes + offset, &m_nextPacketID, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(newBytes + offset, &type, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        memcpy(newBytes + offset, &fragmentSize, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        memcpy(newBytes + offset, &tick, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        //memcpy(newBytes + offset, &m_clientID, sizeof(uint64_t));
        //offset += sizeof(uint64_t);

        uint16_t start = s;
        memcpy(newBytes + offset, &start, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        uint16_t total = size;
        memcpy(newBytes + offset, &total, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        int iack = (int)ack;
        memcpy(newBytes + offset, &ack, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        memcpy(newBytes + offset, data, fragmentSize);
        offset += fragmentSize;

        netcode_client_send_packet(m_client, newBytes, offset);
        free(newBytes);
    }

    if (ack) {
        // Store a copy of the packet
        QGNetworkPacket* packetcopy = new QGNetworkPacket();
        packetcopy->env.id = m_nextPacketID;
        packetcopy->env.ack = ack;
        packetcopy->env.client = m_clientID;
        packetcopy->env.size = size;
        packetcopy->env.tick = tick;
        packetcopy->env.type = type;
        packetcopy->env.start = 0;
        packetcopy->env.total = size;

        unsigned char* cpybytes = (unsigned char*)malloc(size);
        memcpy(cpybytes, data, size);
        packetcopy->bytes = cpybytes;

        m_packets[m_nextPacketID] = packetcopy;
    }
}

void QGNetworkClient::SendAckPacket(uint64_t sequence_num) {
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

    QGNetworkAckPacket packet;
    packet.sequence_num = sequence_num;
    packet.tick = timeSystem->Tick();

    timespec ts;
    timeSystem->Timestamp(&ts);
    packet.sec = ts.tv_sec;
    packet.nsec = ts.tv_nsec;

    unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkAckPacket));
    int offset = 0;

    memcpy(bytes + offset, &packet.sequence_num, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(bytes + offset, &packet.tick, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(bytes + offset, &packet.sec, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(bytes + offset, &packet.nsec, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    this->Send(QGPACKET_ACK, bytes, offset, false);

    free(bytes);
}

void QGNetworkClient::HandleAckPacket(QGNetworkPacket* packet) {
    // Read packet tick and ID
    uint64_t sequence_num;
    uint64_t packetTick;
    uint32_t sec; uint64_t nsec;
    int offset = 0;

    memcpy(&sequence_num, packet->bytes + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(&packetTick, packet->bytes + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(&sec, packet->bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&nsec, packet->bytes + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    timespec serverts;
    serverts.tv_sec = sec;
    serverts.tv_nsec = nsec;

    // Get current tick time
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
    uint64_t currentTick = timeSystem->Tick();

    // Check packet type
    if (m_packets.find(sequence_num) == m_packets.end()) {
        printf("Got ack packet back for unknown packet ID %llu.\n", sequence_num);
        return;
    }

    QGNetworkPacket* ackPacket = m_packets[sequence_num];
    uint64_t ackTick = ackPacket->env.tick;
    if (ackPacket->env.type == QGPACKET_SYNC) {
        // Use to establish a rough RTT based on one direction
        uint64_t diff = ((float)currentTick - ackTick) * (1.0f / QG_TICKS_PER_SECOND) * 1000;

        // Push on and take one off
        QGNetworkClient* client = GetQGSystem<QGNetworkClient>();
        client->m_rtts.push_back((int)diff);
        if (client->m_rtts.size() > 10) {
            client->m_rtts.erase(client->m_rtts.begin());
        }

        // Recalc RTT
        int avg = 0;
        int points = 0;
        int weight = client->m_rtts.size();
        for (auto it = client->m_rtts.begin(); it != client->m_rtts.end(); it++) {
            avg += (*it) * weight;
            points += weight;
        }
        avg /= points;
        client->m_avgRTT = avg;

        printf("Average RTT: %d ms, tick %llu\n", avg, currentTick);

        // Figure out what our current timestamp should be
        if (client->m_rtts.size() >= 5) {
            int RTTinTicks = ((int64_t)currentTick - ackTick);
            int clientServerDiffinTicks = (int64_t)(currentTick - (RTTinTicks / 2) - packetTick);

            float adjust = ((float)clientServerDiffinTicks / QG_TICKS_PER_SECOND);

            //printf("Adjust: %f seconds\n", adjust);

            timespec startupTime = timeSystem->StartupTime();
            timespec clientts = timeSystem->StartupTime();
            int seconds = std::floor(adjust);
            int nanoseconds = std::floor((adjust - seconds) * 1000000000);
            clientts.tv_sec += seconds;
            clientts.tv_nsec = nanoseconds;

            // After initial set, set every X seconds
            if (currentTick > (m_lastTimeSet + (QG_TICKS_PER_SECOND * QGNETWORK_CLIENT_RESYNC_TIME))) {
                timeSystem->StartupTime(clientts);
                uint64_t nowTick = timeSystem->Tick();
                
                printf("Syncing startup time, tick was %llu, adjust %f s, now %llu...\n", currentTick, adjust, nowTick);
                m_lastTimeSet = currentTick;
            }
        }
    }

    // Clean up
    QGNetworkPacket* packetcopy = m_packets[sequence_num];
    delete packetcopy;
    m_packets.erase(sequence_num);
}

void QGNetworkClient::HandleStartupPacket(QGNetworkPacket* packet) {
    QGNetworkClient* client = GetQGSystem<QGNetworkClient>();
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

    client->m_lastSyncTick = timeSystem->Tick() + QGNETWORK_CLIENT_SYNC_TICKS;
}

void QGNetworkClient::SendSyncPacket() {
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

    QGNetworkSyncPacket packet;
    packet.tick = timeSystem->Tick();

    unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkSyncPacket));
    int offset = 0;

    memcpy(bytes + offset, &packet.tick, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    this->Send(QGPACKET_SYNC, bytes, offset, true);

    free(bytes);
}