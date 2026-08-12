
#include <Network/QGReplicationServer.h>
#include <Core/QGApplication.h>
#include <Network/QGNetworkPackets.h>
#include <Core/QGEventSystem.h>
#include <Network/QGNetworkServer.h>
#include <Network/QGNetworkEvents.h>
#include <Core/QGWorld.h>
#include <IO/QGInputSystem.h>
#include <IO/QGInputCommand.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGMetaSystem.h>
#include <Network/QGReplicationClient.h>

void QGReplicationServer::Initialize() {
	// Receive input packets
	QGNetworkSystem* networkSystem = GetQGSystem<QGNetworkSystem>();
	networkSystem->RegisterPacketCallback(QGPACKET_INPUTCOMMAND, &HandleInputCommandReceived);

	// Also receive all events
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	eventSystem->Subscribe<QGNetworkClientConnectEvent>(&HandleClientConnectEvent);
	eventSystem->Subscribe<QGNetworkClientDisconnectEvent>(&HandleClientDisconnectEvent);
}

void QGReplicationServer::Update(float delta) {
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	uint64_t currentTick = timeSystem->Tick();

	QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();

	// Get a list of all entities in the world
	QGWorld* world = QGWorld::GetInstance();
	std::map<uint64_t, QGEntity*> entities = world->GetEntities();

	// Get oldest replay tick
	uint64_t replayTick = 0;
	if (m_replayTicks.size()) {
		bool foundSnapshot = false;
		uint64_t possibleReplayTick = m_replayTicks.begin()->first;

		// Do we have a snapshot for this tick?
		int attempts = 0, maxAttempts = 5;
		while (attempts < maxAttempts) {
			auto it = m_snapshots.find(possibleReplayTick);
			if (it != m_snapshots.end()) {
				foundSnapshot = true;
				replayTick = possibleReplayTick;
				m_replayTicks.clear();
			}
			possibleReplayTick--;
			attempts++;
		}
		
		if(foundSnapshot == false) {
			printf("No snapshot found for tick %llu.\n", possibleReplayTick);
		}
	}

	// If we have a valid replay tick, go back and replay
	if (replayTick > 0 && m_replaying == false) {
		printf("Entering replay mode...\n");
		m_replaying = true;

		// Remove all saved snapshots after this point
		for (uint64_t i = replayTick + 1; i < currentTick; i++) {
			m_snapshots.erase(i);
		}

		// Get the saved snapshot
		QGEntitySnapshot* snapshot = &m_snapshots[replayTick];

		// Remove all non-player entities not included in the snapshot
		for (auto eit = entities.begin(); eit != entities.end(); eit++) {
			// Check if this is a player entity
			bool isPlayer = m_playerEntities.find(eit->first) != m_playerEntities.end();

			// Check if it is NOT in the snapshot and is NOT a player entity
			auto sit = snapshot->entities.find(eit->first);
			if (sit == snapshot->entities.end() && isPlayer == false) {
				// Not in world, remove
				world->RemoveEntity(eit->first);
			}
			else {
				// If this is a player and they were not yet in the snapshot, skip
				if (sit == snapshot->entities.end() && isPlayer == true) continue;

				// Is in world, reset to recorded state
				eit->second->Deserialize(&sit->second.record);

				// Loop over components
				std::vector<QGComponent*> components = eit->second->GetComponents();
				for (auto cit = components.begin(); cit != components.end(); cit++) {
					QGObjectType* type = (*cit)->Type();
					auto iit = sit->second.components.find(type->typeID);

					// If it's not in the list...
					if (iit == sit->second.components.end()) {
						// .. and it should be, remove it
						if(type->networkSync == true)
							eit->second->RemoveComponent(*cit);
					}
					else {
						// If it is in the list, deserialize it
						(*cit)->Deserialize(&(*iit).second.record);
					}
				}
			}
		}

		QGApplication* application = QGApplication::GetInstance();
		QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
		while (replayTick <= currentTick) {
			printf("Replaying tick %llu.\n", replayTick);
			timeSystem->OverrideTick(replayTick);

			// Process any events that happened in this tick
			auto it = m_eventHistory.find(replayTick);
			if (it != m_eventHistory.end()) {
				for (auto eit = it->second.begin(); eit != it->second.end(); eit++) {
					printf("Replaying event.\n");
					eventSystem->Publish((*eit)->event, (*eit)->obj);
				}
			}

			// Update
			application->Update(1.0f / (float)QG_TICKS_PER_SECOND);

			// Make sure we keep current
			timeSystem->OverrideTick(0);
			currentTick = timeSystem->Tick();

			// Advance
			replayTick++;
		}

		// Reset
		printf("Exiting replay mode.\n");
		m_replaying = false;

		timeSystem->OverrideTick(0);
	}

	// Take a snapshot of the world in the current tick
	QGEntitySnapshot snapshot;
	snapshot.tick = currentTick;

	// Get all entities in the world
	for (auto eit = entities.begin(); eit != entities.end(); eit++) {
		// Serialize entity
		QGEntitySnapshot::QGSerializedEntity serializedEntity;
		eit->second->Serialize(&serializedEntity.record);

		// Serialize components
		std::vector<QGComponent*> components = eit->second->GetComponents();
		for (auto cit = components.begin(); cit != components.end(); cit++) {
			// Get the type
			QGObjectType* type = (*cit)->Type();
			if (type->networkSync == false) continue;

			QGEntitySnapshot::QGSerializedComponent serializedComponent;
			(*cit)->Serialize(&serializedComponent.record);

			serializedComponent.type = type->typeID;

			serializedEntity.components[type->typeID] = serializedComponent;
		}

		snapshot.entities[eit->first] = serializedEntity;
	}

	m_snapshots.erase(currentTick);
	m_snapshots[currentTick] = snapshot;

	// Broadcast
	QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
	unsigned int snapshotSize = 0;
	unsigned char* snapshotBytes = snapshot.Serialize(snapshotSize);
	for (auto cit = m_playerClientIndexes.begin(); cit != m_playerClientIndexes.end(); cit++) {
		server->Send(cit->second, QGPACKET_ENTITYSNAPSHOT, snapshotBytes, snapshotSize, false);
	}

	free(snapshotBytes);

	// Clean up old snapshots
	uint64_t cleanupTick = currentTick - (QG_REPLICATION_SNAPSHOT_HISTORY * QG_TICKS_PER_SECOND);
	auto it = m_snapshots.find(cleanupTick);
	if (it != m_snapshots.end()) {
		m_snapshots.erase(it);
	}
}

void QGReplicationServer::HandleInputCommandReceived(QGNetworkPacket* packet) {
	QGReplicationServer* server = GetQGSystem<QGReplicationServer>();
	QGNetworkServer* networkServer = GetQGSystem<QGNetworkServer>();
	QGInputSystem* inputSystem = GetQGSystem<QGInputSystem>();
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

	// Read in the input command
	QGNetworkCommandPacket inputCommand;
	int offset = 0;

	memcpy(&inputCommand.command, packet->bytes + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&inputCommand.state, packet->bytes + offset, sizeof(float));
	offset += sizeof(float);

	printf("Received an input command of type %d set to value %f from client %llu.\n", inputCommand.command, inputCommand.state, packet->env.client);

	// Find the client entity and process an event
	std::string cmd = inputSystem->GetCommandName(inputCommand.command);
	QGEntity* entity = server->m_playerEntities[packet->env.client];

	// Store event
	QGInputCommand* ic = new QGInputCommand(cmd, inputCommand.command, inputCommand.state);

	// Compute client tick using RTT (in milliseconds)
	int client_index = server->m_playerClientIndexes[packet->env.client];
	int avgRTT = networkServer->AverageRTT(client_index);

	// Convert to number of ticks
	int oneWayRTT = std::ceil(((float)avgRTT / 1000.0f) / (1.0f / QG_TICKS_PER_SECOND) / 2.0f);
	uint64_t tick = timeSystem->Tick() - oneWayRTT - QG_REPLICATION_CLIENT_LAG;

	// Store
	auto it = server->m_eventHistory.find(tick);
	if (it == server->m_eventHistory.end()) {
		server->m_eventHistory[tick] = std::vector<QGReplicationEvent*>();
	}

	QGReplicationEvent* repl = new QGReplicationEvent();
	repl->event = ic;
	repl->obj = entity;

	server->m_eventHistory[tick].push_back(repl);

	// Check the oldest replay tick and go back as necessary
	server->m_replayTicks[tick] = true;
	printf("Setting replay tick to %llu.\n", tick);
}

void QGReplicationServer::HandleClientConnectEvent(QGEvent* ev, QGObject* obj) {
	QGReplicationServer* server = GetQGSystem<QGReplicationServer>();
	QGNetworkClientConnectEvent* event = (QGNetworkClientConnectEvent*)ev;
	QGWorld* world = QGWorld::GetInstance();

	// Find client entity or instantiate
	QGEntity* entity = 0;
	auto it = server->m_playerEntities.find(event->clientID);
	if (it != server->m_playerEntities.end()) {
		entity = server->m_playerEntities[event->clientID];
	}
	else {
		if (server->m_playerCreateFunc != 0) {
			entity = server->m_playerCreateFunc();
			world->AddEntity(entity);
		}
		else {
			entity = world->CreateEntity("", event->clientID);
		}
		server->m_playerEntities[event->clientID] = entity;
	}

	server->m_playerClientIndexes[event->clientID] = event->clientIndex;
	printf("Created player entity and starting replication for client ID %llu.\n", event->clientID);

	// Send initial time sync packet
	server->SendStartupPacket(event->clientIndex);

	// Emit event with newly connected player entity
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	eventSystem->Publish(new QGPlayerConnectedEvent(event->clientID, entity));
}

void QGReplicationServer::HandleClientDisconnectEvent(QGEvent* ev, QGObject* obj) {
	QGReplicationServer* server = GetQGSystem<QGReplicationServer>();

	QGNetworkClientDisconnectEvent* event = (QGNetworkClientDisconnectEvent*)ev;
	QGWorld* world = QGWorld::GetInstance();

	// Find client entity or instantiate
	auto it = server->m_playerEntities.find(event->clientID);
	QGASSERT(it != server->m_playerEntities.end(), "Unable to find player entity to disconnect.");

	// Find entity in the world
	QGEntity* entity = world->FindEntity(event->clientID);

	// Remove it from both places
	server->m_playerEntities.erase(it);
	world->RemoveEntity(entity);

	// Delete
	delete entity;

	printf("Removed player entity and ending replication for client ID %llu.\n", event->clientID);
}

void QGReplicationServer::SendStartupPacket(int client_index) {
	QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();

	// Send a startup time packet
	timespec t = timeSystem->StartupTime();

	QGNetworkStartupPacket spacket;
	spacket.sec = t.tv_sec;
	spacket.nsec = t.tv_nsec;

	unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkStartupPacket));

	int offset = 0;
	memcpy(bytes, &spacket.sec, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	memcpy(bytes + offset, &spacket.nsec, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	server->Send(client_index, QGPACKET_STARTUP, bytes, offset, false);
}