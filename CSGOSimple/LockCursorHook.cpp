// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hooks.hpp"

void __fastcall Hooks::Hooked_LockCursor(SourceEngine::ISurface* _this, void* /*edx*/)
{
	_this->UnlockCursor();
	if (Options::menu.main_window)
	{
		_this->UnlockCursor();
		return;
	}

	g_fnOriginalLockCursor(_this);
}