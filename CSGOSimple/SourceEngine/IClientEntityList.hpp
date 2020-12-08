#pragma once
#include "UtlVector.hpp"

namespace SourceEngine
{
	class IClientNetworkable;
	class IClientEntity;
	class CBaseHandle;
	class IClientUnknown;

	class IClientEntityListener {
	public:
		virtual void on_entity_created(IClientEntity* pEntity) = 0;
		virtual void on_entity_deleted(IClientEntity* pEntity) = 0;
	};

	class IClientEntityList
	{
	public:
		virtual IClientNetworkable* GetClientNetworkable(int entnum) = 0;
		virtual IClientNetworkable* GetClientNetworkableFromHandle(CBaseHandle hEnt) = 0;
		virtual IClientUnknown* GetClientUnknownFromHandle(CBaseHandle hEnt) = 0;
		virtual IClientEntity* GetClientEntity(int entNum) = 0;
		virtual IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
		virtual int NumberOfEntities(bool bIncludeNonNetworkable) = 0;
		virtual int GetHighestEntityIndex() = 0;
		virtual void SetMaxEntities(int maxEnts) = 0;
		virtual int GetMaxEntities() = 0;

		IClientEntity* GetClientEntityFromHandleOffset(const void* base, ptrdiff_t offset);
		CUtlVector<IClientEntityListener*>	m_entityListeners;

		// Current count
		int					m_iNumServerEnts;
		// Max allowed
		int					m_iMaxServerEnts;

		int					m_iNumClientNonNetworkable;

		// Current last used slot
		int					m_iMaxUsedServerIndex;
	};

}
