//#pragma once
//
//#include "server_messages/gcsdk_gcmessages.pb.h"
//
//#pragma comment(lib, "libprotobuf.lib")
//#pragma comment(lib, "libprotoc.lib")
//
//class CInventoryChanger { // INVENTORY * MEDAL CHANGER
//public:
//	static void PostRetrieveMessage(const uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
//	static bool PreSendMessage(const uint32_t &unMsgType, void* pubData, const uint32_t &cubData);
//	static void ApplyMedals(CMsgSOCacheSubscribed::SubscribedType* pInventoryCacheObject);
//};
//
//class CInventoryChanger1 { //RANK CHANGER
//public:
//	void PostRetrieveMessage(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
//	bool PreSendMessage(uint32_t &unMsgType, void* pubData, uint32_t &cubData);
//};
//
//
//bool SendClientHello();