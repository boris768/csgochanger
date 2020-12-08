#include "CustomWinAPI.h"
#include "Hooks.hpp"
#include "matchmaking.h"
#include "KeyValues.h"
#include "Utils.hpp"
#include "XorStr.hpp"

void debug_output_event_data(SourceEngine::KeyValues* debug_event)
{
	const char* name = debug_event->GetName();

	Hooks::changer_log->msg(name);
	Hooks::changer_log->msg("\n");

	for (auto kv = debug_event->GetFirstSubKey(); kv; kv = kv->GetNextKey())
	{
		Hooks::changer_log->msg(kv->GetName());
		Hooks::changer_log->msg(" : ");
		switch (kv->GetDataType())
		{
		case SourceEngine::KeyValues::TYPE_STRING:
			Hooks::changer_log->msg(kv->GetString());
			break;
		case SourceEngine::KeyValues::TYPE_INT:
		case SourceEngine::KeyValues::TYPE_PTR:
			Hooks::changer_log->msg(std::to_string( kv->GetInt()));
			break;
		case SourceEngine::KeyValues::TYPE_FLOAT:
			Hooks::changer_log->msg(std::to_string(kv->GetFloat()));
			break;
		case SourceEngine::KeyValues::TYPE_WSTRING:
			Hooks::changer_log->msg("wstr not supported");
			break;
		case SourceEngine::KeyValues::TYPE_NONE:
			Hooks::changer_log->msg("no type");
			break;
		case SourceEngine::KeyValues::TYPE_UINT64:
		case SourceEngine::KeyValues::TYPE_COLOR:
		case SourceEngine::KeyValues::TYPE_COMPILED_INT_BYTE:
		case SourceEngine::KeyValues::TYPE_COMPILED_INT_0:
		case SourceEngine::KeyValues::TYPE_COMPILED_INT_1:
		case SourceEngine::KeyValues::TYPE_NUMTYPES:
			Hooks::changer_log->msg("unsupported value");
			break;
		}
		Hooks::changer_log->msg("\n");
	}
	Hooks::changer_log->msg("-----------------------------------------------------------------");
	Hooks::changer_log->msg("\n");
}

bool SetLocalPlayerReady()
{
	if (Hooks::IsReady) //&& **reinterpret_cast<DWORD**>(reinterpret_cast<DWORD>(accept_game) + 0x7))
	{
		Hooks::IsReady("");
		return true;
	}
	return false;
}


void Hooks::auto_accept() noexcept
{
	do
	{
		g_pWinApi->dwShowWindow(g_hWindow, SW_RESTORE);
		g_pWinApi->dwSetActiveWindow(g_hWindow);
		g_pWinApi->dwSleep(10);
	} while (g_pWinApi->dwGetForegroundWindow() != g_hWindow);

	//disable menu and enable mouse input
	if (Options::menu.main_window)
		Options::menu.main_window = false;

	Options::menu.was_opened = false;

	const DWORD dwStyle = g_pWinApi->dwGetWindowLongW(g_hWindow, GWL_STYLE);
	int x = 0, y = 0;
	RECT rect = { NULL,NULL,NULL,NULL };
	if (g_pWinApi->dwGetWindowRect(g_hWindow, &rect) != FALSE)
	{
		x = rect.left;
		y = rect.top;
	}
	int width = 0, height = 0;
	SourceEngine::interfaces.Engine()->GetScreenSize(width, height);
	width = static_cast<int>(width / 2);
	height = static_cast<int>(height / 2) + 20; // SHITTI VOLVO
	g_pWinApi->dwSetCursorPos(x + width, y + height + (dwStyle & WS_BORDER ? 23 : 0));
	g_pWinApi->dwSendMessageW(g_hWindow, WM_LBUTTONDOWN, VK_LBUTTON, 0x1000000);
	g_pWinApi->dwSleep(29);
	g_pWinApi->dwSendMessageW(g_hWindow, WM_LBUTTONUP, VK_LBUTTON, 0x1000000);
	g_pWinApi->dwSleep(29);
	g_pWinApi->dwSetCursorPos(x + width, y + height + (dwStyle & WS_BORDER ? 23 : 0) + 2);
}

void Hooks::flash_window() noexcept
{
	FLASHWINFO fi = {
		sizeof(FLASHWINFO),
		g_hWindow,
		FLASHW_ALL | FLASHW_TIMERNOFG,
		0,
		0
	};
	g_pWinApi->dwFlashWindowEx(&fi);
}

changer_match_events_sink::changer_match_events_sink()
{
	SourceEngine::interfaces.MatchFramework()->GetEventsSubscription()->Subscribe(this);
}

changer_match_events_sink::~changer_match_events_sink()
{
	SourceEngine::interfaces.MatchFramework()->GetEventsSubscription()->Unsubscribe(this);
}

bool should_exit_accept_thread = false;
volatile bool should_accept = false;
HANDLE accept_thread_handle = nullptr;
unsigned __cdecl accept_thread(void*)
{
	while (!should_exit_accept_thread)
	{
		if (should_accept)
		{
			g_pWinApi->dwSleep(1000);
			if (!SetLocalPlayerReady())
			{
				Hooks::auto_accept();
			}
			Hooks::flash_window();
		}
		g_pWinApi->dwSwitchToThread();
		g_pWinApi->dwSleep(300);
	}
	return 0;
}

void changer_match_events_sink::OnEvent(SourceEngine::KeyValues* pEvent)
{
	if (!Options::enable_auto_accept)
		return;
	
	if (!accept_thread_handle)
	{
		accept_thread_handle = g_pWinApi->dwCreateSimpleThread(accept_thread, nullptr, 0);
	}
	
	if (!pEvent)
		return;
	
	//debug_output_event_data(pEvent);
	const auto name = pEvent->GetName();
	
	if(!name || strcmp(name, XorStr("OnMatchSessionUpdate")) != 0)
		return;
	IMatchSession* match_session = SourceEngine::interfaces.MatchFramework()->GetMatchSession();
	if (!match_session)
		return;
	SourceEngine::KeyValues* session_settings = match_session->GetSessionSettings();
	if (!session_settings)
		return;
	// search state reserved = confirming match/accept button popup
	const char* state = session_settings->GetString(XorStr("game/mmqueue"));
	if(strcmp(state, XorStr("reserved")) != 0)
		return;
	
	should_accept = true;
	//Hooks::changer_log->msg(state);
	//Hooks::changer_log->msg("\n");
	//debug_output_event_data(session_settings);
}
