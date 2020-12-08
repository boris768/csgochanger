#pragma once

typedef unsigned RTime32;

union SteamID_t
{
	enum EUniverse
	{
		k_EUniverseInvalid = 0,
		k_EUniversePublic = 1,
		k_EUniverseBeta = 2,
		k_EUniverseInternal = 3,
		k_EUniverseDev = 4,
		// k_EUniverseRC = 5,				// no such universe anymore
		k_EUniverseMax
	};

	enum EAccountType
	{
		k_EAccountTypeInvalid = 0,
		k_EAccountTypeIndividual = 1,		// single user account
		k_EAccountTypeMultiseat = 2,		// multiseat (e.g. cybercafe) account
		k_EAccountTypeGameServer = 3,		// game server account
		k_EAccountTypeAnonGameServer = 4,	// anonymous game server account
		k_EAccountTypePending = 5,			// pending
		k_EAccountTypeContentServer = 6,	// content server
		k_EAccountTypeClan = 7,
		k_EAccountTypeChat = 8,
		k_EAccountTypeConsoleUser = 9,		// Fake SteamID for local PSN account on PS3 or Live account on 360, etc.
		k_EAccountTypeAnonUser = 10,

		// Max of 16 items in this field
		k_EAccountTypeMax
	};
	struct SteamIDComponent_t
	{
#ifdef VALVE_BIG_ENDIAN
		EUniverse			m_EUniverse : 8;	// universe this account belongs to
		unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
		unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
		uint32				m_unAccountID : 32;			// unique account identifier
#else
		unsigned int		m_unAccountID : 32;			// unique account identifier
		unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
		unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
		EUniverse			m_EUniverse : 8;	// universe this account belongs to
#endif
	} m_comp;

	unsigned long long m_unAll64Bits;
	bool operator==(const SteamID_t& k)
	{
		return m_comp.m_unAccountID == k.m_comp.m_unAccountID;
	}
};

typedef unsigned CellID_t;
constexpr CellID_t k_uCellIDInvalid = 0xFFFFFFFF;

// handle to a Steam API call
typedef unsigned long long SteamAPICall_t;
constexpr SteamAPICall_t k_uAPICallInvalid = 0x0;

typedef unsigned AccountID_t;

typedef unsigned PartnerId_t;
constexpr PartnerId_t k_uPartnerIdInvalid = 0;

// ID for a depot content manifest
typedef unsigned long long ManifestId_t;
constexpr ManifestId_t k_uManifestIdInvalid = 0;

typedef int HSteamUser;

class ISteamUser;
class ISteamGameServer;
class ISteamFriends;
class ISteamUtils;
class ISteamMatchmaking;
class ISteamContentServer;
class ISteamMatchmakingServers;
class ISteamUserStats;
class ISteamApps;
class ISteamNetworking;
class ISteamRemoteStorage;
class ISteamScreenshots;
class ISteamMusic;
class ISteamMusicRemote;
class ISteamGameServerStats;
class ISteamPS3OverlayRender;
class ISteamHTTP;
class ISteamUnifiedMessages;
class ISteamController;
class ISteamUGC;
class ISteamAppList;
class ISteamHTMLSurface;
class ISteamInventory;
class ISteamVideo;

class ISteamUser
{
public:
	// returns the HSteamUser this interface represents
	// this is only used internally by the API, and by a few select interfaces that support multi-user
	virtual HSteamUser GetHSteamUser() = 0;

	// returns true if the Steam client current has a live connection to the Steam servers. 
	// If false, it means there is no active connection due to either a networking issue on the local machine, or the Steam server is down/busy.
	// The Steam client will automatically be trying to recreate the connection as often as possible.
	virtual bool BLoggedOn() = 0;
	// returns the CSteamID of the account currently logged into the Steam client
	// a CSteamID is a unique identifier for an account, and used to differentiate users in all parts of the Steamworks API
	virtual SteamID_t GetSteamID() = 0;
};

class ISteamFriends
{
public:
	// returns the local players name - guaranteed to not be NULL.
	// this is the same name as on the users community profile page
	// this is stored in UTF-8 format
	// like all the other interface functions that return a char *, it's important that this pointer is not saved
	// off; it will eventually be free'd or re-allocated
	virtual const char *GetPersonaName() = 0;
};

class CSteamAPIContext
{
public:
	void* m_pSteamUtilsUnk;
	ISteamUser* m_pSteamUser;
	ISteamFriends* m_pSteamFriends;
	ISteamUtils* m_pSteamUtils;
	ISteamMatchmaking* m_pSteamMatchmaking;
	ISteamUserStats* m_pSteamUserStats;
	ISteamApps* m_pSteamApps;
	ISteamMatchmakingServers* m_pSteamMatchmakingServers;
	ISteamNetworking* m_pSteamNetworking;
	ISteamRemoteStorage* m_pSteamRemoteStorage;
	ISteamScreenshots* m_pSteamScreenshots;
	ISteamHTTP* m_pSteamHTTP;
	ISteamUnifiedMessages* m_pSteamUnifiedMessages;
	ISteamController* m_pController;
	ISteamUGC* m_pSteamUGC;
	ISteamAppList* m_pSteamAppList;
	ISteamMusic* m_pSteamMusic;
	ISteamMusicRemote* m_pSteamMusicRemote;
	ISteamHTMLSurface* m_pSteamHTMLSurface;
};
