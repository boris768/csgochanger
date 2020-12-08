#pragma once
#include "SourceEngine/Definitions.hpp"

class CGameUI {
public:
	void message_box(const char* title, const char* text)
	{
		using original_fn = void(__thiscall*)(void*, const char*, const char*, bool, bool, const char*, const char*, const char*, const char*, const char*);
		SourceEngine::CallVFunction<original_fn>(this, 20)(this, title, text, true, false, "", "", "", "","");
	}

};
