#include "Hooks.hpp"
#include "CustomWinAPI.h"

bool vecPressedKeys[256] = {};

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall Hooks::Hooked_WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	//Captures the keys states
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		vecPressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		vecPressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		vecPressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		vecPressedKeys[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		vecPressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		if((wParam == VK_ESCAPE || wParam == SourceEngine::ButtonCode_ButtonCodeToVirtualKey(Options::menu_key)) && vecPressedKeys[wParam])
		{
			Options::menu.main_window = false;
			//pass button to imgui for correcting input
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		}
		vecPressedKeys[wParam] = false;
		break;
	default:
		break;
	}


	//DEPRECATED!
	//if (vecPressedKeys[VK_HOME])
	//{
	//	isClicked = false;
	//	isDown = true;
	//}
	//else
	//{
	//	isClicked = isDown;
	//	isDown = false;
	//}
	//if (isClicked)
	//{
	//	Options::menu.main_window = !Options::menu.main_window;
	//	Options::menu.was_opened = false;
	//}
	

	if (g_bWasInitialized && Options::menu.main_window)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true; //Input was consumed, return
	}

	return g_pWinApi->dwCallWindowProcW(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
	////Processes the user input using ImGui_ImplDX9_WndProcHandler
	//return g_bWasInitialized && Options::menu.main_window && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) ||
	//	g_pWinApi->dwCallWindowProcW(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
}
