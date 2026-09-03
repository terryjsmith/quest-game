
#include <Core/QGApplication.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputCommand.h>
#include <Core/QGEntity.h>
#include <Physics/QGCollisionSystem.h>
#include <Network/QGRpcClient.h>
#include <Network/QGReplicationClient.h>
#include <Core/QGWorld.h>
#include <Network/QGNetworkEvents.h>

#include "QuestPlayer.h"
#include "QuestGiver.h"
#include "QuestManager.h"

void QuestPlayer::Initialize() {
    QGEntity* entity = this->Entity();

    QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
    eventSystem->Subscribe<QGInputCommand>(InputCommandCallback, entity);

    // Also request available quests
    this->GetAvailableQuests();
}

void QuestPlayer::Serialize(QGDataRecord* record) {
    record->Set("health", this->currentHealth);
    record->Set("max_health", this->maxHealth);
    record->Set("level", this->level);
}

void QuestPlayer::Deserialize(QGDataRecord* record) {
    this->currentHealth = record->Get("health").AsInt();
    this->maxHealth = record->Get("max_health").AsInt();
    this->level = record->Get("level").AsInt();

    if (m_initialized == false) {
        this->Initialize();
        m_initialized = true;
    }
}

void QuestPlayer::Update(float delta) {
    // Update position
    QGEntity* entity = this->Entity();
    entity->transform.Move(delta * moveSpeed * moveModifier * entity->transform.Forward());
    entity->transform.Rotate(entity->transform.Up(), delta * turnSpeed * turnModifier);

    // Check for attached camera
    QGCameraComponent* camera = entity->GetComponent<QGCameraComponent>();
    if (m_camera == 0 && camera != 0) {
        m_camera = camera;
        camera->transform.Rotate(entity->transform.Right(), -30.0f);
    }

    if (m_camera) {
        // Update camera position
        vector3 playerPosition = entity->transform.position;
        vector3 adjustment = vector3(0, (cameraDistance / 2.0f), 0);
        m_camera->transform.position = playerPosition - (entity->transform.Forward() * cameraDistance) + adjustment;

        // Set look
        m_camera->SetTargetPosition(playerPosition);
    }
}

void QuestPlayer::InputCommandCallback(QGEvent* ev, QGObject* obj) {
    QGInputCommand* command = (QGInputCommand*)ev;
    QGEntity* entity = (QGEntity*)obj;

    // If run on client, get player entity
    if (obj == 0) {
        QGReplicationClient* replClient = GetQGSystem<QGReplicationClient>();
        QGWorld* world = QGWorld::GetInstance();

        // Get player
        entity = world->FindEntity(replClient->PlayerID());
    }

    QuestPlayer* player = entity->GetComponent<QuestPlayer>();

    if (command->command == "MOVE")
    {
        printf("Setting move speed for player ID %llu to %f.\n", entity->id, command->state);
        player->moveSpeed = command->state;
    }

    if (command->command == "TURN")
    {
        printf("Setting turn speed for player ID %llu to %f.\n", entity->id, command->state);
        player->turnSpeed = command->state;
    }

    if (command->command == "INTERACT" && command->state == 1.0)
    {
        // Check for intersections
        QGCollisionSystem* collisionSystem = GetQGSystem<QGCollisionSystem>();
        std::vector<QGEntity*> entities = collisionSystem->Intersects(entity);
        if (entities.size() > 0)
        {
            std::vector<QGEntity*> possibleEntities;
            for (auto it = entities.begin(); it != entities.end(); it++) {
                // Intersection, check if this is a quest giver
                QuestGiver* qg = entity->GetComponent<QuestGiver>();
                if (qg != 0)
                {
                    possibleEntities.push_back(entity);
                    printf("I'm interacting with an entity like a real game!\n");
                    break;
                }
            }
        }
    }
}

void QuestPlayer::GetAvailableQuests() {
    QGRpcClient* rpcClient = GetQGSystem<QGRpcClient>();
    if (rpcClient == 0) return;

    QGDataRecord args;
    args.Set("radius", 100.0f);
    rpcClient->Call("get_available_quests", &args, ClientAvailableQuestsCB);
}

void QuestPlayer::ClientAvailableQuestsCB(std::string func, nlohmann::json response) {
    QGReplicationClient* replClient = GetQGSystem<QGReplicationClient>();
    QGWorld* world = QGWorld::GetInstance();

    // Get player
    QGEntity* entity = world->FindEntity(replClient->PlayerID());
    QuestPlayer* player = entity->GetComponent<QuestPlayer>();

    // Read response
    player->m_availableQuests.clear();
    for (auto& el : response) {
        player->m_availableQuests.push_back(el.get<int>());
    }
}

std::string QuestPlayer::ServerAvailableQuestsCB(std::string func, QGDataRecord* args, QGEntity* entity) {
    std::vector<int> retval;

    // Get a list of all quest givers in a certain radius (100m for now)
    QuestManager* questManager = GetQGSystem<QuestManager>();
    std::vector<QuestGiver*> questGivers = questManager->RadiusSearch(entity, args->Get("radius").AsFloat());

    // Get list of completed quests
    QuestPlayer* player = entity->GetComponent<QuestPlayer>();

    // Loop through and check available quests
    for (auto it = questGivers.begin(); it != questGivers.end(); it++) {
        for (auto qi = (*it)->quests.begin(); qi != (*it)->quests.end(); qi++) {
            Quest* quest = (*qi);

            // Have they already completed the quest?
            if (player->m_completedQuests.find(quest->id) != player->m_completedQuests.end()) continue;

            // Do they meet the level requirement?
            if (player->level < quest->minLevel) continue;

            // Have they completed the pre-requisites?
            bool prerequisites = true;
            for (auto pi = quest->prerequisites.begin(); pi != quest->prerequisites.end(); pi++) {
                if (player->m_completedQuests.find(*pi) == player->m_completedQuests.end()) {
                    prerequisites = false;
                    break;
                }
            }

            if (prerequisites == false) continue;

            // Other requirements are met, add
            retval.push_back(quest->id);
        }
    }

    nlohmann::json retobj = retval;
    return(retobj.dump());
}