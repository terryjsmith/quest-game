
#ifndef qgindexbuffer_h
#define qgindexbuffer_h

#include <Core/QGObject.h>

class QUEST_API QGIndexBuffer : public QGObject {
public:
	QGIndexBuffer() {
		m_count = 0;
		m_data = 0;
	}
	~QGIndexBuffer() = default;

	/**
	 * Create a new buffer
	 */
	virtual void Create(int count, unsigned int* data);

	/**
	 * Bind / use this buffer
	 */
	virtual void Bind() {}

	/**
	 * Get the data stored
	 */
	virtual unsigned int* Data() { return m_data; }

	/**
	 * Count of stored vertices
	 */
	unsigned int Count() { return m_count; }

protected:
	// Count of vertices
	unsigned int m_count;

	// Stored vertex data
	unsigned int* m_data;
};

#endif