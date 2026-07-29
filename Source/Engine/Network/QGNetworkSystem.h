
#ifndef qgnetworksystem_h
#define qgnetworksystem_h

#define QGNETCODE_PROTOCOL_ID	0x1122334455667788

#include <Core/QGSystem.h>
#include <netcode.h>
#include <Network/QGNetworkPackets.h>

/**
 * Packet handling callback
 */
typedef void(*QGPacketHandlingCallback)(QGNetworkPacket* packet);

/**
 * Network system base class
 */
class QUEST_API QGNetworkSystem : public QGSystem {
public:
	QGNetworkSystem() : m_time(0.0f) {}
	~QGNetworkSystem() = default;

	virtual void Initialize();

	/**
	 * Process packet receivers
	 */
	void ProcessPacket(QGNetworkPacket* packet);

	/**
	 * Register a type of packet and handler function
	 */
	void RegisterPacketCallback(uint8_t type, QGPacketHandlingCallback func);

protected:
	std::map<uint8_t, std::vector<QGPacketHandlingCallback>> m_handlers;
	double m_time;
};

#endif