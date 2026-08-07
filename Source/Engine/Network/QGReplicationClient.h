
#ifndef qgreplicationclient_h
#define qgreplicationclient_h

#include <Network/QGNetworkSystem.h>
#include <Core/QGEvent.h>
#include <Network/QGEntitySnapshot.h>

#define	QG_REPLICATION_CLIENT_SNAPSHOT_HISTORY		3		// Client snapshot history (in seconds)
#define QG_REPLICATION_CLIENT_LAG					5		// Client lag snapshots for rendering (in ticks)

class QUEST_API QGReplicationClient : public QGSystem {
public:
	QGReplicationClient() : m_playerID(0) {}
	~QGReplicationClient() = default;

	/**
	 * Register to receive input commands (client) 
	 */
	void Initialize();

	/**
	 * Send any messages to the server and receive and process new snapshots
	 */
	void Update(float delta);

	/** 
	 * Set player ID for this session
	 */
	void PlayerID(uint64_t playerID) { m_playerID = playerID; }
	uint64_t PlayerID() { return m_playerID; }

public:
	// Callbacks
	static void HandleNetworkStartupPacket(QGNetworkPacket* packet);
	static void HandleEntitySnapshotPacket(QGNetworkPacket* packet);

	// Process input commands
	static void InputCommandReceived(QGEvent* ev, QGObject* obj);

protected:
	// Entity snapshots received
	std::map<uint64_t, QGEntitySnapshot> m_snapshots;

	// Player ID for session
	uint64_t m_playerID;

	// Player entity
	QGEntity* m_playerEntity;
};

#endif