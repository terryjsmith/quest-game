
#ifndef qgresourceobject_h
#define qgresourceobject_h

#include <IO/QGResource.h>

class QUEST_API QGResourceObject : public QGObject {
public:
	QGResourceObject() {
		resource = 0;
	}
	virtual ~QGResourceObject() = default;

public:
	QGResource* resource;
};

#endif