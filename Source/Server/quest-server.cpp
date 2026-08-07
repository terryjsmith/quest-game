
#include <Core/QGWorld.h>
#include <IO/QGGameWindow.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGKeyboard.h>
#include <Network/QGNetworkServer.h>
#include <Core/QGTimeSystem.h>
#include <Network/QGReplicationServer.h>
#include <Scripting/QGScriptingSystem.h>
#include <Core/QGMetaSystem.h>
#include <Scripting/QGScriptComponent.h>
#include <Network/QGNetworkEvents.h>
#include <Render/QGMeshComponent.h>
#include <Render/QGRenderSystem.h>
#include <IO/QGResourceSystem.h>
#include <Render/QGTextureLoader.h>
#include <Render/QGMeshLoader.h>
#include <Core/QGCameraComponent.h>
#include <Render/QGShaderLoader.h>

// Callback for newly connected players
void initialize_player_prefab(QGEvent* ev, QGObject* obj) {
    QGPlayerConnectedEvent* event = (QGPlayerConnectedEvent*)ev;

    // Add the player script component
    QGScriptingSystem* scriptingSystem = GetQGSystem<QGScriptingSystem>();
    QGScriptComponent* script = event->entity->CreateComponent<QGScriptComponent>();
    script->Initialize(scriptingSystem->GetScript("QuestPlayer"));

    QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
    QGMeshComponent* mesh = event->entity->CreateComponent<QGMeshComponent>();
    mesh->mesh = (QGMesh*)resourceSystem->Load("Resources/Meshes/box.fbx", "Mesh");

    printf("Added mesh and script component to new entity for client ID %llu.\n", event->clientID);
}

int main()
{
    // Create application
    QGApplication* application = QGApplication::GetInstance();

    // Create the game world
    QGWorld* world = QGWorld::GetInstance();

    // Create systems
    QGEventSystem* eventSystem = application->CreateSystem<QGEventSystem>();
    QGInputSystem* inputSystem = application->CreateSystem<QGInputSystem>();
    QGNetworkServer* networkSystem = application->CreateSystem<QGNetworkServer>(60);
    QGTimeSystem* timeSystem = application->CreateSystem<QGTimeSystem>();
    QGReplicationServer* replServer = application->CreateSystem<QGReplicationServer>(60);
    QGScriptingSystem* scriptingSystem = application->CreateSystem<QGScriptingSystem>(60);
    QGMetaSystem* metaSystem = application->CreateSystem<QGMetaSystem>();
    QGRenderSystem* renderSystem = application->CreateSystem<QGRenderSystem>();
    QGResourceSystem* resourceSystem = application->CreateSystem<QGResourceSystem>();

    // Initialize systems
    application->Initialize();

    // Create command mappings
    inputSystem->RegisterCommand(10, "MOVE");
    inputSystem->RegisterCommand(20, "TURN");

    // Register components types
    metaSystem->RegisterType<QGEntity>(1000, "QGEntity");
    metaSystem->RegisterType<QGScriptComponent>(1010, "QGScriptComponent", false);
    metaSystem->RegisterType<QGMeshComponent>(1020, "QGMeshComponent");
    metaSystem->RegisterType<QGCameraComponent>(1030, "QGCameraComponent", false);

    resourceSystem->RegisterResourceLoader<QGTextureLoader>("Texture2D");
    resourceSystem->RegisterResourceLoader<QGMeshLoader>("Mesh");
    resourceSystem->RegisterResourceLoader<QGShaderLoader>("Shader", false);

    // Initialize server
    const char* address = "127.0.0.1:35325";
    networkSystem->Listen(address);

    // Load game library
    scriptingSystem->LoadScriptLibrary("QuestServer.dll");

    // Listen for newly connected players
    eventSystem->Subscribe<QGPlayerConnectedEvent>(initialize_player_prefab, 0);

    // Create a static entity
    QGEntity* floor = world->CreateEntity("floor", 10);
    QGMeshComponent* fmeshc = floor->CreateComponent<QGMeshComponent>();
    fmeshc->mesh = (QGMesh*)resourceSystem->Load("Resources/Meshes/floor.fbx", "Mesh");
    floor->transform.position = vector3(0.0f, -1.0f, 0.0f);

    // Get start time of loop
    timespec lastTimestamp;
    timespec currentTimestamp;
    timeSystem->Timestamp(&lastTimestamp);

    // Enter message loop
    while (true) {
        // Do amazing stuff, update all applications
        timeSystem->Timestamp(&currentTimestamp);
        timespec diff = QGTimeSystem::Diff(&lastTimestamp, &currentTimestamp);
        double d = (double)diff.tv_sec + ((double)diff.tv_nsec / 1000000000);

        application->Update(d);
        lastTimestamp = currentTimestamp;

        // Rest
        Sleep(0);
    }
}

