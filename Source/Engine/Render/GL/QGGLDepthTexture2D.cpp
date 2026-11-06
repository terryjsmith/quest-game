
#ifdef USE_OPENGL

#include <Render/GL/QGGLDepthTexture2D.h>
#include <Render/GL/QGGL.h>

void QGGLDepthTexture2D::Bind(int slot) {
	if (m_texture == 0 || m_dirty == true) {
		if (m_texture == 0) {
			GL_CHECK(glGenTextures(1, &m_texture));
		}

		// Set internal variables from format
		unsigned int internalFormat = GL_DEPTH_COMPONENT32F;

		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture));
		GL_CHECK(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			internalFormat,
			width,
			height,
			0,
			GL_DEPTH_COMPONENT,
			type == QGTextureDataType::QGTEXTURE_BYTE ? GL_UNSIGNED_BYTE : GL_FLOAT,
			m_data));

		m_dirty = false;
	}

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

#endif