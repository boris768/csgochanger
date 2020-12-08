// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Skins.h"
//#include "KeyValues.h"
#include "XorStr.hpp"
#include "CSGOStructs.hpp"
#include <iostream>
#include "Utils.hpp"
#include <filesystem>
#include "Hooks.hpp"

std::vector<WeaponSkin> g_vWeaponSkins;
std::vector<WearableSkin> g_vWearablesSkins;
std::vector<PlayerInfo_t> g_v_T_TeamSkins;
std::vector<PlayerInfo_t> g_v_CT_TeamSkins;
std::vector<PlayerInfo_t> g_v_All_TeamSkins;
std::vector<KnifeSkin> g_vKnifesSkins;
vecStickerInfo g_vStickers;
vecStickerInfo g_vSprays;


#define ITEMS_GAME XorStr("scripts/items/items_game.txt")

#define RESOURCE XorStr("/resource/")
#define ENGLISH XorStr("_english.txt")
#define TOKENS XorStr("Tokens")
#define ITEMS XorStr("items")
#define PREFABS XorStr("prefabs")
#define NAME XorStr("name")
#define PREFAB XorStr("prefab")
#define ITEM_NAME XorStr("item_name")
#define MODEL_WORLD XorStr("model_world")
#define MODEL_PLAYER XorStr("model_player")
#define _PREFAB XorStr("_prefab")
#define ITEM_SUB_POSITION XorStr("item_sub_position")
#define ALTERNATE_ICONS2 XorStr("alternate_icons2")
#define WEAPON_ICONS XorStr("weapon_icons")
#define ITEM_SETS XorStr("item_sets")
#define PAINT_KITS XorStr("paint_kits")
#define DESCRIPTION_TAG XorStr("description_tag")
#define ICON_PATH XorStr("icon_path")
#define MEDIUM XorStr("_medium")
#define CU_M4A1_HOWLING XorStr("cu_m4a1_howling")

#define MELEE_UNUSUAL XorStr("melee_unusual")
#define NINEk1 9001

#define WEAPON XorStr("weapon_")
#define MDL XorStr(".mdl")
#define DROPPED XorStr("_dropped.mdl")
#define GRENADE XorStr("grenade")

#define DEFAULT XorStr("default")
#define STICKERS XorStr("stickers")
#define STICKER_KITS XorStr("sticker_kits")
#define TOURNAMENT_PLAYER_ID XorStr("tournament_player_id")
#define STICKER_MATERIAL XorStr("sticker_material")


#define FOR_EACH_SUBKEY( kvRoot, kvSubKey ) \
	for ( SourceEngine::KeyValues* (kvSubKey) = (kvRoot)->GetFirstSubKey(); kvSubKey; (kvSubKey) = (kvSubKey)->GetNextKey() )

WeaponInfo_t::WeaponInfo_t(int itemdefinitionindex, int maxstickers, std::string stickermodel,
                           std::string shortweaponname, std::wstring weaponname, std::string viewmodel,
                           std::string worldmodel,
                           std::string droppedmodel, SourceEngine::CCStrike15ItemDefinition* __game_item_definition,
                           bool isknife):
	iItemDefinitionIndex(itemdefinitionindex),
	iMaxStickers(maxstickers),
	szStickerModel(std::move(stickermodel)),
	szShortWeaponName(std::move(shortweaponname)),
	szWeaponName(std::move(weaponname)),
	szViewModel(std::move(viewmodel)),
	szWorldModel(std::move(worldmodel)),
	szDroppedModel(std::move(droppedmodel)),
	game_item_definition(__game_item_definition),
	bIsKnife(isknife)
{
	const auto i = ImGui::utf8_encode(szWeaponName.c_str());
	szWeaponNameAsU8 = i;
	delete i;
}

WearableInfo_t::WearableInfo_t(short itemdefinitionindex, std::string shortwearablename, std::wstring wearablename,
                               std::string viewmodel, std::string worldmodel,
                               SourceEngine::CCStrike15ItemDefinition* _game_item_definition):
	iItemDefinitionIndex(itemdefinitionindex),
	szShortWeaponName(std::move(shortwearablename)),
	szWeaponName(std::move(wearablename)),
	szViewModel(std::move(viewmodel)),
	szWorldModel(std::move(worldmodel)),
	game_item_definition(_game_item_definition)
{
	const auto i = ImGui::utf8_encode(szWeaponName.c_str());
	szWeaponNameAsU8 = i;
	delete i;
}

PlayerInfo_t::PlayerInfo_t(short itemdefinitionindex, std::string _model_player, std::string _short_name,
                           std::wstring _item_name): model_player(std::move(_model_player)),
                                                     short_name(std::move(_short_name)),
                                                     item_name(std::move(_item_name)),
                                                     iItemDefinitionIndex(itemdefinitionindex)
{
	const auto i = ImGui::utf8_encode(item_name.c_str());
	item_name_as_u8 = i;
	delete i;
}

SkinInfo_t::SkinInfo_t(int paintkitid, std::wstring skinname, std::string shortskinname,
                       SourceEngine::CPaintKit* paintkit_def):
	iPaintKitID(paintkitid),
	szShortSkinName(std::move(shortskinname)),
	szSkinName(std::move(skinname)),
	m_pPaintKitDefinition(paintkit_def)
{
	const auto i = ImGui::utf8_encode(szSkinName.c_str());
	szSkinNameAsU8 = i;
	delete i;
}

SkinInfo_t::SkinInfo_t(SourceEngine::CPaintKit* paintkit_def):
	iPaintKitID(paintkit_def->PaintKitID),
	szShortSkinName(paintkit_def->name.Get()),
	m_pPaintKitDefinition(paintkit_def)
{
	szSkinName = SourceEngine::interfaces.LocalizeSystem()->FindSafe2(paintkit_def->description_tag);
	const auto i = ImGui::utf8_encode(szSkinName.c_str());
	szSkinNameAsU8 = i;
	delete i;
}

StickerInfo_t::StickerInfo_t(size_t stickerid, std::wstring stickername, std::string stickermaterial, bool _is_patch,
                             bool _is_spray):
	iStickerKitID(stickerid),
	szStickerName(std::move(stickername)),
	szStickerMaterial(std::move(stickermaterial)),
	is_patch(_is_patch),
	is_spray(_is_spray)
{
	const auto i = ImGui::utf8_encode(szStickerName.c_str());
	szStickerNameAsU8.append(i);
	delete i;
}

WeaponSkin::WeaponSkin() : game_item_definition(nullptr)
{
	vSkins.reserve(20);
	const auto i = ImGui::utf8_encode(szWeaponName.c_str());
	szWeaponNameAsU8 = i;
	delete i;
}

WeaponSkin::WeaponSkin(const mapWearableSkinInfo::value_type& in)
{
	this->iItemDefinitionIndex = static_cast<size_t>(in.first.iItemDefinitionIndex);
	this->iMaxStickers = 0;
	this->szWeaponName = in.first.szWeaponName;
	vSkins.reserve(in.second.size() + 1);
	vSkins.emplace_back(0, L"default", "default");
	for (auto& Skin : in.second)
	{
		this->vSkins.emplace_back(Skin);
	}
	game_item_definition = in.first.game_item_definition;
	const auto i = ImGui::utf8_encode(szWeaponName.c_str());
	szWeaponNameAsU8 = i;
	delete i;
}

bool WeaponSkin::init_weapon(const mapSkinInfo::value_type& in)
{
	this->iItemDefinitionIndex = static_cast<short>(in.first.iItemDefinitionIndex);
	this->iMaxStickers = static_cast<size_t>(in.first.iMaxStickers);
	this->szWeaponName = in.first.szWeaponName;
	this->szStickerModel = in.first.szStickerModel;

	vSkins.reserve(in.second.size() + 1);
	SkinInfo_t default_skin(0, L"default", "default", nullptr);
	vSkins.emplace_back(default_skin);

	const auto end = in.second.end();
	for (auto it = in.second.begin(); it != end; ++it)
	{
		this->vSkins.emplace_back(*it);
	}
	this->game_item_definition = in.first.game_item_definition;
	const auto i = ImGui::utf8_encode(szWeaponName.c_str());
	szWeaponNameAsU8 = i;
	delete i;
	return true;
}


void Skin_loader::Dump()
{
	std::cout << std::endl;
	std::cout << std::endl;
	for(auto& glove: g_vWearablesSkins)
	{
		for(auto& glove_skin : glove.vSkins)
		{
			std::cout << glove_skin.iPaintKitID;
			std::cout << " : ";
			std::cout << glove.szShortWeaponName;
			std::cout << " : ";
			std::cout << glove_skin.szShortSkinName;
			std::cout << std::endl;
		}
	}
}

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

bool Skin_loader::Load()
{
	auto itemschema = SourceEngine::interfaces.ItemSchema();
	auto localize = SourceEngine::interfaces.LocalizeSystem();

	const int items_count = itemschema->GetItemDefinitionCount();
	const SourceEngine::mass* items_mass = itemschema->GetItemDefinitions();
	std::vector<SourceEngine::CCStrike15ItemDefinition*> defs;

	for (int i = 0; i < items_count; ++i)
	{
		if (items_mass[i].item)
		{
			auto def = items_mass[i].item;

			if (def->id == SourceEngine::EItemDefinitionIndex::weapon_knife_t ||
				def->id == SourceEngine::EItemDefinitionIndex::weapon_knife_ct ||
				def->id == SourceEngine::EItemDefinitionIndex::weapon_knifegg ||
				def->id == SourceEngine::EItemDefinitionIndex::glove_ct_side||
				def->id == SourceEngine::EItemDefinitionIndex::glove_t_side)
				continue;

			if (strstr(def->m_pszItemClassname, XorStr("knife")))
			{
				vec_weapon_info.emplace_back
				(
					def->id,
					0,
					"",
					def->m_pszDefinitionName,
					localize->FindSafe2(def->item_name),
					def->m_pszBaseDisplayModel,
					def->model_world,
					def->model_dropped,
					def,
					true
				);
				continue;
			}
			if (strstr(def->item_type_name, XorStr("CustomPlayer")))
			{
				if (def->m_pszBaseDisplayModel)
				{
					//PlayerInfo_t(short itemdefinitionindex, std::string _model_player, std::string _short_name, std::wstring _item_name)
					if (strstr(def->item_name, XorStr("CustomPlayer_ct")))
					{
						auto mdl = std::filesystem::path(def->m_pszBaseDisplayModel).filename().string();
						auto ext_enter = mdl.find(MDL);
						mdl.erase(ext_enter, 4);
						vec_ct_player_info.emplace_back
						(
							def->id,
							mdl,
							def->item_name,
							localize->FindSafe2(def->item_name)
						);
					}
					if (strstr(def->item_name, XorStr("CustomPlayer_t")))
					{
						auto mdl = std::filesystem::path(def->m_pszBaseDisplayModel).filename().string();
						auto ext_enter = mdl.find(MDL);
						mdl.erase(ext_enter, 4);
						vec_t_player_info.emplace_back
						(
							def->id,
							mdl,
							def->item_name,
							localize->FindSafe2(def->item_name)
						);
					}
				}
				continue;
			}
			if (strstr(def->m_pszItemClassname, XorStr("wearable")))
			{
				if (def->model_world)
				{
					vec_wearable_info.emplace_back
					(
						def->id,
						def->m_pszDefinitionName,
						localize->FindSafe2(def->item_name),
						def->m_pszBaseDisplayModel,
						def->model_world,
						def
					);
				}
				//else
				//{
				//	vec_wearable_info.emplace_back
				//	(
				//		def->id,
				//		def->name,
				//		localize->FindSafe2(def->item_name),
				//		def->model_player,
				//		""
				//	);
				//}
				continue;
			}
			if (strstr(def->m_pszItemClassname, XorStr("weapon")))
			{
				const SourceEngine::CUtlVector<SourceEngine::StickerData_t_source>* sticker = &def->stickers;
				auto max = sticker->Count();
				if (!max)
					continue;

				std::string world_model(def->model_world);

				const size_t find_result = world_model.find(MDL);
				auto sticker_decals = world_model.substr(17, find_result - 17); //TODO: recheck valid

				vec_weapon_info.emplace_back
				(
					def->id,
					max,
					sticker_decals,
					def->m_pszDefinitionName,
					localize->FindSafe2(def->item_name),
					def->m_pszBaseDisplayModel,
					def->model_world,
					def->model_dropped,
					def,
					false
				);
			}
		}
	}

	auto paint_kits = itemschema->GetPaintKits();
	for (int(it) = 0; (it) < (paint_kits)->MaxElement(); ++(it))
	{
		if (!(paint_kits)->IsValidIndex(it))
			continue;
		auto pk = paint_kits->Element(it);
		if(pk->PaintKitID == NINEk1)
			continue;
		pks.insert_or_assign(pk->PaintKitID, pk);
	}
	
	std::vector < SourceEngine::AlternateIconData_t> alternate_icon_data;
	const auto alternative_icons_map_head = reinterpret_cast<Head_t<int, SourceEngine::AlternateIconData_t>*>(itemschema->GetAlternativeIcons());
	alternate_icon_data.reserve(alternative_icons_map_head->last_element);
	for (auto i = 0; i <= alternative_icons_map_head->last_element; ++i)
	{
		SourceEngine::AlternateIconData_t icon_data = alternative_icons_map_head->memory[i].value;
		//if (strstr(icon_data.icon_path.Get(), XorStr("_medium")))
		if(icon_data.icon_path.Get()[icon_data.icon_path.Length() - 6] != '_') //_medium?
		{
			alternate_icon_data.emplace_back(icon_data);
		}
	}
	
	for(auto p :pks)
	{
		auto PaintKitIcon = p.second->name.Get() + std::string(XorStr("_medium"));
		
		for (const auto& AlternateIcon : alternate_icon_data)
		{
			std::string_view IconPath = AlternateIcon.icon_path.Get();
			/*if (IconPath[IconPath.length() - 7] != '_')
				continue;*/

			auto Find = IconPath.find(PaintKitIcon); // Or just find PaintKit->name and see if it ends with _medium?
			if (Find == std::string_view::npos)
				continue;
			auto item_name_from_icon = IconPath.substr(23, Find - 24);

			if (p.first >= 10000)
			{
				for (auto& i : vec_wearable_info)
				{
					if (i.szShortWeaponName == item_name_from_icon)
					{
						_wearbleskininfo[i].emplace_back(p.second);
						break;
					}
				}
			}
			else
			{
				for (auto& i : vec_weapon_info)
				{
					if (i.szShortWeaponName == item_name_from_icon)
					{
						_skininfo[i].emplace_back(p.second);
						break;
					}
				}
			}
		}
	}
	//kv_values = new SourceEngine::KeyValues(XorStr("CEconItemScrema"));
	//if (kv_values->LoadFromFile(reinterpret_cast<DWORD>(SourceEngine::interfaces.FileSystem()), XorStr("scripts/items/items_game.txt"), XorStr("MOD")))
	//{
	//	std::vector<SourceEngine::KeyValues*> kvPaintKitsRarity;

	//	FOR_EACH_SUBKEY(kv_values, kvSubKey)
	//	{
	//		const auto str_name = kvSubKey->GetName();
	//		if (!strcmp(str_name, XorStr("paint_kits_rarity")))
	//		{
	//			kvPaintKitsRarity.emplace_back(kvSubKey);
	//		}
	//	}
	//	for (auto kv_pkr : kvPaintKitsRarity)
	//	{
	//		FOR_EACH_SUBKEY(kv_pkr, kv)
	//		{
	//			auto name = kv->GetName();

	//			for (auto p : pks)
	//			{
	//				if(p.second->name == name)
	//				{
	//					auto rarity_name = kv->GetString_hack( "default");
	//					int rarity;
	//					switch (rarity_name[0])
	//					{
	//					case 'c':
	//						rarity = 1;
	//						break;
	//					case 'u':
	//						rarity = 2;
	//						break;
	//					case 'r':
	//						rarity = 3;
	//						break;
	//					case 'm':
	//						rarity = 4;
	//						break;
	//					case 'l':
	//						rarity = 5;
	//						break;
	//					case 'a':
	//						rarity = 6;
	//						break;
	//					case 'i':
	//						rarity = 7;
	//						break;
	//					case 'd':
	//					default:
	//						rarity = 0;
	//						break;
	//					}
	//					p.second->rarity = rarity;
	//					Hooks::changer_log->msg(name);
	//					Hooks::changer_log->msg(" : ");
	//					Hooks::changer_log->msg(rarity_name);
	//					Hooks::changer_log->msg(" : ");
	//					Hooks::changer_log->msg(std::to_string(rarity).c_str());
	//					Hooks::changer_log->msg("\n");
	//				}
	//			}
	//		}
	//	}
	//}
	//delete kv_values;
	
	//thanks Namazso!
	{
		const auto sticker_sig = Utils::FindSignature(SourceEngine::memory.client(), XorStr("53 8D 48 04 E8 ? ? ? ? 8B 4D 10")) + 4;

		// Skip the opcode, read rel32 address
		const auto get_sticker_kit_definition_offset = *reinterpret_cast<std::intptr_t*>(sticker_sig + 1);

		// Add the offset to the end of the instruction
		const auto get_sticker_kit_definition_fn = reinterpret_cast<SourceEngine::CPaintKit * (__thiscall*)(SourceEngine::CCStrike15ItemSchema*, int)>(sticker_sig + 5 + get_sticker_kit_definition_offset);

		// The last offset is head_element, we need that

		//	push    ebp
		//	mov     ebp, esp
		//	push    ebx
		//	push    esi
		//	push    edi
		//	mov     edi, ecx
		//	mov     eax, [edi + 2BCh]

		// Skip instructions, skip opcode, read offset
		const auto start_element_offset = *reinterpret_cast<intptr_t*>(std::uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		const auto head_offset = start_element_offset - 12;

		const auto map_head = reinterpret_cast<Head_t<int, SourceEngine::CStickerKit*>*>(std::uintptr_t(itemschema) + head_offset);

		_stickerinfo.reserve(map_head->last_element);
		for (auto i = 1; i <= map_head->last_element; ++i)
		{
			const SourceEngine::CStickerKit* sticker_kit = map_head->memory[i].value;

			char sticker_name_if_valve_fucked_up_their_translations[64];

			auto sticker_name_ptr = sticker_kit->sItemName.Get() + 1;

			if(sticker_name_ptr[1] == 'p' || strstr(sticker_kit->sName, XorStr("graffiti"))) //if spray
			{
				const auto wide_name = localize->FindSafe2(sticker_name_ptr);
				_sprayinfo.emplace_back(static_cast<unsigned int>(sticker_kit->nID), wide_name, sticker_kit->sMaterialPath.Get(), false, true);
				
				//Why not set it as sticker? becouse there is no vmt files! create it or use something default?
				//_stickerinfo.emplace_back(static_cast<unsigned int>(sticker_kit->id), wide_name, sticker_kit->sticker_material.Get(), false, true);
				continue;
			}
			
			if (strstr(sticker_name_ptr, XorStr("StickerKit_dhw2014_dignitas")))
			{
				strcpy_s(sticker_name_if_valve_fucked_up_their_translations, XorStr("StickerKit_dhw2014_teamdignitas"));
				strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
				sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
			}

			const auto wide_name = localize->FindSafe2(sticker_name_ptr);

			if (strstr(sticker_name_ptr, XorStr("PatchKit")))
			{
				_stickerinfo.emplace_back(static_cast<unsigned int>(sticker_kit->nID), wide_name, sticker_kit->sMaterialPath.Get(), true);
			}
			else
			{
				_stickerinfo.emplace_back(static_cast<unsigned int>(sticker_kit->nID), wide_name, sticker_kit->sMaterialPath.Get());
			}
		}

		sort(_stickerinfo.begin(), _stickerinfo.end(), StickerCompare::compare);
		_stickerinfo.emplace(_stickerinfo.begin(), 0, L"default", "");
		
		sort(_sprayinfo.begin(), _sprayinfo.end(), StickerCompare::compare);
		_sprayinfo.emplace(_sprayinfo.begin(), 0, L"default", "");
	}

	//DEPRECATED Old fileparser
	//kv_values = new SourceEngine::KeyValues(XorStr("CEconItemScrema"));

	//if (kv_values->LoadFromFile(reinterpret_cast<DWORD>(SourceEngine::interfaces.FileSystem()), XorStr("scripts/items/items_game.txt"), XorStr("MOD")))
	//{
	//	std::vector<SourceEngine::KeyValues*> kvItemSets;
	//	std::vector<SourceEngine::KeyValues*> kvPaintKits;
	//	std::vector<SourceEngine::KeyValues*> kvStickerKits;

	//	FOR_EACH_SUBKEY(kv_values, kvSubKey)
	//	{
	//		const auto str_name = kvSubKey->GetName();
	//		if (!strcmp(str_name, XorStr("item_sets")))
	//		{
	//			kvItemSets.emplace_back(kvSubKey);
	//			continue;
	//		}
	//		if (!strcmp(str_name, XorStr("paint_kits")))
	//		{
	//			kvPaintKits.emplace_back(kvSubKey);
	//		}
	//		//if (!strcmp(str_name, XorStr("sticker_kits")))
	//		//{
	//		//	kvStickerKits.emplace_back(kvSubKey);
	//		//}
	//	}

	//	//LoadItems(kv_values->FindKey(ITEMS));
	//	//LoadStickerKits(kvStickerKits);
	//	LoadPaintKits(kvPaintKits, kvItemSets);

	//}
	SortData();

	return true;
}
	
//
//void Skin_loader::LoadItems(const SourceEngine::KeyValues* pItems)
//{
//	auto localize = SourceEngine::interfaces.LocalizeSystem();
//	FOR_EACH_SUBKEY(pItems, pItem)
//	{
//		std::string Name = pItem->GetString(NAME);
//		std::string Prefab = pItem->GetString(PREFAB);
//
//		if (Prefab == XorStr("hands_paintable"))
//		{
//			vec_wearable_info.emplace_back(
//				static_cast<unsigned int>(atoi(pItem->GetName())),
//				Name,
//				localize->FindSafe2(pItem->GetString(ITEM_NAME)),
//				pItem->GetString(XorStr("model_player")),
//				pItem->GetString(XorStr("model_world"))
//			);
//		}
//
//		else if (Prefab == MELEE_UNUSUAL)
//		{
//			std::string szWorldModel = pItem->GetString(MODEL_WORLD);
//			std::string szDroppedModel = szWorldModel;
//
//			const size_t FindResult = szDroppedModel.find(MDL);
//			if (FindResult != std::string::npos)
//				szDroppedModel.replace(FindResult, 4, DROPPED);
//
//			vec_weapon_info.emplace_back(
//				atoi(pItem->GetName()),
//				0,
//				"",
//				Name,
//				localize->FindSafe2(pItem->GetString(XorStr("item_name"))),
//				pItem->GetString(XorStr("model_player")),
//				szWorldModel,
//				szDroppedModel,
//				true
//			);
//		}
//		else
//		{
//			if (Prefab != Name + XorStr("_prefab"))
//				continue;
//
//			const char* pszItemSubPosition = pItem->GetString(XorStr("item_sub_position"));
//			if (!pszItemSubPosition[0])
//				continue;
//
//			if (std::string(pszItemSubPosition).find(XorStr("grenade")) != std::string::npos)
//				continue;
//
//			SourceEngine::KeyValues* pPrefab = kv_values->FindKey(XorStr("prefabs"))->FindKey(Prefab.c_str());
//
//			std::string szWorldModel = pPrefab->GetString(XorStr("model_world"));
//			std::string szDroppedModel = szWorldModel;
//
//			const size_t FindResult = szDroppedModel.find(XorStr(".mdl"));
//			if (FindResult != std::string::npos)
//				szDroppedModel.replace(FindResult, 4, XorStr("_dropped.mdl"));
//
//			const auto pStickers = pPrefab->FindKey(XorStr("stickers"));
//
//			vec_weapon_info.emplace_back(
//				atoi(pItem->GetName()),
//				pStickers ? static_cast<size_t>(pStickers->iCount()) : 0,
//				szWorldModel.substr(17, FindResult - 17),
//				Name,
//				localize->FindSafe2(pPrefab->GetString(XorStr("item_name"))),
//				pPrefab->GetString(XorStr("model_player")),
//				szWorldModel,
//				szDroppedModel,
//				false
//			);
//		}
//	}
//}

//void Skin_loader::LoadStickerKits(const std::vector<SourceEngine::KeyValues*>& kvStickerKits)
//{
//	_stickerinfo.reserve(4000);
//	_sprayinfo.reserve(150);
//
//	const char* sticker_material = XorStr("sticker_material");
//	const char* item_name = XorStr("item_name");
//	const char* name = XorStr("name");
//	constexpr char* default_ = ("default");
//	const char* spray_ = XorStr("spray_");
//
//	for (auto pStickerKits : kvStickerKits)
//	{
//		FOR_EACH_SUBKEY(pStickerKits, pStickerKit)
//		{
//			const char* str_name = pStickerKit->GetString(name);
//			if (!strcmp(str_name, default_))
//				continue;
//
//			if (!strncmp(str_name, spray_, 6))
//			{
//				_sprayinfo.emplace_back(
//					static_cast<unsigned int>(atoi(pStickerKit->GetName())),
//					SourceEngine::interfaces.LocalizeSystem()->FindSafe2(pStickerKit->GetString(item_name)),
//					pStickerKit->GetString(sticker_material)
//				);
//				continue;
//			}
//
//			_stickerinfo.emplace_back(
//				static_cast<unsigned int>(atoi(pStickerKit->GetName())),
//				SourceEngine::interfaces.LocalizeSystem()->FindSafe2(pStickerKit->GetString(item_name)),
//				pStickerKit->GetString(sticker_material)
//			);
//		}
//	}
//
//	sort(_stickerinfo.begin(), _stickerinfo.end(), StickerCompare::compare);
//	_stickerinfo.emplace(_stickerinfo.begin(), 0, L"default", "");
//	sort(_sprayinfo.begin(), _sprayinfo.end(), StickerCompare::compare);
//	_sprayinfo.emplace(_sprayinfo.begin(), 0, L"default", "");
//}
//
//void Skin_loader::LoadPaintKits(const std::vector<SourceEngine::KeyValues*>& kvPaintKits,
//                                const std::vector<SourceEngine::KeyValues*>& kvItemSets)
//{
//	const SourceEngine::KeyValues* pWeaponIcons = kv_values->FindKey(XorStr("alternate_icons2"))->FindKey(XorStr("weapon_icons"));
//	auto itemschema = SourceEngine::interfaces.ItemSchema();
//
//	for (auto pPaintKits : kvPaintKits)
//	{
//		FOR_EACH_SUBKEY(pPaintKits, pPaintKit)
//		{
//			const int PaintKitID = atoi(pPaintKit->GetName());
//			if (PaintKitID == NINEk1) // workshop_default
//				continue;
//
//			std::wstring PaintTranslatedKitName = SourceEngine::interfaces.LocalizeSystem()->FindSafe2(
//				pPaintKit->GetString(XorStr("description_tag")));
//			if (PaintTranslatedKitName == L"-")
//				continue;
//
//			std::string PaintKitName = pPaintKit->GetString(XorStr("name"));
//
//			if (PaintKitID >= 10006)
//			{
//				//auto paint_kit_definition = itemschema->GetPaintKitDefinition(PaintKitID);
//				SkinInfo_t SkinInfo(pks[PaintKitID]);//, PaintTranslatedKitName, PaintKitName, paint_kit_definition);
//
//				const size_t FindResult = SkinInfo.szShortSkinName.find('_');
//				std::string wearable_name;
//				if (FindResult != std::string::npos)
//				{
//					constexpr auto hydra = "hydra";
//					const size_t FindResult2 = SkinInfo.szShortSkinName.find(hydra);
//					if (FindResult2 != std::string::npos)
//					{
//						wearable_name.append(hydra);
//					}
//					else
//					{
//						wearable_name.append(PaintKitName.substr(0, FindResult));
//					}
//				}
//				else
//					continue;
//
//				for (auto& it : vec_wearable_info)
//				{
//					if (it.szShortWeaponName.find(wearable_name) != std::string::npos)
//					{
//						_wearbleskininfo[it].emplace_back(SkinInfo); //.push_back(SkinInfo);
//						break;
//					}
//				}
//				continue;
//			}
//
//			for (auto pItemSets : kvItemSets)
//			{
//				FOR_EACH_SUBKEY(pItemSets, pItemSet)
//				{
//					FOR_EACH_SUBKEY(pItemSet->FindKey(ITEMS), pItem)
//					{
//						std::string Weapon = pItem->GetName();
//						if (Weapon.find(PaintKitName + "]") == std::string::npos)
//							continue;
//
//						const size_t FindResult = Weapon.find(']');
//						if (FindResult == std::string::npos)
//							continue;
//
//						Weapon = Weapon.substr(FindResult + 1);
//
//						for (auto& i : vec_weapon_info)
//						{
//							if (i.szShortWeaponName == Weapon)
//							{
//								//auto paint_kit_definition = itemschema->GetPaintKitDefinition(PaintKitID);
//								_skininfo[i].emplace_back(pks[PaintKitID]);//(PaintKitID, PaintTranslatedKitName, PaintKitName, paint_kit_definition);
//								//push_back(SkinInfo);
//								break;
//							}
//						}
//					}
//				}
//			}
//
//			FOR_EACH_SUBKEY(pWeaponIcons, pWeaponIcon)
//			{
//				std::string IconPath = pWeaponIcon->GetString(XorStr("icon_path"));
//				if (IconPath[IconPath.size() - 7] != '_')
//					continue;
//
//				const size_t FindResult = IconPath.find(PaintKitName + XorStr("_medium"));
//				if (FindResult == std::string::npos)
//					continue;
//
//				const std::string weapon = IconPath.substr(23, FindResult - 24);
//
//				for (auto& i : vec_weapon_info)
//				{
//					//auto paint_kit_definition = itemschema->GetPaintKitDefinition(PaintKitID);
//					if ((i.bIsKnife || PaintKitName == XorStr("cu_m4a1_howling") /*again, volvo pls get ur shit together*/)
//						&& i.szShortWeaponName == weapon)
//					{
//						_skininfo[i].emplace_back(pks[PaintKitID]);//(PaintKitID, PaintTranslatedKitName, PaintKitName, paint_kit_definition);
//						break;
//					}
//				}
//			}
//		}
//	}
//}

void Skin_loader::SortData()
{
	g_vWearablesSkins.emplace_back();
	g_vKnifesSkins.emplace_back();

	for(auto&it: vec_t_player_info)
	{
		g_v_T_TeamSkins.emplace_back(it);
		g_v_All_TeamSkins.emplace_back(it);
	}
	for(auto&it: vec_ct_player_info)
	{
		g_v_CT_TeamSkins.emplace_back(it);
		g_v_All_TeamSkins.emplace_back(it);
	}
	
	for (auto& it : _skininfo)
	{
		std::vector<SkinInfo_t>* vec = &it.second;
		sort(vec->begin(), vec->end(), SkinInfo_t::compare);
	}

	for (const mapWearableSkinInfo::value_type& Wearable : GetWearableSkinInfo())
	{
		g_vWearablesSkins.emplace_back(Wearable);
	}

	for (auto& Weapon : GetSkinInfo())
	{
		if (Weapon.first.bIsKnife)
		{
			g_vKnifesSkins.emplace_back(Weapon);
		}
		else
		{
			g_vWeaponSkins.emplace_back(Weapon);
		}
	}
	g_vStickers = GetStickerInfo();
	//g_vSprays = GetSprayInfo();
}
