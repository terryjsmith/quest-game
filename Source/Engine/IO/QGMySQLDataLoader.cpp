
#include <IO/QGMySQLDataLoader.h>

void QGMySQLDataLoader::Open(std::string address) {
	mysql_init(m_dbconn);
	if (mysql_real_connect(m_dbconn, address.c_str(), "qgdb", "Y9Cq1J1k9W07", "questgame", 3306, 0, 0) == NULL) {
		QGASSERT(false, "Unable to connect to database address %s", address.c_str());
		return;
	}
}

std::vector<QGDataRecord*> QGMySQLDataLoader::Load(std::string type, std::map<std::string, std::string> params) {
	std::vector<QGDataRecord*> retval;

	// Assemble query string
	std::string query = "SELECT * FROM " + type;
	if (params.size()) {
		for (auto it = params.begin(); it != params.end(); it++) {
			if (it == params.begin()) {
				query += " WHERE ";
			}
			else {
				query += " AND ";
			}

			query += it->first + " = '" + it->second + "'";
		}
	}

	mysql_query(m_dbconn, query.c_str());

	MYSQL_RES* result = mysql_store_result(m_dbconn);
	int fields = mysql_num_fields(result);
	std::vector<std::string> fieldNames;
	for (int i = 0; i < fields; i++) {
		MYSQL_FIELD* field = mysql_fetch_field(result);
		fieldNames.push_back(field->name);
	}

	while (MYSQL_ROW row = mysql_fetch_row(result)) {
		QGDataRecord* r = new QGDataRecord();
		for (int i = 0; i < fields; i++) {
			if (fieldNames[i] == "id") {
				r->ID(atoi(row[i]));
			}

			r->Set(fieldNames[i], row[i] ? std::string(row[i]) : "");
		}
		retval.push_back(r);
	}

	mysql_free_result(result);
	return(retval);
}

void QGMySQLDataLoader::Save(std::string type, std::vector<QGDataRecord*> records) {

}