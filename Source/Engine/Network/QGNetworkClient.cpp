
#include <Network/QGNetworkClient.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>

#define QGNETCODE_CONNECT_TOKEN_EXPIRY 30
#define QGNETCODE_CONNECT_TOKEN_TIMEOUT 5

void QGNetworkClient::Connect(const char* address) {
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
    netcode_random_bytes(user_data, NETCODE_USER_DATA_BYTES);

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
        memcpy(&qgpacket.env.type, packet + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        memcpy(&qgpacket.env.size, packet + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        memcpy(&qgpacket.env.tick, packet + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(&qgpacket.env.client, &m_clientID, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(&qgpacket.env.ack, packet + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        QGASSERT(packet_bytes - offset == qgpacket.env.size, "Packet size mismatch.");

        qgpacket.bytes = (unsigned char*)malloc(qgpacket.env.size);
        memcpy(qgpacket.bytes, packet + offset, qgpacket.env.size);

        // Ack back
        if (qgpacket.env.ack == 1)
            this->SendAckPacket(qgpacket.env.tick);

        // Process as an ack packet?
        if (qgpacket.env.type == QGPACKET_ACK)
            this->HandleAckPacket(&qgpacket);

        this->ProcessPacket(&qgpacket);

        (void)packet_sequence;
        netcode_client_free_packet(m_client, packet);
    }

    m_time += delta;
}

void QGNetworkClient::Send(uint32_t type, unsigned char* data, uint32_t size, bool ack) {
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
    int newSize = size + sizeof(QGNetworkEnvelope);
    unsigned char* newBytes = (unsigned char*)malloc(newSize);

    int offset = 0;
    memcpy(newBytes + offset, &type, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    memcpy(newBytes + offset, &size, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint64_t tick = timeSystem->Tick();
    memcpy(newBytes + offset, &tick, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(newBytes + offset, &m_clientID, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    int iack = (int)ack;
    memcpy(newBytes + offset, &ack, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    memcpy(newBytes + offset, data, size);
    offset += size;

    if (ack) {
        // Store a copy to be re-sent as necessary
        m_ackPacketTicks[tick] = tick;

        // Store a copy of the packet
        QGNetworkPacket* packetcopy = new QGNetworkPacket();
        packetcopy->env.ack = ack;
        packetcopy->env.client = m_clientID;
        packetcopy->env.size = size;
        packetcopy->env.tick = tick;
        packetcopy->env.type = type;

        unsigned char* cpybytes = (unsigned char*)malloc(newSize);
        memcpy(cpybytes, data, size);
        packetcopy->bytes = cpybytes;

        m_ackPackets[tick] = packetcopy;
    }

    netcode_client_send_packet(m_client, newBytes, offset);

    free(newBytes);
}

void QGNetworkClient::SendAckPacket(uint64_t sequence_num) {
    QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

    QGNetworkAckPacket packet;
    packet.sequence_num = sequence_num;
    packet.tick = timeSystem->Tick();

    unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkAckPacket));
    int offset = 0;

    memcpy(bytes + offset, &packet.sequence_num, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(bytes + offset, &packet.tick, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    this->Send(QGPACKET_ACK, bytes, offset, false);

    free(bytes);
}

void QGNetworkClient::HandleAckPacket(QGNetworkPacket* packet) {
    // Read packet tick and ID
    uint64_t sequence_num;
    uint64_t packetTick;
    int offset = 0;

    memcpy(&sequence_num, packet->bytes + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(&packetTick, packet->bytes + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    // Remove from ackable packet list
    m_ackPacketTicks.erase(sequence_num);

    // Clean up
    QGNetworkPacket* packetcopy = m_ackPackets[sequence_num];
    delete packetcopy;
    m_ackPackets.erase(sequence_num);
}