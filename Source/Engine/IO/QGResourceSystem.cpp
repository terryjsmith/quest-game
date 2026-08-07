
#include <IO/QGResourceSystem.h>
#include <Core/QGApplication.h>
#include <Core/QGMetaSystem.h>

QGResourceSystem::~QGResourceSystem() {
	for (auto it = m_resourceObjects.begin(); it != m_resourceObjects.end(); it++) {
		delete it->second;
	}
	m_resourceObjects.clear();
}

QGResourceObject* QGResourceSystem::Load(std::string filename, std::string type) {
	// Check if we already have this resource
	auto it = m_resourceObjects.find(filename);
	if (it != m_resourceObjects.end()) return(it->second);

	// If we do not, attempt to load it
	bool isBinary = m_resourceIsBinary[type];
	FILE* fp = fopen(filename.c_str(), isBinary ? "rb" : "r");
	QGASSERT(fp != 0, "Unable to open file for reading.");

	// Get the filesize
	fseek(fp, 0, SEEK_END);
	unsigned int filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Parse filename from path from extension
	std::filesystem::path path(filename);

	// Create a new resource
	QGResource* resource = new QGResource();
	resource->filename = path.filename().string();
	resource->extension = path.extension().string();
	resource->path = path.parent_path().string();
	resource->filesize = filesize;
	resource->m_fp = fp;

	// Fix extension
	resource->extension = resource->extension.substr(1, resource->extension.length() - 1);

	// Load the data
	resource->m_data = (unsigned char*)malloc(resource->filesize);
	memset(resource->m_data, 0, resource->filesize);
	fread(resource->m_data, 1, resource->filesize, resource->m_fp);

	m_resources[filename] = resource;

	// Get the loader
	QGASSERT(m_resourceLoaders.find(type) != m_resourceLoaders.end(), "Unable to find registered resource loader for type.");
	QGResourceLoader* loader = m_resourceLoaders[type];
	QGResourceObject* obj = loader->LoadResource(resource, type);
	obj->resource = m_resources[filename];

	// Save it
	m_resourceObjects[filename] = obj;
	return(obj);
	
}