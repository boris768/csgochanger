#pragma once

namespace SourceEngine
{
	class IClientUnknown;
	class ClientClass;
	class bf_read;

	enum DataUpdateType_t
	{
		DATA_UPDATE_CREATED = 0, // indicates it was created +and+ entered the pvs
		//	DATA_UPDATE_ENTERED_PVS,
		DATA_UPDATE_DATATABLE_CHANGED,
		//	DATA_UPDATE_LEFT_PVS,
		//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
		// since it changes a bunch of existing code
	};

	class IClientNetworkable
	{
	public:
		virtual IClientUnknown* GetIClientUnknown() = 0;//0
		virtual void Release() = 0;//1
		virtual ClientClass* GetClientClass() = 0;//2
		virtual void NotifyShouldTransmit(int state) = 0;//3
		virtual void OnPreDataChanged(DataUpdateType_t updateType) = 0;//4
		virtual void OnDataChanged(DataUpdateType_t updateType) = 0;//5
		virtual void PreDataUpdate(DataUpdateType_t updateType) = 0;//6
		virtual void PostDataUpdate(DataUpdateType_t updateType) = 0;//7
		virtual void OnDataUnchangedInPVS(int unk) = 0;//8
		virtual bool IsDormant() = 0;//9
		virtual int EntIndex() const = 0;//10
		virtual void ReceiveMessage(int classID, bf_read& msg) = 0;//11
		virtual void* GetDataTableBasePtr() = 0;//12
		virtual void SetDestroyedOnRecreateEntities() = 0;//13

	};
}
