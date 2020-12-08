#include "Hooks.hpp"

SourceEngine::CUtlVector<const char*>* t_models_spoof;
SourceEngine::CUtlVector<const char*>* ct_models_spoof;

SourceEngine::CUtlVector<const char*>* __fastcall Hooks::Hooked_GetTModelsForMap(SourceEngine::IGameTypes* ecx, void* /*edx*/,const char* map_name )
{
	if (!Options::config_t.enable_custom_players)
		return static_cast<SourceEngine::CUtlVector<const char*>*>(g_fnOriginalGetTModelsForMap_t(ecx, map_name));
	//changer_log->msg("Hooked_GetTModelsForMap: ");
	//changer_log->msg(map_name);

	if (!t_models_spoof)
		t_models_spoof = new (local_alloc1(sizeof(SourceEngine::CUtlVector<const char*>))) SourceEngine::CUtlVector<const char*>();

	t_models_spoof->RemoveAll();
	t_models_spoof->EnsureCount(5);
	
	for (int i = 0; i < 5; ++i)
	{
		if (Options::config_t.team.skins[i])
		{
			t_models_spoof->Element(i) = Options::config_t.team.skins[i]->model_player.c_str();
		}
		else
		{
			t_models_spoof->Element(i) = g_v_T_TeamSkins.at(0).model_player.c_str();
		}
	}
	//changer_log->msg("Hooked_GetTModelsForMap: result");
	return t_models_spoof;
}

SourceEngine::CUtlVector<const char*>* __fastcall Hooks::Hooked_GetCTModelsForMap(SourceEngine::IGameTypes* ecx, void* /*edx*/, const char* map_name)
{
	if (!Options::config_ct.enable_custom_players)
		return static_cast<SourceEngine::CUtlVector<const char*>*>(g_fnOriginalGetCTModelsForMap_t(ecx, map_name));
	
	//changer_log->msg("Hooked_GetCTModelsForMap: ");
	//changer_log->msg(map_name);

	if (!ct_models_spoof)
		ct_models_spoof = new (local_alloc1(sizeof(SourceEngine::CUtlVector<const char*>))) SourceEngine::CUtlVector<const char*>();

	ct_models_spoof->RemoveAll();
	ct_models_spoof->EnsureCount(5);
	
	for (int i = 0; i < 5; ++i)
	{
		if (Options::config_ct.team.skins[i])
		{
			ct_models_spoof->Element(i) = Options::config_ct.team.skins[i]->model_player.c_str();
		}
		else
		{
			ct_models_spoof->Element(i) = g_v_CT_TeamSkins.at(0).model_player.c_str();
		}
	}
	//changer_log->msg("Hooked_GetCTModelsForMap: result");
	return ct_models_spoof;
}