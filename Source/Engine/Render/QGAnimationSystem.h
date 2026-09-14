
#ifndef qganimationsystem_h
#define qganimationsystem_h

#include <Core/QGSystem.h>
#include <Render/QGAnimatedMeshComponent.h>

class QUEST_API QGAnimationSystem : public QGSystem {
public:
	QGAnimationSystem() = default;
	~QGAnimationSystem() = default;

	void Update(float delta);

protected:
	void ProcessNodeHierarchy(QGNode3D* node, QGAnimatedMeshComponent* mc, int currentFrame, matrix4 parent = matrix4(1.0f));
};

#endif