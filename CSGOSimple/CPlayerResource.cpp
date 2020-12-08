// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CPlayerResource.hpp"
#include "CSGOStructs.hpp"
#include "Utils.hpp"
#include "XorStr.hpp"

C_CSPlayerResource* player_resource = nullptr;

netvars_CPlayerResource::netvars_CPlayerResource()
{
	const auto netvar_mgr = netvar_manager::get();

	m_iPing = netvar_mgr.get_offset(FNV("CPlayerResource->m_iPing"));
	m_iKills = netvar_mgr.get_offset(FNV("CPlayerResource->m_iKills"));
	m_iAssists = netvar_mgr.get_offset(FNV("CPlayerResource->m_iAssists"));
	m_iDeaths = netvar_mgr.get_offset(FNV("CPlayerResource->m_iDeaths"));
	m_bConnected = netvar_mgr.get_offset(FNV("CPlayerResource->m_bConnected"));
	m_iTeam = netvar_mgr.get_offset(FNV("CPlayerResource->m_iTeam"));
	m_iPendingTeam = netvar_mgr.get_offset(FNV("CPlayerResource->m_iPendingTeam"));
	m_bAlive = netvar_mgr.get_offset(FNV("CPlayerResource->m_bAlive"));
	m_iHealth = netvar_mgr.get_offset(FNV("CPlayerResource->m_iHealth"));

	m_iPlayerC4 = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iPlayerC4"));
	m_iMVPs = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iMVPs"));
	m_iArmor = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iArmor"));
	m_iScore = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iScore"));
	m_iCompetitiveRanking = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iCompetitiveRanking"));
	m_iCompetitiveWins = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iCompetitiveWins"));
	m_iCompTeammateColor = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_iCompTeammateColor"));
	m_szClan = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_szClan"));
	m_nActiveCoinRank = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_nActiveCoinRank"));
	m_nMusicID = netvar_mgr.get_offset(FNV("CCSPlayerResource->m_nMusicID"));
	m_nPersonaDataPublicCommendsLeader = netvar_mgr.get_offset(
		FNV("CCSPlayerResource->m_nPersonaDataPublicCommendsLeader"));
	m_nPersonaDataPublicCommendsTeacher = netvar_mgr.get_offset(
		FNV("CCSPlayerResource->m_nPersonaDataPublicCommendsTeacher"));
	m_nPersonaDataPublicCommendsFriendly = netvar_mgr.get_offset(
		FNV("CCSPlayerResource->m_nPersonaDataPublicCommendsFriendly"));
}

C_CSPlayerResource* C_CSPlayerResource::GetPlayerResource()
{
	//auto entitylist = SourceEngine::interfaces.EntityList();
	
	//for (int i = 64; i < entitylist->GetHighestEntityIndex(); ++i)
	//{
	//	player_resource = reinterpret_cast<C_CSPlayerResource*>(entitylist->GetClientEntity(i));
	//	if (!player_resource)
	//		continue;
	//	SourceEngine::ClientClass * client_class = player_resource->GetClientClass();
	//	if (!client_class)
	//		continue;
	//	if (strcmp(client_class->m_pNetworkName, XorStr("CCSPlayerResource")) == 0)
	//		break;
	//}

	if(!player_resource)
	{
		player_resource = *reinterpret_cast<C_CSPlayerResource**>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7")) + 2);
	}

	return player_resource;
}
