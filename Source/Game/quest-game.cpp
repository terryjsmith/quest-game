
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
#include <Scripting/QGScriptComponent.h>
#include <Scripting/QGScriptingSystem.h>

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
    QGScriptingSystem* scriptingSystem = application->CreateSystem<QGScriptingSystem>();

    // Initialize systems
    application->Initialize();

    // Create main game window
    QGGameWindow* window = QGGameWindow::GetInstance();
    window->Create("Game Window", 800, 600, false);

    // Create command mappings
    inputSystem->RegisterCommand(10, "MOVE");
    inputSystem->RegisterCommand(20, "TURN");

    // Register components types
    metaSystem->RegisterType<QGEntity>(1000, "QGEntity");
    metaSystem->RegisterType<QGScriptComponent>(1010, "QGScriptComponent");

    // Load game library
    scriptingSystem->LoadScriptLibrary("QuestClient.dll");

    // Create a keyboard
    QGKeyboard* keyboard = inputSystem->CreateInputDevice<QGKeyboard>();

    // Register key mappings
    inputSystem->AssociateCommandInput("MOVE", keyboard, QGKeys::KEY_UP,    1.0f);
    inputSystem->AssociateCommandInput("MOVE", keyboard, QGKeys::KEY_DOWN, -1.0f);
    inputSystem->AssociateCommandInput("TURN", keyboard, QGKeys::KEY_RIGHT, 1.0f);
    inputSystem->AssociateCommandInput("TURN", keyboard, QGKeys::KEY_LEFT, -1.0f);

    // Initialize server
    const char* address = "127.0.0.1:35325";
    networkSystem->Connect(address);

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

        // Swap buffers
        window->SwapBuffers();
    }
}
