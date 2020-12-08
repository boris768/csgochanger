// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CCStrike15ItemSchema.h"
#include "KeyValues.h"
#include "Utils.hpp"
#include "Hooks.hpp"
#include "C_EconItemView.h"
#include "CustomWinAPI.h"
#include <ctime>
//#include "XorStr.hpp"

offsets_ItemSchema* offsets_itemschema = nullptr;;

SourceEngine::offsets_CCStrike15ItemSchema::offsets_CCStrike15ItemSchema()
{	
	CUtlHashMapLarge__Find = Utils::FindSignature(memory.client(), /*XorStr*/("55 8B EC 83 EC 10 56 8B F1 89 ? ? 83 ? ? ? 75 ? 83 C8 FF"));
	//   55 8B EC 51 8B ? ? ? ? ? 53 56 8B ? ? ? ? ? 89
	AddPaintKitDefinition = reinterpret_cast<AddPaintKitDefinition_t>(Utils::FindSignature(memory.client(), /*XorStr*/("55 8B EC 83 EC 0C 8B 81 ? ? ? ? 53 56 8D B1")));
	//   55 8B EC 51 53 8B D9 56 33 F6 57 39 ? ? ? ? ? 0F
	GetPaintKitDefinitionByName = reinterpret_cast<GetPaintKitDefinitionByName_t>(Utils::FindSignature(memory.client(), /*XorStr*/("55 8B EC 51 53 8B D9 56 33 F6 57 39 B3 ? ? ? ? 0F")));
	GetPaintKitDefinition = reinterpret_cast<GetPaintKitDefinition_t>
	(Utils::dwGetCallAddress(Utils::FindSignature(memory.client(), /*XorStr*/("E8 ? ? ? ? 85 C0 74 47 8B 07 8B"))));
	s_nCurrentPaintKitId = Utils::FindSignature(memory.client(), /*XorStr*/("55 8B EC 83 EC 08 56 57 C7")) + 25;
	Items = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.client(), /*XorStr*/("8B 87 ? ? ? ? 83 7C 88 08 FF 7C 6A") ) + 2);
	alternative_icons2_offset = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.client(), /*XorStr*/("8D B0 ? ? ? ? 89 45 E8 8B CE E8")) + 2) + 4;
	paint_kits_rarity_offset = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.client(), /*XorStr*/("74 75 8B 83 ? ? ? ? 8D 0C 76")) + 4);
}

bool SourceEngine::CStickerKit::InitFromKeyValues(KeyValues* pKVEntry)
{
	sName.Set(pKVEntry->GetString("name", this->sName.String()));
	sDescriptionString.Set(pKVEntry->GetString("description_string", this->sDescriptionString.String()));
	sItemName.Set(pKVEntry->GetString("item_name", this->sItemName.String()));

	nRarity = 5;

	sMaterialPath.Set(pKVEntry->GetString("sticker_material", this->sMaterialPath.String()));
	if (*sMaterialPath.String())
	{
		sMaterialPathNoDrips.Set(pKVEntry->GetString("sticker_material_nodrips", nullptr));
	}
	
	m_strInventoryImage.Set(pKVEntry->GetString("image_inventory", ""));

	m_sIconURLSmall.Get()[0] = '\0';
	m_sIconURLLarge.Get()[0] = '\0';


	flRotateStart = pKVEntry->GetFloat("gc_generation_settings/rotate_start", this->flRotateStart);
	flRotateEnd = pKVEntry->GetFloat("gc_generation_settings/rotate_end", this->flRotateEnd);

	flScaleMin = pKVEntry->GetFloat("gc_generation_settings/scale_min", this->flScaleMin);
	flScaleMax = pKVEntry->GetFloat("gc_generation_settings/scale_max", this->flScaleMax);

	flWearMin = pKVEntry->GetFloat("gc_generation_settings/wear_min", this->flWearMin);
	flWearMax = pKVEntry->GetFloat("gc_generation_settings/wear_max", this->flWearMax);

	m_nEventID = pKVEntry->GetInt("tournament_event_id", this->m_nEventID);
	m_nEventTeamID = pKVEntry->GetInt("tournament_team_id", this->m_nEventTeamID);
	m_nPlayerID = pKVEntry->GetInt("tournament_player_id", this->m_nPlayerID);

	m_pKVItem = pKVEntry ? pKVEntry->MakeCopy() : nullptr;
	return true;
}

int SourceEngine::CPaintKit::BInitFromKV(KeyValues* kv, CPaintKit* default_paintkit)
{
	PaintKitID = atoi(kv->GetName());

	rarity = kv->GetInt(/*XorStr*/("rarity"), default_paintkit->rarity);
	same_name_family_aggregate.Set(kv->GetString("same_name_family_aggregate", default_paintkit->same_name_family_aggregate.Get() ));
	name.Set(kv->GetString(/*XorStr*/("name"), default_paintkit->name.Get()));
	description_string.Set(kv->GetString(/*XorStr*/("description_string"), default_paintkit->description_string.Get()));
	description_tag.Set(kv->GetString(/*XorStr*/("description_tag"), default_paintkit->description_tag.Get()));

	const char* pattern_path = kv->GetString(/*XorStr*/("pattern"));
	if (g_pWinApi->dwPathIsRelativeA(pattern_path) == FALSE)
	{
		pattern.Format("//./%s", pattern_path);
	}
	else
	{
		pattern.Set(pattern_path);
	}
	use_normal = kv->GetInt(/*XorStr*/("use_normal"), default_paintkit->use_normal) != 0;
	if (use_normal)
	{
		const char* normal_path = kv->GetString(/*XorStr*/("normal"));
		if(g_pWinApi->dwPathIsRelativeA(normal_path) == FALSE)
		{
			pattern.Format("//./%s", normal_path);
		}
		else
		{
			normal.Set(normal_path);
		}
	}
	logo_material.Set(kv->GetString(/*XorStr*/("logo_material"), default_paintkit->logo_material.Get()));
	vmt_path.Set(kv->GetString(/*XorStr*/("vmt_path"), default_paintkit->vmt_path.Get()));

	const auto kv2 = kv->FindKey(/*XorStr*/("vmt_overrides"), false);
	if(kv2)
	{
		vmt_overrides = new KeyValues(/*XorStr*/("CustomCharacter"));
		vmt_overrides->MergeFrom(kv2, KeyValues::MERGE_KV_UPDATE);
	}

	style = kv->GetInt(/*XorStr*/("style"), default_paintkit->style);
	seed = static_cast<int8_t>(kv->GetInt(/*XorStr*/("seed"), default_paintkit->seed));
	phongexponent = static_cast<int8_t>(kv->GetInt(/*XorStr*/("phongexponent"), default_paintkit->phongexponent));
	phongalbedoboost = static_cast<int8_t>(kv->GetInt(/*XorStr*/("phongalbedoboost"), default_paintkit->phongalbedoboost));
	phongintensity = static_cast<int8_t>(kv->GetInt(/*XorStr*/("phongintensity"), default_paintkit->phongintensity));

	wear_default = static_cast<float>(kv->GetFloat(/*XorStr*/("wear_default"), default_paintkit->wear_default));
	wear_remap_min = static_cast<float>(kv->GetFloat(/*XorStr*/("wear_remap_min"), default_paintkit->wear_remap_min));
	wear_remap_max = static_cast<float>(kv->GetFloat(/*XorStr*/("wear_remap_max"), default_paintkit->wear_remap_max));

	pattern_scale = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_scale"), default_paintkit->pattern_scale));
	pattern_offset_x_start = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_offset_x_start"), default_paintkit->pattern_offset_x_start));
	pattern_offset_x_end = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_offset_x_end"), default_paintkit->pattern_offset_x_end));
	pattern_offset_y_start = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_offset_y_start"), default_paintkit->pattern_offset_y_start));
	pattern_offset_y_end = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_offset_y_end"), default_paintkit->pattern_offset_y_end));
	pattern_rotate_start = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_rotate_start"), default_paintkit->pattern_rotate_start));
	pattern_rotate_end = static_cast<float>(kv->GetFloat(/*XorStr*/("pattern_rotate_end"), default_paintkit->pattern_rotate_end));
	logo_scale = static_cast<float>(kv->GetFloat(/*XorStr*/("logo_scale"), default_paintkit->logo_scale));
	logo_offset_x = static_cast<float>(kv->GetFloat(/*XorStr*/("logo_offset_x"), default_paintkit->logo_offset_x));
	logo_offset_y = static_cast<float>(kv->GetFloat(/*XorStr*/("logo_offset_y"), default_paintkit->logo_offset_y));
	logo_rotation = static_cast<float>(kv->GetFloat(/*XorStr*/("logo_rotation"), default_paintkit->logo_rotation));

	ignore_weapon_size_scale = kv->GetInt(/*XorStr*/("ignore_weapon_size_scale"), default_paintkit->ignore_weapon_size_scale != 0) != 0;
	view_model_exponent_override_size = kv->GetInt(/*XorStr*/("view_model_exponent_override_size"), default_paintkit->view_model_exponent_override_size);
	only_first_material = kv->GetInt(/*XorStr*/("only_first_material"), default_paintkit->only_first_material != 0) != 0;
	pearlescent = static_cast<float>(kv->GetFloat(/*XorStr*/("pearlescent"), default_paintkit->pearlescent));
	char* buffer = new char[16];
	for (size_t i = 0; i < 4; ++i)
	{
		snprintf(buffer, 16, ("color%i"), i);
		auto* found = kv->FindKey(buffer, false);
		if (found)
		{
			const auto* const color_value = found->GetString();
			unsigned _color[4];

			sscanf(color_value, ("%u %u %u"), &_color[0], &_color[1], &_color[2]);
			_color[3] = -1;
			//memcpy(color[i], _color, sizeof(int8_t) * 4);
			for (size_t j = 0; j < 4; ++j)
			{
				color[i][j] = static_cast<uint8_t>(_color[j]);
			}
		}
		else
		{
			memcpy(color[i], default_paintkit->color[i], sizeof(int8_t) * 4);
		}

		snprintf(buffer, 16, ("logocolor%i"), i);
		found = kv->FindKey(buffer, false);
		if (found)
		{
			const auto* const color_value = found->GetString();
			unsigned _color[4];

			sscanf(color_value, ("%u %u %u"), &_color[0], &_color[1], &_color[2]);
			_color[3] = -1;
			//memcpy(logocolor[i], _color, sizeof(int8_t) * 4);
			for (size_t j = 0; j < 4; ++j)
			{
				logocolor[i][j] = static_cast<uint8_t>(_color[j]);
			}
		}
		else
		{
			memcpy(logocolor[i], default_paintkit->logocolor[i], sizeof(int8_t) * 4);
		}
	}
	delete[] buffer;

	return 1;
}

void InjectStickerkit(const char* stickerkit_kv_file, SourceEngine::CRC32_t texture_crc32)
{
	auto* filesystem = SourceEngine::interfaces.FileSystem();
	auto* memalloc = SourceEngine::interfaces.MemAlloc();
	if (!filesystem->FileExists(stickerkit_kv_file))
	{
		return;
	}

	auto kv = new SourceEngine::KeyValues(/*XorStr*/("CEconItemSchema"));
	if(kv->LoadFromFile(DWORD(filesystem), stickerkit_kv_file, /*XorStr*/("MOD")))
	{
		auto material_name = kv->GetString(/*XorStr*/("material"), /*XorStr*/("default/sticker_default"));
		std::string path_to_pattern(/*XorStr*/("materials/models/weapons/customization/stickers/"));
		path_to_pattern.append(material_name);
		path_to_pattern.append(/*XorStr*/(".vtf"));

		if (texture_crc32 != 0xFACE14)
		{
			const FileHandle_t file = filesystem->Open(path_to_pattern.c_str(), /*XorStr*/("rb"));
			if (!file)
			{
				//cant open file to check, exit
				delete kv;
				return;
			}
			const size_t filesize = filesystem->Size(file);
			if (!filesize)
			{
				//file empty
				delete kv;
				return;
			}
			void* p_output = memalloc->Alloc(4096);
			int readed_len;
			SourceEngine::CRC32_t checksum;
			SourceEngine::CRC32_Init(&checksum);
			while ((readed_len = filesystem->Read(p_output, 4096, file)))
			{
				SourceEngine::CRC32_ProcessBuffer(&checksum, p_output, readed_len);
			}
			filesystem->Close(file);
			SourceEngine::CRC32_Final(&checksum);

			if (checksum != texture_crc32)
			{
				//checksums not valid, exit
				delete kv;
				memalloc->Free(p_output);
				return;
			}
			memalloc->Free(p_output);
		}

		//StickerInfo_t new_sticker(
		//	atoi(kv->GetName()), 
		//	kv->GetWString(/*XorStr*/("description_string"), /*XorStr*/W(L"no name")), 
		//	material_name
		//);
		//new_sticker.iStickerKitID = atoi(kv->GetName());
		//new_sticker.szStickerMaterial = kv->GetString(/*XorStr*/("material"), /*XorStr*/("default/sticker_default"));		
		//new_sticker.szStickerName = kv->GetWString(/*XorStr*/("description_string"), /*XorStr*/W(L"no name"));
		int iStickerKitID = atoi(kv->GetName());
		const char* cname = kv->GetString(/*XorStr*/("name"), /*XorStr*/("no name"));
		const char* description_string = kv->GetString(/*XorStr*/("description_string"), /*XorStr*/("no name"));
		std::wstring name = kv->GetWString(/*XorStr*/("name"), /*XorStr*/(L"no name"));
		g_vStickers.emplace_back(iStickerKitID, name, material_name);
		//description_string_localize
		SourceEngine::CUtlMap<int, SourceEngine::CStickerKit*, int, SourceEngine::CDefLess<int> >* m_mapStickerKits = SourceEngine::interfaces.ItemSchema()->GetStickerKits();
		const int iIndex = m_mapStickerKits->Find(iStickerKitID);
		if (m_mapStickerKits->IsValidIndex(iIndex))
		{
			m_mapStickerKits->Remove(iStickerKitID);
		}
		auto* pStickerKit = new SourceEngine::CStickerKit();

		pStickerKit->sName.Set(cname);
		pStickerKit->sDescriptionString.Set(description_string);
		pStickerKit->sItemName.Set(cname);

		pStickerKit->nRarity = 5;

		pStickerKit->sMaterialPath.Set(material_name);
		
		m_mapStickerKits->Insert(iStickerKitID, pStickerKit);
	}
	delete kv;
}

int itemid_index = 2000000;

void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	const std::vector<CustomSkinWeapon>::iterator& config)
{
	if (item)
	{
		Hooks::g_fnCleanInventoryImageCacheDir();
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return;
		local_player_inventory->RemoveItem(item);
	}
	config->inventory_item = InjectItem(team, *config);
	
}

void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	CustomSkinKnife& config)
{
	if (item)
	{
		Hooks::g_fnCleanInventoryImageCacheDir();
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return;
		local_player_inventory->RemoveItem(item);
	}
	config.inventory_item = InjectKnife(team, config);
}

void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	CustomSkinWearable& config)
{
	if (item)
	{
		Hooks::g_fnCleanInventoryImageCacheDir();
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return;
		local_player_inventory->RemoveItem(item);
	}
	config.inventory_item = InjectWearable(team, config);
}

SourceEngine::CEconItem* InjectItem(SourceEngine::TeamID team, CustomSkinWeapon& weapon)
{
#ifdef _DEBUG
	assert(weapon.skin && weapon.skin->iPaintKitID);
#endif
	if(weapon.skin->iPaintKitID > 0)
	{
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return nullptr;
		SourceEngine::CEconItem* item = SourceEngine::CPlayerInventory::CreateEconItem();
		const auto steam_id = local_player_inventory->GetSteamID();
		*item->GetAccountID() = steam_id;
		*item->GetDefIndex() = weapon.info->iItemDefinitionIndex;
		*item->GetItemID() = itemid_index++;//CombinedItemIdMakeFromDefIndexAndPaint(weapon.info->iItemDefinitionIndex, weapon.skin->iPaintKitID, weapon.iEntityQuality);
		*item->GetInventory() = 1;
		*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;
		item->SetAttributeValue(75, static_cast<uint32_t>(std::time(nullptr)));
		if(weapon.bStatTrack)
		{
			item->SetStatTrak(weapon.iKills);
		}
		else
		{
			item->SetQuality(static_cast<SourceEngine::ItemQuality>(weapon.iEntityQuality));
		}
		item->SetPaintKit(static_cast<float>(weapon.skin->iPaintKitID));
		item->SetPaintSeed(static_cast<float>(weapon.iSeed));
		item->SetPaintWear(weapon.fWear);
		item->SetOrigin(24);
		item->SetRarity(static_cast<SourceEngine::ItemRarity>(Utils::CombinedItemAndPaintRarity(
			weapon.info->game_item_definition->item_rarity, weapon.skin->m_pPaintKitDefinition->rarity)));
		item->SetLevel(1);
		item->SetInUse(true);
		if(weapon.name[0])
		{
			item->SetCustomName(weapon.name);
		}
		for (size_t j = 0; j < weapon.info->iMaxStickers; ++j)
		{
			auto* const stiker = &weapon.Stickers.at(j);
			if (stiker->iStickerKitID == 0)
				continue;

			item->AddSticker(j, stiker->iStickerKitID, stiker->fStickerWear, stiker->fStickerScale, stiker->fStickerRotation);
		}
		
		//Hooks::g_fnCleanInventoryImageCacheDir();
		local_player_inventory->AddEconItem(item, 1, 0, 1);

		SourceEngine::C_EconItemView* itemView = local_player_inventory->GetInventoryItemByItemID(*item->GetItemID());
		if (!itemView)
			return item;
		
		SourceEngine::CCStrike15ItemDefinition* itemStaticData = itemView->GetStaticData();
		if (!itemStaticData)
			return item;
		
		SourceEngine::interfaces.InventoryManager()->EquipItemInLoadout(team, GetSlotID(weapon.info->iItemDefinitionIndex), *item->GetItemID());
		return item;
	}
	return nullptr;
}


SourceEngine::CEconItem* InjectKnife(SourceEngine::TeamID team, CustomSkinKnife& weapon)
{
#ifdef _DEBUG
	assert(weapon.skin && weapon.skin->iPaintKitID);
#endif
	if (weapon.skin->iPaintKitID > 0)
	{
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return nullptr;
		SourceEngine::CEconItem* item = SourceEngine::CPlayerInventory::CreateEconItem();
		const auto steam_id = local_player_inventory->GetSteamID();
		*item->GetAccountID() = steam_id;
		*item->GetDefIndex() = weapon.info->iItemDefinitionIndex;
		*item->GetItemID() = itemid_index++; //CombinedItemIdMakeFromDefIndexAndPaint(weapon.info->iItemDefinitionIndex, weapon.skin->iPaintKitID, SourceEngine::ItemQuality::ITEM_QUALITY_SKIN);
		*item->GetInventory() = 1;
		*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;
		item->SetAttributeValue(75, static_cast<uint32_t>(std::time(nullptr)));
		if (weapon.bStatTrack)
		{
			item->SetStatTrak(weapon.iKills);
		}
		else
		{
			item->SetQuality(SourceEngine::ItemQuality::ITEM_QUALITY_SKIN);
		}
		item->SetPaintKit(static_cast<float>(weapon.skin->iPaintKitID));
		item->SetPaintSeed(static_cast<float>(weapon.iSeed));
		item->SetPaintWear(weapon.fWear);
		item->SetOrigin(24);
		item->SetRarity(static_cast<SourceEngine::ItemRarity>(Utils::CombinedItemAndPaintRarity(
			weapon.info->game_item_definition->item_rarity, weapon.skin->m_pPaintKitDefinition->rarity)));
		item->SetLevel(1);
		item->SetInUse(true);
		if (weapon.name[0])
		{
			item->SetCustomName(weapon.name);
		}
		//Hooks::g_fnCleanInventoryImageCacheDir();
		local_player_inventory->AddEconItem(item, 1, 0, 1);

		SourceEngine::C_EconItemView* itemView = local_player_inventory->GetInventoryItemByItemID(*item->GetItemID());
		if (!itemView)
			return item;

		SourceEngine::CCStrike15ItemDefinition* itemStaticData = itemView->GetStaticData();
		if (!itemStaticData)
			return item;

		SourceEngine::interfaces.InventoryManager()->EquipItemInLoadout(team, GetSlotID(weapon.info->iItemDefinitionIndex), *item->GetItemID());
		return item;
	}
	return nullptr;
}
SourceEngine::CEconItem* InjectWearable(SourceEngine::TeamID team, CustomSkinWearable& weapon)
{

#ifdef _DEBUG
	assert(weapon.skin && weapon.skin->iPaintKitID);
#endif
	if (weapon.skin->iPaintKitID > 0)
	{
		auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
		if (!local_player_inventory)
			return nullptr;
		SourceEngine::CEconItem* item = SourceEngine::CPlayerInventory::CreateEconItem();
		const auto steam_id = local_player_inventory->GetSteamID();
		*item->GetAccountID() = steam_id;
		*item->GetDefIndex() = weapon.info->iItemDefinitionIndex;
		*item->GetItemID() = itemid_index++; //CombinedItemIdMakeFromDefIndexAndPaint(weapon.info->iItemDefinitionIndex, weapon.skin->iPaintKitID);
		*item->GetInventory() = 1;
		*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;
		item->SetAttributeValue(75, static_cast<uint32_t>(std::time(nullptr)));
		item->SetQuality(SourceEngine::ItemQuality::ITEM_QUALITY_SKIN);
		
		item->SetPaintKit(static_cast<float>(weapon.skin->iPaintKitID));
		item->SetPaintSeed(static_cast<float>(weapon.iSeed));
		item->SetPaintWear(weapon.fWear);
		item->SetOrigin(24);
		item->SetRarity(static_cast<SourceEngine::ItemRarity>(Utils::CombinedItemAndPaintRarity(
			weapon.info->game_item_definition->item_rarity, weapon.skin->m_pPaintKitDefinition->rarity)));
		item->SetLevel(1);
		item->SetInUse(true);
		if (weapon.name[0])
		{
			item->SetCustomName(weapon.name);
		}
		//Hooks::g_fnCleanInventoryImageCacheDir();
		local_player_inventory->AddEconItem(item, 1, 0, 1);

		SourceEngine::C_EconItemView* itemView = local_player_inventory->GetInventoryItemByItemID(*item->GetItemID());
		if (!itemView)
			return item;

		SourceEngine::CCStrike15ItemDefinition* itemStaticData = itemView->GetStaticData();
		if (!itemStaticData)
			return item;

		SourceEngine::interfaces.InventoryManager()->EquipItemInLoadout(team, GetSlotID(weapon.info->iItemDefinitionIndex), *item->GetItemID());
		return item;
	}
	return nullptr;
}

void LoadInventory()
{
	auto* local_player_inventory = SourceEngine::interfaces.InventoryManager()->GetLocalPlayerInventory();
	if (!local_player_inventory)
		return;
	const auto steam_id = local_player_inventory->GetSteamID();

	for (int i = 3; i <= 49; ++i)
	{
		SourceEngine::CEconItem* music_kit = SourceEngine::CPlayerInventory::CreateEconItem();
		*music_kit->GetAccountID() = steam_id;
		*music_kit->GetDefIndex() = 1314;
		*music_kit->GetItemID() = itemid_index++; //CombinedItemIdMakeFromDefIndexAndPaint(1314, i, SourceEngine::ItemQuality::ITEM_QUALITY_SKIN);
		*music_kit->GetInventory() = 1;
		*music_kit->GetFlags() = 0;
		*music_kit->GetOriginalID() = 0;
		music_kit->SetAttributeValue(75, static_cast<uint32_t>(std::time(nullptr)));
		music_kit->SetAttributeValue(166, i);
		music_kit->SetOrigin(9);
		music_kit->SetRarity(SourceEngine::ITEM_RARITY_ANCIENT);
		music_kit->SetQuality(SourceEngine::ITEM_QUALITY_SKIN);
		music_kit->SetLevel(1);
		music_kit->SetInUse(true);
		// Time acquired attribute

		local_player_inventory->AddEconItem(music_kit, true, false, true);
		if (Options::musickit_index == i)
		{
			SourceEngine::interfaces.InventoryManager()->EquipItemInLoadout(0, 54, *music_kit->GetItemID());
		}
	}
		for(std::vector<CustomSkinWeapon>::value_type& weapon : Options::config_ct.weapons)
		{
			if(weapon.info && weapon.skin && weapon.skin->iPaintKitID > 0)
			{
				const auto avalTeam = GetAvailableClassID(weapon.info->iItemDefinitionIndex);

				if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_COUNTER_TERRORIST)
				{
					weapon.inventory_item = InjectItem(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST,weapon);
				}
			}
		}
		for(std::vector<CustomSkinWeapon>::value_type& weapon : Options::config_t.weapons)
		{
			if(weapon.info && weapon.skin && weapon.skin->iPaintKitID > 0)
			{
				const auto avalTeam = GetAvailableClassID(weapon.info->iItemDefinitionIndex);

				if (avalTeam <= SourceEngine::TeamID::TEAM_TERRORIST)
				{
					weapon.inventory_item = InjectItem(SourceEngine::TeamID::TEAM_TERRORIST, weapon);
				}
			}
		}
		if(Options::config_ct.knife.info && Options::config_ct.knife.skin && Options::config_ct.knife.skin->iPaintKitID > 0)
		{
			Options::config_ct.knife.inventory_item = InjectKnife(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, Options::config_ct.knife);
		}
		if(Options::config_t.knife.info && Options::config_t.knife.skin && Options::config_t.knife.skin->iPaintKitID > 0)
		{
			Options::config_t.knife.inventory_item = InjectKnife(SourceEngine::TeamID::TEAM_TERRORIST, Options::config_t.knife);
		}

		if(Options::config_ct.wearable.info && Options::config_ct.wearable.skin && Options::config_ct.wearable.skin->iPaintKitID > 0)
		{
			Options::config_ct.wearable.inventory_item = InjectWearable(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, Options::config_ct.wearable);
		}
		if(Options::config_t.wearable.info && Options::config_t.wearable.skin && Options::config_t.wearable.skin->iPaintKitID > 0)
		{
			Options::config_t.wearable.inventory_item = InjectWearable(SourceEngine::TeamID::TEAM_TERRORIST, Options::config_t.wearable);
		}
}

SourceEngine::TeamID GetAvailableClassID(int definition_index)
{
	switch (definition_index)
	{
	case SourceEngine::EItemDefinitionIndex::weapon_knife_bayonet:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_flip:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_gut:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_karambit:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_m9_bayonet:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_tactical:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_falchion:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_survival_bowie:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_butterfly:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_push:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_ursus:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_gypsy_jackknife:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_stiletto:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_widowmaker:
	case SourceEngine::EItemDefinitionIndex::weapon_elite:
	case SourceEngine::EItemDefinitionIndex::weapon_p250:
	case SourceEngine::EItemDefinitionIndex::weapon_cz75a:
	case SourceEngine::EItemDefinitionIndex::weapon_deagle:
	case SourceEngine::EItemDefinitionIndex::weapon_revolver:
	case SourceEngine::EItemDefinitionIndex::weapon_mp7:
	case SourceEngine::EItemDefinitionIndex::weapon_ump:
	case SourceEngine::EItemDefinitionIndex::weapon_p90:
	case SourceEngine::EItemDefinitionIndex::weapon_bizon:
	case SourceEngine::EItemDefinitionIndex::weapon_ssg08:
	case SourceEngine::EItemDefinitionIndex::weapon_awp:
	case SourceEngine::EItemDefinitionIndex::weapon_nova:
	case SourceEngine::EItemDefinitionIndex::weapon_xm1014:
	case SourceEngine::EItemDefinitionIndex::weapon_m249:
	case SourceEngine::EItemDefinitionIndex::weapon_negev:
	case SourceEngine::EItemDefinitionIndex::glove_studded_bloodhound:
	case SourceEngine::EItemDefinitionIndex::glove_sporty:
	case SourceEngine::EItemDefinitionIndex::glove_slick:
	case SourceEngine::EItemDefinitionIndex::glove_leather_wrap:
	case SourceEngine::EItemDefinitionIndex::glove_motorcycle:
	case SourceEngine::EItemDefinitionIndex::glove_specialist:
	case SourceEngine::EItemDefinitionIndex::glove_studded_hydra:
		return SourceEngine::TeamID::TEAM_SPECTATOR;

	case SourceEngine::EItemDefinitionIndex::weapon_glock:
	case SourceEngine::EItemDefinitionIndex::weapon_ak47:
	case SourceEngine::EItemDefinitionIndex::weapon_mac10:
	case SourceEngine::EItemDefinitionIndex::weapon_g3sg1:
	case SourceEngine::EItemDefinitionIndex::weapon_tec9:
	case SourceEngine::EItemDefinitionIndex::weapon_galilar:
	case SourceEngine::EItemDefinitionIndex::weapon_sg556:
	case SourceEngine::EItemDefinitionIndex::weapon_sawedoff:
		return SourceEngine::TeamID::TEAM_TERRORIST;

	case SourceEngine::EItemDefinitionIndex::weapon_aug:
	case SourceEngine::EItemDefinitionIndex::weapon_famas:
	case SourceEngine::EItemDefinitionIndex::weapon_mag7:
	case SourceEngine::EItemDefinitionIndex::weapon_fiveseven:
	case SourceEngine::EItemDefinitionIndex::weapon_usp_silencer:
	case SourceEngine::EItemDefinitionIndex::weapon_hkp2000:
	case SourceEngine::EItemDefinitionIndex::weapon_mp9:
	case SourceEngine::EItemDefinitionIndex::weapon_m4a1_silencer:
	case SourceEngine::EItemDefinitionIndex::weapon_m4a1:
	case SourceEngine::EItemDefinitionIndex::weapon_scar20:
		return SourceEngine::TeamID::TEAM_COUNTER_TERRORIST;

	default:
		return SourceEngine::TeamID::TEAM_UNASSIGNED;
	}
}

int GetSlotID(int definition_index)
{
	switch (definition_index)
	{
	case SourceEngine::EItemDefinitionIndex::weapon_knife_bayonet:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_flip:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_gut:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_karambit:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_m9_bayonet:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_tactical:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_falchion:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_survival_bowie:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_butterfly:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_push:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_ursus:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_gypsy_jackknife:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_stiletto:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_widowmaker:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_ct:
	case SourceEngine::EItemDefinitionIndex::weapon_knife_t:
		return 0;
	case SourceEngine::EItemDefinitionIndex::weapon_usp_silencer:
	case SourceEngine::EItemDefinitionIndex::weapon_hkp2000:
	case SourceEngine::EItemDefinitionIndex::weapon_glock:
		return 2;
	case SourceEngine::EItemDefinitionIndex::weapon_elite:
		return 3;
	case SourceEngine::EItemDefinitionIndex::weapon_p250:
		return 4;
	case SourceEngine::EItemDefinitionIndex::weapon_tec9:
	case SourceEngine::EItemDefinitionIndex::weapon_cz75a:
	case SourceEngine::EItemDefinitionIndex::weapon_fiveseven:
		return 5;
	case SourceEngine::EItemDefinitionIndex::weapon_deagle:
	case SourceEngine::EItemDefinitionIndex::weapon_revolver:
		return 6;
	case SourceEngine::EItemDefinitionIndex::weapon_mp9:
	case SourceEngine::EItemDefinitionIndex::weapon_mac10:
		return 8;
	case SourceEngine::EItemDefinitionIndex::weapon_mp5sd:
	case SourceEngine::EItemDefinitionIndex::weapon_mp7:
		return 9;
	case SourceEngine::EItemDefinitionIndex::weapon_ump:
		return 10;
	case SourceEngine::EItemDefinitionIndex::weapon_p90:
		return 11;
	case SourceEngine::EItemDefinitionIndex::weapon_bizon:
		return 12;
	case SourceEngine::EItemDefinitionIndex::weapon_famas:
	case SourceEngine::EItemDefinitionIndex::weapon_galilar:
		return 14;
	case SourceEngine::EItemDefinitionIndex::weapon_m4a1_silencer:
	case SourceEngine::EItemDefinitionIndex::weapon_m4a1:
	case SourceEngine::EItemDefinitionIndex::weapon_ak47:
		return 15;
	case SourceEngine::EItemDefinitionIndex::weapon_ssg08:
		return 16;
	case SourceEngine::EItemDefinitionIndex::weapon_sg556:
	case SourceEngine::EItemDefinitionIndex::weapon_aug:
		return 17;
	case SourceEngine::EItemDefinitionIndex::weapon_awp:
		return 18;
	case SourceEngine::EItemDefinitionIndex::weapon_g3sg1:
	case SourceEngine::EItemDefinitionIndex::weapon_scar20:
		return 19;
	case SourceEngine::EItemDefinitionIndex::weapon_nova:
		return 20;
	case SourceEngine::EItemDefinitionIndex::weapon_xm1014:
		return 21;
	case SourceEngine::EItemDefinitionIndex::weapon_sawedoff:
	case SourceEngine::EItemDefinitionIndex::weapon_mag7:
		return 22;
	case SourceEngine::EItemDefinitionIndex::weapon_m249:
		return 23;
	case SourceEngine::EItemDefinitionIndex::weapon_negev:
		return 24;
	case SourceEngine::EItemDefinitionIndex::glove_t_side:
	case SourceEngine::EItemDefinitionIndex::glove_ct_side:
	case SourceEngine::EItemDefinitionIndex::glove_studded_bloodhound:
	case SourceEngine::EItemDefinitionIndex::glove_sporty:
	case SourceEngine::EItemDefinitionIndex::glove_slick:
	case SourceEngine::EItemDefinitionIndex::glove_leather_wrap:
	case SourceEngine::EItemDefinitionIndex::glove_motorcycle:
	case SourceEngine::EItemDefinitionIndex::glove_specialist:
	case SourceEngine::EItemDefinitionIndex::glove_studded_hydra:
		return 41;
	case SourceEngine::weapon_taser:
	case SourceEngine::weapon_knifegg:
	case SourceEngine::weapon_flashbang:
	case SourceEngine::weapon_hegrenade:
	case SourceEngine::weapon_smokegrenade:
	case SourceEngine::weapon_molotov:
	case SourceEngine::weapon_decoy:
	case SourceEngine::weapon_incgrenade:
	case SourceEngine::weapon_c4:
	default:
		return -1;
	}
}

offsets_ItemSchema::offsets_ItemSchema(): GCSDK_CGCClient_FindSOCache(nullptr)
{
	GetItemSchema = reinterpret_cast<GetItemSchemaFn1>(Utils::FindSignature(
		SourceEngine::memory.client(), /*XorStr*/("A1 ? ? ? ? 85 C0 75 53")));
	SetDynamicAttributeValue = reinterpret_cast<SetDynamicAttributeValue_t>(Utils::FindSignature(
		SourceEngine::memory.client(), /*XorStr*/("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A 00")));
	AddEconItem = reinterpret_cast<AddEconItem_t>(Utils::FindSignature(SourceEngine::memory.client(),
	                                                                   /*XorStr*/(
		                                                                   "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B 08"))
	);
	GetInventoryItemByItemID = reinterpret_cast<GetInventoryItemByItemID_t>(Utils::FindSignature(
		SourceEngine::memory.client(), /*XorStr*/("55 8B EC 8B 55 08 83 EC 10 8B C2")));
	CreateSharedObjectSubclass_EconItem = reinterpret_cast<CreateSharedObjectSubclass_EconItem_t>(
		*reinterpret_cast<uintptr_t*>(Utils::FindSignature(SourceEngine::memory.client(),
		                                                   /*XorStr*/(
			                                                   "C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4"))
			+ 3));
	SetCustomName = reinterpret_cast<SetCustomName_t>(Utils::dwGetCallAddress(
		Utils::FindSignature(SourceEngine::memory.client(),
		                     /*XorStr*/("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"))));
	SetCustomDesc = reinterpret_cast<SetCustomName_t>(Utils::dwGetCallAddress(
		Utils::FindSignature(SourceEngine::memory.client(), /*XorStr*/("E8 ? ? ? ? 33 DB 39 5E 3C 7E 5E"))));

	UpdateEquippedState = reinterpret_cast<UpdateEquippedState_t>(
		Utils::FindSignature(SourceEngine::memory.client(), /*XorStr*/("55 8B EC 8B 45 08 8B D0 C1 EA 10")));

	EquipItemInLoadout = reinterpret_cast<EquipItemInLoadout_t>(
		Utils::FindSignature(SourceEngine::memory.client(),
		                     /*XorStr*/("55 8B EC 83 E4 F8 83 EC 24 83 ? ? ? ? ? ? 53 56 57 8B F9")));

	FindOrCreateReferenceEconItem = reinterpret_cast<FindOrCreateReferenceEconItem_t>(Utils::FindSignature(
		SourceEngine::memory.client(), /*XorStr*/("55 8B EC 51 8B 55 0C 53 56")));


	GCSDK_CSharedObjectCache_CreateBaseTypeCache = reinterpret_cast<GCSDK_CSharedObjectCache_CreateBaseTypeCache_t>(
		Utils::FindSignature(SourceEngine::memory.client(), /*XorStr*/("55 8B EC 51 53 56 8B D9 8D 45 08")));

	g_GCClientSystem = **reinterpret_cast<uintptr_t**>(Utils::FindSignature(
		SourceEngine::memory.client(), /*XorStr*/("8B 0D ? ? ? ? 6A 00 83 EC 10")) + 2);
	so_cache_map = *reinterpret_cast<uintptr_t*>((Utils::FindSignature(SourceEngine::memory.client(),
	                                                                   /*XorStr*/("8D 8B ? ? ? ? E8 ? ? ? ? 0F B7 F8")) + 2)
	);

	RemoveItem = reinterpret_cast<RemoveItem_t>(Utils::FindSignature(SourceEngine::memory.client(),
	                                                                 /*XorStr*/(
		                                                                 "55 8B EC 83 E4 F8 51 53 56 57 FF 75 0C 8B F1"))
	);

	find_so_cache = Utils::FindSignature(SourceEngine::memory.client(),
	                                     /*XorStr*/("55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08"));
	create_base_type_cache = Utils::FindSignature(SourceEngine::memory.client(),
	                                              /*XorStr*/("55 8B EC 51 53 56 8B D9 8D 45 08"));
	gc_cs = Utils::FindSignature(SourceEngine::memory.client(), /*XorStr*/("8B 0D ? ? ? ? 6A 00 83 EC 10"));
}

SourceEngine::CPaintKit* paint_kit_buffer = nullptr;
void inject_all_paint_kits(std::vector<paint_kit_inject_request>& requested)
{
	if (requested.empty())
		return;

	auto* memalloc = SourceEngine::interfaces.MemAlloc();
	auto* filesystem = SourceEngine::interfaces.FileSystem();
	auto* localize = SourceEngine::interfaces.LocalizeSystem();
	auto* itemschema = SourceEngine::interfaces.ItemSchema();

	SourceEngine::CPaintKit* paint_kit2 = itemschema->GetPaintKitDefinitionByName(("default")); //using default paint_kit def for build new 
	SourceEngine::CPaintKit* paint_kit3 = itemschema->GetPaintKitDefinitionByName(/*XorStr*/("cu_awp_asimov")); //used to get sample localized string -

	int* s_nCurrentPaintKitId = *reinterpret_cast<int**>(SourceEngine::offsets_cc_strike15_item_schema->s_nCurrentPaintKitId);

	const int desc_str_index = localize->FindIndex(paint_kit3->description_string.Get());
	const char* localize_filename = localize->GetFileNameByIndex(desc_str_index);

	SourceEngine::KeyValues* kv2 = nullptr;

	paint_kit_buffer = static_cast<SourceEngine::CPaintKit*>(memalloc->Alloc(sizeof(SourceEngine::CPaintKit) * requested.size()));
	
	size_t hash_sum_file_buffer_size = 10240;
	void* hash_sum_file_buffer = memalloc->Alloc(hash_sum_file_buffer_size + 1U);
	memset(hash_sum_file_buffer, 0, hash_sum_file_buffer_size);
	size_t index = 0;
	for(auto& request: requested)
	{
		if(!filesystem->FileExists(request.paintkit_kv_file.c_str()))
		{
			continue;
		}

		if (request.kv_crc32 != 0xFACE14)
		{
			const FileHandle_t file = filesystem->Open(request.paintkit_kv_file.c_str(), /*XorStr*/("r"));
			const int filesize = static_cast<int>(filesystem->Size(file));
			if (filesize > hash_sum_file_buffer_size)
			{
				auto* const realloc_buffer = memalloc->Realloc(hash_sum_file_buffer, hash_sum_file_buffer_size * 2U + 1U);
				if (!realloc_buffer)
				{
					//NO MEMORY
					continue;
				}
				hash_sum_file_buffer = realloc_buffer;
				hash_sum_file_buffer_size = hash_sum_file_buffer_size * 2U + 1U;
			}

			const int readed = filesystem->Read(hash_sum_file_buffer, filesize, file);
			filesystem->Close(file);

			if (readed != filesize)
			{
				//file read has errors: not full read
				continue;
			}
			const SourceEngine::CRC32_t checksum = SourceEngine::CRC32_ProcessSingleBuffer(hash_sum_file_buffer, filesize);

			if (checksum != request.kv_crc32)
			{
				//checksums not valid, exit
				continue;
			}
		}

		// ReSharper disable CppInitializedValueIsAlwaysRewritten
		SourceEngine::CPaintKit* pk = &paint_kit_buffer[index];
		// ReSharper restore CppInitializedValueIsAlwaysRewritten
		if (kv2)
		{
			delete kv2;
			kv2 = nullptr;
		}
		kv2 = new SourceEngine::KeyValues(/*XorStr*/("CEconItemSchema"));

		if (kv2->LoadFromFile(reinterpret_cast<DWORD>(filesystem), request.paintkit_kv_file.c_str(), /*XorStr*/("MOD")))
			//if(!kv2->LoadFromBuffer(paintkit_kv_file, (char*)p_output, (DWORD*)SourceEngine::interfaces.FileSystem(), /*XorStr*/("MOD"))) // more safe, but 
		{
			pk = new(pk)SourceEngine::CPaintKit();
			pk->BInitFromKV(kv2, paint_kit2);
		}
		else
		{
			continue;
		}

		if (request.texture_crc32 != 0xFACE14)
		{

			std::string path_to_pattern(/*XorStr*/("materials/models/weapons/customization/paints/"));
			switch (pk->style)
			{
			case 7:
				path_to_pattern.append(/*XorStr*/("custom/"));
				break;
			case 9:
				path_to_pattern.append(/*XorStr*/("gunsmith/"));
				break;
			default:
				continue;
			}
			path_to_pattern.append(pk->pattern.Get());
			path_to_pattern.append(/*XorStr*/(".vtf"));

			const FileHandle_t file = filesystem->Open(path_to_pattern.c_str(), /*XorStr*/("rb"));
			if (!file)
			{
				//cant open file to check, exit
				continue;
			}
			const int filesize = static_cast<int>(filesystem->Size(file));
			if (!filesize)
			{
				//file empty
				continue;
			}
			if (filesize > hash_sum_file_buffer_size)
			{
				auto* const realloc_buffer = memalloc->Realloc(hash_sum_file_buffer, hash_sum_file_buffer_size * 2U + 1U);
				if (!realloc_buffer)
				{
					//NO MEMORY
					continue;
				}
				hash_sum_file_buffer = realloc_buffer;
				hash_sum_file_buffer_size = hash_sum_file_buffer_size * 2U + 1U;
			}

			int readed_len;
			SourceEngine::CRC32_t checksum;
			SourceEngine::CRC32_Init(&checksum);
			while ((readed_len = filesystem->Read(hash_sum_file_buffer, 8192, file)))
			{
				SourceEngine::CRC32_ProcessBuffer(&checksum, hash_sum_file_buffer, readed_len);
			}
			filesystem->Close(file);
			SourceEngine::CRC32_Final(&checksum);

			if (checksum != request.texture_crc32)
			{
				//checksums not valid, exit
				continue;
			}
		}

		if (!localize->addLocalizeString(pk->description_string, kv2->GetWString(/*XorStr*/("description_string_localize")), localize_filename))
		{
			//Warning("Localize string %s cant be add to game", pk->description_string.Get());
			continue;
		}
		if (!localize->addLocalizeString(pk->description_tag, kv2->GetWString(/*XorStr*/("description_tag_localize")), localize_filename))
		{
			//Warning("Localize string %s cant be add to game", pk->description_string.Get());
			continue;
		}

		if(!Hooks::g_pSkinDataBase->add_to_base(pk))
		{
			//skin with same name and paint kit id was loaded previouly
			continue;
		}
		
		*s_nCurrentPaintKitId = pk->PaintKitID;
		const int added_paintkit_id = itemschema->AddPaintKitDefinition(pk->PaintKitID, pk);

		if (added_paintkit_id == -1)
		{
			//Warning("PaintKit %n cant be add!", pk->PaintKitID);
			continue;
		}
		//55 8B EC 83 EC 08 56 57 C7 +24 +25
		//self-test
		//SourceEngine::CPaintKit* check = itemschema->GetPaintKitDefinition(pk->PaintKitID);
		SourceEngine::CPaintKit* check = itemschema->GetPaintKitDefinitionByName(pk->name);
		if (check != nullptr)
		{
			//add skin to our skin db
			const short WeaponID = static_cast<short>(kv2->GetInt(/*XorStr*/("item_definition_index"), -1));
			if (WeaponID != -1)
			{
				
				WeaponSkin* weapon = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(WeaponID);
				if (weapon)
				{
					//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
					//custom.iPaintKitID = check->PaintKitID;
					//custom.szSkinName = localize->Find(check->description_tag.Get());
					++index;
					weapon->vSkins.emplace_back(check);
					continue;
				}
				KnifeSkin* knife = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(WeaponID);
				if (knife)
				{
					//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
					//custom.iPaintKitID = check->PaintKitID;
					//custom.szSkinName = localize->Find(check->description_tag.Get());
					++index;
					knife->vSkins.emplace_back(check);
					continue;
				}
				WearableSkin* glove = WearableSkin::GetWearableSkinByItemDefinitionIndex(WeaponID);
				if (glove)
				{
					//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
					//custom.iPaintKitID = check->PaintKitID;
					//custom.szSkinName = localize->Find(check->description_tag.Get());
					++index;
					glove->vSkins.emplace_back(check);
					continue;
				}
			}
		}

		++index;
	}

	memalloc->Free(hash_sum_file_buffer);
	delete kv2;

}
// BUG: NOT IMPLEMENTED
//void inject_all_paint_kits_async(std::vector<paint_kit_inject_request>& requested)
//{
//
//
//}


void patch_vmtcache_file()
{
	//const auto target_file = /*XorStr*/("resource/vmtcache.txt");
	const auto target_file2 = /*XorStr*/("SkinChanger/custom_gloves.txt");
	auto* filesystem = SourceEngine::interfaces.FileSystem();

	auto* cman = SourceEngine::interfaces.MaterialSystem()->GetCustomMaterialManager();
	//FOR_EACH_MAP_FAST(cman->m_mapVMTKeyValues, i)
	//{
	//	Hooks::changer_log->msg(cman->m_mapVMTKeyValues[i]->GetName());
	//	Hooks::changer_log->msg(" : ");
	//	Hooks::changer_log->msg(cman->m_mapVMTKeyValues[i]->GetString());
	//}
	auto pVMTCache = new SourceEngine::KeyValues(/*XorStr*/("VMTCache"));

	if (!pVMTCache->LoadFromFile(reinterpret_cast<DWORD>(filesystem), target_file2, /*XorStr*/("MOD")))
	{
		return;
	}
	SourceEngine::KeyValues* pValue = pVMTCache->GetFirstValue();
	while (pValue)
	{
		const char* pszVMTToCache = pValue->GetString();
		if (pszVMTToCache && pszVMTToCache[0] != 0)
		{
			auto* pVMTKeyValues = new SourceEngine::KeyValues(/*XorStr*/("VertexLitGeneric"));
			const bool bVMTExists = pVMTKeyValues->LoadFromFile(reinterpret_cast<DWORD>(filesystem), pszVMTToCache, "MOD");
			if (bVMTExists)
			{
				cman->m_mapVMTKeyValues.Insert(pszVMTToCache, pVMTKeyValues);
				//Hooks::changer_log->msg("CustomMaterialManager: Cached KeyValues: ");
				//Hooks::changer_log->msg(pszVMTToCache);
				//Hooks::changer_log->msg("\n");
			}
			else
			{
				Hooks::changer_log->msg("Failed to load VMT: ");
				Hooks::changer_log->msg(pszVMTToCache);
				Hooks::changer_log->msg("\n");
			}
		}
		pValue = pValue->GetNextValue();
	}
	

	////HACKLY WAY BUT WORKS....
	////not found?
	//if (!filesystem->FileExists(target_file))
	//	return;
	//
	//auto filehandle = filesystem->Open(target_file, "r");
	//if(filehandle)
	//{
	//	const auto size = filesystem->Size(filehandle);
	//	std::unique_ptr<char[]> file_content = std::make_unique<char[]>(size);
	//	filesystem->Read(file_content.get(), size, filehandle);
	//	if(strstr(file_content.get(), /*XorStr*/("handwrap_leathery_fabric_beige_camo")))
	//	{
	//		//no need to merge - already patched
	//		filesystem->Close(filehandle);
	//		//delete[] file_content;
	//		return;
	//	}
	//	//free resources
	//	filesystem->Close(filehandle);
	//	//delete[] file_content;
	//}
	//else
	//{
	//	//cant open file
	//	return;
	//}
	//auto kv = std::make_unique<SourceEngine::KeyValues>(/*XorStr*/("VMTCache"));

	//if (!kv->LoadFromFile(reinterpret_cast<DWORD>(filesystem), target_file, /*XorStr*/("MOD")))
	//{
	//	return;
	//}

	//auto kv2 = std::make_unique<SourceEngine::KeyValues>(/*XorStr*/("VMTCache"));
	//if (!kv2->LoadFromFile(reinterpret_cast<DWORD>(filesystem), target_file2, /*XorStr*/("MOD")))
	//{
	//	return;
	//}

	//kv->MergeFrom(kv2.get(), SourceEngine::KeyValues::MERGE_KV_UPDATE);
	//kv->SaveToFile(reinterpret_cast<DWORD>(filesystem), target_file, /*XorStr*/("MOD"));

}

void InjectPaintKit(const char* paintkit_kv_file, SourceEngine::CRC32_t kv_crc32, SourceEngine::CRC32_t texture_crc32)
{
	//alloc memory for CPaintKit by g_pMemAlloc
	auto* memalloc = SourceEngine::interfaces.MemAlloc();
	auto* filesystem = SourceEngine::interfaces.FileSystem();
	auto* localize = SourceEngine::interfaces.LocalizeSystem();
	auto* itemschema = SourceEngine::interfaces.ItemSchema();
	void* paint_kit = memalloc->Alloc(sizeof(SourceEngine::CPaintKit));
	if (!paint_kit)
		return;
	//and setup properly
	//memset(paint_kit, 0, sizeof(SourceEngine::CPaintKit));
	//we can use constructor, provided by game, but if update broke our sig, it will be bad :(
	auto* pk = new(paint_kit) SourceEngine::CPaintKit();
	SourceEngine::CPaintKit* paint_kit2 = itemschema->GetPaintKitDefinitionByName(("default")); //using default paint_kit def for build new 
	SourceEngine::CPaintKit* paint_kit3 = itemschema->GetPaintKitDefinitionByName(/*XorStr*/("cu_awp_asimov")); //used to get sample localized string - 


	if(!filesystem->FileExists(paintkit_kv_file))
	{
		memalloc->Free(paint_kit);
		return;
	}
	FileHandle_t file = filesystem->Open(paintkit_kv_file, /*XorStr*/("r"));
	unsigned int filesize = filesystem->Size(file);

	void* p_output = memalloc->Alloc(filesize);
	const auto readed = filesystem->Read(p_output, filesize, file);
	filesystem->Close(file);

	if (kv_crc32 != 0xFACE14)
	{
		if(readed != filesize)
		{
			//file read has errors: not full read
			memalloc->Free(paint_kit);
			memalloc->Free(p_output);
			return;
		}
		const SourceEngine::CRC32_t checksum = SourceEngine::CRC32_ProcessSingleBuffer(p_output, filesize);

		if (checksum != kv_crc32)
		{
			//checksums not valid, exit
			memalloc->Free(paint_kit);
			memalloc->Free(p_output);
			return;
		}
		memalloc->Free(p_output);
	}
	SourceEngine::KeyValues* kv2 = new SourceEngine::KeyValues(/*XorStr*/("CEconItemSchema"));

	if (kv2->LoadFromFile(reinterpret_cast<DWORD>(filesystem), paintkit_kv_file, /*XorStr*/("MOD")))
	//if(!kv2->LoadFromBuffer(paintkit_kv_file, (char*)p_output, (DWORD*)SourceEngine::interfaces.FileSystem(), /*XorStr*/("MOD")))
	{
		pk->BInitFromKV(kv2, paint_kit2);
	}
	else
	{
		delete kv2;
		memalloc->Free(paint_kit);
		return;
	}
	if (texture_crc32 != 0xFACE14)
	{
		std::string path_to_pattern(/*XorStr*/("materials/models/weapons/customization/paints/"));
		switch (pk->style)
		{
		case 7:
			path_to_pattern.append(/*XorStr*/("custom/"));
			break;
		case 9:
			path_to_pattern.append(/*XorStr*/("gunsmith/"));
			break;
		default:
			delete kv2;
			memalloc->Free(paint_kit);
			return;
		}
		path_to_pattern.append(pk->pattern.Get());
		path_to_pattern.append(/*XorStr*/(".vtf"));
		
		file = filesystem->Open(path_to_pattern.c_str(), /*XorStr*/("rb"));
		if (!file)
		{
			//cant open file to check, exit
			delete kv2;
			memalloc->Free(paint_kit);
			return;
		}
		filesize = filesystem->Size(file);
		if (!filesize)
		{
			//file empty
			delete kv2;
			memalloc->Free(paint_kit);
			return;
		}
		p_output = memalloc->Alloc(8192);
		int readed_len;
		SourceEngine::CRC32_t checksum;
		SourceEngine::CRC32_Init(&checksum);
		while ((readed_len = filesystem->Read(p_output, 8192, file)))
		{
			SourceEngine::CRC32_ProcessBuffer(&checksum, p_output, readed_len);
		}
		filesystem->Close(file);
		SourceEngine::CRC32_Final(&checksum);

		if (checksum != texture_crc32)
		{
			//checksums not valid, exit
			delete kv2;
			memalloc->Free(paint_kit);
			memalloc->Free(p_output);
			return;
		}
		memalloc->Free(p_output);
	}


	const int desc_str_index = localize->FindIndex(paint_kit3->description_string.Get());
	const char* localize_filename = localize->GetFileNameByIndex(desc_str_index);

	if (!localize->addLocalizeString(pk->description_string, kv2->GetWString(/*XorStr*/("description_string_localize")), localize_filename))
	{
		//Warning("Localize string %s cant be add to game", pk->description_string.Get());
		delete kv2;
		memalloc->Free(paint_kit);
		return;
	}
	if (!localize->addLocalizeString(pk->description_tag, kv2->GetWString(/*XorStr*/("description_tag_localize")), localize_filename))
	{
		//Warning("Localize string %s cant be add to game", pk->description_string.Get());
		delete kv2;
		memalloc->Free(paint_kit);
		return;
	}

	//self-test
	const int desc_str_index2 = localize->FindIndex(pk->description_string.Get());
	//auto find_it = SourceEngine::interfaces.LocalizeSystem()->Find(pk->description_string.Get());
	if (desc_str_index2 == -1)
	{
		//Warning("Localize string %s was not found", pk->description_string.Get());
		delete kv2;
		memalloc->Free(paint_kit);
		return;
	}

	//overwrite global var 
	int* s_nCurrentPaintKitId = *reinterpret_cast<int**>(SourceEngine::offsets_cc_strike15_item_schema->s_nCurrentPaintKitId);
	*s_nCurrentPaintKitId = pk->PaintKitID;

	const int ret_ = itemschema->AddPaintKitDefinition(pk->PaintKitID, pk);

	if (ret_ == -1)
	{
		//Warning("PaintKit %n cant be add!", pk->PaintKitID);
		delete kv2;
		memalloc->Free(paint_kit);
		return;
	}
	//55 8B EC 83 EC 08 56 57 C7 +24 +25
	//self-test
	//SourceEngine::CPaintKit* check = itemschema->GetPaintKitDefinition(pk->PaintKitID);
	SourceEngine::CPaintKit* check = itemschema->GetPaintKitDefinitionByName(pk->name);

	if (check != nullptr)
	{
		//add skin to our skin db
		const int WeaponID = kv2->GetInt(/*XorStr*/("item_definition_index"), -1);
		if (WeaponID != -1)
		{
			WeaponSkin* weapon = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(WeaponID);
			if (weapon)
			{
				//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
				//custom.iPaintKitID = check->PaintKitID;
				//custom.szSkinName = localize->Find(check->description_tag.Get());

				weapon->vSkins.emplace_back(check);

				delete kv2;
				return;
			}
			KnifeSkin* knife = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(WeaponID);
			if (knife)
			{
				//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
				//custom.iPaintKitID = check->PaintKitID;
				//custom.szSkinName = localize->Find(check->description_tag.Get());
				knife->vSkins.emplace_back(check);

				delete kv2;
				return;

			}
			WearableSkin* glove = WearableSkin::GetWearableSkinByItemDefinitionIndex(WeaponID);
			if (glove)
			{
				//auto custom = SkinInfo_t(check->PaintKitID, localize->Find(check->description_tag.Get()), "");
				//custom.iPaintKitID = check->PaintKitID;
				//custom.szSkinName = localize->Find(check->description_tag.Get());

				glove->vSkins.emplace_back(check);

				delete kv2;
				return;
			}
			delete kv2;
			return;
		}
	}

	delete kv2;
}
namespace SourceEngine
{
	CCStrike15ItemSchema * CEconItem::getItemSchema()
	{
		return offsets_itemschema->GetItemSchema();
	}
	
	static uint32_t ComputeEconItemCustomDataOptimizedObjectAllocationSize(uint32_t numAttributes)
	{
		const uint32_t numBytesNeeded = sizeof(CEconItem::CustomDataOptimizedObject_t) + numAttributes * sizeof(CEconItem::attribute_t);
		return numBytesNeeded;
	}

	void CEconItem::CustomDataOptimizedObject_t::FreeObjectAndAttrMemory()
	{
		//attribute_t* pAttr = reinterpret_cast<CEconItem::attribute_t*>(this + 1);
		//const auto pAttrEnd = pAttr + m_numAttributes;
		//for (; pAttr < pAttrEnd; ++pAttr)
		//{
		//	CEconItem::FreeAttributeMemory(pAttr);
		//}
		free(this);
	}

	CEconItem::CustomDataOptimizedObject_t* CEconItem::CustomDataOptimizedObject_t::Alloc(uint32_t numAttributes)
	{
		const uint32_t numBytesNeeded = ComputeEconItemCustomDataOptimizedObjectAllocationSize(numAttributes);
		auto* const ptr = static_cast<CustomDataOptimizedObject_t*>(interfaces.MemAlloc()->Alloc(numBytesNeeded));
		ptr->m_equipInstanceSlot1 = INVALID_EQUIPPED_SLOT_BITPACKED;
		ptr->m_numAttributes = numAttributes;
		return ptr;
	}

	CEconItem::attribute_t* CEconItem::CustomDataOptimizedObject_t::AddAttribute(CustomDataOptimizedObject_t*& rptr)
	{
		const uint32_t numAttributesOld = rptr->m_numAttributes;

		const uint32_t numBytesOldAllocated = ComputeEconItemCustomDataOptimizedObjectAllocationSize(numAttributesOld);
		const uint32_t numBytesNeededNew = ComputeEconItemCustomDataOptimizedObjectAllocationSize(numAttributesOld + 1);

		if (numBytesNeededNew > numBytesOldAllocated)
		{
			CustomDataOptimizedObject_t* ptr = Alloc(numAttributesOld + 1);
			memcpy(ptr, rptr, sizeof(CEconItem::CustomDataOptimizedObject_t) + rptr->m_numAttributes * sizeof(CEconItem::attribute_t));
			free(rptr);
			rptr = ptr;
		}
		rptr->m_numAttributes = numAttributesOld + 1;
		return rptr->GetAttribute(numAttributesOld);
	}

	CEconItem::SetDynamicAttributeValue_t CEconItem::getSetDynamicAttributeValue()
	{
		return offsets_itemschema->SetDynamicAttributeValue;
	}

	CUtlVector<C_EconItemView*>* CPlayerInventory::GetInventoryItems()
	{
		return reinterpret_cast<CUtlVector<C_EconItemView*>*>(this + 0x2C);
	}

	bool CPlayerInventory::AddEconItem(CEconItem* item, bool bUpdateAckFile, bool bWriteAckFile, bool bCheckForNewItems)
	{
		CSharedObjectTypeCache* cache = GetBaseTypeCache();
		cache->AddObject(reinterpret_cast<CSharedObject*>(item));
		const auto ret = offsets_itemschema->AddEconItem(this, item, bUpdateAckFile, bWriteAckFile, bCheckForNewItems);

		if (ret)
		{
			auto* i = GetInventoryItemByItemID(*item->GetItemID());

			*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(i) + 0xA1) = true;
		}
#ifdef _DEBUG
		cache->Dump();
#endif
		return ret;
	}

	void CPlayerInventory::RemoveItem(const uint64_t ID)
	{
		offsets_itemschema->RemoveItem(this, ID);
	}

	void CPlayerInventory::RemoveItem(CEconItem* item)
	{
		RemoveItem(*item->GetItemID());
		GetBaseTypeCache()->RemoveObject(reinterpret_cast<CSharedObject*>(item));
	}

	void CPlayerInventory::ClearInventory()
	{
		auto* BaseTypeCache = this->GetBaseTypeCache();
		auto items = BaseTypeCache->GetEconItems();
		for (auto* item : items)
		{
			RemoveItem(*item->GetItemID());
			BaseTypeCache->RemoveObject(reinterpret_cast<CSharedObject*>(item));
		}
	}

		class CGCClient
		{

			virtual ~CGCClient() = 0;
			char pad[796];
		public:
			
			CUtlMap<SteamID_t, CUtlMap<SteamID_t, CSharedObjectCache*> > Caches;
		};

		//CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
		//{
		//	auto find_so_cache = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, unsigned __int64, unsigned __int64, bool)>(offsets_itemschema->find_so_cache);
		//	auto create_base_type_cache = reinterpret_cast<CSharedObjectTypeCache * (__thiscall*)(uintptr_t, int)>(offsets_itemschema->create_base_type_cache);

		//	const auto so_cache = find_so_cache(**reinterpret_cast<uintptr_t**>(offsets_itemschema->gc_cs + 0x2) + 0x70, *reinterpret_cast<unsigned __int64*>(this + 0x8), *reinterpret_cast<unsigned __int64*>(this + 0x10), false);

		//	return create_base_type_cache(so_cache, 1);
		//}
		//
		//
	CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
	{
		auto* client = reinterpret_cast<CGCClient*>(offsets_itemschema->g_GCClientSystem + 0x70);
		uintptr_t SOCache = 0;
		{
			auto* some_map_type = (CUtlRBTree<uint64_t>*)((uintptr_t)client + offsets_itemschema->so_cache_map);
			//const auto found_map_iterator = some_map_type->Find(this->m_OwnerID.ID());

			SOCache = *(uintptr_t*)((uintptr_t*)(reinterpret_cast<int>(some_map_type->m_pElements) + 24));
			//if(found_map_iterator != static_cast<unsigned short>(-1))
			//{
			//	//auto cache = some_map_type->Element(found_map_iterator);

			//	if (found_map_iterator <= some_map_type->m_Root /*&& v4 <= LOWORD(this->field_336)*/)
			//	{
			//		bool not_valid = found_map_iterator == -1;
			//		if (not_valid)
			//			not_valid = *(WORD*)(32 * found_map_iterator + *(int*)(&some_map_type->m_pElements)) == found_map_iterator;
			//		if (!not_valid)
			//			SOCache = *(uintptr_t*)(32 * found_map_iterator + (uintptr_t*)(reinterpret_cast<int>(some_map_type->m_pElements) + 24));
			//	}
			//}
		}
		return offsets_itemschema->GCSDK_CSharedObjectCache_CreateBaseTypeCache(SOCache, 1);
	}
	

	C_EconItemView* CSInventoryManager::FindOrCreateReferenceEconItem(int64_t ID)
	{
		return offsets_itemschema->FindOrCreateReferenceEconItem(this, ID);
	}

	CPlayerInventory* local_inventory= nullptr;
	CPlayerInventory* CSInventoryManager::GetLocalPlayerInventory()
	{
		if(!local_inventory)
		{
			local_inventory = CallVFunction<CPlayerInventory * (__thiscall*)(void*)>(this, 23)(this);
		}
		if(local_inventory)
			return local_inventory;
		//if(!local_inventory_offset)
		//{
		//	local_inventory_offset = 0xE0B8;//*reinterpret_cast<uintptr_t*>(Utils::FindSignature(
		//	//memory.client(), /*XorStr*/("8B 8B ? ? ? ? E8 ? ? ? ? 89 44 24 18")) + 0x2);
		//}
		//return *reinterpret_cast<CPlayerInventory**>(this + local_inventory_offset);
		return nullptr;
	}

	bool CSInventoryManager::EquipItemInLoadout(int team, int slot, unsigned long long itemID, bool bSwap)
	{
		return offsets_itemschema->EquipItemInLoadout(this, team, slot, itemID, bSwap);
	}

	CEconItem* CPlayerInventory::CreateEconItem()
	{	
		return offsets_itemschema->CreateSharedObjectSubclass_EconItem();
	}

	C_EconItemView* CPlayerInventory::GetInventoryItemByItemID(int64_t ID)
	{
		auto* econ = offsets_itemschema->GetInventoryItemByItemID(this, ID);

		if (!econ || !*reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(econ) + 0x214))
			return interfaces.InventoryManager()->FindOrCreateReferenceEconItem(ID);
		return econ;
	}

	uint32_t CPlayerInventory::GetSteamID() const
	{
		return static_cast<uint32_t>(m_OwnerID.ID());
		//return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	uint32_t* CEconItem::GetAccountID()
	{
		return &m_unAccountID;
		//return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x1C);
	}
	uint64_t* CEconItem::GetItemID()
	{
		//return &m_ulID;
		return reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	uint64_t* CEconItem::GetOriginalID()
	{
		return &m_ulOriginalID;
		//return reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	uint16_t* CEconItem::GetDefIndex()
	{
		return &m_unDefIndex;
		//return reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(this) + 0x24);
	}

	uint32_t* CEconItem::GetInventory()
	{
		return &m_unInventory;
		//return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x20);
	}

	unsigned char* CEconItem::GetFlags()
	{
		return &m_unFlags;
		//return reinterpret_cast<unsigned char*>(this + 0x30);
	}

	unsigned short* CEconItem::GetEconItemData()
	{
		constexpr size_t i = sizeof(unsigned short);
		return reinterpret_cast<unsigned short*>(reinterpret_cast<uintptr_t>(this) + 0x26);
	}

	//void CEconItem::UpdateEquippedState(unsigned int state)
	//{
	//	offsets_itemschema->UpdateEquippedState(this, state);
	//}

	bool CEconItem::IsEquipped() const
	{
		return m_pCustomDataOptimizedObject && (m_pCustomDataOptimizedObject->m_equipInstanceSlot1 != INVALID_EQUIPPED_SLOT_BITPACKED);
	}

	// --------------------------------------------------------------------------
	// Purpose:
	// --------------------------------------------------------------------------
	bool CEconItem::IsEquippedForClass(equipped_class_t unClass) const
	{
		return m_pCustomDataOptimizedObject && (m_pCustomDataOptimizedObject->m_equipInstanceSlot1 != INVALID_EQUIPPED_SLOT_BITPACKED) &&
			(	// CS:GO optimized test - equip class is 0 or 2/3
				(unClass == m_pCustomDataOptimizedObject->m_equipInstanceClass1) ||
				((unClass == 3) && (m_pCustomDataOptimizedObject->m_equipInstanceClass1 == 2) && (m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit != 0))
				);
	}

	// --------------------------------------------------------------------------
	// Purpose:
	// --------------------------------------------------------------------------
	equipped_slot_t CEconItem::GetEquippedPositionForClass(equipped_class_t unClass) const
	{
		return IsEquippedForClass(unClass) ? m_pCustomDataOptimizedObject->m_equipInstanceSlot1 : INVALID_EQUIPPED_SLOT;
	}

	// --------------------------------------------------------------------------
	// Purpose:
	// --------------------------------------------------------------------------
	void CEconItem::UpdateEquippedState(EquippedInstance_t equipInstance)
	{
		// If it's invalid we need to remove it
		if (equipInstance.m_unEquippedSlot == INVALID_EQUIPPED_SLOT)
		{
			if (m_pCustomDataOptimizedObject && (m_pCustomDataOptimizedObject->m_equipInstanceSlot1 != INVALID_EQUIPPED_SLOT_BITPACKED))
			{
				if (equipInstance.m_unEquippedClass == m_pCustomDataOptimizedObject->m_equipInstanceClass1)
				{
					if (m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit)
					{
						// Move the 2nd class down
						if (equipInstance.m_unEquippedClass == 2)
						{	// leave it equipped for class 3
							m_pCustomDataOptimizedObject->m_equipInstanceClass1 = 3;
							m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
							return;
						}
						//else
						//{
						//	Assert(equipInstance.m_unEquippedClass == 2); // weird case, claims to be equipped for both classes, but first class is invalid!
						//}
					}

					// Fully unequip
					m_pCustomDataOptimizedObject->m_equipInstanceSlot1 = INVALID_EQUIPPED_SLOT_BITPACKED;
					if (!m_pCustomDataOptimizedObject->m_numAttributes)
					{
						m_pCustomDataOptimizedObject->FreeObjectAndAttrMemory();
						m_pCustomDataOptimizedObject = nullptr;
					}
					return;
				}
				if ((equipInstance.m_unEquippedClass == 3) && (m_pCustomDataOptimizedObject->m_equipInstanceClass1 == 2) && (m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit != 0))
				{
					// was equipped for both, unequipping 3rd class
					m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
					return;
				}
			}
			// item was not equipped to begin with
			return;
		}

		// If this item is already equipped
		if (m_pCustomDataOptimizedObject && (m_pCustomDataOptimizedObject->m_equipInstanceSlot1 != INVALID_EQUIPPED_SLOT_BITPACKED))
		{
			m_pCustomDataOptimizedObject->m_equipInstanceSlot1 = equipInstance.m_unEquippedSlot;
			switch (equipInstance.m_unEquippedClass)
			{
			case 0: // non-team item
				m_pCustomDataOptimizedObject->m_equipInstanceClass1 = 0;
				m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
				return;
			case 2:
				if (m_pCustomDataOptimizedObject->m_equipInstanceClass1 == 3)
				{
					m_pCustomDataOptimizedObject->m_equipInstanceClass1 = 2;
					m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 1;
				}
				else
				{
					m_pCustomDataOptimizedObject->m_equipInstanceClass1 = 2;
					m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
				}
				return;
			case 3:
				if (m_pCustomDataOptimizedObject->m_equipInstanceClass1 == 2)
				{
					m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 1;
				}
				else
				{
					m_pCustomDataOptimizedObject->m_equipInstanceClass1 = 3;
					m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
				}
				return;
			default:
				assert(false);
				return;
			}
		}

		// Otherwise this item has not been equipped yet
		if (!m_pCustomDataOptimizedObject)
		{
			m_pCustomDataOptimizedObject = CustomDataOptimizedObject_t::Alloc(0);
		}

		m_pCustomDataOptimizedObject->m_equipInstanceSlot1 = equipInstance.m_unEquippedSlot;
		m_pCustomDataOptimizedObject->m_equipInstanceClass1 = equipInstance.m_unEquippedClass;
		m_pCustomDataOptimizedObject->m_equipInstanceClass2Bit = 0;
	}

	void CEconItem::AddSticker(int index, int kit, float wear, float scale, float rotation)
	{
		SetAttributeValue<int>(113 + 4 * index, kit);
		SetAttributeValue<float>(114 + 4 * index, wear);
		SetAttributeValue<float>(115 + 4 * index, scale);
		SetAttributeValue<float>(116 + 4 * index, rotation);
	}

	void CEconItem::SetStatTrak(const int val)
	{
		SetAttributeValue<int>(80, val);
		SetAttributeValue<int>(81, 0);
		SetQuality(ITEM_QUALITY_STRANGE);
	}

	void CEconItem::SetPaintKit(const float kit)
	{
		SetAttributeValue<float>(6, kit);
	}

	void CEconItem::SetPaintSeed(const float seed)
	{
		SetAttributeValue<float>(7, seed);
	}

	void CEconItem::SetPaintWear(const float wear)
	{
		SetAttributeValue<float>(8, wear);
	}

	void CEconItem::SetQuality(ItemQuality quality)
	{
		//const auto data = *GetEconItemData();
		//*GetEconItemData() = data ^ ((data ^ 32U * quality) & 0x1E0U);
		EconItemData = EconItemData ^ ((EconItemData ^ 32U * quality) & 0x1E0U);
	}

	void CEconItem::SetRarity(ItemRarity rarity)
	{
		//const auto data = *GetEconItemData();
		//*GetEconItemData() = ((data ^ (rarity << 11U)) & 0x7800U) ^ data;
		EconItemData = ((EconItemData ^ (rarity << 11U)) & 0x7800U) ^ EconItemData;
	}

	void CEconItem::SetOrigin(int origin)
	{
		//const auto data = *GetEconItemData();
		//*GetEconItemData() = data ^ ((static_cast<unsigned int8_t>(data) ^ static_cast<unsigned int8_t>(origin)) & 0x1F);
		EconItemData = EconItemData ^ ((static_cast<uint8_t>(EconItemData) ^ static_cast<uint8_t>(origin)) & 0x1F);
	}

	void CEconItem::SetLevel(int level)
	{
		//const auto data = *GetEconItemData();
		//*GetEconItemData() = data ^ ((data ^ (level << 9U)) & 0x600U);
		EconItemData = EconItemData ^ ((EconItemData ^ (level << 9U)) & 0x600U);
	}

	void CEconItem::SetInUse(bool in_use)
	{
		//const auto data = *GetEconItemData();
		//*GetEconItemData() = (data & 0x7FFFU) | (in_use << 15U);
		EconItemData = (EconItemData & 0x7FFFU) | (in_use << 15U);
	}

	void CEconItem::SetCustomName(const char* name)
	{

		offsets_itemschema->SetCustomName(this, name);
	}
	
	void CEconItem::SetCustomDesc(const char* name)
	{
		offsets_itemschema->SetCustomDesc(this, name);
	}
	
	std::vector<CEconItem*> CSharedObjectTypeCache::GetEconItems()
	{
		std::vector<CEconItem*> ret;

		for (int i = 0; i < this->m_vecObjects.Count(); ++i)
			ret.push_back(reinterpret_cast<CEconItem* const&>(this->m_vecObjects[i]));

		return ret;
	}
}