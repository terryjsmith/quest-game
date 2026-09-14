
#ifndef qgnode3d_h
#define qgnode3d_h

#include <Core/QGObject.h>

class QUEST_API QGNode3D : public QGObject {
public:
	QGNode3D() {
		parent = 0;
		transform = matrix4(1.0f);
	}
	~QGNode3D() = default;

public:
	// Parent and child nodes
	QGNode3D* parent;
	std::vector<QGNode3D*> children;

	// Node name
	std::string name;

	// Node transformation
	matrix4 transform;
};

#endif