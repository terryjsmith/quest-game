
#ifndef qgmouse_h
#define qgmouse_h

#include <IO/QGCursor.h>

class QUEST_API QGMouse : public QGCursor {
public:
	QGMouse() = default;
	~QGMouse() = default;

	void Initialize();

	float GetButtonState(int button);
	void SetButtonState(int button, float state);

	vector2 GetCursorPosition();

protected:
	float m_buttonStates[CURSOR_BUTTON_LAST];
};

#endif