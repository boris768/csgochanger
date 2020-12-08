#include "Utils.hpp"
#include <random>
#include <fstream>
#include "XorStr.hpp"
#include "CustomWinAPI.h"
#include "Hooks.hpp"
#include <ctime>

#define INRANGE(x,a,b)  ((x) >= (a) && (x) <= ((b)))
#define getBits( x )    (INRANGE(((x)&(~0x20)),'A','F') ? (((x)&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? (x) - '0' : 0))
#define getByte( x )    (getBits((x)[0]) << 4 | getBits((x)[1]))

namespace Utils
{
	ptrdiff_t FindSignature(const char* szSignature, const ptrdiff_t startAddress, const ptrdiff_t endAddress)
	{
		const char* pat = szSignature;
		ptrdiff_t firstMatch = 0;
		for (ptrdiff_t pCur = startAddress; pCur < endAddress; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if ((*(PBYTE)pat == '\?' && *((PBYTE)pat + 1) == '\?') || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2; //one ?
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return 0;
	}

	ptrdiff_t FindSignature(const MODULEINFO& in_module, const char* szSignature)
	{
		const auto start_address = reinterpret_cast<ptrdiff_t>(in_module.lpBaseOfDll);
		const auto end_address = static_cast<ptrdiff_t>(start_address + in_module.SizeOfImage);
		const char* pat = szSignature;
		ptrdiff_t firstMatch = 0;
		for (ptrdiff_t pCur = start_address; pCur < end_address; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if ((*(PBYTE)pat == '\?' && *((PBYTE)pat + 1) == '\?') || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2; //one ?
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return 0;
	}


	ULONG get_image_size(PVOID ImageBase) noexcept
	{
		if (const PIMAGE_NT_HEADERS pinth = g_pWinApi->dwRtlImageNtHeader(ImageBase))
		{
			return pinth->OptionalHeader.SizeOfImage;
		}

		return 0;
	}

	ptrdiff_t dwGetCallAddress(const ptrdiff_t dwAddress)
	{
		return *reinterpret_cast<ptrdiff_t*>(dwAddress + 1) + dwAddress + 5;
	}

	//TODO: add FindNextFileA to customwinapi
	void PrecacheSounds()
	{
		auto filesystem = SourceEngine::interfaces.FileSystem();
		WIN32_FIND_DATAA FindFileData;
		const std::string sound_path = XorStr("csgo\\sound\\changer\\old_weapons\\");
		void* const hf = FindFirstFileA((sound_path + XorStr("*")).c_str(), &FindFileData);
		if (hf != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // if is directory
				{
					WIN32_FIND_DATAA FindSubFileData;
					void* const sub_hf = FindFirstFileA((sound_path + FindFileData.cFileName + XorStr("\\*")).c_str(),
					                                    &FindSubFileData);
					if (sub_hf != INVALID_HANDLE_VALUE)
					{
						do
						{
							filesystem->Precache(
								(sound_path + FindFileData.cFileName + XorStr("\\") + FindSubFileData.cFileName).
								c_str(),
								(sound_path + FindFileData.cFileName + XorStr("\\") + FindSubFileData.cFileName).
								c_str());
							//std::string out = sound_path + FindFileData.cFileName + XorStr("\\") + FindSubFileData.cFileName + XorStr("\n");
							if (!Options::sounds_precached)
								Options::sounds_precached = true;
						}
						while (FindNextFileA(sub_hf, &FindSubFileData) != 0);
						FindClose(sub_hf);
					}
				}
				else
				{
					filesystem->Precache((sound_path + FindFileData.cFileName).c_str(),
					                     (sound_path + FindFileData.cFileName).c_str());
					//std::string out = sound_path + FindFileData.cFileName + XorStr("\n");
					if (!Options::sounds_precached)
						Options::sounds_precached = true;
				}
			}
			while (FindNextFileA(hf, &FindFileData) != 0);
			FindClose(hf);
		}
	}

	int CombinedItemAndPaintRarity(int item, int paint)
	{
		int result = 0;
		if (paint + item > 0)
		{
			const int comb_rarity = paint + item - 1;
			result = std::clamp(comb_rarity, 0, 7);
		}
		return result;
	}

	int RandomInt(const int iMin, const int iMax)
	{
		std::random_device rd;
		std::mt19937 mt;
		std::uniform_int_distribution<int> dist;

		mt.seed(rd());
		return dist(mt, decltype(dist)::param_type(iMin, iMax));
	}

	int RandomInt(unsigned iMin, unsigned iMax)
	{
		std::random_device rd;
		std::mt19937 mt;
		std::uniform_int_distribution<int> dist;

		mt.seed(rd());
		return dist(mt, decltype(dist)::param_type(iMin, iMax));
	}

	int FalchionLookat()
	{
		std::random_device rd;
		std::mt19937 mt;
		const std::discrete_distribution<int> dist2{14, 1};
		mt.seed(rd());
		return SourceEngine::falchion_lookat01 + dist2(mt);
	}


	bool IsCodePtr(void* ptr)
	{
		constexpr DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
			PAGE_EXECUTE_WRITECOPY;

		MEMORY_BASIC_INFORMATION out;
		g_pWinApi->dwVirtualQuery(ptr, &out, sizeof(out));

		return out.Type
			&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			&& out.Protect & protect_flags;
	}

	bool FileExists(const char* path)
	{
		std::ifstream file;
		file.open(path);

		const bool is_open = file.is_open();

		if (is_open)
			file.close();

		return is_open;
	}

	std::string format_to_string(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::vector<char> v(1024);
		while (true)
		{
			va_list args2;
			va_copy(args2, args);
			const int res = vsnprintf(v.data(), v.size(), fmt, args2);
			if ((res >= 0) && (res < static_cast<int>(v.size())))
			{
				va_end(args);
				va_end(args2);
				return std::string(v.data());
			}
			size_t size;
			if (res < 0)
				size = v.size() * 2;
			else
				size = static_cast<size_t>(res) + 1;
			v.clear();
			v.resize(size);
			va_end(args2);
		}
	}
}

namespace SourceEngine
{
	factory::CFactory::CFactory(const HMODULE& module) : s_pInterfaceRegs(nullptr)
	{
		Init(reinterpret_cast<ptrdiff_t>(g_pWinApi->GetProcAddress(module, XorStr("CreateInterface"))));
	}

	factory::CFactory::CFactory(const LPVOID& module) : s_pInterfaceRegs(nullptr)
	{
		Init(reinterpret_cast<ptrdiff_t>(g_pWinApi->GetProcAddress(reinterpret_cast<DWORD>(module),
		                                                           XorStr("CreateInterface"))));
	}

	void factory::CFactory::Init(const ptrdiff_t& dwCreateInterface)
	{
		DWORD s_p_interface_regs = *reinterpret_cast<PDWORD>(dwCreateInterface + 5);
		if (s_p_interface_regs & (1 << 0x1F))
			s_p_interface_regs = 0xFFFFFFF1 - s_p_interface_regs;
		else
			s_p_interface_regs = 0; //-0x6;

		if (s_p_interface_regs != 0xfffffffA && s_p_interface_regs > 0x2000)
			return;

		this->s_pInterfaceRegs = reinterpret_cast<InterfaceReg*>(**reinterpret_cast<PDWORD*>(dwCreateInterface -
			s_p_interface_regs));
	}

	std::vector<factory::interface_info> factory::CFactory::dump() const
	{
		std::vector<interface_info> ret;
		for (InterfaceReg* pCur = s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
		{
			ret.emplace_back(pCur->interface_info_);
		}
		return ret;
	}

	DWORD factory::CFactory::dwGetInterface(const char* pszName, const int iVersion) const
	{
		const size_t iNameLen = strlen(pszName);

		for (InterfaceReg* pCur = s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
			if (!strncmp(pCur->interface_info_.m_pName, pszName, iNameLen))
				if (atoi(pCur->interface_info_.m_pName + iNameLen) > iVersion)
					return pCur->interface_info_.m_CreateFn();

		return 0;
	}
}
