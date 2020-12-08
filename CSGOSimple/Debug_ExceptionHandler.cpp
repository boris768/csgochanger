#include <Windows.h>
#include <fstream>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Hooks.hpp"
#include "Utils.hpp"

namespace Utils
{
	std::string GetTimestamp()
	{
		std::time_t t = std::time(nullptr);
		std::tm tm{};
		localtime_s(&tm, &t);
		const std::locale loc(std::cout.getloc());

		std::basic_stringstream<char> ss;
		ss.imbue(loc);
		ss << std::put_time(&tm, "[%A %b %e %H:%M:%S %Y]");

		return ss.str();
	}
	
	void Log(const char* fmt, ...)
	{
		if (!fmt) return;

		va_list va_alist;
		char logBuf[1024] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
		va_end(va_alist);

		std::ofstream file;

		file.open(std::string("csgo changer").append(".log"), std::ios::app);

		file << GetTimestamp() << " " << logBuf << std::endl;

		file.close();
	}
}


long __stdcall debug_ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	Utils::Log("-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -");
	Utils::Log("Gladiatorcheatz has crashed");
	Utils::Log("Base address: 0x%p", reinterpret_cast<ULONG>(&__ImageBase));
	Utils::Log("Exception at address: 0x%p", ExceptionInfo->ExceptionRecord->ExceptionAddress);

	const unsigned int m_ExceptionCode = static_cast<unsigned>(ExceptionInfo->ExceptionRecord->ExceptionCode);
	const int m_exceptionInfo_0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	const int m_exceptionInfo_1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	const int m_exceptionInfo_2 = ExceptionInfo->ExceptionRecord->ExceptionInformation[2];
	switch (m_ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		Utils::Log("Cause: EXCEPTION_ACCESS_VIOLATION");
		if (m_exceptionInfo_0 == 0)
		{
			// bad read
			Utils::Log("Attempted to read from: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 1)
		{
			// bad write
			Utils::Log("Attempted to write to: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 8)
		{
			// user-mode data execution prevention (DEP)
			Utils::Log("Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1);
		}
		else
		{
			// unknown, shouldn't happen
			Utils::Log("Unknown access violation at: 0x%08x", m_exceptionInfo_1);
		}
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		Utils::Log("Cause: EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		break;

	case EXCEPTION_BREAKPOINT:
		Utils::Log("Cause: EXCEPTION_BREAKPOINT");
		break;

	case EXCEPTION_DATATYPE_MISALIGNMENT:
		Utils::Log("Cause: EXCEPTION_DATATYPE_MISALIGNMENT");
		break;

	case EXCEPTION_FLT_DENORMAL_OPERAND:
		Utils::Log("Cause: EXCEPTION_FLT_DENORMAL_OPERAND");
		break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		Utils::Log("Cause: EXCEPTION_FLT_DIVIDE_BY_ZERO");
		break;

	case EXCEPTION_FLT_INEXACT_RESULT:
		Utils::Log("Cause: EXCEPTION_FLT_INEXACT_RESULT");
		break;

	case EXCEPTION_FLT_INVALID_OPERATION:
		Utils::Log("Cause: EXCEPTION_FLT_INVALID_OPERATION");
		break;

	case EXCEPTION_FLT_OVERFLOW:
		Utils::Log("Cause: EXCEPTION_FLT_OVERFLOW");
		break;

	case EXCEPTION_FLT_STACK_CHECK:
		Utils::Log("Cause: EXCEPTION_FLT_STACK_CHECK");
		break;

	case EXCEPTION_FLT_UNDERFLOW:
		Utils::Log("Cause: EXCEPTION_FLT_UNDERFLOW");
		break;

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		Utils::Log("Cause: EXCEPTION_ILLEGAL_INSTRUCTION");
		break;

	case EXCEPTION_IN_PAGE_ERROR:
		Utils::Log("Cause: EXCEPTION_IN_PAGE_ERROR");
		if (m_exceptionInfo_0 == 0)
		{
			// bad read
			Utils::Log("Attempted to read from: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 1)
		{
			// bad write
			Utils::Log("Attempted to write to: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 8)
		{
			// user-mode data execution prevention (DEP)
			Utils::Log("Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1);
		}
		else
		{
			// unknown, shouldn't happen
			Utils::Log("Unknown access violation at: 0x%08x", m_exceptionInfo_1);
		}

		Utils::Log("NTSTATUS: 0x%08x", m_exceptionInfo_2);
		break;

	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		Utils::Log("Cause: EXCEPTION_INT_DIVIDE_BY_ZERO");
		break;

	case EXCEPTION_INT_OVERFLOW:
		Utils::Log("Cause: EXCEPTION_INT_OVERFLOW");
		break;

	case EXCEPTION_INVALID_DISPOSITION:
		Utils::Log("Cause: EXCEPTION_INVALID_DISPOSITION");
		break;

	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		Utils::Log("Cause: EXCEPTION_NONCONTINUABLE_EXCEPTION");
		break;

	case EXCEPTION_PRIV_INSTRUCTION:
		Utils::Log("Cause: EXCEPTION_PRIV_INSTRUCTION");
		break;

	case EXCEPTION_SINGLE_STEP:
		Utils::Log("Cause: EXCEPTION_SINGLE_STEP");
		break;

	case EXCEPTION_STACK_OVERFLOW:
		Utils::Log("Cause: EXCEPTION_STACK_OVERFLOW");
		break;

	case DBG_CONTROL_C:
		Utils::Log("Cause: DBG_CONTROL_C");
		break;

	default:
		Utils::Log("Cause: %08x", m_ExceptionCode);
	}

	Utils::Log("-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -");

	Utils::Log("Dumping ASM registers:");
	Utils::Log("EAX: 0x%08x || ESI: 0x%08x", ExceptionInfo->ContextRecord->Eax, ExceptionInfo->ContextRecord->Esi);
	Utils::Log("EBX: 0x%08x || EDI: 0x%08x", ExceptionInfo->ContextRecord->Ebx, ExceptionInfo->ContextRecord->Edi);
	Utils::Log("ECX: 0x%08x || EBP: 0x%08x", ExceptionInfo->ContextRecord->Ecx, ExceptionInfo->ContextRecord->Ebp);
	Utils::Log("EDX: 0x%08x || ESP: 0x%08x", ExceptionInfo->ContextRecord->Edx, ExceptionInfo->ContextRecord->Esp);

	return EXCEPTION_CONTINUE_SEARCH;
}