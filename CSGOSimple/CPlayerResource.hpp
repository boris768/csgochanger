#pragma once


#include <memory>
#include "NetvarManager.hpp"

class netvars_CPlayerResource
{
public:
	ptrdiff_t m_iPing;
	ptrdiff_t m_iKills;
	ptrdiff_t m_iAssists;
	ptrdiff_t m_iDeaths;
	ptrdiff_t m_bConnected;
	ptrdiff_t m_iTeam;
	ptrdiff_t m_iPendingTeam;
	ptrdiff_t m_bAlive;
	ptrdiff_t m_iHealth;
	ptrdiff_t m_iPlayerC4;
	ptrdiff_t m_iMVPs;
	ptrdiff_t m_iArmor;
	ptrdiff_t m_iScore;
	ptrdiff_t m_iCompetitiveRanking;
	ptrdiff_t m_iCompetitiveWins;
	ptrdiff_t m_iCompTeammateColor;
	ptrdiff_t m_szClan;
	ptrdiff_t m_nActiveCoinRank;
	ptrdiff_t m_nMusicID;
	ptrdiff_t m_nPersonaDataPublicCommendsLeader;
	ptrdiff_t m_nPersonaDataPublicCommendsTeacher;
	ptrdiff_t m_nPersonaDataPublicCommendsFriendly;

	netvars_CPlayerResource();
};
extern netvars_CPlayerResource* netvars_c_player_resource;

class CPlayerResource : public SourceEngine::IClientEntity
{};

class C_CSPlayerResource : public CPlayerResource
{
public:
	static C_CSPlayerResource* GetPlayerResource();

	int GetPing(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iPing + index * sizeof(void*));
	}
	int GetKills(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iKills + index * sizeof(void*));
	}
	int GetAssists(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iAssists + index * sizeof(void*));
	}
	int GetDeaths(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iDeaths + index * sizeof(void*));
	}
	bool GetConnected(int index)
	{
		return *(bool*)((ptrdiff_t)this + netvars_c_player_resource->m_bConnected + index);
	}
	int GetTeam(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iTeam + index * sizeof(void*));
	}
	int GetPendingTeam(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iPendingTeam + index * sizeof(void*));
	}
	bool GetAlive(int index)
	{
		return *(bool*)((ptrdiff_t)this + netvars_c_player_resource->m_bAlive + index);
	}
	int GetHealth(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iHealth + index * sizeof(void*));
	}
	int GetPlayerC4()
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iPlayerC4);
	}
	int GetMVPs(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iMVPs + index * sizeof(void*));
	}
	int GetArmor(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iArmor + index * sizeof(void*));
	}
	int GetScore(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iScore + index * sizeof(void*));
	}
	int* GetCompetitiveRanking(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_iCompetitiveRanking + index * sizeof(void*));
	}
	int* GetCompetitiveWins(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_iCompetitiveWins + index * sizeof(void*));
	}
	int GetCompTeammateColor(int index)
	{
		return *(int*)((ptrdiff_t)this + netvars_c_player_resource->m_iCompTeammateColor + index * sizeof(void*));
	}
	const char* GetClan(int index)
	{
		return (const char*)((ptrdiff_t)this + netvars_c_player_resource->m_szClan + index * 16);
	}
	int* GetActiveCoinRank(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_nActiveCoinRank + index * sizeof(void*));
	}
	int* GetMusicID(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_nMusicID + index * sizeof(void*));
	}
	int* GetPersonaDataPublicCommendsLeader(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_nPersonaDataPublicCommendsLeader + index * sizeof(void*));
	}
	int* GetPersonaDataPublicCommendsTeacher(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_nPersonaDataPublicCommendsTeacher + index * sizeof(void*));
	}
	int* GetPersonaDataPublicCommendsFriendly(int index)
	{
		return (int*)((ptrdiff_t)this + netvars_c_player_resource->m_nPersonaDataPublicCommendsFriendly + index * sizeof(void*));
	}
};