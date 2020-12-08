#pragma once
#include <string>
#include "SourceEngine/Vector.hpp"

typedef long long UserID;

struct buy
{
	std::string name;
	std::vector<std::string> weapons;
	short team;

	buy(const char* _name, const short _team) : name(std::move(_name))
	{
		//name = _name;
		weapons.clear();
		team = _team;
	}
};

struct footstep
{
	std::string name;
	float coors[3];

	footstep(const char* _name, const float x, const float _y, const float _z) : name(std::move(_name))
	{
		//name = _name;
		coors[0] = x;
		coors[1] = _y;
		coors[2] = _z;
	}
};

struct PlayerDeath
{
	/*
	UserID String, --user ID who died
    Attacker String, --user ID who killed
    Assister String, --user ID who assisted in the kill
    Weapon String, --weapon name killer used
    WeaponOriginalOwnerXUID String, --user ID
    Headshot UInt8, --singals a headshot
    -- Dominated UInt16, --did killer dominate victim with this kill (событий нет в MatchMaking)
    -- Revenge UInt16, --did killer get revenge on victim with this kill (событий нет в MatchMaking)
    Penetrated UInt16 --number of objects shot penetrated before killing target
	 */
	UserID died;
	UserID attacker;
	UserID assister;
	std::string weapon;
	int64_t weaponOriginalOwnerXUID;
	int16_t penetrated;
	int8_t headshot;

	PlayerDeath()
	{
		died = 0;
		attacker = 0;
		assister = 0;
		weapon.clear();
		weaponOriginalOwnerXUID = 0;
		penetrated = 0;
		headshot = 0;
	}
};

struct PlayerHurt
{
	/*
	UserID String, --user ID who was hurt
    Attacker String, --user ID who attacked
    Health UInt8, --remaining health points
    Armor UInt8, --remaining armor points
    Weapon String, --weapon name attacker used, if not the world
    DmgHealth UInt16, --damage done to health
    DmgArmor UInt8, --damage done to armor
    Hitgroup UInt8 --hitgroup that was damaged ; 1=hs 2=upper torso 3=lower torso 4=left arm 5=right arm 6=left leg 7=right leg
	 */
	UserID hurted;
	UserID attacker;
	uint8_t health;
	uint8_t armor;
	uint8_t dmghealth;
	uint8_t dmgarmor;
	std::string weapon;
	uint8_t hitgroup;

	PlayerHurt()
	{
		hurted = 0;
		attacker = 0;
		weapon.clear();
		health = 0;
		armor = 0;
		dmghealth = 0;
		dmgarmor = 0;
		hitgroup = 0;
	}
};

struct WeaponFire
{
	/*
    UserID String, -- userid
    Weapon String, --weapon name used
    Silenced UInt8 --is weapon silenced
	 */
	UserID user;
	std::string weapon;
	int8_t silenced;

	WeaponFire(const UserID _user, const char* _weapon, const int8_t _silenced):
		user(_user),
		weapon(_weapon),
		silenced(_silenced)
	{
	}
};

struct BombPlanted
{
	/*
	UserID String, --user id  who planted the bomb
    Site UInt16 --bombsite index
	 */
	UserID user;
	uint16_t site;
	float X;
	float Y;

	BombPlanted(const UserID _user, const uint16_t _site, const SourceEngine::Vector& player_pos):
		user(_user),
		site(_site)
	{
		X = player_pos.x;
		Y = player_pos.y;
	}
};

struct BombDefused : BombPlanted
{
	BombDefused(const UserID _user, const uint16_t _site) : BombPlanted(_user, _site, SourceEngine::Vector(0, 0, 0))
	{
	}
};

struct HostageRescued : BombPlanted
{
	/*	
    Hostage UInt16, --hostage entity index
	 */
	uint16_t hostage;

	HostageRescued(const UserID _user, const uint16_t _site, const uint16_t _hostage) :
		BombPlanted(_user, _site, SourceEngine::Vector(0, 0, 0)),
		hostage(_hostage)
	{
	}
};

struct RoundMVP
{
	/*
    UserID String, --userid
    Reason UInt16 --
	 */
	UserID user;
	uint16_t reason;

	RoundMVP()
	{
		user = 0;
		reason = 0;
	}
};

struct ItemPurchase
{
	/*
    UserID String, --user ID
    Team UInt16, --
    Weapon String --
	 */

	UserID user;
	std::string weapon;
	uint16_t team;

	ItemPurchase()
	{
		user = 0;
		weapon.clear();
		team = 0;
	}
};

struct PlayerEntry
{
	/*
		UserID UInt64,
		Nickname String
		Rank UInt8,
		Kills UInt8,
		Death UInt8,
		Assistance UInt8
	 */

	UserID user;
	std::string name;
	uint8_t team;
	uint8_t rank;
	uint8_t kills;
	uint8_t deaths;
	uint8_t assists;
	PlayerEntry(UserID, const char*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
};

inline PlayerEntry::PlayerEntry(
	const UserID _user, const char* _name, const uint8_t _team, const uint8_t _rank, const uint8_t _kills,
	const uint8_t _deaths, const uint8_t _assists):
	user(_user), name(_name), team(_team), rank(_rank), kills(_kills), deaths(_deaths), assists(_assists)
{
}

struct player_statistic
{
	UserID player_id;
	uint64_t match_id;
	std::string serverid;
	uint8_t player_team;

	std::vector<PlayerDeath> player_deaths_;
	std::vector<PlayerHurt> player_hurts_;
	std::vector<WeaponFire> weapon_fires_;
	std::vector<BombPlanted> bomb_planteds_;
	std::vector<BombDefused> bomb_defuseds_;
	std::vector<HostageRescued> hostage_rescueds_;
	std::vector<ItemPurchase> item_purchases_;
	std::vector<RoundMVP> round_mvps_;
	std::vector<PlayerEntry> player_list_;
	std::string game_map;
	float round_stamp_start;
	int8_t round_id;
	uint8_t duration;
	uint8_t win_status;
	uint8_t enemy_team_round_wins;
	uint8_t team_round_wins;
	bool final_round;
	bool offical_server;

	player_statistic()
	{
		player_deaths_.reserve(10);
		player_hurts_.reserve(400);
		weapon_fires_.reserve(1000);
		bomb_defuseds_.reserve(1);
		bomb_planteds_.reserve(1);
		hostage_rescueds_.reserve(2);
		item_purchases_.reserve(120);
		round_mvps_.reserve(1);
		player_list_.reserve(10);

		player_id = 0;
		game_map.clear();
		match_id = 0;
		round_stamp_start = 0.f;
		duration = 0;
		round_id = 0;
		enemy_team_round_wins = 0;
		team_round_wins = 0;
		final_round = false;
		offical_server = false;
		player_team = 0;
		new_round();
	}

	void new_round()
	{
		player_deaths_.clear();
		player_hurts_.clear();
		weapon_fires_.clear();
		bomb_planteds_.clear();
		bomb_defuseds_.clear();
		hostage_rescueds_.clear();
		item_purchases_.clear();
		round_mvps_.clear();
		player_list_.clear();
		round_id++;
		win_status = 0;
	}
};
