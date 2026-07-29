
#include <Scripting/QGScriptComponent.h>
#include <Scripting/QGScriptingSystem.h>
#include <Core/QGApplication.h>

void QGScriptComponent::Initialize(QGScript* script) {
	// Save script
	this->m_script = script;

	// Get or create a remote object
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	m_object = (QGScriptObject*)scriptSystem->GetRemoteObject(script->className, this);

	// Set variables in C# for variables (from variant values)
	auto it = vars.begin();
	for (; it != vars.end(); it++) {
		MonoClass* monoClass = mono_object_get_class(m_object->remoteObj);
		MonoClassField* field = mono_class_get_field_from_name(monoClass, it->first.c_str());
		mono_field_set_value(m_object->remoteObj, field, scriptSystem->VariantToMonoObject(it->second));
	}

	// Call object Initialize function
	QGScript* monoScript = (QGScript*)this->m_script;
	if (monoScript->initFunc) {
		MonoException* ex = NULL;
		monoScript->initFunc(m_object->remoteObj, &ex);
		if (ex) {
			QGASSERT(true, "Mono object Initialize function error.");
		}
	}
}

void QGScriptComponent::Update(float delta) {
	QGScript* monoScript = (QGScript*)this->m_script;

	if (monoScript->updateFunc) {
		MonoException* ex = NULL;
		monoScript->updateFunc(m_object->remoteObj, delta, &ex);
		if (ex != NULL) {
			QGASSERT(true, "Mono object Update function error.");
		}
	}
}

void QGScriptComponent::FixedUpdate(float delta) {
	QGScript* monoScript = (QGScript*)this->m_script;
	if (monoScript->fixedUpdateFunc) {
		MonoException* ex = NULL;
		monoScript->fixedUpdateFunc(m_object->remoteObj, delta, &ex);
		if (ex) {
			QGASSERT(true, "Mono object FixedUpdate function error.");
		}
	}
}

void QGScriptComponent::Destroy() {
	QGScript* monoScript = (QGScript*)this->m_script;
	if (monoScript->destroyFunc) {
		MonoException* ex = NULL;
		monoScript->destroyFunc(m_object->remoteObj, &ex);
		if (ex) {
			QGASSERT(true, "Mono object Destroy function error.");
		}
	}
}

void QGScriptComponent::Serialize(QGDataRecord* record) {
	record->Set("script", m_script->className);
}

void QGScriptComponent::Deserialize(QGDataRecord* record) {
	std::string className = record->Get("script").AsString();
	if (m_script) {
		if (m_script->className != className) {
			QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
			QGScript* script = scriptSystem->GetScript(className);
			this->Initialize(script);
		}
		return;
	}

	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	QGScript* script = scriptSystem->GetScript(className);
	this->Initialize(script);
}