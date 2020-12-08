// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include <process.h>
#include "CustomWinAPI.h"

uintptr_t initialize_thread_id = 0;

extern "C" BOOL WINAPI DllMain(HMODULE hModule, const DWORD dwReason, const LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{

		initialize_thread_id =	_beginthread(Hooks::Initialize, 0, hModule);//g_pWinApi->dwCreateSimpleThread(Hooks::Initialize, hModule, 0);
		
		//_beginthread(Hooks::Initialize, 0, nullptr);
	}
	else if (dwReason == DLL_PROCESS_DETACH && !lpReserved)
	{
		Hooks::Restore();
	}
	return TRUE;
}
