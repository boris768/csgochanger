#pragma once
//#include "SourceEngine/Definitions.hpp"

namespace SourceEngine
{
	class CMemAlloc
	{
	public:
		virtual void pad0() = 0;
		virtual void* Alloc(size_t nSize) = 0;
		virtual void pad1() = 0;
		virtual void* Realloc(void* pMem, size_t nSize) = 0;
		virtual void pad2() = 0;
		virtual void* Free(void *pMem) = 0;
		virtual void pad3() = 0;
		virtual void pad4() = 0;
		virtual SIZE_T GetSize(void* pMem) = 0;

		virtual ~CMemAlloc() = 0; //dont use, just silent
		//void* Alloc(size_t nSize)
		//{
		//	return CallVFunction<void*(__thiscall*)(void*, size_t)>(this, 1)(this, nSize);//1 - 4
		//}
		////2 - 8
		//void* Realloc(void* pMem, size_t nSize)
		//{
		//	return CallVFunction < void*(__thiscall*)(void*, void*, size_t) >(this, 3)(this, pMem, nSize);//3 - 12
		//}
		////3 - 16
		////4 - 20

		//void Free(void *pMem)
		//{
		//	CallVFunction<void(__thiscall*)(void*, void*)>(this, 5)(this, pMem);//5 - 20
		//}

		HANDLE hHeap;
		DWORD  dwFlags;
		volatile LONG *RequstedAllocedMemory;
		volatile LONG *AllocedMemory;
		volatile LONG *Allocations;
		int padded0;
		volatile LONG *Allocations_safe;

	};
}
