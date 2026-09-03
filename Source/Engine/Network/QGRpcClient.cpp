
#include <Network/QGRpcClient.h>
#include <Network/QGNetworkClient.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>

void QGRpcClient::Initialize() {
	QGNetworkClient* networkSystem = GetQGSystem<QGNetworkClient>();
	networkSystem->RegisterPacketCallback(QGPACKET_RPCRESPONSE, RpcResponseHandlingCallback);
}

void QGRpcClient::Call(std::string name, QGDataRecord* args, QGRpcClientHandlingCallback cb) {
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	QGNetworkClient* networkSystem = GetQGSystem<QGNetworkClient>();

	// Store for callback
	PendingCallback* pcb = new PendingCallback();
	pcb->tick = timeSystem->Tick();
	pcb->func = name;

	// Serialize args
	unsigned int size = 0;
	if (args) {
		args->Serialize(0, size);

		pcb->args = (unsigned char*)malloc(size);
		args->Serialize(pcb->args, size);
	}

	// Save callback
	pcb->cb = cb;
	m_callbacks.push_back(pcb);

	// Make the call
	int packetSize = 0;
	packetSize += sizeof(uint8_t);
	packetSize += name.length() + 1;
	packetSize += sizeof(uint64_t);
	packetSize += sizeof(uint32_t);
	packetSize += size;

	// Assemble packet data
	unsigned char* bytes = (unsigned char*)malloc(packetSize);
	memset(bytes, 0, packetSize);
	int offset = 0;

	uint8_t nameLength = name.length() + 1;
	memcpy(bytes + offset, &nameLength, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	memcpy(bytes + offset, name.c_str(), name.length());
	offset += nameLength;

	memcpy(bytes + offset, &pcb->tick, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	memcpy(bytes + offset, &size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if (args) {
		memcpy(bytes + offset, pcb->args, size);
		offset += size;
	}

	QGASSERT(offset == packetSize, "Packet size mismatch.");

	printf("Calling RPC function %s on server.\n", name.c_str());
	networkSystem->Send(QGPACKET_RPCREQUEST, bytes, packetSize, true);
	free(bytes);
}

void QGRpcClient::RpcResponseHandlingCallback(QGNetworkPacket* packet) {
	QGRpcClient* rpc = GetQGSystem<QGRpcClient>();

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

	// Read response length
	uint32_t length = 0;
	memcpy(&length, packet->bytes + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	// Read response
	char* response = (char*)malloc(length + 1);
	memset(response, 0, length + 1);
	memcpy(response, packet->bytes + offset, length);

	printf("Received RPC response to function %s.\n", funcname);

	// Attempt to parse response to JSON
	std::string jsonstr = response;
	nlohmann::json obj = nlohmann::json::parse(jsonstr);

	// Attempt to find the caller
	std::string funcstr = funcname;
	for (auto it = rpc->m_callbacks.begin(); it != rpc->m_callbacks.end(); it++) {
		if ((*it)->func == funcstr && (*it)->tick == tick) {
			(*it)->cb(funcstr, obj);
		}
	}
}