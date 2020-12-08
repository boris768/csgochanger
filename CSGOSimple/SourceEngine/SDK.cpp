// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SDK.hpp"

//#include "../Utils.hpp"
#include "../XorStr.hpp"
#include "../IPhysicsSurfaceProps.h"
#include "../CustomWinAPI.h"
#include "../Utils.hpp"
#include "../Hooks.hpp"
#include <iostream>

namespace SourceEngine
{
	Interfaces interfaces;
	Memory memory;

    HSteamUser hSteamUser;
    HSteamPipe hSteamPipe;

	namespace factory
	{
		void init()
		{
			Engine_factory = new CFactory(memory.engine().lpBaseOfDll);
			Client_factory = new CFactory(memory.client().lpBaseOfDll);
			Matchmaking_factory = new CFactory(memory.matchmaking().lpBaseOfDll);
			Vgui_factory = new CFactory(memory.vguimatsurface().lpBaseOfDll);
			Vgui2_factory = new CFactory(memory.vgui2().lpBaseOfDll);
			VStdLib_factory = new CFactory(memory.vstdlib().lpBaseOfDll);
			VPhysics_factory = new CFactory(memory.vphysics().lpBaseOfDll);
			StudioRender_factory = new CFactory(memory.studiorender().lpBaseOfDll);
			InputSystem_factory = new CFactory(memory.inputsystem().lpBaseOfDll);
			FileSystem_factory = new CFactory(memory.filesystem_stdio().lpBaseOfDll);
			MaterialSystem_factory = new CFactory(memory.materialsystem().lpBaseOfDll);
			Localize_factory = new CFactory(memory.localize().lpBaseOfDll);
			Datacache_factory = new CFactory(memory.datacache().lpBaseOfDll);
		}

		CFactory* Engine_factory = nullptr;
		CFactory* Client_factory = nullptr;
		CFactory* Matchmaking_factory = nullptr;
		CFactory* Vgui_factory = nullptr;
		CFactory* Vgui2_factory = nullptr;
		CFactory* VStdLib_factory = nullptr;
		CFactory* VPhysics_factory = nullptr;
		CFactory* StudioRender_factory = nullptr;
		CFactory* InputSystem_factory = nullptr;
		CFactory* FileSystem_factory = nullptr;
		CFactory* MaterialSystem_factory = nullptr;
		CFactory* Localize_factory = nullptr;
		CFactory* Datacache_factory = nullptr;
	}

	//Utils::CFactory			Engine_factory((memory.engine().lpBaseOfDll));
	//Utils::CFactory			Client_factory((memory.client().lpBaseOfDll));
	//Utils::CFactory			Vgui_factory((memory.vguimatsurface().lpBaseOfDll));
	//Utils::CFactory			Vgui2_factory((memory.vgui2().lpBaseOfDll));
	//Utils::CFactory			VStdLib_factory((memory.vstdlib().lpBaseOfDll));
	//Utils::CFactory			VPhysics_factory((memory.vphysics().lpBaseOfDll));
	//Utils::CFactory			StudioRender_factory((memory.studiorender().lpBaseOfDll));
	//Utils::CFactory			InputSystem_factory((memory.inputsystem().lpBaseOfDll));
	//Utils::CFactory			FileSystem_factory((memory.filesystem_stdio().lpBaseOfDll));
	//Utils::CFactory			MaterialSystem_factory((memory.materialsystem().lpBaseOfDll));
	//Utils::CFactory			Localize_factory((memory.localize().lpBaseOfDll));
	//Utils::CFactory			Datacache_factory((memory.datacache().lpBaseOfDll));

	void dump_all_interfaces()
	{
		auto base = factory::Engine_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "engine: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::Client_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "client: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::Vgui_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "vgui: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::Vgui2_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "vgui2: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::VStdLib_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "vstdlib: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::VPhysics_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "vphysycs: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::StudioRender_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "studiorender: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::FileSystem_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "filesystem: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::MaterialSystem_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "materialsystem: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;

		std::cout << "--------------------------------------------------------------------" << std::endl;

		base = factory::Localize_factory->dump();
		for (auto it = base.begin(); it != base.end(); ++it)
			std::cout << "localize: " << it->m_pName << " : " << reinterpret_cast<unsigned long>(it->m_CreateFn) << std::endl;
	}

	IVEngineClient* Interfaces::Engine()
	{
		if (!m_pEngine)
		{
			m_pEngine = reinterpret_cast<IVEngineClient*>(factory::Engine_factory->dwGetInterface(XorStr("VEngineClient")));
#ifdef DEBUG
			//std::cout << "m_pEngine: " << std::hex << m_pEngine << std::endl;
#endif
		}
		return m_pEngine;
	}

	IBaseClientDLL* Interfaces::Client()
	{
		if (!m_pClient)
		{
			m_pClient = reinterpret_cast<IBaseClientDLL*>(factory::Client_factory->dwGetInterface(XorStr("VClient")));
#ifdef DEBUG
			//std::cout << XorStr("m_pClient: ") << std::hex << m_pClient << std::endl;
#endif
		}

		return m_pClient;
	}
//
//	IVDebugOverlay* Interfaces::DebugOverlay()
//	{
//		if (!m_pDebugOverlay)
//		{
//			m_pDebugOverlay = reinterpret_cast<IVDebugOverlay*>(SourceEngine::factory::Engine_factory->dwGetInterface(XorStr("VDebugOverlay")));
//#ifdef DEBUG
//			std::cout << "m_pDebugOverlay: " << std::hex << m_pDebugOverlay << std::endl;
//#endif
//		}
//		return m_pDebugOverlay;
//	}

	IStudioRender* Interfaces::StudioRender()
	{
		if (!m_pStudioRender)
		{
			m_pStudioRender = reinterpret_cast<IStudioRender*>(factory::StudioRender_factory->dwGetInterface(XorStr("VStudioRender")));
#ifdef DEBUG
			//std::cout << XorStr("m_pStudioRender: ") << std::hex << m_pStudioRender << std::endl;
#endif
		}
		return m_pStudioRender;
	}

	IClientEntityList* Interfaces::EntityList()
	{
		if (!m_pEntityList)
		{
			m_pEntityList = reinterpret_cast<IClientEntityList*>(factory::Client_factory->dwGetInterface(XorStr("VClientEntityList")));
#ifdef DEBUG
			//std::cout << XorStr("m_pEntityList: ") << std::hex << m_pEntityList << std::endl;
#endif
		}
		return m_pEntityList;
	}

	CGlobalVarsBase* Interfaces::GlobalVars()
	{
		if (!m_pGlobals)
		{
			m_pGlobals = **reinterpret_cast<CGlobalVarsBase***>(Utils::FindSignature(memory.client(), "A1 ? ? ? ? 5E 8B 40 10") + 1);
			//**reinterpret_cast< CGlobalVarsBase*** >((*reinterpret_cast< DWORD** >(Client()))[0] + 0x1B);
#ifdef DEBUG
			//std::cout << XorStr("m_pGlobals: ") << std::hex << m_pGlobals << std::endl;
#endif
		}
		return m_pGlobals;
	}

	IPanel* Interfaces::VGUIPanel()
	{
		if (!m_pVGuiPanel)
		{
			m_pVGuiPanel = reinterpret_cast<IPanel*>(factory::Vgui2_factory->dwGetInterface(XorStr("VGUI_Panel")));
#ifdef DEBUG
			//std::cout << XorStr("m_pVGuiPanel: ") << std::hex << m_pVGuiPanel << std::endl;
#endif
		}
		return m_pVGuiPanel;
	}

	ISurface* Interfaces::MatSurface()
	{
		if (!m_pVGuiSurface)
		{
			m_pVGuiSurface = reinterpret_cast<ISurface*>(factory::Vgui_factory->dwGetInterface(XorStr("VGUI_Surface")));
#ifdef DEBUG
			//std::cout << XorStr("m_pVGuiSurface: ") << std::hex << m_pVGuiSurface << std::endl;
#endif
		}
		return m_pVGuiSurface;
	}

	CInput* Interfaces::Input()
	{
		if (!m_pInput)
		{
			m_pInput = *reinterpret_cast<CInput**>(Utils::FindSignature(memory.client(), XorStr("B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85")) + 1);
#ifdef DEBUG
			//std::cout << XorStr("m_pInput: ") << std::hex << m_pInput << std::endl;
#endif
		}
		return m_pInput;
	}

	IEngineTrace* Interfaces::EngineTrace()
	{
		if (!m_pEngineTrace)
		{
			m_pEngineTrace = reinterpret_cast<IEngineTrace*>(factory::Engine_factory->dwGetInterface(XorStr("EngineTraceClient")));
#ifdef DEBUG
			//std::cout << XorStr("m_pEngineTrace: ") << std::hex << m_pEngineTrace << std::endl;
#endif
		}
		return m_pEngineTrace;
	}

	ICvar* Interfaces::CVar()
	{
		if (!m_pCVar)
		{
			m_pCVar = reinterpret_cast<ICvar*>(factory::VStdLib_factory->dwGetInterface(XorStr("VEngineCvar")));
#ifdef DEBUG
			//std::cout << XorStr("m_pCVar: ") << std::hex << m_pCVar << std::endl;
#endif
		}
		return m_pCVar;
	}

	IClientMode* Interfaces::ClientMode()
	{
		if (!m_pClientMode)
		{
			//auto uAddress = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(XorStr("client.dll"), XorStr("8B 35 ? ? ? ? 85 FF 74 73")) + 2);
			//m_pClientMode = *reinterpret_cast<IClientMode**>(uAddress);
			m_pClientMode = *reinterpret_cast<IClientMode**>(*reinterpret_cast<uintptr_t***>((*reinterpret_cast<uintptr_t**>(Client()))[10] + 0x5));
#ifdef DEBUG
			//std::cout << XorStr("m_pClientMode: ") << std::hex << m_pClientMode << std::endl;
#endif
		}
		return m_pClientMode;
	}

	IVModelInfo* Interfaces::ModelInfo()
	{
		if (!m_pModelInfo)
		{
			m_pModelInfo = reinterpret_cast<IVModelInfo*>(factory::Engine_factory->dwGetInterface(XorStr("VModelInfoClient")));
		}
		return m_pModelInfo;
	}

	IPhysicsSurfaceProps* Interfaces::PhysicsSurface()
	{
		if(!m_pPhysicsSurface)
		{
			m_pPhysicsSurface = reinterpret_cast<IPhysicsSurfaceProps*>(factory::VPhysics_factory->dwGetInterface(XorStr("VPhysicsSurfaceProps")));
		}
		return m_pPhysicsSurface;
	}

	IInputSystem* Interfaces::InputSystem()
	{
		if(!m_pInputSystem)
		{
			m_pInputSystem = reinterpret_cast<IInputSystem*>(factory::InputSystem_factory->dwGetInterface(XorStr("InputSystemVersion")));
		}
		return m_pInputSystem;
	}

	C_CSPlayerResource** Interfaces::PlayerResource()
	{
		//"dwPlayerResource": 
		//"extra": 2,
		//"mode_read": true,
		//"mode_subtract": true,
		//"module": "client.dll",
		//"offset": 0,
		//"pattern": "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"

		//DumpPatternOffset("DT_CSPlayerResource", "CSPlayerResource", "client.dll",
		//	"8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7",
		//	Remote::SignatureType_t::READ | Remote::SignatureType_t::SUBTRACT, 0x2, 0x0, ss);

		if(!m_pPlayerResource)
		{
			ptrdiff_t buffer = Utils::FindSignature(memory.client(), XorStr("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"));
			buffer = *reinterpret_cast<ptrdiff_t*>(buffer);
			//buffer -= memory.client();
			buffer += 2;//0x02F063A4
			m_pPlayerResource = reinterpret_cast<C_CSPlayerResource**>(buffer);
		}
		return m_pPlayerResource; //+ memory.client();
		
	}

	IGameMovement* Interfaces::GameMovement()
	{
		if(!m_pGameMovement)
		{
			m_pGameMovement = reinterpret_cast<IGameMovement*>(SourceEngine::factory::Client_factory->dwGetInterface(XorStr("GameMovement")));
#ifdef DEBUG
			//std::cout << XorStr("m_pGameMovement: ") << std::hex << m_pGameMovement << std::endl;
#endif
		}
		return m_pGameMovement;
	}

	IPrediction* Interfaces::Prediction()
	{
		if (!m_pPrediction)
		{
			m_pPrediction = reinterpret_cast<IPrediction*>(SourceEngine::factory::Client_factory->dwGetInterface(XorStr("VClientPrediction")));
#ifdef DEBUG
			//std::cout << XorStr("m_pPrediction: ") << std::hex << m_pPrediction << std::endl;
#endif
		}
		return m_pPrediction;
	}

	IMoveHelper* Interfaces::MoveHelper()
	{
		if (!m_pMoveHelper)
		{			
			m_pMoveHelper = **reinterpret_cast<IMoveHelper***>(Utils::FindSignature(memory.client(), XorStr("8B 0D ? ? ? ? 8B 46 08 68")) + 2);

		}
		return m_pMoveHelper;
	}

	CClientState* Interfaces::ClientState()
	{
		if(!m_pClientState)
		{			
			m_pClientState = reinterpret_cast<CClientState*>(**reinterpret_cast<PDWORD*>(Utils::FindSignature(memory.engine(), XorStr("8B 3D ? ? ? ? 8A F9")) + 2));
		}
		return m_pClientState;
	}

	CMemAlloc* Interfaces::MemAlloc()
	{
		if(!m_pMemAlloc)
		{
			m_pMemAlloc = *static_cast<CMemAlloc**>(g_pWinApi->GetProcAddress(static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("g_pMemAlloc")));
		}
		return m_pMemAlloc;
	}

	IFileSystem* Interfaces::FileSystem()
	{
		if (!m_pFileSystem)
		{
			m_pFileSystem = reinterpret_cast<IFileSystem*>(factory::FileSystem_factory->dwGetInterface(XorStr("VFileSystem")));
		}
		return m_pFileSystem;
	}

	CLocalize* Interfaces::LocalizeSystem()
	{
		if(!m_pLocalizeSystem)
		{
			m_pLocalizeSystem = reinterpret_cast<CLocalize*>(factory::Localize_factory->dwGetInterface(XorStr("Localize_")));
			while (m_pLocalizeSystem->Find(XorStr("#GameUI_DisplayMode")) == XorStr(L"GameUI_DisplayMode"))
				g_pWinApi->dwSleep(50);
		}
		return m_pLocalizeSystem;
	}

	IMaterialSystem* Interfaces::MaterialSystem()
	{
		if (!m_pMaterialSystem)
		{
			m_pMaterialSystem = reinterpret_cast<IMaterialSystem*>(factory::MaterialSystem_factory->dwGetInterface(XorStr("VMaterialSystem")));
		}
		return m_pMaterialSystem;
	}

	CGameEventManager* Interfaces::GameEventManager()
	{
		if (!m_pGameEventManager)
		{
			m_pGameEventManager = reinterpret_cast<CGameEventManager*>(factory::Engine_factory->dwGetInterface(XorStr("GAMEEVENTSMANAGER"), 1));
		}
		return m_pGameEventManager;
	}

	CCStrike15ItemSchema* Interfaces::ItemSchema()
	{
		if(!m_pStrike15ItemSchema)
		{
			using GetItemSchemaFn = DWORD(__stdcall*)();
			const GetItemSchemaFn GetItemSchema = reinterpret_cast<GetItemSchemaFn>(Utils::FindSignature(memory.client(), XorStr("A1 ? ? ? ? 85 C0 75 53")));
			m_pStrike15ItemSchema = reinterpret_cast<CCStrike15ItemSchema*>(GetItemSchema() + sizeof(void*));
			//m_pStrike15ItemSchema = reinterpret_cast<CCStrike15ItemSchema*>(GetItemSchema());
		}
		return m_pStrike15ItemSchema;
	}

	ITextureDictionary* Interfaces::TextureDictionary()
	{
		if(!m_pTextureDictionary)
		{
			m_pTextureDictionary = *reinterpret_cast<ITextureDictionary**>(Utils::FindSignature(memory.vguimatsurface(), XorStr("55 8B EC 56 8B ? ? B9 ? ? ? ? 56")) + 8);
		}
		return m_pTextureDictionary;
	}

	LoggingSystem* Interfaces::GameLoggingSystem()
	{
		if(!m_pLoggingSystem)
		{
			m_pLoggingSystem = new LoggingSystem();
		}
		return m_pLoggingSystem;
	}

	CSteamNetworkingUtils* Interfaces::SteamNetworkingUtils()
	{
		if(!m_pSteamNetworkingUtils)
		{
			using get_SNU_t = CSteamNetworkingUtils*(__cdecl*)();
			const get_SNU_t get_snuFn = reinterpret_cast<get_SNU_t>(Utils::FindSignature(memory.engine(), XorStr("A1 ? ? ? ? A8 01 75 12 83 C8 01 C7 05 ? ? ? ? ? ? ? ? A3 ? ? ? ? B8")));

			m_pSteamNetworkingUtils = get_snuFn();//*reinterpret_cast<CSteamNetworkingUtils**>(Utils::FindSignature(memory.engine(), XorStr("A1 ? ? ? ? A8 01 75 12 83 C8 01 C7 05 ? ? ? ? ? ? ? ? A3 ? ? ? ? B8")) + 24);  //get_snuFn();
		}
		return m_pSteamNetworkingUtils;
	}

	INetworkStringTableContainer* Interfaces::NetworkStringTableContainer()
	{
		if(!m_pNetworkStringTableContainer)
		{
			m_pNetworkStringTableContainer = reinterpret_cast<INetworkStringTableContainer*>(factory::Engine_factory->dwGetInterface(XorStr("VEngineClientStringTable")));
		}
		return m_pNetworkStringTableContainer;
	}

	IMDLCache* Interfaces::MDLCache()
	{
		if(!m_pMDLCache)
		{
			m_pMDLCache = reinterpret_cast<IMDLCache*>(factory::Datacache_factory->dwGetInterface(XorStr("MDLCache")));
		}
		return m_pMDLCache;
	}

    ISteamClient * Interfaces::SteamClient()
    {
        if (!m_pSteamClient)
        {
            m_pSteamClient = reinterpret_cast<ISteamClient*(__cdecl*)()>(GetProcAddress(static_cast<HMODULE>(memory.steamapi().lpBaseOfDll), "SteamClient"))();
        }
        return m_pSteamClient;
    }

    ISteamGameCoordinator * Interfaces::GameCoordinator()
    {
        if (!m_pGameCoordinator)
        {
            m_pGameCoordinator = static_cast<ISteamGameCoordinator*>(this->SteamClient()->GetISteamGenericInterface(
                hSteamUser, hSteamPipe, XorStr("SteamGameCoordinator001")));
        }
        return m_pGameCoordinator;
    }

	IClientLeafSystem * Interfaces::ClientLeafSystem()
	{
		if (!m_pClientLeafSystem)
		{
			m_pClientLeafSystem = *reinterpret_cast<IClientLeafSystem**>(Utils::FindSignature(memory.client(), XorStr("A1 ? ? ? ? FF 50 18 8B 86 ? ? ? ? B9"))+1);
		}
		return m_pClientLeafSystem;
	}

	IEngineSound * Interfaces::EngineSoundClient()
	{
		if (!m_pEngineSoundClient)
		{
			m_pEngineSoundClient = reinterpret_cast<IEngineSound*>(factory::Engine_factory->dwGetInterface(XorStr("IEngineSoundClient")));
		}
		return m_pEngineSoundClient;
	}

	CSInventoryManager* Interfaces::InventoryManager()
	{
		//g_CSInventoryManager = *reinterpret_cast<CSInventoryManager**>(Pattern::FindSignature("client.dll", "B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14") + 0x1);
		if(!m_pInventoryManager)
		{
			m_pInventoryManager= *reinterpret_cast<CSInventoryManager**>(Utils::FindSignature(memory.client(), XorStr("B9 ? ? ? ? 8D 44 24 10 89 54 24 14")) + 0x1);
		}
		return m_pInventoryManager;
	}

	ISteamHTTP* Interfaces::SteamHTTP()
	{
		if (!m_pSteamHTTP)
		{
			m_pSteamHTTP = Engine()->GetSteamAPIContext()->m_pSteamHTTP;
		}
		return m_pSteamHTTP;
	}

	IGameTypes* Interfaces::GameTypes()
	{
		if(!m_pGameTypes)
		{
			m_pGameTypes = reinterpret_cast<IGameTypes*>(factory::Matchmaking_factory->dwGetInterface(XorStr("VENGINE_GAMETYPES_VERSION")));
		}
		return m_pGameTypes;
	}

	CGameUI* Interfaces::GameUI()
	{
		if (!m_pGameUI)
		{
			m_pGameUI = reinterpret_cast<CGameUI*>(factory::Client_factory->dwGetInterface(XorStr("GameUI")));
		}
		return m_pGameUI;
	}

	CCSCustomMaterialSwapManager* Interfaces::CustomMaterialSwapManager()
	{
		if(!m_pCustomMaterialSwapManager)
		{
			m_pCustomMaterialSwapManager = reinterpret_cast<CCSCustomMaterialSwapManager*>(*reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.client(), XorStr("8B 0D ? ? ? ? 03 CE 89 4D FC 8B 79 0C 83")) + 2) - 4);

		}
		return m_pCustomMaterialSwapManager;
	}

	IMatchFramework* Interfaces::MatchFramework()
	{
		if(!g_pMMF)
		{
			g_pMMF = reinterpret_cast<IMatchFramework*>(factory::Matchmaking_factory->dwGetInterface(XorStr("MATCHFRAMEWORK_")));
		}
		return g_pMMF;
	}

	void* Interfaces::RenderToRTHelper()
	{
		if(!m_pRenderToRTHelper)
		{
			m_pRenderToRTHelper = reinterpret_cast<void*>(factory::Client_factory->dwGetInterface(XorStr("RenderToRTHelper")));
		}
		return m_pRenderToRTHelper;
	}
	
	ptrdiff_t Memory::dwViewMatrix()
	{
		/*	"extra": 4,
		"mode_read" : true,
		"mode_subtract" : true,
		"module" : "client.dll",
		"offset" : 176,
		"pattern" : "F3 0F 6F 05 ? ? ? ? 8D 85"*/

		if (!_dwViewMatrix)
		{
			//_dwViewMatrix = 0x4AC7724;
			_dwViewMatrix = static_cast<ptrdiff_t>(Utils::FindSignature(memory.client(), XorStr("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")));
			_dwViewMatrix += 0x3;
			_dwViewMatrix = *reinterpret_cast<uintptr_t*>(_dwViewMatrix);
			_dwViewMatrix -= reinterpret_cast<ptrdiff_t>(client().lpBaseOfDll);
			_dwViewMatrix += 176;

		}
		return _dwViewMatrix;
	}
	
	MODULEINFO Memory::client()
	{
		while (!_client.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"client.dll"), _client);
			g_pWinApi->dwSleep(50);
		}
		return _client;
	}

	MODULEINFO Memory::engine()
	{
		while (!_engine.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"engine.dll"), _engine);
		}
		return _engine;
	}

	MODULEINFO Memory::tier0()
	{
		while (!_tier0.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"tier0.dll"), _tier0);
		}
		return _tier0;
	}

	MODULEINFO Memory::vguimatsurface()
	{
		while (!_vguimatsurface.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"vguimatsurface.dll"), _vguimatsurface);
		}
		return _vguimatsurface;
	}

	MODULEINFO Memory::vgui2()
	{
		while (!_vgui2.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"vgui2.dll"), _vgui2);
		}
		return _vgui2;
	}

	MODULEINFO Memory::vphysics()
	{
		while (!_vphysics.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"vphysics.dll"), _vphysics);
		}
		return _vphysics;
	}

	MODULEINFO Memory::studiorender()
	{
		while (!_studiorender.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"studiorender.dll"), _studiorender);
		}
		return _studiorender;
	}

	MODULEINFO Memory::inputsystem()
	{
		while (!_inputsystem.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"inputsystem.dll"), _inputsystem);
		}
		return _inputsystem;
	}

	MODULEINFO Memory::filesystem_stdio()
	{
		while (!_filesystem_stdio.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"filesystem_stdio.dll"), _filesystem_stdio);
		}
		return _filesystem_stdio;
	}

	MODULEINFO Memory::materialsystem()
	{
		while (!_materialsystem.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"materialsystem.dll"), _materialsystem);
		}
		return _materialsystem;
	}

	MODULEINFO Memory::vstdlib()
	{
		while (!_vstdlib.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"vstdlib.dll"), _vstdlib);
		}
		return _vstdlib;
	}

	MODULEINFO Memory::localize()
	{
		while (!_localize.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"localize.dll"), _localize);
		}
		return _localize;
	}

	MODULEINFO Memory::shaderapidx9()
	{
		while (!_shaderapidx9.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"shaderapidx9.dll"), _shaderapidx9);
		}
		return _shaderapidx9;
	}

	MODULEINFO Memory::gameoverlayrenderer()
	{
		while (!_gameoverlayrenderer.lpBaseOfDll || !_gameoverlayrenderer.EntryPoint || !_gameoverlayrenderer.SizeOfImage)
		{
			spoofed_GetModuleInfo(FNV(L"gameoverlayrenderer.dll"), _gameoverlayrenderer);
		}
		return _gameoverlayrenderer;
	}

	MODULEINFO Memory::datacache()
	{
		while (!_datacache.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"datacache.dll"), _datacache);
		}
		return _datacache;
	}
    MODULEINFO Memory::steamapi()
    {
        while (!_steamapi.lpBaseOfDll)
        {
            spoofed_GetModuleInfo(FNV(L"steam_api.dll"), _steamapi);
        }
        return _steamapi;
    }

	MODULEINFO Memory::matchmaking()
	{
		while (!_matchmaking.lpBaseOfDll)
		{
			spoofed_GetModuleInfo(FNV(L"matchmaking.dll"), _matchmaking);
		}
		return _matchmaking;
	}
}
#undef DEBUG