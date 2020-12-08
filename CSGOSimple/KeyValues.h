#pragma once

#pragma once
#include <string>
#include <cmath>
#include <memory>
#include "SourceEngine/Definitions.hpp"

namespace SourceEngine
{
	struct _offsets_keyvalues
	{
		ptrdiff_t dwKeyValuesSystem;
		ptrdiff_t dwKeyValuesConstructor;
		ptrdiff_t dwKeyValuesLoadFromFile;
		ptrdiff_t dwKeyValuesSaveToFile;
		ptrdiff_t dwKeyValuesFindKey;
		ptrdiff_t dwKeyValuesGetString;
		ptrdiff_t dwKeyValuesMergeFrom;
		ptrdiff_t dwKeyValuesLoadFromBuffer;
		_offsets_keyvalues();
	};
	extern _offsets_keyvalues* offsets_keyvalues;
	
	typedef int HKeySymbol;
	constexpr HKeySymbol INVALID_KEY_SYMBOL = -1;
	class KeyValuesSystem
	{
	public:
		virtual void RegisterSizeofKeyValues(int size) = 0;
		virtual void* AllocKeyValuesMemory(int size) = 0;
		virtual void FreeKeyValuesMemory(void* pMem) = 0;
		virtual HKeySymbol GetSymbolForString( const char *name, bool bCreate = true ) = 0;
		virtual const char* GetStringForSymbol(HKeySymbol symbol) = 0;
		virtual void AddKeyValuesToMemoryLeakList(void* pMem, HKeySymbol name) = 0;
		virtual void RemoveKeyValuesFromMemoryLeakList(void* pMem) = 0;
		virtual void SetKeyValuesExpressionSymbol(const char*, bool) = 0;
		virtual void GetKeyValuesExpressionSymbol(const char*) = 0;
		virtual HKeySymbol GetSymbolForStringCaseSensitive(int*, const char*, bool) = 0;
	};

	class KeyValues
	{
	public:
		explicit KeyValues(const char* setName);
		
		[[nodiscard]] const char* GetName() const;

		void UsesEscapeSequences(const bool state);

		bool LoadFromFile(const DWORD filesystem, const char* resourceName, const char* pathID = nullptr,
		                  const DWORD pfnEvaluateSymbolProc = 0);

		bool LoadFromBuffer(const char* resourceName, const char* pBuffer, DWORD* pFileSystem = nullptr,
		                    const char* pPathID = nullptr);

		bool SaveToFile(const DWORD filesystem, const char* resourceName, const char* pathID = nullptr,
		                const bool unk = false);

		KeyValues* FindKey(const char* keyName, const bool bCreate = false);

		[[nodiscard]] KeyValues* GetFirstSubKey() const;

		[[nodiscard]] KeyValues* GetNextKey() const;

		[[nodiscard]] KeyValues* GetFirstTrueSubKey() const;

		[[nodiscard]] KeyValues* GetNextTrueSubKey() const;

		[[nodiscard]] KeyValues* GetFirstValue() const;

		[[nodiscard]] KeyValues* GetNextValue() const;

		[[nodiscard]] KeyValues* MakeCopy() const;

		void CopySubkeys(KeyValues* pParent) const;

		uint64_t GetUint64(const char* keyName, const uint64_t defaultValue = 0);

		const char* GetString_hack(const char* default_value = "") const;

		const char* GetString(const char* keyName = nullptr, const char* default_value = "");

		std::wstring GetWString(const char* keyName = nullptr, const wchar_t* default_value = L"")
		//Warning: bkoken by gcc
		;

		float GetFloat(const char* keyName = nullptr, const float default_value = 0.0F);

		int GetInt(const char* keyName = nullptr, const int default_value = 0);

		void SetInt(const char* keyName, const int value);

		void SetFloat(const char* keyName, const float value);

		void SetColor(const char* keyName, const char r, const char g, const char b, const char a);

		void SetString(const char* keyName, const char* value);

		void SetUint64(const char* keyName, uint64_t value);

		void* operator new(size_t iAllocSize);

		void operator delete(void* pMem);

		enum types_t : char
		{
			TYPE_NONE = 0,
			TYPE_STRING,
			TYPE_INT,
			TYPE_FLOAT,
			TYPE_PTR,
			TYPE_WSTRING,
			TYPE_COLOR,
			TYPE_UINT64,
			TYPE_COMPILED_INT_BYTE,
			TYPE_COMPILED_INT_0,
			TYPE_COMPILED_INT_1,
			TYPE_NUMTYPES,
		};

		[[nodiscard]] types_t GetDataType() const;

		enum MergeKeyValuesOp_t
		{
			MERGE_KV_ALL,
			MERGE_KV_UPDATE,
			MERGE_KV_DELETE,
			MERGE_KV_BORROW
		};

		void MergeFrom(KeyValues* kvMerge, const MergeKeyValuesOp_t eOp = MERGE_KV_ALL);

		[[nodiscard]] size_t iCount() const;

	private:
		static KeyValuesSystem* KeyValuesSystem();

		DWORD m_iKeyName : 24;
		DWORD m_iKeyNameCaseSensitive1 : 8;

		char* m_sValue;
		wchar_t* m_wsValue;

		union
		{
			int m_iValue;
			float m_flValue;
			void* m_pValue;
			BYTE m_Color[4];
		};

		types_t m_iDataType;
		bool m_bHasEscapeSequences;
		WORD m_iKeyNameCaseSensitive2;

		KeyValues* m_pPeer;
		KeyValues* m_pSub;
		KeyValues* m_pChain;

		//DWORD pad0;
	};

	constexpr size_t size = sizeof(KeyValues);
}
