// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
* Implementation of Structured Exceptions Handling inside Vectored Exception Handler
* For x86-platforms
* Usage: 
* 1. Include this file
* 2. Call `EnableSEHoverVEH()` (once)
* Teq (c) 01.2015
*/

#include <windows.h>
#include <WinNT.h>
#include <excpt.h>
#include "../CustomWinAPI.h"

// from \VC\crt\src\eh\i386\trnsctrl.cpp
//constexpr auto EXCEPTION_UNWINDING = 0x2;         // Unwind is in progress
//constexpr auto EXCEPTION_EXIT_UNWIND = 0x4;       // Exit unwind is in progress
//constexpr auto EXCEPTION_STACK_INVALID = 0x8;     // Stack out of limits or unaligned
//constexpr auto EXCEPTION_NESTED_CALL = 0x10;      // Nested exception handler call
//constexpr auto EXCEPTION_TARGET_UNWIND = 0x20;    // Target unwind in progress
//constexpr auto EXCEPTION_COLLIDED_UNWIND = 0x40;  // Collided exception handler call

typedef struct EXCEPTION_REGISTRATION
{
	EXCEPTION_REGISTRATION* nextframe;
	PEXCEPTION_ROUTINE handler;
}* PEXCEPTION_REGISTRATION;

EXCEPTION_DISPOSITION __cdecl ExecuteExceptionHandler(EXCEPTION_RECORD* ExceptionRecord, const LONG* const pEstablisherFrame,
                                                      CONTEXT* /*ContextRecord*/, PLONG pDispatcherContext)
{
	if (ExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND))
		return ExceptionContinueSearch;
	*pDispatcherContext = *(pEstablisherFrame + 2); // + sizeof(long) x2
	return ExceptionNestedException;
}

EXCEPTION_DISPOSITION __cdecl SafeExecuteHandler(EXCEPTION_RECORD* ExceptionRecord, PVOID EstablisherFrame,
                                                 CONTEXT* ContextRecord, PVOID DispatcherContext,
                                                 PEXCEPTION_ROUTINE pHandler)
{
	const EXCEPTION_DISPOSITION Disposition =
		pHandler(ExceptionRecord, EstablisherFrame, ContextRecord, DispatcherContext);
	return Disposition;
}

// VectoredDispatcherOfStructuredExceptions
_declspec(noreturn) void __stdcall VectoredSEH(_EXCEPTION_POINTERS* ExceptionInfo)
{
	CONTEXT* const ctx = ExceptionInfo->ContextRecord;
	PEXCEPTION_RECORD ex = ExceptionInfo->ExceptionRecord;
	PEXCEPTION_REGISTRATION Registration = nullptr; //GetRegistrationHead();
	PEXCEPTION_REGISTRATION DispatcherContext = nullptr;
	PEXCEPTION_REGISTRATION NestedRegistration = nullptr;

	while (Registration && reinterpret_cast<LONG>(Registration) != -1) // -1 means end of chain
	{
		const EXCEPTION_DISPOSITION Disposition = SafeExecuteHandler(ex, Registration, ctx, &DispatcherContext,
		                                                       Registration->handler);

		if (NestedRegistration == Registration)
		{
			ex->ExceptionFlags &= (~EXCEPTION_NESTED_CALL);
			NestedRegistration = nullptr;
		}

		switch (Disposition)
		{
			EXCEPTION_RECORD nextEx;

		case ExceptionContinueExecution:
			if (!(ex->ExceptionFlags & EXCEPTION_NONCONTINUABLE))
				g_pWinApi->dwNtContinue(ctx, 0);
			else
			{
				nextEx.ExceptionCode = EXCEPTION_NONCONTINUABLE_EXCEPTION;
				nextEx.ExceptionFlags = 1;
				nextEx.ExceptionRecord = ex;
				nextEx.ExceptionAddress = nullptr;
				nextEx.NumberParameters = 0;
				g_pWinApi->dwRtlRaiseException(&nextEx);
			}
			break;

		case ExceptionContinueSearch:
			if (ex->ExceptionFlags & EXCEPTION_STACK_INVALID)
				g_pWinApi->dwNtRaiseException(ex, ctx, false);
			break;

		case ExceptionNestedException:
			ex->ExceptionFlags |= EXCEPTION_NESTED_CALL;
			// renew context
			if (DispatcherContext > NestedRegistration)
				NestedRegistration = DispatcherContext;
			break;

		default:
			nextEx.ExceptionRecord = ex;
			nextEx.ExceptionCode = STATUS_INVALID_DISPOSITION;
			nextEx.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
			nextEx.NumberParameters = 0;
			g_pWinApi->dwRtlRaiseException(&nextEx);
			break;
		}
		Registration = Registration->nextframe;
	}

	/* 
		dispatcher hasn't found appropriate hander for exception in SEH chain, 
		if this handler was first in the VEH-chain - there are could be other vectored handlers, 
		those handlers needs to take a chance to handle this exception (return EXCEPTION_CONTINUE_SEARCH;).
		if this handler is first - just call: NtRaiseException(ex, ctx, false);
	*/
	g_pWinApi->dwNtRaiseException(ex, ctx, false);
}

void __cdecl EnableSEHoverVEH()
{
	g_pWinApi->dwRtlAddVectoredExceptionHandler(0, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(&VectoredSEH));
}
