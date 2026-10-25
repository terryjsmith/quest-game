
#ifndef qguitextlabel_h
#define qguitextlabel_h

#include <Render/UI/QGUIFont.h>
#include <Render/UI/QGUIControl.h>

class QUEST_API QGUITextLabel : public QGUIControl {
public:
	QGUITextLabel() {
		m_xstretch = m_ystretch = false;
		m_font = 0;
		m_fontSize = 10;
		m_color = vector3(255.0f);
		m_alignment = ALIGN_CENTER | ALIGN_VCENTER;
	}

	~QGUITextLabel() = default;

	QGUIFont* Font() { return m_font; }
	void Font(QGUIFont* font) { m_font = font; m_dirty = true;  }

	int FontSize() { return m_fontSize; }
	void FontSize(int size) { m_fontSize = size; m_dirty = true; }

	std::string Text() { return m_text; }
	void Text(std::string text) { m_text = text; m_dirty = true; }

	vector3 Color() { return m_color; }
	void Color(vector3 color) { m_color = color; m_dirty = true; }

	/**
	 * Draw
	 */
	void Redraw();

protected:
	QGUIFont* m_font;
	int m_fontSize;

	vector3 m_color;

	std::string m_text;
};

#endif