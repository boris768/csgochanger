#pragma once
#include "minimal_UtlMemory.h"
/*
 *.rdata:004EFCC4 ; const CNetworkStringTable::`vftable'
.rdata:004EFCC4 ??_7CNetworkStringTable@@6B@ dd offset INetworkStringTable_destructor
.rdata:004EFCC4                                         ; DATA XREF: sub_277F20+1Co
.rdata:004EFCC4                                         ; INetworkStringTable_destructor+Eo
.rdata:004EFCC8                 dd offset INetworkStringTable_GetTableName
.rdata:004EFCCC                 dd offset GetTableId
.rdata:004EFCD0                 dd offset GetNumStrings
.rdata:004EFCD4                 dd offset GetMaxStrings
.rdata:004EFCD8                 dd offset GetEntryBits
.rdata:004EFCDC                 dd offset SetTick
.rdata:004EFCE0                 dd offset ChangedSinceTick
.rdata:004EFCE4                 dd offset INetworkStringTable__AddString
.rdata:004EFCE8                 dd offset GetString
.rdata:004EFCEC                 dd offset SetStringUserData
.rdata:004EFCF0                 dd offset GetStringUserData
.rdata:004EFCF4                 dd offset FindStringIndex
.rdata:004EFCF8                 dd offset SetStringChangedCallback
.rdata:004EFCFC                 dd offset Dump
.rdata:004EFD00                 dd offset Lock
 *
 *
 */

typedef int TABLEID;
#define INVALID_STRING_TABLE -1
const unsigned short INVALID_STRING_INDEX = (unsigned short)-1;

// table index is sent in log2(MAX_TABLES) bits
#define MAX_TABLES	32  // Table id is 4 bits

class INetworkStringTable;

class CNetworkStringTableItem
{
public:
	enum
	{
		MAX_USERDATA_BITS = 14,
		MAX_USERDATA_SIZE = (1 << MAX_USERDATA_BITS)
	};

	struct itemchange_s {
		int				tick;
		int				length;
		unsigned char	*data;
	};

	unsigned char	*m_pUserData;
	int				m_nUserDataLength;
	int				m_nTickChanged;

	int				m_nTickCreated;
	CUtlVector< itemchange_s > *m_pChangeList;
};


class INetworkStringDict
{
public:
	virtual ~INetworkStringDict() {}

	virtual unsigned int Count() = 0;
	virtual void Purge() = 0;
	virtual const char *String(int index) = 0;
	virtual bool IsValidIndex(int index) = 0;
	virtual int Insert(const char *pString) = 0;
	virtual int Find(const char *pString) = 0;
	virtual INetworkStringDict* UpdateDictionary(int) = 0; //unknown usage
	virtual int DictionaryIndex(int) = 0; //unknown usage
	virtual CNetworkStringTableItem	&Element(int index) = 0;
	virtual const CNetworkStringTableItem &Element(int index) const = 0;
};

typedef void(*pfnStringChanged)(void *object, INetworkStringTable *stringTable, int stringNumber, char const *newString, void const *newData);

class INetworkStringTable
{
public:
	virtual					~INetworkStringTable() = 0;
	virtual const char		*GetTableName(void) const = 0;
	virtual TABLEID			GetTableId(void) const = 0;
	virtual int				GetNumStrings(void) const = 0;
	virtual int				GetMaxStrings(void) const = 0;
	virtual int				GetEntryBits(void) const = 0;

	// Networking
	virtual void			SetTick(int tick) = 0;
	virtual bool			ChangedSinceTick(int tick) const = 0;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int				AddString(bool bIsServer, const char *value, int length = -1, const void *userdata = 0) = 0;

	virtual const char		*GetString(int stringNumber) = 0;
	virtual void			SetStringUserData(int stringNumber, int length, const void *userdata) = 0;
	virtual const void		*GetStringUserData(int stringNumber, int *length) = 0;
	virtual int				FindStringIndex(char const *string) = 0; // returns INVALID_STRING_INDEX if not found

																	 // Callbacks
	virtual void			SetStringChangedCallback(void *object, pfnStringChanged changeFunc) = 0;
	virtual	void			Dump() = 0;
	virtual int				Lock(bool enable) = 0;

	TABLEID					m_id;
	char					*m_pszTableName;
	// Must be a power of 2, so encoding can determine # of bits to use based on log2
	int						m_nMaxEntries;
	int						m_nEntryBits;
	int						m_nTickCount;
	int						m_nLastChangedTick;

	bool					m_bChangeHistoryEnabled : 1;
	bool					m_bLocked : 1;
	bool					m_bAllowClientSideAddString : 1;
	bool					m_bUserDataFixedSize : 1;
	bool					m_bIsFilenames : 1;

	int						m_nUserDataSize;
	int						m_nUserDataSizeBits;

	// Change function callback
	pfnStringChanged		m_changeFunc;
	// Optional context/object
	void					*m_pObject;
	INetworkStringTable		*m_pMirrorTable;

	INetworkStringDict		*m_pItems;
	INetworkStringDict		*m_pItemsClientSide;

};