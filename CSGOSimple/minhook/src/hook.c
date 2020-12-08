﻿/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <windows.h>
#include <tlhelp32.h>
#include <climits>

#include "../include/MinHook.h"
#include "buffer.h"
#include "trampoline.h"

#ifndef ARRAYSIZE
    #define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif

// Initial capacity of the HOOK_ENTRY buffer.
constexpr auto INITIAL_HOOK_CAPACITY = 32;

// Initial capacity of the thread IDs buffer.
constexpr auto  INITIAL_THREAD_CAPACITY = 128;

// Special hook position values.
constexpr auto  INVALID_HOOK_POS = UINT_MAX;
constexpr auto  ALL_HOOKS_POS = UINT_MAX;

// Freeze() action argument defines.
constexpr auto  ACTION_DISABLE = 0;
constexpr auto  ACTION_ENABLE = 1;
constexpr auto  ACTION_APPLY_QUEUED = 2;

// Thread access rights for suspending/resuming threads.
constexpr auto  THREAD_ACCESS = (THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SET_CONTEXT);

// Hook information.
typedef struct _HOOK_ENTRY
{
    LPVOID pTarget;             // Address of the target function.
    LPVOID pDetour;             // Address of the detour or relay function.
    LPVOID pTrampoline;         // Address of the trampoline function.
    UINT8  backup[8];           // Original prologue of the target function.

    UINT8  patchAbove  : 1;     // Uses the hot patch area.
    UINT8  isEnabled   : 1;     // Enabled.
    UINT8  queueEnable : 1;     // Queued for enabling/disabling when != isEnabled.

    UINT   nIP : 4;             // Count of the instruction boundaries.
    UINT8  oldIPs[8];           // Instruction boundaries of the target function.
    UINT8  newIPs[8];           // Instruction boundaries of the trampoline function.
} HOOK_ENTRY, *PHOOK_ENTRY;

// Suspended threads for Freeze()/Unfreeze().
typedef struct _FROZEN_THREADS
{
    LPDWORD pItems;         // Data heap
    UINT    capacity;       // Size of allocated data heap, items
    UINT    size;           // Actual number of data items
} FROZEN_THREADS, *PFROZEN_THREADS;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------

// Spin lock flag for EnterSpinLock()/LeaveSpinLock().
volatile LONG g_isLocked = FALSE;

// Private heap handle. If not NULL, this library is initialized.
HANDLE g_hHeap = nullptr;

// Hook entries.
struct
{
    PHOOK_ENTRY pItems;     // Data heap
    UINT        capacity;   // Size of allocated data heap, items
    UINT        size;       // Actual number of data items
} g_hooks;

//-------------------------------------------------------------------------
// Returns INVALID_HOOK_POS if not found.
static UINT FindHookEntry(LPVOID pTarget)
{
	for (UINT i = 0; i < g_hooks.size; ++i)
    {
        if (reinterpret_cast<ULONG_PTR>(pTarget) == reinterpret_cast<ULONG_PTR>(g_hooks.pItems[i].pTarget))
            return i;
    }

    return INVALID_HOOK_POS;
}

//-------------------------------------------------------------------------
static PHOOK_ENTRY AddHookEntry()
{
    if (g_hooks.pItems == nullptr)
    {
        g_hooks.capacity = INITIAL_HOOK_CAPACITY;
        g_hooks.pItems = static_cast<PHOOK_ENTRY>(HeapAlloc(
	        g_hHeap, 0, g_hooks.capacity * sizeof(HOOK_ENTRY)));
        if (g_hooks.pItems == nullptr)
            return nullptr;
    }
    else if (g_hooks.size >= g_hooks.capacity)
    {
        auto* p = static_cast<PHOOK_ENTRY>(HeapReAlloc(
	        g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity * 2) * sizeof(HOOK_ENTRY)));
        if (p == nullptr)
            return nullptr;

        g_hooks.capacity *= 2;
        g_hooks.pItems = p;
    }

    return &g_hooks.pItems[g_hooks.size++];
}

//-------------------------------------------------------------------------
static void DeleteHookEntry(UINT pos)
{
    if (pos < g_hooks.size - 1)
        g_hooks.pItems[pos] = g_hooks.pItems[g_hooks.size - 1];

    g_hooks.size--;

    if (g_hooks.capacity / 2 >= INITIAL_HOOK_CAPACITY && g_hooks.capacity / 2 >= g_hooks.size)
    {
	    const auto p = static_cast<PHOOK_ENTRY>(HeapReAlloc(
		    g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity / 2) * sizeof(HOOK_ENTRY)));
        if (p == nullptr)
            return;

        g_hooks.capacity /= 2;
        g_hooks.pItems = p;
    }
}

//-------------------------------------------------------------------------
static DWORD_PTR FindOldIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
	if (pHook->patchAbove && ip == (reinterpret_cast<DWORD_PTR>(pHook->pTarget) - sizeof(JMP_REL)))
        return reinterpret_cast<DWORD_PTR>(pHook->pTarget);

    for (UINT i = 0; i < pHook->nIP; ++i)
    {
        if (ip == (reinterpret_cast<DWORD_PTR>(pHook->pTrampoline) + pHook->newIPs[i]))
            return reinterpret_cast<DWORD_PTR>(pHook->pTarget) + pHook->oldIPs[i];
    }

#if defined(_M_X64) || defined(__x86_64__)
    // Check relay function.
    if (ip == (DWORD_PTR)pHook->pDetour)
        return (DWORD_PTR)pHook->pTarget;
#endif

    return 0;
}

//-------------------------------------------------------------------------
static DWORD_PTR FindNewIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
	for (UINT i = 0; i < pHook->nIP; ++i)
    {
        if (ip == (reinterpret_cast<DWORD_PTR>(pHook->pTarget) + pHook->oldIPs[i]))
            return reinterpret_cast<DWORD_PTR>(pHook->pTrampoline) + pHook->newIPs[i];
    }

    return 0;
}

//-------------------------------------------------------------------------
static void ProcessThreadIPs(HANDLE hThread, UINT pos, UINT action)
{
    // If the thread suspended in the overwritten area,
    // move IP to the proper address.

    CONTEXT c;
#if defined(_M_X64) || defined(__x86_64__)
    DWORD64 *pIP = &c.Rip;
#else
    DWORD   *pIP = &c.Eip;
#endif
    UINT count;

    c.ContextFlags = CONTEXT_CONTROL;
    if (!GetThreadContext(hThread, &c))
        return;

    if (pos == ALL_HOOKS_POS)
    {
        pos = 0;
        count = g_hooks.size;
    }
    else
    {
        count = pos + 1;
    }

    for (; pos < count; ++pos)
    {
	    const PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
        BOOL        enable;
        DWORD_PTR   ip;

        switch (action)
        {
        case ACTION_DISABLE:
            enable = FALSE;
            break;

        case ACTION_ENABLE:
            enable = TRUE;
            break;

        default: // ACTION_APPLY_QUEUED
            enable = pHook->queueEnable;
            break;
        }
        if (pHook->isEnabled == enable)
            continue;

        if (enable)
            ip = FindNewIP(pHook, *pIP);
        else
            ip = FindOldIP(pHook, *pIP);

        if (ip != 0)
        {
            *pIP = ip;
            SetThreadContext(hThread, &c);
        }
    }
}

//-------------------------------------------------------------------------
static VOID EnumerateThreads(PFROZEN_THREADS pThreads)
{
	const HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hSnapshot, &te))
        {
            do
            {
                if (te.dwSize >= (FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(DWORD))
                    && te.th32OwnerProcessID == GetCurrentProcessId()
                    && te.th32ThreadID != GetCurrentThreadId())
                {
                    if (pThreads->pItems == nullptr)
                    {
                        pThreads->capacity = INITIAL_THREAD_CAPACITY;
                        pThreads->pItems
                            = static_cast<LPDWORD>(HeapAlloc(g_hHeap, 0, pThreads->capacity * sizeof(DWORD)));
                        if (pThreads->pItems == nullptr)
                            break;
                    }
                    else if (pThreads->size >= pThreads->capacity)
                    {
	                    const auto p = static_cast<LPDWORD>(HeapReAlloc(
		                    g_hHeap, 0, pThreads->pItems, (pThreads->capacity * 2) * sizeof(DWORD)));
                        if (p == nullptr)
                            break;

                        pThreads->capacity *= 2;
                        pThreads->pItems = p;
                    }
                    pThreads->pItems[pThreads->size++] = te.th32ThreadID;
                }

                te.dwSize = sizeof(THREADENTRY32);
            } while (Thread32Next(hSnapshot, &te));
        }
        CloseHandle(hSnapshot);
    }
}

//-------------------------------------------------------------------------
static VOID Freeze(PFROZEN_THREADS pThreads, UINT pos, UINT action)
{
    pThreads->pItems   = nullptr;
    pThreads->capacity = 0;
    pThreads->size     = 0;
    EnumerateThreads(pThreads);

    if (pThreads->pItems != nullptr)
    {
	    for (UINT i = 0; i < pThreads->size; ++i)
        {
	        const HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, pThreads->pItems[i]);
            if (hThread != nullptr)
            {
                SuspendThread(hThread);
                ProcessThreadIPs(hThread, pos, action);
                CloseHandle(hThread);
            }
        }
    }
}

//-------------------------------------------------------------------------
static VOID Unfreeze(PFROZEN_THREADS pThreads)
{
    if (pThreads->pItems != nullptr)
    {
	    for (UINT i = 0; i < pThreads->size; ++i)
        {
	        const HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, pThreads->pItems[i]);
            if (hThread != nullptr)
            {
                ResumeThread(hThread);
                CloseHandle(hThread);
            }
        }

        HeapFree(g_hHeap, 0, pThreads->pItems);
    }
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHookLL(UINT pos, BOOL enable)
{
    PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
    DWORD  oldProtect;
    SIZE_T patchSize    = sizeof(JMP_REL);
    auto pPatchTarget = static_cast<LPBYTE>(pHook->pTarget);

    if (pHook->patchAbove)
    {
        pPatchTarget -= sizeof(JMP_REL);
        patchSize    += sizeof(JMP_REL_SHORT);
    }

    if (!VirtualProtect(pPatchTarget, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect))
        return MH_ERROR_MEMORY_PROTECT;

    if (enable)
    {
	    const auto pJmp = reinterpret_cast<PJMP_REL>(pPatchTarget);
        pJmp->opcode = 0xE9;
        pJmp->operand = static_cast<UINT32>(static_cast<LPBYTE>(pHook->pDetour) - (pPatchTarget + sizeof(JMP_REL)));

        if (pHook->patchAbove)
        {
	        const auto pShortJmp = static_cast<PJMP_REL_SHORT>(pHook->pTarget);
            pShortJmp->opcode = 0xEB;
            pShortJmp->operand = static_cast<UINT8>(0 - (sizeof(JMP_REL_SHORT) + sizeof(JMP_REL)));
        }
    }
    else
    {
        if (pHook->patchAbove)
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        else
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL));
    }

    VirtualProtect(pPatchTarget, patchSize, oldProtect, &oldProtect);

    // Just-in-case measure.
    FlushInstructionCache(GetCurrentProcess(), pPatchTarget, patchSize);

    pHook->isEnabled   = enable;
    pHook->queueEnable = enable;

    return MH_OK;
}

//-------------------------------------------------------------------------
static MH_STATUS EnableAllHooksLL(BOOL enable)
{
    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;

    for (i = 0; i < g_hooks.size; ++i)
    {
        if (g_hooks.pItems[i].isEnabled != enable)
        {
            first = i;
            break;
        }
    }

    if (first != INVALID_HOOK_POS)
    {
        FROZEN_THREADS threads;
        Freeze(&threads, ALL_HOOKS_POS, enable ? ACTION_ENABLE : ACTION_DISABLE);

        for (i = first; i < g_hooks.size; ++i)
        {
            if (g_hooks.pItems[i].isEnabled != enable)
            {
                status = EnableHookLL(i, enable);
                if (status != MH_OK)
                    break;
            }
        }

        Unfreeze(&threads);
    }

    return status;
}

//-------------------------------------------------------------------------
static VOID EnterSpinLock()
{
    SIZE_T spinCount = 0;

    // Wait until the flag is FALSE.
    while (InterlockedCompareExchange(&g_isLocked, TRUE, FALSE) != FALSE)
    {
        // No need to generate a memory barrier here, since InterlockedCompareExchange()
        // generates a full memory barrier itself.

        // Prevent the loop from being too busy.
        if (spinCount < 32)
            Sleep(0);
        else
            Sleep(1);

        spinCount++;
    }
}

//-------------------------------------------------------------------------
static VOID LeaveSpinLock()
{
    // No need to generate a memory barrier here, since InterlockedExchange()
    // generates a full memory barrier itself.

    InterlockedExchange(&g_isLocked, FALSE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Initialize()
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap == nullptr)
    {
        g_hHeap = HeapCreate(0, 0, 0);
        if (g_hHeap != nullptr)
        {
            // Initialize the internal function buffer.
            InitializeBuffer();
        }
        else
        {
            status = MH_ERROR_MEMORY_ALLOC;
        }
    }
    else
    {
        status = MH_ERROR_ALREADY_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Uninitialize()
{
    MH_STATUS status;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
        status = EnableAllHooksLL(FALSE);
        if (status == MH_OK)
        {
            // Free the internal function buffer.

            // HeapFree is actually not required, but some tools detect a false
            // memory leak without HeapFree.

            UninitializeBuffer();

            HeapFree(g_hHeap, 0, g_hooks.pItems);
            HeapDestroy(g_hHeap);

            g_hHeap = nullptr;

            g_hooks.pItems   = nullptr;
            g_hooks.capacity = 0;
            g_hooks.size     = 0;
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal)
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
        if (IsExecutableAddress(pTarget) && IsExecutableAddress(pDetour))
        {
	        const UINT pos = FindHookEntry(pTarget);
            if (pos == INVALID_HOOK_POS)
            {
	            const LPVOID pBuffer = AllocateBuffer(pTarget);
                if (pBuffer != nullptr)
                {
                    TRAMPOLINE ct;

                    ct.pTarget     = pTarget;
                    ct.pDetour     = pDetour;
                    ct.pTrampoline = pBuffer;
                    if (CreateTrampolineFunction(&ct))
                    {
                        PHOOK_ENTRY pHook = AddHookEntry();
                        if (pHook != nullptr)
                        {
                            pHook->pTarget     = ct.pTarget;
#if defined(_M_X64) || defined(__x86_64__)
                            pHook->pDetour     = ct.pRelay;
#else
                            pHook->pDetour     = ct.pDetour;
#endif
                            pHook->pTrampoline = ct.pTrampoline;
                            pHook->patchAbove  = ct.patchAbove;
                            pHook->isEnabled   = FALSE;
                            pHook->queueEnable = FALSE;
                            pHook->nIP         = ct.nIP;
                            memcpy(pHook->oldIPs, ct.oldIPs, ARRAYSIZE(ct.oldIPs));
                            memcpy(pHook->newIPs, ct.newIPs, ARRAYSIZE(ct.newIPs));

                            // Back up the target function.

                            if (ct.patchAbove)
                            {
                                memcpy(
                                    pHook->backup,
                                    static_cast<LPBYTE>(pTarget) - sizeof(JMP_REL),
                                    sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
                            }
                            else
                            {
                                memcpy(pHook->backup, pTarget, sizeof(JMP_REL));
                            }

                            if (ppOriginal != nullptr)
                                *ppOriginal = pHook->pTrampoline;
                        }
                        else
                        {
                            status = MH_ERROR_MEMORY_ALLOC;
                        }
                    }
                    else
                    {
                        status = MH_ERROR_UNSUPPORTED_FUNCTION;
                    }

                    if (status != MH_OK)
                    {
                        FreeBuffer(pBuffer);
                    }
                }
                else
                {
                    status = MH_ERROR_MEMORY_ALLOC;
                }
            }
            else
            {
                status = MH_ERROR_ALREADY_CREATED;
            }
        }
        else
        {
            status = MH_ERROR_NOT_EXECUTABLE;
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget)
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
	    const UINT pos = FindHookEntry(pTarget);
        if (pos != INVALID_HOOK_POS)
        {
            if (g_hooks.pItems[pos].isEnabled)
            {
                FROZEN_THREADS threads;
                Freeze(&threads, pos, ACTION_DISABLE);

                status = EnableHookLL(pos, FALSE);

                Unfreeze(&threads);
            }

            if (status == MH_OK)
            {
                FreeBuffer(g_hooks.pItems[pos].pTrampoline);
                DeleteHookEntry(pos);
            }
        }
        else
        {
            status = MH_ERROR_NOT_CREATED;
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHook(LPVOID pTarget, BOOL enable)
{
    MH_STATUS status;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
        if (pTarget == nullptr)
        {
            status = EnableAllHooksLL(enable);
        }
        else
        {
            FROZEN_THREADS threads;
            const UINT pos = FindHookEntry(pTarget);
            if (pos != INVALID_HOOK_POS)
            {
                if (g_hooks.pItems[pos].isEnabled != enable)
                {
                    Freeze(&threads, pos, ACTION_ENABLE);

                    status = EnableHookLL(pos, enable);

                    Unfreeze(&threads);
                }
                else
                {
                    status = enable ? MH_ERROR_ENABLED : MH_ERROR_DISABLED;
                }
            }
            else
            {
                status = MH_ERROR_NOT_CREATED;
            }
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget)
{
    return EnableHook(pTarget, TRUE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget)
{
    return EnableHook(pTarget, FALSE);
}

//-------------------------------------------------------------------------
static MH_STATUS QueueHook(LPVOID pTarget, BOOL queueEnable)
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
        if (pTarget == nullptr)
        {
	        for (UINT i = 0; i < g_hooks.size; ++i)
                g_hooks.pItems[i].queueEnable = queueEnable;
        }
        else
        {
	        const UINT pos = FindHookEntry(pTarget);
            if (pos != INVALID_HOOK_POS)
            {
                g_hooks.pItems[pos].queueEnable = queueEnable;
            }
            else
            {
                status = MH_ERROR_NOT_CREATED;
            }
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueEnableHook(LPVOID pTarget)
{
    return QueueHook(pTarget, TRUE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueDisableHook(LPVOID pTarget)
{
    return QueueHook(pTarget, FALSE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_ApplyQueued()
{
    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;

    EnterSpinLock();

    if (g_hHeap != nullptr)
    {
        for (i = 0; i < g_hooks.size; ++i)
        {
            if (g_hooks.pItems[i].isEnabled != g_hooks.pItems[i].queueEnable)
            {
                first = i;
                break;
            }
        }

        if (first != INVALID_HOOK_POS)
        {
            FROZEN_THREADS threads;
            Freeze(&threads, ALL_HOOKS_POS, ACTION_APPLY_QUEUED);

            for (i = first; i < g_hooks.size; ++i)
            {
	            const PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                if (pHook->isEnabled != pHook->queueEnable)
                {
                    status = EnableHookLL(i, pHook->queueEnable);
                    if (status != MH_OK)
                        break;
                }
            }

            Unfreeze(&threads);
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHookApiEx(
    LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour,
    LPVOID *ppOriginal, LPVOID *ppTarget)
{
	const HMODULE hModule = GetModuleHandleW(pszModule);
    if (hModule == nullptr)
        return MH_ERROR_MODULE_NOT_FOUND;

	const auto pTarget = static_cast<LPVOID>(GetProcAddress(hModule, pszProcName));
    if (pTarget == nullptr)
        return MH_ERROR_FUNCTION_NOT_FOUND;

    if(ppTarget != nullptr)
        *ppTarget = pTarget;

    return MH_CreateHook(pTarget, pDetour, ppOriginal);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHookApi(
    LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID *ppOriginal)
{
   return MH_CreateHookApiEx(pszModule, pszProcName, pDetour, ppOriginal, nullptr);
}

//-------------------------------------------------------------------------
const char * WINAPI MH_StatusToString(MH_STATUS status)
{

    switch (status) {
        case MH_UNKNOWN: return "MH_UNKNOWN";
        case MH_OK: return "MH_OK";
        case MH_ERROR_ALREADY_INITIALIZED: return "MH_ERROR_ALREADY_INITIALIZED";
        case MH_ERROR_NOT_INITIALIZED: return "MH_ERROR_NOT_INITIALIZED";
        case MH_ERROR_ALREADY_CREATED: return "MH_ERROR_ALREADY_CREATED";
        case MH_ERROR_NOT_CREATED: return "MH_ERROR_NOT_CREATED";
        case MH_ERROR_ENABLED: return "MH_ERROR_ENABLED";
        case MH_ERROR_DISABLED: return "MH_ERROR_DISABLED";
        case MH_ERROR_NOT_EXECUTABLE: return "MH_ERROR_NOT_EXECUTABLE";
        case MH_ERROR_UNSUPPORTED_FUNCTION: return "MH_ERROR_UNSUPPORTED_FUNCTION";
        case MH_ERROR_MEMORY_ALLOC: return "MH_ERROR_MEMORY_ALLOC";
        case MH_ERROR_MEMORY_PROTECT: return "MH_ERROR_MEMORY_PROTECT";
        case MH_ERROR_MODULE_NOT_FOUND: return "MH_ERROR_MODULE_NOT_FOUND";
        case MH_ERROR_FUNCTION_NOT_FOUND: return "MH_ERROR_FUNCTION_NOT_FOUND";
		default:
			return "(unknown)";
    }
}
