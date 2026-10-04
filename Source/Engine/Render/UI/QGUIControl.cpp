
#include <Render/UI/QGUIControl.h>

void QGUIControl::RecalculateDimensions(vector2 topleft, vector2 bottomright) {
	int width, height;
	width = height = -1;

	// Before we check the children, check expansion factor
	if (m_xstretch) {
		width = bottomright.x - topleft.x;
	}
	if (m_ystretch) {
		height = bottomright.y - topleft.y;
	}

	// If we have a fixed width, use that
	if (m_fixedWidth) {
		width = m_fixedWidth;
	}

	if (m_fixedHeight) {
		height = m_fixedHeight;
	}

	// Calculate child sizes
	for (auto it = m_children.begin(); it != m_children.end(); it++) {
		(*it)->RecalculateDimensions(topleft, topleft + vector2(width, height));
	}

	// Once the children are figured out, figure ourselves out, figure out our sized based on theirs
	if (width == -1) {
		if (m_align == QGUIAlign::ALIGN_VERTICAL) {
			// Find the widest item
			int maxWidth = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				maxWidth = std::max(maxWidth, (*it)->Width());
			}
			width = maxWidth;
		}
		else {
			// Sum up width of all elements
			width = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				width += (*it)->Width();
			}
		}
	}

	if (height == -1) {
		if (m_align == QGUIAlign::ALIGN_VERTICAL) {
			// Sum height of all items
			height = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				height += (*it)->Height();
			}
		}
		else {
			// Find highest element
			int maxHeight = 0;
			for (auto it = m_children.begin(); it != m_children.end(); it++) {
				maxHeight = std::max(maxHeight, (*it)->Height());
			}
			height = height;
		}
	}

	if (m_width != width || m_height != height) m_dirty = true;

	m_width = width;
	m_height = height;
}