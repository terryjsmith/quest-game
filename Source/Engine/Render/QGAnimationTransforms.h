
#ifndef qganimationtransforms_h
#define qganimationtransforms_h

#include <Core/QGObject.h>

class QUEST_API QGAnimationTransforms : public QGObject {
public:
	QGAnimationTransforms() = default;
	~QGAnimationTransforms() = default;

	struct ScalingKey {
		double time;
		vector3 scaling;
	};

	struct TranslationKey {
		double time;
		vector3 position;
	};

	struct RotationKey {
		double time;
		quaternion rotation;
	};

public:
	std::map<double, ScalingKey> scalingKeys;
	std::map<double, TranslationKey> translationKeys;
	std::map<double, RotationKey> rotationKeys;
};

#endif