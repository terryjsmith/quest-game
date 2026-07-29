
#include <Core/QGTransform.h>

QGTransform::QGTransform() {
	position = vector3(0.0f);
	scaling = vector3(1.0f);
	rotation = quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

void QGTransform::Rotate(vector3 axis, float degrees) {
	quaternion newRotation = glm::angleAxis(glm::radians(degrees), axis);
	rotation = this->rotation * newRotation;
}

void QGTransform::Look(vector3 look) {
	// First, check for parallel vectors
	float dot = glm::dot(vector3(0, 0, -1), look);
	if (dot > 0.999999 || dot < -0.999999) {
		rotation = quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	// Otherwise, set rotation
	vector3 a = glm::cross(vector3(0, 0, -1), look);
	float v1len = glm::length(vector3(0, 0, -1));
	float v2len = glm::length(look);
	float w = glm::sqrt((v1len * v1len) * (v2len * v2len)) + dot;

	rotation.x = a.x;
	rotation.y = a.y;
	rotation.z = a.z;
	rotation.w = w;
}

vector3 QGTransform::Forward() {
	return(rotation * vector3(0, 0, -1));
}

vector3 QGTransform::Up() {
	return(rotation * vector3(0, 1, 0));
}

vector3 QGTransform::Right() {
	return(rotation * vector3(1, 0, 0));
}

matrix4 QGTransform::Matrix() {
	matrix4 t = glm::translate(glm::mat4(1.0), position);
	matrix4 s = glm::scale(glm::mat4(1.0), scaling);
	matrix4 r = glm::toMat4(rotation);

	matrix4 modelMatrix = t * r * s;

	return(modelMatrix);
}