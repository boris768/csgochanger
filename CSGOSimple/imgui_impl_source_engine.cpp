// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "imgui_impl_source_engine.h"
#include <windef.h>
#include <profileapi.h>
#include "ImGUI/imgui.h"
#include "IMaterialSystem.h"
#include "SourceEngine/ISurface.hpp"
#include "CSGOStructs.hpp"
#include "ImGUI/DX9/font_resource.h"
#include "CustomWinAPI.h"

HWND g_hWnd1 = nullptr;
SourceEngine::ISurface* g_pSurface = nullptr;
SourceEngine::IMaterialSystem* g_pMaterialSystem = nullptr;
SourceEngine::IMesh* g_pMesh = nullptr;

int g_FontTexture;

SourceEngine::IVertexBuffer* g_pVertexBuffer;
SourceEngine::IIndexBuffer* g_pIndexBuffer;

int streamID = 0;

INT64 g_TicksPerSecond1 = NULL;
INT64 g_Time1 = NULL;

bool ImGui_SourceEngine_Init(void* hwnd, SourceEngine::ISurface* _surface, SourceEngine::IMaterialSystem* _materialsystem)
{
	HWND g_hWnd = static_cast<HWND>(hwnd);
	g_pSurface = _surface;
	g_pMaterialSystem = _materialsystem;
	if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&g_TicksPerSecond1)))
		return false;
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&g_Time1)))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	io.RenderDrawListsFn = ImGui_SourceEngine_RenderDrawLists;
	io.ImeWindowHandle = g_hWnd;

	return true;
}


//for (int n = 0; n < draw_data->CmdListsCount; n++)
//{
//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
//	const ImDrawIdx* idx_buffer_offset = 0;
//
//	glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
//	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);
//
//	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
//	{
//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
//		if (pcmd->UserCallback)
//		{
//			pcmd->UserCallback(cmd_list, pcmd);
//		}
//		else
//		{
//			glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
//			glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
//			glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
//		}
//		idx_buffer_offset += pcmd->ElemCount;
//	}
//}

//g_pMesh = pRenderContext->GetDynamicMesh(true, nullptr, nullptr, /*g_pSurface->m_pWhite.GetMaterial()*/SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(g_FontTexture));
//
//SourceEngine::CMeshBuilder mesh_builder;
//
//mesh_builder.Begin(g_pMesh, SourceEngine::MATERIAL_TRIANGLES, draw_data->TotalVtxCount, draw_data->TotalIdxCount);
//for (size_t n = 0; n < draw_data->CmdListsCount; ++n)
//{
//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
//	const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
//	for (size_t i = 0; i < cmd_list->VtxBuffer.Size; ++i)
//	{
//		mesh_builder.Position3f(vtx_src->pos.x, vtx_src->pos.y, 0.f);
//		mesh_builder.Color4ub((vtx_src->col >> IM_COL32_R_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_G_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_B_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_A_SHIFT) & 0xFF);
//		mesh_builder.TexCoord2f(0, vtx_src->uv.x, vtx_src->uv.y);
//		mesh_builder.AdvanceVertex();
//		vtx_src++;
//	}
//	for (int j = 0; j < cmd_list->IdxBuffer.Size; ++j)
//	{
//		mesh_builder.FastIndex(cmd_list->IdxBuffer.Data[j]);
//	}
//}
//mesh_builder.End();
//pRenderContext->BeginRender();
//
//int vtx_offset = 0;
//int idx_offset = 0;
//for (size_t n = 0; n < draw_data->CmdListsCount; ++n)
//{
//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
//	for (size_t cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
//	{
//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
//		const RECT r = {
//			static_cast<LONG>(pcmd->ClipRect.x),
//			static_cast<LONG>(pcmd->ClipRect.y),
//			static_cast<LONG>(pcmd->ClipRect.z),
//			static_cast<LONG>(pcmd->ClipRect.w) };
//
//		auto material = SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(reinterpret_cast<int>(pcmd->TextureId));
//		if (material)
//			pRenderContext->Bind(material);
//		else
//			pRenderContext->Bind(g_pSurface->m_pWhite.GetMaterial());
//		SourceEngine::IMesh *mesh = pRenderContext->GetDynamicMesh();
//		//g_pMesh = pRenderContext->GetDynamicMesh(true, nullptr, nullptr, material);
//
//		//int iStartVert,		// Which vertices to copy.
//		//	int nVerts,
//		//	int iStartIndex,	// Which indices to copy.
//		//	int nIndices,
//		//	int indexOffset,	// This is added to each index.
//		//	CMeshBuilder &builder) = 0;
//
//		g_pMesh->CopyToMeshBuilder(vtx_offset, cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount / 3, idx_offset, mesh_builder);
//		mesh_builder.Begin(mesh, SourceEngine::MATERIAL_TRIANGLES, cmd_list->VtxBuffer.Size, cmd_list->IdxBuffer.Size);
//
//
//	}
//}


void ImGui_SourceEngine_RenderDrawLists(ImDrawData* draw_data)
{
	ImGuiIO& io = ImGui::GetIO();

	SourceEngine::IMatRenderContext* pRenderContext = g_pMaterialSystem->GetRenderContext();
	if (!pRenderContext)
	{
		//no render context, dont draw
		return;
	}
	RECT rect;
	g_pWinApi->dwGetClientRect(g_hWnd1, &rect);
	io.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
	
	g_pMesh = pRenderContext->GetDynamicMesh(true, nullptr, nullptr, g_pSurface->m_pWhite.GetMaterial());

	g_pVertexBuffer = pRenderContext->CreateStaticVertexBuffer(g_pMesh->GetVertexFormat(), draw_data->TotalVtxCount + 5000, "ImGui");
	SourceEngine::CVertexBuilder vertex_builder;
	g_pIndexBuffer = pRenderContext->CreateStaticIndexBuffer(SourceEngine::MATERIAL_INDEX_FORMAT_16BIT, draw_data->TotalIdxCount + 5000, "ImGui");
	SourceEngine::CIndexBuilder index_builder;


	pRenderContext->BeginRender();
	vertex_builder.Begin(g_pVertexBuffer, draw_data->TotalVtxCount);
	index_builder.Begin(g_pIndexBuffer, draw_data->TotalIdxCount);
	vertex_builder.Lock(draw_data->TotalVtxCount);
	index_builder.Lock(draw_data->TotalIdxCount, 0);
	for(size_t n = 0; n < draw_data->CmdListsCount; ++n)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		for (size_t i = 0; i < cmd_list->VtxBuffer.Size; ++i)
		{
			{
				vertex_builder.Position3f(vtx_src->pos.x, vtx_src->pos.y, 0.f);
				vertex_builder.Color4ub((vtx_src->col >> IM_COL32_R_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_G_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_B_SHIFT) & 0xFF, (vtx_src->col >> IM_COL32_A_SHIFT) & 0xFF);
				vertex_builder.TexCoord2f(0, vtx_src->uv.x, vtx_src->uv.y);
				vertex_builder.AdvanceVertex();
				vtx_src++;
			}

		}
		for (size_t j = 0; j < cmd_list->IdxBuffer.Size; ++j)
		{
			index_builder.FastIndex(cmd_list->IdxBuffer.Data[j]);
		}
	}
	index_builder.Bind(pRenderContext);
	vertex_builder.Bind(pRenderContext, streamID, g_pMesh->GetVertexFormat());
	vertex_builder.End();
	index_builder.End();
		
	int vtx_offset = 0;
	int idx_offset = 0;	
	for (size_t n = 0; n < draw_data->CmdListsCount; ++n)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];

		for(size_t cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			auto material = SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(reinterpret_cast<int>(pcmd->TextureId));
			if (material)
				pRenderContext->Bind(material);
			else
				pRenderContext->Bind(g_pSurface->m_pWhite.GetMaterial());

			pRenderContext->Draw(SourceEngine::MATERIAL_TRIANGLES, vtx_offset, pcmd->ElemCount / 3);
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
	pRenderContext->DestroyVertexBuffer(g_pVertexBuffer);
	pRenderContext->DestroyIndexBuffer(g_pIndexBuffer);
	//			//STDMETHOD(DrawIndexedPrimitive)
	//			//(THIS_ D3DPRIMITIVETYPE,		D3DPT_TRIANGLELIST
	//			//	INT BaseVertexIndex,		vtx_offset
	//			//	UINT MinVertexIndex,		0
	//			//	UINT NumVertices,			static_cast<UINT>(cmd_list->VtxBuffer.Size)
	//			//	UINT startIndex,			idx_offset
	//			//	UINT primCount) PURE;		pcmd->ElemCount / 3
	//g_pMesh->Draw();

	pRenderContext->EndRender();
	//g_pMesh = pRenderContext->GetDynamicMesh(true, nullptr, nullptr, g_pSurface->m_pWhite.GetMaterial());
	//mesh_builder->Begin(g_pMesh, SourceEngine::MATERIAL_TRIANGLES, draw_data->TotalVtxCount, draw_data->TotalIdxCount);

	//virtual void BindVertexBuffer(int streamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	//virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes) = 0;
	//virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;

	//int vtx_offset = 0;
	//int idx_offset = 0;	
	//for (size_t n = 0; n < draw_data->CmdListsCount; ++n)
	//{
	//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
	//	for (size_t cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
	//	{
	//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
	//		if (pcmd->UserCallback)
	//		{
	//			pcmd->UserCallback(cmd_list, pcmd);
	//		}
	//		else
	//		{
	//			const RECT r = { static_cast<LONG>(pcmd->ClipRect.x), static_cast<LONG>(pcmd->ClipRect.y), static_cast<LONG>(pcmd->ClipRect.z), static_cast<LONG>(pcmd->ClipRect.w) };
	//			auto material = SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(reinterpret_cast<int>(pcmd->TextureId));
	//			if (material)
	//				pRenderContext->Bind(material);
	//			else
	//				pRenderContext->Bind(g_pSurface->m_pWhite.GetMaterial());

	//			g_pMesh = pRenderContext->GetDynamicMesh(true, reinterpret_cast<SourceEngine::IMesh*>(g_pVertexBuffer), reinterpret_cast<SourceEngine::IMesh*>(g_pIndexBuffer));
	//			g_pMesh->Draw(idx_offset, pcmd->ElemCount / 3);
	//			//pRenderContext->PushScissorRect(r.left, r.top, r.right, r.bottom);
	//			//pRenderContext->Draw(SourceEngine::MATERIAL_TRIANGLES, idx_offset, draw_data->TotalIdxCount);
	//			//pRenderContext->PopScissorRect();

	//			
	//			idx_offset += pcmd->ElemCount;
	//			//g_pd3dDevice->SetTexture(0, static_cast<LPDIRECT3DTEXTURE9>(pcmd->TextureId));
	//			//g_pd3dDevice->SetScissorRect(&r);
	//			//g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, static_cast<UINT>(cmd_list->VtxBuffer.Size), idx_offset, pcmd->ElemCount / 3);
	//		}
	//		vtx_offset += cmd_list->VtxBuffer.Size;
	//	}
	//}
	//pRenderContext->DestroyIndexBuffer(g_pIndexBuffer);
	//pRenderContext->DestroyVertexBuffer(g_pVertexBuffer);
}


//int vtx_offset = 0;
//int idx_offset = 0;
//for (size_t n = 0; n < draw_data->CmdListsCount; ++n)
//{
//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
//	for (size_t cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
//	{
//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
//		if (pcmd->UserCallback)
//		{
//			pcmd->UserCallback(cmd_list, pcmd);
//		}
//		else
//		{
//			const RECT r = { static_cast<LONG>(pcmd->ClipRect.x), static_cast<LONG>(pcmd->ClipRect.y), static_cast<LONG>(pcmd->ClipRect.z), static_cast<LONG>(pcmd->ClipRect.w) };
//			auto material = SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(reinterpret_cast<int>(pcmd->TextureId));
//			//auto texture = g_pSurface->DrawSetTexture(pcmd->TextureId)
//			//pRenderContext->SetRenderTarget();
//			if (material)
//			{
//				//STDMETHOD(DrawIndexedPrimitive)
//				//(THIS_ D3DPRIMITIVETYPE, 
//				//INT BaseVertexIndex,
//				//UINT MinVertexIndex,
//				//UINT NumVertices,
//				//UINT startIndex,
//				//UINT primCount) PURE;

//				SourceEngine::IMesh *mesh = pRenderContext->GetDynamicMesh(true, nullptr, nullptr, material);
//				mesh_builder->Begin(mesh, SourceEngine::MATERIAL_TRIANGLES, pcmd->ElemCount / 3);
//				mesh_builder->End();
//				pRenderContext->PushScissorRect(r.left, r.top, r.right, r.bottom);
//				mesh->Draw(idx_offset);
//				pRenderContext->PopScissorRect();
//			}
//			//g_pd3dDevice->SetTexture(0, static_cast<LPDIRECT3DTEXTURE9>(pcmd->TextureId));
//			//g_pd3dDevice->SetScissorRect(&r);
//			//g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, static_cast<UINT>(cmd_list->VtxBuffer.Size), idx_offset, pcmd->ElemCount / 3);
//		}
//		idx_offset += pcmd->ElemCount;
//	}
//	vtx_offset += cmd_list->VtxBuffer.Size;
//}

//pRenderContext
//->
//EndRender();
////vertex_builder.End();
////index_builder.End();
////g_pVertexBuffer->
//
//}

struct font_atlas11
{
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	UINT last_pitch;

	font_atlas11()
	{
		pixels = nullptr;
		width = height = bytes_per_pixel = 0;
		last_pitch = 0;
	}

	font_atlas11(const font_atlas11& in)
	{
		this->width = in.width;
		this->height = in.height;
		this->bytes_per_pixel = in.bytes_per_pixel;
		this->pixels = static_cast<unsigned char*>(malloc(in.width * in.height * 8));
		this->last_pitch = in.last_pitch;
		for (int y = 0; y < this->height; ++y)
			memcpy(static_cast<unsigned char *>(this->pixels) + this->last_pitch * y, in.pixels + (in.width * in.bytes_per_pixel) * y, (in.width * in.bytes_per_pixel));
	}

	font_atlas11& operator=(const font_atlas11& in)
	{
		if (this == &in)
			return *this;
		this->width = in.width;
		this->height = in.height;
		this->bytes_per_pixel = in.bytes_per_pixel;
		this->pixels = static_cast<unsigned char*>(malloc(in.width * in.height * 8));
		this->last_pitch = in.last_pitch;
		for (int y = 0; y < this->height; ++y)
			memcpy(static_cast<unsigned char *>(this->pixels) + this->last_pitch * y, in.pixels + (in.width * in.bytes_per_pixel) * y, (in.width * in.bytes_per_pixel));
		return *this;
	}
}* cached_font;


bool ImGui_SourceEngine_CreateFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	if (!cached_font)
	{
		font_atlas11 font;
		io.Fonts->TexDesiredWidth = 4096;
		auto ranges = io.Fonts->GetGlyphRangesCyrillic();
		ImFontConfig config;
		config.MergeMode = false;
		config.OversampleH = 2;
		config.OversampleV = 1;
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF(XorStr("C:\\Cuprum.ttf"), 15.0f, nullptr, ranges);

		io.Fonts->AddFontFromMemoryTTF(hellfire_font, 100608, 14, &config, ranges);

		io.Fonts->GetTexDataAsRGBA32(&font.pixels, &font.width, &font.height, &font.bytes_per_pixel);

		// Upload texture to graphics system
		auto texturedictionary = SourceEngine::interfaces.TextureDictionary();
		g_FontTexture = texturedictionary->CreateTexture(true);
		texturedictionary->SetTextureRGBA(g_FontTexture, reinterpret_cast<const char*>(font.pixels), font.width, font.height);
		auto check = texturedictionary->GetTextureMaterial(g_FontTexture);
		if (!check->IsErrorMaterial())
		{
			SourceEngine::interfaces.CVar()->ConsolePrintf("material name: %s", check->GetName());
		}
		// Store our identifier
		io.Fonts->TexID = reinterpret_cast<void*>(g_FontTexture);
		cached_font = new font_atlas11(font);
	}
	else
	{
		io.Fonts->TexDesiredWidth = 4096;
		auto ranges = io.Fonts->GetGlyphRangesCyrillic();
		ImFontConfig config;
		config.MergeMode = false;
		config.OversampleH = 2;
		config.OversampleV = 1;

		io.Fonts->AddFontFromMemoryTTF(hellfire_font, 100608, 14, &config, ranges);

		// Upload texture to graphics system
		g_FontTexture = SourceEngine::interfaces.TextureDictionary()->CreateTexture(true); //-V807
		SourceEngine::interfaces.TextureDictionary()->SetTextureRGBA(g_FontTexture, reinterpret_cast<const char*>(cached_font->pixels), cached_font->width, cached_font->height);
		auto check = SourceEngine::interfaces.TextureDictionary()->GetTextureMaterial(g_FontTexture);
		if (!check->IsErrorMaterial())
		{
			SourceEngine::interfaces.CVar()->ConsolePrintf("material name: %s", check->GetName());
		}
		// Store our identifier
		io.Fonts->TexID = reinterpret_cast<void*>(g_FontTexture);
	}


	return true;
}

bool ImGui_SourceEngine_CreateDeviceObjects()
{
	if (!g_pMaterialSystem || !g_pSurface)
		return false;
	if (!ImGui_SourceEngine_CreateFontsTexture())
		return false;
	return true;
}

void ImGui_SourceEngine_InvalidateDeviceObjects()
{
	if (!g_pMaterialSystem || g_pSurface)
		return;
	if (g_pMesh)
	{
		g_pMesh = nullptr;
	}
	SourceEngine::interfaces.TextureDictionary()->DestroyTexture(g_FontTexture);
	//g_FontTexture.reset(nullptr);
}

void ImGui_SourceEngine_NewFrame()
{
	if (!g_FontTexture)
		ImGui_SourceEngine_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	g_pWinApi->dwGetClientRect(g_hWnd1, &rect);
	io.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&current_time));
	io.DeltaTime = static_cast<float>(current_time - g_Time1) / g_TicksPerSecond1;
	g_Time1 = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (g_pWinApi->dwGetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (g_pWinApi->dwGetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (g_pWinApi->dwGetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		g_pWinApi->dwSetCursor(nullptr);

	// Start the frame
	ImGui::NewFrame();
}
