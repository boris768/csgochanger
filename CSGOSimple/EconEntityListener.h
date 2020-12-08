#pragma once
#include "SourceEngine/IClientEntityList.hpp"
#include "CSGOStructs.hpp"

namespace SourceEngine
{
	//struct ent_container_t {
	//	ent_container_t(): m_weapon(nullptr), m_idx(0)
	//	{
	//	}

	//	ent_container_t(IClientEntity* ent) {
	//		m_idx = ent->EntIndex();
	//		m_weapon = static_cast<C_BaseCombatWeapon*>(ent);
	//	}
	//	C_BaseCombatWeapon* m_weapon;
	//	int m_idx;
	//};

	class EconEntityListener :
		public IClientEntityListener
	{
	public:
		EconEntityListener();
		~EconEntityListener();
		void on_entity_created(IClientEntity* pEntity) override;
		void on_entity_deleted(IClientEntity* pEntity) override;
		void on_level_shutdown();

		std::vector<int> weapons;

	private:
		int entity_listner_handle;
	};

}
