
#include <Render/UI/QGUISystem.h>

void QGUISystem::Update(float delta) {
	// Process cursor position and update states
}

void QGUISystem::Render(int width, int height) {
	if (m_activeScreen == 0) return;

	// Set screen width and height
	m_activeScreen->FixedWidth(width);
	m_activeScreen->FixedHeight(height);

	m_activeScreen->RecalculateDimensions(vector2(0, 0), vector2(width, height));

	// Traverse the hierarchy and ask to re-draw (if needed)
	m_activeScreen->Redraw();

	// Check for need new texture data
	if (m_width != width || m_height != height) {
		m_width = width;
		m_height = height;

		free(m_data);
		m_data = 0;

		m_data = (unsigned char*)malloc(width * height * 4);
	}

	if (m_data == 0) {
		m_data = (unsigned char*)malloc(width * height * 4);
	}

	memset(m_data, 0, width * height * 4);
	RecursiveRender(m_activeScreen, vector2(0.0f), vector2(width, height), m_data);
}

void QGUISystem::RecursiveRender(QGUIControl* node, vector2 topleft, vector2 bottomright, unsigned char* output) {
	// If disabled, don't render this or any children
	if (node->Enabled() == false) return;

	// Start from parent position
	vector2 position = vector2(0.0f);

	// Based on alignment, figure out differences
	int alignment = node->Alignment();
	if (alignment & QGUIAlignment::ALIGN_LEFT) {
		position.x = topleft.x;
	}
	if (alignment & QGUIAlignment::ALIGN_CENTER) {
		position.x = topleft.x + ((bottomright.x - topleft.x) / 2.0f) - (node->Width() / 2.0f);
	}
	if (alignment & QGUIAlignment::ALIGN_RIGHT) {
		position.x = bottomright.x - node->Width();
	}

	// Vertical alignment
	if (alignment & QGUIAlignment::ALIGN_TOP) {
		position.y = topleft.y;
	}
	if (alignment & QGUIAlignment::ALIGN_VCENTER) {
		position.y = topleft.y + ((bottomright.y - topleft.y) / 2.0f) - (node->Height() / 2.0f);
	}
	if (alignment & QGUIAlignment::ALIGN_BOTTOM) {
		position.y = bottomright.y - node->Height();
	}

	// Copy into data
	unsigned char* bytes = node->Bytes();
	if (bytes) {
		for (int y = 0; y < node->Height(); y++) {
			if (y >= bottomright.y || y >= m_height) continue;
			for (int x = 0; x < node->Width(); x++) {
				if (x >= bottomright.x || x >= m_width) continue;

				int localoffset = ((y * node->Width()) + x) * 4;
				int screenoffset = (((topleft.y + y) * m_width) + (topleft.x + x)) * 4;

				memcpy(output + screenoffset, bytes + localoffset, 4);
			}
		}
	}

	vector2 newbottomright = vector2(position.x + node->Width(), position.y + node->Height());
	auto children = node->Children();
	for (auto it = children.begin(); it != children.end(); it++) {
		RecursiveRender(*it, position, newbottomright, output);
	}
}