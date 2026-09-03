
#pragma once

#include <quest-game.h>
#include <Scripting/QGScriptComponent.h>
#include <Core/QGEvent.h>
#include <Core/QGCameraComponent.h>
#include <Network/QGRpcServer.h>

class QUEST_GAME QuestPlayer : public QGScriptComponent {
public:
	QuestPlayer() {
		moveSpeed = turnSpeed = 0.0f;
		moveModifier = 1.0f;
		turnModifier = 30.0f;
		m_camera = 0;
		cameraDistance = 5.0f;

		level = 0;
		currentHealth = maxHealth = 0;

		m_initialized = false;
	}
	~QuestPlayer() = default;

	/**
	* Overridable functions
	*/
	void Initialize();
	void Update(float delta);

	/**
	 * Get available quests
	 */
	void GetAvailableQuests();

	/**
	 * Callbacks
	 */
	static void InputCommandCallback(QGEvent* ev, QGObject* obj);

	static void ClientAvailableQuestsCB(std::string func, nlohmann::json response);
	static std::string ServerAvailableQuestsCB(std::string func, QGDataRecord* args, QGEntity* entity);

	/**
	 * Serialization
	 */
	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

public:
	float moveSpeed;
	float moveModifier;
	float turnSpeed;
	float turnModifier;
	float cameraDistance;

	int level;
	int currentHealth;
	int maxHealth;

protected:
	QGCameraComponent* m_camera;
	std::vector<int> m_availableQuests;
	std::map<int, bool> m_completedQuests;
	bool m_initialized;
};