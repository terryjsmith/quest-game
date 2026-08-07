
#include <Render/QGShaderLoader.h>
#include <Render/QGRenderSystem.h>
#include <Core/QGApplication.h>

QGResourceObject* QGShaderLoader::LoadResource(QGResource* resource, std::string type) {
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
	QGShader* shader = renderSystem->CreateShader();
	return(shader);
}