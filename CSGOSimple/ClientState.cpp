#include "CClientState.h"
#include "CSGOStructs.hpp"
#include "Utils.hpp"
#include "XorStr.hpp"

SourceEngine::offsets_CClientState::offsets_CClientState()
{
	FreeEntityBaselines = reinterpret_cast<FreeEntityBaselines_t>(Utils::FindSignature(memory.engine(), XorStr("55 8B EC 83 EC 08 53 56 57 8B F9 8B 8F")));
	SendServerCmdKeyValues = reinterpret_cast<SendServerCmdKeyValues_t>(Utils::FindSignature(memory.engine(), XorStr("55 8B EC 83 EC 34 56 8B 75 08 57")));
	m_nDeltaTick = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.engine(), XorStr("83 BE ? ? ? ? FF 74 25")) + 2);
	m_nSignonState = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.engine(), XorStr("83 B8 ? ? ? ? ? 0F 94 C0 C3")) + 2);
	m_bCheckCRCsWithServer = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(memory.engine(), XorStr("80 BB ? ? ? ? ? 0F 84 ? ? ? ? 83")) + 2);
}