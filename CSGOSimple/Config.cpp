// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Config.h"
#include "XorStr.hpp"
#include <fstream>
#include "CustomWinAPI.h"
#include <ctime>
//#include "base64.h"
#include "Hooks.hpp"

bool dirExists(const std::string& dirName_in) noexcept
{
	const DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true; // this is a directory!

	return false; // this is not a directory!
}

config::config()
{
	builder.settings_[XorStr("commentStyle")] = XorStr("None");
	writer = builder.newStreamWriter();
	char* path = new char[MAX_PATH];
	const auto size = GetEnvironmentVariableA(XorStr("USERPROFILE"), path, MAX_PATH);
	if (!size)
	{
		GetCurrentDirectoryA( MAX_PATH, path);
	}
	config_ct_path = path;
	config_ct_path.append(XorStr("\\CSGO Changer"));// \\config_ct.json
	config_t_path = path;
	config_t_path.append(XorStr("\\CSGO Changer\\config_t.json"));// \\config_t.json
	config_other_path = path;
	config_other_path.append(XorStr("\\CSGO Changer\\config_other.json"));
	config_old_sounds_path = path;
	config_old_sounds_path.append(XorStr("\\CSGO Changer\\config_old_sounds.json"));
	
	if (!dirExists(config_ct_path))
	{
		CreateDirectoryA(config_ct_path.c_str(), nullptr);
	}

	config_ct_path.append(XorStr("\\config_ct.json"));
}


config::~config()
= default;

void config::write_other_options()
{
	settings.clear();

	settings[XorStr("Other")][XorStr("AutoAccept")] = Options::enable_auto_accept;
	settings[XorStr("Other")][XorStr("RandomSkin")] = Options::enable_random_skin;
	settings[XorStr("Other")][XorStr("ViewModelFix")] = Options::enable_fix_viewmodel;
	settings[XorStr("Other")][XorStr("SuperStattrack")] = Options::enable_super_stattrack;
	settings[XorStr("Other")][XorStr("RandomSkinKey")] = static_cast<int>(Options::random_skin_key);
	settings[XorStr("Other")][XorStr("MenuKey")] = static_cast<int>(Options::menu_key);
	settings[XorStr("Other")][XorStr("AutoSave_if_changed")] = Options::autosave_if_changed;
	settings[XorStr("Other")][XorStr("SkyBoxID")] = Options::menu.weapon.current_sky;//current_sky
	settings[XorStr("Other")][XorStr("clantag_name")] = Options::clantag_name;
	settings[XorStr("Other")][XorStr("clantag_tag")] = Options::clantag_tag;
	settings[XorStr("Other")][XorStr("Last_run")] = Options::current_session;
	
	std::ofstream out;// << writer->write(settings);
	out.open(config_other_path);
	if (out.is_open())
	{
		writer->write(settings, &out);
		out.flush();
		out.close();
	}
	else
	{
		g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
	}
}

void config::read_other_options()
{
	settings.clear();
	std::ifstream config_doc(config_other_path, std::ifstream::binary);
	if (!config_doc.is_open())
	{
		write_other_options();
		return;
	}	
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors?
		return;
	}

	config_doc.close();

	get_bool(settings[XorStr("Other")][XorStr("AutoAccept")], &Options::enable_auto_accept);
	get_bool(settings[XorStr("Other")][XorStr("RandomSkin")], &Options::enable_random_skin);
	get_bool(settings[XorStr("Other")][XorStr("ViewModelFix")], &Options::enable_fix_viewmodel);
	get_bool(settings[XorStr("Other")][XorStr("SuperStattrack")], &Options::enable_super_stattrack); 
	get_bool(settings[XorStr("Other")][XorStr("AutoSave_if_changed")], & Options::autosave_if_changed);
	get_time(settings[XorStr("Other")][XorStr("Last_run")], &Options::last_session);
	get_unsigned_int(settings[XorStr("Other")][XorStr("SkyBoxID")], &Options::menu.weapon.current_sky);//current_sky
	int buffer_key = 95;
	get_int(settings[XorStr("Other")][XorStr("RandomSkinKey")], &buffer_key);
	Options::random_skin_key = static_cast<ButtonCode_t>(buffer_key); 
	buffer_key = 74;
	get_int(settings[XorStr("Other")][XorStr("MenuKey")], &buffer_key);
	Options::menu_key = static_cast<ButtonCode_t>(buffer_key);

	get_c_string(settings[XorStr("Other")][XorStr("clantag_name")], Options::clantag_name, 32);
	get_c_string(settings[XorStr("Other")][XorStr("clantag_tag")], Options::clantag_tag, 32);
}


// ReSharper disable once CppMemberFunctionMayBeConst
void __cdecl save_statistic(void* arg)
{
//	player_statistic* stat = static_cast<player_statistic*>(arg);
//	Json::Value statistic_json;
//	Json::StyledWriter styledWriter2;
//	std::stringstream msg;
//	size_t is_need_upload = 0;
//	/*
//casual (школоло с тайзером, броней и no block) "game_type 0" "game_mode 0"
//competitive (турнирная версия,без халявного броника, тайзера, есть блокировка тел) "game_type 0" "game_mode 1"
//arms race (GunGame DM Turbo режим) "game_type 1" "game_mode 0"
//demolition (GunGame без DM и без turbo, плюс установка бомбы) "game_type 1" "game_mode 1"
//deathmatch "game_type 1" "game_mode 2"
//	 *
//	 */
//
//	statistic_json[XorStr("UserID")] = stat->player_id;
//	statistic_json[XorStr("MatchID")] = stat->match_id;
//	statistic_json[XorStr("ServerID")] = stat->serverid;
//	statistic_json[XorStr("Team")] = stat->player_team;
//	statistic_json[XorStr("RoundID")] = stat->round_id;
//	statistic_json[XorStr("Duration")] = stat->duration;
//	statistic_json[XorStr("WinStatus")] = stat->win_status;
//	statistic_json[XorStr("Map")] = stat->game_map;
//	statistic_json[XorStr("Final")] = static_cast<int>(stat->final_round);
//	if (stat->offical_server)
//	{
//		auto cvar = SourceEngine::interfaces.CVar();
//		SourceEngine::ConVar* game_type = cvar->FindVar(XorStr("game_type"));
//		SourceEngine::ConVar* game_mode = cvar->FindVar(XorStr("game_mode"));
//		SourceEngine::ConVar* sv_cheats = cvar->FindVar(XorStr("sv_cheats"));
//		const int type = game_type->GetInt();
//		const int mode = game_mode->GetInt();
//		const int cheats = sv_cheats->GetInt();
//		if (type != 0 || mode != 1 /*|| c_o_5v5 != 1*/ || cheats != 0)
//			stat->offical_server = false;
//	}
//
//	statistic_json[XorStr("Mode")] = 0;
//	Json::Value temp_array;
//	temp_array.clear();
//	if (!stat->player_deaths_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("PlayerDeath")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->player_deaths_.cbegin(); it != stat->player_deaths_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->died;
//			temp_array[XorStr("Attacker")] = it->attacker;
//			temp_array[XorStr("Assister")] = it->assister;
//			temp_array[XorStr("Weapon")] = it->weapon;
//			temp_array[XorStr("WeaponOriginalOwnerXUID")] = std::to_string(it->weaponOriginalOwnerXUID);
//			temp_array[XorStr("Headshot")] = it->headshot;
//			temp_array[XorStr("Penetrated")] = it->penetrated;
//			statistic_json[XorStr("PlayerDeath")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->player_hurts_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("PlayerHurt")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->player_hurts_.cbegin(); it != stat->player_hurts_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->hurted;
//			temp_array[XorStr("Attacker")] = it->attacker;
//			temp_array[XorStr("Weapon")] = it->weapon;
//			temp_array[XorStr("Health")] = it->health;
//			temp_array[XorStr("Armor")] = it->armor;
//			temp_array[XorStr("DmgHealth")] = it->dmghealth;
//			temp_array[XorStr("DmgArmor")] = it->dmgarmor;
//			temp_array[XorStr("Hitgroup")] = it->hitgroup;
//			statistic_json[XorStr("PlayerHurt")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//
//	if (!stat->weapon_fires_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("WeaponFire")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->weapon_fires_.cbegin(); it != stat->weapon_fires_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Weapon")] = it->weapon;
//			temp_array[XorStr("Silenced")] = it->silenced;
//			statistic_json[XorStr("WeaponFire")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->bomb_planteds_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("BombPlanted")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->bomb_planteds_.cbegin(); it != stat->bomb_planteds_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Site")] = it->site;
//			temp_array[XorStr("X")] = it->X;
//			temp_array[XorStr("Y")] = it->Y;
//			statistic_json[XorStr("BombPlanted")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->bomb_defuseds_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("BombDefused")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->bomb_defuseds_.cbegin(); it != stat->bomb_defuseds_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Site")] = it->site;
//			statistic_json[XorStr("BombDefused")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->hostage_rescueds_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("HostageRescued")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->hostage_rescueds_.cbegin(); it != stat->hostage_rescueds_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Hostage")] = it->hostage;
//			temp_array[XorStr("Site")] = it->site;
//			statistic_json[XorStr("HostageRescued")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->item_purchases_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("ItemPurchase")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->item_purchases_.cbegin(); it != stat->item_purchases_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Team")] = it->team;
//			temp_array[XorStr("Weapon")] = it->weapon;
//			statistic_json[XorStr("ItemPurchase")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->round_mvps_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("RoundMVP")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->round_mvps_.cbegin(); it != stat->round_mvps_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Reason")] = it->reason;
//			statistic_json[XorStr("RoundMVP")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	if (!stat->player_list_.empty())
//	{
//		is_need_upload++;
//		statistic_json[XorStr("Players")] = Json::Value(Json::arrayValue);
//		for (auto it = stat->player_list_.cbegin(); it != stat->player_list_.cend(); ++it)
//		{
//			temp_array[XorStr("UserID")] = it->user;
//			temp_array[XorStr("Nickname")] = it->name;
//			temp_array[XorStr("Rank")] = it->rank;
//			temp_array[XorStr("Kills")] = it->rank;
//			temp_array[XorStr("Death")] = it->rank;
//			temp_array[XorStr("Assistance")] = it->rank;
//			temp_array[XorStr("Team")] = it->rank;
//			statistic_json[XorStr("Players")].append(temp_array);
//			temp_array.clear();
//		}
//	}
//	msg << styledWriter2.write(statistic_json);
//	msg.flush();
//	std::string ready_to_encrypt = msg.str();
//	statistic_json.clear();
//
//	srand(static_cast<unsigned int>(time(nullptr)));
//	const size_t len = ready_to_encrypt.size();
//	std::basic_string<unsigned char> ready_to_send;
//
//	for (size_t i = 0; i < len; ++i)
//	{
//		ready_to_send += ready_to_encrypt[i];
//	}
//	
//	if (is_need_upload > 0 && stat->offical_server)
//	{
//		const std::string send = base64_encode(ready_to_send);
//		//SourceEngine::interfaces.CVar()->ConsolePrintf("Sending %d bytes to server", send.size());
//		//process(XorStr("http://78.155.219.44/dev"), ready_to_encrypt.c_str(), ready_to_encrypt.size());
//		//process(XorStr("https://stat.cschanger.com/v1"), send);
//		data_to_send.enqueue(send);
//	}
//	_endthread();
}

void config::write_customization(customization& config, const std::string& filename,
                                 std::vector<CustomSkinWeapon>::iterator* overwrite_weapon, const bool knife_only,
                                 const bool wearable_only)
{
	//read_customization(config, filename);
	settings.clear();
	std::ifstream config_doc(filename, std::ifstream::binary);
	if (config_doc.is_open())
	{
		Json::CharReaderBuilder builder;
		builder["collectComments"] = false;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, config_doc, &settings, &errs))
		{
			//log errors
		}

	}

	config_doc.close();
	if (wearable_only)
	{
		if (config.wearable.info != nullptr)
			write_wearble(config.wearable);

		std::ofstream out;// << writer->write(settings);
		out.open(filename);
		if (out.is_open())
		{
			writer->write(settings, &out);
			out.flush();
			out.close();
		}
		else
		{
			g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
		}
		return;
	}
	if (knife_only)
	{
		if (config.knife.info != nullptr)
			write_knife(config.knife);

		std::ofstream out;// << writer->write(settings);
		out.open(filename);
		if (out.is_open())
		{
			writer->write(settings, &out);
			out.flush();
			out.close();
		}
		else
		{
			g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
		}
		return;
	}
	if (overwrite_weapon)
	{
		write_weapon(*overwrite_weapon);
		std::ofstream out;// << writer->write(settings);
		out.open(filename);
		if (out.is_open())
		{
			writer->write(settings, &out);
			out.flush();
			out.close();
		}
		else
		{
			g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
		}
		return;
	}
	const auto end_wep = config.weapons.end();
	for (auto it = config.weapons.begin(); it != end_wep; ++it)
	{
		write_weapon(it);
	}
	if (config.knife.info != nullptr)
		write_knife(config.knife);
	if (config.wearable.info != nullptr)
		write_wearble(config.wearable);
	if (config.enable_custom_players)
		write_custom_players(config.team);
	
	std::ofstream out;// << writer->write(settings);
	out.open(filename);
	if (out.is_open())
	{
		writer->write(settings, &out);
		out.flush();
		out.close();
	}
	else
	{
		g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
	}
}

void config::read_customization(customization& config, const std::string& filename)
{
	settings.clear();
	std::ifstream config_doc(filename, std::ifstream::binary);
	if (!config_doc.is_open())
	{
		//g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"No config file, switching to default config"), XorStr(L"Config error"),
		//                         MB_OK);
		write_customization(config, filename);
		return;
	}
	//config_doc >> settings;

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs)) 
	{
		//log errors
		return;
	}
	for (int i = SourceEngine::weapon_deagle; i <= SourceEngine::weapon_revolver; ++i)
	{
		if (C_BaseCombatWeapon::GetEconClass(i) == C_BaseCombatWeapon::weapon)
			read_weapon(config.weapons, i);
	}
	for (auto it = config.weapons.begin(); it != config.weapons.end();)
	{
		if (it->info == nullptr)
		{
			it = config.weapons.erase(it);
		}
		else
		{
			++it;
		}
	}
	int def;
	get_int(settings[XorStr("SkinChanger")][XorStr("Knife")][XorStr("LastKnife")], &def);
	if (def >= 0)
		read_knife(config.knife, def);
	get_int(settings[XorStr("SkinChanger")][XorStr("Wearable")][XorStr("LastWearable")], &def);
	if (def >= 0)
		read_wearable(config.wearable, def);
	config.enable_custom_players = read_custom_players(config.team);
	config_doc.close();
}

void config::write_old_sounds(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds,
                              const std::string& filename)
{
	if (old_sounds.empty())
		return;

	settings.clear();
	std::ifstream config_doc(filename, std::ifstream::binary);
	if (config_doc.is_open())
	{
		Json::CharReaderBuilder builder;
		builder["collectComments"] = false;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, config_doc, &settings, &errs))
		{
			//log errors
			return;
		}
	}
	config_doc.close();

	for (auto& old_sound : old_sounds)
	{
		settings[std::to_string(old_sound.first)] = old_sound.second;
	}

	std::ofstream out(filename);
	if (out.is_open())
	{
		writer->write(settings, &out);
		out.flush();
		out.close();
	}
	//else
	//{
		//g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Cant save config file"), XorStr(L"Config error"), MB_OK);
	//}
}

void config::read_old_sounds(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds,
	const std::string& filename)
{
	std::ifstream config_doc(filename, std::ifstream::binary);
	if (!config_doc.is_open())
	{
		//g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"No audio-config file, using default"), XorStr(L"Config error"), MB_OK);
		save_old_sounds_config(old_sounds); // creating default config
		return;
	}

	settings.clear();
	//config_doc >> settings;
	
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors
		return;
	}
	
	config_doc.close();

	for (auto& old_sound : old_sounds)
		old_sound.second = settings[std::to_string(old_sound.first)].asBool();
}

void config::write_custom_players(const CustomPlayerSkin& team)
{
	for(int i = 0; i < 5; ++i)
	{
		int def;
		if (!team.skins[i])
			def = 0;
		else
			def = team.skins[i]->iItemDefinitionIndex;
		
		const std::string _def(std::to_string(def));
		const std::string _id(std::to_string(i));
		settings[XorStr("SkinChanger")][XorStr("CustomPlayer")][_id] = def;
	}
}

bool config::read_custom_players(CustomPlayerSkin& team)
{
	bool should_enable = false;

	for(int i = 0; i < 5; ++i)
	{
		bool is_ct = false;
		const std::string _id(std::to_string(i));
		int item_definition_index = 0;
		get_int(settings[XorStr("SkinChanger")][XorStr("CustomPlayer")][_id], &item_definition_index);
		if(item_definition_index > 0)
		{
			for (auto j = 0; j < g_v_CT_TeamSkins.size(); ++j)
			{
				const auto ct = &g_v_CT_TeamSkins.at(j);
				if (ct->iItemDefinitionIndex == item_definition_index)
				{
					team.skins[i] = ct;
					Options::menu.weapon.current_ct_team_skin[i] = j;
					should_enable = true;
					is_ct = true;
					break;
				}
			}
			if(!is_ct)
			{
				for (auto j = 0U; j < g_v_CT_TeamSkins.size(); ++j)
				{
					const auto t = &g_v_T_TeamSkins.at(j);
					if (t->iItemDefinitionIndex == item_definition_index)
					{
						team.skins[i] = t;
						Options::menu.weapon.current_t_team_skin[i] = j;
						should_enable = true;
						break;
					}
				}
			}
		}
	}
	return should_enable;
}


void config::read_weapon(std::vector<CustomSkinWeapon>& weapons, const int item_def)
{
	int last_pk;
	std::string def(std::to_string(item_def));
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][XorStr("LastPaintKit")], &last_pk);

	if (last_pk >= 0)
	{
		std::vector<CustomSkinWeapon>::value_type* exist = nullptr;
		for(std::vector<CustomSkinWeapon>::value_type& it : weapons)
		{
			if (it.info->iItemDefinitionIndex == item_def)
			{
				exist = &it;
				break;
			}
		}
		if (exist != nullptr)
		{
			return read_weapon(exist, def, last_pk);
		}
		CustomSkinWeapon reading(static_cast<const short &>(item_def), last_pk);
		read_weapon(reading, def, last_pk);
		weapons.emplace_back(reading);
	}
}

void config::read_weapon(std::vector<CustomSkinWeapon>::value_type* reading, std::string& def, const int pk)
{
	const std::string _pk(std::to_string(pk));
	get_c_string(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Name")], reading->name, 64);
	get_float(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Wear")], &reading->fWear);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Seed")], &reading->iSeed);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Kills")], &reading->iKills);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Quality")], &reading->iEntityQuality);
	get_bool(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("StatTrak")], &reading->bStatTrack);
	auto stickers = &reading->Stickers;
	for (size_t i = 0; i < stickers->size(); ++i)
	{
		const std::string sticker_pos = std::to_string(i);
		auto sticker = &stickers->at(i);
		get_int(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("StickerKit")],
			&sticker->iStickerKitID);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Wear")],
			&sticker->fStickerWear);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Scale")],
			&sticker->fStickerScale);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Rotation")],
			&sticker->fStickerRotation);
	}
	reading->info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(static_cast<const short&>(stoi(def)));
	if (reading->info)
	{
		auto skins = &reading->info->vSkins;
		reading->skin = &skins->at(0);
		const auto end = skins->data() + skins->size();
		for(auto it = skins->data(); it != end; ++it)
		{
			if (it->iPaintKitID == pk)
			{
				reading->skin = it;
				break;
			}
		}
		//const auto finded = std::find(skins->begin(), skins->end(), pk);
		//if (finded != skins->end())
		//	reading->skin = &*finded;
	}
}

void config::read_weapon(std::vector<CustomSkinWeapon>::iterator reading, std::string& def, const int pk)
{
	const std::string _pk(std::to_string(pk));
	get_c_string(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Name")], reading->name, 64);
	get_float(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Wear")], &reading->fWear);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Seed")], &reading->iSeed);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Kills")], &reading->iKills);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Quality")], &reading->iEntityQuality);
	get_bool(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("StatTrak")], &reading->bStatTrack);
	auto stickers = &reading->Stickers;
	for (size_t i = 0; i < stickers->size(); ++i)
	{
		const std::string sticker_pos = std::to_string(i);
		auto sticker = &stickers->at(i);
		get_int(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("StickerKit")],
			&sticker->iStickerKitID);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Wear")],
			&sticker->fStickerWear);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Scale")],
			&sticker->fStickerScale);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Rotation")],
			&sticker->fStickerRotation);
	}
	reading->info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(static_cast<const short &>(stoi(def)));
	if (reading->info)
	{
		auto skins = &reading->info->vSkins;
		reading->skin = &skins->at(0);
		const auto end = skins->data() + skins->size();
		for (auto it = skins->data(); it != end; ++it)
		{
			if (it->iPaintKitID == pk)
			{
				reading->skin = it;
				break;
			}
		}
		//const auto finded = std::find(skins->begin(), skins->end(), pk);
		//if (finded != skins->end())
		//	reading->skin = &*finded;
	}
}

void config::read_weapon(CustomSkinWeapon& reading, std::string& def, const int pk)
{
	const std::string _pk(std::to_string(pk));
	get_c_string(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Name")], reading.name, 64);
	get_float(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Wear")], &reading.fWear);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Seed")], &reading.iSeed);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Kills")], &reading.iKills);
	get_int(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Quality")], &reading.iEntityQuality);
	get_bool(settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("StatTrak")], &reading.bStatTrack);
	auto stickers = &reading.Stickers;
	for (size_t i = 0; i < stickers->size(); ++i)
	{
		const std::string sticker_pos = std::to_string(i);
		auto sticker = &stickers->at(i);
		get_int(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("StickerKit")],
			&sticker->iStickerKitID);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Wear")],
			&sticker->fStickerWear);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Scale")],
			&sticker->fStickerScale);
		get_float(
			settings[XorStr("SkinChanger")][XorStr("Weapons")][def][_pk][XorStr("Stickers")][sticker_pos][XorStr("Rotation")],
			&sticker->fStickerRotation);
	}

	reading.info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(static_cast<const short &>(stoi(def)));
	if (reading.info)
	{
		reading.skin = &reading.info->vSkins.at(0);
		for (auto& it : reading.info->vSkins)
		{
			if (it.iPaintKitID == pk)
			{
				reading.skin = &it;
				break;
			}
		}
	}
}

void config::write_weapon(const std::vector<CustomSkinWeapon>::const_iterator weapon)
{
	const auto def = weapon->info->iItemDefinitionIndex;
	int id;
	if (weapon->skin != nullptr)
		id = weapon->skin->iPaintKitID;
	else
		id = 0;
	const std::string _def(std::to_string(def));
	const std::string _id(std::to_string(id));
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][XorStr("LastPaintKit")] = id;

	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Name")] = weapon->name;
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Wear")] = weapon->fWear;
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Seed")] = weapon->iSeed;
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Kills")] = weapon->iKills;
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("StatTrak")] = weapon->bStatTrack;
	settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Quality")] = weapon->iEntityQuality;
	auto stickers = weapon->Stickers;
	for (size_t i = 0; i < stickers.size(); ++i)
	{
		const std::string sticker_pos = std::to_string(i);
		const auto sticker = stickers.at(i);
		settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Stickers")][sticker_pos][XorStr("StickerKit")] =
			sticker.iStickerKitID;
		settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Stickers")][sticker_pos][XorStr("Wear")] =
			sticker.fStickerWear;
		settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Stickers")][sticker_pos][XorStr("Scale")] =
			sticker.fStickerScale;
		settings[XorStr("SkinChanger")][XorStr("Weapons")][_def][_id][XorStr("Stickers")][sticker_pos][XorStr("Rotation")] =
			sticker.fStickerRotation;
	}
}

void config::read_knife(CustomSkinKnife& knife, const int item_def)
{
	int last_pk;
	std::string def(std::to_string(item_def));
	get_int(settings[XorStr("SkinChanger")][XorStr("Knife")][def][XorStr("LastPaintKit")], &last_pk);
	if (last_pk >= 0)
		read_knife(knife, def, last_pk);
}

void config::read_knife(CustomSkinKnife& knife, std::string& def, const int pk)
{
	const std::string _pk(std::to_string(pk));
	get_c_string(settings[XorStr("SkinChanger")][XorStr("Knife")][def][_pk][XorStr("Name")], knife.name, 64);
	get_float(settings[XorStr("SkinChanger")][XorStr("Knife")][def][_pk][XorStr("Wear")], &knife.fWear);
	get_int(settings[XorStr("SkinChanger")][XorStr("Knife")][def][_pk][XorStr("Seed")], &knife.iSeed);
	get_int(settings[XorStr("SkinChanger")][XorStr("Knife")][def][_pk][XorStr("Kills")], &knife.iKills);
	get_bool(settings[XorStr("SkinChanger")][XorStr("Knife")][def][_pk][XorStr("StatTrak")], &knife.bStatTrack);
	knife.info = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(stoi(def));
	if (knife.info && def != "0")
	{
		knife.skin = &knife.info->vSkins.at(0);
		for (auto it = knife.info->vSkins.begin(); it != knife.info->vSkins.end(); ++it)
		{
			if (*it == pk)
			{
				knife.skin = &*it;
				break;
			}
		}
	}
}

void config::write_knife(const CustomSkinKnife& knife)
{
	const auto def = knife.info->iItemDefinitionIndex;
	int id;
	if (knife.skin != nullptr)
		id = knife.skin->iPaintKitID;
	else
		id = 0;
	const std::string _def(std::to_string(def));
	const std::string _id(std::to_string(id));
	settings[XorStr("SkinChanger")][XorStr("Knife")][XorStr("LastKnife")] = def;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][XorStr("LastPaintKit")] = id;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][_id][XorStr("Name")] = knife.name;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][_id][XorStr("Wear")] = knife.fWear;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][_id][XorStr("Seed")] = knife.iSeed;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][_id][XorStr("Kills")] = knife.iKills;
	settings[XorStr("SkinChanger")][XorStr("Knife")][_def][_id][XorStr("StatTrak")] = knife.bStatTrack;
}

void config::read_wearable(CustomSkinWearable& wearable, const int def)
{
	int last_pk;
	std::string _def(std::to_string(def));
	get_int(settings[XorStr("SkinChanger")][XorStr("Wearable")][_def][XorStr("LastPaintKit")], &last_pk);
	if (last_pk >= 0)
		read_wearable(wearable, _def, last_pk);
}

void config::read_wearable(CustomSkinWearable& wearable, std::string& def, const int pk)
{
	get_float(settings[XorStr("SkinChanger")][XorStr("Wearable")][def][std::to_string(pk)][XorStr("Wear")],
	         &wearable.fWear);
	wearable.info = WearableSkin::GetWearableSkinByItemDefinitionIndex(stoi(def));
	if (wearable.info && def != "0")
	{
		wearable.skin = &wearable.info->vSkins.at(0);
		for (auto it = wearable.info->vSkins.begin(); it != wearable.info->vSkins.end(); ++it)
		{
			if (*it == pk)
			{
				wearable.skin = &*it;
				break;
			}
		}
	}
}

void config::write_wearble(const CustomSkinWearable& wearable)
{
	const auto def = wearable.info->iItemDefinitionIndex;
	int id;
	if (wearable.skin != nullptr)
		id = wearable.skin->iPaintKitID;
	else
		id = 0;
	const std::string _def(std::to_string(def));
	const std::string _id(std::to_string(id));
	settings[XorStr("SkinChanger")][XorStr("Wearable")][XorStr("LastWearable")] = def;
	settings[XorStr("SkinChanger")][XorStr("Wearable")][_def][XorStr("LastPaintKit")] = id;
	settings[XorStr("SkinChanger")][XorStr("Wearable")][_def][_id][XorStr("Wear")] = wearable.fWear;
}

void config::save_t_config(customization& config, std::vector<CustomSkinWeapon>::iterator* overwrite_weapon,
                           const bool knife_only, const bool wearable_only)
{
	write_customization(config, config_t_path, overwrite_weapon, knife_only, wearable_only);
}

void config::save_ct_config(customization& config, std::vector<CustomSkinWeapon>::iterator* overwrite_weapon,
                            const bool knife_only, const bool wearable_only)
{
	write_customization(config, config_ct_path, overwrite_weapon, knife_only, wearable_only);
}

void config::save_old_sounds_config(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds)
{
	write_old_sounds(old_sounds, config_old_sounds_path);
}

void config::load_old_sounds_config(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds)
{
	read_old_sounds(old_sounds, config_old_sounds_path);
}

void config::load_ct_config(customization& config)
{
	read_customization(config, config_ct_path);
}

void config::load_t_config(customization& config)
{
	read_customization(config, config_t_path);
}

void config::load_knife_pk_from_t(customization& config, const int item_def, const int pk)
{
	settings.clear();
	std::ifstream config_doc(config_t_path, std::ifstream::binary);
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors
		return;
	}
	std::string _item_def(std::to_string(item_def));
	read_knife(config.knife, _item_def, pk);
}

void config::load_knife_pk_from_ct(customization& config, const int item_def, const int pk)
{
	settings.clear();
	std::ifstream config_doc(config_ct_path, std::ifstream::binary);
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors
		return;
	}
	std::string _item_def(std::to_string(item_def));
	read_knife(config.knife, _item_def, pk);
}

void config::load_wearable_pk_from_t(customization& config, const int item_def, const int pk)
{
	settings.clear();
	std::ifstream config_doc(config_t_path, std::ifstream::binary);
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors
		return;
	}
	std::string _item_def(std::to_string(item_def));
	read_wearable(config.wearable, _item_def, pk);
}

void config::load_wearable_pk_from_ct(customization& config, const int item_def, const int pk)
{
	settings.clear();
	std::ifstream config_doc(config_ct_path, std::ifstream::binary);
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, config_doc, &settings, &errs))
	{
		//log errors
		return;
	}
	std::string _item_def(std::to_string(item_def));
	read_wearable(config.wearable, _item_def, pk);
}
