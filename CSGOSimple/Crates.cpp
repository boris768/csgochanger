#include <vector>
#include "Utils.hpp"
#include "SourceEngine/SDK.hpp"
#include "Crates.h"

#include "XorStr.hpp"


void RecursiveAddLootToLootList(SourceEngine::CEconLootListDefinition* lootlist, std::vector<WeaponDropInfo>& drop)
{
	const auto size = lootlist->items.Count();
	auto i = 0;
	do
	{
		SourceEngine::item_list_entry_t& entry = lootlist->items.Element(i);

		if (entry.unk_flag)
		{
			static auto fnGetLootListInterfaceByIndex
				= reinterpret_cast<SourceEngine::CEconLootListDefinition * (__thiscall*)(SourceEngine::CCStrike15ItemSchema*, uintptr_t)>(
					Utils::FindSignature(SourceEngine::memory.client(), "55 8B EC 8B 55 08 56 8B F1 85 D2 78 47")
					);
			const auto insided_lootlist = fnGetLootListInterfaceByIndex(SourceEngine::interfaces.ItemSchema(), entry.item);
			RecursiveAddLootToLootList(insided_lootlist, drop);
		}
		else
		{
			if (entry.item != 0)
			{
				std::string set_rarity(lootlist->name);

				auto rarity = 0;

				if (set_rarity.find("_common") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_COMMON;
				else if (set_rarity.find("_uncommon") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_UNCOMMON;
				else if (set_rarity.find("_rare") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_RARE;
				else if (set_rarity.find("_mythical") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_MYTHICAL;
				else if (set_rarity.find("_legendary") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_LEGENDARY;
				else if (set_rarity.find("_ancient") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_ANCIENT;
				else if (set_rarity.find("_immortal") != std::string::npos)
					rarity = SourceEngine::ITEM_RARITY_IMMORTAL;

				drop.emplace_back(entry.item, entry.paintkit, rarity );
			}
		}
		++i;
	} while (i < size);
}

std::vector<WeaponDropInfo> GetWeaponsForCrate(const char* name)
{
	std::vector<WeaponDropInfo> drop;

	static auto fnGetLootListByName
		= reinterpret_cast<SourceEngine::CEconLootListDefinition*(__thiscall*)(SourceEngine::CCStrike15ItemSchema*, const char*, signed int)>(
			Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 8B 55 08 81 EC ? ? ? ? 56 57"))
			);

	SourceEngine::CEconLootListDefinition* lootlist = fnGetLootListByName(
		SourceEngine::interfaces.ItemSchema(), name, 0);

	RecursiveAddLootToLootList(lootlist, drop);
	
	return drop;
}
