
#include <Physics/QGSphereCollider.h>

void QGSphereCollider::Initialize(float radius) {
	if (m_collisionShape == 0) {
		m_collisionShape = new btSphereShape(radius);
	}
	else {
		if (m_radius != radius) {
			btSphereShape* shape = (btSphereShape*)m_collisionShape;
			shape->setUnscaledRadius(radius);
		}
	}

	m_radius = radius;
}

void QGSphereCollider::Serialize(QGDataRecord* record) {
	record->Set("radius", m_radius);
}

void QGSphereCollider::Deserialize(QGDataRecord* record) {
	float radius = record->Get("radius").AsFloat();
	this->Initialize(radius);
}