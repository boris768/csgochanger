// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include "XorStr.hpp"

namespace Hooks
{
	FileNameHandle_t ReplaceSound(const char* pSoundFileName, IFileSystem* self, int edx)
	{
		char patched_name[MAX_PATH] = "changer/old_weapons/";
		strcpy(patched_name + 20, pSoundFileName + 9);
		return g_fnOriginalFindOrAddFilename(self, edx, patched_name);
	}

	FileNameHandle_t __fastcall Hooked_FindOrAddFileName(IFileSystem* self, int edx, char const* pFileName)
	{
		// Exceptions
		if (strstr(pFileName, XorStr("fx")) || strstr(pFileName, XorStr("smoke")) || strstr(pFileName, XorStr("knife")) ||
			strstr(pFileName, XorStr("lowa" /*lowammo*/)))
			return g_fnOriginalFindOrAddFilename(self, edx, pFileName);

		const auto start = strstr(pFileName, XorStr("weapons/"));
		if (start && strstr(pFileName, XorStr(".wav")))
		{
			// don't replace .mdls
			for (auto& config_old_sound : Options::config_old_sounds)
			{
				if (strstr(pFileName, C_BaseCombatWeapon::GetWeaponName(config_old_sound.first).c_str()) &&
					config_old_sound.second)
					return ReplaceSound(pFileName, self, edx);
			}
		}

		return g_fnOriginalFindOrAddFilename(self, edx, pFileName);
	}
	
}
