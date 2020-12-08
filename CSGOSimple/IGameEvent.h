#pragma once
#include "KeyValues.h"
#include <vector>
#include <unordered_map>
#include "Statistic.h"
#include "SourceEngine/UtlVector.hpp"

extern std::string current_server;
extern long long current_matchid;

namespace SourceEngine
{
	constexpr auto MAX_EVENT_NAME_LENGTH = 32;
	constexpr auto EVENT_DEBUG_ID_INIT = 42;
	constexpr auto EVENT_DEBUG_ID_SHUTDOWN = 13;

	class IGameEvent
	{
	public:
		virtual ~IGameEvent();
		[[nodiscard]] virtual const char* GetName() const = 0;

		[[nodiscard]] virtual bool IsReliable() const = 0;
		[[nodiscard]] virtual bool IsLocal() const = 0;
		[[nodiscard]] virtual bool IsEmpty(const char* keyName = nullptr) = 0;

		[[nodiscard]] virtual bool GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
		[[nodiscard]] virtual int GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
		[[nodiscard]] virtual unsigned long GetUint64(const char* keyName = nullptr, int defaultValue1 = 0, int defaultValue2 = 0) = 0;
		[[nodiscard]] virtual float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;
		[[nodiscard]] virtual const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;
		[[nodiscard]] virtual const wchar_t* GetWString(const char* keyName, const wchar_t* defaultValue = L"") = 0;
		[[nodiscard]] virtual const void* GetPtr(const char* keyname = nullptr, const void* default_values = nullptr) = 0;

		virtual void SetBool(const char* keyName, bool value) = 0;
		virtual void SetInt(const char* keyName, int value) = 0;
		virtual void SetUint64(const char* keyName, unsigned long value) = 0;
		virtual void SetFloat(const char* keyName, float value) = 0;
		virtual void SetString(const char* keyname, const char* value) = 0;
		virtual void SetWString(const char* keyname, const wchar_t* value) = 0;
		virtual void SetPtr(const char* keyname, const void* value) = 0;
	};

	class IGameEventListener2
	{
	public:
		virtual ~IGameEventListener2() {}
		virtual void FireGameEvent(IGameEvent* event) = 0;
		virtual int GetEventDebugID() = 0;

	};

	class CGameEventCallback
	{
	public:
		IGameEventListener2* m_pCallback; // callback pointer
		int m_nListenerType; // client or server side ?
	};

	class CGameEventDescriptor
	{
	public:
		int pad0;
		int eventid;
		KeyValues* keys;
		CUtlVector<CGameEventCallback*> listeners;
		bool local; // local event, never tell clients about that
		bool reliable; // send this event as reliable message
		char o[16];

		//CGameEventDescriptor()
		//{
		//	name[0] = '\0';
		//	eventid = -1;
		//	keys = nullptr;
		//	local = false;
		//	reliable = true;
		//}

		//char name[MAX_EVENT_NAME_LENGTH]; // name of this event
		//int eventid; // network index number, -1 = not networked
		//KeyValues* keys; // KeyValue describing data types, if NULL only name 
		//bool local; // local event, never tell clients about that
		//bool reliable; // send this event as reliable message
		//CUtlVector<CGameEventCallback*> listeners; // registered listeners
	};

	class CSVCMsg_GameEvent
	{
	};

	class CGameEvent : public IGameEvent
	{
	public:

		CGameEvent(CGameEventDescriptor* descriptor, const char* name);
		virtual ~CGameEvent() = 0;

		[[nodiscard]] const char *GetName() const override = 0;
		[[nodiscard]] bool IsLocal() const override = 0;
		[[nodiscard]] bool IsReliable() const override = 0;
		[[nodiscard]] bool  IsEmpty(const char *keyName = nullptr) override = 0;

		[[nodiscard]] bool GetBool(const char* keyName = nullptr, bool defaultValue = false) override = 0;
		[[nodiscard]] int GetInt(const char* keyName = nullptr, int defaultValue = 0) override = 0;
		[[nodiscard]] unsigned long GetUint64(const char* keyName = nullptr, int defaultValue1 = 0, int defaultValue2 = 0) override = 0;
		[[nodiscard]] float GetFloat(const char* keyname = nullptr, float default_value = 0.0f) override = 0;
		[[nodiscard]] const char* GetString(const char* keyname = nullptr, const char* default_value = "") override = 0;
		[[nodiscard]] const wchar_t* GetWString(const char* keyname = nullptr, const wchar_t* default_value = L"") override = 0;
		[[nodiscard]] const void* GetPtr(const char* keyname = nullptr, const void* default_values = nullptr) override = 0;

		void SetBool(const char* keyname, bool value) override = 0;
		void SetInt(const char* keyname, int value) override = 0;
		void SetUint64(const char* keyname, unsigned long value) override = 0;
		void SetFloat(const char* keyname, float value) override = 0;
		void SetString(const char* keyname, const char* value) override = 0;
		void SetWString(const char* keyname, const wchar_t* value) override = 0;
		void SetPtr(const char* keyname, const void* value) override = 0;

		CGameEventDescriptor	*m_pDescriptor;
		KeyValues				*m_pDataKeys;
	};

	class CGameEventManager
	{
	public:
		enum event_side
		{
			SERVERSIDE = 0,		// this is a server side listener, event logger etc
			CLIENTSIDE,			// this is a client side listenet, HUD element etc
			CLIENTSTUB,			// this is a serverside stub for a remote client listener (used by engine only)
			SERVERSIDE_OLD,		// legacy support for old server event listeners
			CLIENTSIDE_OLD,		// legecy support for old client event listeners
		};
		
		virtual ~CGameEventManager() = 0;
		virtual int LoadEventsFromFile(const char* filename) = 0;
		virtual int Reset() = 0;
		virtual bool AddListener(IGameEventListener2* listener2, const char*, bool) = 0;
		virtual bool FindListener(IGameEventListener2* listener2, const char* name) = 0;
		virtual int RemoveListener(IGameEventListener2*) = 0;
		virtual bool AddListenerGlobal(IGameEventListener2* listener, bool serverside) = 0;
		virtual bool CreateEvent(const char* name, bool force = false, int* cookie = nullptr) = 0;
		virtual bool FireEvent(IGameEvent*, bool bDontBroadcast = false) = 0;
		virtual bool FireEventClientSide(IGameEvent*) = 0; //9
		virtual IGameEvent* DuplicateEvent(IGameEvent*) = 0;
		virtual void FreeEvent(IGameEvent*) = 0;
		virtual bool SerializeEvent(IGameEvent*, CSVCMsg_GameEvent* buffer) = 0;
		virtual IGameEvent* UnserializeEvent(CSVCMsg_GameEvent*) = 0;
		virtual KeyValues* GetEventDataTypes(IGameEvent* event) = 0;

		CGameEventDescriptor* GetEventDescriptor(const char* name);

		static CGameEventDescriptor *GetEventDescriptor(IGameEvent *_event)
		{
			if (!_event)
				return nullptr;

			CGameEvent* game_event = static_cast<CGameEvent*>(_event);
			
			return game_event->m_pDescriptor;
		}
		CGameEventDescriptor *GetEventDescriptor(int eventid)
		{
			if (eventid < 0)
				return nullptr;

			for (int i = 0; i < m_GameEvents.Count(); ++i)
			{
				CGameEventDescriptor *descriptor = &m_GameEvents[i];

				if (descriptor->eventid == eventid)
					return descriptor;
			}

			return nullptr;
		}

		CUtlVector<CGameEventDescriptor> m_GameEvents;	// list of all known events

	};

	class player_team_listener_pre_all final :public IGameEventListener2
	{
		void FireGameEvent(IGameEvent* event) override;
		int GetEventDebugID() override
		{
			return EVENT_DEBUG_ID_INIT;
		}
	};

	class player_team_listener_post_all final :public IGameEventListener2
	{
		void FireGameEvent(IGameEvent* event) override;
		int GetEventDebugID() override
		{
			return EVENT_DEBUG_ID_INIT;
		}
	};
	
	class player_death_listener final :public IGameEventListener2
	{
		void FireGameEvent(IGameEvent* event) override;
		int GetEventDebugID() override
		{
			return EVENT_DEBUG_ID_INIT;
		}
	};
	
	class EventListener : public IGameEventListener2
	{
	public:
		EventListener(std::vector<const char*> events);
		~EventListener();
		void FireGameEvent(IGameEvent* event) override;

		int GetEventDebugID() override
		{
			return EVENT_DEBUG_ID_INIT;
		}
		//size_t get_kills() const
		//{
		//	return kills;
		//}
		//size_t get_bullets() const
		//{
		//	return fired_bullets;
		//}
		//size_t get_grenades() const
		//{
		//	return grenades_throwed;
		//}
	//private:
		//size_t kills;
		//size_t fired_bullets;		
		//size_t grenades_throwed;
	private:
		uintptr_t worker_threadid = -1;
		uintptr_t sender_threadid = -1;
	};

}
extern SourceEngine::EventListener *stat_events;
extern std::unordered_map<UserID, buy> buy_table;
//extern std::vector<footstep> footsteps;

extern player_statistic observable_player;
