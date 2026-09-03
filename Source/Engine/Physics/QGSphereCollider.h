
#ifndef qgspherecollider_h
#define qgspherecollider_h

#include <Physics/QGCollisionShape.h>
#include <Core/QGSerializable.h>

class QUEST_API QGSphereCollider : public QGCollisionShape {
public:
	QGSphereCollider() : m_radius(0.0f) {}
	~QGSphereCollider() = default;

	/** 
	 * Initialize shape
	 */
	void Initialize(float radius);

	/**
	 * Serialize / deserialize
	 */
	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

	int ShapeType() { return QGCOLLISIONSHAPE_SPHERE; }

protected:
	// Radius of sphere
	float m_radius;
};

#endif