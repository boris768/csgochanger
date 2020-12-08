#pragma once
#include "CSGOStructs.hpp"
#include "Skins.h"
#include "ImGUI/imgui.h"

constexpr auto MAX_STICKERS = 6;

enum AimTargetType : int
{
	FOV,
	DISTANCE,
	HP
};

struct menu_nav
{
	int menu_current_team_buffer{};
	SourceEngine::TeamID current_team;
	ImVec2 menuSize;
	ImGuiTextFilter sticker_filter;
	struct _weapon
	{
		size_t current_weapon;
		size_t current_weapon_skin;
		size_t current_knife;
		size_t current_knife_skin;
		size_t current_wearable;
		size_t current_wearable_skin;
		size_t current_t_team_skin[5];
		size_t current_ct_team_skin[5];
		size_t current_sky;
	}
	weapon{};

	char filter[40]{};
	bool main_window{};
	bool skin_editor_window{};
	bool was_opened{};
	bool first_launch{};

	menu_nav();
};

struct stat_nav
{
	bool main_window;
};

struct leaks_nav
{
	char profile_link[260];
	bool main_window;
};

class CustomSkinWeapon
{
public:
	CustomSkinWeapon(WeaponSkin* _info, SkinInfo_t* _skin);

	CustomSkinWeapon(const short& iItemDefinitionIndex, const size_t iSkinIndex);

	CustomSkinWeapon(const short& iItemDefinitionIndex, const int iPaintKitID);

	CustomSkinWeapon(const short& iItemDefinitionIndex, const std::string& short_skin_name);

	bool operator==(const int& iItemDefinitionIndex) const;

	void RandomSkin(bool random_stickers);
	mutable WeaponSkin* info = nullptr;
	mutable SkinInfo_t* skin = nullptr;

	struct CustomWeaponSkinSticker
	{
		size_t iStickerKitID;
		float fStickerWear;
		float fStickerScale;
		float fStickerRotation;

		CustomWeaponSkinSticker(bool random = false);

		CustomWeaponSkinSticker(const StickerInfo_t& in);

		CustomWeaponSkinSticker& operator=(const StickerInfo_t& in);
		void RandomSticker();
	};

	std::vector<CustomWeaponSkinSticker> Stickers;
	SourceEngine::CEconItem* inventory_item;
	int iEntityQuality = 0;
	char name[64] = { '\0' };
	mutable int iKills = 0;
	int iSeed = 0;
	mutable float fWear = 0.f;
	bool bStatTrack = false;
	bool bWearAsFloat = false;
};

class CustomSkinKnife
{
public:
	CustomSkinKnife();

	CustomSkinKnife(KnifeSkin* _info, SkinInfo_t* _skin);

	CustomSkinKnife(const size_t& iItemDefinitionIndex, const size_t iSkinIndex);

	CustomSkinKnife(const size_t& iItemDefinitionIndex, const int iPaintKitID);

	CustomSkinKnife(const size_t& iItemDefinitionIndex, const std::string& short_skin_name);

	bool operator==(const int& iItemDefinitionIndex) const;

	void RandomSkin();
	mutable KnifeSkin* info{};
	mutable SkinInfo_t* skin{};

	struct ModelIndexes
	{
		int iViewModel = -1;
		int iWorldModel = -1;
		int iDroppedModel = -1;
	}
	model_indexes;

	void UpdateModelIndexes();
	SourceEngine::CEconItem* inventory_item;
	char name[64] = { '\0' };
	int iKills = 0;
	int iSeed = 0;
	float fWear = 0.f;
	bool bStatTrack = false;
	bool bWearAsFloat = false;
};

class CustomSkinWearable
{
public:
	CustomSkinWearable();

	CustomSkinWearable(WearableSkin* _info, SkinInfo_t* _skin);

	CustomSkinWearable(const size_t& iItemDefinitionIndex, const size_t iSkinIndex);

	CustomSkinWearable(const size_t& iItemDefinitionIndex, const std::string& short_skin_name);

	CustomSkinWearable(const size_t& iItemDefinitionIndex, const int iPaintKitID);

	bool operator==(const int& iItemDefinitionIndex) const;

	void RandomSkin() const;

	mutable WearableSkin* info{};
	mutable SkinInfo_t* skin{};

	SourceEngine::CEconItem* inventory_item = nullptr;
	struct ModelIndexes
	{
		int iViewModel;
		int iWorldModel;
	}
	model_indexes{};

	void Update();
	int iSeed = 0;
	char name[64]{};
	float fWear = 0.f;
};

struct CustomPlayerSkin
{
	PlayerInfo_t* skins[5] {nullptr, nullptr, nullptr, nullptr, nullptr };
};

struct customization
{
	customization();

	customization(const customization& src);

	customization& operator=(const customization& src);
	void randomize_config();

	std::vector<CustomSkinWeapon> weapons;
	CustomSkinKnife knife;
	CustomSkinWearable wearable;
	CustomPlayerSkin team;
	bool enable_custom_players;
};

namespace Options
{
	extern customization config_t, config_ct;
	extern std::map<SourceEngine::EItemDefinitionIndex, bool> config_old_sounds;

	extern bool enable_fix_viewmodel;
	extern bool enable_auto_accept;
	extern bool enable_super_stattrack;
	extern bool enable_random_skin;
	extern bool enable_random_stickers;
	extern bool is_ready_draw;
	extern bool autosave_if_changed;
	extern bool sounds_precached;
	extern ButtonCode_t random_skin_key;
	extern ButtonCode_t menu_key;
	extern int musickit_index;
	extern menu_nav menu;
	extern leaks_nav leaks;
	extern std::string esp_config_path;
	extern std::string aim_config_path;
	extern char clantag_name[32];
	extern char clantag_tag[32];
	extern const time_t start_time;
	extern time_t current_session;
	extern time_t last_session;
	//Add others as needed. 
	//Remember they must be defined on Options.cpp as well
}
