#pragma once
#include "json/json.h"
#include "Options.hpp"
#include "ImGUI/imgui.h"

void __cdecl save_statistic(void* arg);

class config
{
public:
	config();
	~config();

	config(const config&) = delete;
	config(config&&) = delete;
	config& operator=(const config&) = delete;
	
	void write_other_options();
	void read_other_options();

	void save_t_config(customization& config, std::vector<CustomSkinWeapon>::iterator* overwrite_weapon = nullptr,
	                   bool knife_only = false, bool wearable_only = false);
	void save_ct_config(customization& config, std::vector<CustomSkinWeapon>::iterator* overwrite_weapon = nullptr,
	                    bool knife_only = false, bool wearable_only = false);

	void save_old_sounds_config(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds);
	void load_old_sounds_config(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds);

	void load_t_config(customization& config);
	void load_ct_config(customization& config);

	//void load_weapon_pk_from_t(customization& config, int item_def, int pk);
	//void load_weapon_pk_from_ct(customization& config, int item_def, int pk);
	
	void load_knife_pk_from_t(customization& config, int item_def, int pk);
	void load_knife_pk_from_ct(customization& config, int item_def, int pk);

	void load_wearable_pk_from_t(customization& config, int item_def, int pk);
	void load_wearable_pk_from_ct(customization& config, int item_def, int pk);

private:

	Json::Value settings;

	Json::StreamWriterBuilder builder;
	Json::StreamWriter* writer;

	void write_customization(customization& config, const std::string& filename,
	                         std::vector<CustomSkinWeapon>::iterator* overwrite_weapon = nullptr, bool knife_only = false,
	                         bool wearable_only = false);
	void read_customization(customization& config, const std::string& filename);

	void write_old_sounds(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds, const std::string& filename);
	void read_old_sounds(std::map<SourceEngine::EItemDefinitionIndex, bool>& old_sounds, const std::string& filename);

	void write_weapon(std::vector<CustomSkinWeapon>::const_iterator weapon);
	void write_knife(const CustomSkinKnife& knife);
	void write_wearble(const CustomSkinWearable& wearable);
	void write_custom_players(const CustomPlayerSkin& team);

	void read_weapon(std::vector<CustomSkinWeapon>& weapons, int item_def);
	void read_weapon(CustomSkinWeapon& reading, std::string& def, int pk);
	void read_weapon(std::vector<CustomSkinWeapon>::value_type* reading, std::string& def, const int pk);
	void read_weapon(std::vector<CustomSkinWeapon>::iterator reading, std::string& def, int pk);
	void read_knife(CustomSkinKnife& knife, int item_def);
	void read_knife(CustomSkinKnife& knife, std::string& def, int pk);
	void read_wearable(CustomSkinWearable& wearable, int item_def);
	void read_wearable(CustomSkinWearable& wearable, std::string& item_def, int pk);

	bool read_custom_players(CustomPlayerSkin& team);
	mutable std::string config_t_path;
	mutable std::string config_ct_path;
	mutable std::string config_other_path;
	mutable std::string config_old_sounds_path;
};

static void get_time(const Json::Value& config, time_t* setting)
{
	if (config.isNull())
		return;

	*setting = config.asInt64();
}

static void get_bool(const Json::Value& config, bool* setting)
{
	if (config.isNull())
		return;

	*setting = config.asBool();
}

static void get_c_string(const Json::Value& config, char** setting)
{
	if (config.isNull())
		return;

	*setting = _strdup(config.asCString());
}

static void get_c_string(const Json::Value& config, char* setting, size_t size)
{
	if (config.isNull())
		return;

	strcpy(setting, config.asCString());
}

static void get_string(const Json::Value& config, std::string* setting)
{
	if (config.isNull())
		return;

	*setting = config.asString();
}

template <typename Type>
static void get_int(const Json::Value& config, Type* setting)
{
	if (config.isNull())
		return;

	*setting = static_cast<Type>(config.asInt());
}

template <typename Type>
static void get_unsigned_int(const Json::Value& config, Type* setting)
{
	if (config.isNull())
		return;

	*setting = static_cast<Type>(config.asUInt());
}

static void get_float(const Json::Value& config, float* setting)
{
	if (config.isNull())
		return;

	*setting = config.asFloat();
}

static void GetColor(Json::Value& config, ImColor* setting)
{
	get_float(config["r"], &setting->Value.x);
	get_float(config["g"], &setting->Value.y);
	get_float(config["b"], &setting->Value.z);
	get_float(config["a"], &setting->Value.w);
}

static void LoadColor(Json::Value& config, const ImColor& color)
{
	config["r"] = color.Value.x;
	config["g"] = color.Value.y;
	config["b"] = color.Value.z;
	config["a"] = color.Value.w;
}
