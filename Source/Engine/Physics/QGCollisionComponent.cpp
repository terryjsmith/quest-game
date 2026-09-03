
#include <Physics/QGCollisionComponent.h>
#include <Physics/QGSphereCollider.h>

void QGCollisionComponent::Serialize(QGDataRecord* record) {
	record->Set("offset", offset);

	// Serialize shape info
	record->Set("type", m_shape->ShapeType());
	m_shape->Serialize(record);
}

void QGCollisionComponent::Deserialize(QGDataRecord* record) {
	this->offset = record->Get("offset").AsVector3();

	// Parse / create shape
	int type = record->Get("type").AsInt();
	if (m_shape) {
		// If we already have a shape, check the type
		if (m_shape->ShapeType() == type) {
			// Deserialize
			m_shape->Deserialize(record);
			return;
		}

		delete m_shape;
		m_shape = 0;
	}

	// If we get here, we need a new shape
	if (type == QGCOLLISIONSHAPE_SPHERE) {
		m_shape = new QGSphereCollider();
	}
	
	m_shape->Deserialize(record);
	this->Shape(m_shape);
}

void QGCollisionComponent::Shape(QGCollisionShape* shape) {
	if (m_collider == 0) {
		m_collider = new btGhostObject();
	}

	m_shape = shape;
	m_collider->setCollisionShape(m_shape->Shape());
	m_collider->setActivationState(DISABLE_DEACTIVATION);
	m_collider->setUserPointer(this);
}
