
#ifndef qgvertexbuffer_h
#define qgvertexbuffer_h

#include <Render/QGVertexAttributeList.h>

class QUEST_API QGVertexBuffer : public QGObject {
public:
	QGVertexBuffer() {
		m_attribs = 0;
		m_count = 0;
		m_data = 0;
	}
	~QGVertexBuffer() = default;

	/**
	 * Create a new buffer
	 */
	virtual void Create(QGVertexAttributeList* type, int count, float* data, bool dynamic);

	/**
	 * Bind / use this buffer
	 */
	virtual void Bind() {} 

	/**
	 * Get the data stored
	 */
	virtual float* Data() { return m_data; }

	/**
	 * Count of stored vertices
	 */
	unsigned int Count() { return m_count; }

	/**
	 * Get vertex attribute list
	 */
	QGVertexAttributeList* VertexAttributeList() { return m_attribs; }

protected:
	// Vertex attributes
	QGVertexAttributeList* m_attribs;

	// Count of vertices
	unsigned int m_count;

	// Stored vertex data
	float* m_data;
};

#endif