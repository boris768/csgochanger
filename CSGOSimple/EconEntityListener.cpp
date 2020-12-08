#include "EconEntityListener.h"
#include "Hooks.hpp"

//moved for complier silence
SourceEngine::IClientEntity* SourceEngine::IClientEntityList::GetClientEntityFromHandleOffset(const void* base, ptrdiff_t offset)
{
	return GetClientEntityFromHandle(static_cast<CBaseHandle>(*reinterpret_cast<int*>(reinterpret_cast<ptrdiff_t>(base) + offset)));
}

SourceEngine::EconEntityListener::EconEntityListener()
{
	auto entitylist = interfaces.EntityList();
	assert(entitylist);

	entity_listner_handle = entitylist->m_entityListeners.AddToTail(this);
}

SourceEngine::EconEntityListener::~EconEntityListener()
{
	auto entitylist = interfaces.EntityList();
	assert(entitylist);

	entitylist->m_entityListeners.Remove(entity_listner_handle);
}

bool should_be_deleted(const int t)
{
	return t == -1;
}

void SourceEngine::EconEntityListener::on_entity_created(IClientEntity* pEntity)
{
	auto weapon = reinterpret_cast<C_BaseCombatWeapon*>(pEntity);
	if (!weapon || weapon->EntIndex() < 65 || !weapon->IsWeaponByClass())
		return;

	weapons.erase(std::remove_if(weapons.begin(), weapons.end(), should_be_deleted), weapons.end());

	const auto added = weapons.emplace_back(weapon->EntIndex());
	//Hooks::changer_log->msg(std::string(XorStr("EntityListener: adding ") + std::to_string(added.m_idx)));
	//Hooks::changer_log->msg(std::string(XorStr("EntityListener: current storage:\n")));
	//for(auto& it: weapons)
	//{
	//	Hooks::changer_log->msg(it.m_weapon->GetClientClass()->m_pNetworkName);
	//	Hooks::changer_log->msg(std::to_string(it.m_idx));
	//	Hooks::changer_log->msg("\n");
	//}
}

void SourceEngine::EconEntityListener::on_entity_deleted(IClientEntity* pEntity)
{
	if(!weapons.empty())
	{
		const auto index = pEntity->EntIndex();
		for (auto& weapon : weapons)
		{
			if (weapon == index)
			{
				weapon = -1;
				break;
			}
		}
	}
}

void SourceEngine::EconEntityListener::on_level_shutdown()
{
	weapons.clear();
}
