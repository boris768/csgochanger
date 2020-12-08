// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "banner_downloader.h"
#include "json\json.h"
#include "Config.h"
#include "md5.h"
#include "XorStr.hpp"

#include <fstream>
#include "banner.h"

//#include <curl/curl.h>
#include "ISteamHTTP.h"
#include "Hooks.hpp"
//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"wldap32.lib")
//#pragma comment(lib, "curl/libcurl.lib")

//std::vector<banner_info> banners;
//
//
//struct MemoryStruct
//{
//	unsigned char* memory;
//	size_t size;
//};
//
//size_t __cdecl write_memory_callback(void* contents, const size_t size, const size_t nmemb, void* userp) //-V2009
//{
//	const size_t realsize = size * nmemb;
//	auto mem = static_cast<struct MemoryStruct *>(userp);
//	const auto realloced_memory = realloc(mem->memory, mem->size + realsize + 1);
//	if (!realloced_memory)
//	{
//		/* out of memory! */
//		return 0;
//	}
//	mem->memory = static_cast<unsigned char*>(realloced_memory);
//	
//	memcpy(&(mem->memory[mem->size]), contents, realsize);
//	mem->size += realsize;
//	mem->memory[mem->size] = 0;
//
//	return realsize;
//}
//
//unsigned char* Server::download(const char* file_address, size_t* size, int* exit_http_code)
//{
//	//struct curl_slist *headers = nullptr;
//	long http_code = 0;
//	MemoryStruct chunk{ static_cast<unsigned char*>(malloc(1)) ,0};
//
//	CURL* curl = curl_easy_init();
//	if (curl)
//	{
//		curl_easy_setopt(curl, CURLOPT_URL, file_address);
//
//		/* send all data to this function  */
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
//
//		/* we pass our 'chunk' struct to the callback function */
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&chunk));
//
//		/* some servers don't like requests that are made without a user-agent field, so we provide one */
//		curl_easy_setopt(curl, CURLOPT_USERAGENT, XorStr("libcurl-agent/1.0"));
//
//		/* get it! */
//		const CURLcode res = curl_easy_perform(curl);
//		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//		if (exit_http_code)
//			*exit_http_code = http_code;
//		if (res != CURLE_OK || http_code != 200)
//		{
//			//TODO: http status parsing
//		}
//		else
//		{
//			curl_easy_cleanup(curl);
//			if (size)
//			{
//				*size = chunk.size;
//			}
//			return chunk.memory;
//		}
//		curl_easy_cleanup(curl);
//	}
//	return nullptr;
//}

std::vector<banner_info>* parse_banners_from_response(const char* response)
{
	return nullptr;
	//Json::Reader reader;
	//Json::Value banner_info;
	//reader.parse(response, banner_info, false);
	//std::string url;
	//std::string source;
	//std::string md5;
	//int position;
	//auto banner = banner_info[XorStr("banner")];
	//if (banner.isArray())
	//{
	//	for (Json::Value::iterator it = banner.begin(); it != banner.end(); ++it)
	//	{
	//		GetString(it->operator[](XorStr("url")), &url);
	//		GetString(it->operator[](XorStr("source")), &source);
	//		GetString(it->operator[](XorStr("md5")), &md5);
	//		GetInt(it->operator[](XorStr("position")), &position);
	//		banners.emplace_back(url, source, md5, position);
	//	}
	//}
	//else
	//{
	//	//TODO: parsing error!
	//}
	//return &banners;
}

std::string parse_country_from_response(const char* response)
{
	Json::Reader reader;
	Json::Value client_info;
	reader.parse(response, client_info, false);

	std::string country_name;
	get_string(client_info.operator[](XorStr("country_name")), &country_name);
	return country_name;
}

void __cdecl banners_cfg_accepted(CHTTPRequest* request, HTTPRequestCompleted* answer)
{
	if (!answer->m_bRequestSuccessful || answer->m_eStatusCode != 200 || answer->m_unBodySize == 0)
	{
		request->ClearCallback();
		return;
	}

	const auto buffer = new uint8_t[answer->m_unBodySize + 1];
	if (!SourceEngine::interfaces.SteamHTTP()->GetHTTPResponseBodyData(answer->m_hRequest, buffer, answer->m_unBodySize))
		return;

	//TODO: catch parsing errors		
	std::vector<banner_info>* banners = parse_banners_from_response(reinterpret_cast<const char*>(buffer));

	delete[] buffer;
}

//STAGE 2: request current banner config
void get_banners_cfg(CHTTPRequest* request, const std::string& target_country)
{
	std::string banner_cfg = XorStr("http://csc-server.ru/ADV/");
	banner_cfg.append(target_country);
	banner_cfg.append(XorStr("/json_response.txt"));

	request->SetCallback(banners_cfg_accepted);
	request->SetEndpoint(banner_cfg.c_str());
	request->Execute();
}

void __cdecl country_accepted(CHTTPRequest* freegeo_request, HTTPRequestCompleted* answer)
{
	std::string country = XorStr("OTHER");

	//if answer was bad, try get default banners
	if (!answer->m_bRequestSuccessful || answer->m_eStatusCode != 200 || answer->m_unBodySize == 0)
	{
		freegeo_request->ClearCallback();
		get_banners_cfg(freegeo_request, country);
		return;
	}

	const auto buffer = new uint8_t[answer->m_unBodySize + 1];
	if(!SourceEngine::interfaces.SteamHTTP()->GetHTTPResponseBodyData(answer->m_hRequest, buffer, answer->m_unBodySize))
		get_banners_cfg(freegeo_request, country);

	//TODO: catch parsing errors
	country = parse_country_from_response(reinterpret_cast<const char*>(buffer));
	delete [] buffer;

	get_banners_cfg(freegeo_request, country);

	//auto country = parse_country_from_response(answer->)
}

void Server::GetClientCountry()
{
	//int http_code;
	//char* const response = reinterpret_cast<char* const>(download(
	//	XorStr("https://freegeoip.net/json/"), nullptr, &http_code));
	//if (http_code == 200)
	//{
	//	std::string res = parse_country_from_response(response);
	//	free(response);
	//	return res;
	//}

	//CHTTPRequest* freegeo_request = new CHTTPRequest(HTTP_METHOD_GET, XorStr("https://freegeoip.net/json/"));
	//freegeo_request->SetCallback(country_accepted);
	//freegeo_request->Execute();

}

namespace AdvertiseTool
{
	void Banners::GetBannersInfoFromServer(banner_info& first_banner_info, banner_info& second_banner_info)
	{

		////STAGE 1: request current user country from GEOIP
		////Server::GetClientCountry();


		//const std::string country = Server::GetClientCountry();
		//const std::string serverpath = XorStr("http://csc-server.ru/ADV/") + country + XorStr("/json_response.txt");
		//int http_code;
		//char* banner_json = reinterpret_cast<char* const>(Server::download(serverpath.c_str(), nullptr, &http_code));
		//if (http_code != 200)
		//	banner_json = reinterpret_cast<char* const>(Server::download(
		//		XorStr("http://csc-server.ru/ADV/OTHER/json_response.txt"), nullptr, &http_code)); // downloads default banners
		//if (http_code == 200)
		//{
		//	const std::vector<banner_info>* banners = parse_banners_from_response(banner_json);
		//	free(banner_json);
		//	for (const auto& it : *banners)
		//	{
		//		if (it.position == 1)
		//			first_banner_info = it;
		//		else if (it.position == 2)
		//			second_banner_info = it;
		//	}
		//}
	}
}

banner* banner_info::GetBanner() const
{
	//banner* res = nullptr;
	//if (!md5.empty())
	//{
	//	while (!res)
	//	{
	//		std::basic_ifstream<unsigned char> in(md5, std::ifstream::binary);
	//		if (in.is_open())
	//		{
	//			auto banner_image_data = new std::basic_string<unsigned char>();
	//			in.seekg(0, std::ios::end);
	//			const size_t size = in.tellg();
	//			banner_image_data->reserve(size);
	//			in.seekg(0);
	//			in.read(&banner_image_data->operator[](0), size);
	//			in.close();

	//			md5::md5_t check;
	//			check.process(banner_image_data->c_str(), size);
	//			check.finish();
	//			char sig[MD5_STRING_SIZE] = {'\0'};
	//			check.get_string(sig);
	//			if (sig == md5)
	//			{
	//				res = new banner(url, L"", banner_image_data, size);
	//			}
	//		}
	//		else
	//		{
	//			size_t size = 0;
	//			unsigned char*const banner_image_data = Server::download(source.c_str(), &size);
	//			if (size > 0 && banner_image_data != nullptr)
	//			{
	//				md5::md5_t check;
	//				check.process(banner_image_data, size);
	//				check.finish();
	//				char sig[MD5_STRING_SIZE] = {'\0'};
	//				check.get_string(sig);
	//				std::basic_ofstream<unsigned char> out(sig, std::ofstream::binary);
	//				if (out.is_open())
	//				{
	//					out.write(banner_image_data, size);
	//				}
	//				out.flush();
	//				out.close();
	//				free(banner_image_data);
	//			}
	//			else
	//			{
	//				// Image wasn't downloaded
	//				return nullptr;
	//			}
	//		}
	//	}
	//}
	//return res;
	return nullptr;
}
