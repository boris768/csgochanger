#pragma once
#include <Windows.h>
#include <d3d9.h>

#ifdef CHANGER_PROFILE
#include <ittnotify.h>
#endif

#include <memory> //unique_ptr & make_unique

#include "SourceEngine/SDK.hpp"
#include "Config.h"
#include "Skins.h"
//#include "banner.h"
#include "Protobuffs.h"
#include "EconEntityListener.h"

#ifdef _DEBUG
void _Debug(const wchar_t *msg, ...);
#define debug(msg, ...) _Debug(msg, __VA_ARGS__)
#else
#define debug(msg, ...)
#endif

enum localize_strings_index : int
{
	SkinChanger_Team,
	SkinChanger_Weapons,
	SkinChanger_Skins,
	SkinChanger_Name,
	SkinChanger_Wear,
	SkinChanger_Seed,
	SkinChanger_StatTrak,
	SkinChanger_Kills,
	SkinChanger_Quality,
	SkinChanger_Stickers,
	SkinChanger_Sticker_slot,
	SkinChanger_Scale,
	SkinChanger_Rotation,
	SkinChanger_Knifes,
	SkinChanger_Wearables,
	SkinChanger_Save_config,
	SkinChanger_Load_config,
	SkinChanger_Copy_T_config_to_CT,
	SkinChanger_Copy_CT_config_to_T,
	SkinChanger_AutoAccept,
	SkinChanger_Fix_ViewModel_after_changing_team,
	SkinChanger_SuperStattrack,
	SkinChanger_Sky,
	SkinChanger_Sticker_filter,
	SkinChanger_Reset,
	SkinChanger_RandomSkin,
	SkinChanger_Randomize,
	SkinChanger_Random_skin_key,
	SkinChanger_Reload_inventory,
	SkinChanger_OldSounds,
	SkinChanger_OldSounds_NotInGameWarning,	//"Can not be changed while playing!\nAll changes will be applied after the game"
	SkinChanger_OldSounds_PrecacheSounds,//"Precache sounds (only one time)"
	SkinChanger_OldSounds_NotDownloadedWarning,//"Sounds are not downloaded from the client!"
	SkinChanger_SuperStattrack_Description,
	SkinChanger_RandomSkin_Description,
	SkinChanger_Randomize_Description,
	SkinChanger_Fix_ViewModel_after_changing_team_Description,
	SkinChanger_Menu_key,
	SkinChanger_AutoSave_if_changed,
	SkinChanger_Custom_Players,
	SkinChanger_Custom_Players_enable,
	SkinChanger_sizeof
};

namespace Hooks
{
	///<summary>
	/// Sets up and hooks some functions. This should be called when the dll is loaded.
	///</summary>
	void __cdecl Initialize(void*);

	///<summary>
	/// Cleans up and removes the hooks. This should be called when the dll is unloaded.
	///</summary>
	void Restore();

	///<summary>
	/// Initialized the GUI system. Called from EndScene.
	///</summary>
	///<param name="pDevice"> Pointer to the D3D9 Device </param>
	void GUI_Init(IDirect3DDevice9* pDevice);
	void flash_window() noexcept;
	void auto_accept() noexcept;

	//---------------------------------------------------
	// Hook prototypes
	//---------------------------------------------------
	typedef void(__fastcall* EmitSound_t)(void*, void*, void*, int, int, const char*, unsigned int, const char*, float, float, int, int, int,
		const SourceEngine::Vector* pOrigin, const SourceEngine::Vector* pDirection, SourceEngine::Vector* pUtlVecOrigins,
		bool bUpdatePositions, float soundtime, int speakerentity, SourceEngine::StartSoundParams_t& params);
	typedef HRESULT(__stdcall* Present_t)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
	typedef HRESULT(__stdcall* EndScene_t)(IDirect3DDevice9*);
	typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef void(__stdcall* ReceivedTicket_t)(CMsgSteamDatagramGameServerAuthTicket*);
	typedef void(__thiscall* PlaySound_t)(SourceEngine::ISurface*, const char*);
	typedef void* (__thiscall* FrameStageNotify_t)(void*, SourceEngine::ClientFrameStage_t);
	typedef bool(__thiscall* FireEventClientSide_t)(void*, SourceEngine::IGameEvent*);
	typedef void(__thiscall* PaintTraverse_t)(void*, unsigned int, bool, bool);
	typedef EGCResults(__fastcall* SendMessage_t)(void*, void*, uint32_t, const void*, uint32_t);
	typedef EGCResults(__fastcall* RetrieveMessage_t)(void*, void*, uint32_t*, void*, uint32_t, uint32_t*);
	typedef SourceEngine::MDLHandle_t(__thiscall* FindMDL_t)(void*, const char*);
	typedef FileNameHandle_t(__fastcall* FindOrAddFilename_t)(IFileSystem*, int, char const*);
	typedef bool __fastcall LooseFileAllowed_t(IFileSystem*, void*);
	typedef int(__fastcall* GetUnverifiedFileHashes_t)(IFileSystem*, void*, CUnverifiedFileHash*, int);
	typedef bool(__stdcall* SetLocalPlayerReady_t)(const char*);
	typedef void(__thiscall* LockCursor_t)(void*);
	typedef DWORD(__thiscall* FindHudElement_t)(void*, const char*);
	typedef int(__thiscall* ClearHudWeapon_t)(void*, int);
	typedef void* (__thiscall* GetModelsForMap_t) (SourceEngine::IGameTypes* ecx, const char* map_name);
	typedef void(__thiscall* LevelInitPostEntity_t)(void*);
	typedef void(__thiscall* LevelShutdown_t)(void*);
	typedef void(__thiscall* ItemPostFrame_ProcessIdleNoAction_t)(C_BaseCombatWeapon*, C_CSPlayer*);
	
	extern SetLocalPlayerReady_t IsReady;
	extern FindHudElement_t FindHudElement;
	extern ClearHudWeapon_t ClearHudWeapon;

	//---------------------------------------------------
	// Original functions
	//---------------------------------------------------
	extern Present_t g_fnOriginalPresent;
	extern EndScene_t g_fnOriginalEndScene;
	extern Reset_t g_fnOriginalReset;
	extern PlaySound_t g_fnOriginalPlaySound;
	extern ReceivedTicket_t g_fnOriginalReceivedTicket;
	extern FrameStageNotify_t g_fnOriginalFrameStageNotify;
	extern SourceEngine::RecvVarProxyFn g_fnOriginalnSequence;
	extern FireEventClientSide_t g_fnOriginalFireEventClientSide;
	extern PaintTraverse_t g_fnOriginalPaintTraverse;
	extern SendMessage_t g_fnOriginalSendMessage;
	extern RetrieveMessage_t g_fnOriginalRetrieveMessage;
	extern FindOrAddFilename_t g_fnOriginalFindOrAddFilename;
	extern FindMDL_t g_fnOriginalFindMDL;
	extern SourceEngine::RecvVarProxyFn g_fnOriginalSequence;
	extern SourceEngine::RecvVarProxyFn g_fnOriginalMusicIDInTable;
	extern SourceEngine::DataTableRecvVarProxyFn g_fnOriginalMusicID;
	extern LockCursor_t g_fnOriginalLockCursor;
	extern GetModelsForMap_t g_fnOriginalGetTModelsForMap_t;
	extern GetModelsForMap_t g_fnOriginalGetCTModelsForMap_t;
	extern LevelInitPostEntity_t g_fnOriginalLevelInitPostEntity;
	extern LevelShutdown_t g_fnOriginalLevelShutdown;
	extern ItemPostFrame_ProcessIdleNoAction_t g_fnItemPostFrame_ProcessIdleNoAction;
	//---------------------------------------------------
	// Hooked functions
	//---------------------------------------------------
	void __fastcall Hooked_ReceivedTicket(void* ecx, void* edx, CMsgSteamDatagramGameServerAuthTicket* ticket);
	HRESULT __stdcall Hooked_Present(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void __cdecl Hooked_nSequence(SourceEngine::CRecvProxyData* pData, SourceEngine::IClientEntity* pStruct, void* pOut);
	void __fastcall Hooked_FrameStageNotify(void* ecx, void* /*edx*/, SourceEngine::ClientFrameStage_t curStage);
	bool __fastcall Hooked_FireEventClientSide(void* ecx, void* /*edx*/, SourceEngine::IGameEvent* event);
	EGCResults __fastcall  Hooked_SendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData);
	EGCResults __fastcall Hooked_RetrieveMessage(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	FileNameHandle_t __fastcall Hooked_FindOrAddFileName(IFileSystem* self, int edx, char const* pFileName);
	bool __fastcall Hooked_LooseFileAllowed(IFileSystem* filesystem, void*);
	int __fastcall Hooked_GetUnverifiedFileHashes(IFileSystem* filesystem, void*, CUnverifiedFileHash* pFiles, int nMaxFiles);
	SourceEngine::MDLHandle_t __fastcall Hooked_FindMDL(void* ecx, void*, const char* path);
	void __fastcall Hooked_LevelInitPostEntity(void* ecx, void* /*edx*/);
	void __fastcall Hooked_LevelShutdown(void* ecx, void* /*edx*/);
	void __fastcall Hooked_ItemPostFrame_ProcessIdleNoAction(C_BaseCombatWeapon* weapon, void* /*edx*/, C_CSPlayer* player);
	
	SourceEngine::CUtlVector<const char*>* __fastcall Hooked_GetTModelsForMap(SourceEngine::IGameTypes* ecx, void* /*edx*/, const char* map_name);
	SourceEngine::CUtlVector<const char*>* __fastcall Hooked_GetCTModelsForMap(SourceEngine::IGameTypes* ecx, void* /*edx*/, const char* map_name);

	//We're gonna replace the main window's WNDPROC for this one to capture user input and pass it down to the GUI
	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void __fastcall Hooked_LockCursor(SourceEngine::ISurface* _this, void* /*edx*/);

	void __cdecl test(const SourceEngine::RecvProp* pProp, void** pOut, void* pData, int objectID);
	void __cdecl test2(SourceEngine::CRecvProxyData* pData, SourceEngine::IClientEntity* pStruct, void* pOut);

	extern WNDPROC g_pOldWindowProc;
	extern HWND g_hWindow;

	extern FILE* Log;
	extern std::unique_ptr<config> g_pConfig;
	extern std::unique_ptr<Skin_loader> g_pSkinDataBase;
	extern int round_time;
	extern bool required_random_skin;
	extern bool g_bWasInitialized;
	extern std::unique_ptr<std::vector<std::string>> localized_strs;
	extern SourceEngine::CreateClientClassFn CreateWearables;
	extern SourceEngine::EconEntityListener* ent_listner;
	extern SourceEngine::CGameEventDescriptor* player_death_descriptor;
	extern SourceEngine::CGameEventDescriptor* player_team_descriptor;
	//extern banner* top_banner;
	//extern banner* bottom_banner;
	using CleanInventoryImageCacheDir_t = void(__stdcall*)();
	extern CleanInventoryImageCacheDir_t g_fnCleanInventoryImageCacheDir;

	extern void* pThisFindHudElement;
	extern void* pThisClearHudWeaponIcon;
	extern Protobuffs ProtoFeatures;
	extern std::unique_ptr<SourceEngine::log> changer_log;
	extern std::unique_ptr < std::unordered_map<fnv::hash, SourceEngine::ConCommandBase*>> g_allCvars;

	extern int original_sequences[20];

	struct client_ctx
	{
		int local_player_index = -1;
		C_CSPlayer* local_player = nullptr;
		SourceEngine::player_info_t player_info;
	};
	extern client_ctx g_ctx;
	extern volatile char* last_function_name;

	extern volatile bool is_pressed_random;


#ifdef CHANGER_PROFILE
	extern __itt_domain* domain;
	extern __itt_string_handle* initialize_work;
	extern __itt_string_handle* FSN_work;
	extern __itt_string_handle* EmitSound_work;
	extern __itt_string_handle* Present_work;
#endif
}

long __stdcall debug_ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);