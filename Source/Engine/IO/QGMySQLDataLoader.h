
#ifndef qgmysqldataloader_h
#define qgmysqldataloader_h

#include <IO/QGDataLoader.h>
#include <mysql.h>

class QUEST_API QGMySQLDataLoader : public QGDataLoader {
public:
	QGMySQLDataLoader() :m_dbconn(0) {}
	~QGMySQLDataLoader() = default;

	void Open(std::string address);

	std::vector<QGDataRecord*> Load(std::string type, std::map<std::string, std::string> params);
	void Save(std::string type, std::vector<QGDataRecord*> records);

protected:
	MYSQL* m_dbconn;
};

#endif