// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Options.hpp"

#include <ctime>

#include "Utils.hpp"

namespace Options
{
	//Here we defined the extern variables declared on Options.hpp

	menu_nav menu;
	leaks_nav leaks;

	customization config_t, config_ct;
	std::map<SourceEngine::EItemDefinitionIndex, bool> config_old_sounds = {
		{SourceEngine::EItemDefinitionIndex::weapon_ak47, false},
		{SourceEngine::EItemDefinitionIndex::weapon_aug, false},
		{SourceEngine::EItemDefinitionIndex::weapon_awp, false},
		{SourceEngine::EItemDefinitionIndex::weapon_bizon, false},
		{SourceEngine::EItemDefinitionIndex::weapon_cz75a, false},
		{SourceEngine::EItemDefinitionIndex::weapon_deagle, false},
		{SourceEngine::EItemDefinitionIndex::weapon_elite, false},
		{SourceEngine::EItemDefinitionIndex::weapon_famas, false},
		{SourceEngine::EItemDefinitionIndex::weapon_fiveseven, false},
		{SourceEngine::EItemDefinitionIndex::weapon_g3sg1, false},
		{SourceEngine::EItemDefinitionIndex::weapon_galilar, false},
		{SourceEngine::EItemDefinitionIndex::weapon_glock, false},
		{SourceEngine::EItemDefinitionIndex::weapon_hkp2000, false},
		{SourceEngine::EItemDefinitionIndex::weapon_m249, false},
		{SourceEngine::EItemDefinitionIndex::weapon_m4a1, false},
		{SourceEngine::EItemDefinitionIndex::weapon_m4a1_silencer, false},
		{SourceEngine::EItemDefinitionIndex::weapon_mac10, false},
		{SourceEngine::EItemDefinitionIndex::weapon_mag7, false},
		{SourceEngine::EItemDefinitionIndex::weapon_mp7, false},
		{SourceEngine::EItemDefinitionIndex::weapon_mp9, false},
		{SourceEngine::EItemDefinitionIndex::weapon_negev, false},
		{SourceEngine::EItemDefinitionIndex::weapon_nova, false},
		{SourceEngine::EItemDefinitionIndex::weapon_p250, false},
		{SourceEngine::EItemDefinitionIndex::weapon_p90, false},
		{SourceEngine::EItemDefinitionIndex::weapon_revolver, false},
		{SourceEngine::EItemDefinitionIndex::weapon_sawedoff, false},
		{SourceEngine::EItemDefinitionIndex::weapon_scar20, false},
		{SourceEngine::EItemDefinitionIndex::weapon_sg556, false},
		{SourceEngine::EItemDefinitionIndex::weapon_ssg08, false},
		{SourceEngine::EItemDefinitionIndex::weapon_tec9, false},
		{SourceEngine::EItemDefinitionIndex::weapon_ump, false},
		{SourceEngine::EItemDefinitionIndex::weapon_usp_silencer, false},
		{SourceEngine::EItemDefinitionIndex::weapon_xm1014, false},
	};

	bool sounds_precached = false;

	ButtonCode_t random_skin_key = KEY_F4; 
	ButtonCode_t menu_key = KEY_HOME;
	char clantag_name[32] = "";
	char clantag_tag[32] = "";
	int musickit_index = 16;
	bool enable_random_skin = false;
	bool enable_random_stickers = false;
	bool enable_auto_accept = true;
	bool enable_fix_viewmodel = false;
	bool enable_super_stattrack = false;
	bool is_ready_draw = true;
	bool autosave_if_changed = false;
	const time_t start_time = time(nullptr);
	time_t current_session = 0;
	time_t last_session = 0;
}

void CustomSkinWeapon::RandomSkin(bool random_stickers)
{
	if (!info)
		return;
	int paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);
	if (skin && skin->iPaintKitID == paintkit)
		paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);

	skin = &info->vSkins.at(paintkit);
	iSeed = Utils::RandomInt(1U, 200000U);
	if (random_stickers)
	{
		if (Stickers.empty())
		{
			for (auto i = 0U; i < info->iMaxStickers; ++i)
				Stickers.emplace_back(true);
		}
		else
		{
			for (auto& Sticker : Stickers)
				Sticker.RandomSticker();
		}
	}
}

void CustomSkinWeapon::CustomWeaponSkinSticker::RandomSticker()
{	
	iStickerKitID = g_vStickers.at(Utils::RandomInt(1U, g_vStickers.size() - 1)).iStickerKitID;
	fStickerWear = 0.000001f;
	fStickerScale = 1.0f;
	fStickerRotation = 0.0f;
}

CustomSkinKnife::CustomSkinKnife(): inventory_item(nullptr)
{
}

CustomSkinKnife::CustomSkinKnife(KnifeSkin* _info, SkinInfo_t* _skin): inventory_item(nullptr)
{
	info = _info;
	skin = _skin;
}

CustomSkinKnife::CustomSkinKnife(const size_t& iItemDefinitionIndex, const size_t iSkinIndex): inventory_item(nullptr)
{
	info = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
		skin = &info->vSkins[iSkinIndex];
}

CustomSkinKnife::CustomSkinKnife(const size_t& iItemDefinitionIndex, const int iPaintKitID): inventory_item(nullptr)
{
	info = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
	{
		skin = nullptr;
		for (auto& vSkin : info->vSkins)
		{
			if (vSkin == iPaintKitID)
			{
				skin = &vSkin;
				break;
			}
		}
	}
}

CustomSkinKnife::CustomSkinKnife(const size_t& iItemDefinitionIndex, const std::string& short_skin_name):
	inventory_item(nullptr)
{
	info = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
		skin = Skins::GetSkinByShortName(short_skin_name, info->vSkins);
}

bool CustomSkinKnife::operator==(const int& iItemDefinitionIndex) const
{
	return iItemDefinitionIndex == this->info->iItemDefinitionIndex;
}

void CustomSkinKnife::RandomSkin()
{
	int itemdef = Utils::RandomInt(1U, g_vKnifesSkins.size() - 1);
	if (info && info->iItemDefinitionIndex == itemdef)
		itemdef = Utils::RandomInt(1U, g_vKnifesSkins.size() - 1);
	info = &g_vKnifesSkins.at(itemdef);

	int paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);
	if (skin && skin->iPaintKitID == paintkit)
		paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);
	skin = &info->vSkins.at(paintkit);

	iSeed = Utils::RandomInt(1U, 200000U);
}

void CustomSkinKnife::UpdateModelIndexes()
{
	if (model_indexes.iViewModel == -1)
	{
		const auto modelinfo = SourceEngine::interfaces.ModelInfo();
		model_indexes.iViewModel = modelinfo->GetModelIndex(info->szViewModel.c_str());
		model_indexes.iWorldModel = modelinfo->GetModelIndex(info->szWorldModel.c_str());
		model_indexes.iDroppedModel = modelinfo->GetModelIndex(info->szDroppedModel.c_str());
	}
	//if (model_indexes.iViewModel == -1 && model_indexes.iWorldModel == -1 && model_indexes.iDroppedModel == -1)
	//{
	//	auto def = SourceEngine::interfaces.ItemSchema()->GetItemDefinitionByName("weapon_knife_bat");
	//	INetworkStringTable* model_precache = SourceEngine::interfaces.NetworkStringTableContainer()->FindTable(XorStr("modelprecache"));

	//	SourceEngine::CUtlVector<const char *> strings;
	//	def->GeneratePrecacheModelStrings(false, &strings);
	//	for (int i = 0; i < strings.Count(); ++i)
	//	{
	//		const char* s = strings[i];
	//		const auto model = modelinfo->FindOrLoadModel(s);
	//		if(model)
	//			model_precache->AddString(false, s);
	//	}
	//}
}

CustomSkinWearable::CustomSkinWearable()
= default;

CustomSkinWearable::CustomSkinWearable(WearableSkin* _info, SkinInfo_t* _skin)
{
	info = _info;
	skin = _skin;
}

CustomSkinWearable::CustomSkinWearable(const size_t& iItemDefinitionIndex, const size_t iSkinIndex)
{
	info = WearableSkin::GetWearableSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
		skin = &info->vSkins[iSkinIndex];
}

CustomSkinWearable::CustomSkinWearable(const size_t& iItemDefinitionIndex, const std::string& short_skin_name)
{
	info = WearableSkin::GetWearableSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
		skin = Skins::GetSkinByShortName(short_skin_name, info->vSkins);
}

CustomSkinWearable::CustomSkinWearable(const size_t& iItemDefinitionIndex, const int iPaintKitID)
{
	info = WearableSkin::GetWearableSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
	{
		//skin = &*find(info->vSkins.begin(), info->vSkins.end(), iPaintKitID);
		skin = nullptr;
		for (auto& vSkin : info->vSkins)
		{
			if (vSkin == iPaintKitID)
			{
				skin = &vSkin;
				break;
			}
		}
	}
}

bool CustomSkinWearable::operator==(const int& iItemDefinitionIndex) const
{
	return iItemDefinitionIndex == this->info->iItemDefinitionIndex;
}

void CustomSkinWearable::RandomSkin() const
{
	int itemdef = Utils::RandomInt(1U, g_vWearablesSkins.size() - 1);
	if (info && info->iItemDefinitionIndex == itemdef)
		itemdef = Utils::RandomInt(1U, g_vWearablesSkins.size() - 1);
	info = &g_vWearablesSkins.at(itemdef);

	int paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);
	if (skin && skin->iPaintKitID == paintkit)
		paintkit = Utils::RandomInt(1U, info->vSkins.size() - 1);
	skin = &info->vSkins.at(paintkit);
}

void CustomSkinWearable::Update()
{
	if (model_indexes.iViewModel == -1)
	{
		const auto modelinfo = SourceEngine::interfaces.ModelInfo();
		model_indexes.iViewModel = modelinfo->GetModelIndex(info->szViewModel.c_str());
		model_indexes.iWorldModel = modelinfo->GetModelIndex(info->szWorldModel.c_str());
	}
}

customization::customization(): enable_custom_players(false)
{
	knife = CustomSkinKnife();
	wearable = CustomSkinWearable();
}

customization::customization(const customization& src)
{
	weapons.clear();
	weapons.reserve(src.weapons.size());
	weapons = src.weapons;
	knife = src.knife;
	wearable = src.wearable;
	for (auto& skin : team.skins)
	{
		skin = nullptr;
	}
	enable_custom_players = false;
}

customization& customization::operator=(const customization& src)
{
	if (this == &src)
		return *this;

	weapons.clear();
	weapons.reserve(src.weapons.size());
	weapons = src.weapons;
	knife = src.knife;
	wearable = src.wearable;
	return *this;
}

void customization::randomize_config()
{
	knife.RandomSkin();
	wearable.RandomSkin();
	weapons.clear();

	for(int i = 1; i < 70; ++i)
	{
		if (C_BaseCombatWeapon::GetEconClass(i) != C_BaseCombatWeapon::weapon)
			continue;
		auto& weapon = weapons.emplace_back(i, 0);
		weapon.RandomSkin(false);
	}
}

menu_nav::menu_nav(): current_team(SourceEngine::TEAM_TERRORIST)
{
	menuSize = {458, 430};
}

CustomSkinWeapon::CustomSkinWeapon(WeaponSkin* _info, SkinInfo_t* _skin): inventory_item(nullptr)
{
	info = _info;
	skin = _skin;
	Stickers.reserve(info->iMaxStickers);
	for (size_t i = 0; i < info->iMaxStickers; ++i)
		Stickers.emplace_back(CustomWeaponSkinSticker());
}

CustomSkinWeapon::CustomSkinWeapon(const short& iItemDefinitionIndex, const size_t iSkinIndex): inventory_item(nullptr)
{
	info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
	{
		skin = &info->vSkins[iSkinIndex];
		Stickers.reserve(info->iMaxStickers);
		for (size_t i = 0; i < info->iMaxStickers; ++i)
			Stickers.emplace_back(CustomWeaponSkinSticker());
	}
}

CustomSkinWeapon::CustomSkinWeapon(const short& iItemDefinitionIndex, const int iPaintKitID): inventory_item(nullptr)
{
	info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
	{
		skin = nullptr;
		for (auto& vSkin : info->vSkins)
		{
			if (vSkin == iPaintKitID)
			{
				skin = &vSkin;
				break;
			}
		}

		Stickers.reserve(info->iMaxStickers);
		for (size_t i = 0; i < info->iMaxStickers; ++i)
			Stickers.emplace_back(CustomWeaponSkinSticker());
	}
}

CustomSkinWeapon::CustomSkinWeapon(const short& iItemDefinitionIndex, const std::string& short_skin_name):
	inventory_item(nullptr)
{
	info = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(iItemDefinitionIndex);
	if (info)
	{
		skin = Skins::GetSkinByShortName(short_skin_name, info->vSkins);
		Stickers.reserve(info->iMaxStickers);

		for (size_t i = 0; i < info->iMaxStickers; ++i)
			Stickers.emplace_back(CustomWeaponSkinSticker());
	}
}

bool CustomSkinWeapon::operator==(const int& iItemDefinitionIndex) const
{
	if (this->info)
		return iItemDefinitionIndex == this->info->iItemDefinitionIndex;
	return false;
}

CustomSkinWeapon::CustomWeaponSkinSticker::CustomWeaponSkinSticker(bool random)
{
	if (random)
		RandomSticker();
	else
	{
		iStickerKitID = 0;
		fStickerWear = 0.0f;
		fStickerRotation = 0.0f;
		fStickerScale = 1.0f;
	}
}

CustomSkinWeapon::CustomWeaponSkinSticker::CustomWeaponSkinSticker(const StickerInfo_t& in)
{
	iStickerKitID = in.iStickerKitID;
	fStickerWear = 0.0f;
	fStickerRotation = 0.0f;
	fStickerScale = 1.0f;
}

CustomSkinWeapon::CustomWeaponSkinSticker& CustomSkinWeapon::CustomWeaponSkinSticker::operator=(const StickerInfo_t& in)
{
	iStickerKitID = in.iStickerKitID;
	fStickerWear = 0.0f;
	fStickerRotation = 0.0f;
	fStickerScale = 1.0f;
	return *this;
}
