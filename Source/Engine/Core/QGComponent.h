
#ifndef component_h
#define component_h

#include <Core/QGObject.h>
#include <Core/QGSerializable.h>

class QUEST_API QGEntity;

class QUEST_API QGComponent : public QGObject, public QGSerializable {
public:
	QGComponent() : m_parent(0) { }
	virtual ~QGComponent() = default;

	QGEntity* Entity() { return m_parent; }

	friend class QGEntity;

protected:
	QGEntity* m_parent;
};

#endif