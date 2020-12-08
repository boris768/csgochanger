#pragma once
#include <memory>

enum SignOnState : int
{
	state_none = 0,		// no state yet, about to connect
	state_challenge,	// client challenging server, all OOB packets
	state_connected,	// client is connected to server, netchans ready
	state_new,			// just got serverinfo and string tables
	state_prespawn,		// received signon buffers
	state_spawn,		// ready to receive entity packets
	state_full,			// we are fully connected, first non-delta packet received
	state_changelevel	// server is changing level, please wait
};

namespace SourceEngine
{
	class KeyValues;
	typedef void (__thiscall* FreeEntityBaselines_t)(void*);
	typedef void (__thiscall* SendServerCmdKeyValues_t)(void*, KeyValues*);

	class offsets_CClientState
	{
	public:
		offsets_CClientState();

		FreeEntityBaselines_t FreeEntityBaselines;
		SendServerCmdKeyValues_t SendServerCmdKeyValues;
		ptrdiff_t m_nDeltaTick;
		ptrdiff_t m_nSignonState;
		ptrdiff_t m_bCheckCRCsWithServer;
	};
}


extern SourceEngine::offsets_CClientState* offsets_c_client_state;

namespace SourceEngine
{
	class CClientState
	{
		template <class T>
		T* GetFieldPointer(ptrdiff_t offset)
		{
			const auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
			return (T*)address;
		}

	public:
		void SendServerCmdKeyValues(KeyValues* kv);
		void ForceFullUpdate();
		int* GetDeltaTick();
		bool* GetCheckCRCsWithServer();
		SignOnState GetSignonState();
	};
	void UpdatePanorama();
}
