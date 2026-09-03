
#include "QuestManager.h"

#include <Core/QGWorld.h>

void QuestManager::Update(float delta) {
	QGWorld* world = QGWorld::GetInstance();
	m_questGivers = world->GetComponents<QuestGiver>();
}

std::vector<QuestGiver*> QuestManager::RadiusSearch(QGEntity* entity, float radius) {
	std::vector<QuestGiver*> retval;
	vector3 playerPosition = entity->transform.position;

	for (auto it = m_questGivers.begin(); it != m_questGivers.end(); it++) {
		QGEntity* giverEntity = (*it)->Entity();
		if (giverEntity == 0) continue;

		vector3 giverPosition = giverEntity->transform.position;
		vector3 difference = playerPosition - giverPosition;
		float distance = difference.length();

		if (distance < radius) {
			retval.push_back(*it);
		}
	}

	return(retval);
}