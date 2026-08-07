
#include <Render/GL/QGGLVertexBuffer.h>
#include <Render/GL/QGGL.h>

void QGGLVertexBuffer::Bind() {
	// If we do not yet have a buffer, create one and load data into it
	if (m_buffer == 0) {
		GL_CHECK(glCreateBuffers(1, &m_buffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_attribs->GetVertexSize() * m_count, m_data, GL_STATIC_DRAW));
		return;
	}

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
}