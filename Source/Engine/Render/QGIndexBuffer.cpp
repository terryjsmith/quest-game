
#include <Render/QGIndexBuffer.h>

void QGIndexBuffer::Create(int count, unsigned int* data) {
    m_data = (unsigned int*)malloc(sizeof(unsigned int) * count);
    memcpy(m_data, data, sizeof(unsigned int) * count);
    m_count = count;
}
