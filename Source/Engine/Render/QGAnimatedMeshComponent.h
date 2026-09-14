
#ifndef qganimatedmeshcomponent_h
#define qganimatedmeshcomponent_h

#include <Render/QGMeshComponent.h>
#include <Render/QGAnimation.h>

class QUEST_API QGAnimatedMeshComponent : public QGMeshComponent {
public:
	QGAnimatedMeshComponent() {
		startTime = 0;
		looping = false;
		activeAnimation = 0;
	}
	~QGAnimatedMeshComponent() = default;

	void Play(std::string animationName, bool loop = false, uint64_t startTick = 0);
	void Stop();

	void AddAnimation(std::string name, QGAnimation* animation);

	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

public:
	// Active animation name
	QGAnimation* activeAnimation;

	// Available animations
	std::map<std::string, QGAnimation*> animations;

	// Start timestamp (in ticks)
	uint64_t startTime;

	// Looping
	bool looping;

	// Current bone matrices
	std::map<std::string, matrix4> boneMatrix;
};

#endif