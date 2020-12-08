// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IMaterialSystem.h"
#include "SourceEngine/UtlMemory.hpp"


#define INVALID_BUFFER_OFFSET 0xFFFFFFFFUL
#define NELEMS(x) ((sizeof(x))/sizeof(x[0]))

 int Float2Int(float a)
{

	//int RetVal;

	//int CtrlwdHolder;
	//int CtrlwdSetter;
	//__asm
	//{
	//	fld    a					// push 'a' onto the FP stack
	//	fnstcw CtrlwdHolder		// store FPU control word
	//	movzx  eax, CtrlwdHolder	// move and zero extend word into eax
	//	and    eax, 0xFFFFF3FF	// set all bits except rounding bits to 1
	//	or eax, 0x00000C00	// set rounding mode bits to round towards zero
	//	mov    CtrlwdSetter, eax	// Prepare to set the rounding mode -- prepare to enter plaid!
	//	fldcw  CtrlwdSetter		// Entering plaid!
	//	fistp  RetVal				// Store and converted (to int) result
	//	fldcw  CtrlwdHolder		// Restore control word
	//}
	return int(a);
}

// Over 15x faster than: (int)floor(value)
int Floor2Int(float a)
{
	//int RetVal;

	//int CtrlwdHolder;
	//int CtrlwdSetter;
	//__asm
	//{
	//	fld    a					// push 'a' onto the FP stack
	//	fnstcw CtrlwdHolder		// store FPU control word
	//	movzx  eax, CtrlwdHolder	// move and zero extend word into eax
	//	and    eax, 0xFFFFF3FF	// set all bits except rounding bits to 1
	//	or eax, 0x00000400	// set rounding mode bits to round down
	//	mov    CtrlwdSetter, eax	// Prepare to set the rounding mode -- prepare to enter plaid!
	//	fldcw  CtrlwdSetter		// Entering plaid!
	//	fistp  RetVal				// Store floored and converted (to int) result
	//	fldcw  CtrlwdHolder		// Restore control word
	//}
	return static_cast<int>(a);
}

float *OffsetFloatPointer(float *pBufferPointer, int nVertexCount, int vertexSize)
{
	return reinterpret_cast<float *>(
		reinterpret_cast<unsigned char *>(pBufferPointer) +
		nVertexCount * vertexSize);
}

const float *OffsetFloatPointer(const float *pBufferPointer, int nVertexCount, int vertexSize)
{
	return reinterpret_cast<const float*>(
		reinterpret_cast<unsigned char const*>(pBufferPointer) +
		nVertexCount * vertexSize);
}

void IncrementFloatPointer(float* &pBufferPointer, int vertexSize)
{
	pBufferPointer = reinterpret_cast<float*>(reinterpret_cast<unsigned char*>(pBufferPointer) + vertexSize);
}

 unsigned char FastFToC(float c)
{
	// ieee trick
	volatile float dc = c * 255.0f + (float)(1 << 23);

	// return the lsb
	return *(unsigned char*)&dc;
}
//-----------------------------------------------------------------------------
// Helper methods to create various standard index buffer types
//-----------------------------------------------------------------------------
void GenerateSequentialIndexBuffer(unsigned short* pIndices, int nIndexCount, int nFirstVertex)
{
	if (!pIndices)
		return;

	// Format the sequential buffer
	for (int i = 0; i < nIndexCount; ++i)
	{
		pIndices[i] = static_cast<unsigned short>(i + nFirstVertex);
	}
}

void GenerateQuadIndexBuffer(unsigned short* pIndices, int nIndexCount, int nFirstVertex)
{
	if (!pIndices)
		return;

    const int numQuads = nIndexCount / 6;
	int baseVertex = nFirstVertex;
	for (int i = 0; i < numQuads; ++i)
	{
		// Triangle 1
		pIndices[0] = static_cast<unsigned short>(baseVertex);
		pIndices[1] = static_cast<unsigned short>(baseVertex + 1);
		pIndices[2] = static_cast<unsigned short>(baseVertex + 2);

		// Triangle 2
		pIndices[3] = static_cast<unsigned short>(baseVertex);
		pIndices[4] = static_cast<unsigned short>(baseVertex + 2);
		pIndices[5] = static_cast<unsigned short>(baseVertex + 3);

		baseVertex += 4;
		pIndices += 6;
	}
}

void GeneratePolygonIndexBuffer(unsigned short* pIndices, int nIndexCount, int nFirstVertex)
{
	if (!pIndices)
		return;

    const int numPolygons = nIndexCount / 3;
	for (int i = 0; i < numPolygons; ++i)
	{
		// Triangle 1
		pIndices[0] = static_cast<unsigned short>(nFirstVertex);
		pIndices[1] = static_cast<unsigned short>(nFirstVertex + i + 1);
		pIndices[2] = static_cast<unsigned short>(nFirstVertex + i + 2);
		pIndices += 3;
	}
}


void GenerateLineStripIndexBuffer(unsigned short* pIndices, int nIndexCount, int nFirstVertex)
{
	if (!pIndices)
		return;

    const int numLines = nIndexCount / 2;
	for (int i = 0; i < numLines; ++i)
	{
		pIndices[0] = static_cast<unsigned short>(nFirstVertex + i);
		pIndices[1] = static_cast<unsigned short>(nFirstVertex + i + 1);
		pIndices += 2;
	}
}

void GenerateLineLoopIndexBuffer(unsigned short* pIndices, int nIndexCount, int nFirstVertex)
{
	if (!pIndices)
	{
		return;
	}

    const int numLines = nIndexCount / 2;

	pIndices[0] = static_cast<unsigned short>(nFirstVertex + numLines - 1);
	pIndices[1] = static_cast<unsigned short>(nFirstVertex);
	pIndices += 2;

	for (int i = 1; i < numLines; ++i)
	{
		pIndices[0] = static_cast<unsigned short>(nFirstVertex + i - 1);
		pIndices[1] = static_cast<unsigned short>(nFirstVertex + i);
		pIndices += 2;
	}
}


namespace SourceEngine
{
	 unsigned int * PackNormal_SHORT2(float nx, float ny, float nz, unsigned int *pPackedNormal, float binormalSign = +1.0f)
	{
		// Pack a vector (ASSUMED TO BE NORMALIZED) into Jason's 4-byte (SHORT2) format.
		// This simply reconstructs Z from X & Y. It uses the sign bits of the X & Y coords
		// to reconstruct the sign of Z and, if this is a tangent vector, the sign of the
		// binormal (this is needed because tangent/binormal vectors are supposed to follow
		// UV gradients, but shaders reconstruct the binormal from the tangent and normal
		// assuming that they form a right-handed basis).

		nx += 1;					// [-1,+1] -> [0,2]
		ny += 1;
		nx *= 16384.0f;				// [ 0, 2] -> [0,32768]
		ny *= 16384.0f;

		// '0' and '32768' values are invalid encodings
		nx = max(nx, 1.0f);		// Make sure there are no zero values
		ny = max(ny, 1.0f);
		nx = min(nx, 32767.0f);	// Make sure there are no 32768 values
		ny = min(ny, 32767.0f);

		if (nz < 0.0f)
			nx = -nx;				// Set the sign bit for z

		ny *= binormalSign;			// Set the sign bit for the binormal (use when encoding a tangent vector)

									// FIXME: short math is slow on 360 - use ints here instead (bit-twiddle to deal w/ the sign bits), also use Float2Int()
		auto sX = static_cast<short>(nx);		// signed short [1,32767]
		auto sY = static_cast<short>(ny);

		*pPackedNormal = (sX & 0x0000FFFF) | (sY << 16); // NOTE: The mask is necessary (if sX is negative and cast to an int...)

		return pPackedNormal;
	}

	 unsigned int * PackNormal_SHORT2(const float *pNormal, unsigned int *pPackedNormal, float binormalSign = +1.0f)
	{
		return PackNormal_SHORT2(pNormal[0], pNormal[1], pNormal[2], pPackedNormal, binormalSign);
	}

	int VertexFlags(VertexFormat_t vertexFormat)
	{
		return static_cast<int> (vertexFormat & ((1 << (VERTEX_LAST_BIT + 1)) - 1));
	}

	int NumBoneWeights(VertexFormat_t vertexFormat)
	{
		return static_cast<int> ((vertexFormat >> VERTEX_BONE_WEIGHT_BIT) & 0x7);
	}

	int UserDataSize(VertexFormat_t vertexFormat)
	{
		return static_cast<int> ((vertexFormat >> USER_DATA_SIZE_BIT) & 0x7);
	}

	int TexCoordSize(int nTexCoordIndex, VertexFormat_t vertexFormat)
	{
		return static_cast<int> ((vertexFormat >> (TEX_COORD_SIZE_BIT + 3 * nTexCoordIndex)) & 0x7);
	}

	bool UsesVertexShader(VertexFormat_t vertexFormat)
	{
		return (vertexFormat & VERTEX_FORMAT_VERTEX_SHADER) != 0;
	}

	VertexCompressionType_t CompressionType(VertexFormat_t vertexFormat)
	{
		// This is trivial now, but we may add multiple flavours of compressed vertex later on
		if (vertexFormat & VERTEX_FORMAT_COMPRESSED)
			return VERTEX_COMPRESSION_ON;
		return VERTEX_COMPRESSION_NONE;
	}

	CIndexBuilder::CIndexBuilder() : m_pIndexBuffer(nullptr), m_nMaxIndexCount(0), m_nIndexCount(0),
	                                 m_nIndexOffset(0), m_nCurrentIndex(0), m_bModify(false)
	{
		m_nTotalIndexCount = 0;
		m_nBufferOffset = INVALID_BUFFER_OFFSET;
		m_nBufferFirstIndex = 0;
	}

	CIndexBuilder::CIndexBuilder(IIndexBuffer* pIndexBuffer, MaterialIndexFormat_t fmt): m_nIndexOffset(0), m_bModify(false)
	{
		m_pIndexBuffer = pIndexBuffer;
		m_nBufferOffset = INVALID_BUFFER_OFFSET;
		m_nBufferFirstIndex = 0;
		m_nIndexCount = 0;
		m_nCurrentIndex = 0;
		m_nMaxIndexCount = 0;
		m_nTotalIndexCount = 0;
		if (m_pIndexBuffer->IsDynamic())
		{
			m_pIndexBuffer->BeginCastBuffer(fmt);
		}
		else
		{
			//Assert(m_pIndexBuffer->IndexFormat() == fmt);
		}
	}

	bool CIndexBuilder::Lock(int nMaxIndexCount, int nIndexOffset, bool bAppend)
	{
		m_bModify = false;
		m_nIndexOffset = nIndexOffset;
		m_nMaxIndexCount = nMaxIndexCount;
		bool bFirstLock = (m_nBufferOffset == INVALID_BUFFER_OFFSET);
		if (bFirstLock)
		{
			bAppend = false;
		}
		if (!bAppend)
		{
			m_nTotalIndexCount = 0;
		}
		Reset();

		// Lock the index buffer
		if (!m_pIndexBuffer->Lock(m_nMaxIndexCount, bAppend, *this))
		{
			m_nMaxIndexCount = 0;
			return false;
		}

		if (bFirstLock)
		{
			m_nBufferOffset = m_nOffset;
			m_nBufferFirstIndex = m_nFirstIndex;
		}

		return true;
	}

	void CIndexBuilder::Unlock()
	{
		m_pIndexBuffer->Unlock(m_nIndexCount, *this);
		m_nTotalIndexCount += m_nIndexCount;

		m_nMaxIndexCount = 0;

#ifdef _DEBUG
		// Null out our data...
		memset((IndexDesc_t*)this, 0, sizeof(IndexDesc_t));
#endif
	}

	void CIndexBuilder::SpewData() const
	{
		m_pIndexBuffer->Spew(m_nIndexCount, *this);
	}


	//-----------------------------------------------------------------------------
	// Binds this index buffer
	//-----------------------------------------------------------------------------
	void CIndexBuilder::Bind(IMatRenderContext* pContext) const
	{
		if (m_pIndexBuffer && (m_nBufferOffset != INVALID_BUFFER_OFFSET))
		{
			pContext->BindIndexBuffer(m_pIndexBuffer, m_nBufferOffset);
		}
		else
		{
			pContext->BindIndexBuffer(nullptr, 0);
		}
	}


	//-----------------------------------------------------------------------------
	// Returns the byte offset
	//-----------------------------------------------------------------------------
	int CIndexBuilder::Offset() const
	{
		return m_nBufferOffset;
	}

	int CIndexBuilder::GetFirstIndex() const
	{
		return m_nBufferFirstIndex;
	}


	//-----------------------------------------------------------------------------
	// Begins, ends modification of the index buffer
	//-----------------------------------------------------------------------------
	void CIndexBuilder::Begin(IIndexBuffer* pIndexBuffer, int nMaxIndexCount, int nIndexOffset)
	{
		m_pIndexBuffer = pIndexBuffer;
		m_nIndexCount = 0;
		m_nMaxIndexCount = nMaxIndexCount;
		m_nIndexOffset = nIndexOffset;

		m_bModify = false;

		// Lock the index buffer
		m_pIndexBuffer->Lock(m_nMaxIndexCount, false, *this);

		// Point to the start of the buffers..
		Reset();
	}

	void CIndexBuilder::End(bool bSpewData)
	{
		// Make sure they called Begin()  

		if (bSpewData)
		{
			m_pIndexBuffer->Spew(m_nIndexCount, *this);
		}

		// Unlock our buffers
		m_pIndexBuffer->Unlock(m_nIndexCount, *this);

		m_pIndexBuffer = nullptr;
		m_nMaxIndexCount = 0;

#ifdef _DEBUG
		// Null out our data...
		memset((IndexDesc_t*)this, 0, sizeof(IndexDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// Begins, ends modification of an existing index buffer which has already been filled out
	//-----------------------------------------------------------------------------
	void CIndexBuilder::BeginModify(IIndexBuffer* pIndexBuffer, int nFirstIndex, int nIndexCount, int nIndexOffset)
	{
		m_pIndexBuffer = pIndexBuffer;
		m_nIndexCount = nIndexCount;
		m_nMaxIndexCount = nIndexCount;
		m_nIndexOffset = nIndexOffset;
		m_bModify = true;

		// Lock the vertex and index buffer
		m_pIndexBuffer->ModifyBegin(false, nFirstIndex, nIndexCount, *this);

		// Point to the start of the buffers..
		Reset();
	}

	void CIndexBuilder::EndModify(bool bSpewData)
	{
		// Make sure they called BeginModify.

		if (bSpewData)
		{
			m_pIndexBuffer->Spew(m_nIndexCount, *this);
		}

		// Unlock our buffers
		m_pIndexBuffer->ModifyEnd(*this);

		m_pIndexBuffer = nullptr;
		m_nMaxIndexCount = 0;

#ifdef _DEBUG
		// Null out our data...
		memset((IndexDesc_t*)this, 0, sizeof(IndexDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// FIXME: Remove! Backward compat so we can use this from a CMeshBuilder.
	//-----------------------------------------------------------------------------
	void CIndexBuilder::AttachBegin(IMesh* pMesh, int nMaxIndexCount, const MeshDesc_t& desc)
	{
		m_pIndexBuffer = pMesh;
		m_nIndexCount = 0;
		m_nMaxIndexCount = nMaxIndexCount;

		m_bModify = false;

		// Copy relevant data from the mesh desc
		m_nIndexOffset = desc.m_nFirstVertex;
		m_pIndices = desc.m_pIndices;
		m_nIndexSize = desc.m_nIndexSize;

		// Point to the start of the buffers..
		Reset();
	}

	void CIndexBuilder::AttachEnd()
	{
		// Make sure they called AttachBegin.

		m_pIndexBuffer = nullptr;
		m_nMaxIndexCount = 0;

#ifdef _DEBUG
		// Null out our data...
		memset((IndexDesc_t*)this, 0, sizeof(IndexDesc_t));
#endif
	}

	void CIndexBuilder::AttachBeginModify(IMesh* pMesh, int nFirstIndex, int nIndexCount, const MeshDesc_t& desc)
	{
		m_pIndexBuffer = pMesh;
		m_nIndexCount = nIndexCount;
		m_nMaxIndexCount = nIndexCount;
		m_bModify = true;

		// Copy relevant data from the mesh desc
		m_nIndexOffset = desc.m_nFirstVertex;
		m_pIndices = desc.m_pIndices;
		m_nIndexSize = desc.m_nIndexSize;

		// Point to the start of the buffers..
		Reset();
	}

	void CIndexBuilder::AttachEndModify()
	{
		// Make sure they called AttachBeginModify.
		AttachEnd();
//		m_pIndexBuffer = nullptr;
//		m_nMaxIndexCount = 0;
//
//#ifdef _DEBUG
//		// Null out our data...
//		memset((IndexDesc_t*)this, 0, sizeof(IndexDesc_t));
//#endif
	}


	//-----------------------------------------------------------------------------
	// Resets the index buffer builder so it points to the start of everything again
	//-----------------------------------------------------------------------------
	void CIndexBuilder::Reset() const
	{
		m_nCurrentIndex = 0;
	}


	//-----------------------------------------------------------------------------
	// returns the number of indices
	//-----------------------------------------------------------------------------
	int CIndexBuilder::IndexCount() const
	{
		return m_nIndexCount;
	}


	//-----------------------------------------------------------------------------
	// Returns the total number of indices across all Locks()
	//-----------------------------------------------------------------------------
	int CIndexBuilder::TotalIndexCount() const
	{
		return m_nTotalIndexCount;
	}


	//-----------------------------------------------------------------------------
	// Advances the current index
	//-----------------------------------------------------------------------------
	void CIndexBuilder::AdvanceIndex()
	{
		m_nCurrentIndex += m_nIndexSize;
		if (m_nCurrentIndex > m_nIndexCount)
		{
			m_nIndexCount = m_nCurrentIndex;
		}
	}

	void CIndexBuilder::AdvanceIndices(int nIndices)
	{
		m_nCurrentIndex += nIndices * m_nIndexSize;
		if (m_nCurrentIndex > m_nIndexCount)
		{
			m_nIndexCount = m_nCurrentIndex;
		}
	}


	//-----------------------------------------------------------------------------
	// Returns the current index
	//-----------------------------------------------------------------------------
	int CIndexBuilder::GetCurrentIndex() const
	{
		return m_nCurrentIndex;
	}

	unsigned short const* CIndexBuilder::Index() const
	{
		return &m_pIndices[m_nCurrentIndex];
	}

	void CIndexBuilder::SelectIndex(int nIndex) const
	{
		m_nCurrentIndex = nIndex * m_nIndexSize;
	}


	//-----------------------------------------------------------------------------
	// Used to write data into the index buffer
	//-----------------------------------------------------------------------------
	void CIndexBuilder::Index(unsigned short nIndex) const
	{
		m_pIndices[m_nCurrentIndex] = (unsigned short)(m_nIndexOffset + nIndex);
	}

	// Fast Index! No need to call advance index
	void CIndexBuilder::FastIndex(unsigned short nIndex)
	{
		m_pIndices[m_nCurrentIndex] = (unsigned short)(m_nIndexOffset + nIndex);
		m_nCurrentIndex += m_nIndexSize;
		m_nIndexCount = m_nCurrentIndex;
	}

	//-----------------------------------------------------------------------------
	// NOTE: This version is the one you really want to achieve write-combining;
	// Write combining only works if you write in 4 bytes chunks.
	//-----------------------------------------------------------------------------
	void CIndexBuilder::FastIndex2(unsigned short nIndex1, unsigned short nIndex2)
	{
		//Assert(m_pIndices);
		//Assert(m_nCurrentIndex < m_nMaxIndexCount - 1);
		//	Assert( ( (int)( &m_pIndices[m_nCurrentIndex] ) & 0x3 ) == 0 );

#ifndef _X360
		unsigned int nIndices = ((unsigned int)nIndex1 + m_nIndexOffset) | (((unsigned int)nIndex2 + m_nIndexOffset) << 16);
#else
		unsigned int nIndices = ((unsigned int)nIndex2 + m_nIndexOffset) | (((unsigned int)nIndex1 + m_nIndexOffset) << 16);
#endif

		*(int*)(&m_pIndices[m_nCurrentIndex]) = nIndices;
		m_nCurrentIndex += m_nIndexSize + m_nIndexSize;
		m_nIndexCount = m_nCurrentIndex;
	}


	//-----------------------------------------------------------------------------
	// Generates indices for a particular primitive type
	//-----------------------------------------------------------------------------
	void CIndexBuilder::GenerateIndices(MaterialPrimitiveType_t primitiveType, int nIndexCount)
	{
		// FIXME: How to make this work with short vs int sized indices?
		// Don't generate indices if we've got an empty buffer
		if (m_nIndexSize == 0)
			return;

		int nMaxIndices = m_nMaxIndexCount - m_nCurrentIndex;
		nIndexCount = min(nMaxIndices, nIndexCount);
		if (nIndexCount == 0)
			return;

		unsigned short* pIndices = &m_pIndices[m_nCurrentIndex];

		switch (primitiveType)
		{
		case MATERIAL_INSTANCED_QUADS:
			//Assert(0); // Shouldn't get here (this primtype is unindexed)
			break;
		case MATERIAL_QUADS:
			GenerateQuadIndexBuffer(pIndices, nIndexCount, m_nIndexOffset);
			break;
		case MATERIAL_POLYGON:
			GeneratePolygonIndexBuffer(pIndices, nIndexCount, m_nIndexOffset);
			break;
		case MATERIAL_LINE_STRIP:
			GenerateLineStripIndexBuffer(pIndices, nIndexCount, m_nIndexOffset);
			break;
		case MATERIAL_LINE_LOOP:
			GenerateLineLoopIndexBuffer(pIndices, nIndexCount, m_nIndexOffset);
			break;
		case MATERIAL_POINTS:
			//Assert(0); // Shouldn't get here (this primtype is unindexed)
			break;
		default:
			GenerateSequentialIndexBuffer(pIndices, nIndexCount, m_nIndexOffset);
			break;
		}

		AdvanceIndices(nIndexCount);
	}


	//-----------------------------------------------------------------------------
	//
	// methods of CVertexBuilder
	//
	//-----------------------------------------------------------------------------
	CVertexBuilder::CVertexBuilder(): m_bModify(false), m_pCurrPosition(nullptr), m_pCurrNormal(nullptr), m_pCurrColor(nullptr), m_bWrittenNormal(false), m_bWrittenUserData(false)
	{
		m_pVertexBuffer = nullptr;
		m_nBufferOffset = INVALID_BUFFER_OFFSET;
		m_nBufferFirstVertex = 0;
		m_nVertexCount = 0;
		m_nCurrentVertex = 0;
		m_nMaxVertexCount = 0;
		m_nTotalVertexCount = 0;
		m_CompressionType = VERTEX_COMPRESSION_INVALID;
		memset(m_pCurrTexCoord, 0, sizeof(float*[8]));

#ifdef _DEBUG
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		m_bModify = false;
#endif
	}

	CVertexBuilder::CVertexBuilder(IVertexBuffer* pVertexBuffer, VertexFormat_t fmt): m_bModify(false), m_pCurrPosition(nullptr), m_pCurrNormal(nullptr), m_pCurrColor(nullptr), m_bWrittenNormal(false), m_bWrittenUserData(false)
	{
		m_pVertexBuffer = pVertexBuffer;
		m_nBufferOffset = INVALID_BUFFER_OFFSET;
		m_nBufferFirstVertex = 0;
		m_nVertexCount = 0;
		m_nCurrentVertex = 0;
		m_nMaxVertexCount = 0;
		m_nTotalVertexCount = 0;
		m_CompressionType = VERTEX_COMPRESSION_INVALID;

		memset(m_pCurrTexCoord, 0, sizeof(float*[8]));

		if (m_pVertexBuffer->IsDynamic())
		{
			m_pVertexBuffer->BeginCastBuffer(fmt);
		}
		else
		{
		}

#ifdef _DEBUG
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		m_bModify = false;
#endif
	}

	CVertexBuilder::~CVertexBuilder()
	{
		if (m_pVertexBuffer && m_pVertexBuffer->IsDynamic())
		{
			m_pVertexBuffer->EndCastBuffer();
		}
	}

	//-----------------------------------------------------------------------------
	// Begins, ends modification of the index buffer
	//-----------------------------------------------------------------------------
	bool CVertexBuilder::Lock(int nMaxVertexCount, bool bAppend)
	{
		m_bModify = false;
		m_nMaxVertexCount = nMaxVertexCount;
		bool bFirstLock = (m_nBufferOffset == INVALID_BUFFER_OFFSET);
		if (bFirstLock)
		{
			bAppend = false;
		}
		if (!bAppend)
		{
			m_nTotalVertexCount = 0;
		}

		// Lock the vertex buffer
		if (!m_pVertexBuffer->Lock(m_nMaxVertexCount, bAppend, *this))
		{
			m_nMaxVertexCount = 0;
			return false;
		}

		Reset();

		if (bFirstLock)
		{
			m_nBufferOffset = m_nOffset;
			m_nBufferFirstVertex = m_nFirstVertex;
		}

		return true;
	}

	void CVertexBuilder::Unlock()
	{

#ifdef _DEBUG
		m_pVertexBuffer->ValidateData(m_nVertexCount, *this);
#endif

		m_pVertexBuffer->Unlock(m_nVertexCount, *this);
		m_nTotalVertexCount += m_nVertexCount;

		m_nMaxVertexCount = 0;

#ifdef _DEBUG
		// Null out our data...
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		memset(static_cast<VertexDesc_t*>(this), 0, sizeof(VertexDesc_t));
#endif
	}

	void CVertexBuilder::SpewData() const
	{
		m_pVertexBuffer->Spew(m_nVertexCount, *this);
	}


	//-----------------------------------------------------------------------------
	// Binds this vertex buffer
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Bind(IMatRenderContext* pContext, int nStreamID, VertexFormat_t usage) const
	{
		if (m_pVertexBuffer && (m_nBufferOffset != INVALID_BUFFER_OFFSET))
		{
			pContext->BindVertexBuffer(nStreamID, m_pVertexBuffer, m_nBufferOffset,
			                           m_nFirstVertex, m_nTotalVertexCount, usage ? usage : m_pVertexBuffer->GetVertexFormat());
		}
		else
		{
			pContext->BindVertexBuffer(nStreamID, nullptr, 0, 0, 0, 0);
		}
	}


	//-----------------------------------------------------------------------------
	// Returns the byte offset
	//-----------------------------------------------------------------------------
	int CVertexBuilder::Offset() const
	{
		return m_nBufferOffset;
	}

	int CVertexBuilder::GetFirstVertex() const
	{
		return m_nBufferFirstVertex;
	}

	//-----------------------------------------------------------------------------
	// Specify the type of vertex compression that this CMeshBuilder will perform
	//-----------------------------------------------------------------------------
	void CVertexBuilder::SetCompressionType(VertexCompressionType_t compressionType)
	{
		// The real purpose of this method is to allow us to emit a Warning in Begin()
		m_CompressionType = compressionType;
	}

	void CVertexBuilder::ValidateCompressionType()
	{
//#ifdef _DEBUG
//		VertexCompressionType_t vbCompressionType = CompressionType(m_pVertexBuffer->GetVertexFormat());
//		if (vbCompressionType != VERTEX_COMPRESSION_NONE)
//		{
//			if (m_CompressionType != vbCompressionType)
//			{
//			}
//			// Never use vertex compression for dynamic VBs (the conversions can really hurt perf)
//		}
//#endif
	}

	void CVertexBuilder::Begin(IVertexBuffer* pVertexBuffer, int nVertexCount)
	{

		m_pVertexBuffer = pVertexBuffer;
		m_bModify = false;

		m_nMaxVertexCount = nVertexCount;
		m_nVertexCount = 0;

		// Make sure SetCompressionType was called correctly, if this VB is compressed
		ValidateCompressionType();

		// Lock the vertex and index buffer
		m_pVertexBuffer->Lock(m_nMaxVertexCount, false, *this);

		// Point to the start of the buffers..
		Reset();
	}


	//-----------------------------------------------------------------------------
	// Use this when you're done modifying the mesh
	//-----------------------------------------------------------------------------
	void CVertexBuilder::End(bool bSpewData)
	{
		// Make sure they called Begin()
		if (bSpewData)
		{
			m_pVertexBuffer->Spew(m_nVertexCount, *this);
		}

#ifdef _DEBUG
		m_pVertexBuffer->ValidateData(m_nVertexCount, *this);
#endif

		// Unlock our buffers
		m_pVertexBuffer->Unlock(m_nVertexCount, *this);

		m_pVertexBuffer = nullptr;
		m_nMaxVertexCount = 0;

		m_CompressionType = VERTEX_COMPRESSION_INVALID;

#ifdef _DEBUG
		// Null out our pointers...
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		memset(static_cast<VertexDesc_t*>(this), 0, sizeof(VertexDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// FIXME: Remove! Backward compat so we can use this from a CMeshBuilder.
	//-----------------------------------------------------------------------------
	void CVertexBuilder::AttachBegin(IMesh* pMesh, int nMaxVertexCount, const MeshDesc_t& desc)
	{
		m_pVertexBuffer = pMesh;
		memcpy(static_cast<VertexDesc_t*>(this), static_cast<const VertexDesc_t*>(&desc), sizeof(VertexDesc_t));
		m_nMaxVertexCount = nMaxVertexCount;
		m_NumBoneWeights = m_NumBoneWeights == 0 ? 0 : 2; // Two weights if any
		m_nVertexCount = 0;
		m_bModify = false;

		// Make sure SetCompressionType was called correctly, if this VB is compressed
		ValidateCompressionType();

		if (m_nBufferOffset == INVALID_BUFFER_OFFSET)
		{
			m_nTotalVertexCount = 0;
			m_nBufferOffset = static_cast<const VertexDesc_t*>(&desc)->m_nOffset;
			m_nBufferFirstVertex = desc.m_nFirstVertex;
		}
	}

	void CVertexBuilder::AttachEnd()
	{
		// Make sure they called Begin()

		m_nMaxVertexCount = 0;
		m_pVertexBuffer = nullptr;

		m_CompressionType = VERTEX_COMPRESSION_INVALID;

#ifdef _DEBUG
		// Null out our pointers...
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		memset(static_cast<VertexDesc_t*>(this), 0, sizeof(VertexDesc_t));
#endif
	}

	void CVertexBuilder::AttachBeginModify(IMesh* pMesh, int nFirstVertex, int nVertexCount, const MeshDesc_t& desc)
	{
		m_pVertexBuffer = pMesh;
		memcpy(static_cast<VertexDesc_t*>(this), static_cast<const VertexDesc_t*>(&desc), sizeof(VertexDesc_t));
		m_nMaxVertexCount = m_nVertexCount = nVertexCount;
		m_NumBoneWeights = m_NumBoneWeights == 0 ? 0 : 2; // Two weights if any
		m_bModify = true;

		// Make sure SetCompressionType was called correctly, if this VB is compressed
		ValidateCompressionType();
	}

	void CVertexBuilder::AttachEndModify()
	{// Make sure they called BeginModify.

		m_pVertexBuffer = nullptr;
		m_nMaxVertexCount = 0;

		m_CompressionType = VERTEX_COMPRESSION_INVALID;

#ifdef _DEBUG
		// Null out our pointers...
		m_pCurrPosition = nullptr;
		m_pCurrNormal = nullptr;
		m_pCurrColor = nullptr;
		memset(m_pCurrTexCoord, 0, sizeof(m_pCurrTexCoord));
		memset(static_cast<VertexDesc_t*>(this), 0, sizeof(VertexDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// Computes the first min non-null address
	//-----------------------------------------------------------------------------
	unsigned char* FindMinAddress(void* pAddress1, void* pAddress2, int nAddress2Size)
	{
		if (nAddress2Size == 0)
			return (unsigned char*)pAddress1;
		if (!pAddress1)
			return (unsigned char*)pAddress2;
		return (pAddress1 < pAddress2) ? (unsigned char*)pAddress1 : (unsigned char*)pAddress2;
	}

	//-----------------------------------------------------------------------------
	// Resets the vertex buffer builder so it points to the start of everything again
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Reset()
	{
		m_nCurrentVertex = 0;

		m_pCurrPosition = m_pPosition;
		m_pCurrNormal = m_pNormal;
		for (int i = 0; i < NELEMS(m_pCurrTexCoord); i++)
		{
			m_pCurrTexCoord[i] = m_pTexCoord[i];
		}
		m_pCurrColor = m_pColor;

#if ( defined( _DEBUG ) && ( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 ) )
		m_bWrittenNormal = false;
		m_bWrittenUserData = false;
#endif

#ifdef DEBUG_WRITE_COMBINE
		// Logic for m_pLastWrittenAddress is tricky. It really wants the min of the
		// non-null address pointers.
	m_nLastWrittenField = MB_FIELD_NONE;
	m_pLastWrittenAddress = NULL;
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pPosition, m_VertexSize_Position);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pBoneWeight, m_VertexSize_BoneWeight);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pBoneMatrixIndex, m_VertexSize_BoneMatrixIndex);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pNormal, m_VertexSize_Normal);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pColor, m_VertexSize_Color);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pSpecular, m_VertexSize_Specular);
	for (int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; ++i)
	{
		m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pTexCoord[i], m_VertexSize_TexCoord[i]);
	}
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pTangentS, m_VertexSize_TangentS);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pTangentT, m_VertexSize_TangentT);
	m_pLastWrittenAddress = FindMinAddress(m_pLastWrittenAddress, m_pUserData, m_VertexSize_UserData);
#endif
	}


	//-----------------------------------------------------------------------------
	// returns the number of vertices
	//-----------------------------------------------------------------------------
	int CVertexBuilder::VertexCount() const
	{
		return m_nVertexCount;
	}


	//-----------------------------------------------------------------------------
	// Returns the total number of vertices across all Locks()
	//-----------------------------------------------------------------------------
	int CVertexBuilder::TotalVertexCount() const
	{
		return m_nTotalVertexCount;
	}


	//-----------------------------------------------------------------------------
	// Returns the base vertex memory pointer
	//-----------------------------------------------------------------------------
	void* CVertexBuilder::BaseVertexData() const
	{
		// FIXME: If there's no position specified, we need to find
		// the base address 
		return m_pPosition;
	}


	//-----------------------------------------------------------------------------
	// Selects the current vertex
	//-----------------------------------------------------------------------------
	void CVertexBuilder::SelectVertex(int nIndex)
	{
		// NOTE: This index is expected to be relative 
		m_nCurrentVertex = nIndex;

		m_pCurrPosition = OffsetFloatPointer(m_pPosition, m_nCurrentVertex, m_VertexSize_Position);
		m_pCurrNormal = OffsetFloatPointer(m_pNormal, m_nCurrentVertex, m_VertexSize_Normal);

		m_pCurrTexCoord[0] = OffsetFloatPointer(m_pTexCoord[0], m_nCurrentVertex, m_VertexSize_TexCoord[0]);
		m_pCurrTexCoord[1] = OffsetFloatPointer(m_pTexCoord[1], m_nCurrentVertex, m_VertexSize_TexCoord[1]);
		m_pCurrTexCoord[2] = OffsetFloatPointer(m_pTexCoord[2], m_nCurrentVertex, m_VertexSize_TexCoord[2]);
		m_pCurrTexCoord[3] = OffsetFloatPointer(m_pTexCoord[3], m_nCurrentVertex, m_VertexSize_TexCoord[3]);
		m_pCurrTexCoord[4] = OffsetFloatPointer(m_pTexCoord[4], m_nCurrentVertex, m_VertexSize_TexCoord[4]);
		m_pCurrTexCoord[5] = OffsetFloatPointer(m_pTexCoord[5], m_nCurrentVertex, m_VertexSize_TexCoord[5]);
		m_pCurrTexCoord[6] = OffsetFloatPointer(m_pTexCoord[6], m_nCurrentVertex, m_VertexSize_TexCoord[6]);
		m_pCurrTexCoord[7] = OffsetFloatPointer(m_pTexCoord[7], m_nCurrentVertex, m_VertexSize_TexCoord[7]);
		m_pCurrColor = m_pColor + m_nCurrentVertex * m_VertexSize_Color;

#if ( defined( _DEBUG ) && ( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 ) )
		m_bWrittenNormal = false;
		m_bWrittenUserData = false;
#endif
	}


	//-----------------------------------------------------------------------------
	// Advances vertex after you're done writing to it.
	//-----------------------------------------------------------------------------
	void CVertexBuilder::AdvanceVertex()
	{
		if (++m_nCurrentVertex > m_nVertexCount)
		{
			m_nVertexCount = m_nCurrentVertex;
		}

		IncrementFloatPointer(m_pCurrPosition, m_VertexSize_Position);
		IncrementFloatPointer(m_pCurrNormal, m_VertexSize_Normal);

		IncrementFloatPointer(m_pCurrTexCoord[0], m_VertexSize_TexCoord[0]);
		IncrementFloatPointer(m_pCurrTexCoord[1], m_VertexSize_TexCoord[1]);
		IncrementFloatPointer(m_pCurrTexCoord[2], m_VertexSize_TexCoord[2]);
		IncrementFloatPointer(m_pCurrTexCoord[3], m_VertexSize_TexCoord[3]);
		IncrementFloatPointer(m_pCurrTexCoord[4], m_VertexSize_TexCoord[4]);
		IncrementFloatPointer(m_pCurrTexCoord[5], m_VertexSize_TexCoord[5]);
		IncrementFloatPointer(m_pCurrTexCoord[6], m_VertexSize_TexCoord[6]);
		IncrementFloatPointer(m_pCurrTexCoord[7], m_VertexSize_TexCoord[7]);
		m_pCurrColor += m_VertexSize_Color;

#if ( defined( _DEBUG ) && ( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 ) )
		m_bWrittenNormal = false;
		m_bWrittenUserData = false;
#endif
	}

	void CVertexBuilder::AdvanceVertices(int nVerts)
	{
		m_nCurrentVertex += nVerts;
		if (m_nCurrentVertex > m_nVertexCount)
		{
			m_nVertexCount = m_nCurrentVertex;
		}

		IncrementFloatPointer(m_pCurrPosition, m_VertexSize_Position * nVerts);
		IncrementFloatPointer(m_pCurrNormal, m_VertexSize_Normal * nVerts);

		IncrementFloatPointer(m_pCurrTexCoord[0], m_VertexSize_TexCoord[0] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[1], m_VertexSize_TexCoord[1] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[2], m_VertexSize_TexCoord[2] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[3], m_VertexSize_TexCoord[3] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[4], m_VertexSize_TexCoord[4] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[5], m_VertexSize_TexCoord[5] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[6], m_VertexSize_TexCoord[6] * nVerts);
		IncrementFloatPointer(m_pCurrTexCoord[7], m_VertexSize_TexCoord[7] * nVerts);
		m_pCurrColor += m_VertexSize_Color * nVerts;

#if ( defined( _DEBUG ) && ( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 ) )
		m_bWrittenNormal = false;
		m_bWrittenUserData = false;
#endif
	}


	//-----------------------------------------------------------------------------
	// For use with the FastVertex methods, advances the current vertex by N
	//-----------------------------------------------------------------------------
	void CVertexBuilder::FastAdvanceNVertices(int n)
	{
		m_nCurrentVertex += n;
		m_nVertexCount = m_nCurrentVertex;
	}

	//-----------------------------------------------------------------------------
	// Returns the current vertex
	//-----------------------------------------------------------------------------
	int CVertexBuilder::GetCurrentVertex() const
	{
		return m_nCurrentVertex;
	}
	
	//-----------------------------------------------------------------------------
	// Data retrieval...
	//-----------------------------------------------------------------------------
	const float* CVertexBuilder::Position() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return m_pCurrPosition;
	}

	const float* CVertexBuilder::Normal() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return m_pCurrNormal;
	}

	unsigned int CVertexBuilder::Color() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		// Swizzle it so it returns the same format as accepted by Color4ubv - rgba
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		unsigned int color = (m_pCurrColor[3] << 24) | (m_pCurrColor[0] << 16) | (m_pCurrColor[1] << 8) | (m_pCurrColor[2]);

		return color;
	}

	unsigned char* CVertexBuilder::Specular() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return m_pSpecular + m_nCurrentVertex * m_VertexSize_Specular;
	}

	const float* CVertexBuilder::TexCoord(int stage) const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return m_pCurrTexCoord[stage];
	}

	const float* CVertexBuilder::TangentS() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return OffsetFloatPointer(m_pTangentS, m_nCurrentVertex, m_VertexSize_TangentS);
	}

	const float* CVertexBuilder::TangentT() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return OffsetFloatPointer(m_pTangentT, m_nCurrentVertex, m_VertexSize_TangentT);
	}

	float CVertexBuilder::Wrinkle() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return *OffsetFloatPointer(m_pWrinkle, m_nCurrentVertex, m_VertexSize_Wrinkle);
	}

	const float* CVertexBuilder::BoneWeight() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return OffsetFloatPointer(m_pBoneWeight, m_nCurrentVertex, m_VertexSize_BoneWeight);
	}

	int CVertexBuilder::NumBoneWeights() const
	{
		return m_NumBoneWeights;
	}

#ifndef NEW_SKINNING
	unsigned char* CVertexBuilder::BoneMatrix() const
	{
		// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
		//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
		Assert(m_nCurrentVertex < m_nMaxVertexCount);
		return m_pBoneMatrixIndex + m_nCurrentVertex * m_VertexSize_BoneMatrixIndex;
	}
#else
float* CVertexBuilder::BoneMatrix() const
{
	// FIXME: add a templatized accessor (return type varies to ensure calling code is updated appropriately)
	//        for code that needs to access compressed data (and/or a return-by-value templatized accessor)
	Assert(m_CompressionType == VERTEX_COMPRESSION_NONE);
	Assert(m_nCurrentVertex < m_nMaxVertexCount);
	return m_pBoneMatrixIndex + m_nCurrentVertex * m_VertexSize_BoneMatrixIndex;
}
#endif


	//-----------------------------------------------------------------------------
	// Position setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Position3f(float x, float y, float z) const
	{
		Assert(m_pPosition && m_pCurrPosition);
		Assert(IsFinite(x) && IsFinite(y) && IsFinite(z));
		float* pDst = m_pCurrPosition;
		*pDst++ = x;
		*pDst++ = y;
		*pDst = z;
	}

	void CVertexBuilder::Position3fv(const float* v) const
	{
		Assert(v);
		Assert(m_pPosition && m_pCurrPosition);

		float* pDst = m_pCurrPosition;
		*pDst++ = *v++;
		*pDst++ = *v++;
		*pDst = *v;
	}


	//-----------------------------------------------------------------------------
	// Normal setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Normal3f(float nx, float ny, float nz) const
	{
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE); // Use the templatized version if you want to support compression
		Assert(m_pNormal);
		Assert(IsFinite(nx) && IsFinite(ny) && IsFinite(nz));
		Assert(nx >= -1.05f && nx <= 1.05f);
		Assert(ny >= -1.05f && ny <= 1.05f);
		Assert(nz >= -1.05f && nz <= 1.05f);

		float* pDst = m_pCurrNormal;
		*pDst++ = nx;
		*pDst++ = ny;
		*pDst = nz;
	}

	void CVertexBuilder::Normal3fv(const float* n) const
	{
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE); // Use the templatized version if you want to support compression
		Assert(n);
		Assert(m_pNormal && m_pCurrNormal);
		Assert(IsFinite(n[0]) && IsFinite(n[1]) && IsFinite(n[2]));
		Assert(n[0] >= -1.05f && n[0] <= 1.05f);
		Assert(n[1] >= -1.05f && n[1] <= 1.05f);
		Assert(n[2] >= -1.05f && n[2] <= 1.05f);

		float* pDst = m_pCurrNormal;
		*pDst++ = *n++;
		*pDst++ = *n++;
		*pDst = *n;
	}

	void CVertexBuilder::NormalDelta3f(float nx, float ny, float nz) const
	{
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE); // Use the templatized version if you want to support compression
		Assert(m_pNormal);
		Assert(IsFinite(nx) && IsFinite(ny) && IsFinite(nz));

		float* pDst = m_pCurrNormal;
		*pDst++ = nx;
		*pDst++ = ny;
		*pDst = nz;
	}

	void CVertexBuilder::NormalDelta3fv(const float* n) const
	{
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE); // Use the templatized version if you want to support compression
		Assert(n);
		Assert(m_pNormal && m_pCurrNormal);
		Assert(IsFinite(n[0]) && IsFinite(n[1]) && IsFinite(n[2]));

		float* pDst = m_pCurrNormal;
		*pDst++ = *n++;
		*pDst++ = *n++;
		*pDst = *n;
	}

	//-----------------------------------------------------------------------------
	// Templatized normal setting methods which support compressed vertices
	//-----------------------------------------------------------------------------
	template <VertexCompressionType_t T>
	void CVertexBuilder::CompressedNormal3f(float nx, float ny, float nz) const
	{
		Assert(T == m_CompressionType);
		Assert(m_pNormal && m_pCurrNormal);
		Assert(IsFinite(nx) && IsFinite(ny) && IsFinite(nz));
		Assert(nx >= -1.05f && nx <= 1.05f);
		Assert(ny >= -1.05f && ny <= 1.05f);
		Assert(nz >= -1.05f && nz <= 1.05f);
		// FIXME: studiorender is passing in non-unit normals
		//float lengthSqd = nx*nx + ny*ny + nz*nz;
		//Assert( lengthSqd >= 0.95f && lengthSqd <= 1.05f );

		if (T == VERTEX_COMPRESSION_ON)
		{
#if		( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_SEPARATETANGENTS_SHORT2 )
			PackNormal_SHORT2(nx, ny, nz, (unsigned int *)m_pCurrNormal);

#else //( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 )
			// NOTE: write the normal into the lower 16 bits of a word, clearing the top 16 bits - a userdata4
			//       tangent must be written into the upper 16 bits by CompressedUserData() *AFTER* this.
#ifdef _DEBUG
		Assert(m_bWrittenUserData == false);
		m_bWrittenNormal = true;
#endif
		PackNormal_UBYTE4(nx, ny, nz, (unsigned int *)m_pCurrNormal);
#endif
		}
		else
		{
			float* pDst = m_pCurrNormal;
			*pDst++ = nx;
			*pDst++ = ny;
			*pDst = nz;
		}
	}

	template <VertexCompressionType_t T>
	void CVertexBuilder::CompressedNormal3fv(const float* n) const
	{
		Assert(n);
		CompressedNormal3f<T>(n[0], n[1], n[2]);
	}


	//-----------------------------------------------------------------------------
	// Color setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Color3f(float r, float g, float b) const
	{
		Assert(m_pColor && m_pCurrColor);
		Assert(IsFinite(r) && IsFinite(g) && IsFinite(b));
		Assert((r >= 0.0) && (g >= 0.0) && (b >= 0.0));
		Assert((r <= 1.0) && (g <= 1.0) && (b <= 1.0));

		int col = (FastFToC(b)) | (FastFToC(g) << 8) | (FastFToC(r) << 16) | 0xFF000000;
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color3fv(const float* rgb) const
	{
		Assert(rgb);
		Assert(m_pColor && m_pCurrColor);
		Assert(IsFinite(rgb[0]) && IsFinite(rgb[1]) && IsFinite(rgb[2]));
		Assert((rgb[0] >= 0.0) && (rgb[1] >= 0.0) && (rgb[2] >= 0.0));
		Assert((rgb[0] <= 1.0) && (rgb[1] <= 1.0) && (rgb[2] <= 1.0));

		int col = (FastFToC(rgb[2])) | (FastFToC(rgb[1]) << 8) | (FastFToC(rgb[0]) << 16) | 0xFF000000;
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color4f(float r, float g, float b, float a) const
	{
		Assert(m_pColor && m_pCurrColor);
		Assert(IsFinite(r) && IsFinite(g) && IsFinite(b) && IsFinite(a));
		Assert((r >= 0.0) && (g >= 0.0) && (b >= 0.0) && (a >= 0.0));
		Assert((r <= 1.0) && (g <= 1.0) && (b <= 1.0) && (a <= 1.0));

		int col = (FastFToC(b)) | (FastFToC(g) << 8) | (FastFToC(r) << 16) | (FastFToC(a) << 24);
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color4fv(const float* rgba) const
	{
		Assert(rgba);
		Assert(m_pColor && m_pCurrColor);
		Assert(IsFinite(rgba[0]) && IsFinite(rgba[1]) && IsFinite(rgba[2]) && IsFinite(rgba[3]));
		Assert((rgba[0] >= 0.0) && (rgba[1] >= 0.0) && (rgba[2] >= 0.0) && (rgba[3] >= 0.0));
		Assert((rgba[0] <= 1.0) && (rgba[1] <= 1.0) && (rgba[2] <= 1.0) && (rgba[3] <= 1.0));

		int col = (FastFToC(rgba[2])) | (FastFToC(rgba[1]) << 8) | (FastFToC(rgba[0]) << 16) | (FastFToC(rgba[3]) << 24);
		*(int*)m_pCurrColor = col;
	}


	//-----------------------------------------------------------------------------
	// Faster versions of color
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Color3ub(unsigned char r, unsigned char g, unsigned char b) const
	{
		Assert(m_pColor && m_pCurrColor);
		int col = b | (g << 8) | (r << 16) | 0xFF000000;
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color3ubv(unsigned char const* rgb) const
	{
		Assert(rgb);
		Assert(m_pColor && m_pCurrColor);

		int col = rgb[2] | (rgb[1] << 8) | (rgb[0] << 16) | 0xFF000000;
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const
	{
		Assert(m_pColor && m_pCurrColor);
		int col = b | (g << 8) | (r << 16) | (a << 24);
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Color4ubv(unsigned char const* rgba) const
	{
		Assert(rgba);
		Assert(m_pColor && m_pCurrColor);
		int col = rgba[2] | (rgba[1] << 8) | (rgba[0] << 16) | (rgba[3] << 24);
		*(int*)m_pCurrColor = col;
	}

	void CVertexBuilder::Specular3f(float r, float g, float b) const
	{
		Assert(m_pSpecular);
		Assert(IsFinite(r) && IsFinite(g) && IsFinite(b));
		Assert((r >= 0.0) && (g >= 0.0) && (b >= 0.0));
		Assert((r <= 1.0) && (g <= 1.0) && (b <= 1.0));

		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = (FastFToC(b)) | (FastFToC(g) << 8) | (FastFToC(r) << 16) | 0xFF000000;
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular3fv(const float* rgb) const
	{
		Assert(rgb);
		Assert(m_pSpecular);
		Assert(IsFinite(rgb[0]) && IsFinite(rgb[1]) && IsFinite(rgb[2]));
		Assert((rgb[0] >= 0.0) && (rgb[1] >= 0.0) && (rgb[2] >= 0.0));
		Assert((rgb[0] <= 1.0) && (rgb[1] <= 1.0) && (rgb[2] <= 1.0));

		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = (FastFToC(rgb[2])) | (FastFToC(rgb[1]) << 8) | (FastFToC(rgb[0]) << 16) | 0xFF000000;
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular4f(float r, float g, float b, float a) const
	{
		Assert(m_pSpecular);
		Assert(IsFinite(r) && IsFinite(g) && IsFinite(b) && IsFinite(a));
		Assert((r >= 0.0) && (g >= 0.0) && (b >= 0.0) && (a >= 0.0));
		Assert((r <= 1.0) && (g <= 1.0) && (b <= 1.0) && (a <= 1.0f));

		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = (FastFToC(b)) | (FastFToC(g) << 8) | (FastFToC(r) << 16) | (FastFToC(a) << 24);
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular4fv(const float* rgb) const
	{
		Assert(rgb);
		Assert(m_pSpecular);
		Assert(IsFinite(rgb[0]) && IsFinite(rgb[1]) && IsFinite(rgb[2]) && IsFinite(rgb[3]));
		Assert((rgb[0] >= 0.0) && (rgb[1] >= 0.0) && (rgb[2] >= 0.0) && (rgb[3] >= 0.0));
		Assert((rgb[0] <= 1.0) && (rgb[1] <= 1.0) && (rgb[2] <= 1.0) && (rgb[3] <= 1.0));

		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = (FastFToC(rgb[2])) | (FastFToC(rgb[1]) << 8) | (FastFToC(rgb[0]) << 16) | (FastFToC(rgb[3]) << 24);
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular3ub(unsigned char r, unsigned char g, unsigned char b) const
	{
		Assert(m_pSpecular);
		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = b | (g << 8) | (r << 16) | 0xFF000000;
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular3ubv(unsigned char const* c) const
	{
		Assert(m_pSpecular);
		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = c[2] | (c[1] << 8) | (c[0] << 16) | 0xFF000000;
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const
	{
		Assert(m_pSpecular);
		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = b | (g << 8) | (r << 16) | (a << 24);
		*(int*)pSpecular = col;
	}

	void CVertexBuilder::Specular4ubv(unsigned char const* c) const
	{
		Assert(m_pSpecular);
		unsigned char* pSpecular = &m_pSpecular[m_nCurrentVertex * m_VertexSize_Specular];
		int col = c[2] | (c[1] << 8) | (c[0] << 16) | (c[3] << 24);
		*(int*)pSpecular = col;
	}


	//-----------------------------------------------------------------------------
	// Texture coordinate setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::TexCoord1f(int nStage, float s) const
	{
		Assert(m_pTexCoord[nStage] && m_pCurrTexCoord[nStage]);
		Assert(IsFinite(s));

		float* pDst = m_pCurrTexCoord[nStage];
		*pDst = s;
	}

	void CVertexBuilder::TexCoord2f(int nStage, float s, float t) const
	{
		Assert(m_pTexCoord[nStage] && m_pCurrTexCoord[nStage]);
		Assert(IsFinite(s) && IsFinite(t));

		float* pDst = m_pCurrTexCoord[nStage];
		*pDst++ = s;
		*pDst = t;
	}

	void CVertexBuilder::TexCoord2fv(int nStage, const float* st) const
	{
		Assert(st);
		Assert(m_pTexCoord[nStage] && m_pCurrTexCoord[nStage]);
		Assert(IsFinite(st[0]) && IsFinite(st[1]));

		float* pDst = m_pCurrTexCoord[nStage];
		*pDst++ = *st++;
		*pDst = *st;
	}

	void CVertexBuilder::TexCoord3f(int stage, float s, float t, float u) const
	{
		// Tried to add too much!
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(s) && IsFinite(t) && IsFinite(u));
		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = s;
		*pDst++ = t;
		*pDst = u;
	}

	void CVertexBuilder::TexCoord3fv(int stage, const float* stu) const
	{
		Assert(stu);
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(stu[0]) && IsFinite(stu[1]) && IsFinite(stu[2]));

		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = *stu++;
		*pDst++ = *stu++;
		*pDst = *stu;
	}

	void CVertexBuilder::TexCoord4f(int stage, float s, float t, float u, float v) const
	{
		// Tried to add too much!
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(s) && IsFinite(t) && IsFinite(u));
		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = s;
		*pDst++ = t;
		*pDst++ = u;
		*pDst = v;
	}

	void CVertexBuilder::TexCoord4fv(int stage, const float* stuv) const
	{
		Assert(stuv);
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(stuv[0]) && IsFinite(stuv[1]) && IsFinite(stuv[2]));

		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = *stuv++;
		*pDst++ = *stuv++;
		*pDst++ = *stuv++;
		*pDst = *stuv;
	}


	void CVertexBuilder::TexCoordSubRect2f(int stage, float s, float t, float offsetS, float offsetT, float scaleS, float scaleT) const
	{
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(s) && IsFinite(t));

		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = (s * scaleS) + offsetS;
		*pDst = (t * scaleT) + offsetT;
	}

	void CVertexBuilder::TexCoordSubRect2fv(int stage, const float* st, const float* offset, const float* scale) const
	{
		Assert(st);
		Assert(m_pTexCoord[stage] && m_pCurrTexCoord[stage]);
		Assert(IsFinite(st[0]) && IsFinite(st[1]));

		float* pDst = m_pCurrTexCoord[stage];
		*pDst++ = (*st++ * *scale++) + *offset++;
		*pDst = (*st * *scale) + *offset;
	}


	//-----------------------------------------------------------------------------
	// Tangent space setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::TangentS3f(float sx, float sy, float sz) const
	{
		Assert(m_pTangentS);
		Assert(IsFinite(sx) && IsFinite(sy) && IsFinite(sz));

		float* pTangentS = OffsetFloatPointer(m_pTangentS, m_nCurrentVertex, m_VertexSize_TangentS);
		*pTangentS++ = sx;
		*pTangentS++ = sy;
		*pTangentS = sz;
	}

	void CVertexBuilder::TangentS3fv(const float* s) const
	{
		Assert(s);
		Assert(m_pTangentS);
		Assert(IsFinite(s[0]) && IsFinite(s[1]) && IsFinite(s[2]));

		float* pTangentS = OffsetFloatPointer(m_pTangentS, m_nCurrentVertex, m_VertexSize_TangentS);
		*pTangentS++ = *s++;
		*pTangentS++ = *s++;
		*pTangentS = *s;
	}

	void CVertexBuilder::TangentT3f(float tx, float ty, float tz) const
	{
		Assert(m_pTangentT);
		Assert(IsFinite(tx) && IsFinite(ty) && IsFinite(tz));

		float* pTangentT = OffsetFloatPointer(m_pTangentT, m_nCurrentVertex, m_VertexSize_TangentT);
		*pTangentT++ = tx;
		*pTangentT++ = ty;
		*pTangentT = tz;
	}

	void CVertexBuilder::TangentT3fv(const float* t) const
	{
		Assert(t);
		Assert(m_pTangentT);
		Assert(IsFinite(t[0]) && IsFinite(t[1]) && IsFinite(t[2]));

		float* pTangentT = OffsetFloatPointer(m_pTangentT, m_nCurrentVertex, m_VertexSize_TangentT);
		*pTangentT++ = *t++;
		*pTangentT++ = *t++;
		*pTangentT = *t;
	}


	//-----------------------------------------------------------------------------
	// Wrinkle setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::Wrinkle1f(float flWrinkle) const
	{
		Assert(m_pWrinkle);
		Assert(IsFinite(flWrinkle));

		float* pWrinkle = OffsetFloatPointer(m_pWrinkle, m_nCurrentVertex, m_VertexSize_Wrinkle);
		*pWrinkle = flWrinkle;
	}


	//-----------------------------------------------------------------------------
	// Bone weight setting methods
	//-----------------------------------------------------------------------------
	void CVertexBuilder::BoneWeight(int idx, float weight) const
	{
		Assert(m_pBoneWeight);
		Assert(IsFinite(weight));
		Assert(idx >= 0);
		Assert(m_NumBoneWeights == 2);

		// This test is here because we store N-1 bone weights (the Nth is computed in
		// the vertex shader as "1 - C", where C is the sum of the (N-1) other weights)
		if (idx < m_NumBoneWeights)
		{
			float* pBoneWeight = OffsetFloatPointer(m_pBoneWeight, m_nCurrentVertex, m_VertexSize_BoneWeight);
			pBoneWeight[idx] = weight;
		}
	}

	void CVertexBuilder::BoneMatrix(int idx, int matrixIdx) const
	{
		Assert(m_pBoneMatrixIndex);
		Assert(idx >= 0);
		Assert(idx < 4);

		// garymcthack
		if (matrixIdx == BONE_MATRIX_INDEX_INVALID)
		{
			matrixIdx = 0;
		}
		Assert((matrixIdx >= 0) && (matrixIdx < 53));

#ifndef NEW_SKINNING
		unsigned char* pBoneMatrix = &m_pBoneMatrixIndex[m_nCurrentVertex * m_VertexSize_BoneMatrixIndex];

		pBoneMatrix[idx] = (unsigned char)matrixIdx;
#else
	float* pBoneMatrix = &m_pBoneMatrixIndex[m_nCurrentVertex * m_VertexSize_BoneMatrixIndex];
	pBoneMatrix[idx] = matrixIdx;
#endif
	}

	//-----------------------------------------------------------------------------
	// Templatized bone weight setting methods which support compressed vertices
	//-----------------------------------------------------------------------------
	template <VertexCompressionType_t T>
	void CVertexBuilder::CompressedBoneWeight3fv(const float* pWeights) const
	{
		Assert(T == m_CompressionType);
		Assert(m_pBoneWeight);
		Assert(pWeights);

		float* pDestWeights = OffsetFloatPointer(m_pBoneWeight, m_nCurrentVertex, m_VertexSize_BoneWeight);

		if (T == VERTEX_COMPRESSION_ON)
		{
			// Quantize to 15 bits per weight (we use D3DDECLTYPE_SHORT2)
			// NOTE: we perform careful normalization (weights sum to 1.0f in the vertex shader), so
			// as to avoid cracking at boundaries between meshes with different numbers of weights
			// per vertex. For example, (1) needs to yield the same normalized weights as (1,0),
			// and (0.5,0.49) needs to normalize the same normalized weights as (0.5,0.49,0).
			// The key is that values which are *computed* in the shader (e.g. the second weight
			// in a 2-weight mesh) must exactly equal values which are *read* from the vertex
			// stream (e.g. the second weight in a 3-weight mesh).

			// Only 1 or 2 weights (SHORT2N) supported for compressed verts so far
			Assert(m_NumBoneWeights <= 2);

			const int WEIGHT0_SHIFT = 0;
			const int WEIGHT1_SHIFT = 16;
			unsigned int* weights = (unsigned int *)pDestWeights;

			// We scale our weights so that they sum to 32768, then subtract 1 (which gets added
			// back in the shader), because dividing by 32767 introduces nasty rounding issues.
			Assert(IsFinite(pWeights[0]) && (pWeights[0] >= 0.0f) && (pWeights[0] <= 1.0f));
			unsigned int weight0 = Float2Int(pWeights[0] * 32768.0f);
			*weights = (0x0000FFFF & (weight0 - 1)) << WEIGHT0_SHIFT;


			if (m_NumBoneWeights > 1)
			{
				// This path for 3 weights per vert (2 are stored and the 3rd is computed
				// in the shader - we do post-quantization normalization here in such a
				// way as to avoid mesh-boundary cracking)
				Assert(m_NumBoneWeights == 2);
				Assert(IsFinite(pWeights[1]) && (pWeights[1] >= 0.0f) && (pWeights[1] <= 1.0f));
				Assert(IsFinite(pWeights[2]) && (pWeights[2] >= 0.0f) && (pWeights[2] <= 1.0f));
				unsigned int weight1 = Float2Int(pWeights[1] * 32768.0f);
				unsigned int weight2 = Float2Int(pWeights[2] * 32768.0f);
				Assert((weight0 + weight1 + weight2) <= 32768);
				unsigned int residual = 32768 - (weight0 + weight1 + weight2);
				weight1 += residual; // Normalize
				*weights |= (0x0000FFFF & (weight1 - 1)) << WEIGHT1_SHIFT;
			}
		}
		else // Uncompressed path
		{
			pDestWeights[0] = pWeights[0];
			pDestWeights[1] = pWeights[1];
		}
	}

	//-----------------------------------------------------------------------------
	// Generic per-vertex data setting method
	//-----------------------------------------------------------------------------
	void CVertexBuilder::UserData(const float* pData) const
	{
		Assert(m_CompressionType == VERTEX_COMPRESSION_NONE); // Use the templatized version if you want to support compression
		Assert(pData);

		int userDataSize = 4; // garymcthack
		float* pUserData = OffsetFloatPointer(m_pUserData, m_nCurrentVertex, m_VertexSize_UserData);
		memcpy(pUserData, pData, sizeof(float) * userDataSize);
	}

	//-----------------------------------------------------------------------------
	// Templatized generic per-vertex data setting method which supports compressed vertices
	//-----------------------------------------------------------------------------
	template <VertexCompressionType_t T>
	void CVertexBuilder::CompressedUserData(const float* pData) const
	{
		Assert(T == m_CompressionType);
		Assert(pData);
		// This is always in fact a tangent vector, not generic 'userdata'
		Assert(IsFinite(pData[0]) && IsFinite(pData[1]) && IsFinite(pData[2]));
		Assert(pData[0] >= -1.05f && pData[0] <= 1.05f);
		Assert(pData[1] >= -1.05f && pData[1] <= 1.05f);
		Assert(pData[2] >= -1.05f && pData[2] <= 1.05f);
		Assert(pData[3] == +1.0f || pData[3] == -1.0f);
		// FIXME: studiorender is passing in non-unit normals
		//float lengthSqd = pData[0]*pData[0] + pData[1]*pData[1] + pData[2]*pData[2];
		//Assert( lengthSqd >= 0.95f && lengthSqd <= 1.05f );

		if (T == SourceEngine::VERTEX_COMPRESSION_ON)
		{
			float binormalSign = pData[3];

#if		( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_SEPARATETANGENTS_SHORT2 )
			float* pUserData = OffsetFloatPointer(m_pUserData, m_nCurrentVertex, m_VertexSize_UserData);
			PackNormal_SHORT2(pData, (unsigned int *)pUserData, binormalSign);
#else //( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 ) 
			// FIXME: add a combined CompressedNormalAndTangent() accessor, to avoid reading back from write-combined memory here
			// The normal should have already been written into the lower 16
			// bits - here, we OR in the tangent into the upper 16 bits
		unsigned int existingNormalData = *(unsigned int *)m_pCurrNormal;
		Assert((existingNormalData & 0xFFFF0000) == 0);
#ifdef _DEBUG
		Assert(m_bWrittenNormal == true);
		m_bWrittenUserData = true;
#endif
		bool bIsTangent = true;
		unsigned int tangentData = 0;
		PackNormal_UBYTE4(pData, &tangentData, bIsTangent, binormalSign);
		*(unsigned int *)m_pCurrNormal = existingNormalData | tangentData;
#endif
		}
		else
		{
			int userDataSize = 4; // garymcthack
			float* pUserData = OffsetFloatPointer(m_pUserData, m_nCurrentVertex, m_VertexSize_UserData);
			memcpy(pUserData, pData, sizeof(float) * userDataSize);
		}
	}

	//-----------------------------------------------------------------------------
	// Forward compat
	//-----------------------------------------------------------------------------
	void CMeshBuilder::Begin(IVertexBuffer* pVertexBuffer, MaterialPrimitiveType_t type, int numPrimitives)
	{
		Assert(0);
		//	Begin( pVertexBuffer->GetMesh(), type, numPrimitives );
	}

	void CMeshBuilder::Begin(IVertexBuffer* pVertexBuffer, IIndexBuffer *pIndexBuffer, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount, int *nFirstVertex)
	{
		Assert(0);
		//	Begin( pVertexBuffer->GetMesh(), type, nVertexCount, nIndexCount, nFirstVertex );
	}

	void CMeshBuilder::Begin(IVertexBuffer* pVertexBuffer, IIndexBuffer *pIndexBuffer, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount)
	{
		Assert(0);
		//	Begin( pVertexBuffer->GetMesh(), type, nVertexCount, nIndexCount );
	}


	//-----------------------------------------------------------------------------
	// Constructor
	//-----------------------------------------------------------------------------
	CMeshBuilder::CMeshBuilder() : m_pMesh(nullptr), m_Type(), m_bGenerateIndices(false)
	{
	}


	//-----------------------------------------------------------------------------
	// Computes the number of verts and indices based on primitive type and count
	//-----------------------------------------------------------------------------
	void CMeshBuilder::ComputeNumVertsAndIndices(int *pMaxVertices, int *pMaxIndices,
		MaterialPrimitiveType_t type, int nPrimitiveCount)
	{
		switch (type)
		{
		case MATERIAL_POINTS:
			*pMaxVertices = *pMaxIndices = nPrimitiveCount;
			break;

		case MATERIAL_LINES:
			*pMaxVertices = *pMaxIndices = nPrimitiveCount * 2;
			break;

		case MATERIAL_LINE_STRIP:
			*pMaxVertices = nPrimitiveCount + 1;
			*pMaxIndices = nPrimitiveCount * 2;
			break;

		case MATERIAL_LINE_LOOP:
			*pMaxVertices = nPrimitiveCount;
			*pMaxIndices = nPrimitiveCount * 2;
			break;

		case MATERIAL_TRIANGLES:
			*pMaxVertices = *pMaxIndices = nPrimitiveCount * 3;
			break;

		case MATERIAL_TRIANGLE_STRIP:
			*pMaxVertices = *pMaxIndices = nPrimitiveCount + 2;
			break;

		case MATERIAL_QUADS:
			*pMaxVertices = nPrimitiveCount * 4;
			*pMaxIndices = nPrimitiveCount * 6;
			break;

		case MATERIAL_INSTANCED_QUADS:
			*pMaxVertices = nPrimitiveCount;
			*pMaxIndices = 0; // This primtype is unindexed
			break;

		case MATERIAL_POLYGON:
			*pMaxVertices = nPrimitiveCount;
			*pMaxIndices = (nPrimitiveCount - 2) * 3;
			break;

		default:
			Assert(0);
		}

		// FIXME: need to get this from meshdx8.cpp, or move it to somewhere common
		Assert(*pMaxVertices <= 32768);
		Assert(*pMaxIndices <= 32768);
	}


	int CMeshBuilder::IndicesFromVertices(MaterialPrimitiveType_t type, int nVertexCount)
	{
		switch (type)
		{
		case MATERIAL_QUADS:
			Assert((nVertexCount & 0x3) == 0);
			return (nVertexCount * 6) / 4;

		case MATERIAL_INSTANCED_QUADS:
			// This primtype is unindexed
			return 0;

		case MATERIAL_POLYGON:
			Assert(nVertexCount >= 3);
			return (nVertexCount - 2) * 3;

		case MATERIAL_LINE_STRIP:
			Assert(nVertexCount >= 2);
			return (nVertexCount - 1) * 2;

		case MATERIAL_LINE_LOOP:
			Assert(nVertexCount >= 3);
			return nVertexCount * 2;

		default:
			return nVertexCount;
		}
	}

	//-----------------------------------------------------------------------------
	// Specify the type of vertex compression that this CMeshBuilder will perform
	//-----------------------------------------------------------------------------
	void CMeshBuilder::SetCompressionType(VertexCompressionType_t vertexCompressionType)
	{
		m_VertexBuilder.SetCompressionType(vertexCompressionType);
	}

	//-----------------------------------------------------------------------------
	// Begins modifying the mesh
	//-----------------------------------------------------------------------------
	void CMeshBuilder::Begin(IMesh *pMesh, MaterialPrimitiveType_t type, int numPrimitives)
	{
		Assert(pMesh && (!m_pMesh));
		Assert(type != MATERIAL_HETEROGENOUS);

		m_pMesh = pMesh;
		m_bGenerateIndices = true;
		m_Type = type;

		int nMaxVertexCount, nMaxIndexCount;
		ComputeNumVertsAndIndices(&nMaxVertexCount, &nMaxIndexCount, type, numPrimitives);

		switch (type)
		{
		case MATERIAL_INSTANCED_QUADS:
			m_pMesh->SetPrimitiveType(MATERIAL_INSTANCED_QUADS);
			break;

		case MATERIAL_QUADS:
		case MATERIAL_POLYGON:
			m_pMesh->SetPrimitiveType(MATERIAL_TRIANGLES);
			break;

		case MATERIAL_LINE_STRIP:
		case MATERIAL_LINE_LOOP:
			m_pMesh->SetPrimitiveType(MATERIAL_LINES);
			break;

		default:
			m_pMesh->SetPrimitiveType(type);
		}

		// Lock the mesh
		m_pMesh->LockMesh(nMaxVertexCount, nMaxIndexCount, *this);

		m_IndexBuilder.AttachBegin(pMesh, nMaxIndexCount, *this);
		m_VertexBuilder.AttachBegin(pMesh, nMaxVertexCount, *this);

		// Point to the start of the index and vertex buffers
		Reset();
	}

	void CMeshBuilder::Begin(IMesh *pMesh, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount, int *nFirstVertex)
	{
		Begin(pMesh, type, nVertexCount, nIndexCount);

		*nFirstVertex = m_VertexBuilder.m_nFirstVertex * m_VertexBuilder.VertexSize();
	}

	void CMeshBuilder::Begin(IMesh* pMesh, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount)
	{
		Assert(pMesh && (!m_pMesh));

		// NOTE: We can't specify the indices when we use quads, polygons, or
		// linestrips; they aren't actually directly supported by 
		// the material system
		Assert((type != MATERIAL_QUADS) && (type != MATERIAL_INSTANCED_QUADS) && (type != MATERIAL_POLYGON) &&
			(type != MATERIAL_LINE_STRIP) && (type != MATERIAL_LINE_LOOP));

		// Dx8 doesn't support indexed points...
		Assert(type != MATERIAL_POINTS);

		m_pMesh = pMesh;
		m_bGenerateIndices = false;
		m_Type = type;

		// Set the primitive type
		m_pMesh->SetPrimitiveType(type);

		// Lock the vertex and index buffer
		m_pMesh->LockMesh(nVertexCount, nIndexCount, *this);

		m_IndexBuilder.AttachBegin(pMesh, nIndexCount, *this);
		m_VertexBuilder.AttachBegin(pMesh, nVertexCount, *this);

		// Point to the start of the buffers..
		Reset();
	}


	//-----------------------------------------------------------------------------
	// Use this when you're done modifying the mesh
	//-----------------------------------------------------------------------------
	void CMeshBuilder::End(bool bSpewData, bool bDraw)
	{
		if (m_bGenerateIndices)
		{
			int nIndexCount = IndicesFromVertices(m_Type, m_VertexBuilder.VertexCount());
			m_IndexBuilder.GenerateIndices(m_Type, nIndexCount);
		}

		if (bSpewData)
		{
			m_pMesh->Spew(m_VertexBuilder.VertexCount(), m_IndexBuilder.IndexCount(), *this);
		}

#ifdef _DEBUG
		m_pMesh->ValidateData(m_VertexBuilder.VertexCount(), m_IndexBuilder.IndexCount(), *this);
#endif

		// Unlock our buffers
		m_pMesh->UnlockMesh(m_VertexBuilder.VertexCount(), m_IndexBuilder.IndexCount(), *this);

		m_IndexBuilder.AttachEnd();
		m_VertexBuilder.AttachEnd();

		if (bDraw)
		{
			m_pMesh->Draw();
		}

		m_pMesh = nullptr;

#ifdef _DEBUG
		memset((MeshDesc_t*)this, 0, sizeof(MeshDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// Locks the vertex buffer to modify existing data
	//-----------------------------------------------------------------------------
	void CMeshBuilder::BeginModify(IMesh* pMesh, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount)
	{
		Assert(pMesh && (!m_pMesh));

		if (nVertexCount < 0)
		{
			nVertexCount = pMesh->VertexCount();
		}

		m_pMesh = pMesh;
		m_bGenerateIndices = false;

		// Locks mesh for modifying
		pMesh->ModifyBeginEx(false, nFirstVertex, nVertexCount, nFirstIndex, nIndexCount, *this);

		m_IndexBuilder.AttachBeginModify(pMesh, nFirstIndex, nIndexCount, *this);
		m_VertexBuilder.AttachBeginModify(pMesh, nFirstVertex, nVertexCount, *this);

		// Point to the start of the buffers..
		Reset();
	}

	void CMeshBuilder::EndModify(bool bSpewData)
	{
		Assert(m_pMesh);

		if (bSpewData)
		{
			m_pMesh->Spew(m_VertexBuilder.VertexCount(), m_IndexBuilder.IndexCount(), *this);
		}
#ifdef _DEBUG
		m_pMesh->ValidateData(m_VertexBuilder.VertexCount(), m_IndexBuilder.IndexCount(), *this);
#endif

		// Unlocks mesh
		m_pMesh->ModifyEnd(*this);
		m_pMesh = nullptr;

		m_IndexBuilder.AttachEndModify();
		m_VertexBuilder.AttachEndModify();

#ifdef _DEBUG
		// Null out our pointers...
		memset((MeshDesc_t*)this, 0, sizeof(MeshDesc_t));
#endif
	}


	//-----------------------------------------------------------------------------
	// Resets the mesh builder so it points to the start of everything again
	//-----------------------------------------------------------------------------
	void CMeshBuilder::Reset()
	{
		m_IndexBuilder.Reset();
		m_VertexBuilder.Reset();
	}


	//-----------------------------------------------------------------------------
	// Selects the current Vertex and Index 
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::SelectVertex(int nIndex)
	{
		m_VertexBuilder.SelectVertex(nIndex);
	}

	void CMeshBuilder::SelectVertexFromIndex(int idx)
	{
		// NOTE: This index is expected to be relative 
		int vertIdx = idx - m_nFirstVertex;
		SelectVertex(vertIdx);
	}

	 void CMeshBuilder::SelectIndex(int idx) const
	 {
		m_IndexBuilder.SelectIndex(idx);
	}


	//-----------------------------------------------------------------------------
	// Advances the current vertex and index by one
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::AdvanceVertex()
	{
		m_VertexBuilder.AdvanceVertex();
	}

	 void CMeshBuilder::AdvanceVertices(int nVertexCount)
	{
		m_VertexBuilder.AdvanceVertices(nVertexCount);
	}

	 void CMeshBuilder::AdvanceIndex()
	{
		m_IndexBuilder.AdvanceIndex();
	}

	 void CMeshBuilder::AdvanceIndices(int nIndices)
	{
		m_IndexBuilder.AdvanceIndices(nIndices);
	}

	 int CMeshBuilder::GetCurrentVertex() const
	{
		return m_VertexBuilder.GetCurrentVertex();
	}

	 int CMeshBuilder::GetCurrentIndex() const
	 {
		return m_IndexBuilder.GetCurrentIndex();
	}


	//-----------------------------------------------------------------------------
	// A helper method since this seems to be done a whole bunch.
	//-----------------------------------------------------------------------------
	void CMeshBuilder::DrawQuad(IMesh* pMesh, const float* v1, const float* v2,
		const float* v3, const float* v4, unsigned char const* pColor, bool wireframe)
	{
		if (!wireframe)
		{
			Begin(pMesh, MATERIAL_TRIANGLE_STRIP, 2);

			Position3fv(v1);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v2);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v4);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v3);
			Color4ubv(pColor);
			AdvanceVertex();
		}
		else
		{
			Begin(pMesh, MATERIAL_LINE_LOOP, 4);
			Position3fv(v1);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v2);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v3);
			Color4ubv(pColor);
			AdvanceVertex();

			Position3fv(v4);
			Color4ubv(pColor);
			AdvanceVertex();
		}

		End();
		pMesh->Draw();
	}


	//-----------------------------------------------------------------------------
	// returns the number of indices and vertices
	//-----------------------------------------------------------------------------
	 int CMeshBuilder::VertexCount() const
	{
		return m_VertexBuilder.VertexCount();
	}

	 int CMeshBuilder::IndexCount() const
	{
		return m_IndexBuilder.IndexCount();
	}


	//-----------------------------------------------------------------------------
	// Returns the base vertex memory pointer
	//-----------------------------------------------------------------------------
	 void* CMeshBuilder::BaseVertexData() const
	 {
		return m_VertexBuilder.BaseVertexData();
	}

	//-----------------------------------------------------------------------------
	// Data retrieval...
	//-----------------------------------------------------------------------------
	 const float* CMeshBuilder::Position() const
	{
		return m_VertexBuilder.Position();
	}

	 const float* CMeshBuilder::Normal()	const
	{
		return m_VertexBuilder.Normal();
	}

	 unsigned int CMeshBuilder::Color() const
	{
		return m_VertexBuilder.Color();
	}

	 unsigned char *CMeshBuilder::Specular() const
	{
		return m_VertexBuilder.Specular();
	}

	 const float* CMeshBuilder::TexCoord(int nStage) const
	{
		return m_VertexBuilder.TexCoord(nStage);
	}

	 const float* CMeshBuilder::TangentS() const
	{
		return m_VertexBuilder.TangentS();
	}

	 const float* CMeshBuilder::TangentT() const
	{
		return m_VertexBuilder.TangentT();
	}

	 float CMeshBuilder::Wrinkle() const
	{
		return m_VertexBuilder.Wrinkle();
	}

	 const float* CMeshBuilder::BoneWeight() const
	{
		return m_VertexBuilder.BoneWeight();
	}

	 int CMeshBuilder::NumBoneWeights() const
	{
		return m_VertexBuilder.NumBoneWeights();
	}

	 unsigned short const* CMeshBuilder::Index() const
	{
		return m_IndexBuilder.Index();
	}

	//-----------------------------------------------------------------------------
	// Index
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::Index(unsigned short idx) const
	 {
		m_IndexBuilder.Index(idx);
	}


	//-----------------------------------------------------------------------------
	// Fast Index! No need to call advance index
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::FastIndex(unsigned short idx)
	{
		m_IndexBuilder.FastIndex(idx);
	}

	// NOTE: Use this one to get write combining! Much faster than the other version of FastIndex
	// Fast Index! No need to call advance index, and no random access allowed
	 void CMeshBuilder::FastIndex2(unsigned short nIndex1, unsigned short nIndex2)
	{
		m_IndexBuilder.FastIndex2(nIndex1, nIndex2);
	}

	//-----------------------------------------------------------------------------
	// For use with the FastVertex methods, advances the current vertex by N
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::FastAdvanceNVertices(int nVertexCount)
	{
		m_VertexBuilder.FastAdvanceNVertices(nVertexCount);
	}

	//-----------------------------------------------------------------------------
	// Vertex field setting methods
	//-----------------------------------------------------------------------------
	 void CMeshBuilder::Position3f(float x, float y, float z) const
	 {
		m_VertexBuilder.Position3f(x, y, z);
	}

	 void CMeshBuilder::Position3fv(const float *v) const
	 {
		m_VertexBuilder.Position3fv(v);
	}

	 void CMeshBuilder::Normal3f(float nx, float ny, float nz) const
	 {
		m_VertexBuilder.Normal3f(nx, ny, nz);
	}

	 void CMeshBuilder::Normal3fv(const float *n) const
	 {
		m_VertexBuilder.Normal3fv(n);
	}

	 void CMeshBuilder::NormalDelta3f(float nx, float ny, float nz) const
	 {
		m_VertexBuilder.NormalDelta3f(nx, ny, nz);
	}

	 void CMeshBuilder::NormalDelta3fv(const float *n) const
	 {
		m_VertexBuilder.NormalDelta3fv(n);
	}

	 void CMeshBuilder::Color3f(float r, float g, float b) const
	 {
		m_VertexBuilder.Color3f(r, g, b);
	}

	 void CMeshBuilder::Color3fv(const float *rgb) const
	 {
		m_VertexBuilder.Color3fv(rgb);
	}

	 void CMeshBuilder::Color4f(float r, float g, float b, float a) const
	 {
		m_VertexBuilder.Color4f(r, g, b, a);
	}

	 void CMeshBuilder::Color4fv(const float *rgba) const
	 {
		m_VertexBuilder.Color4fv(rgba);
	}

	 void CMeshBuilder::Color3ub(unsigned char r, unsigned char g, unsigned char b) const
	 {
		m_VertexBuilder.Color3ub(r, g, b);
	}

	 void CMeshBuilder::Color3ubv(unsigned char const* rgb) const
	 {
		m_VertexBuilder.Color3ubv(rgb);
	}

	 void CMeshBuilder::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const
	 {
		m_VertexBuilder.Color4ub(r, g, b, a);
	}

	 void CMeshBuilder::Color4ubv(unsigned char const* rgba) const
	 {
		m_VertexBuilder.Color4ubv(rgba);
	}

	 void CMeshBuilder::Specular3f(float r, float g, float b) const
	 {
		m_VertexBuilder.Specular3f(r, g, b);
	}

	 void CMeshBuilder::Specular3fv(const float *rgb) const
	 {
		m_VertexBuilder.Specular3fv(rgb);
	}

	 void CMeshBuilder::Specular4f(float r, float g, float b, float a) const
	 {
		m_VertexBuilder.Specular4f(r, g, b, a);
	}

	 void CMeshBuilder::Specular4fv(const float *rgba) const
	 {
		m_VertexBuilder.Specular4fv(rgba);
	}

	 void CMeshBuilder::Specular3ub(unsigned char r, unsigned char g, unsigned char b) const
	 {
		m_VertexBuilder.Specular3ub(r, g, b);
	}

	 void CMeshBuilder::Specular3ubv(unsigned char const *c) const
	 {
		m_VertexBuilder.Specular3ubv(c);
	}

	 void CMeshBuilder::Specular4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const
	 {
		m_VertexBuilder.Specular4ub(r, g, b, a);
	}

	 void CMeshBuilder::Specular4ubv(unsigned char const *c) const
	 {
		m_VertexBuilder.Specular4ubv(c);
	}

	 void CMeshBuilder::TexCoord1f(int nStage, float s) const
	 {
		m_VertexBuilder.TexCoord1f(nStage, s);
	}

	 void CMeshBuilder::TexCoord2f(int nStage, float s, float t) const
	 {
		m_VertexBuilder.TexCoord2f(nStage, s, t);
	}

	 void CMeshBuilder::TexCoord2fv(int nStage, const float *st) const
	 {
		m_VertexBuilder.TexCoord2fv(nStage, st);
	}

	 void CMeshBuilder::TexCoord3f(int nStage, float s, float t, float u) const
	 {
		m_VertexBuilder.TexCoord3f(nStage, s, t, u);
	}

	 void CMeshBuilder::TexCoord3fv(int nStage, const float *stu) const
	 {
		m_VertexBuilder.TexCoord3fv(nStage, stu);
	}

	 void CMeshBuilder::TexCoord4f(int nStage, float s, float t, float u, float v) const
	 {
		m_VertexBuilder.TexCoord4f(nStage, s, t, u, v);
	}

	 void CMeshBuilder::TexCoord4fv(int nStage, const float *stuv) const
	 {
		m_VertexBuilder.TexCoord4fv(nStage, stuv);
	}

	 void CMeshBuilder::TexCoordSubRect2f(int nStage, float s, float t, float offsetS, float offsetT, float scaleS, float scaleT) const
	 {
		m_VertexBuilder.TexCoordSubRect2f(nStage, s, t, offsetS, offsetT, scaleS, scaleT);
	}

	 void CMeshBuilder::TexCoordSubRect2fv(int nStage, const float *st, const float *offset, const float *scale) const
	 {
		m_VertexBuilder.TexCoordSubRect2fv(nStage, st, offset, scale);
	}

	 void CMeshBuilder::TangentS3f(float sx, float sy, float sz) const
	 {
		m_VertexBuilder.TangentS3f(sx, sy, sz);
	}

	 void CMeshBuilder::TangentS3fv(const float* s) const
	 {
		m_VertexBuilder.TangentS3fv(s);
	}

	 void CMeshBuilder::TangentT3f(float tx, float ty, float tz) const
	 {
		m_VertexBuilder.TangentT3f(tx, ty, tz);
	}

	 void CMeshBuilder::TangentT3fv(const float* t) const
	 {
		m_VertexBuilder.TangentT3fv(t);
	}

	 void CMeshBuilder::Wrinkle1f(float flWrinkle) const
	 {
		m_VertexBuilder.Wrinkle1f(flWrinkle);
	}

	 void CMeshBuilder::BoneWeight(int nIndex, float flWeight) const
	 {
		m_VertexBuilder.BoneWeight(nIndex, flWeight);
	}

	template <VertexCompressionType_t T>  void CMeshBuilder::CompressedBoneWeight3fv(const float * pWeights) const
	{
		m_VertexBuilder.CompressedBoneWeight3fv<T>(pWeights);
	}

	 void CMeshBuilder::BoneMatrix(int nIndex, int nMatrixIdx) const
	 {
		m_VertexBuilder.BoneMatrix(nIndex, nMatrixIdx);
	}

	 void CMeshBuilder::UserData(const float* pData) const
	 {
		m_VertexBuilder.UserData(pData);
	}

	template <VertexCompressionType_t T>  void CMeshBuilder::CompressedUserData(const float* pData) const
	{
		m_VertexBuilder.CompressedUserData<T>(pData);
	}

	//-----------------------------------------------------------------------------
	// Templatized vertex field setting methods which support compression
	//-----------------------------------------------------------------------------

	template <VertexCompressionType_t T>  void CMeshBuilder::CompressedNormal3f(float nx, float ny, float nz) const
	{
		m_VertexBuilder.CompressedNormal3f<T>(nx, ny, nz);
	}

	template <VertexCompressionType_t T>  void CMeshBuilder::CompressedNormal3fv(const float *n) const
	{
		m_VertexBuilder.CompressedNormal3fv<T>(n);
	}
}
