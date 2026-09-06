
#ifndef qghttprequest_h
#define qghttprequest_h

#include <Core/QGObject.h>
#include <curl/curl.h>

class QUEST_API QGHttpRequest : public QGObject {
public:
	QGHttpRequest(std::string url);
	~QGHttpRequest();

	/**
	 * Make a request
	 */
	std::string Get();
	std::string Post(std::string data);

	/**
	 * Callback
	 */
	static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

protected:
	std::string m_url;
	CURL* m_handle;
	static bool m_libinitialized;
	std::string m_response;
};

#endif