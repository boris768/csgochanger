#pragma once
#include <cassert>
#define Assert(x)
#include <cstdint>
#include <cstring>
#include <algorithm>


void* __fastcall local_alloc1(size_t size);
void* __fastcall local_realloc1(void* pmem, size_t size);
void __fastcall local_free1(void* pmem);

namespace SourceEngine
{
	template <class T, class I = int>
	class CUtlMemory
	{
	public:
		// constructor, destructor
		explicit CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
		CUtlMemory(T* pMemory, int numElements);
		CUtlMemory(const T* pMemory, int numElements);
		~CUtlMemory();

		// Set the size by which the memory grows
		void Init(int nGrowSize = 0, int nInitSize = 0);

		class Iterator_t
		{
		public:
			Iterator_t(I i) : index(i)
			{
			}

			I index;

			bool operator==(const Iterator_t& it) const { return index == it.index; }
			bool operator!=(const Iterator_t& it) const { return index != it.index; }
		};

		Iterator_t First() const { return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
		Iterator_t Next(const Iterator_t& it) const
		{
			return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex());
		}
		static I GetIndex(const Iterator_t& it) { return it.index; }
		static bool IsIdxAfter(I i, const Iterator_t& it) { return i > it.index; }
		bool IsValidIterator(const Iterator_t& it) const { return IsIdxValid(it.index); }
		Iterator_t InvalidIterator() const { return Iterator_t(InvalidIndex()); }

		// element access
		T& operator[](I i);
		const T& operator[](I i) const;
		T& Element(I i);
		const T& Element(I i) const;

		bool IsIdxValid(I i) const;

		static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
		static I InvalidIndex() { return INVALID_INDEX; }

		T* Base();
		const T* Base() const;

		void SetExternalBuffer(T* pMemory, int numElements);
		void SetExternalBuffer(const T* pMemory, int numElements);
		void AssumeMemory(T* pMemory, int nSize);
		T* Detach();
		void* DetachMemory();

		void Swap(CUtlMemory<T, I>& mem);
		void ConvertToGrowableMemory(int nGrowSize);
		int NumAllocated() const;
		int Count() const;
		void Grow(int num = 1);
		void EnsureCapacity(int num);
		void Purge();
		void Purge(int numElements);
		bool IsExternallyAllocated() const;
		bool IsReadOnly() const;
		void SetGrowSize(int size);

	protected:
		static void ValidateGrowSize()
		{
		}

		enum
		{
			EXTERNAL_BUFFER_MARKER = -1,
			EXTERNAL_CONST_BUFFER_MARKER = -2,
		};

		T* m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
	};

	//-----------------------------------------------------------------------------
	// constructor, destructor
	//-----------------------------------------------------------------------------

	template <class T, class I>
	CUtlMemory<T, I>::CUtlMemory(int nGrowSize, int nInitAllocationCount) : m_pMemory(nullptr),
	                                                                        m_nAllocationCount(nInitAllocationCount),
	                                                                        m_nGrowSize(nGrowSize)
	{
		ValidateGrowSize();
		assert(nGrowSize >= 0);
		if (m_nAllocationCount)
		{
			m_pMemory = (T*)local_alloc1(m_nAllocationCount * sizeof(T));
			//m_pMemory = (T*)interfaces.MemAlloc()->Alloc(m_nAllocationCount * sizeof(T));
		}
	}

	template <class T, class I>
	CUtlMemory<T, I>::CUtlMemory(T* pMemory, int numElements) : m_pMemory(pMemory),
	                                                            m_nAllocationCount(numElements)
	{
		// Special marker indicating externally supplied modifyable memory
		m_nGrowSize = EXTERNAL_BUFFER_MARKER;
	}

	template <class T, class I>
	CUtlMemory<T, I>::CUtlMemory(const T* pMemory, int numElements) : m_pMemory((T*)pMemory),
	                                                                  m_nAllocationCount(numElements)
	{
		// Special marker indicating externally supplied modifyable memory
		m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
	}

	template <class T, class I>
	CUtlMemory<T, I>::~CUtlMemory()
	{
		Purge();
	}

	template <class T, class I>
	void CUtlMemory<T, I>::Init(int nGrowSize /*= 0*/, int nInitSize /*= 0*/)
	{
		Purge();

		m_nGrowSize = nGrowSize;
		m_nAllocationCount = nInitSize;
		ValidateGrowSize();
		assert(nGrowSize >= 0);
		if (m_nAllocationCount)
		{
			//UTLMEMORY_TRACK_ALLOC();
			//MEM_ALLOC_CREDIT_CLASS();
			m_pMemory = (T*)local_alloc1(m_nAllocationCount * sizeof(T));
		}
	}

	//-----------------------------------------------------------------------------
	// Fast swap
	//-----------------------------------------------------------------------------
	template <class T, class I>
	void CUtlMemory<T, I>::Swap(CUtlMemory<T, I>& mem)
	{
		std::swap(m_nGrowSize, mem.m_nGrowSize);
		std::swap(m_pMemory, mem.m_pMemory);
		std::swap(m_nAllocationCount, mem.m_nAllocationCount);
	}


	//-----------------------------------------------------------------------------
	// Switches the buffer from an external memory buffer to a reallocatable buffer
	//-----------------------------------------------------------------------------
	template <class T, class I>
	void CUtlMemory<T, I>::ConvertToGrowableMemory(int nGrowSize)
	{
		if (!IsExternallyAllocated())
			return;

		m_nGrowSize = nGrowSize;
		if (m_nAllocationCount)
		{
			//UTLMEMORY_TRACK_ALLOC();
			//MEM_ALLOC_CREDIT_CLASS();

			const int nNumBytes = m_nAllocationCount * sizeof(T);
			T* pMemory = (T*)local_alloc1(nNumBytes);
			if (!pMemory)
			{
				m_pMemory = NULL;
				return;
			}
			//throw std::runtime_error(XorStr("CUtlMemory::ConvertToGrowableMemory() : malloc was failed. It`s fatal."));
			memcpy(pMemory, m_pMemory, nNumBytes);
			m_pMemory = pMemory;
		}
		else
		{
			m_pMemory = NULL;
		}
	}


	//-----------------------------------------------------------------------------
	// Attaches the buffer to external memory....
	//-----------------------------------------------------------------------------
	template <class T, class I>
	void CUtlMemory<T, I>::SetExternalBuffer(T* pMemory, int numElements)
	{
		// Blow away any existing allocated memory
		Purge();

		m_pMemory = pMemory;
		m_nAllocationCount = numElements;

		// Indicate that we don't own the memory
		m_nGrowSize = EXTERNAL_BUFFER_MARKER;
	}

	template <class T, class I>
	void CUtlMemory<T, I>::SetExternalBuffer(const T* pMemory, int numElements)
	{
		// Blow away any existing allocated memory
		Purge();

		m_pMemory = const_cast<T*>(pMemory);
		m_nAllocationCount = numElements;

		// Indicate that we don't own the memory
		m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
	}

	template <class T, class I>
	void CUtlMemory<T, I>::AssumeMemory(T* pMemory, int numElements)
	{
		// Blow away any existing allocated memory
		Purge();

		// Simply take the pointer but don't mark us as external
		m_pMemory = pMemory;
		m_nAllocationCount = numElements;
	}

	template <class T, class I>
	void* CUtlMemory<T, I>::DetachMemory()
	{
		if (IsExternallyAllocated())
			return nullptr;

		void* pMemory = m_pMemory;
		m_pMemory = 0;
		m_nAllocationCount = 0;
		return pMemory;
	}

	template <class T, class I>
	T* CUtlMemory<T, I>::Detach()
	{
		return (T*)DetachMemory();
	}


	//-----------------------------------------------------------------------------
	// element access
	//-----------------------------------------------------------------------------
	template <class T, class I>
	T& CUtlMemory<T, I>::operator[](I i)
	{
		assert(!IsReadOnly());
		assert(IsIdxValid(i));
		return m_pMemory[i];
	}

	template <class T, class I>
	const T& CUtlMemory<T, I>::operator[](I i) const
	{
		assert(IsIdxValid(i));
		return m_pMemory[i];
	}

	template <class T, class I>
	T& CUtlMemory<T, I>::Element(I i)
	{
		assert(!IsReadOnly());
		assert(IsIdxValid(i));
		return m_pMemory[i];
	}

	template <class T, class I>
	const T& CUtlMemory<T, I>::Element(I i) const
	{
		assert(IsIdxValid(i));
		return m_pMemory[i];
	}


	//-----------------------------------------------------------------------------
	// is the memory externally allocated?
	//-----------------------------------------------------------------------------
	template <class T, class I>
	bool CUtlMemory<T, I>::IsExternallyAllocated() const
	{
		return (m_nGrowSize < 0);
	}


	//-----------------------------------------------------------------------------
	// is the memory read only?
	//-----------------------------------------------------------------------------
	template <class T, class I>
	bool CUtlMemory<T, I>::IsReadOnly() const
	{
		return (m_nGrowSize == EXTERNAL_CONST_BUFFER_MARKER);
	}


	template <class T, class I>
	void CUtlMemory<T, I>::SetGrowSize(int nSize)
	{
		assert(!IsExternallyAllocated());
		assert(nSize >= 0);
		m_nGrowSize = nSize;
		ValidateGrowSize();
	}


	//-----------------------------------------------------------------------------
	// Gets the base address (can change when adding elements!)
	//-----------------------------------------------------------------------------
	template <class T, class I>
	T* CUtlMemory<T, I>::Base()
	{
		assert(!IsReadOnly());
		return m_pMemory;
	}

	template <class T, class I>
	const T* CUtlMemory<T, I>::Base() const
	{
		return m_pMemory;
	}


	//-----------------------------------------------------------------------------
	// Size
	//-----------------------------------------------------------------------------
	template <class T, class I>
	int CUtlMemory<T, I>::NumAllocated() const
	{
		return m_nAllocationCount;
	}

	template <class T, class I>
	int CUtlMemory<T, I>::Count() const
	{
		return m_nAllocationCount;
	}


	//-----------------------------------------------------------------------------
	// Is element index valid?
	//-----------------------------------------------------------------------------
	template <class T, class I>
	bool CUtlMemory<T, I>::IsIdxValid(I i) const
	{
		// GCC warns if I is an unsigned type and we do a ">= 0" against it (since the comparison is always 0).
		// We get the warning even if we cast inside the expression. It only goes away if we assign to another variable.
		const long x = i;
		return (x >= 0) && (x < m_nAllocationCount);
	}

	//-----------------------------------------------------------------------------
	// Grows the memory
	//-----------------------------------------------------------------------------
	inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
	{
		if (nGrowSize)
		{
			nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
		}
		else
		{
			if (!nAllocationCount)
			{
				// Compute an allocation which is at least as big as a cache line...
				nAllocationCount = (31 + nBytesItem) / nBytesItem;
			}

			while (nAllocationCount < nNewSize)
			{
#ifndef _X360
				nAllocationCount *= 2;
#else
				int nNewAllocationCount = (nAllocationCount * 9) / 8; // 12.5 %
				if (nNewAllocationCount > nAllocationCount)
					nAllocationCount = nNewAllocationCount;
				else
					nAllocationCount *= 2;
#endif
			}
		}

		return nAllocationCount;
	}

	template <class T, class I>
	void CUtlMemory<T, I>::Grow(int num)
	{
		assert(num > 0);

		if (IsExternallyAllocated())
		{
			// Can't grow a buffer whose memory was externally allocated 
			assert(0);
			return;
		}

		// Make sure we have at least numallocated + num allocations.
		// Use the grow rules specified for this memory (in m_nGrowSize)
		int nAllocationRequested = m_nAllocationCount + num;

		int nNewAllocationCount = UtlMemory_CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, nAllocationRequested,
		                                                           sizeof(T));

		// if m_nAllocationRequested wraps index type I, recalculate
		if ((int)(I)nNewAllocationCount < nAllocationRequested)
		{
			if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested)
			{
				--nNewAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
			}
			else
			{
				if ((int)(I)nAllocationRequested != nAllocationRequested)
				{
					// we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
					assert(0);
					return;
				}
				while ((int)(I)nNewAllocationCount < nAllocationRequested)
				{
					nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
				}
			}
		}

		m_nAllocationCount = nNewAllocationCount;

		if (m_pMemory)
		{
			auto tmp = (T*)local_realloc1(m_pMemory, m_nAllocationCount * sizeof(T));
			if (!tmp)
			{
				m_pMemory = NULL;
				return;
				//throw std::runtime_error(XorStr("UtlMemory::Grow(): realloc was failed. It`s fatal."));
			}
			m_pMemory = tmp;
		}
		else
		{
			m_pMemory = (T*)local_alloc1(m_nAllocationCount * sizeof(T));
			assert(m_pMemory);
		}
	}


	//-----------------------------------------------------------------------------
	// Makes sure we've got at least this much memory
	//-----------------------------------------------------------------------------
	template <class T, class I>
	void CUtlMemory<T, I>::EnsureCapacity(int num)
	{
		if (m_nAllocationCount >= num)
			return;

		if (IsExternallyAllocated())
		{
			// Can't grow a buffer whose memory was externally allocated 
			assert(0);
			return;
		}
		m_nAllocationCount = num;

		if (m_pMemory)
		{
			auto tmp = (T*)local_realloc1(m_pMemory, m_nAllocationCount * sizeof(T));
			if (!tmp)
			{
				m_pMemory = nullptr;
				return;
				//throw std::runtime_error(XorStr("UtlMemory::Purge(): realloc was failed. It`s fatal."));
			}
			m_pMemory = tmp;
		}
		else
		{
			m_pMemory = (T*)local_alloc1(m_nAllocationCount * sizeof(T));
		}
	}


	//-----------------------------------------------------------------------------
	// Memory deallocation
	//-----------------------------------------------------------------------------
	template <class T, class I>
	void CUtlMemory<T, I>::Purge()
	{
		if (!IsExternallyAllocated())
		{
			if (m_pMemory)
			{
				local_free1((void*)m_pMemory);
				m_pMemory = 0;
			}
			m_nAllocationCount = 0;
		}
	}

	template <class T, class I>
	void CUtlMemory<T, I>::Purge(int numElements)
	{
		assert(numElements >= 0);

		if (numElements > m_nAllocationCount)
		{
			// Ensure this isn't a grow request in disguise.
			//assert(numElements <= m_nAllocationCount);
			return;
		}

		// If we have zero elements, simply do a purge:
		if (numElements == 0)
		{
			Purge();
			return;
		}

		if (IsExternallyAllocated())
		{
			// Can't shrink a buffer whose memory was externally allocated, fail silently like purge 
			return;
		}

		// If the number of elements is the same as the allocation count, we are done.
		if (numElements == m_nAllocationCount)
		{
			return;
		}


		if (!m_pMemory)
		{
			// Allocation count is non zero, but memory is null.
			assert(m_pMemory);
			return;
		}
		m_nAllocationCount = numElements;
		auto tmp = (T*)local_realloc1(m_pMemory, m_nAllocationCount * sizeof(T));
		if (!tmp)
		{
			return;
			//throw std::runtime_error(XorStr("UtlMemory::Purge(): realloc was failed. It`s fatal."));
		}
		m_pMemory = tmp;
	}

	//-----------------------------------------------------------------------------
	// The CUtlMemory class:
	// A growable memory class which doubles in size by default.
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	class CUtlMemoryAligned : public CUtlMemory<T>
	{
	public:
		// constructor, destructor
		CUtlMemoryAligned(int nGrowSize = 0, int nInitAllocationCount = 0);
		CUtlMemoryAligned(T* pMemory, int numElements);
		CUtlMemoryAligned(const T* pMemory, int numElements);
		~CUtlMemoryAligned();

		// Attaches the buffer to external memory....
		void SetExternalBuffer(T* pMemory, int numElements);
		void SetExternalBuffer(const T* pMemory, int numElements);

		// Grows the memory, so that at least allocated + num elements are allocated
		void Grow(int num = 1);

		// Makes sure we've got at least this much memory
		void EnsureCapacity(int num);

		// Memory deallocation
		void Purge();

		// Purge all but the given number of elements (NOT IMPLEMENTED IN CUtlMemoryAligned)
		void Purge(int numElements)
		{
			assert(0);
		}

	private:
		static void* Align(const void* pAddr);
	};


	//-----------------------------------------------------------------------------
	// Aligns a pointer
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	void* CUtlMemoryAligned<T, nAlignment>::Align(const void* pAddr)
	{
		const size_t nAlignmentMask = nAlignment - 1;
		return (void*)(((size_t)pAddr + nAlignmentMask) & (~nAlignmentMask));
	}


	//-----------------------------------------------------------------------------
	// constructor, destructor
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	CUtlMemoryAligned<T, nAlignment>::CUtlMemoryAligned(int nGrowSize, int nInitAllocationCount)
	{
		CUtlMemory<T>::m_pMemory = 0;
		CUtlMemory<T>::m_nAllocationCount = nInitAllocationCount;
		CUtlMemory<T>::m_nGrowSize = nGrowSize;
		this->ValidateGrowSize();

		// Alignment must be a power of two
		//COMPILE_TIME_ASSERT((nAlignment & (nAlignment - 1)) == 0);
		assert((nGrowSize >= 0) && (nGrowSize != CUtlMemory<T>::EXTERNAL_BUFFER_MARKER));
		if (CUtlMemory<T>::m_nAllocationCount)
		{
			//UTLMEMORY_TRACK_ALLOC();
			//MEM_ALLOC_CREDIT_CLASS();
			CUtlMemory<T>::m_pMemory = (T*)_aligned_malloc(nInitAllocationCount * sizeof(T), nAlignment);
		}
	}

	template <class T, int nAlignment>
	CUtlMemoryAligned<T, nAlignment>::CUtlMemoryAligned(T* pMemory, int numElements)
	{
		// Special marker indicating externally supplied memory
		CUtlMemory<T>::m_nGrowSize = CUtlMemory<T>::EXTERNAL_BUFFER_MARKER;

		CUtlMemory<T>::m_pMemory = (T*)Align(pMemory);
		CUtlMemory<T>::m_nAllocationCount = ((int)(pMemory + numElements) - (int)CUtlMemory<T>::m_pMemory) / sizeof(T);
	}

	template <class T, int nAlignment>
	CUtlMemoryAligned<T, nAlignment>::CUtlMemoryAligned(const T* pMemory, int numElements)
	{
		// Special marker indicating externally supplied memory
		CUtlMemory<T>::m_nGrowSize = CUtlMemory<T>::EXTERNAL_CONST_BUFFER_MARKER;

		CUtlMemory<T>::m_pMemory = (T*)Align(pMemory);
		CUtlMemory<T>::m_nAllocationCount = ((int)(pMemory + numElements) - (int)CUtlMemory<T>::m_pMemory) / sizeof(T);
	}

	template <class T, int nAlignment>
	CUtlMemoryAligned<T, nAlignment>::~CUtlMemoryAligned()
	{
		Purge();
	}


	//-----------------------------------------------------------------------------
	// Attaches the buffer to external memory....
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	void CUtlMemoryAligned<T, nAlignment>::SetExternalBuffer(T* pMemory, int numElements)
	{
		// Blow away any existing allocated memory
		Purge();

		CUtlMemory<T>::m_pMemory = (T*)Align(pMemory);
		CUtlMemory<T>::m_nAllocationCount = ((int)(pMemory + numElements) - (int)CUtlMemory<T>::m_pMemory) / sizeof(T);

		// Indicate that we don't own the memory
		CUtlMemory<T>::m_nGrowSize = CUtlMemory<T>::EXTERNAL_BUFFER_MARKER;
	}

	template <class T, int nAlignment>
	void CUtlMemoryAligned<T, nAlignment>::SetExternalBuffer(const T* pMemory, int numElements)
	{
		// Blow away any existing allocated memory
		Purge();

		CUtlMemory<T>::m_pMemory = (T*)Align(pMemory);
		CUtlMemory<T>::m_nAllocationCount = ((int)(pMemory + numElements) - (int)CUtlMemory<T>::m_pMemory) / sizeof(T);

		// Indicate that we don't own the memory
		CUtlMemory<T>::m_nGrowSize = CUtlMemory<T>::EXTERNAL_CONST_BUFFER_MARKER;
	}


	//-----------------------------------------------------------------------------
	// Grows the memory
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	void CUtlMemoryAligned<T, nAlignment>::Grow(int num)
	{
		assert(num > 0);

		if (this->IsExternallyAllocated())
		{
			// Can't grow a buffer whose memory was externally allocated 
			assert(0);
			return;
		}

		//UTLMEMORY_TRACK_FREE();

		// Make sure we have at least numallocated + num allocations.
		// Use the grow rules specified for this memory (in m_nGrowSize)
		const int nAllocationRequested = CUtlMemory<T>::m_nAllocationCount + num;

		CUtlMemory<T>::m_nAllocationCount = UtlMemory_CalcNewAllocationCount(CUtlMemory<T>::m_nAllocationCount,
		                                                                     CUtlMemory<T>::m_nGrowSize, nAllocationRequested,
		                                                                     sizeof(T));

		//UTLMEMORY_TRACK_ALLOC();

		if (CUtlMemory<T>::m_pMemory)
		{
			//MEM_ALLOC_CREDIT_CLASS();
			CUtlMemory<T>::m_pMemory = (T*)__mingw_aligned_realloc(CUtlMemory<T>::m_pMemory,
			                                                       CUtlMemory<T>::m_nAllocationCount * sizeof(T), nAlignment);
			assert(CUtlMemory<T>::m_pMemory);
		}
		else
		{
			//MEM_ALLOC_CREDIT_CLASS();
			CUtlMemory<T>::m_pMemory = (T*)__mingw_aligned_malloc(CUtlMemory<T>::m_nAllocationCount * sizeof(T), nAlignment);
			assert(CUtlMemory<T>::m_pMemory);
		}
	}


	//-----------------------------------------------------------------------------
	// Makes sure we've got at least this much memory
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	inline void CUtlMemoryAligned<T, nAlignment>::EnsureCapacity(int num)
	{
		if (CUtlMemory<T>::m_nAllocationCount >= num)
			return;

		if (this->IsExternallyAllocated())
		{
			// Can't grow a buffer whose memory was externally allocated 
			assert(0);
			return;
		}

		//UTLMEMORY_TRACK_FREE();

		CUtlMemory<T>::m_nAllocationCount = num;

		//UTLMEMORY_TRACK_ALLOC();

		if (CUtlMemory<T>::m_pMemory)
		{
			//MEM_ALLOC_CREDIT_CLASS();
			CUtlMemory<T>::m_pMemory = (T*)__mingw_aligned_realloc(CUtlMemory<T>::m_pMemory,
			                                                       CUtlMemory<T>::m_nAllocationCount * sizeof(T), nAlignment);
		}
		else
		{
			//MEM_ALLOC_CREDIT_CLASS();
			CUtlMemory<T>::m_pMemory = (T*)__mingw_aligned_malloc(CUtlMemory<T>::m_nAllocationCount * sizeof(T), nAlignment);
		}
	}


	//-----------------------------------------------------------------------------
	// Memory deallocation
	//-----------------------------------------------------------------------------
	template <class T, int nAlignment>
	void CUtlMemoryAligned<T, nAlignment>::Purge()
	{
		if (!this->IsExternallyAllocated())
		{
			if (CUtlMemory<T>::m_pMemory)
			{
				//UTLMEMORY_TRACK_FREE();
				__mingw_aligned_free(CUtlMemory<T>::m_pMemory);
				CUtlMemory<T>::m_pMemory = 0;
			}
			CUtlMemory<T>::m_nAllocationCount = 0;
		}
	}

	class CByteswap
	{
	public:
		CByteswap()
		{
			// Default behavior sets the target endian to match the machine native endian (no swap).
			SetTargetBigEndian(IsMachineBigEndian());
		}

		//-----------------------------------------------------------------------------
		// Write a single field.
		//-----------------------------------------------------------------------------
		//void SwapFieldToTargetEndian(void* pOutputBuffer, void* pData, typedescription_t* pField);

		//-----------------------------------------------------------------------------
		// Write a block of fields.  Works a bit like the saverestore code.  
		//-----------------------------------------------------------------------------
		//void SwapFieldsToTargetEndian(void* pOutputBuffer, void* pBaseData, datamap_t* pDataMap);

		// Swaps fields for the templated type to the output buffer.
		template<typename T> inline void SwapFieldsToTargetEndian(T* pOutputBuffer, void* pBaseData, unsigned int objectCount = 1)
		{
			for (unsigned int i = 0; i < objectCount; ++i, ++pOutputBuffer)
			{
				SwapFieldsToTargetEndian((void*)pOutputBuffer, pBaseData, &T::m_DataMap);
				pBaseData = static_cast<char8_t*>(pBaseData) + sizeof(T);
			}
		}

		// Swaps fields for the templated type in place.
		template<typename T> inline void SwapFieldsToTargetEndian(T* pOutputBuffer, unsigned int objectCount = 1)
		{
			SwapFieldsToTargetEndian<T>(pOutputBuffer, (void*)pOutputBuffer, objectCount);
		}

		//-----------------------------------------------------------------------------
		// True if the current machine is detected as big endian. 
		// (Endienness is effectively detected at compile time when optimizations are
		// enabled)
		//-----------------------------------------------------------------------------
		static bool IsMachineBigEndian()
		{
			short nIsBigEndian = 1;

			// if we are big endian, the first byte will be a 0, if little endian, it will be a one.
			return (bool)(0 == *(char*)&nIsBigEndian);
		}

		//-----------------------------------------------------------------------------
		// Sets the target byte ordering we are swapping to or from.
		//
		// Braindead Endian Reference:
		//		x86 is LITTLE Endian
		//		PowerPC is BIG Endian
		//-----------------------------------------------------------------------------
		inline void SetTargetBigEndian(bool bigEndian)
		{
			m_bBigEndian = bigEndian;
			m_bSwapBytes = IsMachineBigEndian() != bigEndian;
		}

		// Changes target endian
		inline void FlipTargetEndian(void)
		{
			m_bSwapBytes = !m_bSwapBytes;
			m_bBigEndian = !m_bBigEndian;
		}

		// Forces byte swapping state, regardless of endianess
		inline void ActivateByteSwapping(bool bActivate)
		{
			SetTargetBigEndian(IsMachineBigEndian() != bActivate);
		}

		//-----------------------------------------------------------------------------
		// Returns true if the target machine is the same as this one in endianness.
		//
		// Used to determine when a byteswap needs to take place.
		//-----------------------------------------------------------------------------
		bool IsSwappingBytes()	// Are bytes being swapped?
		{
			return m_bSwapBytes;
		}

		bool IsTargetBigEndian()	// What is the current target endian?
		{
			return m_bBigEndian;
		}

		//-----------------------------------------------------------------------------
		// IsByteSwapped()
		//
		// When supplied with a chunk of input data and a constant or magic number
		// (in native format) determines the endienness of the current machine in
		// relation to the given input data.
		//
		// Returns:
		//		1  if input is the same as nativeConstant.
		//		0  if input is byteswapped relative to nativeConstant.
		//		-1 if input is not the same as nativeConstant and not byteswapped either.
		//
		// ( This is useful for detecting byteswapping in magic numbers in structure 
		// headers for example. )
		//-----------------------------------------------------------------------------
		template<typename T> inline int SourceIsNativeEndian(T input, T nativeConstant)
		{
			// If it's the same, it isn't byteswapped:
			if (input == nativeConstant)
				return 1;

			int output;
			LowLevelByteSwap<T>(&output, &input);
			if (output == nativeConstant)
				return 0;

			Assert(0);		// if we get here, input is neither a swapped nor unswapped version of nativeConstant.
			return -1;
		}

		//-----------------------------------------------------------------------------
		// Swaps an input buffer full of type T into the given output buffer.
		//
		// Swaps [count] items from the inputBuffer to the outputBuffer.
		// If inputBuffer is omitted or NULL, then it is assumed to be the same as
		// outputBuffer - effectively swapping the contents of the buffer in place.
		//-----------------------------------------------------------------------------
		template<typename T> inline void SwapBuffer(T* outputBuffer, T* inputBuffer = NULL, int count = 1)
		{
			Assert(count >= 0);
			Assert(outputBuffer);

			// Fail gracefully in release:
			if (count <= 0 || !outputBuffer)
				return;

			// Optimization for the case when we are swapping in place.
			if (inputBuffer == NULL)
			{
				inputBuffer = outputBuffer;
			}

			// Swap everything in the buffer:
			for (int i = 0; i < count; i++)
			{
				LowLevelByteSwap<T>(&outputBuffer[i], &inputBuffer[i]);
			}
		}

		//-----------------------------------------------------------------------------
		// Swaps an input buffer full of type T into the given output buffer.
		//
		// Swaps [count] items from the inputBuffer to the outputBuffer.
		// If inputBuffer is omitted or NULL, then it is assumed to be the same as
		// outputBuffer - effectively swapping the contents of the buffer in place.
		//-----------------------------------------------------------------------------
		template<typename T> void SwapBufferToTargetEndian(T* outputBuffer, T* inputBuffer = NULL, int count = 1)
		{
			Assert(count >= 0);
			Assert(outputBuffer);

			// Fail gracefully in release:
			if (count <= 0 || !outputBuffer)
				return;

			// Optimization for the case when we are swapping in place.
			if (inputBuffer == NULL)
			{
				inputBuffer = outputBuffer;
			}

			// Are we already the correct endienness? ( or are we swapping 1 byte items? )
			if (!m_bSwapBytes || (sizeof(T) == 1))
			{
				// If we were just going to swap in place then return.
				//if (!inputBuffer)
				//	return;

				// Otherwise copy the inputBuffer to the outputBuffer:
				if (outputBuffer != inputBuffer)
					memcpy(outputBuffer, inputBuffer, count * sizeof(T));
				return;

			}

			// Swap everything in the buffer:
			for (int i = 0; i < count; i++)
			{
				LowLevelByteSwap<T>(&outputBuffer[i], &inputBuffer[i]);
			}
		}

	private:
		//-----------------------------------------------------------------------------
		// The lowest level byte swapping workhorse of doom.  output always contains the 
		// swapped version of input.  ( Doesn't compare machine to target endianness )
		//-----------------------------------------------------------------------------
		template<typename T> static void LowLevelByteSwap(T* output, T* input)
		{
			T temp = *output;
			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				((unsigned char*)&temp)[i] = ((unsigned char*)input)[sizeof(T) - (i + 1)];
			}
			memcpy(output, &temp, sizeof(T));
		}

		unsigned int m_bSwapBytes : 1;
		unsigned int m_bBigEndian : 1;
	};
	
	class CUtlBuffer
	{
		// Brian has on his todo list to revisit this as there are issues in some cases with CUtlVector using operator = instead of copy construtor in InsertMultiple, etc.
		// The unsafe case is something like this:
		//  CUtlVector< CUtlBuffer > vecFoo;
		// 
		//  CUtlBuffer buf;
		//  buf.Put( xxx );
		//  vecFoo.Insert( buf );
		//
		//  This will cause memory corruption when vecFoo is cleared
		//
		//private:
		//	// Disallow copying
		//	CUtlBuffer( const CUtlBuffer & );// { Assert( 0 ); }
		//	CUtlBuffer &operator=( const CUtlBuffer & );//  { Assert( 0 ); return *this; }

	public:
		enum SeekType_t
		{
			SEEK_HEAD = 0,
			SEEK_CURRENT,
			SEEK_TAIL
		};

		// flags
		enum BufferFlags_t
		{
			TEXT_BUFFER = 0x1,			// Describes how get + put work (as strings, or binary)
			EXTERNAL_GROWABLE = 0x2,	// This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting.
			CONTAINS_CRLF = 0x4,		// For text buffers only, does this contain \n or \n\r?
			READ_ONLY = 0x8,			// For external buffers; prevents null termination from happening.
			AUTO_TABS_DISABLED = 0x10,	// Used to disable/enable push/pop tabs
		};

		// Overflow functions when a get or put overflows
		typedef bool (CUtlBuffer::* UtlBufferOverflowFunc_t)(int nSize);

		// Constructors for growable + external buffers for serialization/unserialization
		CUtlBuffer(int growSize = 0, int initSize = 0, int nFlags = 0);
		CUtlBuffer(const void* pBuffer, int size, int nFlags = 0);
		// This one isn't actually defined so that we catch contructors that are trying to pass a bool in as the third param.
		CUtlBuffer(const void* pBuffer, int size, bool crap) = delete;

		// UtlBuffer objects should not be copyable; we do a slow copy if you use this but it asserts.
		// (REI: I'd like to delete these but we have some python bindings that currently rely on being able to copy these objects)
		CUtlBuffer(const CUtlBuffer&); // = delete;
		CUtlBuffer& operator= (const CUtlBuffer&); // = delete;

#if VALVE_CPP11
	// UtlBuffer is non-copyable (same as CUtlMemory), but it is moveable.  We would like to declare these with '= default'
	// but unfortunately VS2013 isn't fully C++11 compliant, so we have to manually declare these in the boilerplate way.
		CUtlBuffer(CUtlBuffer&& moveFrom); // = default;
		CUtlBuffer& operator= (CUtlBuffer&& moveFrom); // = default;
#endif

		unsigned char	GetFlags() const;

		// NOTE: This will assert if you attempt to recast it in a way that
		// is not compatible. The only valid conversion is binary-> text w/CRLF
		void			SetBufferType(bool bIsText, bool bContainsCRLF);

		// Makes sure we've got at least this much memory
		void			EnsureCapacity(int num);

		// Access for direct read into buffer
		void* AccessForDirectRead(int nBytes);

		// Attaches the buffer to external memory....
		void			SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags = 0);
		bool			IsExternallyAllocated() const;
		void			AssumeMemory(void* pMemory, int nSize, int nInitialPut, int nFlags = 0);
		void* Detach();
		void* DetachMemory();

		// copies data from another buffer
		void			CopyBuffer(const CUtlBuffer& buffer);
		void			CopyBuffer(const void* pubData, int cubData);

		void			Swap(CUtlBuffer& buf);
		void			Swap(CUtlMemory<uint8_t>& mem);



		// Controls endian-ness of binary utlbufs - default matches the current platform
		void			ActivateByteSwapping(bool bActivate);
		void			SetBigEndian(bool bigEndian);
		bool			IsBigEndian();

		// Resets the buffer; but doesn't free memory
		void			Clear();

		// Clears out the buffer; frees memory
		void			Purge();

		// Dump the buffer to stdout
		void			Spew();

		// Read stuff out.
		// Binary mode: it'll just read the bits directly in, and characters will be
		//		read for strings until a null character is reached.
		// Text mode: it'll parse the file, turning text #s into real numbers.
		//		GetString will read a string until a space is reached
		char			GetChar();
		unsigned char	GetUnsignedChar();
		short			GetShort();
		unsigned short	GetUnsignedShort();
		int				GetInt();
		int64_t			GetInt64();
		unsigned int	GetIntHex();
		unsigned int	GetUnsignedInt();
		uint64_t		GetUnsignedInt64();
		float			GetFloat();
		double			GetDouble();
		void* GetPtr();
		void			GetString(char* pString, int nMaxChars);
		bool			Get(void* pMem, int size);
		void			GetLine(char* pLine, int nMaxChars);

		// Used for getting objects that have a byteswap datadesc defined
		template <typename T> void GetObjects(T* dest, int count = 1);

		// This will get at least 1 byte and up to nSize bytes. 
		// It will return the number of bytes actually read.
		int				GetUpTo(void* pMem, int nSize);

		// This version of GetString converts \" to \\ and " to \, etc.
		// It also reads a " at the beginning and end of the string
		//void			GetDelimitedString(CUtlCharConversion* pConv, char* pString, int nMaxChars = 0);
		//char			GetDelimitedChar(CUtlCharConversion* pConv);

		// This will return the # of characters of the string about to be read out
		// NOTE: The count will *include* the terminating 0!!
		// In binary mode, it's the number of characters until the next 0
		// In text mode, it's the number of characters until the next space.
		int				PeekStringLength();

		// This version of PeekStringLength converts \" to \\ and " to \, etc.
		// It also reads a " at the beginning and end of the string
		// NOTE: The count will *include* the terminating 0!!
		// In binary mode, it's the number of characters until the next 0
		// In text mode, it's the number of characters between "s (checking for \")
		// Specifying false for bActualSize will return the pre-translated number of characters
		// including the delimiters and the escape characters. So, \n counts as 2 characters when bActualSize == false
		// and only 1 character when bActualSize == true
		//int				PeekDelimitedStringLength(CUtlCharConversion* pConv, bool bActualSize = true);

		// Just like scanf, but doesn't work in binary mode
		//int				Scanf(SCANF_FORMAT_STRING const char* pFmt, ...);
		int				VaScanf(const char* pFmt, va_list list);

		// Eats white space, advances Get index
		void			EatWhiteSpace();

		// Eats C++ style comments
		bool			EatCPPComment();

		// (For text buffers only)
		// Parse a token from the buffer:
		// Grab all text that lies between a starting delimiter + ending delimiter
		// (skipping whitespace that leads + trails both delimiters).
		// If successful, the get index is advanced and the function returns true,
		// otherwise the index is not advanced and the function returns false.
		bool			ParseToken(const char* pStartingDelim, const char* pEndingDelim, char* pString, int nMaxLen);

		// Advance the get index until after the particular string is found
		// Do not eat whitespace before starting. Return false if it failed
		// String test is case-insensitive.
		bool			GetToken(const char* pToken);

		// Parses the next token, given a set of character breaks to stop at
		// Returns the length of the token parsed in bytes (-1 if none parsed)
		//int				ParseToken(characterset_t* pBreaks, char* pTokenBuf, int nMaxLen, bool bParseComments = true);

		// Write stuff in
		// Binary mode: it'll just write the bits directly in, and strings will be
		//		written with a null terminating character
		// Text mode: it'll convert the numbers to text versions
		//		PutString will not write a terminating character
		void			PutChar(char c);
		void			PutUnsignedChar(unsigned char uc);
		void			PutShort(short s);
		void			PutUnsignedShort(unsigned short us);
		void			PutInt(int i);
		void			PutInt64(int64_t i);
		void			PutUnsignedInt(unsigned int u);
		void			PutUnsignedInt64(uint64_t u);
		void			PutFloat(float f);
		void			PutDouble(double d);
		void			PutPtr(void*); // Writes the pointer, not the pointed to
		void			PutString(const char* pString);
		void			Put(const void* pMem, int size);

		// Used for putting objects that have a byteswap datadesc defined
		template <typename T> void PutObjects(T* src, int count = 1);

		// This version of PutString converts \ to \\ and " to \", etc.
		// It also places " at the beginning and end of the string
		//void			PutDelimitedString(CUtlCharConversion* pConv, const char* pString);
		//void			PutDelimitedChar(CUtlCharConversion* pConv, char c);

		// Just like printf, writes a terminating zero in binary mode
		//void			Printf(PRINTF_FORMAT_STRING const char* pFmt, ...) FMTFUNCTION(2, 3);
		void			VaPrintf(const char* pFmt, va_list list);

		// What am I writing (put)/reading (get)?
		void* PeekPut(int offset = 0);
		const void* PeekGet(int offset = 0) const;
		const void* PeekGet(int nMaxSize, int nOffset);

		// Where am I writing (put)/reading (get)?
		int TellPut() const;
		int TellGet() const;

		// What's the most I've ever written?
		int TellMaxPut() const;

		// How many bytes remain to be read?
		// NOTE: This is not accurate for streaming text files; it overshoots
		int GetBytesRemaining() const;

		// Change where I'm writing (put)/reading (get)
		void SeekPut(SeekType_t type, int offset);
		void SeekGet(SeekType_t type, int offset);

		// Buffer base
		const void* Base() const;
		void* Base();

		// memory allocation size, does *not* reflect size written or read,
		//	use TellPut or TellGet for that
		int Size() const;

		// Am I a text buffer?
		bool IsText() const;

		// Can I grow if I'm externally allocated?
		bool IsGrowable() const;

		// Am I valid? (overflow or underflow error), Once invalid it stays invalid
		bool IsValid() const;

		// Do I contain carriage return/linefeeds? 
		bool ContainsCRLF() const;

		// Am I read-only
		bool IsReadOnly() const;

		// Converts a buffer from a CRLF buffer to a CR buffer (and back)
		// Returns false if no conversion was necessary (and outBuf is left untouched)
		// If the conversion occurs, outBuf will be cleared.
		bool ConvertCRLF(CUtlBuffer& outBuf);

		// Push/pop pretty-printing tabs
		void PushTab();
		void PopTab();

		// Temporarily disables pretty print
		void EnableTabs(bool bEnable);

		// Swap my internal memory with another buffer,
		// and copy all of its other members
		void SwapCopy(CUtlBuffer& other);

	protected:
		// error flags
		enum
		{
			PUT_OVERFLOW = 0x1,
			GET_OVERFLOW = 0x2,
			MAX_ERROR_FLAG = GET_OVERFLOW,
		};

		void SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc);

		bool OnPutOverflow(int nSize);
		bool OnGetOverflow(int nSize);

	protected:
		// Checks if a get/put is ok
		bool CheckPut(int size);
		bool CheckGet(int size);

		// NOTE: Pass in nPut here even though it is just a copy of m_Put.  This is almost always called immediately 
		// after modifying m_Put and this lets it stay in a register
		void AddNullTermination(int nPut);

		// Methods to help with pretty-printing
		bool WasLastCharacterCR();
		void PutTabs();

		// Help with delimited stuff
		//char GetDelimitedCharInternal(CUtlCharConversion* pConv);
		//void PutDelimitedCharInternal(CUtlCharConversion* pConv, char c);

		// Default overflow funcs
		bool PutOverflow(int nSize);
		bool GetOverflow(int nSize);

		// Does the next bytes of the buffer match a pattern?
		bool PeekStringMatch(int nOffset, const char* pString, int nLen);

		// Peek size of line to come, check memory bound
		int	PeekLineLength();

		// How much whitespace should I skip?
		int PeekWhiteSpace(int nOffset);

		// Checks if a peek get is ok
		bool CheckPeekGet(int nOffset, int nSize);

		// Call this to peek arbitrarily long into memory. It doesn't fail unless
		// it can't read *anything* new
		bool CheckArbitraryPeekGet(int nOffset, int& nIncrement);

		template <typename T> void GetType(T& dest);
		template <typename T> void GetTypeBin(T& dest);
		template <typename T> bool GetTypeText(T& value, int nRadix = 10);
		template <typename T> void GetObject(T* src);

		template <typename T> void PutType(T src);
		template <typename T> void PutTypeBin(T src);
		template <typename T> void PutObject(T* src);

		// be sure to also update the copy constructor
		// and SwapCopy() when adding members.
		CUtlMemory<unsigned char> m_Memory;
		int m_Get;
		int m_Put;

		unsigned char m_Error;
		unsigned char m_Flags;
		unsigned char m_Reserved;

		int m_nTab;
		int m_nMaxPut;
		int m_nOffset;

		UtlBufferOverflowFunc_t m_GetOverflowFunc;
		UtlBufferOverflowFunc_t m_PutOverflowFunc;

		CByteswap	m_Byteswap;
	};
	
}
