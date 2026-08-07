
#ifndef qgtextureloader_h
#define qgtextureloader_h

#include <IO/QGResourceLoader.h>

class QUEST_API QGTextureLoader : public QGResourceLoader {
public:
	QGTextureLoader() = default;
	~QGTextureLoader() = default;

	QGResourceObject* LoadResource(QGResource* resource, std::string type);
};

#endif