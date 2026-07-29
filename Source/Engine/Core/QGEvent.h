
#ifndef qgevent_h
#define qgevent_h

#include <Core/QGObject.h>

class QUEST_API QGEvent : public QGObject {
public:
	QGEvent() = default;
	virtual ~QGEvent() = default;
};

#endif