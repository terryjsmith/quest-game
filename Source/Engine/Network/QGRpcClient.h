
#ifndef qgrpcclient_h
#define qgrpcclient_h

#include <Core/QGSystem.h>
#include <Core/QGDataRecord.h>
#include <Network/QGNetworkPackets.h>

/**
 * RPC handling callback
 */
typedef void(*QGRpcClientHandlingCallback)(std::string func, nlohmann::json response);

class QUEST_API QGRpcClient : public QGSystem {
public:
	QGRpcClient() = default;
	~QGRpcClient() = default;

	/**
	 * Initialize
	 */
	void Initialize();

	/**
	 * Call a function and set callback
	 */
	void Call(std::string name, QGDataRecord* args, QGRpcClientHandlingCallback cb);

	/**
	 * Callbacks
	 */
	static void RpcResponseHandlingCallback(QGNetworkPacket* packet);

protected:
	struct PendingCallback {
		uint64_t tick;
		std::string func;
		QGRpcClientHandlingCallback cb;
		unsigned char* args;
	};

	std::vector<PendingCallback*> m_callbacks;
};

#endif