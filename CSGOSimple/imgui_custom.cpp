#include "ImGUI/imgui.h"
#include "ImGUI/imconfig.h"
#include "ImGUI/imgui_internal.h"
#include "Skins.h"
#include "CustomWinAPI.h"
#include "CSGOStructs.hpp"
#include "XorStr.hpp"

namespace ImGui
{
	void hotkey(ButtonCode_t& key, ButtonCode_t default_code) noexcept
	{
		key ? Text("[ %s ]", SourceEngine::GetStringFromButtonCode_t(key)) : TextUnformatted("[ key ]");

		if (!IsItemHovered())
			return;

		SetTooltip("Press any key to change keybind");
		for (int i = static_cast<int>(BUTTON_CODE_NONE); i < KEY_COUNT; ++i)
		{
			const auto vk_key = SourceEngine::ButtonCode_ButtonCodeToVirtualKey(static_cast<ButtonCode_t>(i));
			if (IsKeyReleased(vk_key) && i != key)
				key = vk_key != VK_ESCAPE ? static_cast<ButtonCode_t>(i) : default_code;
		}
	}

	bool ColorEdit4(const char* label, uint8_t* col)
	{
		float col4[4];
		col4[0] = static_cast<float>(col[0]) / 256.f;
		col4[1] = static_cast<float>(col[1]) / 256.f;
		col4[2] = static_cast<float>(col[2]) / 256.f;
		col4[3] = static_cast<float>(col[3]) / 256.f;
		const bool value_changed = ColorEdit4(label, col4, true);
		col[0] = static_cast<char>(col4[0] * 256);
		col[1] = static_cast<char>(col4[1] * 256);
		col[2] = static_cast<char>(col4[2] * 256);
		col[3] = static_cast<char>(col4[3] * 256);
		return value_changed;
	}

	char* utf8_encode(const wchar_t* wstr)
	{
		if (wstr == nullptr)
			return nullptr;
		const size_t size_input = wcslen(wstr);
		const size_t size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(size_input), nullptr, 0, nullptr, nullptr);
		char* out = new(MemAlloc(size_needed + 2)) char[size_needed + 1];
		memset(out, '\0', size_needed);
		WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(size_input), out, size_needed, nullptr, nullptr);
		//size_t runtime_check = strlen(out);
		out[size_needed] = '\0';
		return out;
	}

	bool get_system_font_path(const std::string& name, std::string& path)
	{
		//
		// This code is not as safe as it should be.
		// Assumptions we make:
		//  -> GetWindowsDirectoryA does not fail.
		//  -> The registry key exists.
		//  -> The subkeys are ordered alphabetically
		//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
		//

		char buffer[MAX_PATH];
		HKEY registryKey;

		if (g_pWinApi->dwGetWindowsDirectoryA(buffer, MAX_PATH) == 0)
		{
			return false;
		}

		const std::string fontsFolder = buffer + std::string(XorStr("\\Fonts\\"));

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, XorStr("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), 0, KEY_READ,
			&registryKey))
		{
			return false;
		}

		uint32_t valueIndex = 0;
		char valueName[MAX_PATH];
		uint8_t valueData[MAX_PATH];
		//    std::wstring wsFontFile;

		do
		{
			uint32_t valueNameSize = MAX_PATH;
			uint32_t valueDataSize = MAX_PATH;
			uint32_t valueType;

			const auto error = RegEnumValueA(
				registryKey,
				valueIndex,
				valueName,
				reinterpret_cast<DWORD*>(&valueNameSize),
				nullptr,
				reinterpret_cast<DWORD*>(&valueType),
				valueData,
				reinterpret_cast<DWORD*>(&valueDataSize));

			valueIndex++;

			if (error == ERROR_NO_MORE_ITEMS)
			{
				RegCloseKey(registryKey);
				return false;
			}

			if (error || valueType != REG_SZ)
			{
				continue;
			}

			if (_strnicmp(name.data(), valueName, name.size()) == 0)
			{
				path = fontsFolder + std::string(reinterpret_cast<char*>(valueData), valueDataSize);
				RegCloseKey(registryKey);
				return true;
			}
		} while (true);
	}

	void StyleColorsBlack()
	{
		ImGuiStyle* style = &GetStyle();
		ImVec4* colors = style->Colors;

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

		colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		//colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		//colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		//colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		//colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	}


}
