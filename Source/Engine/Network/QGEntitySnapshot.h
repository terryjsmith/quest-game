
#ifndef qgentitysnapshot_h
#define qgentitysnapshot_h

#include <Core/QGEntity.h>

class QUEST_API QGEntitySnapshot {
public:
	QGEntitySnapshot() : tick(0) {}
	~QGEntitySnapshot() = default;

	unsigned char* Serialize(uint32_t& size);
	void Deserialize(unsigned char* bytes, uint32_t size);

	struct QGSerializedComponent {
		QGDataRecord record;
		uint32_t type;
	};

	struct QGSerializedEntity {
		QGDataRecord record;
		std::map<uint32_t, QGSerializedComponent> components;
	};

public:
	uint64_t tick;
	std::map<uint64_t, QGSerializedEntity> entities;
};

#endif