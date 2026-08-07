
#ifndef qgresourceloader_h
#define qgresourceloader_h

#include <Core/QGObject.h>
#include <IO/QGResourceObject.h>

class QUEST_API QGResourceLoader : public QGObject {
public:
	QGResourceLoader() = default;
	virtual ~QGResourceLoader() = default;

	virtual QGResourceObject* LoadResource(QGResource* resource, std::string type) = 0;
};

#endif