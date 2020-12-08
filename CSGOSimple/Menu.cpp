// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Menu.h"
#include "Options.hpp"
#include "XorStr.hpp"
#include "Hooks.hpp"
#include "float_functions.h"
#include "ImGUI/imgui_internal.h"
#include "Utils.hpp"
#include "CustomWinAPI.h"
#include <filesystem>
//#define _PREMIUM_VERSION

std::vector<const char*>  teams = {
	"Terrorists",
	"Counter-Terrorists"
};

std::vector<const char*> sky_names = {
    "default",
    "cs_baggage_skybox_",
    "cs_tibet",
    "embassy",
    "italy",
    "jungle",
    "nukeblank",
    "office",
    "sky_cs15_daylight01_hdr",
    "sky_cs15_daylight02_hdr",
    "sky_cs15_daylight03_hdr",
    "sky_cs15_daylight04_hdr",
    "sky_csgo_cloudy01",
    "sky_csgo_night02",
    "sky_csgo_night02b",
    "sky_dust",
    "sky_venice",
    "vertigo",
    "vertigoblue_hdr",
    "vietnam",
	"sky_descent"
};

struct _rarity_colors
{
	ImVec4 _default;
	ImVec4 _common;
	ImVec4 _uncommon;
	ImVec4 _rare;
	ImVec4 _mythical;
	ImVec4 _legendary;
	ImVec4 _ancient;
	ImVec4 _immortal;
	constexpr _rarity_colors() noexcept
	{
		_default = ImColor(106, 97, 86).operator ImVec4();
		_common = ImColor(176, 195, 217).operator ImVec4();
		_uncommon = ImColor(94, 152, 217).operator ImVec4();
		_rare = ImColor(75, 105, 255).operator ImVec4();
		_mythical = ImColor(136, 71, 255).operator ImVec4();
		_legendary = ImColor(211, 44, 230).operator ImVec4();
		_ancient = ImColor(235, 75, 75).operator ImVec4();
		_immortal = ImColor(228, 174, 57).operator ImVec4();
	}
};
constexpr _rarity_colors rarity_colors;

const ImVec4& GetRarityColor(int rarity) noexcept
{
	switch (rarity)
	{
	case SourceEngine::ITEM_RARITY_DEFAULT:
		return rarity_colors._default;
	case 	SourceEngine::ITEM_RARITY_COMMON:
		return rarity_colors._common;
	case 	SourceEngine::ITEM_RARITY_UNCOMMON:
		return rarity_colors._uncommon;
	case 	SourceEngine::ITEM_RARITY_RARE:
		return rarity_colors._rare;
	case 	SourceEngine::ITEM_RARITY_MYTHICAL:
		return rarity_colors._mythical;
	case 	SourceEngine::ITEM_RARITY_LEGENDARY:
		return rarity_colors._legendary;
	case 	SourceEngine::ITEM_RARITY_ANCIENT:
		return rarity_colors._ancient;
	case 	SourceEngine::ITEM_RARITY_IMMORTAL:
		return rarity_colors._immortal;
	default:
		return rarity_colors._default;
	}
}

using LoadNamedSky_t = void(__fastcall*)(const char*);
LoadNamedSky_t load_named_sky = nullptr;
char currentloadedsky[128] = "";
void ChangeSky(const unsigned id)
{
    if (!load_named_sky)
    {
        load_named_sky = reinterpret_cast<LoadNamedSky_t>(Utils::FindSignature(
            SourceEngine::memory.engine(), XorStr("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")));
        if (!load_named_sky)
            return; //ERROR, signature outdated
    }
	if ( id > sky_names.size())
	{
		const auto default_sky = static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("sv_skyname")))->GetString();
		if (default_sky)
		{
			if (strcmp(default_sky, currentloadedsky) == 0)
				return;
			load_named_sky(default_sky);
		}
	}
	else
	{
		if (strcmp(sky_names.at(id), currentloadedsky) == 0)
			return;
		if (id == 0)
		{
			const auto default_sky = static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("sv_skyname")))->GetString();
			if (default_sky)
			{
				load_named_sky(default_sky);
				strcpy_s(currentloadedsky, 128, default_sky);
			}
			return;
		}
		static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("r_3dsky")))->SetFlags(FCVAR_NONE);
		static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("r_3dsky")))->SetValue(0);
		load_named_sky(sky_names.at(id));
		strcpy_s(currentloadedsky, 128, sky_names.at(id));
	}
}


char skin_id_buffer[40];

void DrawSkinEditor()
{
    Options::menu.skin_editor_window = true;
    ImGui::Begin("SkinEditor", &Options::menu.skin_editor_window, ImVec2(500, 400), 0.9f,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    ImGui::InputText("PaintKitID", skin_id_buffer, 40);

    if (strlen(skin_id_buffer))
    {
        SourceEngine::CCStrike15ItemSchema* item_schema = SourceEngine::interfaces.ItemSchema();
        SourceEngine::CPaintKit* paint_kit = item_schema->GetPaintKitDefinitionByName(skin_id_buffer);
        if (paint_kit)
        {
            ImGui::InputInt("Rarity", &paint_kit->rarity);

            ImGui::ColorEdit4("1", paint_kit->color[0]);
            ImGui::ColorEdit4("2", paint_kit->color[1]);
            ImGui::ColorEdit4("3", paint_kit->color[2]);
            ImGui::ColorEdit4("4", paint_kit->color[3]);

            ImGui::InputFloat("scale", &paint_kit->pattern_scale);
            ImGui::InputFloat("offset_x_start", &paint_kit->pattern_offset_x_start);
            ImGui::InputFloat("offset_x_end", &paint_kit->pattern_offset_x_end);
            ImGui::InputFloat("offset_y_start", &paint_kit->pattern_offset_y_start);
            ImGui::InputFloat("offset_y_end", &paint_kit->pattern_offset_y_end);
            ImGui::InputFloat("rotate_start", &paint_kit->pattern_rotate_start);
            ImGui::InputFloat("rotate_end", &paint_kit->pattern_rotate_end);
        }
    }

    /*
    __int32 PaintKitID; //0x0000
    ::CUtlString name; //0x0004
    ::CUtlString description_string; //0x0014
    ::CUtlString description_tag; //0x0024
    ::CUtlString pattern; //0x0034
    ::CUtlString normal; //0x0044
    ::CUtlString logo_material; //0x0044
    __int32 unk1; //0x0054
    __int32 rarity; //0x0058
    __int32 style; //0x005C
    __int8 color[4][4]; //0x0060
    __int8 logocolor[4][4]; //0x0070
    float wear_default; //0x0080
    float wear_remap_min; //0x0084
    float wear_remap_max; //0x0088
    __int8 seed; //0x008C
    __int8 phongexponent; //0x008D
    __int8 phongalbedoboost; //0x008E
    __int8 phongintensity; //0x008F
    float pattern_scale; //0x0090
    float pattern_offset_x_start; //0x0094
    float pattern_offset_x_end; //0x0098
    float pattern_offset_y_start; //0x009C
    float pattern_offset_y_end; //0x00A0
    float pattern_rotate_start; //0x00A4
    float pattern_rotate_end; //0x00A8
    float logo_scale; //0x00AC
    float logo_offset_x; //0x00B0
    float logo_offset_y; //0x00B4
    float logo_rotation; //0x00B8
    __int8 ignore_weapon_size_scale; //0x00BC
    //bool pad3;
    //bool pad4;
    //bool pad5;
    __int32 view_model_exponent_override_size; //0x00C0
    bool only_first_material; //0x00C4
    bool use_normal;
    ::CUtlString vmt_path; //0x00C8
    KeyValues* vmt_overrides; //0x00D8
    *
    */

    ImGui::End();
}

void DrawCSGOLeaks()
{
    ImGui::Begin(XorStr("CS:GO Leaks"), &Options::leaks.main_window, ImVec2(500, 400), 1.0f,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    sprintf(Options::leaks.profile_link, "https://stat.cschanger.com/profile/%llu", SourceEngine::interfaces.Engine()->GetSteamAPIContext()->m_pSteamUser->GetSteamID().m_unAll64Bits);
    ImGui::InputText(XorStr("Link to profile"), Options::leaks.profile_link, 260, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::Button(XorStr("Open")))
    {
        ShellExecuteA(nullptr, "open", Options::leaks.profile_link, nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::End();
}

void sendClanTag(const char* name, const char* tag)
{
	auto kv = new SourceEngine::KeyValues("ClanTagChanged");
    kv->SetString("name", name);
    kv->SetString("tag", tag);
    SourceEngine::interfaces.Engine()->ServerCmdKeyValues(kv);
    //seems, ServerCmdKeyValues deletes kv, no need delete in manualy
}

char new_name[32];
char new_musicid[32];

void DrawShit()
{
    ImGui::InputText("name chager", new_name, 32);
    //if (ImGui::Button("accept name"))
    //{
    //    SourceEngine::ConVar* name = static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("name")));
    //    name->m_fnChangeCallbacks.RemoveAll();

    //    name->SetValue(u8"\n\xAD\xAD\xAD");;
    //    name->SetValue(new_name);
    //}
	//if (ImGui::Button("Banned"))
	//{
	//	SourceEngine::ConVar* name = static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("name")));
	//	name->m_fnChangeCallbacks.RemoveAll();

	//	name->SetValue(u8"\n\xAD\xAD\xAD");;
	//	name->SetValue(u8" \x01\x0B\x07 Boris768 has been permanently banned from official CS:GO servers. \x01");
	//}

}
//ImGui::file_browser fileDialog;

volatile bool should_save_config = false;
volatile bool should_exit_thread = false;
HANDLE save_thread_handle = nullptr;
unsigned __cdecl save_thread(void*)
{
	bool should_save_other = false;
	while (!should_exit_thread)
	{
		Options::current_session = time(nullptr) - Options::start_time;
		if (should_save_config)
		{
			Hooks::g_pConfig->save_ct_config(Options::config_ct);
			Hooks::g_pConfig->save_t_config(Options::config_t);
			Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
			Hooks::g_pConfig->write_other_options();
			should_save_config = false;
		}
		if(should_save_other)
		{
			Hooks::g_pConfig->write_other_options();
			should_save_other = false;
		}
		g_pWinApi->dwSwitchToThread();
		g_pWinApi->dwSleep(5000);
		should_save_other = true;
	}
	return 0;
}

volatile bool should_read_config = false;
HANDLE load_thread_handle = nullptr;
unsigned __cdecl read_thread (void*)
{
	while (!should_exit_thread)
	{
		if (should_read_config)
		{
			Hooks::g_pConfig->load_ct_config(Options::config_ct);
			Hooks::g_pConfig->load_t_config(Options::config_t);
			Hooks::g_pConfig->load_old_sounds_config(Options::config_old_sounds);
			Hooks::g_pConfig->read_other_options();
			should_read_config = false;
		}		
		g_pWinApi->dwSwitchToThread();
		g_pWinApi->dwSleep(300);
	}
	return 0;
}
bool need_forceupdate_for_weapon = false;
bool need_forceupdate_for_knife = false;
volatile bool need_forceupdate_for_wearable = false;

void DrawMenu()
{
    //DrawSkinEditor();

    //Options::menu.leaks_window = true;
    //DrawCSGOLeaks();

#ifdef _DEBUG
	DrawShit();
#endif
    if (Options::menu.main_window)
    {
        customization* current_config;
		auto engine = SourceEngine::interfaces.Engine();
        if (!Options::menu.was_opened)
        {
			if (engine->IsInGame())
			{
				C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
				if (pLocal)
				{
					Options::menu.current_team = pLocal->GetTeamNum();
					//this need saves us from nullptrs
					C_BaseCombatWeapon* combat_weapon = pLocal->GetActiveWeapon();

					if (combat_weapon)
					{
						short* itemdefinitionindex = combat_weapon->GetItemDefinitionIndex();
						if (itemdefinitionindex)
						{
							const auto item_definition_index = *itemdefinitionindex;
							if (item_definition_index)
							{
								Options::menu.weapon.current_weapon = static_cast<size_t>(WeaponSkin::iGetWeaponIndexByItemDefinitionIndex(item_definition_index));
								if (!Options::menu.weapon.current_weapon ||
									Options::menu.weapon.current_weapon == size_t(-1))
								{
									Options::menu.weapon.current_weapon = 0;
								}
							}
						}
					}
				}
			}

			if (Options::menu.current_team > SourceEngine::TeamID::TEAM_TERRORIST)
			{
				Options::menu.menu_current_team_buffer = static_cast<int>(Options::menu.current_team) - 2;
			}
			else
			{
				Options::menu.menu_current_team_buffer = 0;
				Options::menu.current_team = SourceEngine::TeamID::TEAM_TERRORIST;
			}

            Options::menu.was_opened = !Options::menu.was_opened;
        }
        //inverted logic, i dont want useless constructors
        if (!Options::menu.first_launch)
        {
            int w = 0, h = 0;
            const ImVec2 size = Options::menu.menuSize;
            engine->GetScreenSize(w, h);
            ImGui::SetNextWindowPos({ static_cast<float>(w) / 2.f - size.x / 2, static_cast<float>(h) / 2.f - size.y / 2});
            ImGui::GetStyle().WindowMinSize = size;
            ImGui::SetNextWindowSize(size);
            Options::menu.first_launch = true;
        }
        //Begin Main window code
        //const auto string = XorStr(" CS:GO Changer | vk.com/csgo_changer ");
#ifndef _PREMIUM_VERSION
        const auto child_name = XorStr("menu_elements");
#endif
        ImGui::Begin(XorStr(" CS:GO Changer | vk.com/csgo_changer "), &Options::menu.main_window, {0,0}, 0.9f,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
        {
#ifndef _PREMIUM_VERSION
            //if (Hooks::top_banner)
            //    Hooks::top_banner->Render();

            //if (Hooks::bottom_banner)
            //{
            //    const ImVec2 cur_size = ImGui::GetWindowSize();
            //    if (!Hooks::top_banner)
            //        ImGui::BeginChild(child_name, {cur_size.x - 33, cur_size.y - 123});
            //    else
            //        ImGui::BeginChild(child_name, {cur_size.x - 33, cur_size.y - 203});
            //}
            //else if (Hooks::top_banner)
            //{
            //    const ImVec2 cur_size = ImGui::GetWindowSize();
            //    ImGui::BeginChild(child_name, {cur_size.x - 33, cur_size.y - 123});
            //}
#endif
            //ImGui::InputText("musickit", new_musicid, 32, ImGuiInputTextFlags_CharsDecimal);
            //if(ImGui::Button("set music"))
            //{

            //}

			constexpr ImGuiComboFlags flags = 0;
			const char* current_team = teams[Options::menu.menu_current_team_buffer];
			ImGui::PushID(FNV("teams_select" __DATE__ __TIME__));
			if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Team).c_str(), current_team, flags))
			{
				for (auto& element : teams)
				{
					const bool is_selected = current_team == element;
					if (ImGui::Selectable(element, is_selected))
					{
						Options::menu.menu_current_team_buffer = element[0] == 'T' ? 0 : 1;
						current_team = teams[Options::menu.menu_current_team_buffer];
						Options::menu.current_team = static_cast<SourceEngine::TeamID>(Options::menu.menu_current_team_buffer + 2); //shift id 
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();

			//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Team).c_str(), &Options::menu.menu_current_team_buffer, teams))
			//{
			//    Options::menu.current_team = static_cast<SourceEngine::TeamID>(Options::menu.menu_current_team_buffer + 2); //shift id 
			//}
			switch (Options::menu.current_team)
			{
			case SourceEngine::TeamID::TEAM_TERRORIST:
				current_config = &Options::config_t;
				break;
			case SourceEngine::TeamID::TEAM_COUNTER_TERRORIST:
				current_config = &Options::config_ct;
				break;
			default:
				current_config = nullptr;
			}
			if (current_config)
			{
				if (ImGui::CollapsingHeader(Hooks::localized_strs->at(SkinChanger_Weapons).c_str()))
				{
					ImGui::PushID(FNV("default_weapons" __TIME__ __DATE__));

					//ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Weapons).c_str(), &Options::menu.weapon.current_weapon, g_vWeaponSkins);
					const auto current_ws_element = &g_vWeaponSkins.at(Options::menu.weapon.current_weapon);
					const auto current_weapon = current_ws_element->szWeaponNameAsU8.c_str();
					if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Weapons).c_str(), current_weapon, flags))
					{
						for (size_t i = 0; i < g_vWeaponSkins.size(); ++i)
						{
							ImGui::PushID(i);
							const bool is_selected = Options::menu.weapon.current_weapon == i;
							const auto weapon = g_vWeaponSkins.at(i).szWeaponNameAsU8.c_str();
							if (ImGui::Selectable(weapon, is_selected))
							{
								Options::menu.weapon.current_weapon = i;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
							ImGui::PopID();
						}
						ImGui::EndCombo();
					}

					std::vector<CustomSkinWeapon>* weapons_optim = &current_config->weapons;
					auto curr_itemdefindex = current_ws_element->iItemDefinitionIndex;
					const auto end = weapons_optim->end();
					std::vector<CustomSkinWeapon>::iterator config_weapon = end;
					for (auto it = weapons_optim->begin(); it != weapons_optim->end(); ++it)
					{
						if (*it == curr_itemdefindex)
						{
							config_weapon = it;
							break;
						}
					}
					if (config_weapon == end)
					{
						//Options::menu.weapon.current_weapon = 0;
						//try
						//{
						//	curr_itemdefindex = g_vWeaponSkins.at(Options::menu.weapon.current_weapon).iItemDefinitionIndex;
						//	config_weapon = find(
						//		weapons_optim->begin(),
						//		weapons_optim->end(),
						//		curr_itemdefindex);
						//}
						//catch (std::exception&)
						//{
						//	Options::menu.weapon.current_weapon = 0;
						//	config_weapon = weapons_optim->end();
						//}
						weapons_optim->emplace_back(curr_itemdefindex, 0);
						config_weapon = --weapons_optim->end();
						Options::menu.weapon.current_weapon_skin = 0;
					}
					else
						Options::menu.weapon.current_weapon_skin = Skins::GetSkinIndexBySkinInfo_t(
							config_weapon->skin, g_vWeaponSkins.at(Options::menu.weapon.current_weapon).vSkins);

					/* Skins */
					auto weapon_skins = &g_vWeaponSkins.at(Options::menu.weapon.current_weapon).vSkins;
					const auto current_weapon_skin = weapon_skins->at(Options::menu.weapon.current_weapon_skin).szSkinNameAsU8.c_str();
					if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), current_weapon_skin, flags))
					{
						for (size_t i = 0; i < weapon_skins->size(); ++i)
						{
							ImGui::PushID(i);
							const bool is_selected = Options::menu.weapon.current_weapon_skin == i;
							auto& skin = weapon_skins->at(i);
							const auto weapon = skin.szSkinNameAsU8.c_str();
							if (skin.m_pPaintKitDefinition)
								ImGui::PushStyleColor(ImGuiCol_Text,
									GetRarityColor(Utils::CombinedItemAndPaintRarity(g_vWeaponSkins.at(Options::menu.weapon.current_weapon).game_item_definition->item_rarity, skin.m_pPaintKitDefinition->rarity)));

							if (ImGui::Selectable(weapon, is_selected))
							{
								Options::menu.weapon.current_weapon_skin = i;
								need_forceupdate_for_weapon = true;
								config_weapon->skin = &g_vWeaponSkins.at(Options::menu.weapon.current_weapon).vSkins.at(
									Options::menu.weapon.current_weapon_skin);
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();

							if (skin.m_pPaintKitDefinition)
								ImGui::PopStyleColor();
							//if (weapon_skins->at(i).m_pPaintKitDefinition && weapon_skins->at(i).m_pPaintKitDefinition->rarity == 6)
							//	ImGui::PopStyleColor();
							ImGui::PopID();
						}
						ImGui::EndCombo();
					}
					//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), &Options::menu.weapon.current_weapon_skin,
					//                 g_vWeaponSkins.at(Options::menu.weapon.current_weapon).vSkins))
					//{
					//    need_forceupdate_for_weapon = true;
					//    config_weapon->skin = &g_vWeaponSkins.at(Options::menu.weapon.current_weapon).vSkins.at(
					//        Options::menu.weapon.current_weapon_skin);
					//}

					/* Name */
					ImGui::InputText(Hooks::localized_strs->at(SkinChanger_Name).c_str(), config_weapon->name, 64);
					need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

					/* Wear */
					ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Wear).c_str(), &config_weapon->fWear, 0.000001f, 1.0f, "%.6f");
					need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

					if (float_functions::is_equal(config_weapon->fWear, 0.0f))
					{
						config_weapon->fWear = 0.000001f;
					}

					/* Seed */
					ImGui::InputInt(Hooks::localized_strs->at(SkinChanger_Seed).c_str(), &config_weapon->iSeed);
					need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

					/* StatTrack */
					need_forceupdate_for_weapon |= ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_StatTrak).c_str(), &config_weapon->bStatTrack);
					if (config_weapon->bStatTrack)
					{
						/* Kills */
						ImGui::InputInt(Hooks::localized_strs->at(SkinChanger_Kills).c_str(), &config_weapon->iKills);
						need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();
					}
					else
					{
						/* Quality */
						ImGui::SliderInt(Hooks::localized_strs->at(SkinChanger_Quality).c_str(), &config_weapon->iEntityQuality, 0, 12);
						need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();
					}
					ImGui::PushID(FNV("Stickers" __TIME__ __DATE__ __TIME__ __TIME__));
					for (size_t i = 0; i < config_weapon->Stickers.size(); ++i)
					{
						ImGui::PushID(i);

						/* Sticker slot */
						if (ImGui::TreeNode(Hooks::localized_strs->at(SkinChanger_Sticker_slot).c_str()))
						{
							CustomSkinWeapon::CustomWeaponSkinSticker* current_sticker = &config_weapon->Stickers.at(i);
							Options::menu.sticker_filter.Draw(Hooks::localized_strs->at(SkinChanger_Sticker_filter).c_str());

							auto current_sticker_in_base = StickerInfo_t::GetSticker(current_sticker->iStickerKitID);
							if(!current_sticker_in_base)
							{
								Hooks::changer_log->msg(XorStr("Invalid sticker id was deleted!\n"));
								current_sticker_in_base = &g_vStickers.at(0);
								current_sticker->iStickerKitID = 0;
							}

							ImGui::PushID(current_sticker_in_base->szStickerNameAsU8.c_str());
							if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Stickers).c_str(), current_sticker_in_base->szStickerNameAsU8.c_str(), flags))
							{
								const StickerInfo_t* stickers_vec_end = g_vStickers.data() + g_vStickers.size();
								for (StickerInfo_t* it = g_vStickers.data(); it != stickers_vec_end; ++it)
								{
									const auto weapon = it->szStickerNameAsU8.c_str();
									if (!Options::menu.sticker_filter.PassFilter(weapon))
									{
										continue;
									}

									//if (wfilter[0] != L'\0' && it->szStickerName.find(wfilter) == std::wstring::npos)
									//	continue;

									ImGui::PushID(it->iStickerKitID);
									const bool is_selected = it == current_sticker_in_base;
									if (ImGui::Selectable(weapon, is_selected))
									{
										current_sticker->iStickerKitID = it->iStickerKitID;
										need_forceupdate_for_weapon = true;
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
									ImGui::PopID();
								}
								
								ImGui::EndCombo();
							}
							
							//need_forceupdate_for_weapon |= ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Stickers).c_str(), &current_sticker->iStickerKitID, g_vStickers, Options::menu.filter);

							/* Wear */
							ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Wear).c_str(), &current_sticker->fStickerWear, 0.000001f, 1.0f);
							need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

							/* Scale */
							ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Scale).c_str(), &current_sticker->fStickerScale, 0.0f, 5.0f);
							need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

							/* Rotation */
							ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Rotation).c_str(), &current_sticker->fStickerRotation, -179.0f, 180.0f);
							need_forceupdate_for_weapon |= ImGui::IsItemDeactivatedAfterEdit();

							/* Reset */
							if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Reset).c_str()))
							{
								current_sticker->fStickerWear = 0.000001f;
								current_sticker->fStickerScale = 1.0f;
								current_sticker->fStickerRotation = 0.0f;
								need_forceupdate_for_weapon = true;
							}
							ImGui::PopID();
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::PopID();

					if (need_forceupdate_for_weapon)
					{
						SourceEngine::interfaces.ClientState()->ForceFullUpdate();
						//Hooks::g_fnCleanInventoryImageCacheDir();
						//Protobuffs::SendClientHello();
						//Protobuffs::SendMatchmakingClient2GCHello();
#ifdef _PREMIUM_VERSION
						if (config_weapon->skin)
						{
							const auto avalTeam = GetAvailableClassID(config_weapon->info->iItemDefinitionIndex);
							if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == Options::menu.current_team)
							{
								UpdateItem(Options::menu.current_team, config_weapon->inventory_item, config_weapon);
							}
						}
#endif
						/*if (avalTeam == SourceEngine::TeamID::TEAM_SPECTATOR || avalTeam == SourceEngine::TeamID::TEAM_COUNTER_TERRORIST)
							InjectItem(
								SourceEngine::TeamID::TEAM_COUNTER_TERRORIST,
								config_weapon->info->iItemDefinitionIndex,
								config_weapon->skin->iPaintKitID,
								config_weapon->name,
								config_weapon->name[0] != 0,
								config_weapon->bStatTrack,
								config_weapon->iKills,
								config_weapon->iSeed,
								config_weapon->fWear,
								static_cast<SourceEngine::ItemQuality>(config_weapon->iEntityQuality)
							);*/
						
						if (Options::autosave_if_changed)
						{
							should_save_config = true;
						}
						need_forceupdate_for_weapon = false;
					}
					ImGui::PopID();
				}

				/* Knifes */
				if (ImGui::CollapsingHeader(Hooks::localized_strs->at(SkinChanger_Knifes).c_str()))
				{
					ImGui::PushID(FNV("default_knifes" __TIME__ __DATE__));
					if (current_config->knife.info == nullptr)
					{
						current_config->knife.info = &g_vKnifesSkins.at(0);
						Options::menu.weapon.current_knife = 0;
					}
					else
					{
						Options::menu.weapon.current_knife = KnifeSkin::iGetKnifeIndexByItemDefinitionIndex(
							current_config->knife.info->iItemDefinitionIndex);
					}
					/* Knifes */
					const auto current_knife = g_vKnifesSkins.at(Options::menu.weapon.current_knife).szWeaponNameAsU8.c_str();
					ImGui::PushID(FNV("knife" __TIME__ __DATE__ __TIME__));
					if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Knifes).c_str(), current_knife, flags))
					{
						for (size_t i = 0; i < g_vKnifesSkins.size(); ++i)
						{
							ImGui::PushID(i);
							const bool is_selected = Options::menu.weapon.current_knife == i;
							const auto weapon = g_vKnifesSkins.at(i).szWeaponNameAsU8.c_str();
							if (ImGui::Selectable(weapon, is_selected))
							{
								Options::menu.weapon.current_knife = i;
								need_forceupdate_for_knife = true;
								current_config->knife.info = &g_vKnifesSkins.at(Options::menu.weapon.current_knife);
								Options::menu.weapon.current_knife_skin = 0;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
							ImGui::PopID();
						}
						ImGui::EndCombo();
					}
					else
					{
						Options::menu.weapon.current_knife_skin = Skins::GetSkinIndexBySkinInfo_t(
							current_config->knife.skin,
							g_vKnifesSkins.at(Options::menu.weapon.current_knife).vSkins);
					}
					ImGui::PopID();

					//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Knifes).c_str(), &Options::menu.weapon.current_knife,
					//                 g_vKnifesSkins))
					//{
					//    need_forceupdate_for_knife = true;
					//    current_config->knife.info = &g_vKnifesSkins.at(Options::menu.weapon.current_knife);
					//    Options::menu.weapon.current_knife_skin = 0;
					//}
					//else
					//    Options::menu.weapon.current_knife_skin = Skins::GetSkinIndexBySkinInfo_t(
					//        current_config->knife.skin, g_vKnifesSkins.at(Options::menu.weapon.current_knife).vSkins);

					if (current_config->knife.info->iItemDefinitionIndex != 0)
					{
						/* Skins */

						auto knife_skins = &g_vKnifesSkins.at(Options::menu.weapon.current_knife).vSkins;
						const auto current_knife_skin = knife_skins->at(Options::menu.weapon.current_knife_skin).szSkinNameAsU8.c_str();
						if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), current_knife_skin, flags))
						{
							for (size_t i = 0; i < knife_skins->size(); ++i)
							{
								ImGui::PushID(i);
								const bool is_selected = Options::menu.weapon.current_knife_skin == i;
								const auto weapon = knife_skins->at(i).szSkinNameAsU8.c_str();
								if (ImGui::Selectable(weapon, is_selected))
								{
									Options::menu.weapon.current_knife_skin = i;
									need_forceupdate_for_knife = true;
									current_config->knife.skin = &knife_skins->at(Options::menu.weapon.current_knife_skin);
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
								ImGui::PopID();
							}
							ImGui::EndCombo();
						}
						//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), &Options::menu.weapon.current_knife_skin,
						//                 g_vKnifesSkins.at(Options::menu.weapon.current_knife).vSkins))
						//{
						//    need_forceupdate_for_knife = true;
						//    current_config->knife.skin = &g_vKnifesSkins.at(Options::menu.weapon.current_knife).vSkins.
						//                                                 at(Options::menu.weapon.current_knife_skin);
						//}

						/* Name */
						ImGui::InputText(Hooks::localized_strs->at(SkinChanger_Name).c_str(), current_config->knife.name, 64);
						need_forceupdate_for_knife |= ImGui::IsItemDeactivatedAfterEdit();

						/* Wear */
						ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Wear).c_str(), &current_config->knife.fWear, 0.000001f, 1.0f);
						need_forceupdate_for_knife |= ImGui::IsItemDeactivatedAfterEdit();

						/* Seed */
						ImGui::InputInt(Hooks::localized_strs->at(SkinChanger_Seed).c_str(), &current_config->knife.iSeed);
						need_forceupdate_for_knife |= ImGui::IsItemDeactivatedAfterEdit();

						/* StatTrack */
						need_forceupdate_for_knife |= ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_StatTrak).c_str(), &current_config->knife.bStatTrack);
						if (current_config->knife.bStatTrack)
						{
							/* Kills */
							ImGui::InputInt(Hooks::localized_strs->at(SkinChanger_Kills).c_str(), &current_config->knife.iKills);
							need_forceupdate_for_knife |= ImGui::IsItemDeactivatedAfterEdit();
						}
						if (need_forceupdate_for_knife )
						{
							SourceEngine::interfaces.ClientState()->ForceFullUpdate();
							//
							//Hooks::g_fnCleanInventoryImageCacheDir();
#ifdef _PREMIUM_VERSION
							if (current_config->knife.skin)
								UpdateItem(Options::menu.current_team, current_config->knife.inventory_item, current_config->knife);
#endif
							//Protobuffs::SendMatchmakingClient2GCHello();
							if (Options::autosave_if_changed)
							{
								should_save_config = true;
							}
							need_forceupdate_for_knife = false;
						}
					}
					else
					{
						//Invalidate skin ptr, no need setup skin config for default knife
						current_config->knife.skin = nullptr;
					}
					ImGui::PopID();
				}

				/* Wearables */
				if (ImGui::CollapsingHeader(Hooks::localized_strs->at(SkinChanger_Wearables).c_str()))
				{
					ImGui::PushID(FNV("default_wearables" __TIME__ __DATE__));
					if (current_config->wearable.info == nullptr)
					{
						current_config->wearable.info = &g_vWearablesSkins.at(0);
						Options::menu.weapon.current_wearable = 0;
						//current_config->wearable.skin = &current_config->wearable.info->vSkins.at(0);
					}
					else
					{
						Options::menu.weapon.current_wearable = WearableSkin::iGetWearableIndexByItemDefinitionIndex(
							current_config->wearable.info->iItemDefinitionIndex);
					}

					/* Wearables */

					const auto current_wearable = g_vWearablesSkins.at(Options::menu.weapon.current_wearable).szWeaponNameAsU8.c_str();
					ImGui::PushID(FNV("wearables_combo" __TIME__ __DATE__));
					if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Wearables).c_str(), current_wearable, flags))
					{
						for (size_t i = 0; i < g_vWearablesSkins.size(); ++i)
						{
							ImGui::PushID(i);
							const bool is_selected = Options::menu.weapon.current_wearable == i;
							const auto weapon = g_vWearablesSkins.at(i).szWeaponNameAsU8.c_str();
							if (ImGui::Selectable(weapon, is_selected))
							{
								Options::menu.weapon.current_wearable = i;
								need_forceupdate_for_wearable = true;
								current_config->wearable.info = &g_vWearablesSkins.at(Options::menu.weapon.current_wearable);
								Options::menu.weapon.current_wearable_skin = 0;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
							ImGui::PopID();
						}
						ImGui::EndCombo();
					}
					else
					{
						Options::menu.weapon.current_wearable_skin = Skins::GetSkinIndexBySkinInfo_t(
							current_config->wearable.skin,
							g_vWearablesSkins.at(Options::menu.weapon.current_wearable).vSkins);
					}
					ImGui::PopID();
					//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Wearables).c_str(), &Options::menu.weapon.current_wearable,
					//                 g_vWearablesSkins))
					//{
					//    need_forceupdate_for_wearable = true;
					//    current_config->wearable.info = &g_vWearablesSkins.at(Options::menu.weapon.current_wearable);
					//    Options::menu.weapon.current_wearable_skin = 0;
					//}
					//else
					//    Options::menu.weapon.current_wearable_skin = Skins::GetSkinIndexBySkinInfo_t(
					//        current_config->wearable.skin,
					//        g_vWearablesSkins.at(Options::menu.weapon.current_wearable).vSkins);

					if (current_config->wearable.info->iItemDefinitionIndex != 0)
					{
						/* Skins */
						auto wearable_skins = &g_vWearablesSkins.at(Options::menu.weapon.current_wearable).vSkins;
						const auto current_wearable_skin = wearable_skins->at(Options::menu.weapon.current_wearable_skin).szSkinNameAsU8.c_str();
						if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), current_wearable_skin, flags))
						{
							for (size_t i = 0; i < wearable_skins->size(); ++i)
							{
								ImGui::PushID(i);
								const bool is_selected = Options::menu.weapon.current_wearable_skin == i;
								const auto weapon = wearable_skins->at(i).szSkinNameAsU8.c_str();
								if (ImGui::Selectable(weapon, is_selected))
								{
									Options::menu.weapon.current_wearable_skin = i;
									need_forceupdate_for_wearable = true;
									current_config->wearable.skin = &wearable_skins->at(Options::menu.weapon.current_wearable_skin);
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
								ImGui::PopID();
							}
							ImGui::EndCombo();
						}
						//need_forceupdate_for_wearable |= ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Skins).c_str(), &Options::menu.weapon.current_wearable_skin, g_vWearablesSkins.at(Options::menu.weapon.current_wearable).vSkins);

						if (need_forceupdate_for_wearable)
							current_config->wearable.skin = &g_vWearablesSkins.at(Options::menu.weapon.current_wearable)
							.vSkins.at(
								Options::menu.weapon.
								current_wearable_skin);

						/* Wear */
						ImGui::SliderFloat(Hooks::localized_strs->at(SkinChanger_Wear).c_str(), &current_config->wearable.fWear, 0.000001f, 1.0f);
						need_forceupdate_for_wearable |= ImGui::IsItemDeactivatedAfterEdit();

						ImGui::InputInt(Hooks::localized_strs->at(SkinChanger_Seed).c_str(), &current_config->wearable.iSeed);
						need_forceupdate_for_wearable |= ImGui::IsItemDeactivatedAfterEdit();
						
						if (need_forceupdate_for_wearable /*&& (!ImGui::IsMouseDown(0))*/)
						{
							//SourceEngine::interfaces.ClientState()->ForceFullUpdate();
							//Hooks::g_fnCleanInventoryImageCacheDir();
							//Protobuffs::SendClientHello();
#ifdef _PREMIUM_VERSION
							if(current_config->wearable.skin && current_config->wearable.skin->iPaintKitID)
								UpdateItem(Options::menu.current_team, current_config->wearable.inventory_item, current_config->wearable);
#endif
							//Protobuffs::SendMatchmakingClient2GCHello();
							if (Options::autosave_if_changed)
							{
								should_save_config = true;
							}
						}
					}
					ImGui::PopID();
				}

#ifdef _PREMIUM_VERSION
				if (ImGui::CollapsingHeader(Hooks::localized_strs->at(SkinChanger_Custom_Players).c_str()))
				{
					ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_Custom_Players_enable).c_str(), &current_config->enable_custom_players);
					if (current_config->enable_custom_players)
					{
						const char* current_team_skin;
						std::vector<PlayerInfo_t>* current_skins;
						size_t* selected_team_skin;

						std::string player_combo_name("Set player a");
						for (size_t i = 0; i < 5; ++i)
						{
							player_combo_name.pop_back();
							player_combo_name.push_back('a' + static_cast<char>(i));
							if (Options::menu.menu_current_team_buffer == 0)
							{
								current_team_skin = g_v_T_TeamSkins.at(Options::menu.weapon.current_t_team_skin[i]).item_name_as_u8.c_str();
								current_skins = &g_v_T_TeamSkins;
								selected_team_skin = Options::menu.weapon.current_t_team_skin;
							}
							else
							{
								current_team_skin = g_v_CT_TeamSkins.at(Options::menu.weapon.current_ct_team_skin[i]).item_name_as_u8.c_str();
								current_skins = &g_v_CT_TeamSkins;
								selected_team_skin = Options::menu.weapon.current_ct_team_skin;
							}
							if (ImGui::BeginCombo(player_combo_name.c_str(), current_team_skin, flags))
							{
								for (size_t j = 0; j < current_skins->size(); ++j)
								{
									const bool is_selected = selected_team_skin[i] == j;
									const auto weapon = current_skins->at(j).item_name_as_u8.c_str();
									if (ImGui::Selectable(weapon, is_selected))
									{
										selected_team_skin[i] = j;
										current_config->team.skins[i] = &current_skins->at(selected_team_skin[i]);
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
						}
					}
				}
#endif
				if (ImGui::CollapsingHeader(Hooks::localized_strs->at(SkinChanger_OldSounds).c_str()))
				{
					if (!Options::sounds_precached)
					{
						ImGui::Text("%s", Hooks::localized_strs->at(SkinChanger_OldSounds_NotDownloadedWarning).c_str());
					}
					else
					{
						if (engine->IsInGame())
						{
							ImGui::TextColored(ImColor(66, 188, 244), "%s", Hooks::localized_strs->at(SkinChanger_OldSounds_NotInGameWarning).c_str());
						}
						if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_OldSounds_PrecacheSounds).c_str()))
						{
							//SourceEngine::ConCommand* fn_rebuild_audiocache = SourceEngine::interfaces.CVar()->FindCommand("snd_rebuildaudiocache");
							Options::menu.main_window = false;
							engine->ExecuteClientCmd(XorStr("snd_rebuildaudiocache"));
						}
						if (ImGui::TreeNode(XorStr("Rifles")))
						{
							if (ImGui::Checkbox(
								XorStr("AK-47"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_ak47)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("M4A1-S"),
								&Options::config_old_sounds.
								at(SourceEngine::EItemDefinitionIndex::weapon_m4a1_silencer)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("M4A4"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_m4a1)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("AWP"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_awp)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("AUG"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_aug)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("FAMAS"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_famas)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("G3SG1"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_g3sg1)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Galil AR"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_galilar)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("SG 553"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_sg556)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("SSG 08"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_ssg08)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("SCAR-20"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_scar20)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNode(XorStr("Pistols")))
						{
							if (ImGui::Checkbox(
								XorStr("Glock-18"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_glock)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("USP-S"),
								&Options::config_old_sounds.at(
									SourceEngine::EItemDefinitionIndex::weapon_usp_silencer))
								)
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("P2000"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_hkp2000)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Dual Berettas"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_elite)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("P250"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_p250)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Tec-9"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_tec9)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Five-Seven"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_fiveseven)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("CZ75-Auto"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_cz75a)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Revolver R8"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_revolver)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Desert Eagle"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_deagle)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNode(XorStr("SMGs")))
						{
							if (ImGui::Checkbox(
								XorStr("MAC-10"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_mac10)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("MP9"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_mp9)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("MP7"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_mp7)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("UMP-45"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_ump)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("P90"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_p90)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Bizon"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_mac10)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNode(XorStr("Heavy")))
						{
							if (ImGui::Checkbox(
								XorStr("Nova"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_nova)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("XM1014"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_xm1014)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("MAG-7"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_mag7)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Sawed-Off"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_sawedoff)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("M249"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_m249)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							if (ImGui::Checkbox(
								XorStr("Negev"),
								&Options::config_old_sounds.at(SourceEngine::EItemDefinitionIndex::weapon_negev)))
							{
								Hooks::g_pConfig->save_old_sounds_config(Options::config_old_sounds);
							}
							ImGui::TreePop();
						}
					}
				}
			}

			ImGui::PushID(FNV("ClanTEGE" __TIME__ __DATE__));
			if(ImGui::CollapsingHeader(XorStr("ClanTag")))
			{
				ImGui::InputText(XorStr("clantag tag"), Options::clantag_name, 32);
				ImGui::InputText(XorStr("clantag name"), Options::clantag_tag, 32);

				if (ImGui::Button(XorStr("Set clantag")))
				{
					sendClanTag(Options::clantag_name, Options::clantag_tag);
				}
				if (ImGui::Button(XorStr("CSGO : Changer clantag")))
				{
					sendClanTag("[CSGO:Changer]", "[CSGO:Changer]");
				}
				if (ImGui::Button(XorStr("Clear clantag")))
				{
					sendClanTag("\n", "\n");
				}
			}
			ImGui::PopID();
#ifdef _PREMIUM_VERSION
			if (ImGui::Button(XorStr("Reload user skins")))
			{
				auto path = std::filesystem::current_path() / XorStr("csgo") / XorStr("SkinChanger_user");
				if (std::filesystem::exists(path))
				{
					std::vector<paint_kit_inject_request> custom_paint_kit_inject_request;
					std::string to_add;
					for (auto& p : std::filesystem::directory_iterator(path))
					{
						if (p.is_directory())
							continue;
						if (!p.is_regular_file())
							continue;

						const auto filename_path = p.path().filename();//p.path().filename();
						std::string name((const char*)filename_path.u8string().c_str());//filename_path.string();
						if (name.empty())
							continue;
						to_add = XorStr("SkinChanger_user/");
						to_add.append(name);
						custom_paint_kit_inject_request.emplace_back(to_add, 0xFACE14, 0xFACE14);
					}
					inject_all_paint_kits(custom_paint_kit_inject_request);
				}
				else
				{
					std::filesystem::create_directories(path);
				}
			}
#endif
			ImGui::Separator();
			/* Save Config */
			if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Save_config).c_str()))
			{
				should_save_config = true;
			}
			ImGui::SameLine();

			/* Load Config */
			if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Load_config).c_str()))
			{
				should_read_config = true;
			}
			ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_AutoSave_if_changed).c_str(), &Options::autosave_if_changed);
		}
		/* Copy T config to CT */
		if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Copy_T_config_to_CT).c_str()))
		{
			Options::config_ct = Options::config_t;
			SourceEngine::interfaces.ClientState()->ForceFullUpdate();
			//Protobuffs::SendClientHello();

		}
		ImGui::SameLine();

		/* Copy CT config to T */
		if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Copy_CT_config_to_T).c_str()))
		{
			Options::config_t = Options::config_ct;
			SourceEngine::interfaces.ClientState()->ForceFullUpdate();
			//Protobuffs::SendClientHello();
		}

		ImGui::Separator();
    	
		if (ImGui::Button(Hooks::localized_strs->at(SkinChanger_Randomize).c_str()))
		{
			Options::config_ct.randomize_config();
			Options::config_t.randomize_config();
			SourceEngine::interfaces.ClientState()->ForceFullUpdate();
			//Protobuffs::SendClientHello();
		}

		/* Random Skin */
		ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_RandomSkin).c_str(), &Options::enable_random_skin);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", Hooks::localized_strs->at(SkinChanger_RandomSkin_Description).c_str());

		if (Options::enable_random_skin)
		{
			ImGui::PushID(FNV("RandomSkin Key Rebind" __TIME__ __DATE__));
			ImGui::Text("%s", Hooks::localized_strs->at(SkinChanger_Random_skin_key).c_str());
			ImGui::SameLine();
			ImGui::hotkey(Options::random_skin_key, KEY_F4);
			ImGui::PopID();
			//int key = Options::random_skin_key;
			//ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Random_skin_key).c_str(), &key, s_pButtonCodeName, 162);
			//Options::random_skin_key = static_cast<ButtonCode_t>(key);
		}

		ImGui::Separator();
    	
		/* AutoAccept */
		ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_AutoAccept).c_str(), &Options::enable_auto_accept);
		/* Fix Viewmodel */
		ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_Fix_ViewModel_after_changing_team).c_str(), &Options::enable_fix_viewmodel);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", Hooks::localized_strs->at(SkinChanger_Fix_ViewModel_after_changing_team_Description).c_str());
		/* SuperStatrack */
		//ImGui:: 
		ImGui::Checkbox(Hooks::localized_strs->at(SkinChanger_SuperStattrack).c_str(), &Options::enable_super_stattrack);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", Hooks::localized_strs->at(SkinChanger_SuperStattrack_Description).c_str());

		/* Sky */
        const auto current_sky_name = sky_names[Options::menu.weapon.current_sky];
		if (ImGui::BeginCombo(Hooks::localized_strs->at(SkinChanger_Sky).c_str(), current_sky_name))
		{
			for (size_t i = 0; i < sky_names.size(); ++i)
			{
				ImGui::PushID(i);
				const bool is_selected = Options::menu.weapon.current_sky == i;
				const auto weapon = sky_names[i];
				if (ImGui::Selectable(weapon, is_selected))
				{
					Options::menu.weapon.current_sky = i;
					if (Options::menu.weapon.current_sky != 0 && engine->IsInGame())
					{
						ChangeSky(Options::menu.weapon.current_sky);
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		//if (ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Sky).c_str(), &Options::menu.weapon.current_sky, sky_names))
		//{
		//    if (Options::menu.weapon.current_sky != 0 && SourceEngine::interfaces.Engine()->IsInGame())
		//    {
		//        ChangeSky(Options::menu.weapon.current_sky);
		//    }
		//}

		//if(ImGui::Button(Hooks::localized_strs->at(SkinChanger_Reload_inventory).c_str()))
		//{
		//	Protobuffs::SendClientHello();
		//	Protobuffs::SendMatchmakingClient2GCHello();
		//}

		ImGui::PushID(FNV("Menu Key Rebind" __TIME__ __DATE__));
		ImGui::Text("%s", Hooks::localized_strs->at(SkinChanger_Menu_key).c_str());
		ImGui::SameLine();
		ImGui::hotkey(Options::menu_key, KEY_HOME);
		ImGui::PopID();
		//int key = Options::menu_key;
		//ImGui::Combo(Hooks::localized_strs->at(SkinChanger_Menu_key).c_str(), &key, s_pButtonCodeName, 162);
		//Options::menu_key = static_cast<ButtonCode_t>(key);

#ifndef _PREMIUM_VERSION
        //if (Hooks::bottom_banner || Hooks::top_banner)
        //    ImGui::EndChild();

        //if (Hooks::bottom_banner)
        //{
        //    ImGui::SetCursorPosY(ImGui::GetWindowSize().y + ImGui::GetScrollY() - 80);
        //    Hooks::bottom_banner->Render();
        //}
#endif
		ImGui::End();
		//fileDialog.Display();
		//if (fileDialog.HasSelected())
		//{
		//	//std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
		//	Hooks::changer_log->msg(XorStr("Selected filename: "));
		//	Hooks::changer_log->msg(fileDialog.GetSelected().string());
		//	Hooks::changer_log->msg(XorStr("\n"));
		//	InjectPaintKit(fileDialog.GetSelected().string().c_str(), 0xFACE14, 0xFACE14);
		//	fileDialog.ClearSelected();
		//}
    }
}
