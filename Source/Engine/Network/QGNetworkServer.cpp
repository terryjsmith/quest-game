
#include <Network/QGNetworkServer.h>
#include <Core/QGApplication.h>
#include <Core/QGEventSystem.h>
#include <Network/QGNetworkEvents.h>
#include <Network/QGNetworkPackets.h>
#include <Core/QGTimeSystem.h>

void QGNetworkServer::HandleConnectDisconnect(void* context, int client_index, int connected) {
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	QGNetworkServer* server = GetQGSystem<QGNetworkServer>();

	uint64_t client_id = netcode_server_client_id(server->m_server, client_index);

	if (connected == 1) {
		// Publish connected event
		eventSystem->Publish(new QGNetworkClientConnectEvent(client_index, client_id));

		QGConnectedClient* client = new QGConnectedClient();
		server->m_clients[client_index] = client;
		server->m_clientIDtoIndex[client_id] = client_index;
	}
	else {
		eventSystem->Publish(new QGNetworkClientDisconnectEvent(client_index, netcode_server_client_id(server->m_server, client_index)));
		delete server->m_clients[client_index];
		server->m_clients.erase(client_index);
	}
}

void QGNetworkServer::Listen(const char* address) {
	// Define private key (TODO: move this to server config later)
	static uint8_t private_key[NETCODE_KEY_BYTES] = { 0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea,
												  0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4,
												  0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
												  0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1 };
	// Start with default config and tweak
	struct netcode_server_config_t server_config;
	netcode_default_server_config(&server_config);

	server_config.protocol_id = QGNETCODE_PROTOCOL_ID;
	server_config.connect_disconnect_callback = &HandleConnectDisconnect;
	memcpy(&server_config.private_key, private_key, NETCODE_KEY_BYTES);

	double time = 0.0f;
	m_server = netcode_server_create(address, &server_config, time);

	netcode_server_start(m_server, NETCODE_MAX_CLIENTS);
}

void QGNetworkServer::Update(float delta) {
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	uint64_t tick = timeSystem->Tick();

	netcode_server_update(m_server, m_time);

	uint8_t packet_data[NETCODE_MAX_PACKET_SIZE];
	for (int i = 0; i < NETCODE_MAX_PACKET_SIZE; ++i)
		packet_data[i] = (uint8_t)i;

	for (int i = 0; i < NETCODE_MAX_CLIENTS; ++i)
	{
		if (netcode_server_client_connected(m_server, i) == 0) continue;

		while (true)
		{
			int packet_bytes;
			uint64_t packet_sequence;
			unsigned char* packet = netcode_server_receive_packet(m_server, i, &packet_bytes, &packet_sequence);
			if (!packet)
				break;

			// Create a packet to be processed
			QGNetworkPacket qgpacket;

			int offset = 0;
			memcpy(&qgpacket.env.type, packet + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);

			memcpy(&qgpacket.env.size, packet + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&qgpacket.env.tick, packet + offset, sizeof(uint64_t));
			offset += sizeof(uint64_t);

			// client id - never trust the client
			offset += sizeof(uint64_t);
			qgpacket.env.client = netcode_server_client_id(m_server, i);

			memcpy(&qgpacket.env.ack, packet + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);

			QGASSERT(packet_bytes - offset == qgpacket.env.size, "Packet size mismatch.");

			qgpacket.bytes = (unsigned char*)malloc(qgpacket.env.size);
			memcpy(qgpacket.bytes, packet + offset, qgpacket.env.size);

			// Ack back if necessary
			if (qgpacket.env.ack == 1)
				this->SendAckPacket(qgpacket.env.tick, i);

			// Process as an ack packet?
			if (qgpacket.env.type == QGPACKET_ACK)
				this->HandleAckPacket(&qgpacket);

			this->ProcessPacket(&qgpacket);

			netcode_server_free_packet(m_server, packet);
		}

		// Check the last sync time for clients and send sync where needed
		if (m_clients[i]->lastSyncTick < (tick - QGNETWORK_SERVER_SYNC_TICKS)) {
			this->SendSyncPacket(i);
			m_clients[i]->lastSyncTick = tick;
		}

		// TODO: Check ack packets for responses and re-send if necessary
	}

	m_time += delta;
}

void QGNetworkServer::Send(int client_index, uint32_t type, unsigned char* data, uint32_t size, bool ack) {
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

	uint64_t client_id = netcode_server_client_id(m_server, client_index);
	memcpy(newBytes + offset, &client_id, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	int iack = (int)ack;
	memcpy(newBytes + offset, &iack, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	memcpy(newBytes + offset, data, size);
	offset += size;

	// If we need an ack back on this, record the sequence ID and tick we sent it
	if (ack) {
		m_clients[client_index]->m_ackPacketTicks[tick] = tick;

		// Store a copy of the packet
		QGNetworkPacket* packetcopy = new QGNetworkPacket();
		packetcopy->env.ack = ack;
		packetcopy->env.client = client_id;
		packetcopy->env.size = size;
		packetcopy->env.tick = tick;
		packetcopy->env.type = type;

		unsigned char* cpybytes = (unsigned char*)malloc(newSize);
		memcpy(cpybytes, data, size);
		packetcopy->bytes = cpybytes;

		m_clients[client_index]->m_ackPackets[tick] = packetcopy;
	}

	netcode_server_send_packet(m_server, client_index, newBytes, offset);

	free(newBytes);
}

void QGNetworkServer::SendAckPacket(uint64_t sequence_num, int client_index) {
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

	this->Send(client_index, QGPACKET_ACK, bytes, offset, false);

	free(bytes);
}

void QGNetworkServer::SendSyncPacket(int client_index) {
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

	QGNetworkSyncPacket packet;
	packet.tick = timeSystem->Tick();

	unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkSyncPacket));
	int offset = 0;

	memcpy(bytes + offset, &packet.tick, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	this->Send(client_index, QGPACKET_SYNC, bytes, offset, true);

	free(bytes);
}

void QGNetworkServer::HandleAckPacket(QGNetworkPacket* packet) {
	QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	uint64_t tick = timeSystem->Tick();

	int client_index = server->m_clientIDtoIndex[packet->env.client];

	// Get connected client
	QGConnectedClient* client = server->m_clients[client_index];

	// Read packet tick and ID
	uint64_t sequence_num;
	uint64_t packetTick;
	int offset = 0;

	memcpy(&sequence_num, packet->bytes + offset, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	memcpy(&packetTick, packet->bytes + offset, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	// Get ackable packet tick
	uint64_t ackTick = client->m_ackPacketTicks[sequence_num];
	uint64_t diff = ((float)tick - ackTick) * (1.0f / QG_TICKS_PER_SECOND) * 1000;

	// Push on and take one off
	client->rtts.push_back((int)diff);
	if (client->rtts.size() > 10) {
		client->rtts.erase(client->rtts.begin());
	}

	// Recalc RTT
	int avg = 0;
	auto it = client->rtts.begin();
	for (; it != client->rtts.end(); it++) {
		avg += (*it);
	}
	avg /= client->rtts.size();
	client->avgRTT = avg;

	// Remove from ackable packet list
	server->m_clients[client_index]->m_ackPacketTicks.erase(sequence_num);

	// Clean up
	QGNetworkPacket* packetcopy = server->m_clients[client_index]->m_ackPackets[sequence_num];
	delete packetcopy;
	server->m_clients[client_index]->m_ackPackets.erase(sequence_num);
}

int QGNetworkServer::AverageRTT(int client_index) {
	return(m_clients[client_index]->avgRTT);
}