
#ifndef qganimationloader_h
#define qganimationloader_h

#include <IO/QGResourceLoader.h>
#include <Render/QGAnimation.h>

class QUEST_API QGAnimationLoader : public QGResourceLoader {
public:
	QGAnimationLoader() = default;
	~QGAnimationLoader() = default;

	QGResourceObject* LoadResource(QGResource* resource, std::string type);
};

#endif