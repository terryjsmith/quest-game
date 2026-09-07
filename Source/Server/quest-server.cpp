
#include <Core/QGWorld.h>
#include <IO/QGGameWindow.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGKeyboard.h>
#include <Network/QGNetworkServer.h>
#include <Core/QGTimeSystem.h>
#include <Network/QGReplicationServer.h>
#include <Core/QGMetaSystem.h>
#include <Network/QGNetworkEvents.h>
#include <Render/QGMeshComponent.h>
#include <Render/QGRenderSystem.h>
#include <IO/QGResourceSystem.h>
#include <Render/QGTextureLoader.h>
#include <Render/QGMeshLoader.h>
#include <Core/QGCameraComponent.h>
#include <Render/QGShaderLoader.h>
#include <Physics/QGCollisionSystem.h>
#include <Physics/QGSphereCollider.h>
#include <Scripting/QGScriptComponent.h>
#include <Scripting/QGScriptingSystem.h>
#include <Network/QGRpcServer.h>
#include <IO/QGMySQLDataLoader.h>

#ifndef _WIN32
    #define Sleep sleep
#endif

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
    QGCollisionSystem* collisionSystem = application->CreateSystem<QGCollisionSystem>(60);
    QGRpcServer* rpcServer = application->CreateSystem<QGRpcServer>();
    QGMySQLDataLoader* mysql = application->CreateSystem<QGMySQLDataLoader>();

    // Initialize systems
    application->Initialize();

    // Create command mappings
    inputSystem->RegisterCommand(10, "MOVE");
    inputSystem->RegisterCommand(20, "TURN");
    inputSystem->RegisterCommand(30, "INTERACT");

    // Register components types
    metaSystem->RegisterType<QGEntity>(1000, "QGEntity");
    //metaSystem->RegisterType<QGScriptComponent>(1010, "QGScriptComponent");
    metaSystem->RegisterType<QGMeshComponent>(1020, "QGMeshComponent");
    metaSystem->RegisterType<QGCameraComponent>(1030, "QGCameraComponent", false);
    metaSystem->RegisterType<QGCollisionComponent>(1040, "QGCollisionComponent");

    resourceSystem->RegisterResourceLoader<QGTextureLoader>("Texture2D");
    resourceSystem->RegisterResourceLoader<QGMeshLoader>("Mesh");
    resourceSystem->RegisterResourceLoader<QGShaderLoader>("Shader", false);

    // Initialize server
    const char* address = "192.81.208.200:35325";
    networkSystem->Listen(address);

    // Connect to database
    mysql->Open("localhost");

    // Load game library
    scriptingSystem->LoadScriptLibrary("quest-game");

    // Create a static entity
    QGEntity* floor = world->CreateEntity("floor", 10);
    QGMeshComponent* fmeshc = floor->CreateComponent<QGMeshComponent>();
    fmeshc->mesh = (QGMesh*)resourceSystem->Load("Resources/Meshes/floor.fbx", "Mesh");

    QGEntity* questGiver = world->CreateEntity("QuestGiver", 20);
    QGMeshComponent* qmeshc = questGiver->CreateComponent<QGMeshComponent>();
    qmeshc->mesh = (QGMesh*)resourceSystem->Load("Resources/Meshes/box.fbx", "Mesh");
    questGiver->transform.position = vector3(5, 0, 5);

    QGCollisionComponent* colliderc = questGiver->CreateComponent<QGCollisionComponent>();
    QGSphereCollider* collisionShape = new QGSphereCollider();
    collisionShape->Initialize(1.0f);
    colliderc->Shape(collisionShape);

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

