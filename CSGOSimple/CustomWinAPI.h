#pragma once
#include <windows.h>
#include <Psapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "fnv_hash.h"
#include <memory>

template <class T>
struct LIST_ENTRY_T
{
	T Flink;
	T Blink;
};

template <class T>
struct UNICODE_STRING_T
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};
		T dummy;
	};
	T _Buffer;
};

struct _PEB_LDR_DATA
{
	ULONG Length;                                                           //0x0
	UCHAR Initialized;                                                      //0x4
	void* SsHandle;                                                         //0x8
	struct _LIST_ENTRY InLoadOrderModuleList;                               //0xc
	struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x14
	struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x1c
	void* EntryInProgress;                                                  //0x24
	UCHAR ShutdownInProgress;                                               //0x28
	void* ShutdownThreadId;                                                 //0x2c
};

struct _PEB
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;                                                    //0x2
	union
	{
		//dont use, windows version specific
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsLegacyProcess : 1;                                        //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR SpareBits : 3;                                              //0x3
		};
	};
	void* Mutant;                                                           //0x4
	void* ImageBaseAddress;                                                 //0x8
	struct _PEB_LDR_DATA* Ldr;                                              //0xc
	struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;                 //0x10
	void* SubSystemData;                                                    //0x14
	void* ProcessHeap;                                                      //0x18
};

struct _CLIENT_ID32
{
	ULONG UniqueProcess;                                                    //0x0
	ULONG UniqueThread;                                                     //0x4
};

struct NT_TIB32_
{
	ULONG ExceptionList;                                                    //0x0
	ULONG StackBase;                                                        //0x4
	ULONG StackLimit;                                                       //0x8
	ULONG SubSystemTib;                                                     //0xc
	union
	{
		ULONG FiberData;                                                    //0x10
		ULONG Version;                                                      //0x10
	};
	ULONG ArbitraryUserPointer;                                             //0x14
	ULONG Self;                                                             //0x18
};

struct _TEB32
{
	NT_TIB32_ NtTib;                                                        //0x0
	ULONG EnvironmentPointer;                                               //0x1c
	_CLIENT_ID32 ClientId;                                                  //0x20
	ULONG ActiveRpcHandle;                                                  //0x28
	ULONG ThreadLocalStoragePointer;                                        //0x2c
	_PEB* ProcessEnvironmentBlock;                                         //0x30
	ULONG LastErrorValue;                                                   //0x34
	ULONG CountOfOwnedCriticalSections;                                     //0x38
	ULONG CsrClientThread;                                                  //0x3c
	ULONG Win32ThreadInfo;                                                  //0x40
	ULONG User32Reserved[26];                                               //0x44
	ULONG UserReserved[5];                                                  //0xac
	ULONG WOW32Reserved;                                                    //0xc0
	//windows-version specific pad
};

typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID ExitStatus;
	_PEB* PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

#pragma pack(push, 1) 
typedef struct _LDR_DATA_TABLE_ENTRY {

	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;

} LDR_MODULE, *PLDR_MODULE;
#pragma pack(pop)

struct ModuleInfoNode
{
	LIST_ENTRY LoadOrder;
	LIST_ENTRY InitOrder;
	LIST_ENTRY MemoryOrder;
	HMODULE BaseAddress;		// base address AKA module handle
	unsigned long EntryPoint;
	unsigned int Size;			// size of the modules image
	UNICODE_STRING FullPath;
	UNICODE_STRING Name;
	unsigned long Flags;
	unsigned short LoadCount;
	unsigned short TlsIndex;
	LIST_ENTRY HashTable;		// linked list of any other modules that have the same first letter
	unsigned long Timestamp;
};

class manual_winapi
{
	using Beep_t = BOOL	(__stdcall*) (_In_ DWORD dwFreq, _In_ DWORD dwDuration);
	using GetModuleHandleW_t = HMODULE(__stdcall*)(_In_opt_ LPCWSTR lpModuleName);
	using GetModuleHandleA_t = HMODULE(__stdcall*)(_In_opt_ LPCSTR lpModuleName);
	using LoadLibrary_t = HMODULE(__stdcall*)(_In_ LPCSTR lpFileName);

	using Sleep_t = VOID(__stdcall*)(DWORD dwMilliseconds);
	using GetCurrentProcess_t = HANDLE(__stdcall*)();
	using CreateFileW_t = HANDLE(__stdcall*)(
		_In_     LPCTSTR lpFileName,
		         _In_     DWORD dwDesiredAccess,
		         _In_     DWORD dwShareMode,
		         _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		         _In_     DWORD dwCreationDisposition,
		         _In_     DWORD dwFlagsAndAttributes,
		         _In_opt_ HANDLE hTemplateFile);
	using CreateFileA_t = HANDLE(__stdcall*)(
		LPCSTR filename,
		DWORD access,
		DWORD sharing,
		LPSECURITY_ATTRIBUTES sa,
		DWORD creation,
		DWORD attributes,
		HANDLE templateFile);
	using ReadFile_t = BOOL(__stdcall*)(
		_In_        HANDLE hFile,
		            _Out_       LPVOID lpBuffer,
		            _In_        DWORD nNumberOfBytesToRead,
		            _Out_opt_   LPDWORD lpNumberOfBytesRead,
		            _Inout_opt_ LPOVERLAPPED lpOverlapped
	);
	using GetCurrentThreadId_t = DWORD(__stdcall*)();
	using GetCurrentProcessId_t = DWORD(__stdcall*)();
	using VirtualProtect_t = BOOL(__stdcall*)(
		_In_  LPVOID lpAddress,
		      _In_  SIZE_T dwSize,
		      _In_  DWORD flNewProtect,
		      _Out_ PDWORD lpflOldProtect);
	using AttachConsole_t = BOOL(__stdcall *)(unsigned int);
	using AllocConsole_t = BOOL(__stdcall *)();
	using SwitchToThread_t = BOOL(__stdcall *)();
	using GetConsoleWindow_t = HWND(__stdcall *)();
	using DisableThreadLibraryCalls_t = BOOL(__stdcall *)(_In_ HMODULE hModule);
	using TerminateThread_t = BOOL(__stdcall*)(_Inout_ HANDLE hThread, _In_ DWORD  dwExitCode);

	using VirtualQuery_t = SIZE_T(__stdcall*)(
		_In_opt_ LPCVOID lpAddress,
		         _Out_    PMEMORY_BASIC_INFORMATION lpBuffer,
		         _In_     SIZE_T dwLength);
	using VirtualAlloc_t = LPVOID(__stdcall*)(LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType, DWORD flProtect);
	using VirtualFree_t = BOOL(__stdcall*)(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

	using IsBadCodePtr_t = BOOL(__stdcall*)(_In_ FARPROC lpfn);

	using CallWindowProcW_t = LRESULT(__stdcall*)(_In_ WNDPROC lpPrevWndFunc, _In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	using ShowWindow_t = BOOL(__stdcall*)(_In_ HWND hWnd,_In_ int  nCmdShow	);
	using FindWindowA_t = HWND(__stdcall*)(_In_opt_ LPCSTR lpClassName,_In_opt_ LPCSTR lpWindowName);
	using FlashWindowEx_t = BOOL(__stdcall*)(_In_ PFLASHWINFO pfwi);
	using SetActiveWindow_t = HWND(__stdcall*)(_In_ HWND hWnd);
	using GetClientRect_t = BOOL(__stdcall*)(_In_  HWND hWnd, _Out_ LPRECT lpRect);
	using MessageBoxW_t = int(__stdcall*)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType);
	using SetWindowLongW_t = LONG(__stdcall*)(_In_ HWND hWnd, _In_ int nIndex, _In_ LONG dwNewLong);
	using GetForegroundWindow_t = HWND(__stdcall*)();
	using GetWindowLongW_t = LONG(__stdcall*)(_In_ HWND hWnd, _In_ int nIndex);
	using GetWindowRect_t = BOOL(__stdcall*)(_In_ HWND hWnd, _Out_ LPRECT lpRect);
	using SetCursor_t = HCURSOR(__stdcall*)(_In_opt_ HCURSOR hCursor);
	using GetCursor_t = HCURSOR(__stdcall*)();
	using GetKeyState_t = SHORT(__stdcall*)(_In_ int nVirtKey);
	using SetCursorPos_t = BOOL(__stdcall*)(_In_ int X,	_In_ int Y);
	using GetCursorPos_t = BOOL(__stdcall*)(LPPOINT lpPoint);
	using SendMessageW_t = LRESULT(__stdcall*)(_In_ HWND   hWnd, _In_ UINT   Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	using ClientToScreen_t = BOOL(__stdcall*)(HWND hWnd, LPPOINT lpPoint);
	using ScreenToClient_t = BOOL(__stdcall*)(_In_ HWND hWnd, _Inout_ LPPOINT lpPoint);
	using IsChild_t = BOOL(__stdcall*)(_In_ HWND ,_In_ HWND );
	using SetCapture_t = HWND(__stdcall*)(_In_ HWND hWnd);
	using ReleaseCapture_t = HWND(__stdcall*)();

	using AddVectoredExceptionHandler_t = PVOID(__stdcall*)(_In_ ULONG FirstHandler, _In_ PVECTORED_EXCEPTION_HANDLER VectoredHandler);
	using RtlRaiseException_t = void(__stdcall*)(PEXCEPTION_RECORD ExceptionRecord);
	using NtContinue_t = NTSTATUS(__stdcall*)(IN PCONTEXT ThreadContext, IN BOOLEAN RaiseAlert);
	using NtRaiseException_t = NTSTATUS(__stdcall*)(IN PEXCEPTION_RECORD ExceptionRecord, IN PCONTEXT ThreadContext, IN BOOLEAN HandleException);
	using NtReadVirtualMemory_t = NTSTATUS(__stdcall*)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer,	IN ULONG NumberOfBytesToRead, OUT PULONG NumberOfBytesReaded OPTIONAL);
	using RtlImageNtHeader_t = PIMAGE_NT_HEADERS(__stdcall*)(IN PVOID ModuleAddress);
	using ShellExecuteA_t = HINSTANCE(__stdcall*)(_In_opt_ HWND hwnd, _In_opt_ LPCSTR lpOperation, _In_ LPCSTR lpFile, _In_opt_ LPCSTR lpParameters, _In_opt_ LPCSTR lpDirectory, _In_ INT nShowCmd);

	using D3DXCreateTextureFromFileInMemoryEx_t = HRESULT(__stdcall*)
		(
			LPDIRECT3DDEVICE9         pDevice,
			LPCVOID                   pSrcData,
			UINT                      SrcDataSize,
			UINT                      Width,
			UINT                      Height,
			UINT                      MipLevels,
			DWORD                     Usage,
			D3DFORMAT                 Format,
			D3DPOOL                   Pool,
			DWORD                     Filter,
			DWORD                     MipFilter,
			D3DCOLOR                  ColorKey,
			D3DXIMAGE_INFO* pSrcInfo,
			PALETTEENTRY* pPalette,
			LPDIRECT3DTEXTURE9* ppTexture);
	using D3DXCreateTextureFromFileExW_t =	HRESULT(__stdcall*)(
			LPDIRECT3DDEVICE9         pDevice,
			LPCWSTR                   pSrcFile,
			UINT                      Width,
			UINT                      Height,
			UINT                      MipLevels,
			DWORD                     Usage,
			D3DFORMAT                 Format,
			D3DPOOL                   Pool,
			DWORD                     Filter,
			DWORD                     MipFilter,
			D3DCOLOR                  ColorKey,
			D3DXIMAGE_INFO* pSrcInfo,
			PALETTEENTRY* pPalette,
			LPDIRECT3DTEXTURE9* ppTexture);

	using GetWindowsDirectoryA_t = UINT (__stdcall*)
		(
			_In_ LPSTR lpBuffer,
			_In_ UINT uSize
		);

	typedef unsigned long ThreadId_t;
	using ThreadFunc_t = unsigned (__cdecl* )(void* pParam);
	using CreateSimpleThread_t = HANDLE(__cdecl*)(ThreadFunc_t, void*, SIZE_T);
	
	using PathIsRelativeA_t = BOOL(__stdcall*)(_In_ LPCSTR lpszPath);
	using PathIsRelativeW_t = BOOL(__stdcall*)(_In_ LPCWSTR lpszPath);
public:
	manual_winapi();
	static DWORD GetKernel32Address();
	PVOID GetProcAddress(HMODULE dwModule, const char* szProcName) const;
	PVOID GetProcAddress(DWORD dwModule, const char* szProcName) const;

	GetModuleHandleW_t dwGetModuleHandleW;
	GetModuleHandleA_t dwGetModuleHandleA;
	LoadLibrary_t dwLoadLibrary;

	Sleep_t dwSleep;
	GetCurrentProcess_t dwGetCurrentProcess;
	CreateFileW_t dwCreateFileW;
	CreateFileA_t dwCreateFileA;
	ReadFile_t dwReadFile;
	GetCurrentThreadId_t dwGetCurrentThreadId;
	GetCurrentProcessId_t dwGetCurrentProcessId;

	//kernel32.dll
	VirtualProtect_t dwVirtualProtect;
	VirtualQuery_t dwVirtualQuery;
	VirtualAlloc_t dwVirtualAlloc;
	VirtualFree_t dwVirtualFree;
	IsBadCodePtr_t dwIsBadCodePtr;
	AttachConsole_t dwAttachConsole;
	AllocConsole_t dwAllocConsole;
	SwitchToThread_t dwSwitchToThread;
	GetConsoleWindow_t dwGetConsoleWindow;
	DisableThreadLibraryCalls_t dwDisableThreadLibraryCalls;
	TerminateThread_t dwTerminateThread;
	Beep_t dwBeep;
	GetWindowsDirectoryA_t dwGetWindowsDirectoryA;
	
	//user32.dll
	CallWindowProcW_t dwCallWindowProcW;
	ShowWindow_t dwShowWindow;
	FindWindowA_t dwFindWindowA;
	FlashWindowEx_t dwFlashWindowEx;
	SetActiveWindow_t dwSetActiveWindow;
	GetClientRect_t dwGetClientRect;
	MessageBoxW_t dwMessageBoxW;
	SetWindowLongW_t dwSetWindowLongW;
	GetForegroundWindow_t dwGetForegroundWindow;
	GetWindowLongW_t dwGetWindowLongW;
	GetWindowRect_t dwGetWindowRect;
	SetCursor_t dwSetCursor;
	GetCursor_t dwGetCursor;
	SetCursorPos_t dwSetCursorPos;
	GetCursorPos_t dwGetCursorPos;
	ClientToScreen_t dwClientToScreen;
	ScreenToClient_t dwScreenToClient;
	SendMessageW_t dwSendMessageW;
	GetKeyState_t dwGetKeyState;
	IsChild_t dwIsChild;
	SetCapture_t dwSetCapture;
	ReleaseCapture_t dwReleaseCapture;

	//shell32.dll
	ShellExecuteA_t dwShellExecuteA;

	DWORD Kernel32Address;
	//ntdll.dll
	AddVectoredExceptionHandler_t dwRtlAddVectoredExceptionHandler;
	RtlRaiseException_t dwRtlRaiseException;
	NtContinue_t dwNtContinue;
	NtRaiseException_t dwNtRaiseException;
	NtReadVirtualMemory_t dwNtReadVirtualMemory;
	RtlImageNtHeader_t dwRtlImageNtHeader;
	//DWORD RtlUnwind;

	//d3dx9_43.dll
	D3DXCreateTextureFromFileInMemoryEx_t dwD3DXCreateTextureFromFileInMemoryEx;
	D3DXCreateTextureFromFileExW_t dwD3DXCreateTextureFromFileExW;

	//tier0
	CreateSimpleThread_t dwCreateSimpleThread;

	//Shlwapi
	PathIsRelativeA_t dwPathIsRelativeA;
	PathIsRelativeW_t dwPathIsRelativeW;
};

struct _driver_request
{
	unsigned long long target_process;
	unsigned long long real_address;
	unsigned long long fake_address;
	_driver_request(unsigned long long target, unsigned long long real, unsigned long long fake) :
		target_process(target),
		real_address(real),
		fake_address(fake)
	{
	}
};

HMODULE __stdcall spoofed_GetModuleHandleW(IN LPCWSTR lpModuleName);
void __stdcall spoofed_GetModuleInfo(IN fnv::hash lpModuleName, OUT MODULEINFO& moduleinfo);
//void __stdcall spoofed_GetModuleInfo(LPCWSTR lpModuleName, MODULEINFO& moduleinfo);
extern std::unique_ptr<manual_winapi> g_pWinApi;