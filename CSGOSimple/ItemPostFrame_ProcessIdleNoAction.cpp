#include "Hooks.hpp"
#include "IBaseViewModel.h"
#include "Utils.hpp"

int DeterminateSequence1(const int current_sequence, const int configured_knife_idi)
{
	int sequence = current_sequence;

	switch (configured_knife_idi)
	{
	case SourceEngine::weapon_knifegg:
		if (sequence == SourceEngine::default_lookat01)
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
			sequence = Utils::RandomInt(0, 1) ? SourceEngine::css_lookat01 : SourceEngine::css_lookat02;
		}
	default:
		return sequence;

	}
	return sequence;
}

int old_sequence = 0;
namespace Hooks
{
	void __fastcall Hooked_ItemPostFrame_ProcessIdleNoAction(C_BaseCombatWeapon* weapon, void* /*edx*/, C_CSPlayer* player)
	{
		g_fnItemPostFrame_ProcessIdleNoAction(weapon, player);
		const auto item_definition_index = *weapon->GetItemDefinitionIndex();
		if(C_BaseCombatWeapon::GetEconClass(item_definition_index) == C_BaseCombatWeapon::knife)
		{
			//if(player->IsLookingAtWeapon())
			//{
			//old_sequence
				auto view_model = static_cast<SourceEngine::IBaseViewModel*>(player->GetViewModel());
				if(view_model)
				{
					*view_model->GetClientSideAnimation() = true;
					const auto current_sequence = *view_model->GetSequence();
					if (current_sequence != old_sequence)
					{
						const auto result = DeterminateSequence1(current_sequence, item_definition_index);

						weapon->SendViewModelAnim(result);
						//view_model->SendViewModelMatchingSequence(result);
						*view_model->GetAnimTime() = SourceEngine::interfaces.GlobalVars()->curtime;
						old_sequence = result;
					}
				}
				
			//}
		}
	}
}