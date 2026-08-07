
#ifndef qgglvertexattributelist_h
#define qgglvertexattributelist_h

#include <Render/QGVertexAttributeList.h>

class QUEST_API QGGLVertexAttributeList : public QGVertexAttributeList {
public:
	QGGLVertexAttributeList() : m_vao(0) {}
	~QGGLVertexAttributeList() = default;

	/**
	 * Enable a particular vertex attribute (if this vertex type has it)
	 */
	void EnableAttribute(int index, int attrib);

	/**
	 * Bind/unbind this vertex type
	 */
	void Bind();

protected:
	unsigned int m_vao;
};

#endif