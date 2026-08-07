
#ifndef qgscriptmethods_h
#define qgscriptmethods_h

#include <Scripting/QGMonoIncludes.h>

void qgscripting_register_monomethods();

// UComponent
MonoObject* uscript_ucomponent_gameobject_get(MonoObject* obj);

// UEntity
MonoArray* uscript_uentity_components_get(MonoObject* obj);
uint32_t uscript_uentity_id_get(MonoObject* obj);
void uscript_uentity_id_set(MonoObject* obj, uint32_t value);
MonoString* uscript_uentity_name_get(MonoObject* obj);
void uscript_uentity_name_set(MonoObject* obj, MonoString* value);
MonoObject* uscript_uentity_transform_get(MonoObject* obj);

// UTransform
void uscript_utransform_move(MonoObject* obj, MonoObject* amount);
void uscript_utransform_rotate(MonoObject* obj, MonoObject* axis, float degrees);
void uscript_utransform_scale(MonoObject* obj, MonoObject* amount);
MonoObject* uscript_utransform_forward_get(MonoObject* obj);
MonoObject* uscript_utransform_right_get(MonoObject* obj);
MonoObject* uscript_utransform_up_get(MonoObject* obj);
MonoObject* uscript_utransform_position_get(MonoObject* obj);
MonoObject* uscript_utransform_rotation_get(MonoObject* obj);
MonoObject* uscript_utransform_scaling_get(MonoObject* obj);
void uscript_utransform_position_set(MonoObject* obj, MonoObject* position);
void uscript_utransform_rotation_set(MonoObject* obj, MonoObject* rotation);
void uscript_utransform_scaling_set(MonoObject* obj, MonoObject* scaling);

// QGInputCommand
MonoString* uscript_uinputcommand_command_get(MonoObject* obj);
uint32_t uscript_uinputcommand_type_get(MonoObject* obj);
float uscript_uinputcommand_state_get(MonoObject* obj);

// QGCameraComponent
MonoObject* uscript_ucameracomponent_transform_get(MonoObject* obj);
float uscript_ucameracomponent_fov_get(MonoObject* obj);
void uscript_ucameracomponent_fov_set(MonoObject* obj, float fov);

#endif