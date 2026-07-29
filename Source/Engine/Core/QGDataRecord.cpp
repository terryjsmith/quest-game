
#include <Core/QGDataRecord.h>

QGVariant QGDataRecord::Get(std::string key) {
	auto it = m_properties.find(key);
	if (it == m_properties.end()) {
		QGASSERT(false, "Serialized key not found.");
		return((int)0);
	}

	return(m_properties[key]);
}

void QGDataRecord::Set(std::string key, QGVariant value) {
	m_properties[key] = value;
}

void QGDataRecord::Serialize(unsigned char* bytes, unsigned int& size) {
	// Calculate size
	unsigned int byteSize = sizeof(uint8_t); // Number of properties
	for (auto it = m_properties.begin(); it != m_properties.end(); it++) {
		byteSize += sizeof(uint8_t); // Type of variant
		byteSize += sizeof(uint8_t); // Length of property name
		byteSize += it->first.length(); // Property name
		byteSize += sizeof(uint8_t);	// Size of property
		byteSize += it->second.Size(); // Property value
	}

	size = byteSize;

	// Compute only
	if (bytes == 0) return;

	// Create package
	int offset = 0;

	// Number of properties
	uint8_t numComponents = m_properties.size();
	memcpy(bytes + offset, &numComponents, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	// For each property
	for (auto it = m_properties.begin(); it != m_properties.end(); it++) {
		// Type
		uint8_t type = it->second.Type();
		memcpy(bytes + offset, &type, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Property name length
		uint8_t namelen = it->first.size();
		memcpy(bytes + offset, &namelen, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Property name
		memcpy(bytes + offset, it->first.c_str(), namelen);
		offset += namelen;

		// Size of property
		uint8_t psize = it->second.Size();
		memcpy(bytes + offset, &psize, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Property value
		if (psize > 0) {
			memcpy(bytes + offset, it->second.GetPtr(), it->second.Size());
			offset += it->second.Size();
		}
	}
}

void QGDataRecord::Deserialize(unsigned char* bytes, unsigned int size) {
	int offset = 0;
	
	// Number of properties
	uint8_t propertyCount = 0;
	memcpy(&propertyCount, bytes + offset, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	// For each property
	for (int i = 0; i < propertyCount; i++) {
		// Type
		uint8_t type = 0;
		memcpy(&type, bytes + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Length of property name
		uint8_t namelen = 0;
		memcpy(&namelen, bytes + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Property name
		char* name = (char*)malloc(namelen + 1);
		memcpy(name, bytes + offset, namelen);
		offset += namelen;
		name[namelen] = '\0';

		std::string strname = name;
		free(name);

		// Size of property
		uint8_t psize = 0;
		memcpy(&psize, bytes + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Property
		QGVariant v;
		QGASSERT(psize > 0 || (psize == 0 && type == QGVariant::VAR_STRING), "Unexpected zero size value");
		if (psize == 0) {
			// Should be only strings
			v = "";
		}
		else {
			v.FromBytes(bytes + offset, type, psize);
			offset += psize;
		}

		m_properties[strname] = v;
	}

	QGASSERT(offset == size, "Size mismatch.");
}