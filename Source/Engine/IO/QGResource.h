
#ifndef qgresource_h
#define qgresource_h

#include <Core/QGObject.h>

class QUEST_API QGResourceSystem;

class QUEST_API QGResource : public QGObject {
public:
	QGResource() {
		filesize = 0;
		m_loaded = false;
		m_data = 0;
		m_fp = 0;
	}

	~QGResource() {
		if (m_data) free(m_data);
		if (m_fp) fclose(m_fp);
	}

	unsigned char* Data() { return m_data; }

	friend class QGResourceSystem;

public:
	std::string filename;
	std::string path;
	std::string extension;
	unsigned int filesize;

protected:
	bool m_loaded;
	unsigned char* m_data;

	FILE* m_fp;
};

#endif