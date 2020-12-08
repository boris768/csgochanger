#pragma once

#include "SourceEngine/IAppSystem.hpp"

namespace SourceEngine {
	class KeyValues;
}

class ILocalizeTextQuery // unknown usage, dont use
{
public:
	virtual int ComputeTextWidth(const wchar_t *pString) = 0;
};

class ILocalizationChangeCallback // unknown usage, dont use
{
public:
	virtual void OnLocalizationChanged() = 0;
};

typedef unsigned long LocalizeStringIndex_t;
constexpr unsigned long LOCALIZE_INVALID_STRING_INDEX = static_cast<LocalizeStringIndex_t>(-1);

class CLocalize : public SourceEngine::IAppSystem
{
public:
	virtual bool AddFile(const char *fileName, const char *pPathID = nullptr, bool bIncludeFallbackSearchPaths = false) = 0; // 9
	virtual void RemoveAll() = 0;
	virtual const wchar_t* Find(char const* tokenName) = 0;
	virtual const wchar_t* FindSafe(char const* tokenName) = 0;
	virtual int convert_ansi_unicode() = 0; //unknown usage
	virtual int convert_unicode_ansi() = 0; //unknown usage
	virtual int ConvertANSIToUnicode(char const* source, wchar_t *dest, int unicodeBufferSizeInBytes) = 0;
	virtual int ConvertUnicodeToANSI(wchar_t *source, char const* dest, int ansiBufferSize) = 0;
	virtual int FindIndex(char const*tokenname) = 0; //15
	virtual const char* GetNameByIndex(LocalizeStringIndex_t index) = 0;
	virtual const wchar_t* GetValueByIndex(LocalizeStringIndex_t index) = 0;
	virtual LocalizeStringIndex_t GetFirstStringIndex() = 0;
	virtual LocalizeStringIndex_t GetNextStringIndex(LocalizeStringIndex_t index) = 0;
	virtual void AddString(const char *tokenName, wchar_t *unicodeString, const char *fileName) = 0; //20
	virtual void SetValueByIndex(LocalizeStringIndex_t index, wchar_t *newValue) = 0; //21
	virtual bool SaveToFile(const char *fileName) = 0;
	virtual int GetLocalizationFileCount() = 0;
	virtual const char *GetLocalizationFileName(int index) = 0;
	virtual const char *GetFileNameByIndex(LocalizeStringIndex_t index) = 0; //25
	virtual void ReloadLocalizationFiles() = 0;
	virtual void ConstructString(wchar_t *, int, char const*, SourceEngine::KeyValues *) = 0; // unknown usage, dont use
	virtual void ConstructString(wchar_t *, int, LocalizeStringIndex_t, SourceEngine::KeyValues *) = 0; //unknown usage, dont use
	virtual void SetTextQuery(ILocalizeTextQuery *) = 0; //unknown usage, dont use
	virtual void InstallChangeCallback(ILocalizationChangeCallback *pCallback) = 0;
	virtual void RemoveChangeCallback(ILocalizationChangeCallback *pCallback) = 0;
	virtual void GetAsianFrequencySequence(char const* filename) = 0; // unknown usage, dont use
	virtual void ConstructStringVArgsInternal(char *, int, char const*, int, char *) = 0; // unknown usage, dont use
	virtual void ConstructStringVArgsInternal(wchar_t *, int, wchar_t const*, int, char *) = 0; // unknown usage, dont use
	virtual void ConstructStringKeyValuesInternal(char *, int, char const*, SourceEngine::KeyValues *) = 0; // unknown usage, dont use
	virtual void ConstructStringKeyValuesInternal(wchar_t *, int, wchar_t const*, SourceEngine::KeyValues *) = 0; // unknown usage, dont use
	virtual ~CLocalize() = 0; 
	virtual int ConvertANSIToUCS2(LPCSTR lpMultiByteStr, LPWSTR lpWideCharStr, int a3) = 0; 
	virtual int ConvertUCS2ToANSI(LPCWSTR lpWideCharStr, LPSTR lpMultiByteStr, int cbMultiByte) = 0;

	//bool add_file(const char* szFileName);
	
	std::wstring FindSafe2(const char* tokenName)
	{
		const wchar_t*const found = Find(tokenName);
		if (!found)
		{
			std::string szTokenName(tokenName + 1);
			return std::wstring(szTokenName.begin(), szTokenName.end());
		}
		return found;
	}


	bool addLocalizeString(const SourceEngine::CUtlString& pattern, const std::wstring& locale_str, const char* filename)
	{
		if (!locale_str.empty())
		{
			const auto buffer = new wchar_t[locale_str.size() + 1];
			wcscpy(buffer, locale_str.c_str());
			AddString(pattern.Get() + sizeof(char), buffer, filename);
			return true;
		}
		return false;
	}

	char m_szLanguage[64];
	bool m_bUseOnlyLongestLanguageString;
	bool m_bSuppressChangeCallbacks;
	bool m_bQueuedChangeCallback;

	// Stores the symbol lookup
	SourceEngine::CUtlRBTree</*localizedstring_t*/ void*, LocalizeStringIndex_t> m_Lookup;

	// stores the string data
	SourceEngine::CUtlVector<char> m_Names;
	SourceEngine::CUtlVector<wchar_t> m_Values;
	
};

inline bool V_IsAbsolutePath(const char* pStr)
{
	if (!(pStr[0] && pStr[1]))
		return false;

	const bool bIsAbsolute = (pStr[0] && pStr[1] == ':') ||
		((pStr[0] == '/' || pStr[0] == '\\') && (pStr[1] == '/' || pStr[1] == '\\'));

	return bIsAbsolute;
}

//inline bool CLocalize::add_file(const char* szFileName)
//{
//	// use the correct file based on the chosen language
//	constexpr const char* const LANGUAGE_STRING = "%language%";
//	constexpr const char* const ENGLISH_STRING = "english";
//	constexpr int MAX_LANGUAGE_NAME_LENGTH = 64;
//
//	char language[MAX_LANGUAGE_NAME_LENGTH];
//	memset(language, 0, sizeof(language));
//
//	if (V_IsAbsolutePath(szFileName))
//	{
//		//Warning("Full paths not allowed in localization file specificaton %s\n", szFileName);
//		return false;
//	}
//	const char* langptr = strstr(szFileName, LANGUAGE_STRING);
//	if (langptr)
//	{
//		// LOAD THE ENGLISH FILE FIRST
//		// always load the file to make sure we're not missing any strings
//		// copy out the initial part of the string
//		int offs = langptr - szFileName;
//		char fileName[MAX_PATH];
//		strncpy(fileName, szFileName, offs);
//		fileName[offs] = 0;
//
//		//if (m_bUseOnlyLongestLanguageString)
//		//{
//		//	return AddAllLanguageFiles(fileName);
//		//}
//
//		// append "english" as our default language
//		strncat(fileName, ENGLISH_STRING, sizeof(fileName));
//
//		// append the end of the initial string
//		offs += strlen(LANGUAGE_STRING);
//		strncat(fileName, szFileName + offs, sizeof(fileName));
//
//		bool success = AddFile(fileName);
//
//		bool bValid = true;
//
//		if (CommandLine()->CheckParm("-language"))
//		{
//			strncpy(language, CommandLine()->ParmValue("-language", "english"), sizeof(language));
//			bValid = true;
//		}
//		else
//		{
//			bValid = vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", language, sizeof(language) - 1);
//		}
//		if (bValid && !Q_stricmp(language, "unknown"))
//		{
//			// Fall back to english
//			bValid = false;
//		}
//
//		// LOAD THE LOCALIZED FILE IF IT'S NOT ENGLISH
//		// append the language
//		if (bValid)
//		{
//			if (strlen(language) != 0 && stricmp(language, ENGLISH_STRING) != 0)
//			{
//				// copy out the initial part of the string
//				offs = langptr - szFileName;
//				strncpy(fileName, szFileName, offs);
//				fileName[offs] = 0;
//
//				strncat(fileName, language, sizeof(fileName));
//
//				// append the end of the initial string
//				offs += strlen(LANGUAGE_STRING);
//				strncat(fileName, szFileName + offs, sizeof(fileName));
//
//				success &= AddFile(fileName);
//			}
//		}
//		return success;
//	}
//}
