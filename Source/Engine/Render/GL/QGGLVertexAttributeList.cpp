
#ifdef USE_OPENGL

#include <Render/GL/QGGLVertexAttributeList.h>
#include <Render/GL/QGGL.h>

void QGGLVertexAttributeList::EnableAttribute(int index, int attrib) {
	if (((int)attrib & m_attribBitmask) == 0) {
		return;
	}

	VertexAttrib attr = m_attribs[attrib];
	GL_CHECK(glEnableVertexAttribArray(index));
	GL_CHECK(glVertexAttribPointer(index, attr.size, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)(sizeof(float) * attr.offset)));
}

void QGGLVertexAttributeList::Bind() {
	if (m_vao == 0) {
		GL_CHECK(glCreateVertexArrays(1, &m_vao));
	}

	GL_CHECK(glBindVertexArray(m_vao));
}

#endif