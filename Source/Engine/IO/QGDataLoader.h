
#ifndef qgdataloader_h
#define qgdataloader_h

#include <Core/QGSystem.h>
#include <Core/QGSerializable.h>

class QUEST_API QGDataLoader : public QGSystem {
public:
	QGDataLoader() = default;
	virtual ~QGDataLoader() = default;

	virtual void Open(std::string address) = 0;

	virtual std::vector<QGDataRecord*> Load(std::string type, std::map<std::string, std::string> params) = 0;
	virtual void Save(std::string type, std::vector<QGDataRecord*> records) = 0;
};

#endif