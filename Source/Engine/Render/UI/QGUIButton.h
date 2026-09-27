
#ifndef qguibutton_h
#define qguibutton_h

#include <Render/UI/QGUIControl.h>

class QUEST_API QGUIButton : public QGUIControl {
public:
	QGUIButton() {
		m_bgcolors[QGUI_STATE_DEFAULT] = vector4(1.0f);
		m_bgcolors[QGUI_STATE_HOVER] = vector4(0.0f);
		m_bgcolors[QGUI_STATE_CLICK] = vector4(1.0f, 0.0f, 0.0f, 1.0f);

		m_state = QGUI_STATE_DEFAULT;

		m_xstretch = m_ystretch = false;
	}
	~QGUIButton() = default;

	/**
	 * Properties
	 */
	vector4 BGColor() { return m_bgcolors[0]; }
	void BGColor(vector4 color) { m_bgcolors[0] = color; }

	vector4 HoverColor() { return m_bgcolors[1]; }
	void HoverColor(vector4 color) { m_bgcolors[1] = color; }

	vector4 ClickColor() { return m_bgcolors[2]; }
	void ClickColor(vector4 color) { m_bgcolors[2] = color; }

	/**
	 * Draw
	 */
	void Redraw();

protected:
	std::map<int, vector4> m_bgcolors;
	int m_state;
};

#endif