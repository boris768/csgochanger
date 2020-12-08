// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include "ImGUI/DX9/imgui_impl_dx9.h"
#include "Menu.h"
#include "ImGUI/win32/imgui_impl_win32.h"
#include "Utils.hpp"

bool* g_bInThreadedBoneSetup = nullptr;
bool* g_bDoThreadedBoneSetup = nullptr;
bool isDown = false;
bool isClicked = false;
HWND saved_hwnd = nullptr;
HRESULT _stdcall Hooks::Hooked_Present(const LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect,
                                       const HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
#ifdef CHANGER_PROFILE
	__itt_task_begin(domain, __itt_null, __itt_null, Present_work);
#endif
	if (!g_bWasInitialized)
	{
		debug(L"Present_Hook: g_bWasInitialized was %d, trying gui init", g_bWasInitialized);
		GUI_Init(pDevice);
#ifndef _PREMIUM_VERSION
		//if (top_banner)
		//	top_banner->CreateTextures(pDevice);
		//if (bottom_banner)
		//	bottom_banner->CreateTextures(pDevice);
#endif
	}
	else
	{

		const auto inputsystem = SourceEngine::interfaces.InputSystem();

		is_pressed_random = inputsystem->IsButtonDown(Options::random_skin_key);
		const bool is_pressed_menu = inputsystem->IsButtonDown(Options::menu_key);
		if (is_pressed_menu)
		{
			isClicked = false;
			isDown = true;
		}
		else
		{
			isClicked = isDown;
			isDown = false;
		}
		if (isClicked)
		{
			Options::menu.main_window = !Options::menu.main_window;
			Options::menu.was_opened = false;
		}

		if (Options::menu.main_window)
		{
			//inputsystem->ResetInputState();
			std::swap(saved_hwnd, inputsystem->m_hAttachedHWnd);
			
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (Options::is_ready_draw)
			{
				//Gui::GetIO().MouseDrawCursor = Options::menu.main_window;

				DrawMenu();
			}
#ifdef _DEBUG
			ImGui::Begin("debug");

			//
			//g_bInThreadedBoneSetup = *(bool**)(Utils::FindSignature(SourceEngine::memory.client(), XorStr("C6 05 ? ? ? ? 01 81")) + 2);
			//g_bDoThreadedBoneSetup = *(bool**)(Utils::FindSignature(SourceEngine::memory.client(), XorStr("C6 05 ? ? ? ? 01 83 FE 01")) + 2);
			//ImGui::Text("g_bDoThreadedBoneSetup: %d, g_bInThreadedBoneSetup: %d", *g_bDoThreadedBoneSetup, *g_bInThreadedBoneSetup);
			
			auto rules = C_CSGameRulesProxy::GetGameRulesProxy();
			if (rules)
			{
				ImGui::Text("Round time: %f", SourceEngine::interfaces.GlobalVars()->curtime - rules->GetRoundStartTime());
				ImGui::Text("IsWarmupPeriod: %s", rules->IsWarmupPeriod() ? "true" : "false");
				ImGui::Text("IsValveDS: %s", rules->IsValveDS() ? "true" : "false");

				auto resource = C_CSPlayerResource::GetPlayerResource();
				if (resource)
					ImGui::Text("m_iScore: %d", resource->GetScore(SourceEngine::interfaces.Engine()->GetLocalPlayer()));
				auto teamT = C_CSTeam::GetTeamT();
				auto teamCT = C_CSTeam::GetTeamCT();
				if (teamT && teamCT)
				{
					ImGui::Text("%s", "Terrorists");
					ImGui::Text("Score: %d", teamT->GetscoreTotal());
					ImGui::Text("Surrended: %s", teamT->IsSurrended() ? "true" : "false");

					ImGui::Text("%s", "Counter-Terrorists");
					ImGui::Text("Score: %d", teamCT->GetscoreTotal());
					ImGui::Text("Surrended: %s", teamCT->IsSurrended() ? "true" : "false");
				}
			}
			ImGui::End();
#endif
			ImGui::EndFrame();

			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		else
		{
			if (saved_hwnd)
			{
				inputsystem->EnableInput(true);
				std::swap(saved_hwnd, inputsystem->m_hAttachedHWnd);
			}
		}
	}
#ifdef CHANGER_PROFILE
	__itt_task_end(domain, __itt_null, __itt_null, Present_work);
#endif
	//Call original Present now
	return g_fnOriginalPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
