// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IBaseViewModel.h"
#include "Options.hpp"

EXCEPTION_DISPOSITION __stdcall SourceEngine::viewmodel_handler(EXCEPTION_RECORD* ExceptionRecord, PLONG pEstablisherFrame, CONTEXT* ContextRecord, PLONG pDispatcherContext)
{
	ContextRecord->Eip = ContextRecord->Ebx;
	return ExceptionContinueExecution;
}

SourceEngine::iActivity SourceEngine::IBaseViewModel::GetSequenceActivity(int seq)
{
	return netvars_c_base_view_model->GetSequenceActivity(this, seq);
}

SourceEngine::CSequenceTransitioner* SourceEngine::IBaseViewModel::GetSequenceTransitioner()
{
	return GetFieldPointer<SourceEngine::CSequenceTransitioner>(0xa53);
}

void SourceEngine::IBaseViewModel::SetWeaponModel(const char* modelname, void* weapon)
{
	//std::cout << "setting: " << modelname << " setweaponmodel: " << netvars_c_base_combat_weapon->dwSetWeaponModel << std::endl;
	using oSetWeaponModel = void(__thiscall*)(PVOID, const char*, void*);
	reinterpret_cast<oSetWeaponModel>(netvars_c_base_combat_weapon->dwSetWeaponModel)(this, modelname, weapon);
}

void SourceEngine::IBaseViewModel::SendViewModelMatchingSequence(const int sequence)
{
	typedef void (__thiscall* oSendViewModelMatchingSequence)(void*, int);
	return CallVFunction<oSendViewModelMatchingSequence>(this, 246)(this, sequence);
}

int* SourceEngine::IBaseViewModel::GetSequence()
{
	return GetFieldPointer<int>(netvars_c_base_view_model->m_nSequence);
}

void SourceEngine::IBaseViewModel::UpdateAllViewmodelAddons()
{
	netvars_c_base_view_model->UpdateAllViewmodelAddons(this);
}

void SourceEngine::IBaseViewModel::RemoveAttachmentModelForWearable(IClientEntity* player, IClientEntity* wearable)
{
	netvars_c_base_view_model->RemoveAttachmentModelForWearable(this, player, wearable);
}

bool SourceEngine::IBaseViewModel::CreateAttachmentModelForWearable(IClientEntity* player, IClientEntity* wearable)
{
	return netvars_c_base_view_model->CreateAttachmentModelForWearable(this, player, wearable);
}

SourceEngine::CBaseHandle SourceEngine::IBaseViewModel::GetOwner()
{
	return GetFieldValue<CHandle<SourceEngine::IClientEntity>>(netvars_c_base_view_model->m_hOwner);
}
SourceEngine::CBaseHandle SourceEngine::IBaseViewModel::GetWeapon()
{
	return GetFieldValue<CHandle<SourceEngine::IClientEntity>>(netvars_c_base_view_model->m_hWeapon);
}

void SourceEngine::IBaseViewModel::SetModelIndex(int index)
{
	using oSetModelIndex = void(__thiscall*)(void*, int);
	SourceEngine::CallVFunction<oSetModelIndex>(this, netvars_c_base_combat_weapon->dwSetModelIndex)(this, index); // Reverse SetWeaponModel.
}

float* SourceEngine::IBaseViewModel::GetAnimTime()
{
	return GetFieldPointer<float>(netvars_c_base_combat_weapon->m_flAnimTime);
}

bool* SourceEngine::IBaseViewModel::GetClientSideAnimation()
{
	return GetFieldPointer<bool>(netvars_c_base_view_model->m_bClientSideAnimation);
}

bool* SourceEngine::IBaseViewModel::GetClientSideFrameReset()
{
	return GetFieldPointer<bool>(netvars_c_base_view_model->m_bClientSideFrameReset);
}
