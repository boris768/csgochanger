#include "Protobuffs.h"
#include "Protobuffs\inventory_changer.h"
#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "Hooks.hpp"
#include "CustomWinAPI.h"
#include "XorStr.hpp"
#include "Protobuffs/profile_info_changer.h"

#define CAST(cast, address, add) reinterpret_cast<cast>((uint32_t)(address) + (uint32_t)(add))

void Protobuffs::WritePacket(const std::string& packet, void* thisPtr, void* oldEBP, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
{
	if (static_cast<uint32_t>(packet.size()) <= cubDest - 8)
	{
		memcpy(reinterpret_cast<void*>(reinterpret_cast<DWORD>(pubDest) + 8), (void*)packet.data(), packet.size());
		*pcubMsgSize = packet.size() + 8;
	}
	else
	{
		auto* const memPtr = *CAST(void**, thisPtr, 0x14);
		const auto memPtrSize = *CAST(uint32_t*, thisPtr, 0x18);
		const auto newSize = (memPtrSize - cubDest) + packet.size() + 8;

		auto* memory = SourceEngine::interfaces.MemAlloc()->Realloc(memPtr, newSize + 4);

		*CAST(void**, thisPtr, 0x14) = memory;
		*CAST(uint32_t*, thisPtr, 0x18) = newSize;
		*CAST(void**, oldEBP, -0x14) = memory;

		memcpy(CAST(void*, memory, 24), (void*)packet.data(), packet.size());

		*pcubMsgSize = packet.size() + 8;
	}
}

unsigned __cdecl refresh_inventory(void*)
{
	g_pWinApi->dwSleep(100);
	SourceEngine::interfaces.Engine()->ExecuteClientCmd(XorStr("econ_clear_inventory_images"));
	return 0;
}

void Protobuffs::ReceiveMessage(void* thisPtr, void* oldEBP, uint32_t messageType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
{
	switch(messageType)
	{
		case k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello:
		{
			//const auto packet = profile_info_changer(pubDest, pcubMsgSize);
			//WritePacket(packet, thisPtr, oldEBP, pubDest, cubDest, pcubMsgSize);
			break;
		}
		case k_EMsgGCClientWelcome:
		{
			//Hooks::g_fnCleanInventoryImageCacheDir();
			const auto packet = inventory_changer(pubDest, pcubMsgSize);
			WritePacket(packet, thisPtr, oldEBP, pubDest, cubDest, pcubMsgSize);
			g_pWinApi->dwCreateSimpleThread(refresh_inventory, nullptr, 0);
			break;
		}
	default: ;
	}
}

bool Protobuffs::PreSendMessage(uint32_t &unMsgType, void* pubData, uint32_t &cubData)
{
	const uint32_t MessageType = unMsgType & 0x7FFFFFFF;

	if (MessageType == k_EMsgGCAdjustItemEquippedState)
	{
		return inventory_changer_presend(pubData, cubData);
	}

	return true;
}

///////////////////////////////////
/******** Custom Messages ********/
///////////////////////////////////
bool Protobuffs::SendClientHello()
{
	CMsgClientHello msg;
	msg.add_client_session_need(1);
	auto packet = msg.serialize();

	void* ptr = SourceEngine::interfaces.MemAlloc()->Alloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	const bool result = SourceEngine::interfaces.GameCoordinator()->SteamSendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	SourceEngine::interfaces.MemAlloc()->Free(ptr);

	return result;
}

bool Protobuffs::SendMatchmakingClient2GCHello()
{
	ProtoWriter msg(0);
	const auto packet = msg.serialize();
	void* ptr = SourceEngine::interfaces.MemAlloc()->Alloc(packet.size() + 8);

	if (!ptr)
		return false;

	static_cast<uint32_t*>(ptr)[0] = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	static_cast<uint32_t*>(ptr)[1] = 0;

	memcpy(reinterpret_cast<void*>(reinterpret_cast<DWORD>(ptr) + 8), (void*)packet.data(), packet.size());
	const bool result = SourceEngine::interfaces.GameCoordinator()->SteamSendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	SourceEngine::interfaces.MemAlloc()->Free(ptr);

	return result;
}

bool Protobuffs::EquipWeapon(int weaponid, int classid, int slotid)
{
	CMsgAdjustItemEquippedState msg;
	msg.add_item_id(START_ITEM_INDEX + weaponid);
	msg.add_new_class(classid);
	msg.add_new_slot(slotid);
	msg.add_swap(true);
	auto packet = msg.serialize();

	void* ptr = SourceEngine::interfaces.MemAlloc()->Alloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCAdjustItemEquippedState | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	const bool result = SourceEngine::interfaces.GameCoordinator()->SteamSendMessage(k_EMsgGCAdjustItemEquippedState | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	SourceEngine::interfaces.MemAlloc()->Free(ptr);

	return result;
}

/*
 *int item_index = 1;
static void add_config_items(SubscribedType& object, SourceEngine::TeamID team_id, const customization& config)
{
	for (const std::vector<CustomSkinWeapon>::value_type& weapon : config.weapons)
	{
		if (weapon.info && weapon.skin && weapon.skin->iPaintKitID > 0)
		{
			add_item(object, weapon, team_id, item_index++);
		}
	}
	add_item(object, config.knife, team_id, item_index++);
	add_item(object, config.wearable, team_id, item_index++);
}

static void add_all_items(SubscribedType& object)
{
	item_index = 1;
	add_config_items(object, SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, Options::config_ct);
	add_config_items(object, SourceEngine::TeamID::TEAM_TERRORIST, Options::config_t);
}
 *
 */


void EquipTeam(const customization& config, SourceEngine::TeamID team)
{
	for (const std::vector<CustomSkinWeapon>::value_type& weapon : config.weapons)
	{
		if (weapon.info && weapon.skin && weapon.skin->iPaintKitID > 0)
		{
			Protobuffs::EquipWeapon(weapon.info->iItemDefinitionIndex, team, GetSlotID(weapon.info->iItemDefinitionIndex));
		}
	}
	if (config.knife.info && config.knife.skin && config.knife.skin->iPaintKitID > 0)
	{
		Protobuffs::EquipWeapon(config.knife.info->iItemDefinitionIndex, team, GetSlotID(config.knife.info->iItemDefinitionIndex));
	}
	if (config.wearable.info && config.wearable.skin && config.wearable.skin->iPaintKitID > 0)
	{
		Protobuffs::EquipWeapon(config.wearable.info->iItemDefinitionIndex, team, GetSlotID(config.wearable.info->iItemDefinitionIndex));
	}
}

void Protobuffs::EquipAll()
{
	EquipTeam(Options::config_ct, SourceEngine::TeamID::TEAM_COUNTER_TERRORIST);
	EquipTeam(Options::config_ct, SourceEngine::TeamID::TEAM_TERRORIST);

	//for (auto& x : g_skins) {
	//	const auto def_index = x.wId;
	//	EquipWeapon(def_index, SourceEngine::TeamID::TEAM_COUNTER_TERRORIST, GetSlotID(def_index));
	//	EquipWeapon(def_index, SourceEngine::TeamID::TEAM_TERRORIST, GetSlotID(def_index));
	//}
}