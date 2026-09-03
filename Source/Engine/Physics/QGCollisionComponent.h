
#ifndef qgcollisioncomponent_h
#define qgcollisioncomponent_h

#include <Core/QGComponent.h>
#include <Physics/QGCollisionShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Core/QGEntity.h>

class QGCollisionSystem;

class QUEST_API QGCollisionComponent : public QGComponent {
public:
    QGCollisionComponent() {
        m_collider = 0;
        m_shape = 0;
        offset = vector3(0);
    }
	virtual ~QGCollisionComponent() = default;

    void Serialize(QGDataRecord* record);
    void Deserialize(QGDataRecord* record);

    void Shape(QGCollisionShape* shape);
    QGCollisionShape* Shape() { return m_shape; }

    friend class QGCollisionSystem;

public:
    vector3 offset;

protected:
    // Bullet object
    btCollisionObject* m_collider;

    // Collision shape
    QGCollisionShape* m_shape;
};

#endif