
#include <Render/UI/QGUIButton.h>

void QGUIButton::Redraw() {
	if (m_dirty == false) return;
	if (m_bytes) free(m_bytes);
	
	m_bytes = (unsigned char*)malloc(m_width * m_height * 4);
	vector4 color = m_bgcolors[m_state];
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			int offset = ((y * m_width) + x) * 4;
			m_bytes[offset + 0] = color.r;
			m_bytes[offset + 1] = color.g;
			m_bytes[offset + 2] = color.b;
			m_bytes[offset + 3] = color.a;
		}
	}

	m_dirty = false;
}

void QGUIButton::OnMouseEvent(int type, QGUIMouseEvent* event) {
	if (type == QGUI_MOUSE_ENTER) {
		m_state = QGUI_STATE_HOVER;
		m_hover = true;
	}

	if (type == QGUI_MOUSE_EXIT) {
		m_state = QGUI_STATE_DEFAULT;
		m_hover = false;
	}

	if (type == QGUI_MOUSE_CLICK) {
		m_state = QGUI_STATE_CLICK;
		if (m_clickFunction) m_clickFunction();
	}

	if (type == QGUI_MOUSE_RELEASE) {
		if (m_hover) m_state = QGUI_STATE_HOVER;
		else m_state = QGUI_STATE_DEFAULT;
	}

	m_dirty = true;
}