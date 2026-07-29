
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

protected:
	std::map<std::string, QGVariant> m_properties;
};

#endif