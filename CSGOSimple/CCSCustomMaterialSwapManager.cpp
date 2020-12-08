#include "CCSCustomMaterialSwapManager.h"

#include "CSGOStructs.hpp"
#include "IMaterialSystem.h"

void CCSCustomMaterialSwapManager::RequestMaterialSwap(SourceEngine::CBaseHandle hOwner, int nCustomMaterialIndex, SourceEngine::CCustomMaterial* pNewCustomMaterialInterface)
{
	int nSwapIndex = m_pPendingSwaps.AddToTail();
	CCSPendingCustomMaterialSwap_t& materialSwap = m_pPendingSwaps[nSwapIndex];

	materialSwap.m_hOwner = hOwner;
	materialSwap.m_nCustomMaterialIndex = nCustomMaterialIndex;

	materialSwap.m_pNewCustomMaterial = pNewCustomMaterialInterface;
	materialSwap.m_pNewCustomMaterial->AddRef();

	auto owner = (C_BaseCombatWeapon*)SourceEngine::interfaces.EntityList()->GetClientEntityFromHandle(hOwner);
	auto custom_materials = owner->GetCustomMaterials();
	SourceEngine::CCustomMaterial* pOldCustomMaterialInterface = nullptr;
	if(custom_materials->Count())
		pOldCustomMaterialInterface = custom_materials->Element(nCustomMaterialIndex);
	else
	{
		custom_materials->AddToTail();
		pOldCustomMaterialInterface = custom_materials->Element(0);
	}
	
	materialSwap.m_pOldCustomMaterial = pOldCustomMaterialInterface;
	if (materialSwap.m_pOldCustomMaterial)
	{
		materialSwap.m_pOldCustomMaterial->AddRef();
	}
}