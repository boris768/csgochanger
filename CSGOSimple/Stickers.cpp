// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Stickers.h"

#include <functional>

#include "C_EconItemView.h"
#include "Skins.h"
#include "IMaterialSystem.h"
#include "IMaterial.h"
#include "Utils.hpp"
#include "Hooks.hpp"
#include "XorStr.hpp"

_offsets_stickers* offsets_stickers;

_offsets_stickers::_offsets_stickers()
{
	m_vStickerMaterialReferences = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(
		SourceEngine::memory.client(), XorStr("02 8B 88 ? ? ? ? 03 CE E8")) + 3);

	// materials \ models \ weapons \ customization \ patches
}

//std::vector<std::string> created_materials_1st;
//std::vector<std::string> created_materials_3rd;
//
//void invalidate_materials(std::vector<std::string>& list)
//{
//	SourceEngine::IMaterialSystem* materialsystem = SourceEngine::interfaces.MaterialSystem();
//	for (auto it = list.cbegin(); it != list.cend(); ++it)
//	{
//		SourceEngine::IMaterial* material = materialsystem->FindProceduralMaterial(
//			it->c_str(), XorStr("Other textures"), nullptr);
//		if (!material->IsErrorMaterial())
//		{
//			material->DecrementReferenceCount();
//		}
//	}
//	list.clear();
//}
//
//void invalidate_all_materials()
//{
//	invalidate_materials(created_materials_1st);
//	invalidate_materials(created_materials_3rd);
//}

SourceEngine::ConVar* cl_righthand = nullptr;

void GenerateStickers(SourceEngine::C_EconItemView* iconitemview, const std::vector<StickerInfo>& vAppliedStickers,
                      const CustomSkinWeapon* config_weapon)
{
	//auto vector = reinterpret_cast<SourceEngine::CUtlVector<stickerMaterialReference_t>*>(iconitemview + offsets_stickers->m_vStickerMaterialReferences);
	SourceEngine::CUtlVector<stickerMaterialReference_t>* vector = iconitemview->stickers_vector();

	vector->RemoveAll();
	SourceEngine::IMaterialSystem* materialsystem = SourceEngine::interfaces.MaterialSystem();
	const StickerInfo* end = vAppliedStickers.data() + vAppliedStickers.size();
	for (const StickerInfo* it = vAppliedStickers.data(); it != end; ++it)
	{
		const unsigned int position = it->iPosition;
		const CustomSkinWeapon::CustomWeaponSkinSticker& settings = config_weapon->Stickers[position];

		std::string szHash(Utils::format_to_string(XorStr("%f_%f_%f"), settings.fStickerRotation, settings.fStickerScale, settings.fStickerWear));
		const unsigned uHash = fnv::hash_runtime(szHash.c_str());
		
		const char cDecal = static_cast<char>('a' + position);

		std::string szDecal(Utils::format_to_string(XorStr("%s_%s_%c"), config_weapon->info->szStickerModel.c_str(), XorStr("decal"), cDecal));


		std::string szMaterialName1st(Utils::format_to_string(XorStr("sticker_%s_%u"), it->sticker_info->szStickerMaterial.c_str(), uHash));
		
		//std::string szMaterialName1st(XorStr("sticker_"));
		//szMaterialName1st.append(szDecal);
		//szMaterialName1st += '_';
		//szMaterialName1st.append(it->sticker_info->szStickerMaterial);
		//szMaterialName1st += '_';
		//szMaterialName1st.append(std::to_string(uHash));

		std::string szMaterialName3rd(szMaterialName1st);
		szMaterialName3rd.append(XorStr("_3rd"));
		szMaterialName1st.append(XorStr("_1st"));

		SourceEngine::IMaterial* pFirstPersonMaterial = materialsystem->FindProceduralMaterial(
			szMaterialName1st.c_str(), XorStr("Other textures"), nullptr);
		SourceEngine::IMaterial* pThirdPersonMaterial = materialsystem->FindProceduralMaterial(
			szMaterialName3rd.c_str(), XorStr("Other textures"), nullptr);
		const auto need_update_first = pFirstPersonMaterial->IsErrorMaterial();
		const auto need_update_third = pThirdPersonMaterial->IsErrorMaterial();
		if (need_update_first || need_update_third)
		{
			//invalidate_materials(created_materials_1st);
			//invalidate_materials(created_materials_3rd);
			std::string	szPath(XorStr("materials/models/weapons/customization/"));
			if(it->sticker_info->is_patch)
			{
				szPath.append(XorStr("patches/"));
			}
			else
			{
				szPath.append(XorStr("stickers/"));
			}
			szPath.append(it->sticker_info->szStickerMaterial);
			szPath.append(XorStr(".vmt"));
			auto* kv = new SourceEngine::KeyValues(XorStr("vmt"));
			if (kv->LoadFromFile(reinterpret_cast<DWORD>(SourceEngine::interfaces.FileSystem()), szPath.c_str(), XorStr("GAME")))
			{
				kv->SetFloat(XorStr("$wearprogress"), settings.fStickerWear);
				kv->SetFloat(XorStr("$patternrotation"), settings.fStickerRotation);
				kv->SetFloat(XorStr("$patternscale"), settings.fStickerScale);
				if (need_update_first)
				{

					std::string szStickerSlotMaterial(
						Utils::format_to_string(XorStr("materials/models/weapons/customization/%s/%s_decal_%c.vmt"), 
							config_weapon->info->szStickerModel.c_str(), config_weapon->info->szStickerModel.c_str(), cDecal));
					//std::string szStickerSlotMaterial(XorStr("materials/models/weapons/customization/"));
					//szStickerSlotMaterial.append(config_weapon->info->szStickerModel);
					//szStickerSlotMaterial += '/';
					//szStickerSlotMaterial.append(config_weapon->info->szStickerModel);
					//szStickerSlotMaterial.append(XorStr("_decal_"));
					//szStickerSlotMaterial += cDecal;
					//szStickerSlotMaterial.append(XorStr(".vmt"));

					auto* kv2 = new SourceEngine::KeyValues(XorStr("vmt"));
					if (kv2->LoadFromFile(reinterpret_cast<DWORD>(SourceEngine::interfaces.FileSystem()),
					                      szStickerSlotMaterial.c_str(), XorStr("GAME"), 0))
					{
						if (static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("cl_righthand")))->GetInt() == 0)
						{
							kv2->SetInt(XorStr("$mirrorhorizontal"), 1);
						}
						kv2->MergeFrom(kv, SourceEngine::KeyValues::MERGE_KV_UPDATE);

						std::string resource_name(XorStr("materials/models/weapons/customization/stickers/default/sticker_proxies_"));
						resource_name.append(std::to_string(position));
						resource_name.append(XorStr(".vmt"));

						auto kv3 = new SourceEngine::KeyValues(XorStr("vmt"));
						if (kv3->LoadFromFile(reinterpret_cast<DWORD>(SourceEngine::interfaces.FileSystem()), resource_name.c_str(),
						                      XorStr("GAME"), 0))
							kv2->MergeFrom(kv3, SourceEngine::KeyValues::MERGE_KV_UPDATE);

						pFirstPersonMaterial = materialsystem->CreateMaterial(szMaterialName1st.c_str(), kv2);
						pFirstPersonMaterial->Refresh();
						//created_materials_1st.emplace_back(szMaterialName1st);

						delete kv3;
					}

					delete kv2;
				}

				if (need_update_third)
				{
					auto* kv2 = new SourceEngine::KeyValues(XorStr("WeaponDecal"));

					kv2->MergeFrom(kv, SourceEngine::KeyValues::MERGE_KV_UPDATE);
					kv2->SetInt(XorStr("$thirdperson"), 1);

					pThirdPersonMaterial = materialsystem->CreateMaterial(szMaterialName3rd.c_str(), kv2);
					pThirdPersonMaterial->Refresh();
					//created_materials_3rd.emplace_back(szMaterialName3rd);
				}

				delete kv;
			}
		}
		stickerMaterialReference_t* stickerMaterialReference = &(*vector)[vector->AddToTail()];
		stickerMaterialReference->FirstPerson.Init(pFirstPersonMaterial);
		stickerMaterialReference->ThirdPerson.Init(pThirdPersonMaterial);
		stickerMaterialReference->iPosition = position;
	}
}
