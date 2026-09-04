
#ifdef USE_OPENGL

#include <Render/GL/QGGLIndexBuffer.h>
#include <Render/GL/QGGL.h>

void QGGLIndexBuffer::Bind() {
	// If we do not yet have a buffer, create one and load data into it
	if (m_buffer == 0) {
		GL_CHECK(glCreateBuffers(1, &m_buffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_count, m_data, GL_STATIC_DRAW));
		return;
	}

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));
}

#endif