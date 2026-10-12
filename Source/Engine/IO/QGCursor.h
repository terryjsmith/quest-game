
#ifndef qgcursor_h
#define qgcursor_h

#include <IO/QGInputDevice.h>

enum QUEST_API QGCursorButtons {
	CURSOR_BUTTON_LEFT = 0,
	CURSOR_BUTTON_RIGHT = 1,
	CURSOR_BUTTON_MIDDLE = 2,
	CURSOR_BUTTON_LAST
};

class QUEST_API QGCursor : public QGInputDevice {
public:
	QGCursor() = default;
	~QGCursor() = default;

	virtual vector2 GetCursorPosition() { return vector2(-1, -1); }
};

#endif