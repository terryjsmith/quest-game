
#include "quest-game.h"
#include "QuestPlayer.h"
#include "QuestManager.h"

#include <Core/QGObject.h>
#include <Core/QGMetaSystem.h>
#include <Network/QGNetworkServer.h>
#include <Core/QGApplication.h>

void qg_init_library() {
    // Register types
    QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
    metaSystem->RegisterType<QuestPlayer>(5010, "QuestPlayer");
    metaSystem->RegisterType<QuestGiver>(5020, "QuestGiver");

    // Register systems
    QGApplication* app = QGApplication::GetInstance();
    QuestManager* questManager = app->CreateSystem<QuestManager>(60);

    // For servers only
    QGNetworkServer* server = GetQGSystem<QGNetworkServer>();
    if (server) {
        QGRpcServer* rpc = GetQGSystem<QGRpcServer>();
        rpc->Bind("get_available_quests", QuestPlayer::ServerAvailableQuestsCB);
    }
}