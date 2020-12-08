#pragma once
#include <vector>

struct WeaponDropInfo
{
	int ItemDef;
	int Paintkit;
	int Rarity;
	WeaponDropInfo(int item_def, int pk, int rar) : ItemDef(item_def), Paintkit(pk), Rarity(rar) {};
};

std::vector<WeaponDropInfo> GetWeaponsForCrate(const char* name);
