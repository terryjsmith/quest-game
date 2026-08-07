
#ifndef qgglvertexbuffer_h
#define qgglvertexbuffer_h

#include <Render/QGVertexBuffer.h>

class QUEST_API QGGLVertexBuffer : public QGVertexBuffer {
public:
	QGGLVertexBuffer() : m_buffer(0) {}
	~QGGLVertexBuffer() = default;

	/**
	 * Bind / use this buffer
	 */
	void Bind();

protected:
	unsigned int m_buffer;
};

#endif