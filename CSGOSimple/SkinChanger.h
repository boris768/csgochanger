#pragma once
#include "Options.hpp"

void ChangeWeapon(C_BaseCombatWeapon *weapon, const CustomSkinWeapon* config, const SourceEngine::player_info_t &pInfo, bool alive, const
                  bool local_player_dormant);
void ChangeKnife(C_BaseCombatWeapon *knife, CustomSkinKnife &config, const SourceEngine::player_info_t &pInfo, C_CSPlayer *local_player);