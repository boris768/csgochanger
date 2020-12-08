#pragma once
#include "DrawManager.hpp"

namespace SourceEngine {
	class IMaterialSystem;
	class ISurface;
}

bool    ImGui_SourceEngine_Init(void* hwnd, SourceEngine::ISurface* surface, SourceEngine::IMaterialSystem *materialsystem);
void	ImGui_SourceEngine_RenderDrawLists(ImDrawData* draw_data);
bool	ImGui_SourceEngine_CreateFontsTexture();
bool ImGui_SourceEngine_CreateDeviceObjects();
void	ImGui_SourceEngine_InvalidateDeviceObjects();
void	ImGui_SourceEngine_NewFrame();