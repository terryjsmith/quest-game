
#include <Network/QGReplicationClient.h>
#include <IO/QGInputCommand.h>
#include <Core/QGEventSystem.h>
#include <Core/QGApplication.h>
#include <Network/QGNetworkPackets.h>
#include <Network/QGNetworkClient.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGWorld.h>
#include <Core/QGMetaSystem.h>
#include <Network/QGNetworkEvents.h>

void QGReplicationClient::Initialize() {
	// Subscribe to input commands
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	eventSystem->Subscribe<QGInputCommand>(&InputCommandReceived);

	// Receive startup packet
	QGNetworkSystem* networkSystem = GetQGSystem<QGNetworkSystem>();
	networkSystem->RegisterPacketCallback(QGPACKET_STARTUP, &HandleNetworkStartupPacket);
	networkSystem->RegisterPacketCallback(QGPACKET_ENTITYSNAPSHOT, &HandleEntitySnapshotPacket);
}

void QGReplicationClient::InputCommandReceived(QGEvent* ev, QGObject* obj) {
	QGNetworkClient* client = GetQGSystem<QGNetworkClient>();

	// Get the command object
	QGInputCommand* command = dynamic_cast<QGInputCommand*>(ev);

	// Relay to a connected server
	QGNetworkCommandPacket packet;
	packet.command = command->type;
	packet.state = command->state;

	unsigned char* bytes = (unsigned char*)malloc(sizeof(QGNetworkCommandPacket));

	int offset = 0;
	memcpy(bytes, &packet.command, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(bytes + sizeof(uint32_t), &packet.state, sizeof(float));
	offset += sizeof(float);

	QGNetworkClient* networkSystem = GetQGSystem<QGNetworkClient>();
	networkSystem->Send(QGPACKET_INPUTCOMMAND, bytes, offset, true);

	uint64_t client_id = client->ClientID();
	printf("Sent an input command of type %d set to value %f from client ID %llu.\n", packet.command, packet.state, client_id);

	free(bytes);
}

void QGReplicationClient::Update(float delta) {
	// Get current tick
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	uint64_t currentTick = timeSystem->Tick();

	QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
	QGWorld* world = QGWorld::GetInstance();

	// If we have no snapshots yet, return
	if (m_snapshots.size() == 0) return;

	// Get "current" time (need fractional resolution)
	timespec currentTime;
	timeSystem->Timestamp(&currentTime);
	timespec startupTime = timeSystem->StartupTime();

	timespec diff = timeSystem->Diff(&startupTime, &currentTime);
	double d = (double)diff.tv_sec + ((double)diff.tv_nsec / 1000000000);
	int totalTickTime = std::floor((double)d * QG_TICKS_PER_SECOND);
	double interpolate = d - totalTickTime;

	uint64_t replayTick = currentTick - QG_REPLICATION_CLIENT_LAG;

	// Find the most appropriate snapshot
	auto it = m_snapshots.find(replayTick);
	auto first = m_snapshots.begin();
	if (it == m_snapshots.end()) {
		while (true) {
			if (replayTick == 0) break;
			if (replayTick < first->first) break;

			it = m_snapshots.find(replayTick);
			if (it != m_snapshots.end()) break;

			replayTick--;
		}
	}

	// Apply snapshot to world
	if (it != m_snapshots.end()) {
		QGEntitySnapshot* snapshot = &it->second;

		// List of entites in snapshot
		std::map<uint64_t, QGEntity*> worldEntities = world->GetEntities();

		// Iterate over entities
		for (auto eit = snapshot->entities.begin(); eit != snapshot->entities.end(); eit++) {
			// Find entity
			QGEntity* entity = world->FindEntity(eit->first);
			if (entity == 0) {
				// Create new entity
				entity = new QGEntity();
				entity->Deserialize(&eit->second.record);
				world->AddEntity(entity);
			}
			else {
				entity->Deserialize(&eit->second.record);
			}

			// Remove from copy of entity list
			worldEntities.erase(eit->first);

			// Get current components and categorize
			std::vector<QGComponent*> components = entity->GetComponents();
			std::map<uint32_t, QGComponent*> componentTypes;
			for (auto tit = components.begin(); tit != components.end(); tit++) {
				QGObjectType* type = (*tit)->Type();
				componentTypes[type->typeID] = *tit;
			}

			// Update components
			for (auto cit = eit->second.components.begin(); cit != eit->second.components.end(); cit++) {
				auto tit = componentTypes.find(cit->first);
				if (tit != componentTypes.end()) {
					tit->second->Deserialize(&cit->second.record);
				}
				else {
					QGComponent* component = (QGComponent*)metaSystem->CreateObject(cit->second.type);
					component->Deserialize(&cit->second.record);
					entity->AddComponent(component);
				}
			}
		}

		// Check if we got a player entity, and if it is new
		QGEntity* playerEntity = world->FindEntity(m_playerID);
		if (playerEntity != 0 && m_playerEntity == 0) {
			// Save
			m_playerEntity = playerEntity;
			
			// Emit event
			QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
			eventSystem->Publish(new QGPlayerConnectedEvent(m_playerID, m_playerEntity));
		}

		// Remove any entities that shouldn't be here anymore
		for (auto it = worldEntities.begin(); it != worldEntities.end(); it++) {
			world->RemoveEntity(it->first);
		}
	}

	// Remove old snapshots
	uint64_t removalTick = currentTick - (QG_REPLICATION_CLIENT_SNAPSHOT_HISTORY * QG_TICKS_PER_SECOND);
	auto rit = m_snapshots.find(removalTick);
	if(rit != m_snapshots.end()) m_snapshots.erase(m_snapshots.begin(), rit);
}

void QGReplicationClient::HandleNetworkStartupPacket(QGNetworkPacket* packet) {
	// Unpack our packet
	timespec ts;

	int offset = 0;
	memcpy(&ts.tv_sec, packet->bytes + offset, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	memcpy(&ts.tv_nsec, packet->bytes + offset, sizeof(uint32_t));
	offset += sizeof(offset);

	// Set startup time to match server
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	timeSystem->StartupTime(ts);

	printf("Replicated client time to match server time.\n");
}

void QGReplicationClient::HandleEntitySnapshotPacket(QGNetworkPacket* packet) {
	QGReplicationClient* client = GetQGSystem<QGReplicationClient>();

	// Unpack our packet
	QGEntitySnapshot snapshot;
	snapshot.Deserialize(packet->bytes, packet->env.size);

	// Add to our stream
	client->m_snapshots[packet->env.tick] = snapshot;
}