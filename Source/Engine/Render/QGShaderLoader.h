
#ifndef qgshaderloader_h
#define qgshaderloader_h

#include <Render/QGShader.h>
#include <IO/QGResourceLoader.h>

class QUEST_API QGShaderLoader : public QGResourceLoader {
public:
	QGShaderLoader() = default;
	~QGShaderLoader() = default;

	QGResourceObject* LoadResource(QGResource* resource, std::string type);
};

#endif