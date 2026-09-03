#pragma once

#include "quest-game.h"
#include "QuestGiver.h"
#include <Core/QGSystem.h>
#include <Core/QGEntity.h>

class QUEST_GAME QuestManager : public QGSystem {
public:
	QuestManager() = default;
	~QuestManager() = default;

	/**
	 * Update list of (all) quest givers
	 */
	void Update(float delta);

	/**
	 * Get a list of all quest givers within a radius to a player (entity)
	 */
	std::vector<QuestGiver*> RadiusSearch(QGEntity* entity, float radius);

protected:
	// List of all quest givers
	std::vector<QuestGiver*> m_questGivers;
};