
#include <Scripting/QGScriptingSystem.h>
#include <Scripting/QGScriptComponent.h>
#include <Core/QGWorld.h>

void QGScriptingSystem::Initialize() {
	
}

void QGScriptingSystem::Update(float delta) {
	std::vector<QGScriptComponent*> components = QGWorld::GetInstance()->GetComponents<QGScriptComponent>();
	for (auto it = components.begin(); it != components.end(); it++) {
		(*it)->Update(delta);
	}
}
