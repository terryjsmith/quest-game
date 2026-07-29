
#ifndef qginputevent_h
#define qginputevent_h

#include <Core/QGEvent.h>
#include <IO/QGInputDevice.h>

class QUEST_API QGInputEvent : public QGEvent {
public:
	QGInputEvent(QGInputDevice* device, int button, float state) {
		this->device = device;
		this->button = button;
		this->state = state;
	}

	~QGInputEvent() = default;

public:
	QGInputDevice* device;
	int button;
	float state;
};

#endif