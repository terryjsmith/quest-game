
#ifndef qgscene_h
#define qgscene_h

#include <Render/QGMeshComponent.h>
#include <Core/QGCameraComponent.h>

class QUEST_API QGScene : public QGObject {
public:
	QGScene() = default;
	~QGScene() = default;

public:
	QGCameraComponent* camera;
	std::vector<QGMeshComponent*> meshes;
};

#endif