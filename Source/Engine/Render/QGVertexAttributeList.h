
#ifndef qgvertexattributelist_h
#define qgvertexattributelist_h

#include <Core/QGObject.h>

enum QUEST_API QGVertexAttribute {
	ATTRIB_POSITION = 1,
	ATTRIB_COLOR = 1 << 1,
	ATTRIB_NORMAL = 1 << 2,
	ATTRIB_TEXCOORD0 = 1 << 3,
	ATTRIB_TEXCOORD1 = 1 << 4,
	ATTRIB_BONES = 1 << 5,
	ATTRIB_BONEWEIGHTS = 1 << 6,
};

class QUEST_API QGVertexAttributeList {
public:
	QGVertexAttributeList() : m_vertexSize(0), m_attribBitmask(0) {}
	virtual ~QGVertexAttributeList() = default;

	/**
	 * Add/remove/check for vertex attribute
	 */
	void AddVertexAttribute(int attrib, int offset, int size);
	void RemoveVertexAttribute(int attrib);
	bool HasVertexAttribute(int attrib);

	int GetVertexSize() { return m_vertexSize; }

	/**
	 * Enable a particular vertex attribute (if this vertex type has it)
	 */
	virtual void EnableAttribute(int index, int attrib) {}

	/**
	 * Bind/unbind this vertex type
	 */
	virtual void Bind() {}

	// Internal vertex attribute layout
	struct VertexAttrib {
		int attrib;
		int size;
		int offset;
	};

protected:
	int m_vertexSize;
	int m_attribBitmask;
	std::map<int, VertexAttrib> m_attribs;
};

#endif