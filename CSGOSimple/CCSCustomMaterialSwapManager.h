#pragma once
#include "SourceEngine/CHandle.hpp"
#include "SourceEngine/IAppSystem.hpp"
#include "SourceEngine/UtlVector.hpp"

namespace SourceEngine {
	class CCustomMaterial;
}

struct CCSPendingCustomMaterialSwap_t
{
	SourceEngine::CCustomMaterial* m_pNewCustomMaterial;
	SourceEngine::CCustomMaterial* m_pOldCustomMaterial;
	int m_nCustomMaterialIndex;
	SourceEngine::CBaseHandle m_hOwner;
};

class CCSCustomMaterialSwapManager : public SourceEngine::IAppSystem
{
public:
	CCSCustomMaterialSwapManager();
	virtual ~CCSCustomMaterialSwapManager();

	virtual SourceEngine::InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	bool Process();

	void RequestMaterialSwap(SourceEngine::CBaseHandle hOwner, int nCustomMaterialIndex, SourceEngine::CCustomMaterial* pNewCustomMaterialInterface);
	void ClearPendingSwaps(SourceEngine::CBaseHandle hOwner, int nCustomMaterialIndex);
	void ClearAllPendingSwaps();

	SourceEngine::CUtlVector< CCSPendingCustomMaterialSwap_t > m_pPendingSwaps;
};
