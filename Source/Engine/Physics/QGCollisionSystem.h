
#ifndef qgcollisionsystem_h
#define qgcollisionsystem_h

#include <Core/QGSystem.h>
#include <Physics/QGCollisionComponent.h>
#include <btBulletDynamicsCommon.h>

class QUEST_API QGCollisionSystem : public QGSystem {
public:
    QGCollisionSystem() = default;
    ~QGCollisionSystem() = default;

    /**
     * Initialize physics world
     */
    void Initialize();

    /**
    * Update our physics (step simulation)
    */
    void Update(float delta);

    /**
     * Return a list of all objects currently intersecting with a specified object
     */
    std::vector<QGEntity*> Intersects(QGEntity* entity);

protected:
    // Physics collision tracking
    struct QGTrackedCollision {
        QGEntity* objA;
        QGEntity* objB;
    };

protected:
    // Bullet configuration
    btDefaultCollisionConfiguration* m_collisionConfiguration;

    // Collision dispatcher
    btCollisionDispatcher* m_dispatcher;

    // Tracked collision cache
    btBroadphaseInterface* m_overlappingPairCache;

    // Solver
    btSequentialImpulseConstraintSolver* m_solver;

    // Physics world
    btCollisionWorld* m_dynamicsWorld;

    // Cache of collision objects and rigid bodies
    std::map<QGCollisionComponent*, bool> m_collisionObjects;

    // Cache of tracked collisions
    std::map<QGTrackedCollision*, bool> m_collisions;
};

#endif