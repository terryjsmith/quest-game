
#include <Render/UI/QGUIControl.h>

void QGUIControl::RecalculateDimensions(vector2 topleft, vector2 bottomright) {
	m_width = m_height = -1;

	// Before we check the children, check expansion factor
	if (m_xstretch) {
		m_width = bottomright.x - topleft.x;
	}
	if (m_ystretch) {
		m_height = bottomright.y - topleft.y;
	}

	// If we have a fixed width, use that
	if (m_fixedWidth) {
		m_width = m_fixedWidth;
	}

	if (m_fixedHeight) {
		m_height = m_fixedHeight;
	}

	// Calculate child sizes
	for (auto it = m_children.begin(); it != m_children.end(); it++) {
		(*it)->RecalculateDimensions(topleft, topleft + vector2(m_width, m_height));
	}

	// Once the children are figured out, figure ourselves out, figure out our sized based on theirs
	if (m_width == -1) {
		if (m_align == QGUIAlign::ALIGN_VERTICAL) {
			// Find the widest item
			int width = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				width = std::max(width, (*it)->Width());
			}
		}
		else {
			// Sum up width of all elements
			m_width = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				m_width += (*it)->Width();
			}
		}
	}

	if (m_height == -1) {
		if (m_align == QGUIAlign::ALIGN_VERTICAL) {
			// Sum height of all items
			m_height = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				m_height += (*it)->Height();
			}
		}
		else {
			// Find highest element
			int height = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				height = std::max(height, (*it)->Height());
			}
		}
	}
}