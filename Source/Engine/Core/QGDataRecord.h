
#ifndef qgdatarecord_h
#define qgdatarecord_h

#include <Core/QGVariant.h>

class QUEST_API QGDataRecord : public QGObject {
public:
	QGDataRecord() = default;
	~QGDataRecord() = default;

	QGVariant Get(std::string key);
	void Set(std::string key, QGVariant value);

	void Serialize(unsigned char* bytes, unsigned int& size);
	void Deserialize(unsigned char* bytes, unsigned int size);

	void ID(uint64_t id) { m_id = id; }
	uint64_t ID() { return m_id; }

protected:
	std::map<std::string, QGVariant> m_properties;
	uint64_t m_id;
};

#endif