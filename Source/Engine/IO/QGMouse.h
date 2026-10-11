
#ifndef qgmouse_h
#define qgmouse_h

#include <IO/QGCursor.h>

enum QUEST_API QGMouseButtons {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2,
	MOUSE_BUTTON_LAST
};

class QUEST_API QGMouse : public QGCursor {
public:
	QGMouse() = default;
	~QGMouse() = default;

	void Initialize();

	float GetButtonState(int button);
	void SetButtonState(int button, float state);

	vector2 GetCursorPosition();

protected:
	float m_buttonStates[MOUSE_BUTTON_LAST];
};

#endif