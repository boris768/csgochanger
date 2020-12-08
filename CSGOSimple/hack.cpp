// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CSGOStructs.hpp"
#include "SourceEngine/UtlMemory.hpp"

void* __fastcall local_alloc1(size_t size)
{
	return SourceEngine::interfaces.MemAlloc()->Alloc(size);
}

void* __fastcall local_realloc1(void* pmem, size_t size)
{
	return SourceEngine::interfaces.MemAlloc()->Realloc(pmem, size);
}

void __fastcall local_free1(void* pmem)
{
	SourceEngine::interfaces.MemAlloc()->Free(pmem);
}
