// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CClientState.h"
#include "Hooks.hpp"
#include "Inverntory.h"
#include "XorStr.hpp"

void SourceEngine::CClientState::SendServerCmdKeyValues(KeyValues * kv)
{
	offsets_c_client_state->SendServerCmdKeyValues(this + 8, kv);
}

template<class T>
static T* FindHudElement(const char* name)
{
	return (T*)Hooks::FindHudElement(Hooks::pThisFindHudElement, name);
}

struct hud_weapons_t {
	int32_t* get_weapon_count() {
		return reinterpret_cast<int32_t*>(uintptr_t(this) + 0x80);
	}
};

void SourceEngine::UpdatePanorama()
{
	// Panorama HUD Update 

	auto element = FindHudElement<uintptr_t*>(XorStr("CCSGO_HudWeaponSelection"));

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;
	
	for (int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = Hooks::ClearHudWeapon(hud_weapons, i);
}

void SourceEngine::CClientState::ForceFullUpdate()
{
	//Force Update
	if (GetSignonState() != state_full)
		return;

	int* tick = GetDeltaTick();

	if (!tick || *tick == -1)
		return;

	offsets_c_client_state->FreeEntityBaselines(this + 8);
	*tick = -1;

	interfaces.Engine()->ClientCmd_Unrestricted(XorStr("record x"));
	interfaces.Engine()->ClientCmd_Unrestricted(XorStr("stop"));

}

int * SourceEngine::CClientState::GetDeltaTick()
{
	return GetFieldPointer<int>(offsets_c_client_state->m_nDeltaTick);
}

bool* SourceEngine::CClientState::GetCheckCRCsWithServer()
{
	return GetFieldPointer<bool>(offsets_c_client_state->m_bCheckCRCsWithServer);
}

SignOnState SourceEngine::CClientState::GetSignonState()
{
	return *GetFieldPointer<SignOnState>(offsets_c_client_state->m_nSignonState);
}
