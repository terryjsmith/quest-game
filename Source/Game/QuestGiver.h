
#pragma once

#include "quest-game.h"
#include "Quest.h"
#include <Scripting/QGScriptComponent.h>

class QUEST_GAME QuestGiver : public QGScriptComponent {
public:
	QuestGiver() = default;
	~QuestGiver() = default;

	void Serialize(QGDataRecord* record) {}
	void Deserialize(QGDataRecord* record) {}

public:
	std::vector<Quest*> quests;
};