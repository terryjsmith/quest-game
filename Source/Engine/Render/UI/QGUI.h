
#ifndef qgui_h
#define qgui_h

enum QGUIAlignment {
	ALIGN_LEFT = 1 << 0,
	ALIGN_CENTER = 1 << 1,
	ALIGN_RIGHT = 1 << 2,
	ALIGN_TOP = 1 << 3,
	ALIGN_VCENTER = 1 << 4,
	ALIGN_BOTTOM = 1 << 5
};

enum QGUIAlign {
	ALIGN_VERTICAL = 1,
	ALIGN_HORIZONTAL
};

enum QGUIEvents {
	QGUI_MOUSE_CLICK = 1,
	QGUI_MOUSE_RELEASE,
	QGUI_KEY_PRESS,
	QGUI_SCROLL,
	QGUI_MOUSE_ENTER,
	QGUI_MOUSE_EXIT,
};

enum QGUIState {
	QGUI_STATE_DEFAULT = 0,
	QGUI_STATE_HOVER,
	QGUI_STATE_CLICK
};

struct QGUIMouseEvent {
	int type;
	int xpos, ypos;
};

struct QGUIKeyboardEvent {
	int type;
	int key;
};

#endif