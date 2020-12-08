#pragma once

#include <string>
#include <windows.h>

class reghelper
{
public:

	explicit reghelper(const wchar_t* regpath) : lRes(0)
	{
		const LONG Res = RegOpenKeyExW(HKEY_LOCAL_MACHINE, regpath, 0, KEY_READ, &hKey);
		bExistsAndSuccess = (Res == ERROR_SUCCESS);
		bDoesNotExistsSpecifically = (Res == ERROR_FILE_NOT_FOUND);
	}
	~reghelper()
	{
		RegCloseKey(hKey);
	}

	LONG GetDWORDRegKey(const std::wstring& strValueName, DWORD& nValue, const DWORD nDefaultValue) const
	{
		nValue = nDefaultValue;
		DWORD dwBufferSize(sizeof(DWORD));
		DWORD nResult(0);
		const LONG nError = ::RegQueryValueExW(hKey,
			strValueName.c_str(),
			nullptr,
			nullptr,
			reinterpret_cast<LPBYTE>(&nResult),
			&dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			nValue = nResult;
		}
		return nError;
	}


	LONG GetBoolRegKey(const std::wstring& strValueName, bool& bValue, const bool bDefaultValue) const
	{
		const DWORD nDefValue((bDefaultValue) ? 1 : 0);
		DWORD nResult(nDefValue);
		const LONG nError = GetDWORDRegKey(strValueName, nResult, nDefValue);
		if (ERROR_SUCCESS == nError)
		{
			bValue = (nResult != 0) ? true : false;
		}
		return nError;
	}


	LONG GetStringRegKey(const std::wstring& strValueName, std::wstring& strValue, const std::wstring& strDefaultValue) const
	{
		strValue = strDefaultValue;
		WCHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		const ULONG nError = RegQueryValueExW(hKey, strValueName.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(szBuffer), &dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			strValue = szBuffer;
		}
		return nError;
	}

private:
	HKEY hKey;
	LONG lRes;
	bool bExistsAndSuccess;//(lRes == ERROR_SUCCESS);
	bool bDoesNotExistsSpecifically;// (lRes == ERROR_FILE_NOT_FOUND);
	std::wstring strValueOfBinDir;
	std::wstring strKeyDefaultValue;
};
