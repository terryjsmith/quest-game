
#ifndef qguisystem_h
#define qguisystem_h

#include <Render/UI/QGUIScreen.h>
#include <Core/QGSystem.h>
#include <Render/QGTexture2D.h>

class QUEST_API QGUISystem : public QGSystem {
public:
	QGUISystem() {
		m_activeScreen = 0;
		m_data = 0;
		m_width = m_height = 0;
	}
	~QGUISystem() = default;

	/**
	 * Get / set active screen
	 */
	QGUIScreen* ActiveScreen() { return m_activeScreen; }
	void ActiveScreen(QGUIScreen* screen) { m_activeScreen = screen; }

	/**
	 * Update all elements
	 */
	void Update(float delta);

	/**
	 * Draw onto current screen / framebuffer
	 */
	void Render(int width, int height);

	/**
	 * Get texture data
	 */
	unsigned char* Data() { return m_data; }

protected:
	void RecursiveRender(QGUIControl* node, vector2 topleft, vector2 bottomright, unsigned char* output);

protected:
	QGUIScreen* m_activeScreen;
	unsigned char* m_data;
	int m_width, m_height;
};

#endif