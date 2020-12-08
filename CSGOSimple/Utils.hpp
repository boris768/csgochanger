#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <string>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace Utils
{
	ptrdiff_t FindSignature(const char* szSignature, ptrdiff_t startAddress, ptrdiff_t endAddress);
	ptrdiff_t FindSignature(const MODULEINFO& in_module, const char* szSignature);
	ptrdiff_t dwGetCallAddress(ptrdiff_t dwAddress);
	ULONG get_image_size(PVOID ImageBase) noexcept;
	void PrecacheSounds();

	int CombinedItemAndPaintRarity(int item, int paint);

	int RandomInt(int iMin, int iMax);
	int RandomInt(unsigned iMin, unsigned iMax);
	int FalchionLookat();

	bool IsCodePtr(void* ptr);
	bool FileExists(const char* path);
	std::string format_to_string(const char* fmt, ...);
};