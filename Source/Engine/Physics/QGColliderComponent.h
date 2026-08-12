
#ifndef qgcollidercomponent_h
#define qgcollidercomponent_h

#include <Core/QGComponent.h>
#include <btBulletCollisionCommon.h>

class QUEST_API QGColliderComponent : public QGComponent {
public:
	QGColliderComponent() = default;
	~QGColliderComponent() = default;

protected:
    // Bullet object
    btCollisionObject* m_collider;

    // Collision shape
    btCollisionShape* m_collisionShape;
};

#endif