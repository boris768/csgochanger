// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IBaseWorldModel.h"
#include "Options.hpp"

void SourceEngine::IBaseWorldModel::SetModelIndex(const int index)
{
	using oSetModelIndex = void(__thiscall*)(PVOID, int);
	CallVFunction<oSetModelIndex>(this, netvars_c_base_combat_weapon->dwSetModelIndex)(this, index); // Reverse SetWeaponModel.
	//std::cout << "set model index : " << netvars_c_base_combat_weapon->dwSetModelIndex << std::endl;
}
