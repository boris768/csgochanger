#pragma once
#include <windows.h>
void DrawMenu();
void DrawStat();
void ChangeSky(unsigned id);
void sendClanTag(const char* name, const char* tag);
extern char currentloadedsky[128];
extern HANDLE save_thread_handle;
unsigned __cdecl save_thread(void*);
extern HANDLE load_thread_handle;
unsigned __cdecl read_thread(void*);
extern bool need_forceupdate_for_weapon;
extern bool need_forceupdate_for_knife;
extern volatile bool need_forceupdate_for_wearable;