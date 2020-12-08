#pragma once
#include "SourceEngine/UtlVector.hpp"

typedef int TABLEID;

class CNetworkStringTableItem;
class INetworkStringDict
{
public:
	virtual ~INetworkStringDict() {}

	virtual unsigned int Count() = 0;
	virtual void Purge() = 0;
	virtual const char* String(int index) = 0;
	virtual bool IsValidIndex(int index) = 0;
	virtual int Insert(const char* pString) = 0;
	virtual int Find(const char* pString) = 0;
	virtual CNetworkStringTableItem& Element(int index) = 0;
	virtual const CNetworkStringTableItem& Element(int index) const = 0;
};

class INetworkStringTable;
typedef void(__cdecl *pfnStringChanged)(void *object, INetworkStringTable *stringTable, int stringNumber, char const *newString, void const *newData);

constexpr auto INVALID_STRING_TABLE = -1;
constexpr unsigned short INVALID_STRING_INDEX = -1;

class INetworkStringTable
{
public:

	virtual					~INetworkStringTable() = 0;
	// Table Info
	virtual const char		*GetTableName() const = 0;
	virtual TABLEID			GetTableId() const = 0;
	virtual int				GetNumStrings() const = 0;
	virtual int				GetMaxStrings() const = 0;
	virtual int				GetEntryBits() const = 0;
	// Networking
	virtual void			SetTick(int tick) = 0;
	virtual bool			ChangedSinceTick(int tick) const = 0;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int				AddString(bool bIsServer, const char *value, int length = -1, const void *userdata = nullptr) = 0;

	virtual const char		*GetString(int stringNumber) = 0;
	virtual void			SetStringUserData(int stringNumber, int length, const void *userdata) = 0;
	virtual const void		*GetStringUserData(int stringNumber, int *length) = 0;
	virtual int				FindStringIndex(char const *string) = 0; // returns INVALID_STRING_INDEX if not found
																	 // Callbacks
	virtual void			SetStringChangedCallback(void *object, pfnStringChanged changeFunc) = 0;
	virtual void Dump() = 0;
	virtual void Lock(bool locked) = 0;
	enum
	{
		MAX_USERDATA_BITS = 14,
		MAX_USERDATA_SIZE = (1 << MAX_USERDATA_BITS)
	};

	TABLEID m_id; //0x0004
	char* m_pszTableName; //0x0008
	int32_t m_nMaxEntries; //0x000C
	int32_t m_nEntryBits; //0x0010
	int32_t m_nTickCount; //0x0014
	int32_t m_nLastChangedTick; //0x0018
	int32_t m_unk; //0x001C
	char pad_0020[4]; //0x0020
	int32_t m_unk2; //0x0024
	int32_t m_unk3; //0x0028
	int32_t m_unk4; //0x002C
	pfnStringChanged m_changeFunc; //0x0030
	void* m_pObject; //0x0034
	INetworkStringTable* m_pMirrorTable; //0x0038
	INetworkStringDict* m_pItems; //0x003C
	INetworkStringDict* m_pItemsClientSide; //0x0040


};

class INetworkStringTableContainer
{
public:

	virtual					~INetworkStringTableContainer()=0;
	// table creation/destruction
	virtual INetworkStringTable	*CreateStringTable(const char *tableName, unsigned int maxentries, unsigned int userdatafixedsize = 0, unsigned int userdatanetworkbits = 0) = 0;
	virtual void				RemoveAllTables() = 0;
	// table infos
	virtual INetworkStringTable	*FindTable(const char *tableName) const = 0;
	virtual INetworkStringTable	*GetTable(TABLEID stringTable) const = 0;
	virtual int					GetNumTables() const = 0;
	virtual void				SetAllowClientSideAddString(INetworkStringTable *table, bool bAllowClientSideAddString) = 0;
	virtual void				CreateDictionary(const char *dictName) = 0;

	bool		m_bAllowCreation;	// creat guard Guard
	int			m_nTickCount;		// current tick
	bool		m_bLocked;			// currently locked?
	bool		m_bEnableRollback;	// enables rollback feature

	SourceEngine::CUtlVector < INetworkStringTable* > m_Tables;	// the string tables
};