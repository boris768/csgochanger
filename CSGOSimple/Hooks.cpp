// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <vector>
#include <locale>

#include "Utils.hpp"
#include "Hooks.hpp"
#include "Inverntory.h"
#include "ImGUI/DX9/imgui_impl_dx9.h"
#include "IBaseViewModel.h"
#include "CEconWearable.h"
#include "Stickers.h"
#include "LoggingSystem.h"
#include "ImGUI/imgui_internal.h"
#include "VFTableHook.hpp"
#include "Logger.h"
#include "CustomWinAPI.h"
#include "cross_module_vmt.h"
#include "ImGUI/win32/imgui_impl_win32.h"
#include "ImGUI/DX9/font_resource.h"
#include "fnv_hash.h"
#include "EconEntityListener.h"
#include <filesystem>
#include "Crates.h"
//#define _PREMIUM_VERSION
#ifndef _PREMIUM_VERSION
#include "banner_downloader.h"
#include "banner.h"
#endif
#ifdef CHANGER_PROFILE
#include <ittnotify.h>
#endif
#include "ISteamHTTP.h"
#include "Menu.h"
#include "XorStr.hpp"
#include "minhook/include/MinHook.h"

#ifdef _DEBUG
void _Debug(const wchar_t *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vwprintf(msg, args);
    va_end(args);
    wprintf(L"%s", L"\n");
}
#endif

using namespace std;

SourceEngine::ConCommand* dodge_challenger;
_offsets_c_econitemview* offsets_c_econitemview;
netvars_C_BaseCombatWeapon* netvars_c_base_combat_weapon;
netvars_C_CSPlayer* netvars_c_cs_player;
netvars_CPlayerResource* netvars_c_player_resource;
netvars_C_BaseViewModel* netvars_c_base_view_model;
netvars_CSGameRulesProxy* netvars_csgame_rules_proxy;
netvars_CSTeam* netvars_cs_team;
SourceEngine::offsets_CClientState* offsets_c_client_state;
SourceEngine::offsets_CCStrike15ItemSchema* SourceEngine::offsets_cc_strike15_item_schema;

sessionInfo* global_session_info = nullptr;

namespace HooksIndexes
{
	constexpr unsigned int Reset = 16;
	constexpr unsigned int FrameStageNotify = 37;
	constexpr unsigned int FireEventClientSide = 9;
	//constexpr unsigned int PlaySound = 82;
	constexpr unsigned int SendMessage = 0;
	constexpr unsigned int RetrieveMessage = 2;
	constexpr unsigned int FindOrAddFileName = 41;
	constexpr unsigned int LockCursor = 67;
	constexpr unsigned int LevelInitPostEntity = 6;
	constexpr unsigned int LevelShutdown = 7;
}

namespace Hooks
{
	DWORD_PTR* g_pOverlay_PresentHook;
	vf_hook* g_pD3DDevice9Hook;
	vf_hook* g_pMatSurfaceHook;
	vf_hook* g_pEventManagerHook;
	vf_hook* g_pDataCacheHook;
	vf_hook* g_pGameCoordinatorHook;
	vf_hook* g_pFileSystemHook;
	vf_hook* g_pCustomManagerHook;
	vf_hook* g_pGameTypesHook;
	cross_module_vmt* g_pClientHook;

	//unique_ptr<DrawManager> g_pRenderer = nullptr;
	unique_ptr<config> g_pConfig = nullptr;
	unique_ptr<Skin_loader> g_pSkinDataBase = nullptr;

	Present_t g_fnOriginalPresent = nullptr;
	EndScene_t g_fnOriginalEndScene = nullptr;
	Reset_t g_fnOriginalReset = nullptr;
	PlaySound_t g_fnOriginalPlaySound = nullptr;
	ReceivedTicket_t g_fnOriginalReceivedTicket = nullptr;
	FrameStageNotify_t g_fnOriginalFrameStageNotify = nullptr;
	SourceEngine::RecvVarProxyFn g_fnOriginalnSequence = nullptr;
	FireEventClientSide_t g_fnOriginalFireEventClientSide = nullptr;
	PaintTraverse_t g_fnOriginalPaintTraverse = nullptr;
	SendMessage_t g_fnOriginalSendMessage = nullptr;
	RetrieveMessage_t g_fnOriginalRetrieveMessage = nullptr;
	FindOrAddFilename_t g_fnOriginalFindOrAddFilename = nullptr;
	FindMDL_t g_fnOriginalFindMDL = nullptr;
	SourceEngine::RecvVarProxyFn g_fnOriginalSequence = nullptr;
	SourceEngine::RecvVarProxyFn g_fnOriginalMusicIDInTable = nullptr;
	SourceEngine::DataTableRecvVarProxyFn g_fnOriginalMusicID = nullptr;
	LockCursor_t g_fnOriginalLockCursor = nullptr;
	GetModelsForMap_t g_fnOriginalGetTModelsForMap_t = nullptr;
	GetModelsForMap_t g_fnOriginalGetCTModelsForMap_t = nullptr;
	LevelInitPostEntity_t g_fnOriginalLevelInitPostEntity = nullptr;
	LevelShutdown_t g_fnOriginalLevelShutdown = nullptr;
	ItemPostFrame_ProcessIdleNoAction_t g_fnItemPostFrame_ProcessIdleNoAction = nullptr;

	FindHudElement_t FindHudElement = nullptr;
	ClearHudWeapon_t ClearHudWeapon = nullptr;

	WNDPROC g_pOldWindowProc = nullptr; //Old WNDPROC pointer
	HWND g_hWindow = nullptr; //Handle to the CSGO window

	bool g_bWasInitialized = false;

	SourceEngine::EconEntityListener *ent_listner;

	SourceEngine::CreateClientClassFn CreateWearables = nullptr;
	SourceEngine::CreateClientClassFn CreateCRopeKeyframe = nullptr;
	CleanInventoryImageCacheDir_t g_fnCleanInventoryImageCacheDir = nullptr;

	SetLocalPlayerReady_t IsReady;
	FILE* in;
	FILE* out;
	FILE* err;
	SourceEngine::LoggingSystem valve_logging_system;
	SourceEngine::CGameEventDescriptor* player_death_descriptor = nullptr;
	SourceEngine::CGameEventDescriptor* player_team_descriptor = nullptr;
	changer_match_events_sink* match_events_listner = nullptr;
	void* pThisFindHudElement = nullptr;
	void* pThisClearHudWeaponIcon = nullptr;

	std::unique_ptr<SourceEngine::log> changer_log;
	std::unique_ptr < std::unordered_map<fnv::hash, SourceEngine::ConCommandBase*>> g_allCvars;
	
	client_ctx g_ctx;
	volatile char* last_function_name;
	//banner* top_banner;
	//banner* bottom_banner;

	inline SourceEngine::CreateClientClassFn CreateClientClass(const char* name)
	{
		SourceEngine::ClientClass* pClass = SourceEngine::interfaces.Client()->GetAllClasses();
		while (pClass)
		{
			if (strcmp(pClass->m_pNetworkName, name) == 0)
			{
				return pClass->m_pCreateFn;
			}
			pClass = pClass->m_pNext;
		}
		return nullptr;
	}

	std::unique_ptr<vector<string>> localized_strs;

	string utf8_encode(const wstring& wstr)
	{
		if (wstr.empty()) return string();
		const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr,
		                                            nullptr);
		string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);
		return strTo;
	}


#include "IMaterialSystem.h"
	class CCustomMaterialManager;

	using GetVMTKeyValues_t = bool(__thiscall*)(CCustomMaterialManager* manager, const char *name, SourceEngine::KeyValues** kv);
	GetVMTKeyValues_t fnGetVMTKeyValues = nullptr;
	bool __fastcall Hooked_GetVMTKeyValues(CCustomMaterialManager* manager, void*, const char *name, SourceEngine::KeyValues** kv)
	{
		const auto returned = fnGetVMTKeyValues(manager, name, kv);
		return returned;
	}

	void __cdecl HTTPRequestCallback_report_test(CHTTPRequest* request, HTTPRequestCompleted*completed)
	{
		//auto http = SourceEngine::interfaces.SteamHTTP();
		//if(completed->m_bRequestSuccessful)
		//{
		//	changer_log->msg(std::to_string(completed->m_ulContextValue));
		//	changer_log->msg("\n");
		//	uint8_t* data = new uint8_t[completed->m_unBodySize+1];
		//	memset(data, 0, completed->m_unBodySize + 1);
		//	http->GetHTTPResponseBodyData(completed->m_hRequest, data, completed->m_unBodySize);
		//	changer_log->msg(reinterpret_cast<char*>(data));
		//	changer_log->msg("\n");
		//}
		//else
		//{
		//	changer_log->msg("request failed, code:");
		//	changer_log->msg(std::to_string(completed->m_eStatusCode));
		//	changer_log->msg("\n");
		//}
		delete request;
	}
	
	void __cdecl dodge_challenger_callback()
	{
		Options::menu.main_window = !Options::menu.main_window;
		Options::menu.was_opened = false;
	}
#ifdef CHANGER_PROFILE
	__itt_domain* domain = nullptr;
	__itt_string_handle* initialize_work = nullptr;
	__itt_string_handle* FSN_work = nullptr;
	__itt_string_handle* EmitSound_work = nullptr;
	__itt_string_handle* Present_work = nullptr;
#endif

	void __cdecl Initialize(void* arg)
	{
#ifdef CHANGER_PROFILE
		domain = __itt_domain_create(L"CSGO Changer");
		initialize_work = __itt_string_handle_create(L"initialize_work");
		FSN_work = __itt_string_handle_create(L"FrameStageNotify_work");
		EmitSound_work = __itt_string_handle_create(L"EmitSound_work");
		Present_work = __itt_string_handle_create(L"Present_work");

		__itt_task_begin(domain, __itt_null, __itt_null, initialize_work);
#endif
		g_pWinApi = std::make_unique<manual_winapi>();
		struct data_QVM
		{
			unsigned long long target_pid = 0;
			unsigned long long target_base_address = 0;
			unsigned long long target_size = 0;
		} qvm_target;

		qvm_target.target_pid = GetCurrentProcessId();
		qvm_target.target_base_address = reinterpret_cast<unsigned long long>(&__ImageBase);
		qvm_target.target_size = static_cast<unsigned long long>(Utils::get_image_size(&__ImageBase));

		
		auto status = /*0;//*/
			g_pWinApi->dwNtReadVirtualMemory(
				reinterpret_cast<HANDLE>(0xFACE09), 
				nullptr, 
				reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&qvm_target), 
				sizeof(MEMORY_BASIC_INFORMATION), 
				nullptr);
		{
			while (g_hWindow == nullptr)
			{
				g_hWindow = g_pWinApi->dwFindWindowA(XorStr("Valve001"), nullptr);
				g_pWinApi->dwSwitchToThread();	
				g_pWinApi->dwSleep(50);
			}

			MODULEINFO _serverbrowser;
			memset(&_serverbrowser, 0, sizeof(MODULEINFO));
			while (!_serverbrowser.lpBaseOfDll || !_serverbrowser.EntryPoint || !_serverbrowser.SizeOfImage)
			{
				spoofed_GetModuleInfo(FNV(L"serverbrowser.dll"), _serverbrowser);
				g_pWinApi->dwSwitchToThread();
				g_pWinApi->dwSleep(50);
			}
		}
		
		/*SetUnhandledExceptionFilter(debug_ExceptionFilter);*/
		
		changer_log = std::make_unique<SourceEngine::log>(SourceEngine::LS_MESSAGE);

#ifdef _DEBUG
		g_pWinApi->dwAllocConsole();
		out = freopen("CONOUT$", "w", stdout);
		

		//auto itemschema = SourceEngine::interfaces.ItemSchema();
#endif
		changer_log->msg("factory::init\n");
		
		SourceEngine::factory::init();
		if (status != 0)
		{
			//driver may be not initialized hook, but first call was should trigger hook init, call it again

			//status = g_pWinApi->dwNtReadVirtualMemory((HANDLE)0xFACE09, nullptr, (PMEMORY_BASIC_INFORMATION)(&qvm_target), sizeof(MEMORY_BASIC_INFORMATION), nullptr);
			if (status != 0)
			{
				//MESSAGEBOX HERE [NO-BYPASS]
				SourceEngine::interfaces.GameUI()->message_box(XorStr("WARNING"),
					XorStr("CSGO Changer started successfully, but anti-cheat bypass is not activated."
						"Loading will continue, but at the moment you may be banned by anti-cheat.\n"
						"Contact support for this issue with the tag [NO-BYPASS]"));
			}
		}

		auto* const steamapi = SourceEngine::memory.steamapi().lpBaseOfDll;
		SourceEngine::hSteamUser = reinterpret_cast<HSteamUser(__cdecl*)()>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(steamapi), XorStr("SteamAPI_GetHSteamUser")))();
		SourceEngine::hSteamPipe = reinterpret_cast<HSteamPipe(__cdecl*)()>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(steamapi), XorStr("SteamAPI_GetHSteamPipe")))();
		auto* user = SourceEngine::interfaces.SteamClient()->GetISteamUser(SourceEngine::hSteamUser, SourceEngine::hSteamPipe, XorStr("SteamUser021"));
		auto steamid = user->GetSteamID();
		//changer_log->msg(std::to_string(steamid.m_unAll64Bits));
		//changer_log->msg("\n");
#ifndef _PREMIUM_VERSION
		//banner_info pos1, pos2;
		//AdvertiseTool::Banners::GetBannersInfoFromServer(pos1, pos2);
		//if (pos1.position > 0)
		//{
		//	top_banner = pos1.GetBanner();
		//	Options::menu.menuSize.y += 75;
		//}
		//if (pos2.position > 0)
		//{
		//	bottom_banner = pos2.GetBanner();
		//	Options::menu.menuSize.y += 80;
		//}
#endif

#ifdef _DEBUG
		g_fnCleanInventoryImageCacheDir = reinterpret_cast<CleanInventoryImageCacheDir_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 81 EC ? ? ? ? 80 3D ? ? ? ? ? 56 0F 85")));

		//NetvarManager::Instance()->Dump("netvars_dump.txt");
		//InjectItem(7, 233, "", false, false, 0, 0, 0.f, SourceEngine::ItemQuality::ITEM_QUALITY_DEVELOPER);
#endif

		changer_log->msg(XorStr("inital signatures\n"));

		const unsigned dwDevice = **reinterpret_cast<uint32_t **>(Utils::FindSignature(SourceEngine::memory.shaderapidx9(), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
		g_pOverlay_PresentHook = *reinterpret_cast<PDWORD*>(Utils::FindSignature(SourceEngine::memory.gameoverlayrenderer(), XorStr("FF 15 ? ? ? ? 8B F8 85 DB")) + 0x2);
		FindHudElement = reinterpret_cast<FindHudElement_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
		ClearHudWeapon = reinterpret_cast<ClearHudWeapon_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C")));
		pThisFindHudElement = *reinterpret_cast<DWORD**>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89 46 24")) + 1);
		pThisClearHudWeaponIcon = *reinterpret_cast<DWORD**>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC")));
		IsReady = reinterpret_cast<SetLocalPlayerReady_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));
		changer_log->msg(XorStr("allCvars\n"));
		
		auto* cvar_iterator = SourceEngine::interfaces.CVar()->FactoryInternalIterator();
		g_allCvars = std::make_unique<std::unordered_map<fnv::hash, SourceEngine::ConCommandBase*>>();
		for (cvar_iterator->SetFirst(); cvar_iterator->IsValid(); cvar_iterator->Next())
		{
			SourceEngine::ConCommandBase* cmd = cvar_iterator->Get();
			g_allCvars->insert_or_assign(fnv::hash_runtime(cmd->GetName()), cmd);
		}
#ifdef _DEBUG
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("econ_debug_loadout_ui")))->SetFlags(FCVAR_NONE);
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("econ_debug_loadout_ui")))->SetValue(1);
		
		//auto cvar = SourceEngine::interfaces.CVar();
		//SourceEngine::ConVar* mat_verbose_texture_gen = cvar->FindVar("mat_verbose_texture_gen");

		auto* cl_threaded_bone_setup = static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("cl_threaded_bone_setup")));
		cl_threaded_bone_setup->SetFlags(FCVAR_NONE);
		
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("mat_verbose_texture_gen")))->SetValue(1);

		//SourceEngine::ConVar* cl_showanimstate = cvar->FindVar("cl_showanimstate");
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("cl_showanimstate")))->SetFlags(FCVAR_NONE);
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("cl_showanimstate")))->SetValue(1);
		//cl_showanimstate->SetFlags(FCVAR_NONE);
		//cl_showanimstate->SetValue(1);

		//cl_showanimstate_log
		//SourceEngine::ConVar* cl_showanimstate_log = cvar->FindVar("cl_showanimstate_log");
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("cl_showanimstate_log")))->SetFlags(FCVAR_NONE);
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("cl_showanimstate_log")))->SetValue(3);
		//cl_showanimstate_log->SetFlags(FCVAR_NONE);
		//cl_showanimstate_log->SetValue(3);

		//r_debug_sequencesets
		//SourceEngine::ConVar* r_debug_sequencesets = SourceEngine::interfaces.CVar()->FindVar("r_debug_sequencesets");		
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("r_debug_sequencesets")))->SetFlags(FCVAR_NONE);
		static_cast<SourceEngine::ConVar*>(g_allCvars->at(FNV("r_debug_sequencesets")))->SetValue(1);
		//r_debug_sequencesets->SetFlags(FCVAR_NONE);
		//r_debug_sequencesets->SetValue(1);

#endif

		changer_log->msg(XorStr("netvars_offsets\n"));
		CreateWearables = CreateClientClass(XorStr("CEconWearable"));
		debug(L"CreateWearables: 0x%08lx", CreateWearables);
		changer_log->msg(XorStr("netvars_offsets: offsets_keyvalues\n"));
		SourceEngine::offsets_keyvalues = new SourceEngine::_offsets_keyvalues();
		changer_log->msg(XorStr("netvars_offsets: base_combat_weapon\n"));
		netvars_c_base_combat_weapon = new netvars_C_BaseCombatWeapon();
		changer_log->msg(XorStr("netvars_offsets: cs_player\n"));
		netvars_c_cs_player = new netvars_C_CSPlayer();
		changer_log->msg(XorStr("netvars_offsets: c_player_resource\n"));
		netvars_c_player_resource = new netvars_CPlayerResource();
		changer_log->msg(XorStr("netvars_offsets: base_view_model\n"));
		netvars_c_base_view_model = new netvars_C_BaseViewModel();
		changer_log->msg(XorStr("netvars_offsets: csgame_rules_proxy\n"));
		netvars_csgame_rules_proxy = new netvars_CSGameRulesProxy();
		changer_log->msg(XorStr("netvars_offsets: cs_team\n"));
		netvars_cs_team = new netvars_CSTeam();
		changer_log->msg(XorStr("netvars_offsets: econitemview\n"));
		offsets_c_econitemview = new _offsets_c_econitemview();
		changer_log->msg(XorStr("netvars_offsets: c_client_state\n"));
		offsets_c_client_state = new SourceEngine::offsets_CClientState();
		changer_log->msg(XorStr("netvars_offsets: stickers\n"));
		offsets_stickers = new _offsets_stickers();
		changer_log->msg(XorStr("netvars_offsets: cc_strike15_item_schema\n"));
		SourceEngine::offsets_cc_strike15_item_schema = new SourceEngine::offsets_CCStrike15ItemSchema();
		offsets_itemschema = new offsets_ItemSchema();
		
		g_fnCleanInventoryImageCacheDir = reinterpret_cast<CleanInventoryImageCacheDir_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 81 EC ? ? ? ? 80 3D ? ? ? ? ? 56 0F 85")));
		
		player_death_descriptor = SourceEngine::interfaces.GameEventManager()->GetEventDescriptor(XorStr("player_death"));
		player_team_descriptor = SourceEngine::interfaces.GameEventManager()->GetEventDescriptor(XorStr("player_team"));
		auto* memalloc = SourceEngine::interfaces.MemAlloc();
		auto* op = new(memalloc->Alloc(sizeof(SourceEngine::CGameEventCallback)))SourceEngine::CGameEventCallback;
		player_death_descriptor->listeners.AddToHead(op);
		op->m_nListenerType = SourceEngine::CGameEventManager::CLIENTSIDE;
		op->m_pCallback = new(memalloc->Alloc(sizeof(SourceEngine::player_death_listener))) SourceEngine::player_death_listener;

		op = new(memalloc->Alloc(sizeof(SourceEngine::CGameEventCallback)))SourceEngine::CGameEventCallback;
		player_team_descriptor->listeners.AddToHead(op);
		op->m_nListenerType = SourceEngine::CGameEventManager::CLIENTSIDE;
		op->m_pCallback = new(memalloc->Alloc(sizeof(SourceEngine::player_team_listener_pre_all))) SourceEngine::player_team_listener_pre_all;

		op = new(memalloc->Alloc(sizeof(SourceEngine::CGameEventCallback)))SourceEngine::CGameEventCallback;
		player_team_descriptor->listeners.AddToTail(op);
		op->m_nListenerType = SourceEngine::CGameEventManager::CLIENTSIDE;
		op->m_pCallback = new(memalloc->Alloc(sizeof(SourceEngine::player_team_listener_post_all))) SourceEngine::player_team_listener_post_all;

		match_events_listner = new changer_match_events_sink();
		
		debug(L"netvars stored, check dwKeyValuesSystem: 0x%08lx", SourceEngine::offsets_keyvalues->dwKeyValuesSystem);
		
		changer_log->msg(XorStr("localize\n"));

		vector<const char*> localize_strs = {
			"#SkinChanger_Team",
			"#SkinChanger_Weapons",
			"#SkinChanger_Skins",
			"#SkinChanger_Name",
			"#SkinChanger_Wear",
			"#SkinChanger_Seed",
			"#SkinChanger_StatTrak",
			"#SkinChanger_Kills",
			"#SkinChanger_Quality",
			"#SkinChanger_Stickers",
			"#SkinChanger_Sticker_slot",
			"#SkinChanger_Scale",
			"#SkinChanger_Rotation",
			"#SkinChanger_Knifes",
			"#SkinChanger_Wearables",
			"#SkinChanger_Save_config",
			"#SkinChanger_Load_config",
			"#SkinChanger_Copy_T_config_to_CT",
			"#SkinChanger_Copy_CT_config_to_T",
			"#SkinChanger_AutoAccept",
			"#SkinChanger_Fix_ViewModel_after_changing_team",
			"#SkinChanger_SuperStattrack",
			"#SkinChanger_Sky",
			"#SkinChanger_Sticker_filter",
			"#SkinChanger_Reset",
			"#SkinChanger_RandomSkin",
			"#SkinChanger_Randomize",
			"#SkinChanger_Random_skin_key",
			"#SkinChanger_Reload_inventory",
			"#SkinChanger_OldSounds",
			"#SkinChanger_OldSounds_NotInGameWarning",
			"#SkinChanger_OldSounds_PrecacheSounds",
			"#SkinChanger_OldSounds_NotDownloadedWarning",
			"#SkinChanger_SuperStattrack_Description",
			"#SkinChanger_RandomSkin_Description",
			"#SkinChanger_Randomize_Description",
			"#SkinChanger_Fix_ViewModel_after_changing_team_Description",
			"#SkinChanger_Menu_key",
			"#SkinChanger_AutoSave_if_changed"
		};
		
		CLocalize* localize = SourceEngine::interfaces.LocalizeSystem();
		bool localize_loaded = false;
		localized_strs = std::make_unique<std::vector<std::string>>();
		for(int i = 0; i < SkinChanger_sizeof; ++i)
		{
			localized_strs->emplace_back(XorStr("ERROR: no localize"));
		}
		if (!localize->AddFile(XorStr("skinchanger_%language%.txt")))
		{
			changer_log->warning(XorStr("localize file skinchanger_language.txt not found\n"));
			debug(L"localize file skinchanger_language.txt not found %d", 1);
			if (!localize->AddFile(XorStr("skinchanger_english.txt")))
			{
				changer_log->warning(XorStr("localize file skinchanger_english.txt not found\n"));
				changer_log->warning(XorStr("using localize from memory\n"));
				debug(L"localize file skinchanger_english.txt not found, localize from memory %d", 2);

				localized_strs->at(SkinChanger_Team) = XorStr("Team");
				localized_strs->at(SkinChanger_Weapons) = XorStr("Weapons");
				localized_strs->at(SkinChanger_Skins) = XorStr("Skins");
				localized_strs->at(SkinChanger_Name) = XorStr("Name");
				localized_strs->at(SkinChanger_Wear) = XorStr("Wear");
				localized_strs->at(SkinChanger_Seed) = XorStr("Seed");
				localized_strs->at(SkinChanger_StatTrak) = XorStr("StatTrak");
				localized_strs->at(SkinChanger_Kills) = XorStr("Kills");
				localized_strs->at(SkinChanger_Quality) = XorStr("Quality");
				localized_strs->at(SkinChanger_Stickers) = XorStr("Stickers");
				localized_strs->at(SkinChanger_Sticker_slot) = XorStr("Sticker slot");
				localized_strs->at(SkinChanger_Scale) = XorStr("Scale");
				localized_strs->at(SkinChanger_Rotation) = XorStr("Rotation");
				localized_strs->at(SkinChanger_Knifes) = XorStr("Knifes");
				localized_strs->at(SkinChanger_Wearables) = XorStr("Wearables");
				localized_strs->at(SkinChanger_Save_config) = XorStr("Save config");
				localized_strs->at(SkinChanger_Load_config) = XorStr("Load config");
				localized_strs->at(SkinChanger_Copy_T_config_to_CT) = XorStr("Copy T config to CT");
				localized_strs->at(SkinChanger_Copy_CT_config_to_T) = XorStr("Copy CT config to T");
				localized_strs->at(SkinChanger_AutoAccept) = XorStr("AutoAccept");
				localized_strs->at(SkinChanger_Fix_ViewModel_after_changing_team) = XorStr("Fix ViewModel after changing team");
				localized_strs->at(SkinChanger_SuperStattrack) = XorStr("SuperStattrack");
				localized_strs->at(SkinChanger_Sky) = XorStr("Sky");
				localized_strs->at(SkinChanger_Sticker_filter) = XorStr("Sticker filter");
				localized_strs->at(SkinChanger_Reset) = XorStr("Reset");
				localized_strs->at(SkinChanger_RandomSkin) = XorStr("Random Skin");
				localized_strs->at(SkinChanger_Randomize) = XorStr("Random config");
				localized_strs->at(SkinChanger_Random_skin_key) = XorStr("Random skin key");
				localized_strs->at(SkinChanger_Reload_inventory) = XorStr("Reload inventory");
				localized_strs->at(SkinChanger_OldSounds) = XorStr("Old sounds");
				localized_strs->at(SkinChanger_OldSounds_NotInGameWarning) = XorStr("Can not be changed while playing!\nAll changes will be applied after the game");
				localized_strs->at(SkinChanger_OldSounds_PrecacheSounds) = XorStr("Precache sounds (only one time)");
				localized_strs->at(SkinChanger_OldSounds_NotDownloadedWarning) = XorStr("Sounds are not downloaded from the client!");
				localized_strs->at(SkinChanger_SuperStattrack_Description) = XorStr("Every time, when stattrack count kill, this feature will change your skin/knife");

				localized_strs->at(SkinChanger_RandomSkin_Description) = XorStr("This function will change skin on current weapon to random weapon on key pressing");
				localized_strs->at(SkinChanger_Randomize_Description) = XorStr("Overwrites all skin settings with random");
				localized_strs->at(SkinChanger_Fix_ViewModel_after_changing_team_Description) = XorStr("The game does not change gloves when changing teams. This function fixes this bug");
				localized_strs->at(SkinChanger_Menu_key) = XorStr("Menu key bind");
				localized_strs->at(SkinChanger_AutoSave_if_changed) = XorStr("Save config at every change");
				localized_strs->at(SkinChanger_Custom_Players) = XorStr("Custom players");
				localized_strs->at(SkinChanger_Custom_Players_enable) = XorStr("Enable");
			}
			else
			{
				changer_log->msg(XorStr("localize file skinchanger_english.txt loaded\n"));
				debug(L"localize file skinchanger_english.txt loaded %d", 2);
				localize_loaded = true;
			}
		}
		else
		{
			changer_log->msg(XorStr("localize file skinchanger_language.txt loaded\n"));
			debug(L"localize file skinchanger_language.txt loaded %d", 1);
			localize_loaded = true;
		}

		if (localize_loaded)
		{
			for (size_t i = 0; i < localize_strs.size(); ++i)
			{
				debug(L"localizing localized_strs[%d] : %s", i, localize_strs.at(i));
				localized_strs->at(i) = utf8_encode(localize->FindSafe2(localize_strs.at(i)));
				debug(L"localized as : %s", localized_strs->at(i).c_str());
			}
		}

		//SourceEngine::CCStrike15ItemSchema* itemschema = (SourceEngine::CCStrike15ItemSchema*)((uintptr_t)SourceEngine::interfaces.ItemSchema() - 4);
		//SourceEngine::CUtlHashMapLarge<int, SourceEngine::CPaintKit*>* pks = itemschema->GetPaintKits();
		//auto pk_344 = pks->Find(344);
		changer_log->msg(XorStr("skin db\n"));
		g_pSkinDataBase = make_unique<Skin_loader>();
		debug(L"skins loaded %d", 3);

		//stat_events = new SourceEngine::EventListener({
		//	XorStr("round_start"),
		//	XorStr("round_end"),
		//	XorStr("bomb_planted"), //11
		//	XorStr("bomb_defused"),
		//	XorStr("hostage_rescued"),
		//	XorStr("weapon_fire"), //11
		//	XorStr("game_newmap"),
		//	XorStr("player_death"), //11
		//	XorStr("other_death"), //11
		//	XorStr("item_purchase"),
		//	XorStr("player_hurt"), //11 - candidate to get position
		//	XorStr("round_mvp"),
		//	XorStr("cs_game_disconnected"),
		//	XorStr("server_spawn"),
		//});

		
		changer_log->msg(XorStr("engine\n"));
		const auto engine = SourceEngine::memory.engine();
		auto* const g_ClientDLL_engine = *reinterpret_cast<void**>(Utils::FindSignature(engine, XorStr("83 3D ? ? ? ? 00 8B 3D")) + 2);

		changer_log->msg(XorStr("setup tables: client\n"));
		//g_pClientHook.setup(SourceEngine::memory.client().lpBaseOfDll, SourceEngine::interfaces.Client());
		g_pClientHook = new cross_module_vmt(engine.lpBaseOfDll, g_ClientDLL_engine);

		changer_log->msg(XorStr("setup tables: dx\n"));
		g_pD3DDevice9Hook = new vf_hook();
		g_pD3DDevice9Hook->setup(SourceEngine::memory.shaderapidx9().lpBaseOfDll, reinterpret_cast<void*>(dwDevice));
		
		changer_log->msg(XorStr("setup tables: MatSurface\n"));
		g_pMatSurfaceHook = new vf_hook();
		g_pMatSurfaceHook->setup(SourceEngine::memory.vguimatsurface().lpBaseOfDll, SourceEngine::interfaces.MatSurface());
		
		changer_log->msg(XorStr("setup tables: EventManager\n"));
		g_pEventManagerHook = new vf_hook();
		g_pEventManagerHook->setup(engine.lpBaseOfDll, SourceEngine::interfaces.GameEventManager());
		
		g_pGameCoordinatorHook = new vf_hook();
		g_pGameCoordinatorHook->setup(SourceEngine::memory.steamapi().lpBaseOfDll, SourceEngine::interfaces.GameCoordinator());
		
		changer_log->msg(XorStr("setup tables: FS\n"));
		g_pFileSystemHook = new vf_hook();
		g_pFileSystemHook->setup(SourceEngine::memory.filesystem_stdio().lpBaseOfDll, SourceEngine::interfaces.FileSystem());

		changer_log->msg(XorStr("setup tables: Overlay::Present\n"));
		g_fnOriginalPresent = reinterpret_cast<Present_t>(*g_pOverlay_PresentHook);

		changer_log->msg(XorStr("setup tables: GameResources\n"));//hehe
		g_pGameTypesHook = new vf_hook();
		g_pGameTypesHook->setup(SourceEngine::memory.matchmaking().lpBaseOfDll, SourceEngine::interfaces.GameTypes());

		// Hooks
#ifdef _PREMIUM_VERSION
		g_pGameTypesHook->hook_index(Hooked_GetTModelsForMap, 37);
		g_fnOriginalGetTModelsForMap_t = g_pGameTypesHook->get_original<GetModelsForMap_t>(37);
		g_pGameTypesHook->hook_index(Hooked_GetCTModelsForMap, 38);
		g_fnOriginalGetCTModelsForMap_t = g_pGameTypesHook->get_original<GetModelsForMap_t>(38);
#endif
		
		changer_log->msg(XorStr("setup hook: WNDPROC\n"));
		g_pOldWindowProc = reinterpret_cast<WNDPROC>(g_pWinApi->dwSetWindowLongW(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooked_WndProc)));
		
		changer_log->msg(XorStr("setup hook: EntityListener\n"));
		ent_listner = new SourceEngine::EconEntityListener;

		changer_log->msg(XorStr("setup hook: dx::Reset\n"));
		g_pD3DDevice9Hook->hook_index(Hooked_Reset, HooksIndexes::Reset);
		g_fnOriginalReset = g_pD3DDevice9Hook->get_original<Reset_t>(HooksIndexes::Reset);

		changer_log->msg(XorStr("setup hook: Overlay::Present\n"));
		*g_pOverlay_PresentHook = reinterpret_cast<DWORD_PTR>(&Hooked_Present); 

		changer_log->msg(XorStr("setup hook: RecvProp::m_nSequence\n"));
		SourceEngine::RecvProp* sequence_prop = netvar_manager::get().get_prop(FNV("CBaseViewModel->m_nSequence"));
		g_fnOriginalnSequence = sequence_prop->m_ProxyFn;
		sequence_prop->m_ProxyFn = Hooked_nSequence;

		//g_pClientHook.hook_index(Hooked_FrameStageNotify, HooksIndexes::FrameStageNotify);
		changer_log->msg(XorStr("setup hook: Client::FrameStageNotify\n"));
		g_pClientHook->hook(Hooked_FrameStageNotify, HooksIndexes::FrameStageNotify);
		g_fnOriginalFrameStageNotify = g_pClientHook->get_original<FrameStageNotify_t>(HooksIndexes::FrameStageNotify);

		g_pClientHook->hook(Hooked_LevelInitPostEntity, HooksIndexes::LevelInitPostEntity);
		g_fnOriginalLevelInitPostEntity = g_pClientHook->get_original<LevelInitPostEntity_t>(HooksIndexes::LevelInitPostEntity);
		
		g_pClientHook->hook(Hooked_LevelShutdown, HooksIndexes::LevelShutdown);
		g_fnOriginalLevelShutdown = g_pClientHook->get_original<LevelShutdown_t>(HooksIndexes::LevelShutdown);
		
		//changer_log->msg(XorStr("setup hook: EventManager::FireEventClientSide\n"));
		//g_pEventManagerHook->hook_index(Hooked_FireEventClientSide, HooksIndexes::FireEventClientSide);
		//g_fnOriginalFireEventClientSide = g_pEventManagerHook->get_original<FireEventClientSide_t>(HooksIndexes::FireEventClientSide);

		changer_log->msg(XorStr("setup hook: FS::FindOrAddFileName\n"));
		g_pFileSystemHook->hook_index<FindOrAddFilename_t>(Hooked_FindOrAddFileName, HooksIndexes::FindOrAddFileName);
		g_fnOriginalFindOrAddFilename = g_pFileSystemHook->get_original<FindOrAddFilename_t>(HooksIndexes::FindOrAddFileName);
		g_pFileSystemHook->hook_index<LooseFileAllowed_t>(Hooked_LooseFileAllowed, 128);
		g_pFileSystemHook->hook_index<GetUnverifiedFileHashes_t>(Hooked_GetUnverifiedFileHashes, 101);

		changer_log->msg(XorStr("setup hook: MatSurface::LockCursor\n"));
		g_pMatSurfaceHook->hook_index(Hooked_LockCursor, HooksIndexes::LockCursor);
		g_fnOriginalLockCursor = g_pMatSurfaceHook->get_original<LockCursor_t>(HooksIndexes::LockCursor);

		//void* target = (void*)Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 56 57 8B F9 E8 ? ? ? ? 8B 45"));
		//
		//if (MH_Initialize() == MH_OK)
		//{
		//	if (MH_CreateHook(target, &Hooked_ItemPostFrame_ProcessIdleNoAction,
		//		reinterpret_cast<LPVOID*>(&g_fnItemPostFrame_ProcessIdleNoAction)) == MH_OK)
		//	{
		//		if (MH_EnableHook(target) != MH_OK)
		//		{
		//			changer_log->msg("mh_er");
		//		}
		//		else
		//		{
		//			changer_log->msg(XorStr("setup hook: ItemPost\n"));
		//		}
		//	}
		//}
		
		//g_pCustomManagerHook.setup(SourceEngine::memory.materialsystem().lpBaseOfDll, (void*)cman);
		//g_pCustomManagerHook.hook_index(Hooked_GetVMTKeyValues, 3);
		//fnGetVMTKeyValues = g_pCustomManagerHook.get_original<GetVMTKeyValues_t>(3);
			  
		SourceEngine::CPaintKit* dragon_lore = SourceEngine::interfaces.ItemSchema()->GetPaintKitDefinitionByName("cu_medieval_dragon_awp");

		//dragon_lore->rarity = 7;

		changer_log->msg(XorStr("load skins: loading request\n"));
		InjectStickerkit(XorStr("SkinChanger/stickers/aperture"), 0x7C5ECF12);
		std::vector<paint_kit_inject_request> custom_paint_kit_inject_request;
		custom_paint_kit_inject_request.reserve(100);
		auto crypto = XorStr_("SkinChanger/sk_ak47_asiimov_blue");
		custom_paint_kit_inject_request.emplace_back(crypto.crypt_get(), 0xEC4B6552, 0xC41A0BE1);
		auto crypto2 = XorStr_("SkinChanger/sk_ak47_asiimov_cyan");
		custom_paint_kit_inject_request.emplace_back(crypto2.crypt_get(), 0x52AE65A3, 0x1990AF59);
		auto crypto3 = XorStr_("SkinChanger/sk_ak47_asiimov_green");
		custom_paint_kit_inject_request.emplace_back(crypto3.crypt_get(), 0xADDC2B5C, 0x78CA5395);
		auto crypto4 = XorStr_("SkinChanger/sk_ak47_asiimov_orange");
		custom_paint_kit_inject_request.emplace_back(crypto4.crypt_get(), 0xE22057E7, 0x8C851B40);
		auto crypto5 = XorStr_("SkinChanger/sk_ak47_asiimov_pink");
		custom_paint_kit_inject_request.emplace_back(crypto5.crypt_get(), 0x303DFC6F, 0xDFB74B41);
		auto crypto6 = XorStr_("SkinChanger/sk_ak47_asiimov_red");
		custom_paint_kit_inject_request.emplace_back(crypto6.crypt_get(), 0xF01395F8, 0xD4FDAD97);
		auto crypto7 = XorStr_("SkinChanger/sk_ak47_asiimov_yellow");
		custom_paint_kit_inject_request.emplace_back(crypto7.crypt_get(), 0x5B346BE2, 0xE3F06838);

		auto crypto8 = XorStr_("SkinChanger/sk_ak47_isaac");
		custom_paint_kit_inject_request.emplace_back(crypto8.crypt_get(), 0xEC60195F, 0xA4CF41D2);
		auto crypto9 = XorStr_("SkinChanger/sk_ak47_cartoon");
		custom_paint_kit_inject_request.emplace_back(crypto9.crypt_get(), 0xEEB22EC3, 0x5939AA22);
		auto crypto11 = XorStr_("SkinChanger/sk_ak47_cartoon");
		custom_paint_kit_inject_request.emplace_back(crypto11.crypt_get(), 0x13B0D829, 0xFACE14);
		auto crypto12 = XorStr_("SkinChanger/sk_ak47_experienced");
		custom_paint_kit_inject_request.emplace_back(crypto12.crypt_get(), 0x84B6BDE2, 0x0EEAD00B);
		auto crypto13 = XorStr_("SkinChanger/sk_ak47_fiwild");
		custom_paint_kit_inject_request.emplace_back(crypto13.crypt_get(), 0xFF61BC7E, 0x41F14947);
		auto crypto14 = XorStr_("SkinChanger/sk_ak47_vanquish");
		custom_paint_kit_inject_request.emplace_back(crypto14.crypt_get(), 0x0EABECB2, 0xDEB9CC30);
		auto crypto15 = XorStr_("SkinChanger/sk_awp_atom");
		custom_paint_kit_inject_request.emplace_back(crypto15.crypt_get(), 0xE8D88C84, 0x7D40391E);
		auto crypto16 = XorStr_("SkinChanger/sk_awp_black_pattern");
		custom_paint_kit_inject_request.emplace_back(crypto16.crypt_get(), 0xD76BDC2C, 0xFF2AD83B);
		auto crypto17 = XorStr_("SkinChanger/sk_awp_blizzard");
		custom_paint_kit_inject_request.emplace_back(crypto17.crypt_get(), 0xADD51616, 0x29314069);
		auto crypto18 = XorStr_("SkinChanger/sk_awp_blue_asimov");
		custom_paint_kit_inject_request.emplace_back(crypto18.crypt_get(), 0x2760FB44, 0xDF06BF3F);
		auto crypto19 = XorStr_("SkinChanger/sk_awp_devil");
		custom_paint_kit_inject_request.emplace_back(crypto19.crypt_get(), 0xA696F241, 0x3A0373E2);
		auto crypto20 = XorStr_("SkinChanger/sk_awp_geometric");
		custom_paint_kit_inject_request.emplace_back(crypto20.crypt_get(), 0x6C6FF688, 0xDBBFF7A0);
		auto crypto21 = XorStr_("SkinChanger/sk_awp_marine_killer");
		custom_paint_kit_inject_request.emplace_back(crypto21.crypt_get(), 0x3EAFD10B, 0x739E2E2D);
		auto crypto22 = XorStr_("SkinChanger/sk_awp_marivanna");
		custom_paint_kit_inject_request.emplace_back(crypto22.crypt_get(), 0x2F11BB8F, 0x02BF175D);
		auto crypto23 = XorStr_("SkinChanger/sk_awp_minibronze");
		custom_paint_kit_inject_request.emplace_back(crypto23.crypt_get(), 0xBB0FE422, 0x6B1A7A49);
		auto crypto24 = XorStr_("SkinChanger/sk_awp_nbome");
		custom_paint_kit_inject_request.emplace_back(crypto24.crypt_get(), 0xF52F2000, 0xB621CAB8);
		auto crypto25 = XorStr_("SkinChanger/sk_awp_neon_power");
		custom_paint_kit_inject_request.emplace_back(crypto25.crypt_get(), 0xBD2FA81B, 0x582B02D1);
		auto crypto26 = XorStr_("SkinChanger/sk_awp_razer");
		custom_paint_kit_inject_request.emplace_back(crypto26.crypt_get(), 0x93440691, 0x56FD3A03);
		auto crypto27 = XorStr_("SkinChanger/sk_awp_style");
		custom_paint_kit_inject_request.emplace_back(crypto27.crypt_get(), 0x3733589D, 0x7408BD01);
		auto crypto28 = XorStr_("SkinChanger/sk_deagle_heaven_wolf");
		custom_paint_kit_inject_request.emplace_back(crypto28.crypt_get(), 0x14705C53, 0x3B47ED11);
		auto crypto29 = XorStr_("SkinChanger/sk_deagle_pandator");
		custom_paint_kit_inject_request.emplace_back(crypto29.crypt_get(), 0x9DC129F7, 0x3206B1F3);
		auto crypto30 = XorStr_("SkinChanger/sk_glock_frame");
		custom_paint_kit_inject_request.emplace_back(crypto30.crypt_get(), 0x27EF35AE, 0x3D6286BC);
		auto crypto31 = XorStr_("SkinChanger/sk_karam_beast");
		custom_paint_kit_inject_request.emplace_back(crypto31.crypt_get(), 0xC193AA8B, 0x89C94A0F);
		auto crypto32 = XorStr_("SkinChanger/sk_karam_black_camo");
		custom_paint_kit_inject_request.emplace_back(crypto32.crypt_get(), 0x352C4486, 0xE6E46548);
		auto crypto33 = XorStr_("SkinChanger/sk_karam_cherry");
		custom_paint_kit_inject_request.emplace_back(crypto33.crypt_get(), 0xAAF0D80A, 0x509087C5);
		auto crypto34 = XorStr_("SkinChanger/sk_karam_electro");
		custom_paint_kit_inject_request.emplace_back(crypto34.crypt_get(), 0x1FCBDEB3, 0xE3ED6409);
		auto crypto35 = XorStr_("SkinChanger/sk_karam_galaxy");
		custom_paint_kit_inject_request.emplace_back(crypto35.crypt_get(), 0xD7146244, 0x1868DE87);
		auto crypto36 = XorStr_("SkinChanger/sk_karam_horse");
		custom_paint_kit_inject_request.emplace_back(crypto36.crypt_get(), 0x73417365, 0x56FD2915);
		auto crypto37 = XorStr_("SkinChanger/sk_m4a1_face_line");
		custom_paint_kit_inject_request.emplace_back(crypto37.crypt_get(), 0xA8B50348, 0x2F0A4512);
		auto crypto38 = XorStr_("SkinChanger/sk_m4a4_howl_original");
		custom_paint_kit_inject_request.emplace_back(crypto38.crypt_get(), 0x5F4B9908, 0xE1609C24);
		auto crypto39 = XorStr_("SkinChanger/sk_m4a4_sakura");
		custom_paint_kit_inject_request.emplace_back(crypto39.crypt_get(), 0x9AAC10DD, 0x2798A6BA);
		auto crypto40 = XorStr_("SkinChanger/sk_m4a4_suicude_squad");
		custom_paint_kit_inject_request.emplace_back(crypto40.crypt_get(), 0x1BF2355B, 0xFC3BA4B3);
		auto crypto41 = XorStr_("SkinChanger/sk_m4a4_triyellow");
		custom_paint_kit_inject_request.emplace_back(crypto41.crypt_get(), 0xB9856CDD, 0x2A10A5B0);
		auto crypto42 = XorStr_("SkinChanger/sk_tec9_rezonate");
		custom_paint_kit_inject_request.emplace_back(crypto42.crypt_get(), 0x4031F13C, 0xB2DB1F77);
		auto crypto43 = XorStr_("SkinChanger/sk_usp_hype");
		custom_paint_kit_inject_request.emplace_back(crypto43.crypt_get(), 0x3F48A80E, 0xE95F92E9);
		//auto crypto44 = XorStr_("SkinChanger/sk_karam_emerald");
		//custom_paint_kit_inject_request.emplace_back(crypto44.crypt_get(), 0xFACE14, 0xFACE14);

		auto crypto45 = XorStr_("SkinChanger/sk_ak47_dragonlore");
		custom_paint_kit_inject_request.emplace_back(crypto45.crypt_get(), 0x11318B92, 0x392ABA73);
		auto crypto46 = XorStr_("SkinChanger/sk_awp_howl");
		custom_paint_kit_inject_request.emplace_back(crypto46.crypt_get(), 0xEE891A49, 0xAEBBBACA);
		auto crypto47 = XorStr_("SkinChanger/sk_gut_supernova");
		custom_paint_kit_inject_request.emplace_back(crypto47.crypt_get(), 0xEC6CECF3, 0xF632B42D);

		auto crypto48 = XorStr_("SkinChanger/sk_m9_inferno_crystal");
		custom_paint_kit_inject_request.emplace_back(crypto48.crypt_get(), 0xCCEC19D3, 0x5282B8F5);
		auto crypto49 = XorStr_("SkinChanger/sk_m9_nebula_crystal");
		custom_paint_kit_inject_request.emplace_back(crypto49.crypt_get(), 0x37052601, 0x0F5CA747);
		auto crypto50 = XorStr_("SkinChanger/sk_m9_blood_crystal");
		custom_paint_kit_inject_request.emplace_back(crypto50.crypt_get(), 0xD3A06E01, 0xFACE14);
		////TODO: repair and CRC32
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_motorcycle_choco_boom"), 0x5A731AD6, 0xFACE14); //valve added this
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_motorcycle_yellow_camo"), 0x297FD3E6, 0xFACE14);
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_slick_snakeskin"), 0xF6CC7839, 0xFACE14); //valve added this  //TODO: repair and CRC32 //valve added this
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_specialist_forest_brown"), 0x4DB7FB16, 0xFACE14); //valve added this
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_sporty_pink"), 0xEB57BB34, 0xFACE14);//valve added this

		auto crypto51 = XorStr_("SkinChanger/sk_handwrap_leathery_fabric_red_beige");
		custom_paint_kit_inject_request.emplace_back(crypto51.crypt_get(), 0xFE7AC965, 0xFACE14);
		auto crypto52 = XorStr_("SkinChanger/sk_handwrap_leathery_fabric_yellow_camo");
		custom_paint_kit_inject_request.emplace_back(crypto52.crypt_get(), 0x3563B690, 0xFACE14);
		auto crypto53 = XorStr_("SkinChanger/sk_slick_brown");
		custom_paint_kit_inject_request.emplace_back(crypto53.crypt_get(), 0x1AFF0D3B, 0xFACE14); //valve added this  //TODO: repair and CRC32 //valve added this
		auto crypto54 = XorStr_("SkinChanger/sk_specialist_green_red_black");
		custom_paint_kit_inject_request.emplace_back(crypto54.crypt_get(), 0x39A5B248, 0xFACE14);
		auto crypto55 = XorStr_("SkinChanger/sk_sporty_fade");
		custom_paint_kit_inject_request.emplace_back(crypto55.crypt_get(), 0xAB56ED1A, 0xFACE14);//TODO: CRC32
		auto crypto56 = XorStr_("SkinChanger/sk_sporty_red");
		custom_paint_kit_inject_request.emplace_back(crypto56.crypt_get(), 0xFC4CECA0, 0xFACE14);
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_handwrap_leathery_dark_metal"), 0xFACE14, 0xFACE14); //DELETED BY VOLVO TODO: CRC32;
		//custom_paint_kit_inject_request.emplace_back(XorStr("SkinChanger/sk_sporty_mayan_green"), 0xFACE14, 0xFACE14);//DELETED BY VOLVO TODO: CRC32

		//auto crypto57 = XorStr_("SkinChanger/sc_blueasimov47");
		//custom_paint_kit_inject_request.emplace_back(crypto57.crypt_get(), 0xFACE14, 0xFACE14);
		auto crypto58 = XorStr_("SkinChanger/sc_carbon47");
		custom_paint_kit_inject_request.emplace_back(crypto58.crypt_get(), 0x323A10FD, 0x78E09A51);
		auto crypto59 = XorStr_("SkinChanger/sc_damascus47");
		custom_paint_kit_inject_request.emplace_back(crypto59.crypt_get(), 0xCEE50C5C, 0x128F69E6);
		auto crypto60 = XorStr_("SkinChanger/sc_gold47");
		custom_paint_kit_inject_request.emplace_back(crypto60.crypt_get(), 0xF58C58EF, 0xC1352204);
		auto crypto61 = XorStr_("SkinChanger/sc_greenvulcan47");
		custom_paint_kit_inject_request.emplace_back(crypto61.crypt_get(), 0x673BA579, 0x13655E14);
		auto crypto62 = XorStr_("SkinChanger/sc_rubydiamond47");
		custom_paint_kit_inject_request.emplace_back(crypto62.crypt_get(), 0x7AB60CA5, 0xB51644F4);
		auto crypto63 = XorStr_("SkinChanger/sc_sport47");
		custom_paint_kit_inject_request.emplace_back(crypto63.crypt_get(), 0xCFB47BE1, 0x7135F1A4);
		
		auto crypto64 = XorStr_("SkinChanger/sk_ak47_akmy");
		custom_paint_kit_inject_request.emplace_back(crypto64.crypt_get(), 0xFACE14, 0xFACE14);

#ifdef _PREMIUM_VERSION
		auto path = std::filesystem::current_path() / XorStr("csgo") / XorStr("SkinChanger_user");
		if(std::filesystem::exists( path))
		{
			std::string to_add;
			for (const auto& p : std::filesystem::directory_iterator(path))
			{
				if (p.is_directory())
					continue;
				if (!p.is_regular_file())
					continue;
				
				const auto filename_path = p.path().filename();//p.path().filename();
				std::string name(reinterpret_cast<const char*>(filename_path.u8string().c_str()));//filename_path.string();
				if (name.empty())
					continue;
				to_add = XorStr("SkinChanger_user/");
				to_add.append(name);
				custom_paint_kit_inject_request.emplace_back(to_add, 0xFACE14, 0xFACE14);
			}
		}
		else
		{
			std::filesystem::create_directories(path);
		}
#endif
		
		changer_log->msg(XorStr("load skins: parsing request\n"));
		inject_all_paint_kits(custom_paint_kit_inject_request);

		//fix_VMT_CACHE
		changer_log->msg(XorStr("vmtcache: patch file\n"));
		patch_vmtcache_file();

		//InjectPaintKit(XorStr("SkinChanger/sk_handwrap_leathery_dark_metal"), 0xFACE14, 0xFACE14); //TODO: CRC32
		//InjectPaintKit(XorStr("SkinChanger/sk_handwrap_leathery_fabric_red_beige"), 0xFE7AC965, 0xFACE14); 
		//InjectPaintKit(XorStr("SkinChanger/sk_handwrap_leathery_fabric_yellow_camo"), 0xFACE14, 0xFACE14);
		////TODO: repair and CRC32
		//InjectPaintKit(XorStr("SkinChanger/sk_motorcycle_choco_boom"), 0x5A731AD6, 0xFACE14); //valve added this
		//InjectPaintKit(XorStr("SkinChanger/sk_motorcycle_yellow_camo"), 0x297FD3E6, 0xFACE14); 
		//InjectPaintKit(XorStr("SkinChanger/sk_slick_snakeskin"), 0xF6CC7839, 0xFACE14); //valve added this  //TODO: repair and CRC32 //valve added this
		//InjectPaintKit(XorStr("SkinChanger/sk_specialist_forest_brown"), 0x4DB7FB16, 0xFACE14); //valve added this
		//InjectPaintKit(XorStr("SkinChanger/sk_specialist_green_red_black"), 0x39A5B248, 0xFACE14);
		//InjectPaintKit(XorStr("SkinChanger/sk_sporty_fade"), 0x802C22C0, 0xFACE14);
		//InjectPaintKit(XorStr("SkinChanger/sk_sporty_pink"), 0xEB57BB34, 0xFACE14);
		//InjectPaintKit(XorStr("SkinChanger/sk_sporty_red"), 0xFC4CECA0, 0xFACE14);
		//InjectPaintKit(XorStr("SkinChanger/sk_sporty_mayan_green"), 0xFACE14, 0xFACE14);//TODO: CRC32

		
		const SourceEngine::LoggingChannelID_t gloves_channel_id = reinterpret_cast<SourceEngine::LoggingChannelID_t(__stdcall*)()>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 ? ? ? 00 FF FF FF")))();
#ifdef _DEBUG
		valve_logging_system.SetChannelSpewLevel(gloves_channel_id, SourceEngine::LS_MESSAGE);
		auto Log_General = valve_logging_system.FindChannel("General");
		valve_logging_system.SetChannelSpewLevel(Log_General, SourceEngine::LS_MESSAGE);
		
#else
		valve_logging_system.SetChannelSpewLevel(gloves_channel_id, SourceEngine::LS_HIGHEST_SEVERITY);
#endif // _DEBUG

		changer_log->msg(XorStr("oldweapons: Precache\n"));

		Utils::PrecacheSounds();

		changer_log->msg(XorStr("Config: loading base\n"));

		g_pConfig = make_unique<config>();

		changer_log->msg(XorStr("Config: old sounds cfg\n"));
		if (Options::sounds_precached)
			g_pConfig->load_old_sounds_config(Options::config_old_sounds);

		changer_log->msg(XorStr("Config: ct cfg\n"));
		g_pConfig->load_ct_config(Options::config_ct);
		changer_log->msg(XorStr("Config: t cfg\n"));
		g_pConfig->load_t_config(Options::config_t);
		changer_log->msg(XorStr("Config: other cfg\n"));
		g_pConfig->read_other_options();

		load_thread_handle = g_pWinApi->dwCreateSimpleThread(read_thread, nullptr, 0);
		save_thread_handle = g_pWinApi->dwCreateSimpleThread(save_thread, nullptr, 0);
		
		//auto weapons_crate_community_21 = GetWeaponsForCrate("crate_community_21");

#ifdef _PREMIUM_VERSION
		LoadInventory();

		//g_pGameCoordinatorHook->hook_index(reinterpret_cast<SendMessage_t>(Hooked_SendMessage), HooksIndexes::SendMessage);
		//g_fnOriginalSendMessage = g_pGameCoordinatorHook->get_original<SendMessage_t>(HooksIndexes::SendMessage);

		g_pGameCoordinatorHook->hook_index(reinterpret_cast<RetrieveMessage_t>(Hooked_RetrieveMessage), HooksIndexes::RetrieveMessage);
		g_fnOriginalRetrieveMessage = g_pGameCoordinatorHook->get_original<RetrieveMessage_t>(HooksIndexes::RetrieveMessage);

		//Protobuffs::SendClientHello();
		//Protobuffs::SendMatchmakingClient2GCHello();
#endif

		//g_fnCleanInventoryImageCacheDir();
		//std::stringstream out;
		//{
		//	Json::StreamWriterBuilder builder;
		//	Json::StreamWriter* writer = builder.newStreamWriter();
		//	if (writer)
		//	{
		//		Json::Value session;
		//		session.clear();
		//		session[XorStr("steamid")] = steamid.m_unAll64Bits;
		//		session[XorStr("last_session")] = Options::last_session;

		//		writer->write(session, &out);
		//		out.flush();
		//		
		//	}
		//}
		//auto* report = new CHTTPRequest(HTTP_METHOD_POST, XorStr("http://84.201.151.56:80"));
		//report->Register();
		////report->SetHeader("Content-Type", "application/json");
		//report->SetBody(XorStr("application/json"), out.str());
		//report->SetCallback(HTTPRequestCallback_report_test);
		//report->Execute();
		
		changer_log->msg(XorStr("ClientState: ForceFullUpdate\n"));
		SourceEngine::interfaces.ClientState()->ForceFullUpdate();

		changer_log->msg(XorStr("ConCommand: register dodge_challenger \n"));
		auto* memory_concommand = memalloc->Alloc(sizeof(SourceEngine::ConCommand));
		dodge_challenger = new(memory_concommand) SourceEngine::ConCommand("dodge_challenger", dodge_challenger_callback);
		
#ifdef CHANGER_PROFILE
		__itt_task_end(domain);
#endif
		//FillGlobalSessionInfo();

		//if (first_start)
		//	EventLogger::send_event_to_server(XorStr("install_application"));

		//EventLogger::send_event_to_server(XorStr("start_session"));
		//return 0;
	}


	void Restore()
	{
		g_pConfig->save_t_config(Options::config_t);
		g_pConfig->save_ct_config(Options::config_ct);

		//Restore the WindowProc
		g_pWinApi->dwSetWindowLongW(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_pOldWindowProc));

		//Remove the hooks
		*g_pOverlay_PresentHook = reinterpret_cast<DWORD_PTR>(&g_fnOriginalPresent);
		g_pD3DDevice9Hook->unhook_all();
		g_pMatSurfaceHook->unhook_all();
		g_pEventManagerHook->unhook_all();
		g_pDataCacheHook->unhook_all();
		g_pGameCoordinatorHook->unhook_all();
		g_pFileSystemHook->unhook_all();
		g_pCustomManagerHook->unhook_all();
		g_pGameTypesHook->unhook_all();
		g_pD3DDevice9Hook->unhook_all();
		g_pD3DDevice9Hook->unhook_all();
		g_pClientHook->unhook_table();

		delete g_pD3DDevice9Hook;
		delete g_pMatSurfaceHook;
		delete g_pEventManagerHook;
		delete g_pDataCacheHook;
		delete g_pGameCoordinatorHook;
		delete g_pFileSystemHook;
		delete g_pCustomManagerHook;
		delete g_pGameTypesHook;
		delete g_pClientHook;
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		//Initializes the GUI and the renderer
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsBlack();
		ImGui_ImplWin32_Init(g_hWindow);
		ImGui_ImplDX9_Init(pDevice);

		std::basic_string<char> font_path = std::string{};

		io.Fonts->TexDesiredWidth = 4096;
		ImFontConfig config;
		config.MergeMode = false;
		config.OversampleV = 1;
		config.OversampleH = 2;

		auto* font_buffer = static_cast<unsigned char*>(ImGui::MemAlloc(100608));
		memcpy(font_buffer, hellfire_font, 100608);
		io.Fonts->AddFontFromMemoryTTF(font_buffer, 100608, 14, &config, ImFontAtlas::GetGlyphRangesCyrillic());
		config.MergeMode = true;
		if (ImGui::get_system_font_path(XorStr("Tahoma"), font_path))
		{
			io.Fonts->AddFontFromFileTTF(font_path.data(), 14, &config, io.Fonts->GetGlyphRangesJapanese());
		}

		g_bWasInitialized = true;
	}

	bool __fastcall Hooked_LooseFileAllowed(IFileSystem* filesystem, void*)
	{
		return true;
	}
	int __fastcall Hooked_GetUnverifiedFileHashes(IFileSystem* filesystem, void*, CUnverifiedFileHash* pFiles, int nMaxFiles)
	{
		return 0;
	}
}
