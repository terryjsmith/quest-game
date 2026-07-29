
#include <Scripting/QGScriptMethods.h>
#include <Scripting/QGScriptingSystem.h>
#include <Core/QGApplication.h>
#include <Core/QGEntity.h>
#include <Core/QGEventSystem.h>
#include <IO/QGInputCommand.h>

void qgscripting_register_monomethods() {
	// Tie our constructor for new objects
	mono_add_internal_call("QGEngine.QGObject::internal_QGObject_Ctor", QGScriptingSystem::internal_GigaObject_Ctor);

	// UComponent
	mono_add_internal_call("QGEngine.QGComponent::get_gameObject", uscript_ucomponent_gameobject_get);

	// QGEntity
	mono_add_internal_call("QGEngine.QGEntity::get_components", uscript_uentity_components_get);
	mono_add_internal_call("QGEngine.QGEntity::get_id", uscript_uentity_id_get);
	mono_add_internal_call("QGEngine.QGEntity::set_id", uscript_uentity_id_set);
	mono_add_internal_call("QGEngine.QGEntity::get_name", uscript_uentity_name_get);
	mono_add_internal_call("QGEngine.QGEntity::set_name", uscript_uentity_name_set);
	mono_add_internal_call("QGEngine.QGEntity::get_transform", uscript_uentity_transform_get);

	// QGTransform
	mono_add_internal_call("QGEngine.QGTransform::Move", uscript_utransform_move);
	mono_add_internal_call("QGEngine.QGTransform::Rotate", uscript_utransform_rotate);
	mono_add_internal_call("QGEngine.QGTransform::Scale", uscript_utransform_scale);
	mono_add_internal_call("QGEngine.QGTransform::get_forward", uscript_utransform_forward_get);
	mono_add_internal_call("QGEngine.QGTransform::get_right", uscript_utransform_right_get);
	mono_add_internal_call("QGEngine.QGTransform::get_up", uscript_utransform_up_get);
	mono_add_internal_call("QGEngine.QGTransform::get_position", uscript_utransform_position_get);
	mono_add_internal_call("QGEngine.QGTransform::set_position", uscript_utransform_position_set);
	mono_add_internal_call("QGEngine.QGTransform::get_rotation", uscript_utransform_rotation_get);
	mono_add_internal_call("QGEngine.QGTransform::set_rotation", uscript_utransform_rotation_set);
	mono_add_internal_call("QGEngine.QGTransform::get_scaling", uscript_utransform_scaling_get);
	mono_add_internal_call("QGEngine.QGTransform::set_scaling", uscript_utransform_scaling_set);

	// QGInputCommand
	mono_add_internal_call("QGEngine.QGInputCommand::get_command", uscript_uinputcommand_command_get);
	mono_add_internal_call("QGEngine.QGInputCommand::get_type", uscript_uinputcommand_type_get);
	mono_add_internal_call("QGEngine.QGInputCommand::get_state", uscript_uinputcommand_state_get);
}

/**
 * Helper functions
 */
template<class T> T* UMonoMethods_GetLocalObject(MonoObject* obj) {
	// Get our local object
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	QGScriptObject* monoObject = scriptSystem->GetLocalObject(obj);

	// Cast it to the right type
	T* localObj = (T*)monoObject->localObj;
	return(localObj);
}

MonoObject* UMonoMethods_GetRemoteObject(std::string className, QGObject* localObject) {
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	QGScriptObject* umonoobj = scriptSystem->GetRemoteObject(className, localObject);
	return(umonoobj->remoteObj);
}

QGVariant UMonoMethods_MonoObjectToVariant(MonoObject* obj) {
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	return(scriptSystem->MonoObjectToVariant(obj));
}

MonoObject* UMonoMethods_VariantToMonoObject(QGVariant variant, std::string classHint = "") {
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();
	return(scriptSystem->VariantToMonoObject(variant, classHint));
}

/**
 * UComponent
 */

MonoObject* uscript_ucomponent_gameobject_get(MonoObject* obj) {
	// Get our local object
	QGComponent* component = UMonoMethods_GetLocalObject<QGComponent>(obj);

	// Get QGEntity
	return(UMonoMethods_GetRemoteObject("QGEntity", component->Entity()));
}

/**
 * QGEntity
 */

MonoArray* uscript_uentity_components_get(MonoObject* obj) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	QGScriptingSystem* scriptSystem = GetQGSystem<QGScriptingSystem>();

	std::vector<QGComponent*> components = entity->GetComponents();
	std::vector<QGVariant> list;
	for (auto it = components.begin(); it != components.end(); it++) {
		list.push_back(*it);
	}

	return(scriptSystem->VariantListToMonoArray(list, "QGComponent"));
}

uint32_t uscript_uentity_id_get(MonoObject* obj) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	return(entity->id);
}

void uscript_uentity_id_set(MonoObject* obj, uint32_t value) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	entity->id = value;
}

MonoString* uscript_uentity_name_get(MonoObject* obj) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	return((MonoString*)UMonoMethods_VariantToMonoObject(entity->name));
}

void uscript_uentity_name_set(MonoObject* obj, MonoString* value) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	entity->name = mono_string_to_utf8(value);
}

MonoObject* uscript_uentity_transform_get(MonoObject* obj) {
	// Get our local object
	QGEntity* entity = UMonoMethods_GetLocalObject<QGEntity>(obj);
	return(UMonoMethods_GetRemoteObject("QGTransform", &entity->transform));
}

/**
 * QGTransform
 */

void uscript_utransform_move(MonoObject* obj, MonoObject* amount) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);

	// Get X, Y, and Z values
	vector3 value = UMonoMethods_MonoObjectToVariant(amount).AsVector3();

	// Do the move
	transform->Move(value);
}

void uscript_utransform_rotate(MonoObject* obj, MonoObject* axis, float degrees) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);

	// Get X, Y, and Z values
	vector3 vecaxis = UMonoMethods_MonoObjectToVariant(axis).AsVector3();

	// Do the move
	transform->Rotate(vecaxis, degrees);
}

void uscript_utransform_scale(MonoObject* obj, MonoObject* amount) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);

	// Get X, Y, and Z values
	vector3 value = UMonoMethods_MonoObjectToVariant(amount).AsVector3();

	// Do the move
	transform->Scale(value);
}

MonoObject* uscript_utransform_forward_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->Forward()));
}

MonoObject* uscript_utransform_right_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->Right()));
}

MonoObject* uscript_utransform_up_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->Up()));
}

MonoObject* uscript_utransform_position_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->position));
}

void uscript_utransform_position_set(MonoObject* obj, MonoObject* position) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	vector3 pos = UMonoMethods_MonoObjectToVariant(position).AsVector3();
	transform->position = pos;
}

MonoObject* uscript_utransform_rotation_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->rotation));
}

void uscript_utransform_rotation_set(MonoObject* obj, MonoObject* rotation) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	quaternion rot = UMonoMethods_MonoObjectToVariant(rotation).AsQuaternion();
	transform->rotation = rot;
}

void uscript_utransform_scaling_set(MonoObject* obj, MonoObject* scaling) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	vector3 scale = UMonoMethods_MonoObjectToVariant(scaling).AsVector3();
	transform->scaling = scale;
}

MonoObject* uscript_utransform_scaling_get(MonoObject* obj) {
	// Cast it to the right type
	QGTransform* transform = UMonoMethods_GetLocalObject<QGTransform>(obj);
	return(UMonoMethods_VariantToMonoObject(transform->scaling));
}

/**
 * QGInputCommand
 */
MonoString* uscript_uinputcommand_command_get(MonoObject* obj) {
	QGInputCommand* command = UMonoMethods_GetLocalObject<QGInputCommand>(obj);
	return((MonoString*)UMonoMethods_VariantToMonoObject(command->command));
}

uint32_t uscript_uinputcommand_type_get(MonoObject* obj) {
	QGInputCommand* command = UMonoMethods_GetLocalObject<QGInputCommand>(obj);
	return(command->type);
}

float uscript_uinputcommand_state_get(MonoObject* obj) {
	QGInputCommand* command = UMonoMethods_GetLocalObject<QGInputCommand>(obj);
	return(command->state);
}