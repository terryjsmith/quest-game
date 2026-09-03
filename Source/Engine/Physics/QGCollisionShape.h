
#ifndef qgcollisionshape_h
#define qgcollisionshape_h

#include <Core/QGObject.h>
#include <btBulletCollisionCommon.h>
#include <Core/QGSerializable.h>

enum QGCollisionShapeType {
	QGCOLLISIONSHAPE_SPHERE = 1,
	QGCOLLISIONSHAPE_BOX,
	QGCOLLISIONSHAPE_CAPSULE,
	QGCOLLISIONSHAPE_MESH
};

class QUEST_API QGCollisionShape : public QGObject, public QGSerializable {
public:
	QGCollisionShape() : m_collisionShape(0) {}
	virtual ~QGCollisionShape() = default;

	virtual int ShapeType() = 0;
	btCollisionShape* Shape() { return m_collisionShape; }

protected:
	// Collision shape
	btCollisionShape* m_collisionShape;
};

#endif