
#include <Render/GL/QGGLTexture2D.h>
#include <Render/GL/QGGL.h>

void QGGLTexture2D::Bind(int slot) {
	if (m_texture == 0) {
		GL_CHECK(glGenTextures(1, &m_texture));

		// Set internal variables from format
		unsigned int internalFormat = 0;
		switch (channels) {
		case 1:
			internalFormat = GL_RED;
			break;
		case 2:
			internalFormat = GL_RG;
			break;
		case 3:
			internalFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			break;
		default: break;
		}

		QGASSERT(internalFormat != 0, "Unable to determine type.");

		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture));
		GL_CHECK(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			internalFormat,
			width,
			height,
			0,
			internalFormat,
			type == QGTextureDataType::QGTEXTURE_BYTE ? GL_UNSIGNED_BYTE : GL_FLOAT,
			m_data));

		GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture));

	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY ? GL_REPEAT : GL_CLAMP_TO_EDGE));

	GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter));
	GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter));
}