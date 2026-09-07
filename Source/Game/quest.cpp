
#include "quest-game.h"
#include "QuestPlayer.h"
#include "QuestManager.h"

#include <Core/QGObject.h>
#include <Core/QGMetaSystem.h>
#include <Network/QGNetworkServer.h>
#include <Core/QGEventSystem.h>
#include <Network/QGNetworkEvents.h>
#include <Core/QGApplication.h>
#include <IO/QGResourceSystem.h>
#include <Render/QGMeshComponent.h>
#include <Physics/QGCollisionComponent.h>
#include <Physics/QGSphereCollider.h>
#include <IO/QGMySQLDataLoader.h>

// Callback for newly connected players
void initialize_player_prefab(QGEvent* ev, QGObject* obj) {
    QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
    if (server) {
        QGPlayerConnectedEvent* event = (QGPlayerConnectedEvent*)ev;
        QGEntity* entity = event->entity;

        // Add the player script component
        QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
        QGScriptComponent* script = (QGScriptComponent*)metaSystem->CreateObject("QuestPlayer");
        script->Initialize();
        event->entity->AddComponent(script);

        // Load player info from DB
        QGMySQLDataLoader* mysql = GetQGSystem<QGMySQLDataLoader>();
        QGNetworkServer* networkServer = GetQGSystem<QGNetworkServer>();

        std::string connectToken = networkServer->ConnectTokenFromID(event->clientID);
        printf("Loading data from connect token: %s\n", connectToken.c_str());

        std::map<std::string, std::string> params;
        params["id"] = connectToken;

        printf("Querying sessions... ");
        std::vector<QGDataRecord*> records = mysql->Load("sessions", params);
        printf("got back %d records.\n", records.size());

        std::string user_id = records[0]->Get("user_id").AsString();
        printf("Found user ID %s.\n", user_id.c_str());

        printf("Querying characters... ");
        params.clear();
        params["user_id"] = user_id;
        records = mysql->Load("characters", params);
        printf("got back %d records.\n", records.size());

        std::string model = records[0]->Get("type").AsString();
        printf("Found model named: %s.\n", model.c_str());

        QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
        QGMeshComponent* mesh = event->entity->CreateComponent<QGMeshComponent>();
        mesh->mesh = (QGMesh*)resourceSystem->Load("Resources/Meshes/" + model + ".fbx", "Mesh");

        // Set name, position, rotation
        entity->name = records[0]->Get("name").AsString();

        vector3 position;
        sscanf(records[0]->Get("position").AsString().c_str(), "(%f, %f, %f)", &position.x, &position.y, &position.z);
        entity->transform.position = position;

        quaternion rotation;
        sscanf(records[0]->Get("rotation").AsString().c_str(), "(%f, %f, %f, %f)", &rotation.w, &rotation.x, &rotation.y, &rotation.z);
        entity->transform.rotation = rotation;

        // Create collider
        QGCollisionComponent* colliderComponent = event->entity->CreateComponent<QGCollisionComponent>();
        QGSphereCollider* collisionShape = new QGSphereCollider();
        collisionShape->Initialize(1.0f);
        colliderComponent->Shape(collisionShape);

        printf("Added components to new entity for client ID %llu.\n", event->clientID);
    }
    else {
        QGPlayerConnectedEvent* event = (QGPlayerConnectedEvent*)ev;
        QGEntity* entity = event->entity;

        // Add a camera component
        QGCameraComponent* camera = entity->CreateComponent<QGCameraComponent>();
    }
}

extern "C" void QUEST_GAME qg_init_library() {
    // Register types
    QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
    metaSystem->RegisterType<QuestPlayer>(5010, "QuestPlayer");
    metaSystem->RegisterType<QuestGiver>(5020, "QuestGiver");

    // Register systems
    QGApplication* app = QGApplication::GetInstance();
    QuestManager* questManager = app->CreateSystem<QuestManager>(60);

    // Listen for newly connected players
    QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
    eventSystem->Subscribe<QGPlayerConnectedEvent>(initialize_player_prefab, 0);

    // For servers only
    QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
    if (server) {
        QGRpcServer* rpc = GetQGSystem<QGRpcServer>();
        rpc->Bind("get_available_quests", QuestPlayer::ServerAvailableQuestsCB);
    }
}