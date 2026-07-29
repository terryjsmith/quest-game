
#include <IO/QGInputSystem.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputEvent.h>
#include <IO/QGInputCommand.h>
#include <Core/QGApplication.h>

void QGInputSystem::Initialize() {
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	eventSystem->Subscribe<QGInputEvent>(&InputEventCallbackHandler, 0);
}

void QGInputSystem::InputEventCallbackHandler(QGEvent* event, QGObject* device) {
	// Convert to input event
	QGInputEvent* ev = dynamic_cast<QGInputEvent*>(event);

	// Check for mappings
	QGInputSystem* inputSystem = GetQGSystem<QGInputSystem>();
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	auto it = inputSystem->m_commands.begin();
	for (; it != inputSystem->m_commands.end(); it++) {
		if ((*it)->device == ev->device && (*it)->button == ev->button) {
			eventSystem->Publish(new QGInputCommand((*it)->command, inputSystem->m_commandIDs[(*it)->command], ev->state * (*it)->multiplier));

			// Save state
			inputSystem->m_commandStates[(*it)->command] = ev->state * (*it)->multiplier;
			return;
		}
	}
}
