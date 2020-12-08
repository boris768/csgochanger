#pragma once
#include <cstdint>
#include "SourceEngine/Definitions.hpp"

class CUIEngineSource2
{
public:
	bool BroadcastEvent(uintptr_t pEvent)
	{
		typedef bool(__thiscall* tOriginal)(void*, uintptr_t);
		return SourceEngine::CallVFunction<tOriginal>(this, 53)(this, pEvent);
	}

	void DispatchEvent(uintptr_t pEvent)
	{
		typedef void(__thiscall* tOriginal)(void*, uintptr_t);
		return SourceEngine::CallVFunction<tOriginal>(this, 51)(this, pEvent);
	}
};
