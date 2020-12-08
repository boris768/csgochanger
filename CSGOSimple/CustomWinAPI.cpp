// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CustomWinAPI.h"
#include "XorStr.hpp"
#include <intrin.h>
#include "fnv_hash.h"

std::unique_ptr<manual_winapi> g_pWinApi;

extern "C"
{
	ptrdiff_t __cdecl GetPEB();
	ptrdiff_t __cdecl GetTEB();
}

_LIST_ENTRY* GetFirstLoaderModule()
{
	//teb = *reinterpret_cast<ptrdiff_t*>(teb + 0x30);
	//teb = *reinterpret_cast<ptrdiff_t*>(teb + 0x0C);
	//teb = *reinterpret_cast<ptrdiff_t*>(teb + 0x0C);

	//_TEB32* teb = reinterpret_cast<_TEB32*>(GetTEB());
	_PEB* const peb = reinterpret_cast<_PEB*>(GetPEB());
	_PEB_LDR_DATA* const ldr_data = peb->Ldr;
	
	return &ldr_data->InLoadOrderModuleList;
}

void __stdcall spoofed_GetModuleInfo(fnv::hash lpModuleName, MODULEINFO& moduleinfo)
{
	moduleinfo = {nullptr, 0, nullptr};

	const PLIST_ENTRY pHeadEntry = GetFirstLoaderModule();

	PLIST_ENTRY pEntry = pHeadEntry->Flink;
	while (pEntry != pHeadEntry)
	{
		// Retrieve the current MODULE_ENTRY
		const PLDR_MODULE pLdrEntry = CONTAINING_RECORD(pEntry, LDR_MODULE, InLoadOrderLinks);
		if (pLdrEntry->SizeOfImage)
		{
			if (pLdrEntry->BaseDllName.Buffer)
			{
				if (fnv::hash_runtime_lower(pLdrEntry->BaseDllName.Buffer) == lpModuleName)
				{
					moduleinfo.EntryPoint = pLdrEntry->EntryPoint;
					moduleinfo.SizeOfImage = pLdrEntry->SizeOfImage;
					moduleinfo.lpBaseOfDll = pLdrEntry->DllBase;
					return;
				}
			}
		}

		// Iterate to the next entry
		pEntry = pEntry->Flink;
	}

}

HMODULE __stdcall spoofed_GetModuleHandleW_hash(fnv::hash lpModuleName)
{
	const PLIST_ENTRY pHeadEntry = GetFirstLoaderModule();

	PLIST_ENTRY pEntry = pHeadEntry->Flink;
	while (pEntry != pHeadEntry)
	{
		// Retrieve the current MODULE_ENTRY
		const PLDR_MODULE pLdrEntry = CONTAINING_RECORD(pEntry, LDR_MODULE, InLoadOrderLinks);
		if (pLdrEntry->SizeOfImage)
		{
			if (pLdrEntry->BaseDllName.Buffer)
			{
				if (fnv::hash_runtime_lower(pLdrEntry->BaseDllName.Buffer) == lpModuleName)
				{
					return static_cast<HMODULE>(pLdrEntry->DllBase);
				}
			}
		}

		// Iterate to the next entry
		pEntry = pEntry->Flink;
	}
	return nullptr;
}

HMODULE __stdcall spoofed_GetModuleHandleW(IN const LPCWSTR lpModuleName)
{
	const PLIST_ENTRY pHeadEntry = GetFirstLoaderModule();

	PLIST_ENTRY pEntry = pHeadEntry->Flink;
	while (pEntry != pHeadEntry)
	{
		// Retrieve the current MODULE_ENTRY
		const PLDR_MODULE pLdrEntry = CONTAINING_RECORD(pEntry, LDR_MODULE, InLoadOrderLinks);
		if (pLdrEntry->SizeOfImage)
		{
			if (pLdrEntry->BaseDllName.Buffer)
			{
				if (_wcsicmp(pLdrEntry->BaseDllName.Buffer, lpModuleName) == 0)
				{
					return static_cast<HMODULE>(pLdrEntry->DllBase);
				}
			}
		}

		// Iterate to the next entry
		pEntry = pEntry->Flink;
	}
	return nullptr;
}

manual_winapi::manual_winapi()
{
	Kernel32Address = GetKernel32Address();
	dwGetModuleHandleA = reinterpret_cast<GetModuleHandleA_t>(GetProcAddress(Kernel32Address, XorStr("GetModuleHandleA")));
	dwGetModuleHandleW = spoofed_GetModuleHandleW;//reinterpret_cast<GetModuleHandleW_t>(spoofed_GetModuleHandleW);//reinterpret_cast<GetModuleHandleW_t>(GetProcAddress(Kernel32Address, XorStr("GetModuleHandleW")));
	dwLoadLibrary = reinterpret_cast<LoadLibrary_t>(GetProcAddress(Kernel32Address, XorStr("LoadLibraryA")));
	
	dwSleep = reinterpret_cast<Sleep_t>(GetProcAddress(Kernel32Address, XorStr("Sleep")));
	dwGetCurrentProcess = reinterpret_cast<GetCurrentProcess_t>(GetProcAddress(Kernel32Address, XorStr("GetCurrentProcess")));
	dwCreateFileW = reinterpret_cast<CreateFileW_t>(GetProcAddress(Kernel32Address, XorStr("CreateFileW")));
	dwCreateFileA = reinterpret_cast<CreateFileA_t>(GetProcAddress(Kernel32Address, XorStr("CreateFileA")));
	dwReadFile = reinterpret_cast<ReadFile_t>(GetProcAddress(Kernel32Address, XorStr("ReadFile")));
	dwGetCurrentThreadId = reinterpret_cast<GetCurrentThreadId_t>(GetProcAddress(Kernel32Address, XorStr("GetCurrentThreadId")));
	dwGetCurrentProcessId = reinterpret_cast<GetCurrentProcessId_t>(GetProcAddress(Kernel32Address, XorStr("GetCurrentProcessId")));
	dwVirtualProtect = reinterpret_cast<VirtualProtect_t>(GetProcAddress(Kernel32Address, XorStr("VirtualProtect")));
	dwVirtualQuery = reinterpret_cast<VirtualQuery_t>(GetProcAddress(Kernel32Address, XorStr("VirtualQuery")));
	dwVirtualAlloc = reinterpret_cast<VirtualAlloc_t>(GetProcAddress(Kernel32Address, XorStr("VirtualAlloc")));
	dwVirtualFree = reinterpret_cast<VirtualFree_t>(GetProcAddress(Kernel32Address, XorStr("VirtualFree")));
	dwIsBadCodePtr = reinterpret_cast<IsBadCodePtr_t>(GetProcAddress(Kernel32Address, XorStr("IsBadCodePtr")));
	dwAttachConsole = reinterpret_cast<AttachConsole_t>(GetProcAddress(Kernel32Address, XorStr("AttachConsole")));
	dwAllocConsole = reinterpret_cast<AllocConsole_t>(GetProcAddress(Kernel32Address, XorStr("AllocConsole")));
	dwSwitchToThread = reinterpret_cast<SwitchToThread_t>(GetProcAddress(Kernel32Address, XorStr("SwitchToThread")));
	dwGetConsoleWindow = reinterpret_cast<GetConsoleWindow_t>(GetProcAddress(Kernel32Address, XorStr("GetConsoleWindow")));
	dwDisableThreadLibraryCalls = reinterpret_cast<DisableThreadLibraryCalls_t>(GetProcAddress(Kernel32Address, XorStr("DisableThreadLibraryCalls")));
	dwTerminateThread = reinterpret_cast<TerminateThread_t>(GetProcAddress(Kernel32Address, XorStr("TerminateThread")));
	dwBeep = reinterpret_cast<Beep_t>(GetProcAddress(Kernel32Address, XorStr("Beep")));
	dwGetWindowsDirectoryA = reinterpret_cast<GetWindowsDirectoryA_t>(GetProcAddress(Kernel32Address, XorStr("GetWindowsDirectoryA")));

	//const auto fuck_u_visual_studio = XorStr(L"User32.dll");
	const auto User32Address = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"user32.dll")));
	dwCallWindowProcW = reinterpret_cast<CallWindowProcW_t>(GetProcAddress(User32Address, XorStr("CallWindowProcW")));
	dwShowWindow = reinterpret_cast<ShowWindow_t>(GetProcAddress(User32Address, XorStr("ShowWindow")));
	dwFindWindowA = reinterpret_cast<FindWindowA_t>(GetProcAddress(User32Address, XorStr("FindWindowA")));
	dwFlashWindowEx = reinterpret_cast<FlashWindowEx_t>(GetProcAddress(User32Address, XorStr("FlashWindowEx")));
	dwSetActiveWindow = reinterpret_cast<SetActiveWindow_t>(GetProcAddress(User32Address, XorStr("SetActiveWindow")));
	dwGetClientRect = reinterpret_cast<GetClientRect_t>(GetProcAddress(User32Address, XorStr("GetClientRect")));
	dwMessageBoxW = reinterpret_cast<MessageBoxW_t>(GetProcAddress(User32Address, XorStr("MessageBoxW")));
	dwSetWindowLongW = reinterpret_cast<SetWindowLongW_t>(GetProcAddress(User32Address, XorStr("SetWindowLongW")));
	dwGetForegroundWindow = reinterpret_cast<GetForegroundWindow_t>(GetProcAddress(User32Address, XorStr("GetForegroundWindow")));
	dwGetWindowLongW = reinterpret_cast<GetWindowLongW_t>(GetProcAddress(User32Address, XorStr("GetWindowLongW")));
	dwGetWindowRect = reinterpret_cast<GetWindowRect_t>(GetProcAddress(User32Address, XorStr("GetWindowRect")));
	dwSetCursor = reinterpret_cast<SetCursor_t>(GetProcAddress(User32Address, XorStr("SetCursor")));
	dwGetCursor = reinterpret_cast<GetCursor_t>(GetProcAddress(User32Address, XorStr("GetCursor")));
	dwSetCursorPos = reinterpret_cast<SetCursorPos_t>(GetProcAddress(User32Address, XorStr("SetCursorPos")));
	dwGetCursorPos = reinterpret_cast<GetCursorPos_t>(GetProcAddress(User32Address, XorStr("GetCursorPos")));
	dwClientToScreen = reinterpret_cast<ClientToScreen_t>(GetProcAddress(User32Address, XorStr("ClientToScreen")));
	dwScreenToClient = reinterpret_cast<ScreenToClient_t>(GetProcAddress(User32Address, XorStr("ScreenToClient")));
	dwSendMessageW = reinterpret_cast<SendMessageW_t>(GetProcAddress(User32Address, XorStr("SendMessageW")));
	dwGetKeyState = reinterpret_cast<GetKeyState_t>(GetProcAddress(User32Address, XorStr("GetKeyState")));
	dwIsChild = reinterpret_cast<IsChild_t>(GetProcAddress(User32Address, XorStr("IsChild")));
	dwSetCapture = reinterpret_cast<SetCapture_t>(GetProcAddress(User32Address, XorStr("SetCapture")));
	dwReleaseCapture = reinterpret_cast<ReleaseCapture_t>(GetProcAddress(User32Address, XorStr("ReleaseCapture")));

	const auto Shell32Address = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"shell32.dll")));
	dwShellExecuteA = reinterpret_cast<ShellExecuteA_t>(GetProcAddress(Shell32Address, XorStr("ShellExecuteA")));

	const auto ntdllAddress = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"ntdll.dll")));
	dwRtlAddVectoredExceptionHandler = reinterpret_cast<AddVectoredExceptionHandler_t>(GetProcAddress(ntdllAddress, XorStr("RtlAddVectoredExceptionHandler")));
	dwRtlRaiseException = reinterpret_cast<RtlRaiseException_t>(GetProcAddress(ntdllAddress, XorStr("RtlRaiseException")));
	dwNtContinue = reinterpret_cast<NtContinue_t>(GetProcAddress(ntdllAddress, XorStr("NtContinue")));
	dwNtRaiseException = reinterpret_cast<NtRaiseException_t>(GetProcAddress(ntdllAddress, XorStr("NtRaiseException")));
	dwNtReadVirtualMemory = reinterpret_cast<NtReadVirtualMemory_t>(GetProcAddress(ntdllAddress, XorStr("NtReadVirtualMemory")));
	dwRtlImageNtHeader = reinterpret_cast<RtlImageNtHeader_t>(GetProcAddress(ntdllAddress, XorStr("RtlImageNtHeader")));

	const auto shlwapiAddress = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"shlwapi.dll")));

	dwPathIsRelativeA = reinterpret_cast<PathIsRelativeA_t>(GetProcAddress(shlwapiAddress, XorStr("PathIsRelativeA")));
	dwPathIsRelativeW = reinterpret_cast<PathIsRelativeW_t>(GetProcAddress(shlwapiAddress, XorStr("PathIsRelativeW")));
	
	unsigned counter = 100;
	
	DWORD tier0_Address = 0;
	while (!tier0_Address)
	{
		--counter;
		tier0_Address = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"tier0.dll")));
		if (dwSwitchToThread() == 0)
		{
			dwSleep(100);
		}
	}
	if (tier0_Address == 0)
		tier0_Address = reinterpret_cast<DWORD>(dwLoadLibrary(XorStr("tier0.dll")));
	dwCreateSimpleThread = reinterpret_cast<CreateSimpleThread_t>(GetProcAddress(tier0_Address, XorStr("CreateSimpleThread")));

	DWORD d3dx9_43Address = 0;
	counter = 10;
	while (!d3dx9_43Address)
	{
		--counter;
		d3dx9_43Address = reinterpret_cast<DWORD>(spoofed_GetModuleHandleW_hash(FNV(L"d3dx9_43.dll")));
		if(dwSwitchToThread() == 0)
		{
			dwSleep(100);
		}
	}
	if(d3dx9_43Address == 0)
		d3dx9_43Address = reinterpret_cast<DWORD>(dwLoadLibrary(XorStr("d3dx9_43.dll")));
	dwD3DXCreateTextureFromFileInMemoryEx = reinterpret_cast<D3DXCreateTextureFromFileInMemoryEx_t>(GetProcAddress(d3dx9_43Address, XorStr("D3DXCreateTextureFromFileInMemoryEx")));;
	dwD3DXCreateTextureFromFileExW = reinterpret_cast<D3DXCreateTextureFromFileExW_t>(GetProcAddress(d3dx9_43Address, XorStr("D3DXCreateTextureFromFileExW")));
}

DWORD manual_winapi::GetKernel32Address()
{
	ptrdiff_t teb = GetPEB();
	teb = *reinterpret_cast<ptrdiff_t*>(teb + 0x0C);
	teb = *reinterpret_cast<ptrdiff_t*>(teb + 0x14);
	teb = *reinterpret_cast<ptrdiff_t*>(teb);
	teb = *reinterpret_cast<ptrdiff_t*>(teb);
	const DWORD kernelAddr = *reinterpret_cast<DWORD*>(teb + 0x10);
	return kernelAddr;
}


PVOID manual_winapi::GetProcAddress(HMODULE dwModule, const char* szProcName) const
{
	return GetProcAddress(reinterpret_cast<DWORD>(dwModule), szProcName);
}

PVOID manual_winapi::GetProcAddress(const DWORD dwModule, const char* szProcName) const
{
	char* module = reinterpret_cast<char*>(dwModule);

	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
	auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(module + dosHeader->e_lfanew);
	const PIMAGE_OPTIONAL_HEADER optHeader = &ntHeader->OptionalHeader;
	const auto expEntry = static_cast<PIMAGE_DATA_DIRECTORY>(&optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	const auto expDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(module + expEntry->VirtualAddress);

	auto* funcTable = reinterpret_cast<PVOID*>(module + expDir->AddressOfFunctions);
	const auto ordTable = reinterpret_cast<PWORD>(module + expDir->AddressOfNameOrdinals);
	const auto nameTable = reinterpret_cast<PCHAR*>(module + expDir->AddressOfNames);

	void* address = nullptr;

	if (reinterpret_cast<DWORD>(szProcName) >> 16 == 0)
	{
		const WORD ord = LOWORD(szProcName);
		const DWORD ordBase = expDir->Base;

		if (ord < ordBase || ord > ordBase + expDir->NumberOfFunctions)
			return nullptr;

		address = static_cast<PVOID>(module + reinterpret_cast<DWORD>(funcTable[ord - ordBase]));
	}
	else
	{
		for (size_t i = 0; i < expDir->NumberOfNames; ++i)
		{
			if (!strcmp(szProcName, module + reinterpret_cast<DWORD>(nameTable[i])))
			{
				address = static_cast<PVOID>(module + reinterpret_cast<DWORD>(funcTable[ordTable[i]]));
				break;
			}
		}
	}
	if(!address)
	{
		return nullptr;
	}

	if (static_cast<char*>(address) >= reinterpret_cast<char*>(expDir)
		&& (static_cast<char*>(address) < reinterpret_cast<char*>(expDir) + expEntry->Size))
	{
		char* dllName = _strdup(static_cast<char*>(address));

		if (!dllName)
			return nullptr;

		address = nullptr;

		char* funcName = strchr(dllName, '.');
		*funcName++ = 0;

		const size_t len = strlen(dllName) + 1;
		auto*dllNameW = new wchar_t[strlen(dllName) + 1];

		mbstowcs(dllNameW, dllName, len);

		HMODULE forwardModule = dwGetModuleHandleW(dllNameW);
		if (!forwardModule)
		{
			forwardModule = dwLoadLibrary(dllName);
		}

		if (forwardModule)
			address = GetProcAddress(reinterpret_cast<DWORD>(forwardModule), funcName);

		delete [] dllNameW;
		free(dllName);
		free(funcName);
	}

	return address;
}
