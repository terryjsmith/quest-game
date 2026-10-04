
#include <Render/QGTexture2D.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void QGTexture2D::Save(std::string filename) {
	stbi_write_png(filename.c_str(), width, height, channels, m_data, width * channels);
}