
#ifndef qgnetworkclient_h
#define qgnetworkclient_h

#include <Network/QGNetworkSystem.h>
#include <Network/QGNetworkPackets.h>

#define QGNETWORK_CLIENT_SYNC_TICKS	20

class QUEST_API QGNetworkClient : public QGNetworkSystem {
public:
	QGNetworkClient() : m_client(0), m_clientID(0), m_avgRTT(0), m_lastSyncTick(0) { }
	~QGNetworkClient() = default;

	/**
	 * Initialize
	 */
	void Initialize();

	/**
	 * Connect securely to a server
	 */
	void Connect(const char* address, std::string token);

	/**
	 * Send a packet to the server
	 */
	void Send(uint32_t type, unsigned char* data, uint32_t size, bool ack);

	/**
	 * Check for any received packets and process
	 */
	void Update(float delta);

	/**
	 * Get client ID
	 */
	uint64_t ClientID() { return m_clientID; }

protected:
	// Send ack packet
	void SendSyncPacket();
	void SendAckPacket(uint64_t sequence_num);
	void HandleAckPacket(QGNetworkPacket* packet);
	static void HandleSyncPacket(QGNetworkPacket* packet);

protected:
	struct netcode_client_t* m_client;
	uint64_t m_clientID;

	std::map<uint64_t, uint64_t> m_ackPacketTicks;
	std::map<uint64_t, QGNetworkPacket*> m_ackPackets;

	uint64_t m_lastSyncTick;
	std::vector<int> m_rtts;
	int m_avgRTT;
};

#endif