
#include <Render/QGTextureLoader.h>
#include <Render/QGTexture2D.h>
#include <Render/QGRenderSystem.h>
#include <Core/QGApplication.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

QGResourceObject* QGTextureLoader::LoadResource(QGResource* resource, std::string type) {
	// Create a texture
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
	QGTexture2D* texture = renderSystem->CreateTexture2D();

	int width, height, channels;
	unsigned char *data = stbi_load_from_memory(resource->Data(), resource->filesize, &width, &height, &channels, 0);

	texture->Create(width, height, channels, QGTexture2D::QGTEXTURE_BYTE, data);
	stbi_image_free(data);

	return(texture);
}