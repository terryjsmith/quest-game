
#ifndef qgreplicationserver_h
#define qgreplicationserver_h

#include <Network/QGNetworkSystem.h>
#include <Core/QGEvent.h>
#include <Core/QGEntity.h>
#include <Network/QGEntitySnapshot.h>

#define QG_REPLICATION_SNAPSHOT_HISTORY		3		// Snapshot history (in seconds)

class QUEST_API QGReplicationServer : public QGSystem {
public:
	QGReplicationServer() {
		m_playerCreateFunc = 0;
		m_replaying = false;
	}
	~QGReplicationServer() = default;

	/**
	 * Initialize and listen to world changing events
	 */
	void Initialize();

	/**
	 * Update and broadcast world state
	 */
	void Update(float delta);

	/**
	 * Set a template class to be instantiated for each new player
	 */
	template<class T> void SetPlayerClass() {
		m_playerCreateFunc = CreatePlayer<T>;
	}

public:
	// Callbacks
	static void HandleInputCommandReceived(QGNetworkPacket* packet);
	static void HandleClientConnectEvent(QGEvent* ev, QGObject* obj);
	static void HandleClientDisconnectEvent(QGEvent* ev, QGObject* obj);

	struct QGReplicationEvent {
		QGEvent* event;
		QGObject* obj;
	};

	// Internal function to call a C++ player constructor
	typedef QGEntity* (*PlayerCreateFunc)();

protected:
	void SendStartupPacket(int client_index);

	// Create a new C++ object based on a registered object type
	template<typename T> static QGEntity* CreatePlayer() { return new T; }

protected:
	// Map of player IDs to player entities
	std::map<uint64_t, QGEntity*> m_playerEntities;

	// Map of ticks to entity snapshots
	std::map<uint64_t, QGEntitySnapshot> m_snapshots;

	// Map of connected clients to player IDs
	std::map<uint64_t, int> m_playerClientIndexes;

	// History of input command events received from clients to replay
	std::map<uint64_t, std::vector<QGReplicationEvent*>> m_eventHistory;

	// Oldest tick we need to replay back to in next loop
	bool m_replaying;
	std::map<uint64_t, bool> m_replayTicks;

	// Player creation
	PlayerCreateFunc m_playerCreateFunc;
};

#endif