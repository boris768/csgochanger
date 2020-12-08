// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"
#include "SkinChanger.h"
#include "CEconWearable.h"
#include "float_functions.h"
#include "IBaseViewModel.h"
#include "Menu.h"
#include "Utils.hpp"
#include "Protobuffs/inventory_changer.h"
#include "VFTableHook.hpp"


namespace Hooks
{
	inline void GetClientClasses(std::unordered_map<std::string, int>& classid_map)
	{
		SourceEngine::ClientClass* pClass = SourceEngine::interfaces.Client()->GetAllClasses();
		while (pClass)
		{
			const auto found = classid_map.find(pClass->m_pNetworkName);
			if (found != classid_map.end())
			{
				found->second = pClass->m_ClassID;
			}
			pClass = pClass->m_pNext;
		}
	}

	customization* current_config = nullptr;
	CustomSkinWeapon* config_weapon;
	constexpr SourceEngine::Vector abs_pos_wearables(10000.f, 10000.f, 10000.f);
	bool is_need_update_gloves = false;
	bool is_need_get_classIDs = true;
	bool is_need_refresh_weapon_hud = false;
	bool required_random_skin = false;

	volatile bool is_pressed_random = false;
	bool is_down_random = false;
	bool is_clicked_random = false;
	SourceEngine::CBaseHandle created_wearables;

	int wearable_set_random_model_tick = 0;
	bool wearable_random_model_not_setted = false;
	
	pfnStringChanged old_string_changed_callback = nullptr;
	INetworkStringTable* model_precache = nullptr;
	bool hook_vm = true;

	vf_hook* viewmodel_hook = nullptr;
	using FireEvent_t = void(__thiscall*)(void*, const SourceEngine::Vector&, const SourceEngine::QAngle&, int, const char*);
	FireEvent_t orig_FireEvent = nullptr;
	void __fastcall Hooked_FireEvent(void* ecx, void*, const SourceEngine::Vector& origin, const SourceEngine::QAngle& angles, int eventNum, const char* options)
	{
		if(eventNum == 72)
		{
			changer_log->msg("vm: event 72: ");
			changer_log->msg(options);
			changer_log->msg("\n");
			return;
			
		}
		orig_FireEvent(ecx, origin, angles, eventNum, options);
	}
	float vm_flAnimTime = 0.f;
	void __fastcall Hooked_FrameStageNotify(void* ecx, void* /*edx*/, const SourceEngine::ClientFrameStage_t curStage) //-V2008
	{

		g_fnOriginalFrameStageNotify(ecx, curStage);
		auto* clientstate = SourceEngine::interfaces.ClientState();

		if (curStage == SourceEngine::ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START &&
			clientstate->GetSignonState() == state_full)
		{
			//SourceEngine::IVEngineClient* engine = SourceEngine::interfaces.Engine();
			SourceEngine::IClientEntityList* entitylist = SourceEngine::interfaces.EntityList();

			g_ctx.local_player = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(g_ctx.local_player_index));
			if (!g_ctx.local_player)
			{
				return;
			}
			const auto local_player_dormant = g_ctx.local_player->GetDormant();
			const bool alive = g_ctx.local_player->IsAlive();

			if (is_need_get_classIDs)
			{
				GetClientClasses(netvars_c_base_combat_weapon->ClassIDs);
				is_need_get_classIDs = false;
			}
			if (*clientstate->GetDeltaTick() == -1)
			{
				is_need_refresh_weapon_hud = true;
			}
			if (is_need_refresh_weapon_hud && !local_player_dormant && alive)
			{
				SourceEngine::UpdatePanorama();
				is_need_refresh_weapon_hud = false;
			}
			const auto player_team = g_ctx.local_player->GetTeamNum();
			switch (player_team)
			{
			case SourceEngine::TeamID::TEAM_COUNTER_TERRORIST:
				current_config = &Options::config_ct;
				break;
			case SourceEngine::TeamID::TEAM_TERRORIST:
				current_config = &Options::config_t;
				break;
			case SourceEngine::TeamID::TEAM_UNASSIGNED:
			case SourceEngine::TeamID::TEAM_SPECTATOR:
				{
#ifdef CHANGER_PROFILE
				__itt_task_end(domain);
#endif
					return;
				}
			}
			//if(hook_vm && alive)
			//{
			//	auto vm = (SourceEngine::IBaseViewModel*)g_ctx.local_player->GetViewModel();
			//	
			//	viewmodel_hook = new vf_hook;
			//	viewmodel_hook->setup(nullptr, vm);
			//	viewmodel_hook->hook_index(Hooked_FireEvent, 199);
			//	orig_FireEvent = viewmodel_hook->get_original<FireEvent_t>(199);
			//	hook_vm = false;
			//}
			if (Options::enable_random_skin)
			{
				if (is_pressed_random)
				{
					is_clicked_random = false;
					is_down_random = true;
				}
				else
				{
					is_clicked_random = is_down_random;
					is_down_random = false;
				}
				if (is_clicked_random)
				{
					required_random_skin = true;
				}
			}

			//auto inventory = SourceEngine::CSInventoryManager::GetLocalPlayerInventory();

			//for (int i = 65; i <= highest_entity_index; ++i)
			for (int& obj : ent_listner->weapons)
			{
				if(obj == -1)
					continue;

				auto* weapon = static_cast<C_BaseCombatWeapon*>(entitylist->GetClientEntity(obj));
				if(!weapon || !weapon->IsWeaponByClass())
				{
					obj = -1;
					continue;
				}
				SourceEngine::C_EconItemView* iconitemview = weapon->GetEconItemView();

				//fix fucked rare crash
				const SourceEngine::CCStrike15ItemDefinition* weapon_item_defenition = iconitemview->GetStaticData();
				if (!weapon_item_defenition)
				{
					//changer_log->msg(std::string(XorStr("\nEntityListener: item def not found for: "))+ weapon->GetClientClass()->m_pNetworkName + std::to_string(obj.m_idx));
					continue;
				}
				//if not our weapon - delete it
				const int* original_owner_xuid_low = weapon->GetOriginalOwnerXuidLow();
				if (*original_owner_xuid_low != g_ctx.player_info.xuidlow)
				{
					//changer_log->msg(std::string(XorStr("EntityListener: marking for delete ") + std::to_string(obj.m_idx)));
					obj = -1;
					continue;
				}
				const int* original_owner_xuid_high = weapon->GetOriginalOwnerXuidHigh();
				if (*original_owner_xuid_high != g_ctx.player_info.xuidhigh)
				{
					//changer_log->msg(std::string(XorStr("EntityListener: marking for delete ") + std::to_string(obj.m_idx)));
					obj = -1;
					continue;
				}

				const short item_definition_index = *weapon->GetItemDefinitionIndex();
				const C_BaseCombatWeapon::econ_class weapon_econ_class = C_BaseCombatWeapon::GetEconClass(item_definition_index);

				/*  // NOLINT(clang-diagnostic-comment)
				if (weapon_econ_class == C_BaseCombatWeapon::none)
				{
					ent_listner->weapons.erase(ent_listner->weapons.begin() + element);
					continue;
				}
				if (weapon_econ_class == C_BaseCombatWeapon::knife)
				{
					//auto econ_item_view = weapon->GetEconItemView();
					//if (!econ_item_view)
					//	continue;
					//const auto weapon_item_defenition = econ_item_view->GetStaticData();
					//if (!weapon_item_defenition)
					//	continue;
					SourceEngine::C_EconItemView* econSkinData = inventory->GetItemInLoadout(
						(int)player_team, GetSlotID(item_definition_index));
					if (!econSkinData)
						continue;

					auto econData = econSkinData->GetSOCData();
					if (!econData)
						continue;

					int* modelindex = weapon->GetModelIndex();
					SourceEngine::IBaseWorldModel* world_model = weapon->GetWeaponWorldModel();

					//Model settings
					auto pViewModel = reinterpret_cast<SourceEngine::IBaseViewModel*>(local_player->GetViewModel());
					SourceEngine::IClientEntity* pViewModelWeapon = nullptr;
					if (pViewModel)
					{
						const SourceEngine::CBaseHandle handle = *reinterpret_cast<SourceEngine::CBaseHandle*>(reinterpret_cast<ptrdiff_t>(
							pViewModel) + netvars_c_base_view_model->m_hWeapon);
						pViewModelWeapon = SourceEngine::interfaces.EntityList()->GetClientEntityFromHandle(handle);
					}

					const auto knife_skin = KnifeSkin::GetKnifeSkinByItemDefinitionIndex(*econData->GetDefIndex());
					//knife_skin.model
					const auto viewmodel_index = SourceEngine::interfaces.ModelInfo()->GetModelIndex(knife_skin->szViewModel.c_str());
					const auto worldmodel_index = SourceEngine::interfaces.ModelInfo()->GetModelIndex(knife_skin->szWorldModel.c_str());

					if (pViewModel && pViewModelWeapon == weapon)
					{
						if (*modelindex != viewmodel_index)
						{
							pViewModel->SetWeaponModel(knife_skin->szViewModel.c_str(), weapon);
							weapon->SetModelIndex(viewmodel_index);
							//*modelindex = viewmodel_index;

							const auto m_bCustomMaterialInitialized = weapon->GetCustomMaterialInitialized();
							//*m_bCustomMaterialInitialized = false;
							if(m_bCustomMaterialInitialized)
							{
								weapon->PostDataUpdate(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
								weapon->OnDataChanged(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
							}
							//weapon->UpdateCustomMaterial();

						}
						//*timeweaponidle = *timeweaponidle + 0.1f;
					}

					if (world_model)
						world_model->SetModelIndex(worldmodel_index);

					const auto id = *econData->GetItemID();
					const uint64_t mask = 0xFFFFFFFF;// std::numeric_limits<uint32_t>::max();
					const uint32_t low_id = mask & id;
					const uint32_t high_id = id >> 32;

					*weapon->GetItemDefinitionIndex() = *econData->GetDefIndex();
					*weapon->GetItemIDLow() = low_id;
					*weapon->GetItemIDHigh() = high_id;
					*weapon->GetAccountID() = player_info.xuidlow;

					continue;
					
				}
				if (weapon_econ_class == C_BaseCombatWeapon::weapon)
				{
					SourceEngine::C_EconItemView* econ_item_view = weapon->GetEconItemView();
					if (!econ_item_view)
						continue;

					const SourceEngine::CCStrike15ItemDefinition* weapon_item_defenition = econ_item_view->
						GetStaticData();
					if (!weapon_item_defenition || weapon_item_defenition->id != item_definition_index)
						continue;

					SourceEngine::C_EconItemView* econSkinData = inventory->GetItemInLoadout(
						(int)player_team, GetSlotID(item_definition_index));

					if (!econSkinData)
						continue;

					SourceEngine::CEconItem* econData = econSkinData->GetSOCData();
					if (!econData)
						continue;

					const auto id = *econData->GetItemID();
					const uint64_t mask = 0xFFFFFFFF;// std::numeric_limits<uint32_t>::max();
					const uint32_t low_id = mask & id;
					const uint32_t high_id = id >> 32;
					uint32_t* itemid_low = weapon->GetItemIDLow();
					uint32_t* itemid_high = weapon->GetItemIDHigh();
					//if (*itemid_low != low_id || *itemid_high != high_id)
					{
						*itemid_low = low_id;
						*itemid_high = high_id;
						*weapon->GetAccountID() = player_info.xuidlow;

						//const auto m_bCustomMaterialInitialized = weapon->GetCustomMaterialInitialized();
						//*m_bCustomMaterialInitialized = false;

						//weapon->UpdateCustomMaterial();

						//weapon->PostDataUpdate(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
						//weapon->OnDataChanged(SourceEngine::DataUpdateType_t::DATA_UPDATE_CREATED);
					}
				}
			}
			*/

				switch (weapon_econ_class)
				{
				case C_BaseCombatWeapon::weapon:
				{
					std::vector<CustomSkinWeapon>* weapons = &current_config->weapons;
					if (weapons->empty())
					{
						//BUG: create config for weapon, if random skin required
						//if (required_random_skin)
						//{
						//	auto weapon_skin = WeaponSkin::GetWeaponSkinByItemDefinitionIndex(item_definition_index);
						//	weapons->emplace_back(weapon_skin, weapon_skin->vSkins.at(0));
						//}
						//else
						continue;
					}
					bool config_found = false;

					auto* const begin = weapons->data();
					auto* const end = begin + weapons->size();
					for (CustomSkinWeapon* it = begin; it < end; ++it)
					{
						if (it->info->iItemDefinitionIndex == item_definition_index)
						{
							config_weapon = it;
							config_found = true;
							break;
						}
					}
					if (required_random_skin)
					{
						const C_BaseCombatWeapon* active_weapon = g_ctx.local_player->GetActiveWeapon();
						if (active_weapon == weapon)
						{
							if (config_found)
							{
								config_weapon->RandomSkin(Options::enable_random_stickers);
							}
							else
							{
								weapons->emplace_back(CustomSkinWeapon(item_definition_index, 0));
								config_weapon = weapons->data() + (weapons->size() - 1);
								config_weapon->RandomSkin(Options::enable_random_stickers);
								config_found = true;
							}
							required_random_skin = false;
						}
					}
					if (config_found)
					{
						//const auto id = *config_weapon->inventory_item->GetItemID();
						//const uint64_t mask = (std::numeric_limits<size_t>::max)();
						//const uint32_t low_id = mask & id;
						//const uint32_t high_id = id >> 32;
						//*weapon->GetItemIDLow() = low_id;
						//*weapon->GetItemIDHigh() = high_id;
						//*weapon->GetAccountID() = g_ctx.player_info.xuidlow;
						ChangeWeapon(weapon, config_weapon, g_ctx.player_info, alive, local_player_dormant);
					}
					continue;
				}
				case C_BaseCombatWeapon::knife:
				{
					if (required_random_skin)
					{
						const C_BaseCombatWeapon* active_weapon = g_ctx.local_player->GetActiveWeapon();
						if (active_weapon == weapon)
						{
							current_config->knife.RandomSkin();
							required_random_skin = false;
						}
					}
					if (alive && current_config->knife.info != nullptr && current_config->knife.skin != nullptr)
					{
						ChangeKnife(weapon, current_config->knife, g_ctx.player_info, g_ctx.local_player);
						continue;
					}
				}
				case C_BaseCombatWeapon::none:
					//nothing to work here - mark for delete
					//ent_listner->weapons.erase(ent_listner->weapons.begin() + element);
					obj = -1;
				}
			}

			//const int highest_entity_index = entitylist->GetHighestEntityIndex();
			SourceEngine::CBaseHandle* wearables_handle = g_ctx.local_player->GetMyWearables();
			if (wearables_handle)
			{
				auto* wearable = reinterpret_cast<C_CEconWearable*>(entitylist->GetClientEntityFromHandle(wearables_handle[0]));

				if (!wearable)
				{
					auto* wearable_from_saved_handle = reinterpret_cast<C_CEconWearable*>(entitylist->
						GetClientEntityFromHandle(created_wearables));
					if (wearable_from_saved_handle)
					{
						//anal checks
						SourceEngine::ClientClass* clientclass = wearable_from_saved_handle->GetClientClass();
						if (clientclass->m_pCreateFn == CreateWearables)
						{
							wearables_handle[0] = created_wearables;
							wearable = wearable_from_saved_handle;
						}
					}
				}

				if (!alive || g_ctx.local_player->IsControllingBot()|| !current_config->wearable.info || current_config->wearable.info->iItemDefinitionIndex == 0)
				{
					SourceEngine::IClientNetworkable* networkable_wearable;

					if (wearable)
					{
						networkable_wearable = wearable->GetClientNetworkable();

						wearable->UnEquip(g_ctx.local_player);
					}
					else
					{
						networkable_wearable = entitylist->GetClientNetworkableFromHandle(wearables_handle[0]);
					}
					if (networkable_wearable)
					{
						networkable_wearable->SetDestroyedOnRecreateEntities();
						networkable_wearable->Release();
					}
					wearables_handle[0] = INVALID_EHANDLE_INDEX;
					created_wearables = INVALID_EHANDLE_INDEX;
#ifdef CHANGER_PROFILE
					__itt_task_end(domain);
#endif
					return;
				}

				if (!wearable || wearables_handle[0] == INVALID_EHANDLE_INDEX)
				{
					const int iEntry = entitylist->m_iMaxUsedServerIndex + 1;
					const int iSerialNumber = Utils::RandomInt(0, 0xFFF);
					SourceEngine::IClientNetworkable* new_wearables = CreateWearables(iEntry, iSerialNumber);
					if (new_wearables)
					{
						wearables_handle->Init(iEntry, iSerialNumber);
						created_wearables.Init(iEntry, iSerialNumber);
					}
				}

				if (wearables_handle[0] != INVALID_EHANDLE_INDEX)
				{
					wearable = reinterpret_cast<C_CEconWearable*>
						(entitylist->GetClientEntityFromHandle(wearables_handle[0]));
					if (wearable)
					{
						short* itemdefinitionindex = wearable->GetItemDefinitionIndex();
						int* fallbackpaintkit = wearable->GetFallbackPaintKit();
						const bool idi_changed = *itemdefinitionindex != current_config->wearable.info->iItemDefinitionIndex;
						if (idi_changed ||
							*fallbackpaintkit != current_config->wearable.skin->iPaintKitID ||
							need_forceupdate_for_wearable )
						{
							auto* const globalvars = SourceEngine::interfaces.GlobalVars();
							if(!wearable_random_model_not_setted)
							{
								float* fallbackwear = wearable->GetFallbackWear();
								int* itemidhigh = wearable->GetItemIDHigh();
								int* accountid = wearable->GetAccountID();
								int* fallbackseed = wearable->GetFallbackSeed();
								int* fallbackstattrack = wearable->GetFallbackStatTrack();
								int* quality = wearable->GetEntityQuality();
								bool* initialized = wearable->GetInitialized();

								*accountid = g_ctx.player_info.xuidlow;
								*itemidhigh = -1;
								*fallbackseed = current_config->wearable.iSeed;
								*fallbackstattrack = -1;
								*quality = 4;
								if (float_functions::is_equal(current_config->wearable.fWear, 0.0f))
								{
									current_config->wearable.fWear = 0.000001f;
								}
								*fallbackwear = current_config->wearable.fWear;
								if (idi_changed)
									current_config->wearable.model_indexes.iViewModel = -1;
								current_config->wearable.Update();
								wearable->SetModelIndex(current_config->wearable.model_indexes.iViewModel);//!!!!!!
								*fallbackpaintkit = current_config->wearable.skin->iPaintKitID;
								*itemdefinitionindex = current_config->wearable.info->iItemDefinitionIndex;
								*initialized = true;
								//wearable->SetModelIndex(current_config->wearable.model_indexes.iWorldModel);
								*g_ctx.local_player->Body() = 1;
								
								
								wearable_set_random_model_tick = globalvars->tickcount + 2;
								wearable_random_model_not_setted = true;
								need_forceupdate_for_wearable = true;
							}
							if(wearable_random_model_not_setted && wearable_set_random_model_tick <= globalvars->tickcount)
							{
								wearable->Equip(g_ctx.local_player);
								auto* pEconItemView = wearable->GetEconItemView();

								pEconItemView->ClearCustomMaterials(true);
								auto* visual_processors = pEconItemView->GetVec();
								auto* vec2 = reinterpret_cast<SourceEngine::CUtlVector<SourceEngine::__IRefCounted*>*>(visual_processors);

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
								visual_processors->RemoveAll();
								
								wearable->InitializeAttributes();
								//pEconItemView->UpdateGeneratedMaterial(false, SourceEngine::COMPOSITE_TEXTURE_SIZE_512);
								auto* viewmodel = static_cast<SourceEngine::IBaseViewModel*>(g_ctx.local_player->GetViewModel());
								viewmodel->RemoveAttachmentModelForWearable(g_ctx.local_player, wearable);

								viewmodel->UpdateAllViewmodelAddons();
								need_forceupdate_for_wearable = false;
								wearable_random_model_not_setted = false;
								wearable_set_random_model_tick = 0;
							}
						}
					}
				}
			}
			
		}
	}
}
