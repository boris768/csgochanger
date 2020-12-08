// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include "SourceEngine/Recv.hpp"
#include "IBaseViewModel.h"
#include "Utils.hpp"

namespace Hooks
{
	int original_sequences[20];
	
	int DeterminateSequence(const int current_sequence, const int configured_knife_idi)
	{
		int sequence = current_sequence;

		switch (configured_knife_idi)
		{
		case SourceEngine::weapon_knifegg:
			if(sequence == SourceEngine::default_lookat01)
				sequence = Utils::RandomInt(SourceEngine::default_idle1, SourceEngine::default_idle2);
			break;
		case SourceEngine::weapon_knife_butterfly:
		{
			switch (sequence)
			{
			case SourceEngine::default_draw:
			{
				sequence = Utils::RandomInt(SourceEngine::butterfly_draw, SourceEngine::butterfly_draw2);
				break;
			}
			case SourceEngine::default_lookat01:
			{
				sequence = Utils::RandomInt(SourceEngine::butterfly_lookat01, SourceEngine::butterfly_lookat03);
				break;
			}
			default:
				++sequence;
				break;
			}
			break;
		}
		case SourceEngine::weapon_knife_falchion:
			switch (sequence)
			{
			case SourceEngine::default_draw:
			case SourceEngine::default_idle1:
			case SourceEngine::default_heavy_hit1:
			case SourceEngine::default_heavy_backstab:

				break;
			case SourceEngine::default_heavy_miss1:
				sequence = Utils::RandomInt(SourceEngine::falchion_heavy_miss1,
					SourceEngine::falchion_heavy_miss1_noflip);

				break;
			case SourceEngine::default_lookat01:
				sequence = Utils::FalchionLookat();
				break;
			default:
				--sequence;

				break;
			}

			break;
		case SourceEngine::weapon_knife_push:
			switch (sequence)
			{
			case SourceEngine::default_draw:
			case SourceEngine::default_idle1:

				break;
			case SourceEngine::default_idle2:
				sequence = SourceEngine::daggers_idle1;

				break;
			case SourceEngine::default_light_miss1:
				if (Utils::RandomInt(0, 1) == 0)
					sequence = SourceEngine::daggers_light_miss4;
				break;
			case SourceEngine::default_light_miss2:
				switch (Utils::RandomInt(0, 2))
				{
				case 0:
					sequence = SourceEngine::daggers_light_miss1;
					break;
				case 1:
					//already daggers_light_miss3 (==default_light_miss2)
					//sequence = SourceEngine::daggers_light_miss3;
					break;
				case 2:
					sequence = SourceEngine::daggers_light_miss5;
					break;
				default:;
				}

				break;
			case SourceEngine::default_heavy_miss1:
				sequence = Utils::RandomInt(SourceEngine::daggers_heavy_miss2,
					SourceEngine::daggers_heavy_miss1);

				break;

			case SourceEngine::default_heavy_hit1:
			case SourceEngine::default_heavy_backstab:
			case SourceEngine::default_lookat01:
				sequence += 3;

				break;
			default:
				sequence += 2;

				break;
			}

			break;
		case SourceEngine::weapon_knife_survival_bowie:
			switch (sequence)
			{
			case SourceEngine::default_draw:
			case SourceEngine::default_idle1:
				break;
			case SourceEngine::default_idle2:
				sequence = SourceEngine::bowie_idle1;
				break;
			default:
				--sequence;
				break;
			}
			break;
		case SourceEngine::weapon_knife_cord:
		case SourceEngine::weapon_knife_canis:
		case SourceEngine::weapon_knife_ursus:
		case SourceEngine::weapon_knife_outdoor:
		case SourceEngine::weapon_knife_skeleton:
			switch (sequence)
			{
			case SourceEngine::default_draw:
				sequence = Utils::RandomInt(SourceEngine::butterfly_draw, SourceEngine::butterfly_draw2);
				break;
			case SourceEngine::default_lookat01:
				sequence = Utils::RandomInt(SourceEngine::butterfly_lookat01, 14);
				break;
			default:
				sequence++;
				break;
			}
			break;
		case SourceEngine::weapon_knife_stiletto:
			if (sequence == SourceEngine::default_lookat01)
				sequence = Utils::RandomInt(12, 13);
			break;
		case SourceEngine::weapon_knife_widowmaker:
			switch (sequence)
			{
			case SourceEngine::default_lookat01:
				sequence = Utils::RandomInt(14, 15);
				break;
			case SourceEngine::default_heavy_backstab:
				sequence--;
				break;
			default:;
				break;
			}
			break;
		case SourceEngine::weapon_knife_css:
			if (sequence == SourceEngine::default_lookat01)
			{
				sequence = Utils::RandomInt(0,1) ? SourceEngine::css_lookat01 : SourceEngine::css_lookat02;
			}
		default:
			return sequence;
			
		}
		return sequence;
	}

	void __cdecl Hooked_nSequence(SourceEngine::CRecvProxyData* pData, SourceEngine::IClientEntity* pStruct, void* pOut)
	{
		//return g_fnOriginalnSequence(pData, pStruct, pOut);
		if(!pData || !pStruct || !pOut)
			return g_fnOriginalnSequence(pData, pStruct, pOut);

		auto entitylist = SourceEngine::interfaces.EntityList();
		const int& local_player_index = g_ctx.local_player_index;
		auto& local_player = g_ctx.local_player;
		if (!local_player || !local_player->IsAlive())
			return g_fnOriginalnSequence(pData, pStruct, pOut);

		customization *current_config;
		switch (local_player->GetTeamNum())
		{
			case SourceEngine::TeamID::TEAM_COUNTER_TERRORIST:
				current_config = &Options::config_ct;
				break;
			case SourceEngine::TeamID::TEAM_TERRORIST:
				current_config = &Options::config_t;
				break;
			case SourceEngine::TeamID::TEAM_UNASSIGNED:
			case SourceEngine::TeamID::TEAM_SPECTATOR:
			default:
			{
				return g_fnOriginalnSequence(pData, pStruct, pOut);
			}
		}
		const auto knife_info = current_config->knife.info;
		if (knife_info)
		{
			auto* viewmodel = static_cast<SourceEngine::IBaseViewModel*>(pStruct);
			const auto owner_handle = viewmodel->GetOwner();
			if (local_player_index == owner_handle.GetEntryIndex())
			{
				const auto view_model_weapon = static_cast<C_BaseCombatWeapon*>(entitylist->GetClientEntityFromHandle(viewmodel->GetWeapon()));
				if (view_model_weapon)
				{
					const auto item_definition_index = *view_model_weapon->GetItemDefinitionIndex();
					if (item_definition_index == knife_info->iItemDefinitionIndex)
					{
						//if(!knife_info->is_sequences_inited)
						//{							
						//	for(int i = 0; i < 13; ++i)
						//	{
						//		const int& originalActivity = original_sequences[i]; // Get the original sequence activity

						//		//const SourceEngine::iActivity tempActitivity = viewmodel->GetSequenceActivity(i);
						//		//if (originalActivity != -1 && originalActivity == tempActitivity ||
						//		//	originalActivity == -1 && tempActitivity == SourceEngine::ACT_VM_IDLE_LOWERED)
						//		//{
						//			for (int sequence_variants = 0; sequence_variants < 4; ++sequence_variants)
						//				if (viewmodel->GetSequenceActivity(i + sequence_variants) == originalActivity)
						//					knife_info->remap_sequences.at(i).emplace_back(i + sequence_variants);
						//		//}
						//	}
						//	for (int sequence_variants = 0; sequence_variants < 4; ++sequence_variants)
						//		if (viewmodel->GetSequenceActivity(12 + sequence_variants) == SourceEngine::ACT_VM_IDLE_LOWERED)
						//			knife_info->remap_sequences.at(12).emplace_back(12 + sequence_variants);
						//	//dump
						//	Hooks::changer_log->msg("\n");
						//	for(size_t i = 0; i < knife_info->remap_sequences.size(); ++i)
						//	{
						//		Hooks::changer_log->msg(knife_info->szShortWeaponName);
						//		Hooks::changer_log->msg(": orginal: ");
						//		Hooks::changer_log->msg(std::to_string(i));
						//		Hooks::changer_log->msg(" new: ");
						//		auto it = knife_info->remap_sequences.at(i);
						//		for (int j : it)
						//		{
						//			Hooks::changer_log->msg(" ");
						//			Hooks::changer_log->msg(std::to_string(j));
						//		}
						//		Hooks::changer_log->msg("\n");
						//	}
						//	knife_info->is_sequences_inited = true;
						//}
						//int new_sequence;
						//if (knife_info->remap_sequences[pData->m_Value.m_Int].size() > 1)
						//	new_sequence = knife_info->remap_sequences[pData->m_Value.m_Int].at(Utils::RandomInt(0U, knife_info->remap_sequences[pData->m_Value.m_Int].size() - 1));
						//else
						//	new_sequence = knife_info->remap_sequences[pData->m_Value.m_Int].at(0);

						//viewmodel->SendViewModelMatchingSequence(new_sequence);
						//return;

						pData->m_Value.m_Int = DeterminateSequence(pData->m_Value.m_Int, item_definition_index);
						g_fnOriginalnSequence(pData, pStruct, pOut);
						//int new_sequence;
						//if(item_definition_index == SourceEngine::weapon_knife_css)
						//{
						//	new_sequence = DeterminateSequence(pData->m_Value.m_Int, item_definition_index);
						//}
						//else
						//{
						//	new_sequence = SourceEngine::default_idle1;
						//	const int& originalActivity = original_sequences[pData->m_Value.m_Int]; // Get the original sequence activity


						//	int num = 0;
						//	for (int remap_sequence = 0; remap_sequence < 20; ++remap_sequence) // compare activity with original activity and save new activity // havent seen a sequence above 14 but using 20 if theres is :D
						//	{
						//		const SourceEngine::iActivity tempActitivity = viewmodel->GetSequenceActivity(remap_sequence);
						//		if (originalActivity != -1 && originalActivity == tempActitivity ||
						//			originalActivity == -1 && tempActitivity == SourceEngine::ACT_VM_IDLE_LOWERED)
						//		{
						//			new_sequence = remap_sequence;
						//			for (int sequence_variants = 0; sequence_variants < 4; ++sequence_variants)
						//				if (viewmodel->GetSequenceActivity(remap_sequence + sequence_variants) == tempActitivity)
						//					++num;
						//			break;
						//		}
						//	}
						//	if (originalActivity == -1 && new_sequence == 1)
						//		new_sequence = pData->m_Value.m_Int;
						//	if (num)
						//		new_sequence = Utils::RandomInt(new_sequence, new_sequence + num - 1);
						//}
						//view_model_weapon->SetIdealActivity(viewmodel->GetSequenceActivity(pData->m_Value.m_Int));
						//g_fnOriginalnSequence(pData, pStruct, pOut);
						
						//float* timeweaponidle = player_weapon->GetTimeWeaponIdle();
						//*timeweaponidle = SourceEngine::interfaces.GlobalVars()->curtime + 200.f;
					}
					
				}				
			}
		}
		g_fnOriginalnSequence(pData, pStruct, pOut);
	}
}
