// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "KeyValues.h"
#include "Utils.hpp"
#include "Config.h"
#include "CustomWinAPI.h"
#include "XorStr.hpp"

namespace SourceEngine
{
	_offsets_keyvalues* offsets_keyvalues;
	_offsets_keyvalues::_offsets_keyvalues()
	{
		dwKeyValuesSystem = reinterpret_cast<ptrdiff_t>(g_pWinApi->GetProcAddress(static_cast<HMODULE>(memory.vstdlib().lpBaseOfDll), XorStr("KeyValuesSystem")));
		dwKeyValuesConstructor = Utils::dwGetCallAddress(Utils::FindSignature(memory.client(), XorStr("E8 ? ? ? ? EB 02 33 C0 57 8B BE")));
		dwKeyValuesLoadFromFile = Utils::FindSignature(memory.client(), XorStr("55 8B EC 83 E4 F8 83 EC 14 53 56 8B 75 08 57 FF 75 10"));
		dwKeyValuesSaveToFile = Utils::FindSignature(memory.client(), XorStr("55 8B EC 51 53 8B ? ? 56 8B ? ? 57 8B ? ? 57 8B ? 68")); //broken
		dwKeyValuesFindKey = Utils::FindSignature(memory.client(), XorStr("55 8B EC 83 EC 1C 53 8B D9 85 DB"));
		dwKeyValuesGetString = Utils::FindSignature(memory.client(), XorStr("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08"));
		dwKeyValuesMergeFrom = Utils::FindSignature(memory.client(), XorStr("55 8B EC 53 8B D9 56 85 DB"));
		dwKeyValuesLoadFromBuffer = Utils::FindSignature(memory.client(), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"));
	}

	KeyValues::KeyValues(const char* setName): m_iKeyName(0), m_iKeyNameCaseSensitive1(0), m_sValue(nullptr),  // NOLINT(cppcoreguidelines-pro-type-member-init)
	                                           m_wsValue(nullptr),
	                                           m_iValue(0),
	                                           m_iDataType(),
	                                           m_bHasEscapeSequences(false), m_iKeyNameCaseSensitive2(0),
	                                           m_pPeer(nullptr),
	                                           m_pSub(nullptr),
	                                           m_pChain(nullptr)
	{
		int param = -1;
		const HKeySymbol symbol = KeyValuesSystem()->GetSymbolForStringCaseSensitive(&param, setName, true);
		m_iKeyName ^= (param ^ m_iKeyName) & 0xFFFFFF;
		m_iKeyNameCaseSensitive1 = symbol;
		m_iKeyNameCaseSensitive2 = symbol >> 8;
	}
	
	//KeyValues::KeyValues(const char* setName, int old)
	//{
	//	reinterpret_cast<void(__thiscall*)(void*, const char*)>(offsets_keyvalues->dwKeyValuesConstructor)(this, setName);
	//}
	
	const char* KeyValues::GetName() const
	{
		return KeyValuesSystem()->GetStringForSymbol(m_iKeyName);
	}

	void KeyValues::UsesEscapeSequences(const bool state)
	{
		m_bHasEscapeSequences = state;
	}

	bool KeyValues::LoadFromFile(const DWORD filesystem, const char* resourceName, const char* pathID,
	                             const DWORD pfnEvaluateSymbolProc)
	{
		return reinterpret_cast<bool(__thiscall*)(void*, DWORD, const char*, const char*, DWORD)>
			(offsets_keyvalues->dwKeyValuesLoadFromFile)
			(this, filesystem ? filesystem + 4 : 0, resourceName, pathID, pfnEvaluateSymbolProc);
	}

	bool KeyValues::LoadFromBuffer(const char* resourceName, const char* pBuffer, DWORD* pFileSystem,
	                               const char* pPathID)
	{
		typedef bool (__thiscall* oLoadFromBuffer)(void*, const char*, const char*, DWORD*, const char*, DWORD);
		return reinterpret_cast<oLoadFromBuffer>(offsets_keyvalues->dwKeyValuesLoadFromBuffer)
			(this, resourceName, pBuffer, pFileSystem, pPathID, 0);

		//return LoadFromBuffer(this, resourceName, pBuffer, pFileSystem, pPathID, NULL);
	}

	bool KeyValues::SaveToFile(const DWORD filesystem, const char* resourceName, const char* pathID, const bool unk)
	{
		return reinterpret_cast<bool(__thiscall*)(void*, DWORD, const char*, const char*, DWORD)>(offsets_keyvalues->
			dwKeyValuesSaveToFile)(this, filesystem ? filesystem + 4 : 0, resourceName, pathID, unk);
	}

	KeyValues* KeyValues::FindKey(const char* keyName, const bool bCreate)
	{
		// return the current key if a NULL subkey is asked for
		if (!keyName || !keyName[0])
			return this;

		// look for '/' characters deliminating sub fields
		char szBuf[256];
		const char* subStr = strchr(keyName, '/');
		const char* searchStr = keyName;

		// pull out the substring if it exists
		if (subStr)
		{
			const int end_pos = subStr - keyName;
			memcpy(szBuf, keyName, end_pos);
			szBuf[end_pos] = '\0';
			searchStr = szBuf;
		}

		// lookup the symbol for the search string
		const HKeySymbol iSearchStr = KeyValuesSystem()->GetSymbolForString(searchStr, bCreate);
		if (iSearchStr == INVALID_KEY_SYMBOL)
		{
			// not found, couldn't possibly be in key value list
			return nullptr;
		}

		KeyValues* lastItem = nullptr;
		KeyValues* dat;
		// find the searchStr in the current peer list
		for (dat = m_pSub; dat != nullptr; dat = dat->m_pPeer)
		{
			lastItem = dat;	// record the last item looked at (for if we need to append to the end of the list)

			// symbol compare
			if (dat->m_iKeyName == iSearchStr)
			{
				break;
			}
		}

		if (!dat && m_pChain)
		{
			dat = m_pChain->FindKey(keyName, false);
		}

		// make sure a key was found
		if (!dat)
		{
			if (bCreate)
			{
				// we need to create a new key
				dat = new KeyValues(searchStr);
				//			Assert(dat != NULL);

							// insert new key at end of list
				if (lastItem)
				{
					lastItem->m_pPeer = dat;
				}
				else
				{
					m_pSub = dat;
				}
				dat->m_pPeer = nullptr;

				// a key graduates to be a submsg as soon as it's m_pSub is set
				// this should be the only place m_pSub is set
				m_iDataType = TYPE_NONE;
			}
			else
			{
				return nullptr;
			}
		}

		// if we've still got a subStr we need to keep looking deeper in the tree
		if (subStr)
		{
			// recursively chain down through the paths in the string
			return dat->FindKey(subStr + 1, bCreate);
		}

		return dat;
	}

	KeyValues* KeyValues::GetFirstSubKey() const
	{
		return m_pSub;
	}

	KeyValues* KeyValues::GetNextKey() const
	{
		return m_pPeer;
	}

	KeyValues* KeyValues::GetFirstTrueSubKey() const
	{
		KeyValues* pRet = m_pSub;
		while (pRet && pRet->m_iDataType != TYPE_NONE)
		{
			pRet = pRet->m_pPeer;
		}
		return pRet;
	}

	KeyValues* KeyValues::GetNextTrueSubKey() const
	{
		KeyValues* pRet = m_pPeer;
		while (pRet && pRet->m_iDataType != TYPE_NONE)
		{
			pRet = pRet->m_pPeer;
		}
		return pRet;
	}

	KeyValues* KeyValues::GetFirstValue() const
	{
		KeyValues* pRet = m_pSub;
		while (pRet && pRet->m_iDataType == TYPE_NONE)
			pRet = pRet->m_pPeer;

		return pRet;
	}

	KeyValues* KeyValues::GetNextValue() const
	{
		KeyValues* pRet = m_pPeer;
		while (pRet && pRet->m_iDataType == TYPE_NONE)
			pRet = pRet->m_pPeer;

		return pRet;
	}

	KeyValues* KeyValues::MakeCopy() const
	{
		auto* newKeyValue = new KeyValues(GetName());

		// copy data
		newKeyValue->m_iDataType = m_iDataType;
		switch (m_iDataType)
		{
		case TYPE_STRING:
			{
				if (m_sValue)
				{
					const int len = strlen(m_sValue);
					newKeyValue->m_sValue = new char[len + 1];
					memcpy(newKeyValue->m_sValue, m_sValue, len + 1);
				}
			}
			break;
		case TYPE_WSTRING:
			{
				if (m_wsValue)
				{
					const int len = wcslen(m_wsValue);
					newKeyValue->m_wsValue = new wchar_t[len + 1];
					memcpy(newKeyValue->m_wsValue, m_wsValue, (len + 1) * sizeof(wchar_t));
				}
			}
			break;

		case TYPE_INT:
			newKeyValue->m_iValue = m_iValue;
			break;

		case TYPE_FLOAT:
			newKeyValue->m_flValue = m_flValue;
			break;

		case TYPE_PTR:
			newKeyValue->m_pValue = m_pValue;
			break;

		case TYPE_COLOR:
			newKeyValue->m_Color[0] = m_Color[0];
			newKeyValue->m_Color[1] = m_Color[1];
			newKeyValue->m_Color[2] = m_Color[2];
			newKeyValue->m_Color[3] = m_Color[3];
			break;

		case TYPE_UINT64:
			newKeyValue->m_sValue = new char[sizeof(uint64_t)];
			memcpy(newKeyValue->m_sValue, m_sValue, sizeof(uint64_t));
			break;
		case TYPE_NONE:
		case TYPE_COMPILED_INT_BYTE:
		case TYPE_COMPILED_INT_0:
		case TYPE_COMPILED_INT_1:
		case TYPE_NUMTYPES: 
			break;
		}

		// recursively copy subkeys
		CopySubkeys(newKeyValue);
		return newKeyValue;
	}

	void KeyValues::CopySubkeys(KeyValues* pParent) const
	{
		KeyValues* pPrev = nullptr;
		for (KeyValues* sub = m_pSub; sub != nullptr; sub = sub->m_pPeer)
		{
			// take a copy of the subkey
			KeyValues* dat = sub->MakeCopy();

			// add into subkey list
			if (pPrev)
			{
				pPrev->m_pPeer = dat;
			}
			else
			{
				pParent->m_pSub = dat;
			}
			dat->m_pPeer = nullptr;
			pPrev = dat;
		}
	}

	uint64_t KeyValues::GetUint64(const char* keyName, const uint64_t defaultValue)
	{
		KeyValues* dat = FindKey(keyName, false);
		if (dat)
		{
			switch (dat->m_iDataType)
			{
			case TYPE_STRING:
				return atoi(dat->m_sValue);
			case TYPE_WSTRING:
				return _wtoi(dat->m_wsValue);
			case TYPE_FLOAT:
				return static_cast<int>(dat->m_flValue);
			case TYPE_UINT64:
				return *reinterpret_cast<uint64_t *>(dat->m_sValue);
			case TYPE_INT:
			case TYPE_PTR:
			case TYPE_NONE:
			case TYPE_COLOR:
			case TYPE_COMPILED_INT_BYTE:
			case TYPE_COMPILED_INT_0:
			case TYPE_COMPILED_INT_1:
			case TYPE_NUMTYPES:
				return dat->m_iValue;
			}
		}
		return defaultValue;
	}

	const char* KeyValues::GetString_hack(const char* default_value) const
	{
		if (this->m_sValue)
			return this->m_sValue;
		return default_value;
	}

	const char* KeyValues::GetString(const char* keyName, const char* default_value)
	{
		return reinterpret_cast<const char*(__thiscall*)(void*, const char*, const char*)>(offsets_keyvalues->
			dwKeyValuesGetString)(this, keyName, default_value);

		//auto found = FindKey(keyName, false);
		//if (!found)
		//	return  default_value;
		//switch (found->m_iDataType)
		//{
		//case TYPE_STRING:
		//	return found->m_sValue;
		//case TYPE_INT:
		//	snprintf((int)&v13, 64, (int)"%d", v4->m_Value.m_iValue);
		//	break;
		//case TYPE_FLOAT:
		//	snprintf((int)&v13, 64, (int)"%f", COERCE_UNSIGNED_INT64(*(float*)&v4->m_Value.m_iValue));
		//	break;
		//case TYPE_PTR:
		//	snprintf((int)&v13, 64, (int)"%lld", v4->m_Value.m_iValue);
		//	break;
		//case TYPE_WSTRING:
		//	MultiByteStr = 0;
		//	v12 = WideCharToMultiByte(0xFDE9u, 0, v4->m_wsValue, -1, &MultiByteStr, 512, 0, 0);
		//	v15 = 0;
		//	if (!v12)
		//		return (char*)default_value;
		//	sub_108EC740(this, KeyName, &MultiByteStr);
		//	return v4->m_sValue;
		//case TYPE_COLOR:
		//	v9 = v4->m_Value.m_Color[3];
		//	v10 = v4->m_Value.m_Color[2];
		//	v11 = v4->m_Value.m_Color[1];
		//	snprintf((int)&v13, 64, (int)"%d %d %d %d", v4->m_Value.m_Color[0]);
		//	break;
		//case TYPE_UINT64:
		//	v7 = (int*)v4->m_sValue;
		//	v8 = v7[1];
		//	snprintf((int)&v13, 64, (int)"%lld", *v7);
		//	break;
		//default:
		//	return (char*)default_value;
		//}
		//sub_108EC740(this, KeyName, &v13);
		//return v4->m_sValue;
	}

	std::wstring KeyValues::GetWString(const char* keyName, const wchar_t* default_value)
	{
		auto* const found = FindKey(keyName, false);
		if (found)
		{
			switch (found->m_iDataType)
			{
			case TYPE_STRING:
				return std::wstring(found->m_sValue, found->m_sValue + strlen(found->m_sValue));
			case TYPE_WSTRING:
				return found->m_wsValue;
				//case TYPE_FLOAT:
				//	return std::to_wstring(found->m_flValue);
				//case TYPE_INT:
				//	return std::to_wstring(found->m_iValue);
			case TYPE_UINT64:
				{
					return std::to_wstring(*reinterpret_cast<uint64_t*>(found->m_sValue));
				}
				
			case TYPE_NONE:
			case TYPE_INT:
			case TYPE_FLOAT: 
			case TYPE_PTR:
			case TYPE_COLOR:
			case TYPE_COMPILED_INT_BYTE:
			case TYPE_COMPILED_INT_0:
			case TYPE_COMPILED_INT_1:
			case TYPE_NUMTYPES:
				return default_value;
				
			}
		}
		return default_value;
	}

	float KeyValues::GetFloat(const char* keyName, const float default_value)
	{
		KeyValues* dat = FindKey(keyName, false);
		if (dat)
		{
			switch (dat->m_iDataType)
			{
			case TYPE_STRING:
				return static_cast<float>(atof(dat->m_sValue));
			case TYPE_WSTRING:
#ifdef WIN32
				return static_cast<float>(_wtof(dat->m_wsValue));		// no wtof
#else
				return (float)wcstof(dat->m_wsValue, (wchar_t**)NULL);
#endif
			case TYPE_FLOAT:
				return dat->m_flValue;
			case TYPE_INT:
				return static_cast<float>(dat->m_iValue);
			case TYPE_UINT64:
				return static_cast<float>(*reinterpret_cast<uint64_t*>(dat->m_sValue));
			case TYPE_PTR:
			case TYPE_NONE:
			case TYPE_COLOR:
			case TYPE_COMPILED_INT_BYTE:
			case TYPE_COMPILED_INT_0:
			case TYPE_COMPILED_INT_1:
			case TYPE_NUMTYPES:
				return 0.0f;
			}
		}
		return default_value;
	}

	int KeyValues::GetInt(const char* keyName, const int default_value)
	{
		auto* found = FindKey(keyName, false);
		if (found)
		{
			switch (found->m_iDataType)
			{
			case TYPE_STRING:
				return atol(found->m_sValue);
			case TYPE_WSTRING:
				return _wtoi(found->m_wsValue);
			case TYPE_FLOAT:
				return static_cast<int>(floor(found->m_flValue));
			case TYPE_UINT64:
				return 0;
			case TYPE_PTR:
				return reinterpret_cast<int>(found->m_pValue);
			case TYPE_NONE:
			case TYPE_INT:
			case TYPE_COLOR:
			case TYPE_COMPILED_INT_BYTE:
			case TYPE_COMPILED_INT_0:
			case TYPE_COMPILED_INT_1:
			case TYPE_NUMTYPES:
				return found->m_iValue;
			}
		}

		return default_value;
	}

	void KeyValues::SetInt(const char* keyName, const int value)
	{
		KeyValues* dat = FindKey(keyName, true);

		if (dat)
		{
			dat->m_iValue = value;
			dat->m_iDataType = TYPE_INT;
		}
	}

	void KeyValues::SetFloat(const char* keyName, const float value)
	{
		KeyValues* dat = FindKey(keyName, true);

		if (dat)
		{
			dat->m_flValue = value;
			dat->m_iDataType = TYPE_FLOAT;
		}
	}

	void KeyValues::SetUint64(const char* keyName, uint64_t value)
	{
		KeyValues* dat = FindKey(keyName, true);

		if (dat)
		{
			// delete the old value
			delete[] dat->m_sValue;
			// make sure we're not storing the WSTRING  - as we're converting over to STRING
			delete[] dat->m_wsValue;
			dat->m_wsValue = nullptr;

			dat->m_sValue = new char[sizeof(uint64_t)];
			*reinterpret_cast<uint64_t*>(dat->m_sValue) = value;
			dat->m_iDataType = TYPE_UINT64;
		}
	}

	void* KeyValues::operator new(const size_t iAllocSize)
	{
		return KeyValuesSystem()->AllocKeyValuesMemory(iAllocSize);
	}

	void KeyValues::operator delete(void* pMem)
	{
		return KeyValuesSystem()->FreeKeyValuesMemory(pMem);
	}

	KeyValues::types_t KeyValues::GetDataType() const
	{
		return static_cast<types_t>(m_iDataType);
	}

	void KeyValues::MergeFrom(KeyValues* kvMerge, const MergeKeyValuesOp_t eOp)
	{
		reinterpret_cast<void(__thiscall*)(void*, KeyValues*, MergeKeyValuesOp_t)>(offsets_keyvalues->
			dwKeyValuesMergeFrom)(this, kvMerge, eOp);
	}

	size_t KeyValues::iCount() const
	{
		size_t iCount = 0;

		for (KeyValues* dat = m_pSub; dat != nullptr; dat = dat->m_pPeer)
			++iCount;

		return iCount;
	}

	KeyValuesSystem* KeyValues::KeyValuesSystem()
	{
		return reinterpret_cast<SourceEngine::KeyValuesSystem *(__cdecl*)()>(offsets_keyvalues->dwKeyValuesSystem)();
	}

	void KeyValues::SetColor(const char* keyName, const char r, const char g, const char b, const char a)
	{
		KeyValues* dat = FindKey(keyName, true);

		if (dat)
		{
			dat->m_Color[0] = r;
			dat->m_Color[1] = g;
			dat->m_Color[2] = b;
			dat->m_Color[3] = a;
			dat->m_iDataType = TYPE_COLOR;
		}
	}

	void KeyValues::SetString(const char* keyName, const char* value)
	{
		KeyValues* dat = FindKey(keyName, true);

		if (dat)
		{
			auto* memalloc = interfaces.MemAlloc();
			memalloc->Free(dat->m_sValue);
			memalloc->Free(dat->m_wsValue);
			const size_t str_size = strlen(value) + 1;
			if (str_size > 1)
			{
				dat->m_sValue = static_cast<char*>(memalloc->Alloc(str_size));
				memset(dat->m_sValue, 0, str_size);
				strcpy(dat->m_sValue, value);
				dat->m_iDataType = TYPE_STRING;
			}
		}
	}
}
