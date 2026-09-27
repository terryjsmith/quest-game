
#ifndef qguicontrol_h
#define qguicontrol_h

#include <Core/QGObject.h>
#include <Render/UI/QGUI.h>

class QUEST_API QGUIControl : public QGObject {
public:
	QGUIControl() {
		m_width = m_height = -1;
		m_alignment = ALIGN_TOP | ALIGN_LEFT;
		m_xstretch = m_ystretch = false;
		m_bytes = 0;
		m_dirty = false;
		m_fixedWidth = m_fixedHeight = -1;
		m_parent = 0;
		m_align = QGUIAlign::ALIGN_VERTICAL;
		m_enabled = true;
	}
	~QGUIControl() = default;

	/**
	 * Properties
	 */
	int Width() { return m_width; }
	virtual void Width(int width) { m_width = width; }

	int Height() { return m_height; }
	virtual void Height(int height) { m_height = height; }

	int Alignment() { return m_alignment; }
	virtual void Alignment(int alignment) { m_alignment = alignment; }

	bool XStretch() { return m_xstretch; }
	void XStretch(bool stretch) { m_xstretch = stretch; }

	bool YStretch() { return m_ystretch; }
	void YStretch(bool stretch) { m_ystretch = stretch; }

	int FixedWidth() { return m_fixedWidth; }
	void FixedWidth(int width) { m_fixedWidth = width; }

	int FixedHeight() { return m_fixedHeight; }
	void FixedHeight(int height) { m_fixedHeight = height; }

	unsigned char* Bytes() { return m_bytes; }

	bool Enabled() { return m_enabled; }
	void Enabled(bool enabled) { m_enabled = enabled; }

	/**
	 * Hierarchy
	 */
	std::vector<QGUIControl*> Children() { return m_children; }
	void AddChild(QGUIControl* child) { m_children.push_back(child); child->Parent(this); }
	void RemoveChild(QGUIControl* child) { 
		auto it = std::find(m_children.begin(), m_children.end(), child); 
		if (it != m_children.end()) m_children.erase(it);
	}

	QGUIControl* Parent() { return m_parent; }
	void Parent(QGUIControl* parent) { m_parent = parent; }

	/**
	 * Redraw
	 */
	virtual void Redraw() = 0;

	/**
	 * Recalculate width and height from settings
	 */
	virtual void RecalculateDimensions(vector2 topleft, vector2 bottomright);

public:
	/**
	 * Callbacks / event handlers
	 */
	virtual void OnMouseEvent(int type, QGUIMouseEvent* event) {}
	virtual void OnKeyboardEvent(int type, QGUIKeyboardEvent* event) {}

protected:
	std::vector<QGUIControl*> m_children;

protected:
	int m_width, m_height;
	int m_alignment;
	bool m_xstretch, m_ystretch;
	int m_fixedWidth, m_fixedHeight;
	QGUIAlign m_align;

	unsigned char* m_bytes;
	bool m_dirty;

	bool m_enabled;

	QGUIControl* m_parent;
};

#endif