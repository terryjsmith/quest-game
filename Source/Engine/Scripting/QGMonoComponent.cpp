
#include <Scripting/QGMonoComponent.h>
#include <Scripting/QGScriptingSystem.h>
#include <Core/QGMetaSystem.h>
#include <Core/QGApplication.h>

void QGMonoComponent::Initialize() {
	QGObjectType* type = this->Type();
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();

	// Constructor also needs to create an equivalent Mono object
	classType = scriptSystem->GetComponentType(type->className);

	// Get or create a remote object
	m_object = (QGScriptObject*)scriptSystem->GetRemoteObject(type->className, this);
}

void QGMonoComponent::Serialize(QGDataRecord* record) {
	// Ensure initialized
	if (m_object == 0) this->Initialize();

	// Tell the object to serialize
	if (classType->serializeFunc) {
		MonoException* ex = NULL;
		classType->serializeFunc(m_object->remoteObj, &ex);
		if (ex != NULL) {
			QGASSERT(true, "Mono component Serialize function error.");
		}
	}

	// TODO: Serialize the variables tagged as such
}

void QGMonoComponent::Deserialize(QGDataRecord* record) {
	// Ensure initialized
	if (m_object == 0) this->Initialize();

	// TODO: Deserialize into object

	// Call deserialize function
	if (classType->deserializeFunc) {
		MonoException* ex = NULL;
		classType->deserializeFunc(m_object->remoteObj, &ex);
		if (ex != NULL) {
			QGASSERT(true, "Mono component Deserialize function error.");
		}
	}
}