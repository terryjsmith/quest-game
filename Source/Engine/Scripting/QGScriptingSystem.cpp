
#include <Scripting/QGScriptingSystem.h>
#include <Scripting/QGScriptMethods.h>
#include <Core/QGVariant.h>
#include <Scripting/QGScriptComponent.h>
#include <Core/QGWorld.h>
#include <Core/QGApplication.h>
#include <IO/QGInputCommand.h>
#include <Core/QGMetaSystem.h>
#include <Scripting/QGMonoComponent.h>

void QGScriptingSystem::Initialize() {
	// If this is our first Mono library, create domain
	if (m_monoDomain == 0) {
		std::string libDir = "Resources\\mono\\lib";
		std::string configDir = "Resources\\mono\\etc";
		mono_set_dirs(libDir.c_str(), configDir.c_str());
		mono_config_parse(NULL);

		m_monoDomain = mono_jit_init("quest-engine");
	}

	// Load our library
	LoadScriptLibrary("QGEngine.dll");

	// Register callbacks
	qgscripting_register_monomethods();

	// Subscribe to messages to pass on
	QGEventSystem* eventSystem = GetQGSystem<QGEventSystem>();
	eventSystem->Subscribe<QGInputCommand>(InputCommandCallback);
}

void QGScriptingSystem::Update(float delta) {
	std::vector<QGScriptComponent*> components = QGWorld::GetInstance()->GetComponents<QGScriptComponent>();
	for (auto it = components.begin(); it != components.end(); it++) {
		(*it)->Update(delta);
	}
}

void QGScriptingSystem::LoadScriptLibrary(std::string filename) {
	// Load assembly
	MonoAssembly* assembly = mono_domain_assembly_open(m_monoDomain, filename.c_str());
	MonoImage* image = mono_assembly_get_image(assembly);

	// If this is the first library, it should be ours
	if (m_cachedTypes.empty()) {
		m_cachedTypes[QGVariant::VAR_BOOL] = mono_get_boolean_class();
		m_cachedTypes[QGVariant::VAR_INT32] = mono_get_int32_class();
		m_cachedTypes[QGVariant::VAR_INT64] = mono_get_int64_class();
		m_cachedTypes[QGVariant::VAR_UINT32] = mono_get_uint32_class();
		m_cachedTypes[QGVariant::VAR_UINT64] = mono_get_uint64_class();
		m_cachedTypes[QGVariant::VAR_FLOAT] = mono_get_double_class();
		m_cachedTypes[QGVariant::VAR_STRING] = mono_get_string_class();
		m_cachedTypes[QGVariant::VAR_VECTOR2] = mono_class_from_name(image, "QGEngine", "Vector2");
		m_cachedTypes[QGVariant::VAR_VECTOR3] = mono_class_from_name(image, "QGEngine", "Vector3");
		m_cachedTypes[QGVariant::VAR_VECTOR4] = mono_class_from_name(image, "QGEngine", "Vector4");
		m_cachedTypes[QGVariant::VAR_QUATERNION] = mono_class_from_name(image, "QGEngine", "Quaternion");
	}

	// Load classes
	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

	int rows = mono_table_info_get_rows(table_info);
	for (int i = 0; i < rows; i++) {
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(image, name_space, name);

		if (strcmp(name, "<Module>") == 0) {
			continue;
		}

		// Skip sub-classes
		if (_class == 0) continue;

		// Get QGScript class
		MonoClass* scriptClass = 0;
		auto sit = m_monoClasses.find("QGScript");
		if (sit != m_monoClasses.end()) {
			scriptClass = sit->second;
		}

		if (scriptClass == 0) {
			// Try to load from current library
			scriptClass = mono_class_from_name(image, name_space, "QGScript");
			m_monoClasses["QGScript"] = scriptClass;
		}

		QGASSERT(scriptClass != 0, "Unable to find script base class.");

		// Check whether this class inherits from UScript
		if (mono_class_is_subclass_of(_class, scriptClass, false)) {
			// Ignore the QGScript class itself
			if (_class == scriptClass) continue;

			// If so, need to store specific function references
			QGScript* script = new QGScript();
			script->className = name;

			// Find functions - Initialize
			MonoMethod* method = mono_class_get_method_from_name(_class, "Initialize", 0);
			if (method) {
				script->initFunc = (QGMonoScriptInitFunction)mono_method_get_unmanaged_thunk(method);
			}

			// Update
			method = mono_class_get_method_from_name(_class, "Update", 1);
			if (method) {
				script->updateFunc = (QGMonoScriptUpdateFunction)mono_method_get_unmanaged_thunk(method);
			}

			// FixedUpdate
			method = mono_class_get_method_from_name(_class, "FixedUpdate", 1);
			if (method) {
				script->fixedUpdateFunc = (QGMonoScriptFixedUpdateFunction)mono_method_get_unmanaged_thunk(method);
			}

			// Destroy
			method = mono_class_get_method_from_name(_class, "Destroy", 0);
			if (method) {
				script->destroyFunc = (QGMonoScriptDestroyFunction)mono_method_get_unmanaged_thunk(method);
			}

			// Find public variables
			void* iter = NULL;
			MonoClassField* field = mono_class_get_fields(_class, &iter);
			while (field != NULL) {
				std::string fieldName = mono_field_get_name(field);
				MonoCustomAttrInfo* info = mono_custom_attrs_from_field(_class, field);

				// If info is null, no attributes
				if (info == 0) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// Check for custom attribute
				bool serializable = false;
				for (int i = 0; i < info->num_attrs; i++) {
					MonoMethod* attrmethod = info->attrs[i].ctor;
					std::string attrName = mono_class_get_name(mono_method_get_class(attrmethod));
					if (attrName == "SerializeField") serializable = true;
				}

				if (serializable == false) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// Check field accessibility
				uint32_t flags = mono_field_get_flags(field) & MONO_FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ((flags & MONO_FIELD_ATTRIBUTE_PUBLIC) == false) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// For public fields, map type
				MonoType* fieldType = mono_field_get_type(field);
				int enumType = mono_type_get_type(fieldType);
				int variantType = 0;
				switch (enumType) {
				case MONO_TYPE_STRING:
				case MONO_TYPE_CHAR:
					variantType = QGVariant::VAR_STRING;
					break;
				case MONO_TYPE_CLASS:
					variantType = QGVariant::VAR_OBJECT;
					break;
				case MONO_TYPE_BOOLEAN:
					variantType = QGVariant::VAR_BOOL;
					break;
				case MONO_TYPE_I4:
					variantType = QGVariant::VAR_UINT32;
					break;
				case MONO_TYPE_U4:
					variantType = QGVariant::VAR_INT32;
					break;
				case MONO_TYPE_I8:
					variantType = QGVariant::VAR_INT64;
					break;
				case MONO_TYPE_U8:
					variantType = QGVariant::VAR_UINT64;
					break;
				case MONO_TYPE_R4:
				case MONO_TYPE_R8:
					variantType = QGVariant::VAR_FLOAT;
					break;
				default:
					break;
				}

				QGASSERT(variantType != 0, "Unable to map variant type.");

				script->vars[fieldName] = variantType;
				field = mono_class_get_fields(_class, &iter);
			}

			// Add script type
			m_monoScripts[name] = script;
		}

		// Do the same for components
		MonoClass* componentClass = 0;
		auto cit = m_monoClasses.find("QGComponent");
		if (cit != m_monoClasses.end()) {
			componentClass = cit->second;
		}

		if (componentClass == 0) {
			// Try to load from current library
			componentClass = mono_class_from_name(image, name_space, "QGComponent");
			m_monoClasses["QGComponent"] = componentClass;
		}

		QGASSERT(componentClass != 0, "Unable to find component base class.");

		// Check whether this class inherits from UScript
		QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
		if (mono_class_is_subclass_of(_class, componentClass, false)) {
			// Ignore the QGComponent class itself
			if (_class == componentClass) continue;

			// If so, need to store specific function references
			QGMonoComponentType* component = new QGMonoComponentType();
			component->className = name;

			// Find functions - Initialize
			MonoMethod* method = mono_class_get_method_from_name(_class, "Serialize", 0);
			if (method) {
				component->serializeFunc = (QGMonoComponentSerializeFunction)mono_method_get_unmanaged_thunk(method);
			}

			// Find functions - Initialize
			method = mono_class_get_method_from_name(_class, "Deserialize", 0);
			if (method) {
				component->deserializeFunc = (QGMonoComponentDeserializeFunction)mono_method_get_unmanaged_thunk(method);
			}

			// Find public variables
			void* iter = NULL;
			MonoClassField* field = mono_class_get_fields(_class, &iter);
			while (field != NULL) {
				std::string fieldName = mono_field_get_name(field);
				MonoCustomAttrInfo* info = mono_custom_attrs_from_field(_class, field);

				// If info is null, no attributes
				if (info == 0) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// Check for custom attribute
				bool serializable = false;
				for (int i = 0; i < info->num_attrs; i++) {
					MonoMethod* attrmethod = info->attrs[i].ctor;
					std::string attrName = mono_class_get_name(mono_method_get_class(attrmethod));
					if (attrName == "SerializeField") serializable = true;
				}

				if (serializable == false) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// Check field accessibility
				uint32_t flags = mono_field_get_flags(field) & MONO_FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ((flags & MONO_FIELD_ATTRIBUTE_PUBLIC) == false) {
					field = mono_class_get_fields(_class, &iter);
					continue;
				}

				// For public fields, map type
				MonoType* fieldType = mono_field_get_type(field);
				int enumType = mono_type_get_type(fieldType);
				int variantType = 0;
				switch (enumType) {
				case MONO_TYPE_STRING:
				case MONO_TYPE_CHAR:
					variantType = QGVariant::VAR_STRING;
					break;
				case MONO_TYPE_CLASS:
					variantType = QGVariant::VAR_OBJECT;
					break;
				case MONO_TYPE_BOOLEAN:
					variantType = QGVariant::VAR_BOOL;
					break;
				case MONO_TYPE_I4:
					variantType = QGVariant::VAR_UINT32;
					break;
				case MONO_TYPE_U4:
					variantType = QGVariant::VAR_INT32;
					break;
				case MONO_TYPE_I8:
					variantType = QGVariant::VAR_INT64;
					break;
				case MONO_TYPE_U8:
					variantType = QGVariant::VAR_UINT64;
					break;
				case MONO_TYPE_R4:
				case MONO_TYPE_R8:
					variantType = QGVariant::VAR_FLOAT;
					break;
				default:
					break;
				}

				QGASSERT(variantType != 0, "Unable to map variant type.");

				component->vars[fieldName] = variantType;
				field = mono_class_get_fields(_class, &iter);
			}

			m_monoComponentTypes[name] = component;

			// Also need to register it with meta system
			metaSystem->RegisterType<QGMonoComponent>(5000 + m_monoComponentTypes.size(), name);
		}

		// Add to list
		m_monoClasses[name] = _class;
	}
}

QGScript* QGScriptingSystem::GetScript(std::string className) {
	auto it = m_monoScripts.find(className);
	if (it != m_monoScripts.end()) {
		return(it->second);
	}

	return(0);
}

QGMonoComponentType* QGScriptingSystem::GetComponentType(std::string className) {
	auto it = m_monoComponentTypes.find(className);
	if (it != m_monoComponentTypes.end()) {
		return(it->second);
	}

	return(0);
}

QGScriptObject* QGScriptingSystem::GetRemoteObject(std::string className, QGObject* localObject) {
	// Check cache
	auto it = m_objectsFromLocal.find(localObject);
	if (it != m_objectsFromLocal.end()) {
		return(it->second);
	}

	// Otherwise, create an object, start by finding class type
	auto ci = m_monoClasses.find(className);
	QGASSERT(ci != m_monoClasses.end(), "Unable to find class type.");

	// Create a new object
	MonoObject* newMonoObject = mono_object_new(m_monoDomain, ci->second);
	uint32_t handle = mono_gchandle_new(newMonoObject, true);

	// Create a new relationship between local and remote
	QGScriptObject* monoObject = new QGScriptObject();
	monoObject->localObj = localObject;
	monoObject->remoteObj = mono_gchandle_get_target(handle);

	// Store in both directions
	m_objectsFromLocal[localObject] = monoObject;

	// Call mono constructor
	mono_runtime_object_init(newMonoObject);

	return(monoObject);
}

QGScriptObject* QGScriptingSystem::GetLocalObject(void* object) {
	// Convert
	MonoObject* obj = (MonoObject*)object;

	// Get the ptr value
	MonoClass* cl = mono_object_get_class(obj);
//	MonoProperty* prop = mono_class_get_property_from_name(cl, "ptr");
	MonoClassField* fl = mono_class_get_field_from_name(cl, "ptr");

	intptr_t value;
	mono_field_get_value(obj, fl, &value);
	QGObject* ptr = (QGObject*)value;

	// Check cache
	auto it = m_objectsFromLocal.find(ptr);
	if (it != m_objectsFromLocal.end()) {
		return(it->second);
	}

	// Otherwise, need to create a new object - do we want to do this?
	return(0);
}

QGVariant QGScriptingSystem::MonoObjectToVariant(MonoObject* object) {
	MonoClass* _class = mono_object_get_class(object);

	if (_class == m_cachedTypes[QGVariant::VAR_BOOL]) {
		bool value = *(bool*)mono_object_unbox(object);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_INT32]) {
		int32_t value = *(int32_t*)mono_object_unbox(object);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_INT64]) {
		int64_t value = *(int64_t*)mono_object_unbox(object);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_UINT32]) {
		uint32_t value = *(uint32_t*)mono_object_unbox(object);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_UINT64]) {
		uint64_t value = *(uint64_t*)mono_object_unbox(object);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_UINT64]) {
		double value = *(double*)mono_object_unbox(object);
		return((float)value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_STRING]) {
		MonoString* str = (MonoString*)object;
		char* value = mono_string_to_utf8(str);
		return(value);
	}

	if (_class == m_cachedTypes[QGVariant::VAR_VECTOR2]) {
		float x, y;

		MonoClassField* field = mono_class_get_field_from_name(_class, "x");
		mono_field_get_value(object, field, &x);

		field = mono_class_get_field_from_name(_class, "y");
		mono_field_get_value(object, field, &y);

		return(vector2(x, y));
	}

	if (_class == m_cachedTypes[QGVariant::VAR_VECTOR3]) {
		float x, y, z;

		MonoClassField* field = mono_class_get_field_from_name(_class, "x");
		mono_field_get_value(object, field, &x);

		field = mono_class_get_field_from_name(_class, "y");
		mono_field_get_value(object, field, &y);

		field = mono_class_get_field_from_name(_class, "z");
		mono_field_get_value(object, field, &z);

		return(vector3(x, y, z));
	}

	if (_class == m_cachedTypes[QGVariant::VAR_VECTOR4] || _class == m_cachedTypes[QGVariant::VAR_QUATERNION]) {
		float x, y, z, w;

		MonoClassField* field = mono_class_get_field_from_name(_class, "x");
		mono_field_get_value(object, field, &x);

		field = mono_class_get_field_from_name(_class, "y");
		mono_field_get_value(object, field, &y);

		field = mono_class_get_field_from_name(_class, "z");
		mono_field_get_value(object, field, &z);

		field = mono_class_get_field_from_name(_class, "w");
		mono_field_get_value(object, field, &w);

		if (_class == m_cachedTypes[QGVariant::VAR_VECTOR4])
			return(vector4(x, y, z, w));

		return(quaternion(w, x, y, z));
	}

	QGASSERT(false, "Unable to determine variant type.");
	return(0);
}

MonoObject* QGScriptingSystem::VariantToMonoObject(QGVariant var, std::string classHint) {
	MonoObject* mobj = 0;

	if (var.IsInt()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_INT32];
		int iv = var.AsInt();
		mobj = mono_value_box(mono_domain_get(), cl, &iv);
	}

	if (var.IsBool()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_BOOL];
		bool bv = var.AsBool();
		mobj = mono_value_box(mono_domain_get(), cl, &bv);
	}

	if (var.IsUInt()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_UINT32];
		unsigned int iv = var.AsUInt();
		mobj = mono_value_box(mono_domain_get(), cl, &iv);
	}

	if (var.IsFloat()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_FLOAT];
		double dv = var.AsFloat();
		mobj = mono_value_box(mono_domain_get(), cl, &dv);
	}

	if (var.IsString()) {
		mobj = (MonoObject*)mono_string_new(mono_domain_get(), var.AsString().c_str());
	}

	if (var.IsObject()) {
		QGScriptObject* monoObject = (QGScriptObject*)this->GetRemoteObject(classHint, var.AsObject());
		mobj = monoObject->remoteObj;
	}

	if (var.IsVector2()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_VECTOR2];
		mobj = mono_object_new(mono_domain_get(), cl);
		mono_runtime_object_init(mobj);
		vector2 vec = var.AsVector2();

		MonoClassField* field = mono_class_get_field_from_name(cl, "x");
		mono_field_set_value(mobj, field, &vec.x);

		field = mono_class_get_field_from_name(cl, "y");
		mono_field_set_value(mobj, field, &vec.y);
	}

	if (var.IsVector3()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_VECTOR3];
		mobj = mono_object_new(mono_domain_get(), cl);
		mono_runtime_object_init(mobj);
		vector3 vec = var.AsVector3();

		MonoClassField* field = mono_class_get_field_from_name(cl, "x");
		mono_field_set_value(mobj, field, &vec.x);

		field = mono_class_get_field_from_name(cl, "y");
		mono_field_set_value(mobj, field, &vec.y);

		field = mono_class_get_field_from_name(cl, "z");
		mono_field_set_value(mobj, field, &vec.z);
	}

	if (var.IsVector4()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_VECTOR4];
		mobj = mono_object_new(mono_domain_get(), cl);
		mono_runtime_object_init(mobj);
		vector4 vec = var.AsVector4();

		MonoClassField* field = mono_class_get_field_from_name(cl, "x");
		mono_field_set_value(mobj, field, &vec.x);

		field = mono_class_get_field_from_name(cl, "y");
		mono_field_set_value(mobj, field, &vec.y);

		field = mono_class_get_field_from_name(cl, "z");
		mono_field_set_value(mobj, field, &vec.z);

		field = mono_class_get_field_from_name(cl, "w");
		mono_field_set_value(mobj, field, &vec.w);
	}

	if (var.IsQuaternion()) {
		MonoClass* cl = m_cachedTypes[QGVariant::VAR_QUATERNION];
		mobj = mono_object_new(mono_domain_get(), cl);
		mono_runtime_object_init(mobj);
		quaternion quat = var.AsQuaternion();

		MonoClassField* field = mono_class_get_field_from_name(cl, "x");
		mono_field_set_value(mobj, field, &quat.x);

		field = mono_class_get_field_from_name(cl, "y");
		mono_field_set_value(mobj, field, &quat.y);

		field = mono_class_get_field_from_name(cl, "z");
		mono_field_set_value(mobj, field, &quat.z);

		field = mono_class_get_field_from_name(cl, "w");
		mono_field_set_value(mobj, field, &quat.w);
	}

	return(mobj);
}

MonoArray* QGScriptingSystem::VariantListToMonoArray(std::vector<QGVariant> arr, std::string classHint) {
	// Get class image
	auto it = m_monoClasses.find(classHint);
	QGASSERT(it != m_monoClasses.end(), "Cannot find class type.");

	MonoArray* monoarr = mono_array_new(m_monoDomain, it->second, arr.size());
	for (int i = 0; i < arr.size(); i++) {
		mono_array_setref(monoarr, i, VariantToMonoObject(arr[i], it->first));
	}

	return(monoarr);
}

MonoArray* QGScriptingSystem::ObjectListToMonoArray(std::map<QGObject*, std::string> arr, std::string classHint) {
	// Get class image
	auto it = m_monoClasses.find(classHint);
	QGASSERT(it != m_monoClasses.end(), "Cannot find base class type.");

	MonoArray* monoarr = mono_array_new(m_monoDomain, it->second, arr.size());
	int index = 0;
	for (auto oit = arr.begin(); oit != arr.end(); oit++) {
		mono_array_setref(monoarr, index, VariantToMonoObject(oit->first, oit->second));
		index++;
	}

	return(monoarr);
}

void QGScriptingSystem::InputCommandCallback(QGEvent* ev, QGObject* obj) {
	QGScriptingSystem* scriptingSystem = GetQGSystem<QGScriptingSystem>();
	if (scriptingSystem->m_eventPublish == 0) {
		MonoClass* _class = scriptingSystem->m_monoClasses["QGEventSystem"];
		MonoMethod* method = mono_class_get_method_from_name(_class, "Publish", 2);
		scriptingSystem->m_eventPublish = (QGMonoEventHandlerFunction)mono_method_get_unmanaged_thunk(method);
	}

	// Get the command object
	QGInputCommand* command = dynamic_cast<QGInputCommand*>(ev);
	QGScriptObject* evobj = scriptingSystem->GetRemoteObject("QGInputCommand", command);
	QGScriptObject* enobj = 0;
	if(obj) enobj = scriptingSystem->GetRemoteObject("QGEntity", obj);

	MonoException* ex = NULL;
	scriptingSystem->m_eventPublish(evobj->remoteObj, (obj == 0) ? NULL : enobj->remoteObj, &ex);
	QGASSERT(ex == NULL, "Exception in event handler.");
}

QGObject* QGScriptingSystem::internal_GigaObject_Ctor(MonoObject* obj) {
	QGScriptingSystem* scriptingSystem = GetQGSystem<QGScriptingSystem>();

	// Search first
	QGObject* newobj = 0;
	auto oi = scriptingSystem->m_objectsFromLocal.begin();
	for (; oi != scriptingSystem->m_objectsFromLocal.end(); oi++) {
		if (oi->second->remoteObj == obj) {
			newobj = oi->second->localObj;
			break;
		}
	}

	if (newobj) {
		return(newobj);
	}

	// Figure out what type it is
	MonoClass* _class = mono_object_get_class(obj);

	// Process inheritance to find top-level native C++ class
	QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
	std::string className = mono_class_get_name(_class);

	// Create a new script object
	newobj = metaSystem->CreateObject(className);

	QGScriptObject* cobj = new QGScriptObject();
	cobj->localObj = newobj;
	cobj->remoteObj = obj;

	scriptingSystem->m_objectsFromLocal[cobj] = cobj;

	return(newobj);
}