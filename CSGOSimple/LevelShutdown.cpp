#include "Hooks.hpp"

namespace Hooks
{
	void __fastcall Hooked_LevelShutdown(void* ecx, void* /*edx*/)
	{
		g_fnOriginalLevelShutdown(ecx);
		g_ctx.local_player_index = -1;
		g_ctx.local_player = nullptr;
		g_ctx.player_info.clear();
		ent_listner->on_level_shutdown();
	}
}