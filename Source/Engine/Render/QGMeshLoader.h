
#ifndef qgmeshloader_h
#define qgmeshloader_h

#include <IO/QGResourceLoader.h>
#include <Render/QGMesh.h>

class QUEST_API QGMeshLoader : public QGResourceLoader {
public:
	QGMeshLoader() = default;
	~QGMeshLoader() = default;

	QGResourceObject* LoadResource(QGResource* resource, std::string type);
};

#endif