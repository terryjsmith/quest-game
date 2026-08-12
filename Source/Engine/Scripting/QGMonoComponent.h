
#ifndef qgmonocomponent_h
#define qgmonocomponent_h

#include <Core/QGComponent.h>
#include <Scripting/QGMonoIncludes.h>
#include <Scripting/QGMonoComponentType.h>
#include <Scripting/QGScriptObject.h>

class QUEST_API QGMonoComponent : public QGComponent {
public:
	QGMonoComponent() : m_object(0), classType(0) {}
	~QGMonoComponent() = default;

	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

public:
	// Class type
	QGMonoComponentType* classType;

protected:
	// Reference to object we are attached to
	QGScriptObject* m_object;

private:
	void Initialize();
};

#endif