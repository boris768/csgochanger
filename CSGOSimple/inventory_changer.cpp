#include "Protobuffs/inventory_changer.h"
#include "SourceEngine/SDK.hpp"
#include "Protobuffs/ProtoParse.h"
#include "Protobuffs/profile_info_changer.h"
#include <ctime>
#include "Config.h"
#include "Utils.hpp"

std::vector<uint32_t> music_kits = { 3, 4, 5, 6, 7, 8 };
//
//static void add_item(SubscribedType& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear,
//                     const std::string& name)
//{
//	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;
//
//	CSOEconItem item;
//	item.add_id(START_ITEM_INDEX + itemIndex);
//	item.add_account_id(steamid);
//	item.add_def_index(itemIndex);
//	item.add_inventory(START_ITEM_INDEX + index);
//	item.add_origin(24);
//	item.add_quantity(1);
//	item.add_level(1);
//	item.add_style(0);
//	item.add_flags(0);
//	item.add_in_use(true);
//	item.add_original_id(0);
//	item.add_rarity(rarity);
//	item.add_quality(0);
//
//	if (!name.empty())
//		item.add_custom_name(name);
//
//	// Add equipped state for both teams
//	const SourceEngine::TeamID avalTeam = GetAvailableClassID(itemIndex);
//
//	if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_TERRORIST) {
//		item.add_equipped_state(make_equipped_state(SourceEngine::TeamID::TEAM_TERRORIST, GetSlotID(itemIndex)));
//	}
//	if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_COUNTER_TERRORIST) {
//		item.add_equipped_state(make_equipped_state(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, GetSlotID(itemIndex)));
//	}
//
//	// Add CSOEconItemAttribute's
//	item.add_attribute(make_econ_item_attribute(6, float(paintKit)));
//	item.add_attribute(make_econ_item_attribute(7, float(seed)));
//	item.add_attribute(make_econ_item_attribute(8, float(wear)));
//
//	// Time acquired attribute
//	item.add_attribute(make_econ_item_attribute(180, (uint32_t)std::time(nullptr)));
//
//	// Stickers
//	for (int j = 0; j < 4; j++)
//	{
//		item.add_attribute(make_econ_item_attribute(113 + 4 * j, uint32_t(289 + j))); // Sticker Kit
//		item.add_attribute(make_econ_item_attribute(114 + 4 * j, float(0.001f)));     // Sticker Wear
//		item.add_attribute(make_econ_item_attribute(115 + 4 * j, float(1.f)));        // Sticker Scale
//		item.add_attribute(make_econ_item_attribute(116 + 4 * j, float(0.f)));        // Sticker Rotation
//	}
//
//	object.add_object_data(item.serialize());
//}
//

inline std::string make_equipped_state(int team, int slot)
{
	CSOEconItemEquipped equipped_state;
	equipped_state.add_new_class(team);
	equipped_state.add_new_slot(slot);
	return equipped_state.serialize();
}

std::string inventory_changer(void* pubDest, const uint32_t* pcub_msg_size) {
	CMsgClientWelcome msg(reinterpret_cast<void*>(reinterpret_cast<DWORD>(pubDest) + 8), *pcub_msg_size - 8);
	if (!msg.has_outofdate_subscribed_caches())
		return msg.serialize();

	auto cache = msg.get_outofdate_subscribed_caches<CMsgSOCacheSubscribed>();
	// If not have items in inventory, Create null inventory
	fix_null_inventory(cache);
	// Add custom items
	auto objects = cache.getAll_objects();
	for (size_t i = 0; i < objects.size(); i++)
	{
		SubscribedType object(objects[i].String());

		if (!object.has_type_id())
			continue;

		switch (object.get_type_id().Int32())
		{
		case 1: // m_unInventory
		{
			//if (true) //g_Options.skins_packet_clear_default_items
				object.clear_object_data();

			clear_equip_state(object);
			apply_medals(object);
			apply_music_kits(object);
			add_all_items(object);
			cache.replace_objects(object.serialize(), i);
		}
		break;
	default: ;
		}
	}
	msg.replace_outofdate_subscribed_caches(cache.serialize(), 0);

	return msg.serialize();
}

static void add_item(SubscribedType& object, const CustomSkinWeapon &weapon, SourceEngine::TeamID team, int index)
{
	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;

	CSOEconItem item;
	item.add_id(START_ITEM_INDEX + index);
	item.add_account_id(steamid);
	item.add_def_index(weapon.info->iItemDefinitionIndex);
	item.add_inventory(START_ITEM_INDEX + index);
	item.add_origin(24);
	item.add_quantity(1);
	item.add_level(1);
	item.add_style(0);
	item.add_flags(0);
	item.add_in_use(true);
	item.add_original_id(0);
	item.add_rarity(Utils::CombinedItemAndPaintRarity(weapon.info->game_item_definition->item_rarity, weapon.skin->m_pPaintKitDefinition->rarity));
	if (weapon.bStatTrack)
	{
		item.add_quality(1);
	}
	else
	{
		item.add_quality(weapon.iEntityQuality);
	}

	if (weapon.name[0])
		item.add_custom_name(std::string(weapon.name));

	// Add equipped state for both teams
	const SourceEngine::TeamID avalTeam = GetAvailableClassID(weapon.info->iItemDefinitionIndex);

	if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == team) 
	{
		item.add_equipped_state(make_equipped_state(team, GetSlotID(weapon.info->iItemDefinitionIndex)));
	}

	//if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_TERRORIST) {
	//	item.add_equipped_state(make_equipped_state(SourceEngine::TeamID::TEAM_TERRORIST, GetSlotID(weapon.info->iItemDefinitionIndex)));
	//}
	//if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_COUNTER_TERRORIST) {
	//	item.add_equipped_state(make_equipped_state(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, GetSlotID(weapon.info->iItemDefinitionIndex)));
	//}

	// Add CSOEconItemAttribute's
	item.add_attribute(make_econ_item_attribute(6, float(weapon.skin->iPaintKitID)));
	item.add_attribute(make_econ_item_attribute(7, float(weapon.iSeed)));
	item.add_attribute(make_econ_item_attribute(8, weapon.fWear));

	// Time acquired attribute
	item.add_attribute(make_econ_item_attribute(180, static_cast<uint32_t>(std::time(nullptr))));

	if (weapon.bStatTrack)
	{
		item.add_attribute(make_econ_item_attribute(80, weapon.iKills));
		item.add_attribute(make_econ_item_attribute(81, 0));
	}
	// Stickers
	for (size_t j = 0; j < weapon.info->iMaxStickers; ++j)
	{
		const auto stiker = &weapon.Stickers.at(j);
		if (stiker->iStickerKitID == 0)
			continue;

		item.add_attribute(make_econ_item_attribute(113 + 4 * static_cast<int>(j), stiker->iStickerKitID));		// Sticker Kit
		item.add_attribute(make_econ_item_attribute(114 + 4 * static_cast<int>(j), stiker->fStickerWear));		// Sticker Wear
		item.add_attribute(make_econ_item_attribute(115 + 4 * static_cast<int>(j), stiker->fStickerScale));		// Sticker Scale
		item.add_attribute(make_econ_item_attribute(116 + 4 * static_cast<int>(j), stiker->fStickerRotation));	// Sticker Rotation
	}

	object.add_object_data(item.serialize());
}

static void add_item(SubscribedType& object, const CustomSkinKnife& weapon, SourceEngine::TeamID team, int index)
{
	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;

	CSOEconItem item;
	item.add_id(START_ITEM_INDEX + index);
	item.add_account_id(steamid);
	item.add_def_index(weapon.info->iItemDefinitionIndex);
	item.add_inventory(START_ITEM_INDEX + index);
	item.add_origin(24);
	item.add_quantity(1);
	item.add_level(1);
	item.add_style(0);
	item.add_flags(0);
	item.add_in_use(true);
	item.add_original_id(0);
	item.add_rarity(6);
	if (weapon.bStatTrack)
	{
		item.add_quality(1);
	}
	else
	{
		item.add_quality(3);
	}

	if (weapon.name[0])
		item.add_custom_name(std::string(weapon.name));

	// Add equipped state for both teams
	const SourceEngine::TeamID avalTeam = GetAvailableClassID(weapon.info->iItemDefinitionIndex);

	if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == team)
	{
		item.add_equipped_state(make_equipped_state(team, GetSlotID(weapon.info->iItemDefinitionIndex)));
	}

	// Add CSOEconItemAttribute's
	item.add_attribute(make_econ_item_attribute(6, float(weapon.skin->iPaintKitID)));
	item.add_attribute(make_econ_item_attribute(7, float(weapon.iSeed)));
	item.add_attribute(make_econ_item_attribute(8, weapon.fWear));

	// Time acquired attribute
	item.add_attribute(make_econ_item_attribute(180, static_cast<uint32_t>(std::time(nullptr))));

	if (weapon.bStatTrack)
	{
		item.add_attribute(make_econ_item_attribute(80, weapon.iKills));
		item.add_attribute(make_econ_item_attribute(81, 0));
	}

	object.add_object_data(item.serialize());
}

static void add_item(SubscribedType& object, const CustomSkinWearable& weapon, SourceEngine::TeamID team, int index)
{
	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;

	CSOEconItem item;
	item.add_id(START_ITEM_INDEX + index);
	item.add_account_id(steamid);
	item.add_def_index(weapon.info->iItemDefinitionIndex);
	item.add_inventory(START_ITEM_INDEX + index);
	item.add_origin(24);
	item.add_quantity(1);
	item.add_level(1);
	item.add_style(0);
	item.add_flags(0);
	item.add_in_use(true);
	item.add_original_id(0);
	item.add_rarity(6);
	item.add_quality(3);
	
	if (weapon.name[0])
		item.add_custom_name(std::string(weapon.name));

	// Add equipped state for both teams
	const SourceEngine::TeamID avalTeam = GetAvailableClassID(weapon.info->iItemDefinitionIndex);

	if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == team)
	{
		item.add_equipped_state(make_equipped_state(team, GetSlotID(weapon.info->iItemDefinitionIndex)));
	}

	// Add CSOEconItemAttribute's
	item.add_attribute(make_econ_item_attribute(6, float(weapon.skin->iPaintKitID)));
	item.add_attribute(make_econ_item_attribute(7, 0.f));
	item.add_attribute(make_econ_item_attribute(8, weapon.fWear));

	// Time acquired attribute
	item.add_attribute(make_econ_item_attribute(180, static_cast<uint32_t>(std::time(nullptr))));

	object.add_object_data(item.serialize());
}

bool inventory_changer_presend(void* pubData, const uint32_t& cubData)
{
	CMsgAdjustItemEquippedState msg(reinterpret_cast<void*>(reinterpret_cast<DWORD>(pubData) + 8), cubData - 8);
	// Change music kit check
	if ((msg.has_item_id()
		&& msg.get_new_class().UInt32() == 0)
		|| msg.get_new_slot().UInt32() == 54)
	{
		const auto ItemIndex = msg.get_item_id().UInt64() - START_MUSICKIT_INDEX;

		if (ItemIndex > 38 || ItemIndex < 3)
			return true;

		Options::musickit_index = msg.get_new_slot().UInt32() == 0xFFFF ? 0 : ItemIndex - 2;

		return false;
	}
	// Change weapon skin check
	return !msg.has_item_id() || !msg.has_new_class() || !msg.has_new_slot();
}

static void fix_null_inventory(CMsgSOCacheSubscribed& cache)
{
	bool inventory_exist = false;
	auto objects = cache.getAll_objects();
	for (auto& i : objects)
	{
		SubscribedType object(i.String());
		if (!object.has_type_id())
			continue;
		if (object.get_type_id().Int32() != 1)
			continue;
		inventory_exist = true;
		break;
	}
	if (!inventory_exist)
	{
		//int cache_size = objects.size();
		SubscribedType null_object;
		null_object.add_type_id(1);

		cache.add_objects(null_object.serialize());
	}
}

static void clear_equip_state(SubscribedType& object)
{
	auto object_data = object.getAll_object_data();
	for (size_t j = 0; j < object_data.size(); j++)
	{
		CSOEconItem item(object_data[j].String());

		if (!item.has_equipped_state())
			continue;

		// create NOT equiped state for item
		const auto null_equipped_state = make_equipped_state(0, 0);

		// unequip all
		auto equipped_state = item.getAll_equipped_state();
		for (size_t k = 0; k < equipped_state.size(); k++)
			item.replace_equipped_state(null_equipped_state, k);

		object.replace_object_data(item.serialize(), j);
	}
}

std::vector<uint32_t> packets_medals = { 1372, 958, 957, 956, 955 };
int packets_equipped_medal = 874;

static void apply_medals(SubscribedType& object)
{
	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;

	CSOEconItem medal;
	medal.add_account_id(steamid);
	medal.add_origin(9);
	medal.add_rarity(6);
	medal.add_quantity(1);
	medal.add_quality(4);
	medal.add_level(1);

	// Time acquired attribute
	medal.add_attribute(make_econ_item_attribute(222, static_cast<uint32_t>(std::time(nullptr))));

	int i = 10000;
	for (uint32_t MedalIndex : packets_medals)
	{
		medal.add_def_index(MedalIndex);
		medal.add_inventory(i);
		medal.add_id(i);
		object.add_object_data(medal.serialize());
		i++;
	}

	if (packets_equipped_medal)
	{
		medal.add_def_index(packets_equipped_medal);
		medal.add_inventory(i);
		medal.add_id(i);
		medal.add_equipped_state(make_equipped_state(0, 55));
		object.add_object_data(medal.serialize());
	}
}

static void apply_music_kits(SubscribedType& object)
{
	const uint32_t steamid = SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_comp.m_unAccountID;

	CSOEconItem music_kit;
	music_kit.add_account_id(steamid);
	music_kit.add_origin(9);
	music_kit.add_rarity(6);
	music_kit.add_quantity(1);
	music_kit.add_quality(4);
	music_kit.add_level(1);
	music_kit.add_flags(0);
	music_kit.add_def_index(1314);

	// Time acquired attribute
	music_kit.add_attribute(make_econ_item_attribute(75, static_cast<uint32_t>(std::time(nullptr))));

	for (int i = 3; i <= 38; ++i)
	{
		if (Options::musickit_index != i)
		{
			music_kit.add_attribute(make_econ_item_attribute(166, i)); // Music kit id
			music_kit.add_inventory(START_MUSICKIT_INDEX + i);
			music_kit.add_id(START_MUSICKIT_INDEX + i);
			object.add_object_data(music_kit.serialize());
		}
	}

	if (Options::musickit_index)
	{
		music_kit.add_attribute(make_econ_item_attribute(166, Options::musickit_index)); // Music kit id
		music_kit.add_inventory(START_MUSICKIT_INDEX + Options::musickit_index);
		music_kit.add_id(START_MUSICKIT_INDEX + Options::musickit_index);
		music_kit.add_equipped_state(make_equipped_state(0, 54));
		object.add_object_data(music_kit.serialize());
	}
}

int item_index = 1;
static void add_config_items(SubscribedType& object, SourceEngine::TeamID team_id, const customization& config)
{
	for (const std::vector<CustomSkinWeapon>::value_type& weapon : config.weapons)
	{
		if (weapon.info && weapon.skin && weapon.skin->iPaintKitID > 0)
		{
			add_item(object, weapon, team_id, item_index++);
		}
	}
	if (config.knife.info && config.knife.skin && config.knife.skin->iPaintKitID > 0)
	{
		add_item(object, config.knife, team_id, item_index++);
	}
	if (config.wearable.info && config.wearable.skin && config.wearable.skin->iPaintKitID > 0)
	{
		add_item(object, config.wearable, team_id, item_index++);
	}
}

static void add_all_items(SubscribedType& object)
{
	item_index = 1;
	add_config_items(object, SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, Options::config_ct);
	add_config_items(object, SourceEngine::TeamID::TEAM_TERRORIST, Options::config_t);
}

std::string profile_info_changer(void* pubDest, const uint32_t* pcubMsgSize) {
	MatchmakingGC2ClientHello msg((void*)((unsigned long)pubDest + 8), *pcubMsgSize - 8);

	//replace commends
	auto commendation_new = msg.get_commendation<PlayerCommendationInfo>();
	commendation_new.replace_cmd_friendly(1000);
	commendation_new.replace_cmd_teaching(2000);
	commendation_new.replace_cmd_leader(3000);
	msg.replace_commendation(commendation_new.serialize());

	//replace ranks
	auto ranking_new = msg.get_ranking<PlayerRankingInfo>();
	ranking_new.replace_rank_id(9);
	ranking_new.replace_wins(999);
	msg.replace_ranking(ranking_new.serialize());

	//replace private level
	msg.replace_player_level(40);

	return msg.serialize();
}