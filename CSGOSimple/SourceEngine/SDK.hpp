#pragma once

#ifndef H_SDK
#define H_SDK
#include "Definitions.hpp"

#include "CRC.hpp"
#include "Vector.hpp"
#include "Vector2D.hpp"
#include "Vector4D.hpp"
#include "QAngle.hpp"
#include "CHandle.hpp"
#include "CGlobalVarsBase.hpp"
#include "ClientClass.hpp"
#include "Color.hpp"
#include "UtlMemory.hpp"
#include "IBaseClientDll.hpp"
#include "IClientEntity.hpp"
#include "IClientEntityList.hpp"
#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientThinkable.hpp"
#include "IClientUnknown.hpp"
#include "IPanel.hpp"
#include "ISurface.hpp"
#include "IVEngineClient.hpp"
#include "IEngineTrace.hpp"
#include "PlayerInfo.hpp"
#include "Recv.hpp"
#include "VMatrix.hpp"
#include "IClientMode.hpp"
#include "CInput.hpp"
#include "ICvar.hpp"
#include "Convar.hpp"
//#include "../IVDebugOverlay.h"
#include "../IStudioRender.h"
#include "../IVModelInfo.h"
#include "../IPhysicsSurfaceProps.h"
#include "../CPlayerResource.hpp"
#include "../IGameMovement.h"
#include "../IPrediction.h"
#include "../IMoveHelper.h"
#include "../CClientState.h"
#include "../CMemAlloc.h"
#include "../IFileSystem.h"
#include "../ILocalize.h"
#include "../IGameEvent.h"
#include "../CCStrike15ItemSchema.h"
#include "../ITextureDictionary.h"
#include "../CSteamNetworkingUtils.h"
#include "../LoggingSystem.h"
#include "../networkstringtabledefs.h"
#include "../ISteamClient.h"
#include <Psapi.h>
#include "../IMDLCache.h"
#include "../ISteamGameCoordinator.h"
#include "../CEngineSoundClient.h"
#include "../CGameUI.h"
#include "../IGameTypes.h"
#include "../matchmaking.h"
#include "../CCSCustomMaterialSwapManager.h"

//#include "../CPredictionSystem.h"


namespace SourceEngine
{
	void dump_all_interfaces();
    extern HSteamUser hSteamUser;
    extern HSteamPipe hSteamPipe;

	class Interfaces
	{
	public:
		constexpr Interfaces() noexcept: m_pEngine(nullptr), m_pClient(nullptr), m_pStudioRender(nullptr),
		                                 m_pEntityList(nullptr),
		                                 m_pGlobals(nullptr),
		                                 m_pVGuiPanel(nullptr),
		                                 m_pVGuiSurface(nullptr),
		                                 m_pInput(nullptr),
		                                 m_pEngineTrace(nullptr),
		                                 m_pCVar(nullptr),
		                                 m_pClientMode(nullptr),
		                                 m_pModelInfo(nullptr),
		                                 m_pPhysicsSurface(nullptr),
		                                 m_pInputSystem(nullptr),
		                                 m_pPlayerResource(nullptr),
		                                 m_pGameMovement(nullptr),
		                                 m_pPrediction(nullptr),
		                                 m_pMoveHelper(nullptr), m_pClientState(nullptr),
		                                 m_pMemAlloc(nullptr),
		                                 m_pFileSystem(nullptr),
		                                 m_pLocalizeSystem(nullptr),
		                                 m_pMaterialSystem(nullptr),
		                                 m_pGameEventManager(nullptr),
		                                 m_pStrike15ItemSchema(nullptr),
		                                 m_pTextureDictionary(nullptr),
		                                 m_pLoggingSystem(nullptr),
		                                 m_pSteamNetworkingUtils(nullptr),
		                                 m_pNetworkStringTableContainer(nullptr), m_pMDLCache(nullptr),
		                                 m_pSteamClient(nullptr),
		                                 m_pGameCoordinator(nullptr),
		                                 m_pClientLeafSystem(nullptr),
		                                 m_pEngineSoundClient(nullptr),
		                                 m_pInventoryManager(nullptr),
		                                 m_pSteamHTTP(nullptr),
		                                 m_pGameTypes(nullptr), m_pGameUI(nullptr),
		                                 m_pCustomMaterialSwapManager(nullptr), g_pMMF(nullptr),
		                                 m_pRenderToRTHelper(nullptr)
		{
		}

		IVEngineClient* Engine();
		IBaseClientDLL* Client();
		//IVDebugOverlay*				DebugOverlay();
		IStudioRender* StudioRender();
		IClientEntityList* EntityList();
		CGlobalVarsBase* GlobalVars();
		IPanel* VGUIPanel();
		ISurface* MatSurface();
		CInput* Input();
		IEngineTrace* EngineTrace();
		ICvar* CVar();
		IClientMode* ClientMode();
		IVModelInfo* ModelInfo();
		IPhysicsSurfaceProps* PhysicsSurface();
		IInputSystem* InputSystem();
		C_CSPlayerResource** PlayerResource();
		IGameMovement* GameMovement();
		IPrediction* Prediction();
		IMoveHelper* MoveHelper();
		CClientState* ClientState();
		CMemAlloc* MemAlloc();
		IFileSystem* FileSystem();
		CLocalize* LocalizeSystem();
		IMaterialSystem* MaterialSystem();
		CGameEventManager* GameEventManager();
		CCStrike15ItemSchema* ItemSchema();
		ITextureDictionary* TextureDictionary();
		LoggingSystem* GameLoggingSystem();
		CSteamNetworkingUtils* SteamNetworkingUtils();
		INetworkStringTableContainer* NetworkStringTableContainer();
		IMDLCache* MDLCache();
        ISteamClient* SteamClient();
        ISteamGameCoordinator* GameCoordinator();
		IClientLeafSystem* ClientLeafSystem();
		IEngineSound* EngineSoundClient();
		CSInventoryManager* InventoryManager();
		ISteamHTTP* SteamHTTP();
		IGameTypes* GameTypes();
		CGameUI* GameUI();
		CCSCustomMaterialSwapManager* CustomMaterialSwapManager();
		IMatchFramework* MatchFramework();
		void* RenderToRTHelper();
	private:
		IVEngineClient* m_pEngine;
		IBaseClientDLL* m_pClient;
		//IVDebugOverlay*				m_pDebugOverlay;
		IStudioRender* m_pStudioRender;
		IClientEntityList* m_pEntityList;
		CGlobalVarsBase* m_pGlobals;
		IPanel* m_pVGuiPanel;
		ISurface* m_pVGuiSurface;
		CInput* m_pInput;
		IEngineTrace* m_pEngineTrace;
		ICvar* m_pCVar;
		IClientMode* m_pClientMode;
		IVModelInfo* m_pModelInfo;
		IPhysicsSurfaceProps* m_pPhysicsSurface;
		IInputSystem* m_pInputSystem;
		C_CSPlayerResource** m_pPlayerResource;
		IGameMovement* m_pGameMovement;
		IPrediction* m_pPrediction;
		IMoveHelper* m_pMoveHelper;
		CClientState* m_pClientState;
		CMemAlloc* m_pMemAlloc;
		IFileSystem* m_pFileSystem;
		CLocalize* m_pLocalizeSystem;
		IMaterialSystem* m_pMaterialSystem;
		CGameEventManager* m_pGameEventManager;
		CCStrike15ItemSchema* m_pStrike15ItemSchema;
		ITextureDictionary* m_pTextureDictionary;
		LoggingSystem* m_pLoggingSystem;
		CSteamNetworkingUtils* m_pSteamNetworkingUtils;
		INetworkStringTableContainer* m_pNetworkStringTableContainer;
		IMDLCache* m_pMDLCache;
        ISteamClient* m_pSteamClient;
        ISteamGameCoordinator* m_pGameCoordinator;
		IClientLeafSystem* m_pClientLeafSystem;
		IEngineSound* m_pEngineSoundClient;		
		CSInventoryManager* m_pInventoryManager;
		ISteamHTTP* m_pSteamHTTP;
		IGameTypes* m_pGameTypes;
		CGameUI* m_pGameUI;
		CCSCustomMaterialSwapManager* m_pCustomMaterialSwapManager;
		IMatchFramework* g_pMMF;
		void* m_pRenderToRTHelper;
	};

	class Memory
	{
	public:
		constexpr Memory() noexcept : _client(), _engine(), _tier0(), _vguimatsurface(), _vgui2(), _vphysics(), _studiorender(),
		          _inputsystem(),
		          _filesystem_stdio(),
		          _materialsystem(),
		          _vstdlib(),
		          _localize(),
		          _shaderapidx9(),
		          _gameoverlayrenderer(), _datacache(),
		          _steamapi(),
		          _matchmaking(),
		          _dwViewMatrix(0)
		{
		}

		MODULEINFO client();
		MODULEINFO engine();
		MODULEINFO tier0();
		MODULEINFO vguimatsurface();
		MODULEINFO vgui2();
		MODULEINFO vphysics();
		MODULEINFO studiorender();
		MODULEINFO inputsystem();
		MODULEINFO filesystem_stdio();
		MODULEINFO materialsystem();
		MODULEINFO vstdlib();
		MODULEINFO localize();
		MODULEINFO shaderapidx9();
		MODULEINFO gameoverlayrenderer();
		MODULEINFO datacache();
        MODULEINFO steamapi();
		MODULEINFO matchmaking();
		ptrdiff_t dwViewMatrix();

	private:
		MODULEINFO _client;
		MODULEINFO _engine;
		MODULEINFO _tier0;
		MODULEINFO _vguimatsurface;
		MODULEINFO _vgui2;
		MODULEINFO _vphysics;
		MODULEINFO _studiorender;
		MODULEINFO _inputsystem;
		MODULEINFO _filesystem_stdio;
		MODULEINFO _materialsystem;
		MODULEINFO _vstdlib;
		MODULEINFO _localize;
		MODULEINFO _shaderapidx9;
		MODULEINFO _gameoverlayrenderer;
		MODULEINFO _datacache;
        MODULEINFO _steamapi;
		MODULEINFO _matchmaking;
		ptrdiff_t _dwViewMatrix;
	};

	extern Interfaces interfaces;
	extern Memory memory;

	namespace factory
	{		
		struct interface_info
		{
			interface_info() : m_CreateFn(nullptr), m_pName(nullptr) {};
			DWORD(*m_CreateFn)();
			const char* m_pName;
		};

		// credits to Altimor
		class CFactory
		{
		public:

			explicit CFactory(const HMODULE& module);
			explicit CFactory(const LPVOID& module);
			void Init(const ptrdiff_t& dwCreateInterface);
			std::vector<interface_info> dump() const;
			DWORD dwGetInterface(const char* pszName, int iVersion = 0) const;

		private:
			class InterfaceReg
			{
			public:
				InterfaceReg() :m_pNext(nullptr) {};
				interface_info interface_info_;
				InterfaceReg* m_pNext;
			};

			InterfaceReg* s_pInterfaceRegs;
		};

		void init();

		extern CFactory* Engine_factory;
		extern CFactory* Client_factory;
		extern CFactory* Matchmaking_factory;
		extern CFactory* Vgui_factory;
		extern CFactory* Vgui2_factory;
		extern CFactory* VStdLib_factory;
		extern CFactory* VPhysics_factory;
		extern CFactory* StudioRender_factory;
		extern CFactory* InputSystem_factory;
		extern CFactory* FileSystem_factory;
		extern CFactory* MaterialSystem_factory;
		extern CFactory* Localize_factory;
		extern CFactory* Datacache_factory;
	}
}

int64_t Helper_GetLastCompetitiveMatchId();
#endif
