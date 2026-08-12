
#ifndef qgobject_h
#define qgobject_h

#include <quest-engine.h>

class QUEST_API QGObject;
class QUEST_API QGMetaSystem;
class QUEST_API QGObjectType;

// Internal function to call a C++ player constructor
typedef QGObject* (*QGObjectCreateFunc)();

// Registered type info
class QUEST_API QGObjectType {
public:
	QGObjectType() : typeID(0), type(typeid(this)), ctor(0), networkSync(true) {}
	~QGObjectType() = default;

public:
	uint32_t typeID;
	std::type_index type;
	QGObjectCreateFunc ctor;
	std::string className;
	bool networkSync;
};

class QUEST_API QGObject {
public:
	QGObject() : m_type(0) {}
	virtual ~QGObject() = default;

	/**
	 * Return class type info
	 */
	QGObjectType* Type();

	friend class QGMetaSystem;

private:
	QGObjectType* m_type;
};

#endif