
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

	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

public:
	QGMesh* mesh;
};

#endif