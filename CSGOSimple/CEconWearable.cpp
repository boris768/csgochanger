// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CEconWearable.h"

void C_CEconWearable::SetModelIndex(int index)
{
	using oSetModelIndex = void(__thiscall*)(void*, int);
	SourceEngine::CallVFunction<oSetModelIndex>(this, netvars_c_base_combat_weapon->dwSetModelIndex)(this, index); // Reverse SetWeaponModel.
}

SourceEngine::C_EconItemView* C_CEconWearable::GetEconItemView()
{
	return GetFieldPointer<SourceEngine::C_EconItemView>(netvars_c_base_combat_weapon->dwGetEconItemViewSomething);
	//return reinterpret_cast<SourceEngine::C_EconItemView*>(reinterpret_cast<DWORD>(this) + netvars_c_base_combat_weapon->dwGetEconItemViewSomething);
}

int C_CEconWearable::Equip(IClientEntity * player)
{
	return netvars_c_base_combat_weapon->WearableEquip(this, player);
}

int C_CEconWearable::UnEquip(IClientEntity* player)
{
	return netvars_c_base_combat_weapon->WearableUnEquip(this, player);
}

int C_CEconWearable::InitializeAttributes()
{
	return netvars_c_base_combat_weapon->InitializeAttributes(this);
}


bool* C_CEconWearable::GetInitialized()
{
	return GetFieldPointer<bool>(netvars_c_base_combat_weapon->m_bInitialized);
}

int* C_CEconWearable::GetFallbackStatTrack()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackStatTrak);
}

int* C_CEconWearable::GetFallbackPaintKit()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackPaintKit);
}

int* C_CEconWearable::GetFallbackSeed()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackSeed);
}

float* C_CEconWearable::GetFallbackWear()
{
	return GetFieldPointer<float>(netvars_c_base_combat_weapon->m_flFallbackWear);
}

int* C_CEconWearable::GetEntityQuality()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_iEntityQuality);
}

int* C_CEconWearable::GetItemIDHigh()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_iItemIDHigh);
}

int* C_CEconWearable::GetAccountID()
{
	return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_iAccountID);
}

short* C_CEconWearable::GetItemDefinitionIndex()
{
	return GetFieldPointer<short>(netvars_c_base_combat_weapon->m_iItemDefinitionIndex);
}
