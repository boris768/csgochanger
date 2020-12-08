// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SkinChanger.h"
#include "Stickers.h"
#include "float_functions.h"
#include "IBaseViewModel.h"
#include "CCSWeaponVisualsDataProcessor.h"
#include "C_EconItemView.h"
#include "Hooks.hpp"

//bool clearRefCountedVector( SourceEngine::CUtlVector<__IRefCounted*>* vec)
//{
//	for (int i = 0; i < vec->Count(); ++i)
//	{
//		auto elem = vec->Element(i);
//		if (elem)
//		{
//			elem->unreference();
//			// ReSharper disable once CppAssignedValueIsNeverUsed
//			elem = nullptr;
//		}
//	}
//
//	vec->RemoveAll();
//	return true;
//}

bool resetSkin(C_BaseCombatWeapon* weapon, const SkinInfo_t* skin_optim, const bool is_knife)
{
	int* deltatick = SourceEngine::interfaces.ClientState()->GetDeltaTick();
	if (deltatick && *deltatick != -1)
	{
		if (skin_optim && skin_optim->iPaintKitID > 0)
		{
			auto* weapon_networkable = weapon->GetIClientNetworkable();
			if (!weapon_networkable)
				return false;

			SourceEngine::C_EconItemView* iconitemview = weapon->GetEconItemView();

			//fix fucked rare crash
			//const SourceEngine::CCStrike15ItemDefinition* weapon_item_defenition = iconitemview->GetStaticData();
			//if (!weapon_item_defenition)
			//	return false;

			bool* m_bCustomMaterialInitialized = weapon->GetCustomMaterialInitialized();
			
			if (!*m_bCustomMaterialInitialized && !is_knife)
				return false;

			*m_bCustomMaterialInitialized = false;

			iconitemview->ClearCustomMaterials(false);
			auto* weapon_custom_materials = weapon->GetCustomMaterials();
			for (int i = 0; i < weapon_custom_materials->Count(); i++)
			{
				if (weapon_custom_materials->Element(i) != nullptr)
				{
					weapon_custom_materials->Element(i)->Release();
					weapon_custom_materials->Element(i) = nullptr;
				}
			}
			weapon_custom_materials->RemoveAll();

			auto* vec = iconitemview->GetVec();
			auto* vec2 = reinterpret_cast<SourceEngine::CUtlVector<SourceEngine::__IRefCounted*>*>(vec);
			
			for (int i = 0; i < vec2->Count(); ++i)
			{
				auto* elem = vec2->Element(i);
				if (elem)
				{
					elem->unreference();
					// ReSharper disable once CppAssignedValueIsNeverUsed
					elem = nullptr;
				}
			}

			vec->RemoveAll();

			weapon_networkable->PostDataUpdate(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
			weapon_networkable->OnDataChanged(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
			SourceEngine::UpdatePanorama();
			return true;
		}
	}
	return false;
}

void ChangeWeapon(C_BaseCombatWeapon* weapon, const CustomSkinWeapon* config, const SourceEngine::player_info_t& pInfo, const bool alive, const
                  bool local_player_dormant)
{
	auto* const stattrak = weapon->GetFallbackStatTrak();

	if (config->bStatTrack)
	{
		*weapon->GetAccountID() = pInfo.xuidlow;
		*weapon->GetItemIDHigh() = -1;
		*weapon->GetEntityQuality() = 1;			
		if (!local_player_dormant)
		{
			if (*stattrak != config->iKills )
			{
				*stattrak = config->iKills;

				auto* networkable_weapon = weapon->GetIClientNetworkable();
				networkable_weapon->PostDataUpdate(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
				networkable_weapon->OnDataChanged(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
			}
		}
	}
	else
	{
		*weapon->GetEntityQuality() = config->iEntityQuality;
		*stattrak = -1;
	}
	
	const SkinInfo_t* skin_optim = config->skin;

	if (skin_optim &&
		skin_optim->iPaintKitID > 0 &&
		*weapon->GetFallbackPaintKit() != skin_optim->iPaintKitID)
	{
		*weapon->GetItemIDHigh() = -1;
		if (config->name[0])
			strcpy_s(weapon->GetCustomName(), 64, config->name);

		*weapon->GetFallbackPaintKit() = skin_optim->iPaintKitID;
		*weapon->GetFallbackSeed() = config->iSeed;
		*weapon->GetFallbackWear() = config->fWear;
		if (alive)
		{
			resetSkin(weapon, skin_optim, false);
		}
	}
	if (!alive)
		return;

	const size_t max_stickers_weapon = config->info->iMaxStickers;

	if (!max_stickers_weapon)
		return;

	SourceEngine::C_EconItemView* iconitemview = weapon->GetEconItemView();
	if (!iconitemview)
		return;

	std::vector<StickerInfo> vAppliedStickers;
	for (size_t index = 0; index < max_stickers_weapon; ++index)
	{
		const auto sticker_id = config->Stickers.at(index).iStickerKitID;
		if (sticker_id == 0)
			continue;

		StickerInfo_t* aStickerInfo = nullptr;
		for (auto& g_vSticker : g_vStickers)
		{
			StickerInfo_t* iterator = &g_vSticker;
			if(g_vSticker == sticker_id)
			{
				aStickerInfo = iterator;
				break;
			}
		}
		if(!aStickerInfo)
			continue;
		
		vAppliedStickers.emplace_back(aStickerInfo, index);
	}
	if (vAppliedStickers.empty())
		return;

	*weapon->GetItemIDHigh() = -1;

	GenerateStickers(iconitemview, vAppliedStickers, config);
}

int last_knife_sequence = -1;
int set_random_model_tick = 0;
bool random_model_not_setted = false;
SourceEngine::IClientEntity* pLastViewModelWeapon = nullptr;

void ChangeKnife(C_BaseCombatWeapon* knife, CustomSkinKnife& config, const SourceEngine::player_info_t& pInfo,
                 C_CSPlayer* local_player)
{

	int* fallbackpaintkit = knife->GetFallbackPaintKit();
	uint32_t* itemidhigh = knife->GetItemIDHigh();
	short* item_def = knife->GetItemDefinitionIndex();
	int* modelindex = knife->GetModelIndex();
	int* quality = knife->GetEntityQuality();
	//bool* client_side_animation = knife->GetClientSideAnimation();
	SourceEngine::IBaseWorldModel* world_model = knife->GetWeaponWorldModel();

	//*client_side_animation = true;
	
	//Model settings
	auto* pViewModel = reinterpret_cast<SourceEngine::IBaseViewModel *>(local_player->GetViewModel());
	SourceEngine::IClientEntity* pViewModelWeapon = nullptr;
	if (pViewModel)
	{
		const SourceEngine::CBaseHandle handle = *reinterpret_cast<SourceEngine::CBaseHandle*>(reinterpret_cast<ptrdiff_t>(
			pViewModel) + netvars_c_base_view_model->m_hWeapon);
		pViewModelWeapon = SourceEngine::interfaces.EntityList()->GetClientEntityFromHandle(handle);
	}
	if (*item_def != config.info->iItemDefinitionIndex)
	{
		*item_def = config.info->iItemDefinitionIndex;
		config.model_indexes.iViewModel = -1;
	}
	config.UpdateModelIndexes();

	*modelindex = config.model_indexes.iViewModel;

	if (pViewModel && pViewModelWeapon == knife )
	{
		//if (*modelindex != config.model_indexes.iViewModel)
		//{
		//	knife->SetModelIndex(config.model_indexes.iViewModel);
		//pViewModel->SetWeaponModel(config.info->szViewModel.c_str(), knife);
		pViewModel->SetModelIndex(config.model_indexes.iViewModel);
		//}
		
		//float* timeweaponidle = knife->GetTimeWeaponIdle();
		//*timeweaponidle = *timeweaponidle + 2.f;

		//*timeweaponidle += 0.1f;
	}
	//	int sequence;
	//	int* game_sequence = pViewModel->GetSequence();
	//	if (pLastViewModelWeapon != pViewModelWeapon)
	//	{
	//		pLastViewModelWeapon = pViewModelWeapon;
	//		sequence = SourceEngine::KnifeSequences::default_draw;
	//	}
	//	else
	//	{
	//		if (random_model_not_setted)
	//			sequence = SourceEngine::KnifeSequences::default_lookat01;
	//		else
	//			sequence = *game_sequence;
	//	}
	//	if (sequence != last_knife_sequence)
	//	{
	//		//std::cout << config.info->szViewModel << " last: " << last_knife_sequence << ", ";
	//		const auto new_sequence = DeterminateSequence(sequence, config.info->iItemDefinitionIndex);
	//		//std::cout << " new: " << new_sequence << ", ";
	//		pViewModel->SendViewModelMatchingSequence(new_sequence);
	//		last_knife_sequence = *game_sequence;
	//		//std::cout << " writed: " << last_knife_sequence << std::endl;
	//	}
	//	*timeweaponidle = SourceEngine::interfaces.GlobalVars()->curtime + 2;
	//}
	//else
	//{
	//	last_knife_sequence = -1;
	//	pLastViewModelWeapon = nullptr;
	//	//std::cout << config.info->szViewModel << ": MODEL LOST"  << std::endl << std::endl;
	//}
	if (world_model)
		world_model->SetModelIndex(config.model_indexes.iWorldModel);
	auto* const globalvars = SourceEngine::interfaces.GlobalVars();
	//Skin settings
	if (config.skin->iPaintKitID != 0 &&
		(*itemidhigh != static_cast<uint32_t>(-1) ||
			*fallbackpaintkit != config.skin->iPaintKitID))
	{
		uint32_t* accountid = knife->GetAccountID();
		int* fallbackseed = knife->GetFallbackSeed();
		float* fallbackwear = knife->GetFallbackWear();
		char* name = knife->GetCustomName();
		*quality = 3;
		*accountid = pInfo.xuidlow;
		*itemidhigh = -1;
		*fallbackpaintkit = config.skin->iPaintKitID;
		*fallbackseed = config.iSeed;
		if (float_functions::is_equal(config.fWear, 0.0f))
		{
			config.fWear = 0.000001f;
		}
		*fallbackwear = config.fWear;
		if (config.name[0])
			strcpy_s(name, 64, config.name);
		random_model_not_setted = true;
		set_random_model_tick = globalvars->tickcount + 2;
	}
	if(random_model_not_setted && set_random_model_tick == globalvars->tickcount)
	{
		resetSkin(knife, config.skin, true);
		random_model_not_setted = false;
		set_random_model_tick = 0;
	}

	auto* fallbackstattrack = knife->GetFallbackStatTrak();
	if (config.bStatTrack)
	{
		//*itemidhigh = -1;
		//*accountid = pInfo.xuidlow;
		*quality = 1;
		if (*fallbackstattrack != config.iKills)
		{
			*fallbackstattrack = config.iKills;
			if (!local_player->GetDormant())
			{
				SourceEngine::IClientNetworkable* networkable_knife = knife->GetIClientNetworkable();
				networkable_knife->PostDataUpdate(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
				networkable_knife->OnDataChanged(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
			}
		}
	}
	else
	{
		*fallbackstattrack = -1;
	}
	
}
