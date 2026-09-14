
#ifndef qganimationtransforms_h
#define qganimationtransforms_h

#include <Core/QGObject.h>

class QUEST_API QGAnimationTransforms : public QGObject {
public:
	QGAnimationTransforms() = default;
	~QGAnimationTransforms() = default;

	struct ScalingKey {
		int time;
		vector3 scaling;
	};

	struct TranslationKey {
		int time;
		vector3 position;
	};

	struct RotationKey {
		int time;
		quaternion rotation;
	};

public:
	std::vector<ScalingKey*> scalingKeys;
	std::vector<TranslationKey*> translationKeys;
	std::vector<RotationKey*> rotationKeys;
};

#endif