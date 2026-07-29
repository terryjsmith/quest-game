
#ifndef qgserializable_h
#define qgserializable_h

#include <Core/QGDataRecord.h>

class QUEST_API QGSerializable {
public:
	QGSerializable() = default;
	virtual ~QGSerializable() = default;

	virtual void Serialize(QGDataRecord* record) = 0;
	virtual void Deserialize(QGDataRecord* record) = 0;
};

#endif