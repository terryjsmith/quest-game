
#include <Core/QGWorld.h>
#include <IO/QGGameWindow.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGKeyboard.h>
#include <Network/QGNetworkClient.h>
#include <Core/QGTimeSystem.h>
#include <Network/QGReplicationClient.h>
#include <Core/QGMetaSystem.h>
#include <Render/QGMeshComponent.h>
#include <Render/GL/QGGLRenderSystem.h>
#include <IO/QGResourceSystem.h>
#include <Render/QGTextureLoader.h>
#include <Render/QGMeshLoader.h>
#include <Network/QGNetworkEvents.h>
#include <Core/QGCameraComponent.h>
#include <Render/QGShaderLoader.h>
#include <Render/QGRenderDefines.h>
#include <Render/Passes/QGForwardRenderPass.h>
#include <Physics/QGCollisionSystem.h>
#include <Scripting/QGScriptComponent.h>
#include <Scripting/QGScriptingSystem.h>
#include <Network/QGRpcClient.h>

void PlayerConnectedCallback(QGEvent* ev, QGObject* obj) {
    QGPlayerConnectedEvent* event = (QGPlayerConnectedEvent*)ev;
    QGEntity* entity = event->entity;

    // Add a camera component
    QGCameraComponent* camera = entity->CreateComponent<QGCameraComponent>();
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
    QGNetworkClient* networkSystem = application->CreateSystem<QGNetworkClient>(60);
    QGTimeSystem* timeSystem = application->CreateSystem<QGTimeSystem>();
    QGReplicationClient* replClient = application->CreateSystem<QGReplicationClient>(60);
    QGMetaSystem* metaSystem = application->CreateSystem<QGMetaSystem>();
    QGGLRenderSystem* renderSystem = application->CreateSystem<QGGLRenderSystem>();
    QGResourceSystem* resourceSystem = application->CreateSystem<QGResourceSystem>();
    QGCollisionSystem* collisionSystem = application->CreateSystem<QGCollisionSystem>(60);
    QGScriptingSystem* scriptingSystem = application->CreateSystem<QGScriptingSystem>(60);
    QGRpcClient* rpcClient = application->CreateSystem<QGRpcClient>();

    // Initialize systems
    application->Initialize();

    // Create main game window
    QGGameWindow* window = QGGameWindow::GetInstance();
    window->Create("Game Window", 800, 600, false);

    // Setup rendering pipeline
    QGForwardRenderPass* forwardRenderPass = new QGForwardRenderPass();
    renderSystem->AddRenderPass(0, forwardRenderPass);

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

    // Load game library
    // scriptingSystem->LoadScriptLibrary("QuestClient.dll");
    HINSTANCE hDll = LoadLibrary(TEXT("quest-gamed.dll"));

    // Initialize OpenGL
    renderSystem->Initialize(window->Width(), window->Height());
    renderSystem->InitializeGL();

    // Create a keyboard
    QGKeyboard* keyboard = inputSystem->CreateInputDevice<QGKeyboard>();

    // Register key mappings
    inputSystem->AssociateCommandInput("MOVE", keyboard, QGKeys::KEY_UP,    1.0f);
    inputSystem->AssociateCommandInput("MOVE", keyboard, QGKeys::KEY_DOWN, -1.0f);
    inputSystem->AssociateCommandInput("TURN", keyboard, QGKeys::KEY_RIGHT, -1.0f);
    inputSystem->AssociateCommandInput("TURN", keyboard, QGKeys::KEY_LEFT,  1.0f);
    inputSystem->AssociateCommandInput("INTERACT", keyboard, QGKeys::KEY_X);

    // Subscribe to a player connection event
    eventSystem->Subscribe<QGPlayerConnectedEvent>(PlayerConnectedCallback, 0);

    // Initialize server
    const char* address = "127.0.0.1:35325";
    networkSystem->Connect(address);

    // Set player ID
    replClient->PlayerID(networkSystem->ClientID());

    // Get start time of loop
    timespec lastTimestamp;
    timespec currentTimestamp;
    timeSystem->Timestamp(&lastTimestamp);

    // Enter message loop
    while (window->ShouldClose() == false) {
        // Process messages
        window->ProcessMessages();

        // Do amazing stuff, update all applications
        timeSystem->Timestamp(&currentTimestamp);
        timespec diff = QGTimeSystem::Diff(&lastTimestamp, &currentTimestamp);
        double d = (double)diff.tv_sec + ((double)diff.tv_nsec / 1000000000);

        application->Update(d);
        lastTimestamp = currentTimestamp;

        // Draw all of the amazing stuff
        renderSystem->Render();

        // Swap buffers
        window->SwapBuffers();

        // Rest
        Sleep(0);
    }
}
