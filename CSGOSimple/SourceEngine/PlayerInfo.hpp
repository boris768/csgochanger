#pragma once

//#include "Definitions.hpp"

namespace SourceEngine
{
    typedef struct player_info_s
    {
		constexpr player_info_s() noexcept: version(0), xuid(0), name{}, userid(0), guid{},
		                           friendsid(0),
		                           friendsname{},
		                           fakeplayer(false),
		                           ishltv(false),
		                           customfiles{}, filesdownloaded(0)
		{
		}
    	
		void clear()
		{
			memset(this, 0, sizeof(player_info_s));
		}
    	
		uint64_t version;
		union {
			uint64_t xuid;
			struct {
				int xuidlow;
				int xuidhigh;
			};
		};
		char name[128];
		int userid;
		char guid[33];
		unsigned int friendsid;
		char friendsname[128];
		bool fakeplayer;
		bool ishltv;
		unsigned int customfiles[4];
		unsigned char filesdownloaded;
	//private:
	//	char pad_big[0x200];

	} player_info_t;
}

/*
private:
	DWORD pad0[2];

public:
	int XuidLow;
	int XuidHigh;

	char name[128];
	int userID;
	char guid[33];
	DWORD friendsID;
	char friendsName[128];
	bool fakeplayer;
	bool ishltv;
	DWORD customFiles[4];
	BYTE filesDownloaded;
	*/

/*
 *      __int64        unknown;            //0x0000 
        __int64        steamID64;          
        char           szName[128];        //0x0010 - Player Name
        int            userId;             //0x0090 - Unique Server Identifier
        char           szSteamID[20];      //0x0094 - STEAM_X:Y:Z
        char           pad_0x00A8[0x10];   //0x00A8
        unsigned long  iSteamID;           //0x00B8 - SteamID 
        char           unknown2[0x14C];
 */