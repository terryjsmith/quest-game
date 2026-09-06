
#include <Network/QGHttpRequest.h>

bool QGHttpRequest::m_libinitialized = false;

size_t QGHttpRequest::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	QGHttpRequest* request = (QGHttpRequest*)userdata;
	std::string response = ptr;
	response.resize(nmemb);
	request->m_response += response;

	return(size * nmemb);
}

QGHttpRequest::QGHttpRequest(std::string url) {
	if (m_libinitialized == false) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
		m_libinitialized = true;
	}

	m_handle = curl_easy_init();
	curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());

	m_url = url;
	m_response = std::string();
}

QGHttpRequest::~QGHttpRequest() {
	if (m_handle) {
		curl_easy_cleanup(m_handle);
	}
}

std::string QGHttpRequest::Get() {
	curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1);

	struct curl_slist* slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");
	slist1 = curl_slist_append(slist1, "Accept: application/json");

	/* set custom headers */
	curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, &QGHttpRequest::write_callback);

	curl_easy_perform(m_handle);
	return(m_response);
}

std::string QGHttpRequest::Post(std::string data) {
	curl_easy_setopt(m_handle, CURLOPT_POST, 1);

	struct curl_slist* slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Accept: application/json");

	/* set custom headers */
	curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, slist1);

	curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, &QGHttpRequest::write_callback);

	curl_easy_perform(m_handle);
	return(m_response);
}