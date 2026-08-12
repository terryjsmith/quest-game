
#include <Core/QGCameraComponent.h>

matrix4 QGCameraComponent::ProjectionMatrix() {
	return(glm::perspective(glm::radians(fov), aspectRatio, fnear, ffar));
}

matrix4 QGCameraComponent::ViewMatrix() {
	return(glm::lookAt(transform.position, m_target, vector3(0, 1, 0)));
}

void QGCameraComponent::Serialize(QGDataRecord* record) {
	record->Set("position", transform.position);
	record->Set("rotation", transform.rotation);
	record->Set("fov", fov);
}

void QGCameraComponent::Deserialize(QGDataRecord* record) {
	transform.position = record->Get("position").AsVector3();
	transform.rotation = record->Get("rotation").AsQuaternion();
	fov = record->Get("fov").AsFloat();
}