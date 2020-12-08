// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "banner.h"
//#include "ImGUI/imgui.h"
//#include "d3dx9tex.h"
#include "Hooks.hpp"
#include "CustomWinAPI.h"
//#pragma comment(lib, "d3dx9.lib")

//
//banner::banner(std::string _url, std::wstring _texture_filename, std::basic_string<unsigned char>* _texture_data,
//               const size_t texture_size):
//	url(move(_url)), texture_filename(move(_texture_filename)),
//	texture_data(_texture_data), render_texture(nullptr), is_texture_ready_to_draw(false), want_to_draw(false),
//	texture_size_(texture_size)
//{
//	//if (!texture_size_)
//	//{
//	//	if (FILE* file = _wfopen(_texture_filename.c_str(), L"r"))
//	//	{
//	//		fclose(file);
//	//	}
//	//	else
//	//	{
//	//		//TODO: file not found, reload?
//	//	}
//	//}
//}
//
//void banner::CreateTextures(LPDIRECT3DDEVICE9 device)
//{
//	//if (texture_size_ > 0)
//	//{
//	//	
//	//	if (FAILED(g_pWinApi->dwD3DXCreateTextureFromFileInMemoryEx(device, texture_data->c_str(), texture_size_, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &render_texture)))
//	//	{
//	//		is_texture_ready_to_draw = false;
//	//		return;
//	//	}
//	//	is_texture_ready_to_draw = true;
//	//}
//	//else
//	//{
//	//	//if (FAILED(D3DXCreateTextureFromFileW(device, texture_filename.c_str(), &render_texture)))
//	//	if (FAILED(g_pWinApi->dwD3DXCreateTextureFromFileExW(device, texture_filename.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &render_texture)))
//	//	{
//	//		is_texture_ready_to_draw = false;
//	//		return;
//	//	}
//	//	is_texture_ready_to_draw = true;
//	//}
//}
//
//void banner::OnReset()
//{
//	//render_texture->Release();
//	//render_texture = nullptr;
//	//is_texture_ready_to_draw = false;
//}
//
//void banner::Render()
//{
//	//if (is_texture_ready_to_draw)
//	//{
//	//	want_to_draw = true;
//	//	if (ImGui::ImageButton(reinterpret_cast<ImTextureID*>(render_texture), ImVec2(416, 60)))
//	//	{			
//	//		g_pWinApi->dwShellExecuteA(nullptr, XorStr("open"), url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
//	//	}
//	//}
//}
