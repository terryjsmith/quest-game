
#include <Physics/QGCollisionSystem.h>
#include <Core/QGWorld.h>
#include <Core/QGEventSystem.h>
#include <Core/QGApplication.h>
#include <Physics/QGPhysicsEvents.h>

void QGCollisionSystem::Initialize() {
    // Default configuration: stack collisions and allocator
    m_collisionConfiguration = new btDefaultCollisionConfiguration();

    // Use default bullet dispatcher
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    // General broadphase algorithm
    m_overlappingPairCache = new btDbvtBroadphase();

    // Default constraint solver
    m_solver = new btSequentialImpulseConstraintSolver();

    // Initialize physics world
    //m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
    m_dynamicsWorld = new btCollisionWorld(m_dispatcher, m_overlappingPairCache, m_collisionConfiguration);

    // Set gravity
    //m_dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
}

void QGCollisionSystem::Update(float delta) {
    QGWorld* world = QGWorld::GetInstance();
    QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();

    // Create a copy of the list of objects
    std::map<QGCollisionComponent*, bool> trackedColliders = m_collisionObjects;

    // Update position of all collision components
    std::vector<QGCollisionComponent*> colliders = world->GetComponents<QGCollisionComponent>();
    for (auto it = colliders.begin(); it != colliders.end(); it++) {
        vector3 position = (*it)->offset;
        quaternion rotation = quaternion(1, 0, 0, 0);

        QGEntity* entity = (*it)->Entity();
        if (entity) {
            position += entity->transform.position;
            rotation = entity->transform.rotation;
        }

        btTransform transform;
        transform.setIdentity();

        transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
        transform.setOrigin(btVector3(position.x, position.y, position.z));
        (*it)->m_collider->setWorldTransform(transform);

        // Make sure it's in the world
        auto cit = m_collisionObjects.find(*it);
        if (cit == m_collisionObjects.end()) {
            m_collisionObjects[*it] = false;
            m_dynamicsWorld->addCollisionObject((*it)->m_collider);
        }
        else {
            trackedColliders.erase(*it);
        }
    }

    // Remove objects no longer in use
    for (auto it = trackedColliders.begin(); it != trackedColliders.end(); it++) {
        m_collisionObjects.erase(it->first);
        //m_dynamicsWorld->removeCollisionObject(it->first->m_collider);
    }

    // Run simulation
    //m_dynamicsWorld->stepSimulation(delta);
    m_dynamicsWorld->performDiscreteCollisionDetection();

    // Update all collisions to inactive
    for (auto it = m_collisions.begin(); it != m_collisions.end(); it++) {
        it->second = false;
    }

    // Loop over contact manifolds to determine which objects are currently colliding/overlapping
    int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
        btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();

        QGCollisionComponent* bodyA = (QGCollisionComponent*)obA->getUserPointer();
        QGCollisionComponent* bodyB = (QGCollisionComponent*)obB->getUserPointer();

        QGEntity* entityA = bodyA->Entity();
        QGEntity* entityB = bodyB->Entity();

        // Can't collide with self
        if (entityA == entityB) continue;

        // Loop over our current collisions and see if we have this one
        auto it = m_collisions.begin();
        for (; it != m_collisions.end(); i++) {
            if (((it->first->objA == entityA && it->first->objB == entityB)) || ((it->first->objA == entityB && it->first->objB == entityA))) {
                it->second = true;
                break;
            }
        }

        if (it == m_collisions.end()) {
            QGTrackedCollision* newCollision = new QGTrackedCollision();
            newCollision->objA = entityA;
            newCollision->objB = entityB;

            m_collisions[newCollision] = true;

            // Publish events in both directions
            eventSystem->Publish(new QGPhysicsCollisionEvent(entityA), entityB);
            eventSystem->Publish(new QGPhysicsCollisionEvent(entityB), entityA);
        }
    }

    // Remove inactive collisions
    for (auto it = m_collisions.begin(); it != m_collisions.end(); ) {
        if (it->second == false) {
            delete it->first;
            it = m_collisions.erase(it);
        }
        else {
            it++;
        }
    }
}

std::vector<QGEntity*> QGCollisionSystem::Intersects(QGEntity* entity) {
    std::vector<QGEntity*> entities;
    for (auto it = m_collisions.begin(); it != m_collisions.end(); it++) {
        if (it->first->objA == entity) {
            entities.push_back(it->first->objB);
            continue;
        }

        if (it->first->objB == entity) {
            entities.push_back(it->first->objA);
            continue;
        }
    }

    return(entities);
}