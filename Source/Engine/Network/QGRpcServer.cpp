
#include <Network/QGRpcServer.h>
#include <Network/QGNetworkServer.h>
#include <Core/QGApplication.h>
#include <Core/QGWorld.h>

void QGRpcServer::Initialize() {
	QGNetworkServer* networkSystem = GetQGSystem<QGNetworkServer>();
	networkSystem->RegisterPacketCallback(QGPACKET_RPCREQUEST, RpcRequestHandlingCallback);
}

void QGRpcServer::RpcRequestHandlingCallback(QGNetworkPacket* packet) {
	QGRpcServer* rpc = GetQGSystem<QGRpcServer>();

	// Get the entity this is associated with
	QGWorld* world = QGWorld::GetInstance();
	QGEntity* entity = world->FindEntity(packet->env.client);

	// Read packet bytes
	int offset = 0;

	// Read length of string
	uint8_t funclen = 0;
	memcpy(&funclen, packet->bytes + offset, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	// Read string
	char* funcname = (char*)malloc(funclen);
	memset(funcname, 0, funclen);
	memcpy(funcname, packet->bytes + offset, funclen);
	offset += funclen;

	// Read tick
	uint64_t tick = 0;
	memcpy(&tick, packet->bytes + offset, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	// Read size of args
	uint32_t argsize = 0;
	memcpy(&argsize, packet->bytes + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	QGDataRecord record;
	if (argsize) {
		record.Deserialize(packet->bytes + offset, argsize);
	}

	printf("Called RPC function %s... ", funcname);

	// Call the callback function
	auto it = rpc->m_callbacks.find(funcname);
	QGASSERT(it != rpc->m_callbacks.end(), "No callback function found.");
	std::string response = it->second(funcname, &record, entity);

	// Send the response
	QGNetworkServer* networkSystem = GetQGSystem<QGNetworkServer>();

	// Create packet
	int packetSize = sizeof(uint8_t);
	packetSize += funclen;
	packetSize += sizeof(uint64_t);
	packetSize += sizeof(uint32_t);
	packetSize += response.length() + 1;

	unsigned char* bytes = (unsigned char*)malloc(packetSize);
	memset(bytes, 0, packetSize);
	offset = 0;

	memcpy(bytes + offset, &funclen, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	memcpy(bytes + offset, funcname, funclen);
	offset += funclen;

	memcpy(bytes + offset, &tick, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	uint32_t responseLength = response.length();
	memcpy(bytes + offset, &responseLength, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(bytes + offset, response.c_str(), response.length());
	offset += response.length() + 1;

	printf("Sending RPC response.\n");

	// Send
	int clientIndex = networkSystem->ClientIndexFromID(packet->env.client);
	networkSystem->Send(clientIndex, QGPACKET_RPCRESPONSE, bytes, offset, true);
	free(bytes);
}