#pragma once
#include "SourceEngine/UtlVector.hpp"
#include "CCSWeaponVisualsDataProcessor.h"
#include "Skins.h"
#include "Stickers.h"

using Update_t = int(__thiscall*)(SourceEngine::C_EconItemView*);
using UpdateGeneratedMaterial_t = void(__thiscall*)(void*, int, int, int);
using GetStaticData_t = SourceEngine::CCStrike15ItemDefinition*(__thiscall*)(void*);
using GetSOCData_t = SourceEngine::CEconItem*(__thiscall*)(SourceEngine::C_EconItemView*);

struct _offsets_c_econitemview
{
	Update_t update;
	UpdateGeneratedMaterial_t updategeneratedmaterial;
	ptrdiff_t m_VisualsDataProcessor;
	GetStaticData_t GetStaticData;
	GetSOCData_t GetSOCData;
	_offsets_c_econitemview();
};

extern _offsets_c_econitemview* offsets_c_econitemview;

namespace SourceEngine
{
	class CCustomMaterialOwner
	{
	public:
		virtual ~CCustomMaterialOwner() { ClearCustomMaterials(true); }
		CCustomMaterial* GetCustomMaterial(int nIndex = 0) const; // returns NULL if the index is out of range
		virtual void SetCustomMaterial(CCustomMaterial* pCustomMaterial, int nIndex = 0);	// either replaces and existing material (releasing the old one), or adds one to the vector
		bool IsCustomMaterialValid(int nIndex = 0) const;
		int GetCustomMaterialCount() const { return m_ppCustomMaterials.Count(); }
		void ClearCustomMaterials(bool bPurge = false);
		virtual void OnCustomMaterialsUpdated() {}
		virtual void DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const;

		// Pointers to custom materials owned by the mat system for this entity. Index
		// in this vector corresponds to the model material index to override with the custom material.
		CUtlVector< CCustomMaterial* > m_ppCustomMaterials;
	};

	inline CCustomMaterial* CCustomMaterialOwner::GetCustomMaterial(int nIndex) const
	{
		return (m_ppCustomMaterials.Count() > nIndex) ? m_ppCustomMaterials[nIndex] : nullptr;
	}

	inline void CCustomMaterialOwner::SetCustomMaterial(CCustomMaterial* pCustomMaterial, int nIndex)
	{
		while (m_ppCustomMaterials.Count() <= nIndex)
		{
			m_ppCustomMaterials.AddToTail(nullptr);
		}

		pCustomMaterial->AddRef();
		if (m_ppCustomMaterials[nIndex])
			m_ppCustomMaterials[nIndex]->Release();

		m_ppCustomMaterials[nIndex] = pCustomMaterial;
	}

	inline bool CCustomMaterialOwner::IsCustomMaterialValid(int nIndex) const
	{
		return (m_ppCustomMaterials.Count() > nIndex && m_ppCustomMaterials[nIndex] != nullptr) ? m_ppCustomMaterials[nIndex]->IsValid() : false;
	}

	inline void CCustomMaterialOwner::ClearCustomMaterials(bool bPurge)
	{
		for (int i = 0; i < m_ppCustomMaterials.Count(); i++)
		{
			if (m_ppCustomMaterials[i] != nullptr)
			{
				m_ppCustomMaterials[i]->Release();
				m_ppCustomMaterials[i] = nullptr;
			}
		}

		if (bPurge)
		{
			m_ppCustomMaterials.Purge();
		}
		else
		{
			m_ppCustomMaterials.RemoveAll();
		}
	}

	inline void CCustomMaterialOwner::DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const
	{
		pOther->ClearCustomMaterials();
		for (int i = 0; i < m_ppCustomMaterials.Count(); i++)
		{
			if (m_ppCustomMaterials[i] == nullptr)
				continue;

			pOther->SetCustomMaterial(m_ppCustomMaterials[i], i);
		}
	}

	class CDefaultClientRenderable : public IClientUnknown, public IClientRenderable
	{
		typedef unsigned short ClientRenderHandle_t;

		enum
		{
			INVALID_CLIENT_RENDER_HANDLE = (ClientRenderHandle_t)0xffff,
		};
		ClientRenderHandle_t m_hRenderHandle;
	public:
		CDefaultClientRenderable()
		{
			m_hRenderHandle = INVALID_CLIENT_RENDER_HANDLE;
		}
	};
	
	class C_EconItemView : public CDefaultClientRenderable, public IEconItemInterface, public CCustomMaterialOwner
	{
	public:
		//void* vtable1;	//00
		//void* vtable2;	//04
		//int pad1;		//08
		//void* vtable3;	//0c
		//void* vtable4;	//10
		//CUtlVector <__IRefCounted*> m_CustomMaterials;			//14
		bool m_bMaterialsGenerated;			//20
		char pad[0x1FC];
		CUtlVector<IVisualsDataProcessor*> vec; //220

		int Update();
		void UpdateGeneratedMaterial(bool bIgnorePicMip, CompositeTextureSize_t diffuseTextureSize);
		CUtlVector<IVisualsDataProcessor*>* GetVec();

		CUtlVector<stickerMaterialReference_t>* stickers_vector();

		CCStrike15ItemDefinition* GetStaticData();

		CEconItem* GetSOCData();
	};
}

