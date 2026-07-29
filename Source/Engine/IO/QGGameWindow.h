
#ifndef qggamewindow_h
#define qggamewindow_h

#include <Core/QGObject.h>
#include <GLFW/glfw3.h>

class QUEST_API QGGameWindow : public QGObject {
public:
	QGGameWindow() {
		m_width = m_height = 0;
		m_fullscreen = false;
		m_handle = 0;
	}
	~QGGameWindow() = default;

	// Create a new window
	void Create(std::string name, int width, int height, bool fullscreen);

	// Check if the window has a closing message
	bool ShouldClose();

	// Process any messages
	void ProcessMessages();

	// Swap buffers
	void SwapBuffers();

	// Getters
	GLFWwindow* Handle() { return m_handle; }
	std::string Name() { return m_name; }
	int Width() { return m_width; }
	int Height() { return m_height; }
	bool Fullscreen() { return m_fullscreen; }

	// Singleton
	static QGGameWindow* GetInstance();

public:
	std::string m_name;
	int m_width, m_height;
	bool m_fullscreen;

	GLFWwindow* m_handle;

	static QGGameWindow* m_instance;
};

#endif