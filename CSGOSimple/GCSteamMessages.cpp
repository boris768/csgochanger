#include "Hooks.hpp"
#include "ISteamGameCoordinator.h"
#include "Protobuffs.h"
#include "CustomWinAPI.h"
#include "XorStr.hpp"
#include "Protobuffs/ProtobuffMessages.h"

namespace Hooks
{

	unsigned __cdecl reload_inventory(void*)
	{
		g_pWinApi->dwSleep(100);
		SourceEngine::interfaces.Engine()->ExecuteClientCmd(XorStr("econ_clear_inventory_images"));
		LoadInventory();
		return 0;
	}

	
	Protobuffs ProtoFeatures;

	EGCResults __fastcall Hooked_RetrieveMessage(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
	{
		//static auto oGCRetrieveMessage = gc_hook.get_original<SendMessage_t>(2);
		const auto status = g_fnOriginalRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);

		if (status == k_EGCResultOK)
		{

			//void* thisPtr = nullptr;
			//__asm mov thisPtr, ebx;
			//auto oldEBP = *reinterpret_cast<void**>(reinterpret_cast<uint32_t>(_AddressOfReturnAddress()) - 4);

			const uint32_t messageType = *punMsgType & 0x7FFFFFFF;
			if(messageType == k_EMsgGCClientWelcome)
			g_pWinApi->dwCreateSimpleThread(reload_inventory, nullptr, 0);
			//Protobuffs::ReceiveMessage(thisPtr, oldEBP, messageType, pubDest, cubDest, pcubMsgSize);
		}

		return status;
	}

	EGCResults __fastcall Hooked_SendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
	{
		const bool sendMessage = Protobuffs::PreSendMessage(unMsgType, const_cast<void*>(pubData), cubData);

		if (!sendMessage)
			return k_EGCResultOK;

		return g_fnOriginalSendMessage(ecx, edx, unMsgType, const_cast<void*>(pubData), cubData);
	}
}
