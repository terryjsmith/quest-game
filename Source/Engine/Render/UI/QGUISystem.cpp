
#include <Render/UI/QGUISystem.h>
#include <IO/QGInputSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGMouse.h>

void QGUISystem::Update(float delta) {
	// Process cursor position and update states
	QGInputSystem* inputSystem = GetQGSystem<QGInputSystem>();
	QGMouse* mouse = inputSystem->FindInputDevice<QGMouse>();

	vector2 cursor = mouse->GetCursorPosition();

	// Process hierarchy to look for intersections
	RecursiveCursorEventHandler(m_activeScreen, QGUI_MOUSE_MOVE, cursor);
	m_lastCursorPos = cursor;
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

	// Save position
	node->XPos(position.x);
	node->YPos(position.y);

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

void QGUISystem::RecursiveCursorEventHandler(QGUIControl* node, int type, vector2 cursorpos) {
	if (node->Enabled() == false) return;

	// Get last known position and width and height to establish bounds
	vector2 position = vector2(node->XPos(), node->YPos());
	vector2 dimensions = vector2(node->Width(), node->Height());

	// Check for intersection
	bool currentIntersection = false;
	bool previousIntersection = false;
	if (cursorpos.x >= position.x && cursorpos.x <= (position.x + dimensions.x) &&
		cursorpos.y >= position.y && cursorpos.y <= (position.y + dimensions.y)) {
		currentIntersection = true;
	}

	if (m_lastCursorPos.x >= position.x && m_lastCursorPos.x <= (position.x + dimensions.x) &&
		m_lastCursorPos.y >= position.y && m_lastCursorPos.y <= (position.y + dimensions.y)) {
		previousIntersection = true;
	}

	// If this is a new intersection, send a mouse enter event
	if (type == QGUI_MOUSE_MOVE) {
		if (currentIntersection == true && previousIntersection == false) {
			QGUIMouseEvent ev;
			ev.type = QGUI_MOUSE_ENTER;
			ev.xpos = cursorpos.x;
			ev.ypos = cursorpos.y;

			node->OnMouseEvent(QGUI_MOUSE_ENTER, &ev);
		}

		// If we are not intersecting, but were before, send exit event
		if (previousIntersection == true && currentIntersection == false) {
			QGUIMouseEvent ev;
			ev.type = QGUI_MOUSE_EXIT;
			ev.xpos = cursorpos.x;
			ev.ypos = cursorpos.y;

			node->OnMouseEvent(QGUI_MOUSE_EXIT, &ev);
		}
	}

	// If this is a click
	if (type == QGUI_MOUSE_CLICK) {
		if (currentIntersection == true) {
			QGUIMouseEvent ev;
			ev.type = QGUI_MOUSE_CLICK;
			ev.xpos = cursorpos.x;
			ev.ypos = cursorpos.y;

			node->OnMouseEvent(QGUI_MOUSE_CLICK, &ev);
		}
	}

	// If this is a mouse button release
	if (type == QGUI_MOUSE_RELEASE) {
		if (currentIntersection == true) {
			QGUIMouseEvent ev;
			ev.type = QGUI_MOUSE_RELEASE;
			ev.xpos = cursorpos.x;
			ev.ypos = cursorpos.y;

			node->OnMouseEvent(QGUI_MOUSE_RELEASE, &ev);
		}
	}

	// Process children
	auto children = node->Children();
	for (auto it = children.begin(); it != children.end(); it++) {
		RecursiveCursorEventHandler(*it, type, cursorpos);
	}
}