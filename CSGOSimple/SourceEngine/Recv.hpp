#pragma once

#include "../IBaseWorldModel.h"

namespace SourceEngine
{
	class RecvTable;
	class RecvProp;
	class CRecvProxyData;

	typedef void (__cdecl *RecvVarProxyFn)(CRecvProxyData* pData, IClientEntity* pStruct, void* pOut);
	typedef void (__cdecl *DataTableRecvVarProxyFn)(const RecvProp* pProp, void** pOut, void* pData, int objectID);

	enum class SendPropType
	{
		DPT_Int = 0,
		DPT_Float,
		DPT_Vector,
		DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
		DPT_String,
		DPT_Array, // An array of the base types (can't be of datatables).
		DPT_DataTable,
		DPT_Int64,
		DPT_NUMSendPropTypes
	};

	class RecvProp
	{
	public:

		char* m_pVarName;
		SendPropType m_RecvType;
		int m_Flags;
		int m_StringBufferSize;
		bool m_bInsideArray;
		const void* m_pExtraData;
		RecvProp* m_pArrayProp;
		void* m_ArrayLengthProxy;
		RecvVarProxyFn m_ProxyFn;
		DataTableRecvVarProxyFn m_DataTableProxyFn;
		RecvTable* m_pDataTable;
		int m_Offset;
		int m_ElementStride;
		int m_nElements;
		const char* m_pParentArrayPropName;
	};
	class RecvTable
	{
	public:
		RecvProp* m_pProps;
		int m_nProps;
		void* m_pDecoder;
		char* m_pNetTableName;
		bool m_bInitialized;
		bool m_bInMainList;
	};

	struct DVariant
	{
		union
		{
			float m_Float;
			long m_Int;
			char* m_pString;
			void* m_pData;
			float m_Vector[3];
			long long m_Int64;
		};

		int m_Type;
	};

	class CRecvProxyData
	{
	public:
		const RecvProp* m_pRecvProp;
		DVariant m_Value;
		int m_iElement;
		int m_ObjectID;
	};
}
