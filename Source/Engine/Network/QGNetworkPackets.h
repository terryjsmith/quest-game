
#ifndef qgnetworkpackets_h
#define qgnetworkpackets_h

#include <Core/QGObject.h>
#include <Core/QGDataRecord.h>

enum QGNetworkPackets {
	QGPACKET_STARTUP			= 10,
	QGPACKET_ACK				= 20,
	QGPACKET_SYNC				= 30,
	QGPACKET_INPUTCOMMAND		= 40,
	QGPACKET_ENTITYSNAPSHOT		= 50,
	QGPACKET_RPCREQUEST			= 60,
	QGPACKET_RPCRESPONSE		= 70
};

/**
 * Network envelope
 */
struct QGNetworkEnvelope {
	uint64_t id;		// Unique ID of the packet
	uint8_t type;		// Packet type from QGNetworkPackets
	uint32_t size;		// Size of this packet
	uint64_t tick;		// Packet tick
	uint64_t client;	// Which client is sending the packet (not transmitted)
	uint16_t start;		// The starting byte offset of this packet
	uint16_t total;		// The total number of bytes for the packet - total == size for a single packet
	uint8_t ack;		// Whether we want an ack response
};

/**
 * Network packet
 */
class QGNetworkPacket {
public:
	QGNetworkPacket() : bytes(0) {}
	~QGNetworkPacket() {
		if (bytes) free(bytes);
	}

public:
	QGNetworkEnvelope env;
	unsigned char* bytes;
};

struct QGNetworkStartupPacket {
	uint32_t nsec;
	uint64_t sec;
};

struct QGNetworkCommandPacket {
	uint32_t command;
	float state;
};

struct QGNetworkSyncPacket {
	uint64_t tick;
};

struct QGNetworkAckPacket {
	uint64_t sequence_num;
	uint64_t tick;
	uint32_t nsec;
	uint64_t sec;
};

struct QGNetworkRpcRequestPacket {
	std::string function;
	unsigned char* args;
};

struct QGNetworkRpcResponsePacket {
	std::string function;
	std::string response;
};

#endif