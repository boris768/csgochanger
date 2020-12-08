#include "Server.h"

//#define CURL_STATICLIB
//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"wldap32.lib")
//#pragma comment (lib, "crypt32")
//#pragma comment(lib, "curl/libcurl.lib")
//#include "curl/curl.h"
//#include "XorStr.hpp"
#include "Logger.h"

//static size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp)
//{
//	static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
//	return size * nmemb;
//}

void __cdecl Server::post_request(void* pArgs)
{
	//jsonRequest* args = static_cast<jsonRequest*>(pArgs);
	//curl_global_init(CURL_GLOBAL_ALL);

	//struct curl_slist* headers = nullptr;

	//headers = curl_slist_append(headers, XorStr("Content-Type: application/json"));

	//std::string buffer;
	//CURL* curl = curl_easy_init();
	//if (curl)
	//{
	//	curl_easy_setopt(curl, CURLOPT_URL, args->link.c_str());
	//	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->request_msg.c_str());
	//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	//	curl_easy_setopt(curl, CURLOPT_USERAGENT, XorStr("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.5"));
	//	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, args->request_msg.size());
	//	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	//	const CURLcode res = curl_easy_perform(curl);

	//	if (res != CURLE_OK)
	//	{
	//		global_logger.write_line(std::string(XorStr("-- Can't to send event to server :( Code: ") + std::to_string(res) + XorStr(" --")).c_str());
	//	}

	//	curl_easy_cleanup(curl);
	//}
	//curl_global_cleanup();

	//args->response = std::move(buffer);
	//delete args;
}
