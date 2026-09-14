
#ifndef qgbone3d_h
#define qgbone3d_h

#include <Core/QGObject.h>

class QUEST_API QGBone3D : public QGObject {
public:
	QGBone3D() {
		offsetMatrix = matrix4(1.0f);
	}
	~QGBone3D() = default;

public:
	// Name of bone
	std::string name;

	// Offset matrix
	matrix4 offsetMatrix;
};

#endif