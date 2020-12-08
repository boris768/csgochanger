#pragma once
#include <string>
#include <vector>

class banner;

struct banner_info
{
	std::string url;
	std::string source;
	std::string md5;
	int position;

	banner_info(std::string _url, std::string _source, std::string _md5, const int _position)
		: url(move(_url)), source(move(_source)), md5(move(_md5)), position(_position)
	{
	}

	banner* GetBanner() const;

	banner_info()
	{
		url.clear();
		source.clear();
		md5.clear();
		position = 0;
	}
};

namespace AdvertiseTool
{
	namespace Banners
	{
		extern void GetBannersInfoFromServer(banner_info& first_banner_info, banner_info& second_banner_info);
	}
}

namespace Server
{
	extern unsigned char* download(const char* file_address, size_t* size, int* exit_http_code = nullptr);
	void GetClientCountry();
}
