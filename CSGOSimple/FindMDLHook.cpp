// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"

SourceEngine::MDLHandle_t __fastcall Hooks::Hooked_FindMDL(void* ecx, void*, const char* path)
{
	if (strstr(path, "v_knife_default_ct.mdl"))
		return g_fnOriginalFindMDL(ecx, "models/isony/hotstar/knifes/is_dildo/flex/is_dildo_v.mdl");
	return g_fnOriginalFindMDL(ecx, path);
}
