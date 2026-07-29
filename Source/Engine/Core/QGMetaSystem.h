
#ifndef qgmetasystem_h
#define qgmetasystem_h

#include <Core/QGSystem.h>

// Internal function to call a C++ player constructor
typedef QGObject* (*QGObjectCreateFunc)();

// Registered type info
class QGObjectType {
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

class QUEST_API QGMetaSystem : public QGSystem {
public:
	QGMetaSystem() = default;
	~QGMetaSystem() = default;

	/**
	 * Register a new object type
	 */
	template<class T> void RegisterType(uint32_t typeID, std::string className, bool networkSync = true) {
		QGObjectType t;
		t.className = className;
		t.ctor = CreateObject<T>;
		t.type = typeid(T);
		t.typeID = typeID;
		t.networkSync = networkSync;

		m_typeByID[typeID] = t;
		m_typeByType[t.type] = t;
		m_typeByName[className] = t;
	}

	/**
	 * Get type by class
	 */
	template<class T> QGObjectType* GetType() {
		auto it = m_typeByType.find(typeid(T));
		if(it != m_typeByType.end()) return &it->second;
		return(0);
	}

	/**
	 * Get type from an object
	 */
	template<class T> QGObjectType* GetType(T* obj) {
		std::type_index ix = typeid(*obj);
		auto it = m_typeByType.find(ix);
		if (it != m_typeByType.end()) return &it->second;
		return(0);
	}

	/**
	 * Get type by name
	 */
	QGObjectType* GetType(std::string className) {
		auto it = m_typeByName.find(className);
		if (it != m_typeByName.end()) return &it->second;
		return(0);
	}

	/**
	 * Get type by ID
	 */
	QGObjectType* GetType(uint32_t typeID) {
		auto it = m_typeByID.find(typeID);
		if (it != m_typeByID.end()) return &it->second;
		return(0);
	}

protected:
	// Create a new C++ object based on a registered object type
	template<typename T> static QGObject* CreateObject() { return new T; }

	std::map<uint32_t, QGObjectType> m_typeByID;
	std::map<std::string, QGObjectType> m_typeByName;
	std::map<std::type_index, QGObjectType> m_typeByType;
};

#endif