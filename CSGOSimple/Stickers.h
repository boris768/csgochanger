#pragma once
#include <memory>
#include "Options.hpp"

struct _offsets_stickers
{
	ptrdiff_t m_vStickerMaterialReferences{};

	_offsets_stickers();
};

extern _offsets_stickers* offsets_stickers;
void GenerateStickers(SourceEngine::C_EconItemView* iconitemview, const std::vector<StickerInfo>& vAppliedStickers,
                      const CustomSkinWeapon* config_weapon);
