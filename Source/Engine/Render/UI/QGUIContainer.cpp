
#include <Render/UI/QGUIContainer.h>

void QGUIContainer::Redraw() {
	if (m_dirty == false) return;
	if (m_bytes) free(m_bytes);

	m_bytes = (unsigned char*)malloc(m_width * m_height * 4);
	vector4 color = m_bgcolor;
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