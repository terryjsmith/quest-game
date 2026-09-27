
#ifndef qguicontainer_h
#define qguicontainer_h

#include <Render/UI/QGUIControl.h>
#include <Render/UI/QGUI.h>

class QUEST_API QGUIContainer : public QGUIControl {
public:
	QGUIContainer() {
		m_maxHeight = m_maxWidth = 0;
		m_scaleX = m_scaleY = true;
	}
	~QGUIContainer() = default;

	bool ScaleX() { return m_scaleX; }
	virtual void ScaleX(bool scaling) { m_scaleX = scaling; }
	bool ScaleY() { return m_scaleY; }
	virtual void ScaleY(bool scaling) { m_scaleY = scaling; }

	int MaxWidth() { return m_maxWidth; }
	virtual void MaxWidth(int width) { m_maxWidth = width; }
	int MaxHeight() { return m_maxHeight; }
	virtual void MaxHeight(int height) { m_maxHeight = height; }

	/**
	 * Properties
	 */
	vector4 BGColor() { return m_bgcolor; }
	void BGColor(vector4 color) { m_bgcolor = color; }

	/**
	 * Draw
	 */
	void Redraw();

protected:
	bool m_scaleX, m_scaleY;
	int m_maxWidth, m_maxHeight;

	vector4 m_bgcolor;
};

#endif