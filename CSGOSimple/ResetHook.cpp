// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>

#include "Hooks.hpp"
#include "ImGUI/DX9/imgui_impl_dx9.h"

void __cdecl on_reset(void*)
{
	Options::is_ready_draw = false;
	ImGui_ImplDX9_CreateDeviceObjects(); //Recreate GUI resources
	Options::is_ready_draw = true;
}

HRESULT __stdcall Hooks::Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	//Correctly handling Reset calls is very important if you have a DirectX hook.
	//IDirect3DDevice9::Reset is called when you minimize the game, Alt-Tab or change resolutions.
	//When it is called, the IDirect3DDevice9 is placed on "lost" state and many related resources are released
	//This means that we need to recreate our own resources when that happens. If we dont, we crash.

	//GUI wasnt initialized yet, just call Reset and return
	if (!g_bWasInitialized) return g_fnOriginalReset(pDevice, pPresentationParameters);

	//Device is on LOST state.
	ImGui_ImplDX9_InvalidateDeviceObjects(); //Invalidate GUI resources

	//banners cleanup
	//if (top_banner)
	//	top_banner->OnReset();
	//if (bottom_banner)
	//	bottom_banner->OnReset();

	//Call original Reset.
	const auto hr = g_fnOriginalReset(pDevice, pPresentationParameters);
	_beginthread(on_reset, 0, nullptr);
	Options::menu.first_launch = false;

	//banners recreate
#ifndef _PREMIUM_VERSION
	//if (top_banner)
	//	top_banner->CreateTextures(pDevice);
	//if (bottom_banner)
	//	bottom_banner->CreateTextures(pDevice);
#endif

	return hr;
}
