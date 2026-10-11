
#ifndef qgcursor_h
#define qgcursor_h

#include <IO/QGInputDevice.h>

class QUEST_API QGCursor : public QGInputDevice {
public:
	QGCursor() = default;
	~QGCursor() = default;

	virtual vector2 GetCursorPosition() { return vector2(-1, -1); }
};

#endif