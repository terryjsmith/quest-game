
#ifndef qgmesh_h
#define qgmesh_h

#include <IO/QGResourceObject.h>
#include <Render/QGVertexBuffer.h>
#include <Render/QGIndexBuffer.h>
#include <Render/QGTexture2D.h>
#include <Render/QGMaterial.h>
#include <Core/QGTransform.h>

class QUEST_API QGMesh : public QGResourceObject {
public:
	QGMesh() {
		vertexBuffer = 0;
		indexBuffer = 0;
		diffuseTexture = 0;
	}

	~QGMesh() {
		if (vertexBuffer) delete vertexBuffer;
		if (indexBuffer) delete indexBuffer;
	}

public:
	std::string name;

	QGVertexBuffer* vertexBuffer;
	QGIndexBuffer* indexBuffer;

	QGTexture2D* diffuseTexture;

	QGTransform transform;

	std::vector<QGMesh*> children;
};

#endif