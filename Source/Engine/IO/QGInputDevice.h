
#ifndef qginputdevice_h
#define qginputdevice_h

#include <Core/QGObject.h>

class QUEST_API QGInputDevice : public QGObject {
public:
	QGInputDevice() = default;
	virtual ~QGInputDevice() = default;

	virtual void Initialize() { }

	virtual float GetButtonState(int button) { return(0.0f); }
	virtual void SetButtonState(int button, float state) { }
};

#endif