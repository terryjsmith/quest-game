
#ifndef qgmeshcomponent_h
#define qgmeshcomponent_h

#include <Render/QGMesh.h>
#include <Core/QGComponent.h>

class QUEST_API QGMeshComponent : public QGComponent {
public:
	QGMeshComponent() {
		mesh = 0;
	}
	~QGMeshComponent() = default;

	virtual void Serialize(QGDataRecord* record);
	virtual void Deserialize(QGDataRecord* record);

public:
	QGMesh* mesh;
};

#endif