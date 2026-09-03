
#ifndef qgphysicsevents_h
#define qgphysicsevents_h

#include <Core/QGEvent.h>
#include <Core/QGEntity.h>

class QGPhysicsCollisionEvent : public QGEvent {
public:
	QGPhysicsCollisionEvent(QGEntity* objB) : other(objB) {}
	~QGPhysicsCollisionEvent() = default;

public:
	QGEntity* other;
};

#endif