// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SourceEngine/ISurface.hpp"
#include "CSGOStructs.hpp"
#include "ImGUI/imgui.h"

namespace SourceEngine
{
	Vertex_t::Vertex_t(Vector2D pos, Vector2D coord) : m_Position(std::move(pos)), m_TexCoord(std::move(coord))
	{
	}

	void Vertex_t::Init(Vector2D pos, Vector2D coord)
	{
		m_Position = std::move(pos);
		m_TexCoord = std::move(coord);
	}
	void Vertex_t::Init(const ImVec2& pos, const ImVec2& coord)
	{
		memcpy(&m_Position, &pos, sizeof(float) * 2);
		memcpy(&m_TexCoord, &coord, sizeof(float) * 2);
	}

	TextureHolder::TextureHolder(): m_iTexture(0), m_iW(0), m_iH(0), m_bValid(false)
	{
	}

	TextureHolder::TextureHolder(const unsigned char* pRawRGBAData, int W, int H): m_iW(W), m_iH(H), m_bValid(false)
	{
		m_iTexture = interfaces.MatSurface()->CreateNewTextureID(true);
		if (!m_iTexture)
			return;
		interfaces.MatSurface()->DrawSetTextureRGBA(m_iTexture, pRawRGBAData, W, H);
		//g_pSurface->DrawSetTextureFile(m_iTexture, "test", true, true);
		m_bValid = true;
	}

	TextureHolder::TextureHolder(const char* filename, int W, int H) : m_iW(W), m_iH(H), m_bValid(false)
	{
		m_iTexture = interfaces.MatSurface()->CreateNewTextureID(true);
		if (!m_iTexture)
			return;
		interfaces.MatSurface()->DrawSetTextureFile(m_iTexture, filename, 0, true);
		m_bValid = true;
	}

	TextureHolder::~TextureHolder()
	{
		//g_pSurface->DeleteTextureByID(m_iTexture);	
	}

	bool TextureHolder::IsValid() const
	{
		return m_bValid;
	}

	int TextureHolder::GetTextureId() const
	{
		return m_iTexture;
	}

	bool TextureHolder::Draw(int x, int y, float scale)
	{
		auto surface = interfaces.MatSurface();
		m_bValid = surface->IsTextureIDValid(m_iTexture);

		if (!m_bValid)
		{
			return false;
		}

		surface->DrawSetTexture(m_iTexture);
		surface->DrawSetColor(255, 255, 255, 255);
		surface->DrawTexturedRect(x, y, x + m_iW * scale, y + m_iH * scale);
		return true;
	}
}
