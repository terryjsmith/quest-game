
#include <Render/QGVertexBuffer.h>

void QGVertexBuffer::Create(QGVertexAttributeList* type, int count, float* data, bool dynamic) {
    m_attribs = type;
    m_data = (float*)malloc(sizeof(float) * type->GetVertexSize() * count);
    memcpy(m_data, data, sizeof(float) * type->GetVertexSize() * count);
    m_count = count;
}