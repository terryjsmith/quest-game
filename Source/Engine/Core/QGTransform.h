
#ifndef qgtransform_h
#define qgtransform_h

#include <Core/QGObject.h>

/**
 * A transform (combination of position, rotation, and scale)
 */
class QUEST_API QGTransform : public QGObject {
public:
	QGTransform();
	~QGTransform() = default;

	/**
	 * Move from current position (add to position)
	 */
	void Move(vector3 amount) { this->position += amount; }

	/**
	 * Rotate
	 */
	void Rotate(quaternion rotation) { this->rotation *= rotation; }
	void Rotate(vector3 axis, float degrees);

	/**
	 * Scale by a factor
	 */
	void Scale(vector3 factor) { this->scaling *= factor; }

	/**
	 * Set the rotation by specifying a new look vector
	 */
	void Look(vector3 look);

	/**
	 * Axis vectors (oriented to rotation)
	 */
	vector3 Forward();
	vector3 Up();
	vector3 Right();

	/**
	 * Transformation matrix
	 */
	matrix4 Matrix();

public:
	// Position in 3D space
	vector3 position;

	// Scaling factor
	vector3 scaling;

	// Rotation
	quaternion rotation;
};

#endif