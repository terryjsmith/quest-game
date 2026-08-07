
#ifndef qgglindexbuffer_h
#define qgglindexbuffer_h

#include <Render/QGIndexBuffer.h>

class QUEST_API QGGLIndexBuffer : public QGIndexBuffer {
public:
	QGGLIndexBuffer() : m_buffer(0) {}
	~QGGLIndexBuffer() = default;

	/**
	 * Bind / use this buffer
	 */
	void Bind();

protected:
	unsigned int m_buffer;
};

#endif