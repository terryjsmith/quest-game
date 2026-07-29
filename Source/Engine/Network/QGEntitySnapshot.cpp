
#include <Network/QGEntitySnapshot.h>

unsigned char* QGEntitySnapshot::Serialize(uint32_t& size) {
	// Calculate byte size
	int byteSize = sizeof(uint32_t); // Number of entities
	for (auto eit = entities.begin(); eit != entities.end(); eit++) {
		// Entity ID
		byteSize += sizeof(uint64_t);

		// Size of entity record
		byteSize += sizeof(uint32_t);

		// Compute only first
		unsigned int size = 0;
		eit->second.record.Serialize(0, size);
		byteSize += size;

		// Compute size of components
		byteSize += sizeof(uint8_t);
		for (auto cit = eit->second.components.begin(); cit != eit->second.components.end(); cit++) {
			// Type
			byteSize += sizeof(uint32_t);

			// Sizeof record
			byteSize += sizeof(uint32_t);

			// Record
			cit->second.record.Serialize(0, size);
			byteSize += size;
		}
	}

	int offset = 0;
	uint32_t retsize = 0;
	unsigned char* bytes = (unsigned char*)malloc(byteSize);

	// Number of entities
	uint32_t numEntities = entities.size();
	memcpy(bytes + offset, &numEntities, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (auto eit = entities.begin(); eit != entities.end(); eit++) {
		// Entity ID
		memcpy(bytes + offset, &eit->first, sizeof(uint64_t));
		offset += sizeof(uint64_t);

		// Size of entity record
		eit->second.record.Serialize(0, retsize);
		memcpy(bytes + offset, &retsize, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		// Write entity record
		eit->second.record.Serialize(bytes + offset, retsize);
		offset += retsize;

		// Write num components
		uint8_t numComponents = eit->second.components.size();
		memcpy(bytes + offset, &numComponents, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		// Foreach component
		for (auto cit = eit->second.components.begin(); cit != eit->second.components.end(); cit++) {
			// Type
			uint32_t type = cit->second.type;
			memcpy(bytes + offset, &type, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// Sizeof record
			cit->second.record.Serialize(0, retsize);
			memcpy(bytes + offset, &retsize, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// Record
			cit->second.record.Serialize(bytes + offset, retsize);
			offset += retsize;
		}
	}

	size = byteSize;
	return(bytes);
}

void QGEntitySnapshot::Deserialize(unsigned char* bytes, uint32_t size) {
	// Number of entities
	uint32_t numEntities = 0;
	int offset = 0;

	memcpy(&numEntities, bytes + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < numEntities; i++) {
		// Entity ID
		uint64_t entityID;
		memcpy(&entityID, bytes + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);

		// Size of entity record
		uint32_t recordSize = 0;
		memcpy(&recordSize, bytes + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		// Read entity record
		QGSerializedEntity entity;
		entity.record.Deserialize(bytes + offset, recordSize);
		offset += recordSize;

		// Number of components
		uint8_t numComponents = 0;
		memcpy(&numComponents, bytes + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		for (int j = 0; j < numComponents; j++) {
			// Type of record
			uint32_t type = 0;
			memcpy(&type, bytes + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// Size of record
			uint32_t crecordSize = 0;
			memcpy(&crecordSize, bytes + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// Read record
			QGSerializedComponent component;
			component.type = type;
			component.record.Deserialize(bytes + offset, crecordSize);
			offset += crecordSize;

			entity.components[type] = component;
		}

		this->entities[entityID] = entity;
	}

	QGASSERT(offset == size, "Byte count mismatch.");
}