
#ifdef USE_OPENGL

#include <IO/QGMouse.h>
#include <IO/QGGameWindow.h>
#include <IO/QGInputSystem.h>
#include <Core/QGApplication.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputEvent.h>
#include <GLFW/glfw3.h>

void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
	// Get our input system
	QGInputSystem* inputSystem = GetQGSystem<QGInputSystem>();

	// Find our nouse device
	QGMouse* mouse = dynamic_cast<QGMouse*>(inputSystem->FindInputDevice<QGMouse>());

	// Dispatch event
	float state = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1.0f : 0.0f;
	mouse->SetButtonState(button, state);
}

void QGMouse::Initialize() {
	GLFWwindow* window = QGGameWindow::GetInstance()->Handle();
	if (glfwSetMouseButtonCallback(window, MouseCallback) == NULL) {
		QGASSERT(false, "Error initializing mouse callback.");
		return;
	}
}

float QGMouse::GetButtonState(int button) {
	return(m_buttonStates[button]);
}

void QGMouse::SetButtonState(int button, float state) {
	// Set state (if different)
	if (m_buttonStates[button] != state) {
		m_buttonStates[button] = state;

		// Dispatch event
		GetQGSystem<QGEventSystem>()->Publish(new QGInputEvent(this, button, state));
	}
}

vector2 QGMouse::GetCursorPosition() {
	GLFWwindow* window = QGGameWindow::GetInstance()->Handle();

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return(vector2(xpos, ypos));
}

#endif