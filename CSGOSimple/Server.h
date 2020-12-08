#pragma once
#include <string>

struct jsonRequest
{
	std::string request_msg;
	std::string link;

	jsonRequest(const std::string& requestMsg, const std::string& link)
		: request_msg(requestMsg), link(link) {}

	std::string response;
};

class Server
{
public:
	static void __cdecl post_request(void* pArgs);
};