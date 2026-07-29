
#ifndef qgscriptcomponent_h
#define qgscriptcomponent_h

#include <Core/QGComponent.h>
#include <Scripting/QGScript.h>
#include <Core/QGVariant.h>
#include <Scripting/QGScriptObject.h>

class QUEST_API QGScriptComponent : public QGComponent {
public:
	QGScriptComponent() {
		m_script = 0;
		m_object = 0;
	}
	~QGScriptComponent() = default;

	/**
	* Overridable functions
	*/
	void Initialize(QGScript* script);
	void Update(float delta);
	void FixedUpdate(float delta);
	void Destroy();

	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

protected:
	// Script
	QGScript* m_script;

	// Script values
	std::map<std::string, QGVariant> vars;

	// Reference to object we are attached to
	QGScriptObject* m_object;
};

#endif