#include "CSGOStructs.hpp"
#include "IGameEvent.h"
#include "Utils.hpp"
#include "XorStr.hpp"
//#include <vector>
//#include "CSGOStructs.hpp"
//#include "Config.h"
//#include "Hooks.hpp"
//#include "Menu.h"
//#include <future>
//#include "spsc_queue.h"
//#include "network.h"
//#include "CustomWinAPI.h"
//#include "Utils.hpp"
//#include <process.h>
//#include <iostream>


using GetEventDescriptor_t = SourceEngine::CGameEventDescriptor* (__thiscall*)(void*, const char*, int*);
GetEventDescriptor_t GetEventDescriptor_fn = nullptr;

SourceEngine::IGameEvent::~IGameEvent()
{
}

SourceEngine::CGameEvent::CGameEvent(CGameEventDescriptor* descriptor, const char* name)
{
	m_pDescriptor = descriptor;
	m_pDataKeys = new KeyValues(name);
	m_pDataKeys->SetInt((XorStr("splitscreenplayer")), 0);
}

SourceEngine::CGameEventDescriptor* SourceEngine::CGameEventManager::GetEventDescriptor(const char* name)
{
	//E8 ? ? ? ? 8B D8 85 DB 75 27
	if(!GetEventDescriptor_fn)
	{
		GetEventDescriptor_fn = reinterpret_cast<GetEventDescriptor_t>(Utils::dwGetCallAddress(
			Utils::FindSignature(SourceEngine::memory.engine(), XorStr("E8 ? ? ? ? 8B D8 85 DB 75 27"))));
	}
	
	return GetEventDescriptor_fn(this, name, nullptr);
}



//
//SourceEngine::EventListener* stat_events;
////std::unordered_map<std::string, size_t> events_count;
////player_statistic observable_player;
////std::string current_server;
//long long current_matchid;
//
//using Helper_GetLastCompetitiveMatchId_t = int64_t(__cdecl*)();
//Helper_GetLastCompetitiveMatchId_t get_last_competitive_match_id = nullptr;
//
//SourceEngine::EventListener::EventListener(std::vector<const char*> events)
//{
//	for (auto& it : events)
//		interfaces.GameEventManager()->AddListener(this, it, false);
//}
//
//SourceEngine::EventListener::~EventListener()
//{
//	interfaces.GameEventManager()->RemoveListener(this);
//	g_pWinApi->dwTerminateThread(reinterpret_cast<HANDLE>(worker_threadid), 0);
//}
//
//int64_t Helper_GetLastCompetitiveMatchId()
//{
//	if (!get_last_competitive_match_id)
//	{
//		get_last_competitive_match_id = reinterpret_cast<Helper_GetLastCompetitiveMatchId_t>(Utils::FindSignature(
//			SourceEngine::memory.client(), XorStr("55 8B EC A1 ? ? ? ? 83 EC 08 D1 E8 56 57 A8 01")));
//	}
//	return get_last_competitive_match_id();
//}
//
//void increment_event(const std::string& event_name)
//{
//	//const auto event_counter = events_count.find(event_name);
//	//if (event_counter != events_count.end())
//	//{
//	//	event_counter->second++;
//	//}
//	//else
//	//{
//	//	events_count.insert_or_assign(event_name, 1);
//	//}
//}
//
//void GetPlayerList(std::vector<PlayerEntry>& player_list)
//{
//	C_CSPlayerResource* player_resource = C_CSPlayerResource::GetPlayerResource();
//	if (!player_resource)
//		return;
//	SourceEngine::player_info_t info;
//	for (size_t i = 1; i < 64; ++i)
//	{
//		auto player = reinterpret_cast<C_CSPlayer*>(SourceEngine::interfaces.EntityList()->GetClientEntity(i));
//		if (!player)
//			continue;
//		SourceEngine::interfaces.Engine()->GetPlayerInfo(i, &info);
//		if (!info.ishltv)
//		{
//			player_list.emplace_back(
//				info.xuid,
//				info.name,
//				static_cast<uint8_t>(player->GetTeamNum()),
//				*player_resource->GetCompetitiveRanking(i),
//				player_resource->GetKills(i),
//				player_resource->GetDeaths(i),
//				player_resource->GetAssists(i)
//			);
//		}
//	}
//}
//
//void debug_output_event_data(SourceEngine::CGameEvent* debug_event)
//{
//	const char* name = debug_event->m_pDataKeys->GetName();
//
//	std::cout << "event: " << name << std::endl;
//
//	for (auto kv = debug_event->m_pDataKeys->GetFirstSubKey(); kv; kv = kv->GetNextKey())
//	{
//		std::cout << kv->GetName() << " : ";
//		switch (kv->GetDataType())
//		{
//		case SourceEngine::KeyValues::TYPE_STRING:
//			std::cout << kv->GetString();
//			break;
//		case SourceEngine::KeyValues::TYPE_INT:
//		case SourceEngine::KeyValues::TYPE_PTR:
//			std::cout << kv->GetInt();
//			break;
//		case SourceEngine::KeyValues::TYPE_FLOAT:
//			std::cout << kv->GetFloat();
//			break;
//		case SourceEngine::KeyValues::TYPE_WSTRING:
//			std::wcout << kv->GetWString();
//			break;
//		case SourceEngine::KeyValues::TYPE_NONE:
//			std::cout << " error value";
//			break;
//		case SourceEngine::KeyValues::TYPE_UINT64:
//		case SourceEngine::KeyValues::TYPE_COLOR:
//		case SourceEngine::KeyValues::TYPE_COMPILED_INT_BYTE:
//		case SourceEngine::KeyValues::TYPE_COMPILED_INT_0:
//		case SourceEngine::KeyValues::TYPE_COMPILED_INT_1:
//		case SourceEngine::KeyValues::TYPE_NUMTYPES:
//			std::cout << " unsupported value";
//			break;
//		}
//
//		std::cout << std::endl;
//	}
//	std::cout << "-----------------------------------------------------------------" << std::endl;
//}
//
////spsc_queue<SourceEngine::KeyValues*> event_queue;
//
//void __cdecl ProcessEventThread(void* /*arg*/)
//{
//	using namespace SourceEngine;
//
//	IVEngineClient* engine = interfaces.Engine();
//	KeyValues* event_data = nullptr;
//	while (true)
//	{
//		//if (!event_queue.dequeue(event_data) || !event_data)
//		//{
//		//	event_data = nullptr;
//		//	g_pWinApi->dwSwitchToThread();
//		//	g_pWinApi->dwSleep(20);
//		//	continue;
//		//}
//		const std::string event_name = event_data->GetName();
//		if (event_name == XorStr("round_start"))
//		{
//			current_matchid = Helper_GetLastCompetitiveMatchId();
//			if (Options::menu.weapon.current_sky)
//			{
//				ChangeSky(Options::menu.weapon.current_sky);
//			}
//			player_info_t info;
//			engine->GetPlayerInfo(engine->GetLocalPlayer(), &info);
//			observable_player.player_id = info.xuid;
//			observable_player.match_id = current_matchid;
//			observable_player.serverid = current_server;
//			observable_player.game_map = engine->GetLevelName();
//
//			observable_player.new_round();
//
//			Hooks::flash_window();
//		}
//		else if (event_name == XorStr("round_end"))
//		{
//			C_CSGameRulesProxy* rules = C_CSGameRulesProxy::GetGameRulesProxy();
//			const int winner = event_data->GetInt(XorStr("winner"));
//
//			GetPlayerList(observable_player.player_list_);
//			C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
//			if (pLocal)
//			{
//				observable_player.duration = interfaces.GlobalVars()->curtime - rules->GetRoundStartTime();
//
//				TeamID player_teamid = pLocal->GetTeamNum();
//				C_CSTeam* player_team = C_CSTeam::GetTeamByNum(player_teamid);
//				C_CSTeam* enemy_team = C_CSTeam::GetTeamT();
//				if (player_teamid == TeamID::TEAM_TERRORIST)
//					enemy_team = C_CSTeam::GetTeamCT();
//				else if (player_teamid != TeamID::TEAM_COUNTER_TERRORIST)
//					_endthread(); // player is not in any playable team - its fatal
//
//				observable_player.player_team = static_cast<uint8_t>(player_teamid);
//				Sleep(2000); //wait for struct updates
//
//				observable_player.team_round_wins = player_team->GetscoreTotal();
//				observable_player.enemy_team_round_wins = enemy_team->GetscoreTotal();
//
//				if (winner == static_cast<int>(pLocal->GetTeamNum()))
//				{
//					observable_player.win_status = 1;
//				}
//				if (
//					observable_player.team_round_wins == 16 ||
//					observable_player.enemy_team_round_wins == 16 ||
//					(observable_player.team_round_wins == 15 && observable_player.enemy_team_round_wins == 15) ||
//					enemy_team->IsSurrended() ||
//					player_team->IsSurrended()
//				)
//				{
//					observable_player.final_round = true;
//				}
//				observable_player.round_id = observable_player.team_round_wins + observable_player.enemy_team_round_wins;
//				if (current_matchid && !rules->IsWarmupPeriod() && rules->IsValveDS() && !engine->IsHLTV())
//					_beginthread(&save_statistic, 0, &observable_player);
//			}
//		}
//		else if (event_name == XorStr("bomb_planted"))
//		{
//			player_info_t info;
//			const auto playerid = interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid")));
//			interfaces.Engine()->GetPlayerInfo(playerid, &info);
//			auto player = reinterpret_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntity(playerid));
//			Vector pos;
//			if (player)
//				pos = player->GetOrigin();
//			else
//				pos.Zero();
//
//			observable_player.bomb_planteds_.emplace_back(
//				info.xuid,
//				event_data->GetInt(XorStr("site")),
//				pos
//			);
//		}
//		else if (event_name == XorStr("bomb_defused"))
//		{
//			player_info_t info;
//			interfaces.Engine()->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))),
//			                                   &info);
//			observable_player.bomb_defuseds_.emplace_back(
//				info.xuid,
//				event_data->GetInt(XorStr("site"))
//			);
//		}
//		else if (event_name == XorStr("hostage_rescued"))
//		{
//			player_info_t info;
//			interfaces.Engine()->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))),
//			                                   &info);
//			observable_player.hostage_rescueds_.emplace_back(
//				info.xuid,
//				event_data->GetInt(XorStr("hostage")),
//				event_data->GetInt(XorStr("site"))
//			);
//		}
//		else if (event_name == XorStr("weapon_fire"))
//		{
//			player_info_t info;
//			interfaces.Engine()->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))),
//			                                   &info);
//			observable_player.weapon_fires_.emplace_back(
//				info.xuid,
//				event_data->GetString(XorStr("weapon")),
//				event_data->GetInt(XorStr("silenced")) != 0
//			);
//		}
//		else if (event_name == XorStr("player_hurt"))
//		{
//			PlayerHurt add;
//
//			player_info_t info;
//			engine->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("attacker"))), &info);
//			add.attacker = info.xuid;
//			engine->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))), &info);
//			add.hurted = info.xuid;
//			add.weapon = event_data->GetString(XorStr("weapon"));
//			add.health = event_data->GetInt(XorStr("health"));
//			add.armor = event_data->GetInt(XorStr("armor"));
//			add.dmghealth = event_data->GetInt(XorStr("dmg_health"));
//			add.dmgarmor = event_data->GetInt(XorStr("dmg_armor"));
//			add.hitgroup = event_data->GetInt(XorStr("hitgroup"));
//			observable_player.player_hurts_.emplace_back(add);
//		}
//		else if (event_name == XorStr("player_death") || event_name == XorStr("other_death"))
//		{
//			C_CSPlayer* pAttacker = nullptr;
//			const auto userid_attacker = event_data->GetInt(XorStr("attacker"));
//			if (userid_attacker)
//			{
//				const auto entityid_attacker = interfaces.Engine()->GetPlayerForUserID(userid_attacker);
//				if (entityid_attacker)
//				{
//					pAttacker = reinterpret_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntity(entityid_attacker));
//				}
//			}
//			C_CSPlayer* assister = nullptr;
//			const auto userid_assister = event_data->GetInt(XorStr("assister"));
//			if (userid_assister)
//			{
//				const auto entityid_assister = interfaces.Engine()->GetPlayerForUserID(userid_assister);
//				if (entityid_assister)
//				{
//					assister = reinterpret_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntity(entityid_assister));
//				}
//			}
//			C_CSPlayer* died = nullptr;
//			const auto userid_died = event_data->GetInt(XorStr("userid"));
//			if (userid_died)
//			{
//				const auto entityid_died = interfaces.Engine()->GetPlayerForUserID(userid_died);
//				if (entityid_died)
//				{
//					died = reinterpret_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntity(entityid_died));
//				}
//			}
//			if (pAttacker || assister || died)
//			{
//				std::string weapon_name = event_data->GetString(XorStr("weapon"));
//				if (weapon_name.empty())
//					continue;
//
//				//Statistic block
//
//				PlayerDeath add;
//				player_info_t info;
//				engine->GetPlayerInfo(engine->GetPlayerForUserID(event_data->GetInt(XorStr("attacker"))), &info);
//				add.attacker = info.xuid;
//
//				engine->GetPlayerInfo(engine->GetPlayerForUserID(event_data->GetInt(XorStr("assister"))), &info);
//				add.assister = info.xuid;
//
//				engine->GetPlayerInfo(engine->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))), &info);
//				add.died = info.xuid;
//
//				add.weapon = move(weapon_name);
//				add.weaponOriginalOwnerXUID = event_data->GetUint64(XorStr("weaponOriginalOwnerXUID"));
//				add.penetrated = event_data->GetInt(XorStr("penetrated"));
//				add.headshot = event_data->GetInt(XorStr("headshot")) != 0;
//				observable_player.player_deaths_.emplace_back(add);
//			}
//		}
//		else if (event_name == XorStr("server_spawn"))
//		{
//			const std::string steamid = event_data->GetString(XorStr("steamid"));
//			const std::string address = event_data->GetString(XorStr("address"));
//			current_server = steamid + " : " + address;
//			observable_player.offical_server = true;
//		}
//		else if (event_name == XorStr("game_newmap"))
//		{
//			bad_adresses.clear();
//
//			observable_player.new_round();
//			observable_player.final_round = false;
//			observable_player.round_id = -1;
//		}
//		else if (event_name == XorStr("item_purchase"))
//		{
//			ItemPurchase add;
//			player_info_t info;
//			engine->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))), &info);
//			add.user = info.xuid;
//			add.team = event_data->GetInt(XorStr("team"));
//			add.weapon = event_data->GetString(XorStr("weapon"));
//			observable_player.item_purchases_.emplace_back(add);
//		}
//		else if (event_name == XorStr("round_mvp"))
//		{
//			RoundMVP add;
//
//			player_info_t info;
//			engine->GetPlayerInfo(interfaces.Engine()->GetPlayerForUserID(event_data->GetInt(XorStr("userid"))), &info);
//			add.user = info.xuid;
//			add.reason = event_data->GetInt(XorStr("reason"));
//			observable_player.round_mvps_.emplace_back(add);
//		}
//		else if (event_name == XorStr("cs_game_disconnected"))
//		{
//			current_server.clear();
//			current_matchid = 0;
//			observable_player.offical_server = false;
//		}
//
//		delete event_data;
//		event_data = nullptr;
//	}
//}
//
//
//void SourceEngine::EventListener::FireGameEvent(IGameEvent* e)
//{
//	KeyValues* event_data = reinterpret_cast<CGameEvent*>(e)->m_pDataKeys->MakeCopy();
//	//event_queue.enqueue(event_data);
//	if (worker_threadid == -1)
//	{
//		worker_threadid = _beginthread(&ProcessEventThread, 0, nullptr);
//	}
//	if (sender_threadid == -1)
//	{
//		sender_threadid = _beginthread(&process, 0, nullptr);
//	}
//}
