
#include <IO/QGGameWindow.h>

QGGameWindow* QGGameWindow::m_instance = 0;

QGGameWindow* QGGameWindow::GetInstance() {
	if (m_instance == 0) m_instance = new QGGameWindow();
	return(m_instance);
}

void QGGameWindow::Create(std::string name, int width, int height, bool fullscreen) {
	m_name = name;
	m_width = width;
	m_height = height;
	m_fullscreen = fullscreen;

	// Initialize GLFW
	if (!glfwInit()) {
		QGASSERT(false, "Error initializing GLFW.");
		return;
	}

	// Attempt window creation
	m_handle = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (m_handle == NULL) {
		QGASSERT(false, "Error creating window.");
		return;
	}

	// Make the window's context current
	glfwMakeContextCurrent(m_handle);
}

bool QGGameWindow::ShouldClose() {
	return(glfwWindowShouldClose(m_handle));
}

void QGGameWindow::ProcessMessages() {
	glfwPollEvents();
}

void QGGameWindow::SwapBuffers() {
	glfwSwapBuffers(m_handle);
}