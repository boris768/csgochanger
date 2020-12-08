#include "C_EconItemView.h"
#include "Config.h"
#include "Utils.hpp"
#include "XorStr.hpp"


int SourceEngine::C_EconItemView::Update()
{
	return offsets_c_econitemview->update(this);
}

void SourceEngine::C_EconItemView::UpdateGeneratedMaterial(bool bIgnorePicMip = false, CompositeTextureSize_t diffuseTextureSize = COMPOSITE_TEXTURE_SIZE_512)
{
	m_bMaterialsGenerated = false;
	offsets_c_econitemview->updategeneratedmaterial(this, bIgnorePicMip, -1, diffuseTextureSize);
	//offsets_c_econitemview->updategeneratedmaterial(this, 1, -1, 9);
	//m_bMaterialsGenerated = false;
	//offsets_c_econitemview->updategeneratedmaterial(this, 1, 9);
}

SourceEngine::CUtlVector<SourceEngine::IVisualsDataProcessor*>* SourceEngine::C_EconItemView::GetVec()
{
	return reinterpret_cast<CUtlVector<IVisualsDataProcessor*>*>(reinterpret_cast<uintptr_t>(this) + offsets_c_econitemview->m_VisualsDataProcessor);

}

SourceEngine::CUtlVector<stickerMaterialReference_t>* SourceEngine::C_EconItemView::stickers_vector()
{
	return reinterpret_cast<CUtlVector<stickerMaterialReference_t>*>(reinterpret_cast<uintptr_t>(this) + offsets_stickers->m_vStickerMaterialReferences);
}

SourceEngine::CCStrike15ItemDefinition* SourceEngine::C_EconItemView::GetStaticData()
{
	return offsets_c_econitemview->GetStaticData(this);
}

SourceEngine::CEconItem* SourceEngine::C_EconItemView::GetSOCData()
{
	return offsets_c_econitemview->GetSOCData(this);
}

_offsets_c_econitemview::_offsets_c_econitemview()
{		
	// doesn't work
	update = reinterpret_cast<Update_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC A1 ? ? ? ? 83 EC 3C 53 56 8B")));
	//В этом месте когда-то появлялись траблы, и это ему "ребутовец" фиксил)
	//заебись пофиксили, они вообще в курсе че это вообще, хз,  я не особо в этом шарю, Я апрувил еслои оно работало, Антон апрувил если ему качество кода более менее нравилось, спустя там 1-23-4-5-6-14943698 правок от данилы после получения пиздов за гавнокод охуенно
	updategeneratedmaterial = reinterpret_cast<UpdateGeneratedMaterial_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 53 56 57 8B 7D 10 8B F1")));

	m_VisualsDataProcessor = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("81 C7 ? ? ? ? 8B 4F 0C 8B 57 04 89 4C")) + 2);
	GetStaticData = reinterpret_cast<GetStaticData_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 53 8B D9 8B 0D ? ? ? ? 56 57 8B B9")));
	GetSOCData = reinterpret_cast<GetSOCData_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86")));
}
