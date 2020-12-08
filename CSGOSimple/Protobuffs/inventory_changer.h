#pragma once
#include "ProtobuffMessages.h"
#include "..\\SourceEngine\Definitions.hpp"

constexpr auto START_MUSICKIT_INDEX = 1500000;
constexpr auto START_ITEM_INDEX = 2000000;

void fix_null_inventory(CMsgSOCacheSubscribed& cache);
void clear_equip_state(SubscribedType& object);
void apply_medals(SubscribedType& object);
void apply_music_kits(SubscribedType& object);
void add_all_items(SubscribedType& object);
void add_item(SubscribedType& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear,
              const std::string& name);
std::string inventory_changer(void* pubDest, const uint32_t* pcub_msg_size);
inline std::string make_equipped_state(int team, int slot);
bool inventory_changer_presend(void* pubData, const uint32_t& cubData);

extern std::vector<uint32_t> music_kits;

template<typename T>
std::string get_4bytes(T value)
{
	return std::string{ static_cast<const char*>( reinterpret_cast<void*>(&value) ), 4 };
}

template<typename T>
std::string make_econ_item_attribute(int def_index, T value)
{
	CSOEconItemAttribute attribute;
	attribute.add_def_index(def_index);
	attribute.add_value_bytes(get_4bytes<T>(value));
	return attribute.serialize();
}


