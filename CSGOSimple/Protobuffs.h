#pragma once
#include <string>

class Protobuffs
{
public:
	static void WritePacket(const std::string& packet, void* thisPtr, void* oldEBP, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize);
	static void ReceiveMessage(void* thisPtr, void* oldEBP, uint32_t messageType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize);
	static bool PreSendMessage(uint32_t &unMsgType, void* pubData, uint32_t &cubData);
	static bool SendClientHello();
	static bool SendMatchmakingClient2GCHello();
	static bool EquipWeapon(int weaponid, int classid, int slotid);
	static void EquipAll();
};
