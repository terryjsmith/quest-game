
#ifndef qgscriptingsystem_h
#define qgscriptingsystem_h

#include <Scripting/QGScriptObject.h>
#include <Core/QGSystem.h>
#include <Core/QGVariant.h>
#include <Scripting/QGScript.h>
#include <Core/QGEventSystem.h>

typedef void (*QGMonoEventHandlerFunction)(MonoObject*, MonoObject*, MonoException**);

class QUEST_API QGScriptingSystem : public QGSystem {
public:
	QGScriptingSystem() {
		m_monoDomain = 0;
		m_eventPublish = 0;
	}
	~QGScriptingSystem() = default;

	/**
	 * Start mono, load default engine library
	 */
	void Initialize();

	/**
	 * Update, calls update for all objects
	 */
	void Update(float delta);

	/**
	 * Load a game specific library
	 */
	void LoadScriptLibrary(std::string filename);

	/**
	 * Find a script class
	 */
	QGScript* GetScript(std::string className);

	/**
	 * Get or create an object relationship from Mono to local
	 */
	QGScriptObject* GetRemoteObject(std::string className, QGObject* localObject);

	/**
	 * Gets a local object, given a MonoObject
	 */
	QGScriptObject* GetLocalObject(void* remoteObject);

	/**
	 * Convert a MonoObject to a Variant
	 */
	QGVariant MonoObjectToVariant(MonoObject* object);

	/**
	 * Convert a variant to a Mono object
	 */
	MonoObject* VariantToMonoObject(QGVariant variant, std::string classHint = "");

	/**
	 * Handle array case
	 */
	MonoArray* VariantListToMonoArray(std::vector<QGVariant> arr, std::string classHint = "");

	/**
	 * Array of objects
	 */
	MonoArray* ObjectListToMonoArray(std::map<QGObject*, std::string> arr, std::string classHint = "");

public:
	// Callback methods to interface w/ Mono
	static void InputCommandCallback(QGEvent* ev, QGObject* obj);
	static QGObject* internal_GigaObject_Ctor(MonoObject* obj);

protected:
	// Mappings from class types to C# class types
	std::map<std::string, MonoClass*> m_monoClasses;

	// Mono script types
	std::map<std::string, QGScript*> m_monoScripts;

	// Mapping local and remote objects
	std::map<QGObject*, QGScriptObject*> m_objectsFromLocal;

	// Cached built-in Mono types
	std::map<uint32_t, MonoClass*> m_cachedTypes;

	// Cached Mono EventSystem publish function
	QGMonoEventHandlerFunction m_eventPublish;

	// Mono domain
	MonoDomain* m_monoDomain;
};

#endif