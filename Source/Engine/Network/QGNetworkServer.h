
#ifndef qgnetworkserver_h
#define qgnetworkserver_h

#include <Network/QGNetworkSystem.h>

#define QGNETWORK_SERVER_SYNC_TICKS	10

class QUEST_API QGNetworkServer : public QGNetworkSystem {
public:
	QGNetworkServer() : m_server(0) {}
	~QGNetworkServer() = default;

	/**
	 * Initialize
	 */

	/**
	 * Listen on an IP address/port for new connections
	 */
	void Listen(const char* address);

	/**
	 * Update and process any received packets from clients
	 */
	void Update(float delta);

	/**
	 * Send a packet to a client
	 */
	void Send(int client_index, uint32_t type, unsigned char* data, uint32_t size, bool ack);

	/**
	 * Get the average round trip time for a client
	 */
	int AverageRTT(int client_index);

protected:
	// Callbacks
	static void HandleConnectDisconnect(void* context, int client_index, int connected);
	static void HandleAckPacket(QGNetworkPacket* packet);

protected:
	void SendAckPacket(uint64_t sequence_num, int client_index);
	void SendSyncPacket(int client_index);

protected:
	struct QGConnectedClient {
		std::map<uint64_t, uint64_t> m_ackPacketTicks;
		std::map<uint64_t, QGNetworkPacket*> m_ackPackets;
		uint64_t lastSyncTick;

		std::vector<int> rtts;
		int avgRTT;
	};

protected:
	// Internal server
	struct netcode_server_t* m_server;

	// Ack packets
	std::map<int, QGConnectedClient*> m_clients;
	std::map<uint64_t, int> m_clientIDtoIndex;
};

#endif