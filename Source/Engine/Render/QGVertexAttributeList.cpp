
#include <Render/QGVertexAttributeList.h>

void QGVertexAttributeList::AddVertexAttribute(int attrib, int offset, int size) {
	VertexAttrib a;
	a.attrib = attrib;
	a.offset = offset;
	a.size = size;

	m_attribs[attrib] = a;
	m_vertexSize += size;
	m_attribBitmask |= attrib;
}

void QGVertexAttributeList::RemoveVertexAttribute(int attrib) {
	auto it = m_attribs.find(attrib);
	for (; it != m_attribs.end(); it++) {
		if (it->second.attrib == attrib)
			break;
	}

	if (it == m_attribs.end()) {
		// Error
		return;
	}

	m_vertexSize -= it->second.size;
	m_attribs.erase(it);
	m_attribBitmask &= ~attrib;
}

bool QGVertexAttributeList::HasVertexAttribute(int attrib) {
	return(m_attribBitmask & attrib);
}