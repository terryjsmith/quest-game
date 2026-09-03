
#ifndef qgrpcserver_h
#define qgrpcserver_h

#include <Core/QGSystem.h>
#include <Core/QGSerializable.h>
#include <Core/QGEntity.h>
#include <Network/QGNetworkPackets.h>

/**
 * RPC handling callback
 */
typedef std::string(*QGRpcHandlingCallback)(std::string func, QGDataRecord* args, QGEntity* entity);

class QUEST_API QGRpcServer : public QGSystem {
public:
	QGRpcServer() = default;
	~QGRpcServer() = default;

	/**
	 * Initialize
	 */
	void Initialize();

	/**
	 * Bind a function to a name
	 */
	void Bind(std::string name, QGRpcHandlingCallback func) {
		m_callbacks[name] = func;
		printf("Binding RPC function %s...\n", name.c_str());
	}

	/**
	 * Callbacks
	 */
	static void RpcRequestHandlingCallback(QGNetworkPacket* packet);

protected:
	std::map<std::string, QGRpcHandlingCallback> m_callbacks;
};

#endif
