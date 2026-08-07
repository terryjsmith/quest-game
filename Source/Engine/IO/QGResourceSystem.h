
#ifndef qgresourcesystem_h
#define qgresourcesystem_h

#include <Core/QGSystem.h>
#include <IO/QGResourceObject.h>
#include <IO/QGResourceLoader.h>

class QUEST_API QGResourceSystem : public QGSystem {
public:
	QGResourceSystem() = default;
	~QGResourceSystem();

	/**
	 * Load a resource
	 */
	QGResourceObject* Load(std::string filename, std::string type);

	/**
	 * Register a resource type and a loader
	 */
	template<class T> void RegisterResourceLoader(std::string type, bool binary = true) {
		QGResourceLoader* loader = new T();
		m_resourceLoaders[type] = loader;
		m_resourceIsBinary[type] = binary;
	}

protected:
	// List of available resources
	std::map<std::string, QGResource*> m_resources;
	std::map<std::string, QGResourceObject*> m_resourceObjects;

	// List of resource loaders
	std::map<std::string, QGResourceLoader*> m_resourceLoaders;
	std::map<std::string, bool> m_resourceIsBinary;
};

#endif