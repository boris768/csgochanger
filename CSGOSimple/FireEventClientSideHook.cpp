// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include "Menu.h"
#include "XorStr.hpp"


void SourceEngine::player_death_listener::FireGameEvent(IGameEvent* _event)
{
	const auto& local_player = Hooks::g_ctx.local_player;
	if (!local_player)
		return;
	
	IVEngineClient* engine = interfaces.Engine();

	const int player_index = engine->GetPlayerForUserID(_event->GetInt(XorStr("attacker")));
	if (player_index == Hooks::g_ctx.local_player_index)
	{
		player_info_t died_info;
		engine->GetPlayerInfo(engine->GetPlayerForUserID(_event->GetInt(XorStr("userid"))), &died_info);

		customization* current_config;
		switch (local_player->GetTeamNum())
		{
		case TEAM_COUNTER_TERRORIST:
			current_config = &Options::config_ct;
			break;
		case TEAM_TERRORIST:
			current_config = &Options::config_t;
			break;
		case TEAM_UNASSIGNED:
		case TEAM_SPECTATOR: //Spectator killed somebody? WTF??
		default:
			return;
		}

		const auto pszWeapon = _event->GetString(XorStr("weapon"));
		C_BaseCombatWeapon* weapon = local_player->GetActiveWeapon();
		if (!weapon)
			return;

		const short item_def = *weapon->GetItemDefinitionIndex();

		switch (C_BaseCombatWeapon::GetEconClass(item_def))
		{
		case C_BaseCombatWeapon::none: break;
		case C_BaseCombatWeapon::weapon:
		{
			//T1ODO: ENABLE IN RELEASE BUILD
#ifndef _DEBUG
			if (died_info.fakeplayer)
				return;
#endif

			auto it = current_config->weapons.data();
			const auto end = it + current_config->weapons.size();
			auto pCustomWeaponSkin = end;
			for (; it != end; ++it)
			{
				if (*it == item_def)
				{
					pCustomWeaponSkin = it;
					break;
				}
			}
			if (pCustomWeaponSkin != end)
			{
				if (pCustomWeaponSkin->bStatTrack)
				{
					pCustomWeaponSkin->iKills++;

					if (Options::enable_super_stattrack)
						Hooks::required_random_skin = true;
				}
			}
			break;
		}
		case C_BaseCombatWeapon::knife:
			{
			if (current_config->knife.info)
			{
				if (*weapon->GetModelIndex() != current_config->knife.model_indexes.iViewModel)
					return;

				if (strstr(pszWeapon, XorStr("knife")))
				{
					_event->SetString(XorStr("weapon"), current_config->knife.info->szShortWeaponName.c_str());
					//T1ODO: ENABLE IN RELEASE BUILD
#ifdef _DEBUG
					if (current_config->knife.bStatTrack)
#else
					if (current_config->knife.bStatTrack && !died_info.fakeplayer)
#endif
					{
						current_config->knife.iKills++;
						if (Options::enable_super_stattrack && Options::enable_random_skin)
							Hooks::required_random_skin = true;
					}
				}
			}
				break;
			}
		default: ;
		}
	}
}

void SourceEngine::player_team_listener_post_all::FireGameEvent(IGameEvent* event)
{
	if(Options::clantag_name[0] != '\0')
		sendClanTag(Options::clantag_name, Options::clantag_tag);
	ChangeSky(Options::menu.weapon.current_sky);
}

void SourceEngine::player_team_listener_pre_all::FireGameEvent(IGameEvent* _event)
{
	const auto user_id = _event->GetInt(XorStr("userid"));
	auto engine = interfaces.Engine();

	const int player_index = engine->GetPlayerForUserID(user_id);

	if (player_index == Hooks::g_ctx.local_player_index)
	{
		if (Options::enable_fix_viewmodel)
		{
			const auto oldteam = static_cast<TeamID>(_event->GetInt(XorStr("oldteam")));
			if (oldteam >= TEAM_TERRORIST)
			{
				const auto team = static_cast<TeamID>(_event->GetInt(XorStr("team")));
				if (team != oldteam && team >= TEAM_TERRORIST)
				{
					interfaces.ClientState()->ForceFullUpdate();
				}
			}
		}
	}
}

//DEPRECATED due switch to listners
//bool __fastcall Hooks::Hooked_FireEventClientSide(void* ecx, void* /*edx*/, SourceEngine::IGameEvent* _event)
//{
//	if (_event)
//	{
//		if (SourceEngine::interfaces.ClientState()->GetSignonState() != state_full)
//			return g_fnOriginalFireEventClientSide(ecx, _event);
//
//		//if (!player_death_descriptor)
//		//{
//		//	auto* game_event = static_cast<SourceEngine::CGameEvent*>(_event);
//
//		//	if (fnv::hash_runtime(game_event->GetName()) == FNV("player_death"))
//		//		player_death_descriptor = game_event->m_pDescriptor;
//		//	else return g_fnOriginalFireEventClientSide(ecx, _event);
//		//}
//		//if (!player_team_descriptor)
//		//{
//		//	auto* game_event = static_cast<SourceEngine::CGameEvent*>(_event);
//
//		//	if (fnv::hash_runtime(game_event->GetName()) == FNV("player_team"))
//		//		player_team_descriptor = game_event->m_pDescriptor;
//		//	else return g_fnOriginalFireEventClientSide(ecx, _event);
//		//}
//
//		const auto& local_player = g_ctx.local_player;
//		if (!local_player)
//			return g_fnOriginalFireEventClientSide(ecx, _event);
//
//		const auto game_event = static_cast<SourceEngine::CGameEvent*>(_event);
//		if (game_event->m_pDescriptor == player_death_descriptor)
//		{
//			SourceEngine::IVEngineClient* engine = SourceEngine::interfaces.Engine();
//
//			auto* pAttacker = reinterpret_cast<C_CSPlayer*>(SourceEngine::interfaces.EntityList()->GetClientEntity(
//				engine->GetPlayerForUserID(_event->GetInt(XorStr("attacker")))));
//			if (pAttacker == local_player)
//			{
//				SourceEngine::player_info_t died_info;
//				engine->GetPlayerInfo(engine->GetPlayerForUserID(_event->GetInt(XorStr("userid"))), &died_info);
//
//				customization* current_config;
//				switch (local_player->GetTeamNum())
//				{
//				case SourceEngine::TeamID::TEAM_COUNTER_TERRORIST:
//					current_config = &Options::config_ct;
//					break;
//				case SourceEngine::TeamID::TEAM_TERRORIST:
//					current_config = &Options::config_t;
//					break;
//				case SourceEngine::TeamID::TEAM_UNASSIGNED:
//				case SourceEngine::TeamID::TEAM_SPECTATOR: //Spectator killed somebody? WTF??
//				default:
//					return g_fnOriginalFireEventClientSide(ecx, _event);
//				}
//
//				const auto pszWeapon = _event->GetString(XorStr("weapon"));
//				C_BaseCombatWeapon* weapon = pAttacker->GetActiveWeapon();
//				if (!weapon)
//					return g_fnOriginalFireEventClientSide(ecx, _event);
//
//				short* item_def = weapon->GetItemDefinitionIndex();
//
//				//if (!item_def)
//				//	return g_fnOriginalFireEventClientSide(ecx, _event);
//
//				if (C_BaseCombatWeapon::GetEconClass(*item_def) == C_BaseCombatWeapon::knife)
//				{
//					if (current_config->knife.info)
//					{
//						if (*weapon->GetModelIndex() != current_config->knife.model_indexes.iViewModel)
//							return g_fnOriginalFireEventClientSide(ecx, _event);
//
//						if (strstr(pszWeapon, XorStr("knife")))
//						{
//							_event->SetString(XorStr("weapon"), current_config->knife.info->szShortWeaponName.c_str());
//							//T1ODO: ENABLE IN RELEASE BUILD
//#ifdef _DEBUG
//							if (current_config->knife.bStatTrack)
//#else
//							if (current_config->knife.bStatTrack && !died_info.fakeplayer)
//#endif
//							{
//								current_config->knife.iKills++;
//								if (Options::enable_super_stattrack && Options::enable_random_skin)
//									required_random_skin = true;
//							}
//						}
//					}
//				}
//				else
//				{
//					if (C_BaseCombatWeapon::GetEconClass(*item_def) == C_BaseCombatWeapon::weapon)
//					{
//						//T1ODO: ENABLE IN RELEASE BUILD
//#ifndef _DEBUG
//						if (died_info.fakeplayer)
//							return g_fnOriginalFireEventClientSide(ecx, _event);
//#endif
//						auto pCustomWeaponSkin = current_config->weapons.cend();
//						for (auto it = current_config->weapons.begin(); it != current_config->weapons.end(); ++it)
//						{
//							if (*it == *item_def)
//							{
//								pCustomWeaponSkin = it;
//								break;
//							}
//						}
//						if (pCustomWeaponSkin != current_config->weapons.cend())
//						{
//							if (pCustomWeaponSkin->bStatTrack)
//							{
//								pCustomWeaponSkin->iKills++;
//
//								if (Options::enable_super_stattrack)
//									required_random_skin = true;
//							}
//						}
//					}
//				}
//			}
//			return g_fnOriginalFireEventClientSide(ecx, _event);
//		}
//		if (game_event->m_pDescriptor == player_team_descriptor)
//		{
//			/*
//			*event name: player_team
//			*splitscreenplayer : int
//			*userid : int
//			*team : int
//			*oldteam : int
//			*disconnect : int
//			*autoteam : int
//			*silent : int
//			*isbot : int
//			*/
//			const auto user_id = _event->GetInt(XorStr("userid"));
//			auto engine = SourceEngine::interfaces.Engine();
//
//			const auto player = reinterpret_cast<C_CSPlayer*>(SourceEngine::interfaces.EntityList()->GetClientEntity(
//				engine->GetPlayerForUserID(user_id)));
//
//			if (player == local_player)
//			{
//				if (Options::enable_fix_viewmodel)
//				{
//					const auto oldteam = static_cast<SourceEngine::TeamID>(_event->GetInt(XorStr("oldteam")));
//					if (oldteam >= SourceEngine::TeamID::TEAM_TERRORIST)
//					{
//						const auto team = static_cast<SourceEngine::TeamID>(_event->GetInt(XorStr("team")));
//						if (team != oldteam && team >= SourceEngine::TeamID::TEAM_TERRORIST)
//						{
//							SourceEngine::interfaces.ClientState()->ForceFullUpdate();
//						}
//					}
//				}
//				const auto ret = g_fnOriginalFireEventClientSide(ecx, _event);
//				sendClanTag(Options::clantag_name, Options::clantag_tag);
//				return ret;
//			}
//		}
//	}
//
//	return g_fnOriginalFireEventClientSide(ecx, _event);
//}
