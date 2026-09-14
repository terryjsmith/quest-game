
#ifndef qganimation_h
#define qganimation_h

#include <Render/QGAnimationTransforms.h>
#include <IO/QGResourceObject.h>

class QUEST_API QGAnimation : public QGResourceObject {
public:
	QGAnimation() {
		duration = 0;
		speed = 0;
	}
	~QGAnimation() = default;

public:
	// Name of animation
	std::string name;

	// Duration (in frames)
	int duration;

	// Speed (fps)
	int speed;

	// List of node specific transforms
	std::map<std::string, QGAnimationTransforms*> transforms;
};

#endif