#pragma once
#include "IMaterial.h"
#include "SourceEngine/Definitions.hpp"
#include "SourceEngine/Vector4D.hpp"
#include "KeyValues.h"
#include "CMaterialReference.h"
#include "SourceEngine/UtlVector.hpp"
#include "CUtlRBTree.hpp"
#include "SourceEngine/Convar.hpp"

#define DECLARE_POINTER_HANDLE1(name) struct name##__ { int unused; }; typedef struct name##__ *name
namespace SourceEngine
{
	class Vector2D;
	class matrix3x4_t;
	struct FlashlightState_t;
	class VMatrix;
	struct LightDesc_t;
	struct MaterialLightingState_t;
	class ITexture;
	class CMeshBuilder;
	class IMatRenderContext;
#define RetAddRef( p ) ( (p)->AddRef(), (p) )
#define InlineAddRef( p ) ( (p)->AddRef(), (p) )

	DECLARE_POINTER_HANDLE1(OcclusionQueryObjectHandle_t);
	#define INVALID_OCCLUSION_QUERY_OBJECT_HANDLE ( (OcclusionQueryObjectHandle_t)0 )
	typedef unsigned int ColorCorrectionHandle_t;

	enum StencilOperation_t
	{
		STENCILOPERATION_KEEP = 1,
		STENCILOPERATION_ZERO = 2,
		STENCILOPERATION_REPLACE = 3,
		STENCILOPERATION_INCRSAT = 4,
		STENCILOPERATION_DECRSAT = 5,
		STENCILOPERATION_INVERT = 6,
		STENCILOPERATION_INCR = 7,
		STENCILOPERATION_DECR = 8,

		STENCILOPERATION_FORCE_DWORD = 0x7fffffff
	};

	enum StencilComparisonFunction_t
	{
		STENCILCOMPARISONFUNCTION_NEVER = 1,
		STENCILCOMPARISONFUNCTION_LESS = 2,
		STENCILCOMPARISONFUNCTION_EQUAL = 3,
		STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
		STENCILCOMPARISONFUNCTION_GREATER = 5,
		STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
		STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
		STENCILCOMPARISONFUNCTION_ALWAYS = 8,
		STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
	};

	enum MaterialMatrixMode_t
	{
		MATERIAL_VIEW = 0,
		MATERIAL_PROJECTION,

		// Texture matrices
		MATERIAL_TEXTURE0,
		MATERIAL_TEXTURE1,
		MATERIAL_TEXTURE2,
		MATERIAL_TEXTURE3,
		MATERIAL_TEXTURE4,
		MATERIAL_TEXTURE5,
		MATERIAL_TEXTURE6,
		MATERIAL_TEXTURE7,

		MATERIAL_MODEL,

		// Total number of matrices
		NUM_MATRIX_MODES = MATERIAL_MODEL + 1,

		// Number of texture transforms
		NUM_TEXTURE_TRANSFORMS = MATERIAL_TEXTURE7 - MATERIAL_TEXTURE0 + 1
	};
	enum
	{
		VERTEX_MAX_TEXTURE_COORDINATES = 8,
		BONE_MATRIX_INDEX_INVALID = 255
	};
	enum
	{
		INDEX_BUFFER_SIZE = 32768,
		DYNAMIC_VERTEX_BUFFER_MEMORY = (1024 + 512) * 1024,
		DYNAMIC_VERTEX_BUFFER_MEMORY_SMALL = 384 * 1024, // Only allocate this much during map transitions
	};

	enum MaterialPrimitiveType_t
	{
		MATERIAL_POINTS = 0x0,
		MATERIAL_LINES,
		MATERIAL_TRIANGLES,
		MATERIAL_TRIANGLE_STRIP,
		MATERIAL_LINE_STRIP,
		MATERIAL_LINE_LOOP,	// a single line loop
		MATERIAL_POLYGON,	// this is a *single* polygon
		MATERIAL_QUADS,
		MATERIAL_INSTANCED_QUADS, // (X360) like MATERIAL_QUADS, but uses vertex instancing

								  // This is used for static meshes that contain multiple types of
								  // primitive types.	When calling draw, you'll need to specify
								  // a primitive type.
		MATERIAL_HETEROGENOUS
	};
	
	// acceptable property values for MATERIAL_PROPERTY_OPACITY
	enum MaterialPropertyOpacityTypes_t
	{
		MATERIAL_ALPHATEST = 0,
		MATERIAL_OPAQUE,
		MATERIAL_TRANSLUCENT
	};

	enum MaterialBufferTypes_t
	{
		MATERIAL_FRONT = 0,
		MATERIAL_BACK
	};

	enum MaterialCullMode_t
	{
		MATERIAL_CULLMODE_CCW,	// this culls polygons with counterclockwise winding
		MATERIAL_CULLMODE_CW	// this culls polygons with clockwise winding
	};

	enum MaterialHeightClipMode_t
	{
		MATERIAL_HEIGHTCLIPMODE_DISABLE,
		MATERIAL_HEIGHTCLIPMODE_RENDER_ABOVE_HEIGHT,
		MATERIAL_HEIGHTCLIPMODE_RENDER_BELOW_HEIGHT
	};

	enum MaterialIndexFormat_t
	{
		MATERIAL_INDEX_FORMAT_UNKNOWN = -1,
		MATERIAL_INDEX_FORMAT_16BIT = 0,
		MATERIAL_INDEX_FORMAT_32BIT,
	};

	enum MaterialFogMode_t
	{
		MATERIAL_FOG_NONE,
		MATERIAL_FOG_LINEAR,
		MATERIAL_FOG_LINEAR_BELOW_FOG_Z,
	};

	enum MaterialNonInteractiveMode_t
	{
		MATERIAL_NON_INTERACTIVE_MODE_NONE = -1,
		MATERIAL_NON_INTERACTIVE_MODE_STARTUP = 0,
		MATERIAL_NON_INTERACTIVE_MODE_LEVEL_LOAD,

		MATERIAL_NON_INTERACTIVE_MODE_COUNT,
	};

	class CPrimList
	{
	public:
		CPrimList();
		CPrimList(int nFirstIndex, int nIndexCount);

		int			m_FirstIndex;
		int			m_NumIndices;
	};

	inline CPrimList::CPrimList(): m_FirstIndex(0), m_NumIndices(0)
	{
	}

	inline CPrimList::CPrimList(int nFirstIndex, int nIndexCount)
	{
		m_FirstIndex = nFirstIndex;
		m_NumIndices = nIndexCount;
	}

	enum VertexCompressionType_t : uint64_t
	{
		// This indicates an uninitialized VertexCompressionType_t value
		VERTEX_COMPRESSION_INVALID = 0xFFFFFFFF,

		// 'VERTEX_COMPRESSION_NONE' means that no elements of a vertex are compressed
		VERTEX_COMPRESSION_NONE = 0,

		// Currently (more stuff may be added as needed), 'VERTEX_COMPRESSION_ON' means:
		//  - if a vertex contains VERTEX_ELEMENT_NORMAL, this is compressed
		//    (see CVertexBuilder::CompressedNormal3f)
		//  - if a vertex contains VERTEX_ELEMENT_USERDATA4 (and a normal - together defining a tangent
		//    frame, with the binormal reconstructed in the vertex shader), this is compressed
		//    (see CVertexBuilder::CompressedUserData)
		//  - if a vertex contains VERTEX_ELEMENT_BONEWEIGHTSx, this is compressed
		//    (see CVertexBuilder::CompressedBoneWeight3fv)
		VERTEX_COMPRESSION_ON = 1
	};


	struct VertexDesc_t
	{
		// These can be set to zero if there are pointers to dummy buffers, when the
		// actual buffer format doesn't contain the data but it needs to be safe to
		// use all the CMeshBuilder functions.
		int	m_VertexSize_Position;
		int m_VertexSize_BoneWeight;
		int m_VertexSize_BoneMatrixIndex;
		int	m_VertexSize_Normal;
		int	m_VertexSize_Color;
		int	m_VertexSize_Specular;
		int m_VertexSize_TexCoord[8];
		int m_VertexSize_TangentS;
		int m_VertexSize_TangentT;
		int m_VertexSize_Wrinkle;

		int m_VertexSize_UserData;

		int m_ActualVertexSize;	// Size of the vertices.. Some of the m_VertexSize_ elements above
								// are set to this value and some are set to zero depending on which
								// fields exist in a buffer's vertex format.

								// The type of compression applied to this vertex data
		VertexCompressionType_t m_CompressionType;

		// Number of bone weights per vertex...
		int m_NumBoneWeights;

		// Pointers to our current vertex data
		float			*m_pPosition;

		float			*m_pBoneWeight;

#ifndef NEW_SKINNING
		unsigned char	*m_pBoneMatrixIndex;
#else
		float			*m_pBoneMatrixIndex;
#endif

		float			*m_pNormal;

		unsigned char	*m_pColor;
		unsigned char	*m_pSpecular;
		float			*m_pTexCoord[8];

		// Tangent space *associated with one particular set of texcoords*
		float			*m_pTangentS;
		float			*m_pTangentT;

		float			*m_pWrinkle;

		// user data
		float			*m_pUserData;

		// The first vertex index (used for buffered vertex buffers, or cards that don't support stream offset)
		int	m_nFirstVertex;

		// The offset in bytes of the memory we're writing into 
		// from the start of the D3D buffer (will be 0 for static meshes)
		unsigned int	m_nOffset;

#ifdef DEBUG_WRITE_COMBINE
		int m_nLastWrittenField;
		unsigned char* m_pLastWrittenAddress;
#endif
	};

	class IVertexBuffer
	{
	public:
		// NOTE: The following two methods are only valid for static vertex buffers
		// Returns the number of vertices and the format of the vertex buffer
		virtual int VertexCount() const = 0;
		virtual VertexFormat_t GetVertexFormat() const = 0;

		// Is this vertex buffer dynamic?
		virtual bool IsDynamic() const = 0;

		// NOTE: For dynamic vertex buffers only!
		// Casts the memory of the dynamic vertex buffer to the appropriate type
		virtual void BeginCastBuffer(VertexFormat_t format) = 0;
		virtual void EndCastBuffer() = 0;

		// Returns the number of vertices that can still be written into the buffer
		virtual int GetRoomRemaining() const = 0;

		virtual bool Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc) = 0;
		virtual void Unlock(int nVertexCount, VertexDesc_t &desc) = 0;

		// Spews the mesh data
		virtual void Spew(int nVertexCount, const VertexDesc_t &desc) = 0;

		// Call this in debug mode to make sure our data is good.
		virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc) = 0;
	};

	struct IndexDesc_t
	{
		// Pointers to the index data
		unsigned short	*m_pIndices;

		// The offset in bytes of the memory we're writing into 
		// from the start of the D3D buffer (will be 0 for static meshes)
		unsigned int	m_nOffset;

		// The first index (used for buffered index buffers, or cards that don't support stream offset)
		unsigned int	m_nFirstIndex;

		// 1 if the device is active, 0 if the device isn't active.
		// Faster than doing if checks for null m_pIndices if someone is
		// trying to write the m_pIndices while the device is inactive.
		unsigned char m_nIndexSize;
	};


	class IIndexBuffer
	{
	public:
		// NOTE: The following two methods are only valid for static index buffers
		// Returns the number of indices and the format of the index buffer
		virtual int IndexCount() const = 0;
		virtual MaterialIndexFormat_t IndexFormat() const = 0;

		// Is this index buffer dynamic?
		virtual bool IsDynamic() const = 0;

		// NOTE: For dynamic index buffers only!
		// Casts the memory of the dynamic index buffer to the appropriate type
		virtual void BeginCastBuffer(MaterialIndexFormat_t format) = 0;
		virtual void EndCastBuffer() = 0;

		// Returns the number of indices that can still be written into the buffer
		virtual int GetRoomRemaining() const = 0;

		// Locks, unlocks the index buffer
		virtual bool Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc) = 0;
		virtual void Unlock(int nWrittenIndexCount, IndexDesc_t &desc) = 0;

		// FIXME: Remove this!!
		// Locks, unlocks the index buffer for modify
		virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc) = 0;
		virtual void ModifyEnd(IndexDesc_t& desc) = 0;

		// Spews the mesh data
		virtual void Spew(int nIndexCount, const IndexDesc_t &desc) = 0;

		// Ensures the data in the index buffer is valid
		virtual void ValidateData(int nIndexCount, const IndexDesc_t &desc) = 0;
	};

	struct MeshDesc_t : VertexDesc_t, IndexDesc_t
	{
	};

	//-----------------------------------------------------------------------------
	// Interface to the mesh - needs to contain an IVertexBuffer and an IIndexBuffer to emulate old mesh behavior
	//-----------------------------------------------------------------------------
	class IMesh : public IVertexBuffer, public IIndexBuffer
	{
	public:
		// -----------------------------------

		// Sets/gets the primitive type
		virtual void SetPrimitiveType(MaterialPrimitiveType_t type) = 0;

		// Draws the mesh
		virtual void Draw(int nFirstIndex = -1, int nIndexCount = 0) = 0;

		virtual void SetColorMesh(IMesh *pColorMesh, int nVertexOffset) = 0;

		// Draw a list of (lists of) primitives. Batching your lists together that use
		// the same lightmap, material, vertex and index buffers with multipass shaders
		// can drastically reduce state-switching overhead.
		// NOTE: this only works with STATIC meshes.
		virtual void Draw(CPrimList *pLists, int nLists) = 0;

		// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
		virtual void CopyToMeshBuilder(
			int iStartVert,		// Which vertices to copy.
			int nVerts,
			int iStartIndex,	// Which indices to copy.
			int nIndices,
			int indexOffset,	// This is added to each index.
			CMeshBuilder &builder) = 0;

		// Spews the mesh data
		virtual void Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc) = 0;

		// Call this in debug mode to make sure our data is good.
		virtual void ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc) = 0;

		// New version
		// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
		// nIndexCount of -1 means don't lock the index buffer...
		virtual void LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc, void* pad = nullptr) = 0;
		virtual void ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc) = 0;
		virtual void ModifyEnd(MeshDesc_t& desc) = 0;
		virtual void UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc) = 0;

		virtual void ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc) = 0;

		virtual void SetFlexMesh(IMesh *pMesh, int nVertexOffset) = 0;

		virtual void DisableFlexMesh() = 0;

		virtual void MarkAsDrawn() = 0;

	};

	class CIndexBuilder : IndexDesc_t
	{
	public:
		CIndexBuilder();
		CIndexBuilder(IIndexBuffer *pIndexBuffer, MaterialIndexFormat_t fmt = MATERIAL_INDEX_FORMAT_UNKNOWN);
		~CIndexBuilder(){}

		// Begins, ends modification of the index buffer (returns true if the lock succeeded)
		// A lock may not succeed if append is set to true and there isn't enough room
		// NOTE: Append is only used with dynamic index buffers; it's ignored for static buffers
		bool Lock(int nMaxIndexCount, int nIndexOffset, bool bAppend = false);
		void Unlock();

		// Spews the current data
		// NOTE: Can only be called during a lock/unlock block
		void SpewData() const;

		// Returns the number of indices we can fit into the buffer without needing to discard
		int GetRoomRemaining() const;

		// Binds this index buffer
		void Bind(IMatRenderContext *pContext) const;

		// Returns the byte offset
		int Offset() const;

		// Begins, ends modification of the index buffer
		// NOTE: IndexOffset is the number to add to all indices written into the buffer;
		// useful when using dynamic vertex buffers.
		void Begin(IIndexBuffer *pIndexBuffer, int nMaxIndexCount, int nIndexOffset = 0);
		void End(bool bSpewData = false);

		// Locks the index buffer to modify existing data
		// Passing nVertexCount == -1 says to lock all the vertices for modification.
		// Pass 0 for nIndexCount to not lock the index buffer.
		void BeginModify(IIndexBuffer *pIndexBuffer, int nFirstIndex = 0, int nIndexCount = 0, int nIndexOffset = 0);
		void EndModify(bool bSpewData = false);

		// returns the number of indices
		int	IndexCount() const;

		// Returns the total number of indices across all Locks()
		int TotalIndexCount() const;

		// Resets the mesh builder so it points to the start of everything again
		void Reset() const;

		// Selects the nth Index 
		void SelectIndex(int nBufferIndex) const;

		// Advances the current index by one
		void AdvanceIndex();
		void AdvanceIndices(int nIndexCount);

		int GetCurrentIndex() const;
		int GetFirstIndex() const;

		unsigned short const* Index() const;

		// Used to define the indices (only used if you aren't using primitives)
		void Index(unsigned short nIndex) const;

		// Fast Index! No need to call advance index, and no random access allowed
		void FastIndex(unsigned short nIndex);

		// NOTE: This version is the one you really want to achieve write-combining;
		// Write combining only works if you write in 4 bytes chunks.
		void FastIndex2(unsigned short nIndex1, unsigned short nIndex2);

		// Generates indices for a particular primitive type
		void GenerateIndices(MaterialPrimitiveType_t primitiveType, int nIndexCount);

		// FIXME: Remove! Backward compat so we can use this from a CMeshBuilder.
		void AttachBegin(IMesh* pMesh, int nMaxIndexCount, const MeshDesc_t &desc);
		void AttachEnd();
		void AttachBeginModify(IMesh* pMesh, int nFirstIndex, int nIndexCount, const MeshDesc_t &desc);
		void AttachEndModify();

	private:
		// The mesh we're modifying
		IIndexBuffer	*m_pIndexBuffer;

		// Max number of indices
		int				m_nMaxIndexCount;

		// Number of indices
		int				m_nIndexCount;

		// Offset to add to each index as it's written into the buffer
		int				m_nIndexOffset;

		// The current index
		mutable int		m_nCurrentIndex;

		// Total number of indices appended
		int				m_nTotalIndexCount;

		// First index buffer offset + first index
		unsigned int	m_nBufferOffset;
		unsigned int	m_nBufferFirstIndex;

		// Used to make sure Begin/End calls and BeginModify/EndModify calls match.
		bool			m_bModify;
	};

	class CVertexBuilder : VertexDesc_t
	{
	public:
		CVertexBuilder();
		CVertexBuilder(IVertexBuffer *pVertexBuffer, VertexFormat_t fmt = 0);
		~CVertexBuilder();

		// Begins, ends modification of the index buffer (returns true if the lock succeeded)
		// A lock may not succeed if append is set to true and there isn't enough room
		// NOTE: Append is only used with dynamic index buffers; it's ignored for static buffers
		bool Lock(int nMaxIndexCount, bool bAppend = false);
		void Unlock();

		// Spews the current data
		// NOTE: Can only be called during a lock/unlock block
		void SpewData() const;

		// Returns the number of indices we can fit into the buffer without needing to discard
		int GetRoomRemaining() const;

		// Binds this vertex buffer
		void Bind(IMatRenderContext *pContext, int nStreamID, VertexFormat_t usage = 0) const;

		// Returns the byte offset
		int Offset() const;

		// This must be called before Begin, if a vertex buffer with a compressed format is to be used
		void SetCompressionType(VertexCompressionType_t compressionType);
		static void ValidateCompressionType();

		void Begin(IVertexBuffer *pVertexBuffer, int nVertexCount, int *nFirstVertex);
		void Begin(IVertexBuffer *pVertexBuffer, int nVertexCount);

		// Use this when you're done writing
		// Set bDraw to true to call m_pMesh->Draw automatically.
		void End(bool bSpewData = false);

		// Locks the vertex buffer to modify existing data
		// Passing nVertexCount == -1 says to lock all the vertices for modification.
		void BeginModify(IVertexBuffer *pVertexBuffer, int nFirstVertex = 0, int nVertexCount = -1);
		void EndModify(bool bSpewData = false);

		// returns the number of vertices
		int VertexCount() const;

		// Returns the total number of vertices across all Locks()
		int TotalVertexCount() const;

		// Resets the mesh builder so it points to the start of everything again
		void Reset();

		// Returns the size of the vertex
		int VertexSize() const { return m_ActualVertexSize; }

		// returns the data size of a given texture coordinate
		int TextureCoordinateSize(int nTexCoordNumber) { return m_VertexSize_TexCoord[nTexCoordNumber]; }

		// Returns the base vertex memory pointer
		void* BaseVertexData() const;

		// Selects the nth Vertex and Index 
		void SelectVertex(int idx);

		// Advances the current vertex and index by one
		void AdvanceVertex();
		void AdvanceVertices(int nVerts);

		int GetCurrentVertex() const;
		int GetFirstVertex() const;

		// Data retrieval...
		const float *Position() const;

		const float *Normal() const;

		unsigned int Color() const;

		unsigned char *Specular() const;

		const float *TexCoord(int stage) const;

		const float *TangentS() const;
		const float *TangentT() const;

		const float *BoneWeight() const;
		float Wrinkle() const;

		int NumBoneWeights() const;
#ifndef NEW_SKINNING
		unsigned char *BoneMatrix() const;
#else
		float *BoneMatrix() const;
#endif

		// position setting
		void Position3f(float x, float y, float z) const;
		void Position3fv(const float *v) const;

		// normal setting
		void Normal3f(float nx, float ny, float nz) const;
		void Normal3fv(const float *n) const;
		void NormalDelta3fv(const float *n) const;
		void NormalDelta3f(float nx, float ny, float nz) const;
		// normal setting (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedNormal3f(float nx, float ny, float nz) const;
		template <VertexCompressionType_t T> void CompressedNormal3fv(const float *n) const;

		// color setting
		void Color3f(float r, float g, float b) const;
		void Color3fv(const float *rgb) const;
		void Color4f(float r, float g, float b, float a) const;
		void Color4fv(const float *rgba) const;

		// Faster versions of color
		void Color3ub(unsigned char r, unsigned char g, unsigned char b) const;
		void Color3ubv(unsigned char const* rgb) const;
		void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const;
		void Color4ubv(unsigned char const* rgba) const;

		// specular color setting
		void Specular3f(float r, float g, float b) const;
		void Specular3fv(const float *rgb) const;
		void Specular4f(float r, float g, float b, float a) const;
		void Specular4fv(const float *rgba) const;

		// Faster version of specular
		void Specular3ub(unsigned char r, unsigned char g, unsigned char b) const;
		void Specular3ubv(unsigned char const *c) const;
		void Specular4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const;
		void Specular4ubv(unsigned char const *c) const;

		// texture coordinate setting
		void TexCoord1f(int stage, float s) const;
		void TexCoord2f(int stage, float s, float t) const;
		void TexCoord2fv(int stage, const float *st) const;
		void TexCoord3f(int stage, float s, float t, float u) const;
		void TexCoord3fv(int stage, const float *stu) const;
		void TexCoord4f(int stage, float s, float t, float u, float w) const;
		void TexCoord4fv(int stage, const float *stuv) const;

		void TexCoordSubRect2f(int stage, float s, float t, float offsetS, float offsetT, float scaleS, float scaleT) const;
		void TexCoordSubRect2fv(int stage, const float *st, const float *offset, const float *scale) const;

		// tangent space 
		void TangentS3f(float sx, float sy, float sz) const;
		void TangentS3fv(const float* s) const;

		void TangentT3f(float tx, float ty, float tz) const;
		void TangentT3fv(const float* t) const;

		// Wrinkle
		void Wrinkle1f(float flWrinkle) const;

		// bone weights
		void BoneWeight(int idx, float weight) const;
		// bone weights (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedBoneWeight3fv(const float * pWeights) const;

		// bone matrix index
		void BoneMatrix(int idx, int matrixIndex) const;

		// Generic per-vertex data
		void UserData(const float* pData) const;
		// Generic per-vertex data (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedUserData(const float* pData) const;

		// Fast Vertex! No need to call advance vertex, and no random access allowed. 
		// WARNING - these are low level functions that are intended only for use
		// in the software vertex skinner.
		void FastVertex(/*ModelVertexDX7_t*/ const int &vertex);
		void FastVertexSSE(/*ModelVertexDX7_t*/ const int &vertex);

		// store 4 dx7 vertices fast. for special sse dx7 pipeline
		void Fast4VerticesSSE(
			/*ModelVertexDX7_t*/ const int *vtx_a,
			/*ModelVertexDX7_t*/ const int *vtx_b,
			/*ModelVertexDX7_t*/ const int *vtx_c,
			/*ModelVertexDX7_t*/ const int *vtx_d);

		void FastVertex(/*ModelVertexDX8_t*/ const float &vertex);
		void FastVertexSSE(/*ModelVertexDX8_t*/ const float &vertex);

		// Add number of verts and current vert since FastVertex routines do not update.
		void FastAdvanceNVertices(int n);

#if defined( _X360 )
		void VertexDX8ToX360(const ModelVertexDX8_t &vertex);
#endif

		// FIXME: Remove! Backward compat so we can use this from a CMeshBuilder.
		void AttachBegin(IMesh* pMesh, int nMaxVertexCount, const MeshDesc_t &desc);
		void AttachEnd();
		void AttachBeginModify(IMesh* pMesh, int nFirstVertex, int nVertexCount, const MeshDesc_t &desc);
		void AttachEndModify();

	private:
		// The vertex buffer we're modifying
		IVertexBuffer *m_pVertexBuffer;

		// Used to make sure Begin/End calls and BeginModify/EndModify calls match.
		bool m_bModify;

		// Max number of indices and vertices
		int m_nMaxVertexCount;

		// Number of indices and vertices
		int m_nVertexCount;

		// The current vertex and index
		mutable int m_nCurrentVertex;

		// Optimization: Pointer to the current pos, norm, texcoord, and color
		mutable float			*m_pCurrPosition;
		mutable float			*m_pCurrNormal;
		mutable float			*m_pCurrTexCoord[8];
		mutable unsigned char	*m_pCurrColor;

		// Total number of vertices appended
		int m_nTotalVertexCount;

		// First vertex buffer offset + index
		unsigned int m_nBufferOffset;
		unsigned int m_nBufferFirstVertex;

#if ( COMPRESSED_NORMALS_TYPE == COMPRESSED_NORMALS_COMBINEDTANGENTS_UBYTE4 )
		// Debug checks to make sure we write userdata4/tangents AFTER normals
		bool m_bWrittenNormal : 1;
		bool m_bWrittenUserData : 1;
#endif

		friend class CMeshBuilder;
	};

	class CMeshBuilder : public MeshDesc_t
	{
	public:
		CMeshBuilder();
		~CMeshBuilder() { /*if(m_pMesh) throw std::exception("CMeshBuilder: Begin() without an End()");*/ }		// if this fires you did a Begin() without an End()

													// This must be called before Begin, if a vertex buffer with a compressed format is to be used
		void SetCompressionType(VertexCompressionType_t compressionType);

		// Locks the vertex buffer
		// (*cannot* use the Index() call below)
		void Begin(IMesh *pMesh, MaterialPrimitiveType_t type, int numPrimitives);

		// Locks the vertex buffer, can specify arbitrary index lists
		// (must use the Index() call below)
		void Begin(IMesh *pMesh, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount, int *nFirstVertex);
		void Begin(IMesh *pMesh, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount);

		// forward compat
		static void Begin(IVertexBuffer *pVertexBuffer, MaterialPrimitiveType_t type, int numPrimitives);
		static void Begin(IVertexBuffer *pVertexBuffer, IIndexBuffer *pIndexBuffer, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount, int *nFirstVertex);
		static void Begin(IVertexBuffer *pVertexBuffer, IIndexBuffer *pIndexBuffer, MaterialPrimitiveType_t type, int nVertexCount, int nIndexCount);

		// Use this when you're done writing
		// Set bDraw to true to call m_pMesh->Draw automatically.
		void End(bool bSpewData = false, bool bDraw = false);

		// Locks the vertex buffer to modify existing data
		// Passing nVertexCount == -1 says to lock all the vertices for modification.
		// Pass 0 for nIndexCount to not lock the index buffer.
		void BeginModify(IMesh *pMesh, int nFirstVertex = 0, int nVertexCount = -1, int nFirstIndex = 0, int nIndexCount = 0);
		void EndModify(bool bSpewData = false);

		// A helper method since this seems to be done a whole bunch.
		void DrawQuad(IMesh* pMesh, const float *v1, const float *v2,
			const float *v3, const float *v4, unsigned char const *pColor, bool wireframe = false);

		// returns the number of indices and vertices
		int VertexCount() const;
		int	IndexCount() const;

		// Resets the mesh builder so it points to the start of everything again
		void Reset();

		// Returns the size of the vertex
		int VertexSize() const { return m_ActualVertexSize; }

		// returns the data size of a given texture coordinate
		int TextureCoordinateSize(int nTexCoordNumber) { return m_VertexSize_TexCoord[nTexCoordNumber]; }

		// Returns the base vertex memory pointer
		void* BaseVertexData() const;

		// Selects the nth Vertex and Index 
		void SelectVertex(int idx);
		void SelectIndex(int idx) const;

		// Given an index, point to the associated vertex
		void SelectVertexFromIndex(int idx);

		// Advances the current vertex and index by one
		void AdvanceVertex();
		void AdvanceVertices(int nVerts);
		void AdvanceIndex();
		void AdvanceIndices(int nIndices);

		int GetCurrentVertex() const;
		int GetCurrentIndex() const;

		// Data retrieval...
		const float *Position() const;

		const float *Normal() const;

		unsigned int Color() const;

		unsigned char *Specular() const;

		const float *TexCoord(int stage) const;

		const float *TangentS() const;
		const float *TangentT() const;

		const float *BoneWeight() const;
		float Wrinkle() const;

		int NumBoneWeights() const;
#ifndef NEW_SKINNING
		unsigned char *BoneMatrix() const;
#else
		float *BoneMatrix() const;
#endif
		unsigned short const *Index() const;

		// position setting
		void Position3f(float x, float y, float z) const;
		void Position3fv(const float *v) const;

		// normal setting
		void Normal3f(float nx, float ny, float nz) const;
		void Normal3fv(const float *n) const;
		void NormalDelta3fv(const float *n) const;
		void NormalDelta3f(float nx, float ny, float nz) const;

		// normal setting (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedNormal3f(float nx, float ny, float nz) const;
		template <VertexCompressionType_t T> void CompressedNormal3fv(const float *n) const;

		// color setting
		void Color3f(float r, float g, float b) const;
		void Color3fv(const float *rgb) const;
		void Color4f(float r, float g, float b, float a) const;
		void Color4fv(const float *rgba) const;

		// Faster versions of color
		void Color3ub(unsigned char r, unsigned char g, unsigned char b) const;
		void Color3ubv(unsigned char const* rgb) const;
		void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const;
		void Color4ubv(unsigned char const* rgba) const;

		// specular color setting
		void Specular3f(float r, float g, float b) const;
		void Specular3fv(const float *rgb) const;
		void Specular4f(float r, float g, float b, float a) const;
		void Specular4fv(const float *rgba) const;

		// Faster version of specular
		void Specular3ub(unsigned char r, unsigned char g, unsigned char b) const;
		void Specular3ubv(unsigned char const *c) const;
		void Specular4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const;
		void Specular4ubv(unsigned char const *c) const;

		// texture coordinate setting
		void TexCoord1f(int stage, float s) const;
		void TexCoord2f(int stage, float s, float t) const;
		void TexCoord2fv(int stage, const float *st) const;
		void TexCoord3f(int stage, float s, float t, float u) const;
		void TexCoord3fv(int stage, const float *stu) const;
		void TexCoord4f(int stage, float s, float t, float u, float w) const;
		void TexCoord4fv(int stage, const float *stuv) const;

		void TexCoordSubRect2f(int stage, float s, float t, float offsetS, float offsetT, float scaleS, float scaleT) const;
		void TexCoordSubRect2fv(int stage, const float *st, const float *offset, const float *scale) const;

		// tangent space 
		void TangentS3f(float sx, float sy, float sz) const;
		void TangentS3fv(const float *s) const;

		void TangentT3f(float tx, float ty, float tz) const;
		void TangentT3fv(const float *t) const;

		// Wrinkle
		void Wrinkle1f(float flWrinkle) const;

		// bone weights
		void BoneWeight(int idx, float weight) const;
		// bone weights (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedBoneWeight3fv(const float * pWeights) const;

		// bone matrix index
		void BoneMatrix(int idx, int matrixIndex) const;

		// Generic per-vertex data
		void UserData(const float *pData) const;
		// Generic per-vertex data (templatized for code which needs to support compressed vertices)
		template <VertexCompressionType_t T> void CompressedUserData(const float* pData) const;

		// Used to define the indices (only used if you aren't using primitives)
		void Index(unsigned short index) const;

		// NOTE: Use this one to get write combining! Much faster than the other version of FastIndex
		// Fast Index! No need to call advance index, and no random access allowed
		void FastIndex2(unsigned short nIndex1, unsigned short nIndex2);

		// Fast Index! No need to call advance index, and no random access allowed
		void FastIndex(unsigned short index);

		// Fast Vertex! No need to call advance vertex, and no random access allowed. 
		// WARNING - these are low level functions that are intended only for use
		// in the software vertex skinner.
		void FastVertex(/*ModelVertexDX7_t*/ const int &vertex);
		void FastVertexSSE(/*ModelVertexDX7_t*/ const int &vertex);

		// store 4 dx7 vertices fast. for special sse dx7 pipeline
		void Fast4VerticesSSE(
			/*ModelVertexDX7_t*/ const int *vtx_a,
			/*ModelVertexDX7_t*/ const int *vtx_b,
			/*ModelVertexDX7_t*/ const int *vtx_c,
			/*ModelVertexDX7_t*/ const int *vtx_d);

		void FastVertex(/*ModelVertexDX8_t*/ const float &vertex);
		void FastVertexSSE(/*ModelVertexDX8_t*/ const float &vertex);

		// Add number of verts and current vert since FastVertexxx routines do not update.
		void FastAdvanceNVertices(int n);

#if defined( _X360 )
		void VertexDX8ToX360(const ModelVertexDX8_t &vertex);
#endif

	private:
		// Computes number of verts and indices 
		static void ComputeNumVertsAndIndices(int *pMaxVertices, int *pMaxIndices,
			MaterialPrimitiveType_t type, int nPrimitiveCount);
		static int IndicesFromVertices(MaterialPrimitiveType_t type, int nVertexCount);

		// The mesh we're modifying
		IMesh *m_pMesh;

		MaterialPrimitiveType_t m_Type;

		// Generate indices?
		bool m_bGenerateIndices;

		CIndexBuilder	m_IndexBuilder;
		CVertexBuilder	m_VertexBuilder;
	};

	enum DeformationType_t
	{
		DEFORMATION_CLAMP_TO_BOX_IN_WORLDSPACE = 1,							// minxyz.minsoftness / maxxyz.maxsoftness
	};
	
	struct DeformationBase_t										// base class. don't use this
	{
		DeformationType_t m_eType;
	};

	struct Rect_t
	{
		int x, y;
		int width, height;
	};

	struct MorphVertexInfo_t
	{
		int m_nVertexId;		// What vertex is this going to affect?
		int m_nMorphTargetId;	// What morph did it come from?
		Vector m_PositionDelta;	// Positional morph delta
		Vector m_NormalDelta;	// Normal morph delta
		float m_flWrinkleDelta;	// Wrinkle morph delta
		float m_flSpeed;
		float m_flSide;
	};

	enum MorphWeightType_t
	{
		MORPH_WEIGHT = 0,
		MORPH_WEIGHT_LAGGED,
		MORPH_WEIGHT_STEREO,
		MORPH_WEIGHT_STEREO_LAGGED,

		MORPH_WEIGHT_COUNT,
	};

	struct MorphWeight_t
	{
		float m_pWeight[MORPH_WEIGHT_COUNT];
	};

	typedef unsigned int MorphFormat_t;

	class IMorph
	{
	public:
		// Locks the morph, destroys any existing contents
		virtual void Lock(float flFloatToFixedScale = 1.0f) = 0;

		// Adds a morph
		virtual void AddMorph(const MorphVertexInfo_t &info) = 0;

		// Unlocks the morph
		virtual void Unlock() = 0;
	};

	class IRefCounted
	{
		volatile long refCount;

	public:
		virtual int AddRef() = 0;
		virtual int Release() = 0;
	};

	class CFunctor : public IRefCounted
	{
	public:
		CFunctor()
		{
			m_nUserID = 0;
		}
		virtual void operator()() = 0;

		unsigned m_nUserID; // For debugging
	};

	
	class ICallQueue
	{
	public:
		void QueueFunctor(CFunctor *pFunctor)
		{
			QueueFunctorInternal(RetAddRef(pFunctor));
		}

		//FUNC_GENERATE_QUEUE_METHODS();

	private:
		virtual void QueueFunctorInternal(CFunctor *pFunctor) = 0;
	};
	
	class IMatRenderContext : public IRefCounted
	{
	public:
		virtual void				BeginRender() = 0;
		virtual void				EndRender() = 0;

		virtual void				Flush(bool flushHardware = false) = 0;

		virtual void				BindLocalCubemap(ITexture *pTexture) = 0;

		// pass in an ITexture (that is build with "rendertarget" "1") or
		// pass in NULL for the regular backbuffer.
		virtual void				SetRenderTarget(ITexture *pTexture) = 0;
		virtual ITexture *			GetRenderTarget() = 0;

		virtual void				GetRenderTargetDimensions(int &width, int &height) const = 0;

		// Bind a material is current for rendering.
		virtual void				Bind(IMaterial *material, void *proxyData = nullptr) = 0;
		// Bind a lightmap page current for rendering.  You only have to 
		// do this for materials that require lightmaps.
		virtual void				BindLightmapPage(int lightmapPageID) = 0;

		// inputs are between 0 and 1
		virtual void				DepthRange(float zNear, float zFar) = 0;

		virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

		// read to a unsigned char rgb image.
		virtual void				ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) = 0;
		virtual void				ReadPixelsAsync(int, int, int, int, unsigned char *, ImageFormat, ITexture *,/* CThreadEvent * */ void*) = 0;
		virtual void				ReadPixelsAsyncGetResult(int, int, int, int, unsigned char *, ImageFormat,/* CThreadEvent * */ void*) = 0;
		// Sets lighting
		virtual void				SetLightingState(MaterialLightingState_t const&) = 0;
		//virtual void				SetAmbientLight(float r, float g, float b) = 0;
		virtual void				SetLights(int lightNum, const LightDesc_t& desc) = 0;

		// The faces of the cube are specified in the same order as cubemap textures
		virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;

		// Blit the backbuffer to the framebuffer texture
		virtual void				CopyRenderTargetToTexture(ITexture *pTexture) = 0;

		// Set the current texture that is a copy of the framebuffer.
		virtual void				SetFrameBufferCopyTexture(ITexture *pTexture, int textureIndex = 0) = 0;
		virtual ITexture		   *GetFrameBufferCopyTexture(int textureIndex) = 0;

		//
		// end vertex array api
		//

		// matrix api
		virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
		virtual void				PushMatrix() = 0;
		virtual void				PopMatrix() = 0;
		virtual void				LoadMatrix(VMatrix const& matrix) = 0;
		virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
		virtual void				MultMatrix(VMatrix const& matrix) = 0;
		virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
		virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
		virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
		virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix *matrix) = 0;
		virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t *matrix) = 0;
		virtual void				LoadIdentity(void) = 0;
		virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
		virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
		virtual void				PickMatrix(int x, int y, int width, int height) = 0;
		virtual void				Rotate(float angle, float x, float y, float z) = 0;
		virtual void				Translate(float x, float y, float z) = 0;
		virtual void				Scale(float x, float y, float z) = 0;
		// end matrix api

		// Sets/gets the viewport
		virtual void				Viewport(int x, int y, int width, int height) = 0;
		virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;

		// The cull mode
		virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
		virtual void				FlipCullMode() = 0;
		// end matrix api
		
		virtual void				BeginGeneratingCSMs() = 0;
		virtual void				EndGeneratingCSMs() = 0;
		virtual void				PerpareForCascadeDraw(int, float, float) = 0;

		// This could easily be extended to a general user clip plane
		virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
		// garymcthack : fog z is always used for heightclipz for now.
		virtual void				SetHeightClipZ(float z) = 0;

		// Fog methods...
		virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
		virtual void				FogStart(float fStart) = 0;
		virtual void				FogEnd(float fEnd) = 0;
		virtual void				SetFogZ(float fogZ) = 0;
		virtual MaterialFogMode_t	GetFogMode(void) = 0;

		virtual void				FogColor3f(float r, float g, float b) = 0;
		virtual void				FogColor3fv(float const* rgb) = 0;
		virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
		virtual void				FogColor3ubv(unsigned char const* rgb) = 0;

		virtual void				GetFogColor(unsigned char *rgb) = 0;

		// Sets the number of bones for skinning
		virtual void				SetNumBoneWeights(int numBones) = 0;

		// Creates/destroys Mesh
		virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char *pTextureBudgetGroup, IMaterial * pMaterial = nullptr) = 0;
		virtual void DestroyStaticMesh(IMesh* mesh) = 0;

		// Gets the dynamic mesh associated with the currently bound material
		// note that you've got to render the mesh before calling this function 
		// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
		// returned by this call.
		// Use buffered = false if you want to not have the mesh be buffered,
		// but use it instead in the following pattern:
		//		meshBuilder.Begin
		//		meshBuilder.End
		//		Draw partial
		//		Draw partial
		//		Draw partial
		//		meshBuilder.Begin
		//		meshBuilder.End
		//		etc
		// Use Vertex or Index Override to supply a static vertex or index buffer
		// to use in place of the dynamic buffers.
		//
		// If you pass in a material in pAutoBind, it will automatically bind the
		// material. This can be helpful since you must bind the material you're
		// going to use BEFORE calling GetDynamicMesh.
		virtual IMesh* GetDynamicMesh(
			bool buffered = true,
			IMesh* pVertexOverride = nullptr,
			IMesh* pIndexOverride = nullptr,
			IMaterial *pAutoBind = nullptr) = 0;

		// ------------ New Vertex/Index Buffer interface ----------------------------
		// Do we need support for bForceTempMesh and bSoftwareVertexShader?
		// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
		virtual IVertexBuffer *CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char *pTextureBudgetGroup) = 0;
		virtual IIndexBuffer *CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char *pTextureBudgetGroup) = 0;
		virtual void DestroyVertexBuffer(IVertexBuffer *) = 0;
		virtual void DestroyIndexBuffer(IIndexBuffer *) = 0;
		// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
		virtual IVertexBuffer *GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
		virtual IIndexBuffer *GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;
		virtual void BindVertexBuffer(int streamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
		virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes) = 0;
		virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
		// ------------ End ----------------------------

		// Selection mode methods
		virtual int  SelectionMode(bool selectionMode) = 0;
		virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
		virtual void ClearSelectionNames() = 0;
		virtual void LoadSelectionName(int name) = 0;
		virtual void PushSelectionName(int name) = 0;
		virtual void PopSelectionName() = 0;

		// Sets the Clear Color for ClearBuffer....
		virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
		virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;

		// Allows us to override the depth buffer setting of a material
		virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;

		// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
		virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;

		// For debugging and building recording files. This will stuff a token into the recording file,
		// then someone doing a playback can watch for the token.
		virtual void	SyncToken(const char *pToken) = 0;

		// FIXME: REMOVE THIS FUNCTION!
		// The only reason why it's not gone is because we're a week from ship when I found the bug in it
		// and everything's tuned to use it.
		// It's returning values which are 2x too big (it's returning sphere diameter x2)
		// Use ComputePixelDiameterOfSphere below in all new code instead.
		virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;

		//
		// Occlusion query support
		//

		// Allocate and delete query objects.
		virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
		virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

		// Bracket drawing with begin and end so that we can get counts next frame.
		virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
		virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;

		// Get the number of pixels rendered between begin and end on an earlier frame.
		// Calling this in the same frame is a huge perf hit!
		virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;

		virtual void SetFlashlightMode(bool bEnable) = 0;

		virtual void SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture) = 0;

		virtual bool IsCascadedShadowMapping() = 0;
		virtual void SetCascadedShadowMapping(bool) = 0;
		virtual void SetCascadedShadowMappingState(/*CascadedShadowMappingState_t*/ int const&, ITexture *) = 0;

		// Gets the current height clip mode
		virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;

		// This returns the diameter of the sphere in pixels based on 
		// the current model, view, + projection matrices and viewport.
		virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;

		// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
		// planes (which are specified in world space) to generate projection-space user clip planes
		// Occasionally (for the particle system in hl2, for example), we want to override that
		// behavior and explictly specify a ViewProj transform for user clip planes
		virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
		virtual void	UserClipTransform(const VMatrix &worldToView) = 0;

		virtual bool GetFlashlightMode() const = 0;

		virtual bool IsCullingEnabledForSinglePassFlashlight() = 0;
		virtual void EnableCullingForSinglePassFlashlight(bool) = 0;
		// Used to make the handle think it's never had a successful query before
		virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
		
		// Creates/destroys morph data associated w/ a particular material
		virtual IMorph *CreateMorph(MorphFormat_t format, const char *pDebugName) = 0;
		virtual void DestroyMorph(IMorph *pMorph) = 0;

		// Binds the morph data for use in rendering
		virtual void BindMorph(IMorph *pMorph) = 0;

		// Sets flexweights for rendering
		virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;

		virtual void LockRenderData(int) = 0;
		virtual void UnlockRenderData() = 0;
		virtual void AddRefRenderData() = 0;
		virtual void ReleaseRenderData() = 0;
		virtual void IsRenderData(void const*) = 0;

		// Read w/ stretch to a host-memory buffer
		virtual void ReadPixelsAndStretch(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) = 0;

		// Gets the window size
		virtual void GetWindowSize(int &width, int &height) const = 0;

		// This function performs a texture map from one texture map to the render destination, doing
		// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
		// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
		// for non-scaled operations.
		virtual void DrawScreenSpaceRectangle(
			IMaterial *pMaterial,
			int destx, int desty,
			int width, int height,
			float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
																// destx/y
			float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
																// destx+width-1, desty+height-1
			int src_texture_width, int src_texture_height,		// needed for fixup
			void *pClientRenderable = nullptr,
			int nXDice = 1,
			int nYDice = 1) = 0;

		virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;

		// This version will push the current rendertarget + current viewport onto the stack
		virtual void PushRenderTargetAndViewport() = 0;

		// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
		virtual void PushRenderTargetAndViewport(ITexture *pTexture) = 0;

		// This version will push a new rendertarget + a specified viewport onto the stack
		virtual void PushRenderTargetAndViewport(ITexture *pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

		// This version will push a new rendertarget + a specified viewport onto the stack
		virtual void PushRenderTargetAndViewport(ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

		// This will pop a rendertarget + viewport
		virtual void PopRenderTargetAndViewport() = 0;

		// Binds a particular texture as the current lightmap
		virtual void BindLightmapTexture(ITexture *pLightmapTexture) = 0;

		// Blit a subrect of the current render target to another texture
		virtual void CopyRenderTargetToTextureEx(ITexture *pTexture, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect = nullptr) = 0;
		virtual void CopyRenderTargetToTextureExCopyTextureToRenderTargetEx(int, ITexture *, Rect_t *, Rect_t *) = 0;
		// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
		virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;

		// Rendering parameters control special drawing modes withing the material system, shader
		// system, shaders, and engine. renderparm.h has their definitions.
		virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
		virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
		virtual void SetVectorRenderingParameter(int parm_number, Vector const &value) = 0;

		// stencil buffer operations.
		virtual void SetStencilState(/*ShaderStencilState_t*/ int const&) = 0;
		//virtual void SetStencilEnable(bool onoff) = 0;
		//virtual void SetStencilFailOperation(StencilOperation_t op) = 0;
		//virtual void SetStencilZFailOperation(StencilOperation_t op) = 0;
		//virtual void SetStencilPassOperation(StencilOperation_t op) = 0;
		//virtual void SetStencilCompareFunction(StencilComparisonFunction_t cmpfn) = 0;
		//virtual void SetStencilReferenceValue(int ref) = 0;
		//virtual void SetStencilTestMask(uint32 msk) = 0;
		//virtual void SetStencilWriteMask(uint32 msk) = 0;
		virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) = 0;

		virtual void SetRenderTargetEx(int nRenderTargetID, ITexture *pTexture) = 0;

		// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
		// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
		virtual void PushCustomClipPlane(const float *pPlane) = 0;
		virtual void PopCustomClipPlane() = 0;

		// Returns the number of vertices + indices we can render using the dynamic mesh
		// Passing true in the second parameter will return the max # of vertices + indices
		// we can use before a flush is provoked and may return different values 
		// if called multiple times in succession. 
		// Passing false into the second parameter will return
		// the maximum possible vertices + indices that can be rendered in a single batch
		virtual void GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices) = 0;

		// Returns the max possible vertices + indices to render in a single draw call
		virtual int GetMaxVerticesToRender(IMaterial *pMaterial) = 0;
		virtual int GetMaxIndicesToRender() = 0;
		virtual void DisableAllLocalLights() = 0;
		virtual int CompareMaterialCombos(IMaterial *pMaterial1, IMaterial *pMaterial2, int lightMapID1, int lightMapID2) = 0;

		virtual IMesh *GetFlexMesh() = 0;

		virtual void SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture) = 0;

		// Returns the currently bound local cubemap
		virtual ITexture *GetLocalCubemap() = 0;

		// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
		virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;

		//enables/disables all entered clipping planes, returns the input from the last time it was called.
		virtual bool EnableClipping(bool bEnable) = 0;

		//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
		virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ) = 0;

		// Hooks for firing PIX events from outside the Material System...
		virtual void BeginPIXEvent(unsigned long color, const char *szName) = 0;
		virtual void EndPIXEvent() = 0;
		virtual void SetPIXMarker(unsigned long color, const char *szName) = 0;

		// Batch API
		// from changelist 166623:
		// - replaced obtuse material system batch usage with an explicit and easier to thread API
		virtual void BeginBatch(IMesh* pIndices) = 0;
		virtual void BindBatch(IMesh* pVertices, IMaterial *pAutoBind = nullptr) = 0;
		virtual void DrawBatch(int firstIndex, int numIndices) = 0;
		virtual void EndBatch() = 0;

		// Raw access to the call queue, which can be NULL if not in a queued mode
		virtual ICallQueue *GetCallQueue() = 0;

		// Returns the world-space camera position
		virtual void GetWorldSpaceCameraPosition(Vector *pCameraPos) = 0;
		virtual void GetWorldSpaceCameraVectors(Vector *pVecForward, Vector *pVecRight, Vector *pVecUp) = 0;

		// Tone mapping
		//virtual void				ResetToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing
		//virtual void				SetGoalToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing

																							// call TurnOnToneMapping before drawing the 3d scene to get the proper interpolated brightness
																							// value set.
		//virtual void				TurnOnToneMapping() = 0;

		// Set a linear vector color scale for all 3D rendering.
		// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
		virtual void				SetToneMappingScaleLinear(const Vector &scale) = 0;

		virtual Vector				GetToneMappingScaleLinear() = 0;
		virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;

		// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
		virtual void				PerformFullScreenStencilOperation(void) = 0;

		// Sets lighting origin for the current model (needed to convert directional lights to points)
		virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;

		// Set scissor rect for rendering
		virtual void				PushScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom) = 0;
		virtual void				PopScissorRect() = 0;

		// Methods used to build the morph accumulator that is read from when HW morph<ing is enabled.
		virtual void				BeginMorphAccumulation() = 0;
		virtual void				EndMorphAccumulation() = 0;
		virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;

		virtual void				PushDeformation(DeformationBase_t const *Deformation) = 0;
		virtual void				PopDeformation() = 0;
		virtual int					GetNumActiveDeformations() const = 0;

		virtual bool				GetMorphAccumulatorTexCoord(Vector2D *pTexCoord, IMorph *pMorph, int nVertex) = 0;

		// Version of get dynamic mesh that specifies a specific vertex format
		virtual IMesh*				GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
			IMesh* pVertexOverride = nullptr, IMesh* pIndexOverride = nullptr, IMaterial *pAutoBind = nullptr) = 0;

		virtual void				FogMaxDensity(float flMaxDensity) = 0;

		virtual IMaterial *GetCurrentMaterial() = 0;
		virtual int  GetCurrentNumBones() const = 0;
		virtual void *GetCurrentProxy() = 0;

		// Color correction related methods..
		// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
		// FIXME: Make IColorCorrectionSystem threadsafe?
		virtual void EnableColorCorrection(bool bEnable) = 0;
		virtual ColorCorrectionHandle_t AddLookup(const char *pName) = 0;
		virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
		virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
		virtual void LoadLookup(ColorCorrectionHandle_t handle, const char *pLookupName) = 0;
		virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
		virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
		virtual void ResetLookupWeights() = 0;
		virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;

		//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
		//Use this to mark it as invalid and use a dummy texture for depth reads.
		virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;

		// A special path used to tick the front buffer while loading on the 360
		virtual void SetNonInteractivePacifierTexture(ITexture *pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
		virtual void SetNonInteractiveTempFullscreenBuffer(ITexture *pTexture, MaterialNonInteractiveMode_t mode) = 0;
		virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
		virtual void RefreshFrontBufferNonInteractive() = 0;
	};
	
	struct VtfProcessingOptions
	{
		unsigned int cbSize;					// Set to sizeof( VtfProcessingOptions )

		//
		// Flags0
		//
		enum Flags0
		{
			// Have a channel decaying to a given decay goal for the given last number of mips
			OPT_DECAY_R = 0x00000001,	// Red decays
			OPT_DECAY_G = 0x00000002,	// Green decays
			OPT_DECAY_B = 0x00000004,	// Blue decays
			OPT_DECAY_A = 0x00000008,	// Alpha decays

			OPT_DECAY_EXP_R = 0x00000010,	// Channel R decays exponentially (otherwise linearly)
			OPT_DECAY_EXP_G = 0x00000020,	// Channel G decays exponentially (otherwise linearly)
			OPT_DECAY_EXP_B = 0x00000040,	// Channel B decays exponentially (otherwise linearly)
			OPT_DECAY_EXP_A = 0x00000080,	// Channel A decays exponentially (otherwise linearly)

			OPT_NOCOMPRESS = 0x00000100,	// Use uncompressed image format
			OPT_NORMAL_DUDV = 0x00000200,	// dU dV normal map
			OPT_FILTER_NICE = 0x00000400,	// Use nice filtering
			OPT_SRGB_PC_TO_360 = 0x00000800,	// Perform srgb correction for colormaps

			OPT_SET_ALPHA_ONEOVERMIP = 0x00001000,	// Alpha  = 1/miplevel
			OPT_PREMULT_COLOR_ONEOVERMIP = 0x00002000,	// Color *= 1/miplevel
			OPT_MIP_ALPHATEST = 0x00004000,			// Alpha-tested mip generation
			OPT_NORMAL_GA = 0x00008000,			// Use Green-Alpha normal compression
			OPT_PREMULT_ALPHA_MIPFRACTION = 0x00010000,	// Alpha *= miplevel/mipcount 
			OPT_COMPUTE_GRADIENT = 0x00020000			// Compute normalized du du gradients of green channel and store in r and b
		};

		unsigned int flags0;					// A combination of "Flags0"

		//
		// Decay settings
		//
		char clrDecayGoal[4];			// Goal colors for R G B A
		char numNotDecayMips[4];		// Number of first mips unaffected by decay (0 means all below mip0)
		float fDecayExponentBase[4];	// For exponential decay the base number (e.g. 0.75)

		//
		// Alpha fading with mip levels
		//
		char fullAlphaAtMipLevel;
		char minAlpha;
	};
	
	enum LookDir_t
	{
		LOOK_DOWN_X = 0,
		LOOK_DOWN_NEGX,
		LOOK_DOWN_Y,
		LOOK_DOWN_NEGY,
		LOOK_DOWN_Z,
		LOOK_DOWN_NEGZ,
	};
	
	class IVTFTexture
	{
	public:
		virtual ~IVTFTexture() { }
		// Initializes the texture and allocates space for the bits
		// In most cases, you shouldn't force the mip count.
		virtual bool Init(int nWidth, int nHeight, int nDepth, ImageFormat fmt, int nFlags, int iFrameCount, int nForceMipCount = -1) = 0;

		// Methods to set other texture fields
		virtual void SetBumpScale(float flScale) = 0;
		virtual void SetReflectivity(const Vector& vecReflectivity) = 0;

		// Methods to initialize the low-res image
		virtual void InitLowResImage(int nWidth, int nHeight, ImageFormat fmt) = 0;

		// set the resource data (for writers). pass size=0 to delete data. if pdata is not null,
		// the resource data will be copied from *pData
		virtual void* SetResourceData(unsigned int eType, void const* pData, size_t nDataSize) = 0;

		// find the resource data and return a pointer to it. The data pointed to by this pointer will
		// go away when the ivtftexture does. retruns null if resource not present
		virtual void* GetResourceData(unsigned int eType, size_t* pDataSize) const = 0;

		// Locates the resource entry info if it's present, easier than crawling array types
		virtual bool HasResourceEntry(unsigned int eType) const = 0;

		// Retrieve available resource types of this IVTFTextures
		//		arrTypesBuffer			buffer to be filled with resource types available.
		//		numTypesBufferElems		how many resource types the buffer can accomodate.
		// Returns:
		//		number of resource types available (can be greater than "numTypesBufferElems"
		//		in which case only first "numTypesBufferElems" are copied to "arrTypesBuffer")
		virtual unsigned int GetResourceTypes(unsigned int* arrTypesBuffer, int numTypesBufferElems) const = 0;

		// When unserializing, we can skip a certain number of mip levels,
		// and we also can just load everything but the image data
		// NOTE: If you load only the buffer header, you'll need to use the
		// VTFBufferHeaderSize() method below to only read that much from the file
		// NOTE: If you skip mip levels, the height + width of the texture will
		// change to reflect the size of the largest read in mip level
		virtual bool Unserialize(/*CUtlBuffer*/void* buf, bool bHeaderOnly = false, int nSkipMipLevels = 0) = 0;
		virtual bool Serialize(/*CUtlBuffer*/void* buf) = 0;

		// These are methods to help with optimization:
		// Once the header is read in, they indicate where to start reading
		// other data (measured from file start), and how many bytes to read....
		virtual void LowResFileInfo(int* pStartLocation, int* pSizeInBytes) const = 0;
		virtual void ImageFileInfo(int nFrame, int nFace, int nMip, int* pStartLocation, int* pSizeInBytes) const = 0;
		virtual int FileSize(int nMipSkipCount = 0) const = 0;

		// Attributes...
		virtual int Width() const = 0;
		virtual int Height() const = 0;
		virtual int Depth() const = 0;
		virtual int MipCount() const = 0;

		// returns the size of one row of a particular mip level
		virtual int RowSizeInBytes(int nMipLevel) const = 0;

		// returns the size of one face of a particular mip level
		virtual int FaceSizeInBytes(int nMipLevel) const = 0;

		virtual ImageFormat Format() const = 0;
		virtual int FaceCount() const = 0;
		virtual int FrameCount() const = 0;
		virtual int Flags() const = 0;

		virtual float BumpScale() const = 0;

		virtual int LowResWidth() const = 0;
		virtual int LowResHeight() const = 0;
		virtual ImageFormat LowResFormat() const = 0;

		// NOTE: reflectivity[0] = blue, [1] = greem, [2] = red
		virtual const Vector& Reflectivity() const = 0;

		virtual bool IsCubeMap() const = 0;
		virtual bool IsNormalMap() const = 0;
		virtual bool IsVolumeTexture() const = 0;

		// Computes the dimensions of a particular mip level
		virtual void ComputeMipLevelDimensions(int iMipLevel, int* pMipWidth, int* pMipHeight, int* pMipDepth) const = 0;

		// Computes the size (in bytes) of a single mipmap of a single face of a single frame 
		virtual int ComputeMipSize(int iMipLevel) const = 0;

		// Computes the size of a subrect (specified at the top mip level) at a particular lower mip level
		virtual void ComputeMipLevelSubRect(Rect_t* pSrcRect, int nMipLevel, Rect_t* pSubRect) const = 0;

		// Computes the size (in bytes) of a single face of a single frame
		// All mip levels starting at the specified mip level are included
		virtual int ComputeFaceSize(int iStartingMipLevel = 0) const = 0;

		// Computes the total size (in bytes) of all faces, all frames
		virtual int ComputeTotalSize() const = 0;

		// Returns the base address of the image data
		virtual char* ImageData() = 0;

		// Returns a pointer to the data associated with a particular frame, face, and mip level
		virtual char* ImageData(int iFrame, int iFace, int iMipLevel) = 0;

		// Returns a pointer to the data associated with a particular frame, face, mip level, and offset
		virtual char* ImageData(int iFrame, int iFace, int iMipLevel, int x, int y, int z = 0) = 0;

		// Returns the base address of the low-res image data
		virtual char* LowResImageData() = 0;

		// Converts the textures image format. Use IMAGE_FORMAT_DEFAULT
		// if you want to be able to use various tool functions below
		virtual	void ConvertImageFormat(ImageFormat fmt, bool bNormalToDUDV, bool bNormalToDXT5GA = false) = 0;

		// NOTE: The following methods only work on textures using the
		// IMAGE_FORMAT_DEFAULT!

		// Generate spheremap based on the current cube faces (only works for cubemaps)
		// The look dir indicates the direction of the center of the sphere
		// NOTE: Only call this *after* cube faces have been correctly
		// oriented (using FixCubemapFaceOrientation)
		virtual void GenerateSpheremap(LookDir_t lookDir = LOOK_DOWN_Z) = 0;

		// Generate spheremap based on the current cube faces (only works for cubemaps)
		// The look dir indicates the direction of the center of the sphere
		// NOTE: Only call this *after* cube faces have been correctly
		// oriented (using FixCubemapFaceOrientation)
		virtual void GenerateHemisphereMap(uint8_t* pSphereMapBitsRGBA, int targetWidth,
			int targetHeight, LookDir_t lookDir, int iFrame) = 0;

		// Fixes the cubemap faces orientation from our standard to the
		// standard the material system needs.
		virtual void FixCubemapFaceOrientation() = 0;

		// Generates mipmaps from the base mip levels
		virtual void GenerateMipmaps() = 0;

		// Put 1/miplevel (1..n) into alpha.
		virtual void PutOneOverMipLevelInAlpha() = 0;

		// Scale alpha by miplevel/mipcount
		virtual void PremultAlphaWithMipFraction() = 0;

		// Computes the reflectivity
		virtual void ComputeReflectivity() = 0;

		// Computes the alpha flags
		virtual void ComputeAlphaFlags() = 0;

		// Generate the low-res image bits
		virtual bool ConstructLowResImage() = 0;

		// Gets the texture all internally consistent assuming you've loaded
		// mip 0 of all faces of all frames
		virtual void PostProcess(bool bGenerateSpheremap, LookDir_t lookDir = LOOK_DOWN_Z, bool bAllowFixCubemapOrientation = true, bool bLoadedMiplevels = false) = 0;

		// Blends adjacent pixels on cubemap borders, since the card doesn't do it. If the texture
		// is S3TC compressed, then it has to do it AFTER the texture has been compressed to prevent
		// artifacts along the edges.
		//
		// If bSkybox is true, it assumes the faces are oriented in the way the engine draws the skybox
		// (which happens to be different from the way cubemaps have their faces).
		virtual void MatchCubeMapBorders(int iStage, ImageFormat finalFormat, bool bSkybox) = 0;

		// Sets threshhold values for alphatest mipmapping
		virtual void SetAlphaTestThreshholds(float flBase, float flHighFreq) = 0;

		virtual bool IsPreTiled() const = 0;

		// Sets post-processing flags (settings are copied, pointer passed to distinguish between structure versions)
		virtual void SetPostProcessingSettings(VtfProcessingOptions const* pOptions) = 0;
	};
	
	class IVisualsDataCompare
	{
	public:
		virtual void FillCompareBlob() = 0;
		virtual const void* GetCompareBlob() const = 0;
		virtual bool Compare(const void* pOther) = 0;
	};

	class IVisualsDataProcessor
	{
	public:
		IVisualsDataProcessor() {}

		virtual KeyValues* GenerateCustomMaterialKeyValues() = 0;
		virtual KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId) = 0;
		virtual IVisualsDataCompare* GetCompareObject() = 0;
		virtual bool HasCustomMaterial() const = 0;
		virtual const char* GetOriginalMaterialName() const = 0;
		virtual const char* GetOriginalMaterialBaseName() const = 0;
		virtual const char* GetPatternVTFName() const = 0;
		virtual void Refresh() = 0;

	protected:
		virtual ~IVisualsDataProcessor() {}
	};
	
	enum CompositeTextureFormat_t
	{
		COMPOSITE_TEXTURE_FORMAT_DXT1 = 1,
		COMPOSITE_TEXTURE_FORMAT_DXT5 = 5
	};

	enum CompositeTextureSize_t
	{
		COMPOSITE_TEXTURE_SIZE_256 = 8,
		COMPOSITE_TEXTURE_SIZE_512,
		COMPOSITE_TEXTURE_SIZE_1024,
		COMPOSITE_TEXTURE_SIZE_2048
	};

	struct SCompositeTextureInfo
	{
		IVisualsDataProcessor* m_pVisualsDataProcessor;
		CompositeTextureSize_t m_size;
		CompositeTextureFormat_t m_format;
		int m_nMaterialParamID;
		bool m_bSRGB;
	};
	
	class ICompositeTexture
	{
	public:
		virtual bool IsReady() const = 0;
		virtual bool GenerationComplete() const = 0;
		virtual IVTFTexture* GetResultVTF() = 0;
		virtual	const char* GetName() = 0;

		char m_szTextureName[MAX_PATH];
		CompositeTextureSize_t m_size;
		CompositeTextureFormat_t m_format;	// DXT1 or DXT5 only
		int m_nMaterialParamNameId;	// the material parameter that this texture will fill/replace in the eventual Custom Material, also used to interact with the VisualsDataProcessor
		bool m_bSRGB;
		//CompositeTextureStages_t m_nRegenerateStage;
		//CUtlBuffer m_compareBlob;

		//IVTFTexture* m_pResultVTF;
		//CCompositeTextureResult m_ResultTexture;

		//int m_nActualSize;

		//bool m_bNeedsRegenerate;
		//bool m_bNeedsFinalize;
		//IVTFTexture* m_pScratchVTF;
		//ITexture* m_pCustomMaterialRT;
		//KeyValues* m_pCompositingMaterialKeyValues;
		//IMaterial* m_pCompositingMaterial;
		//int m_nLastFrameCount;
		//CThreadEvent* m_pPixelsReadEvent;

		//bool m_bIgnorePicMip;
	};
	class __IRefCounted
	{
		volatile long refCount;
	public:
		int AddRef() { return _InterlockedIncrement(&refCount); }
		int Release()
		{
			int result = _InterlockedDecrement(&refCount);
			if (result)
				return result;
			this->unreference();
			return 0;
		}
		virtual void destructor(char flag) = 0;
		virtual bool OnFinalRelease() = 0;

		void unreference()
		{
			if (_InterlockedDecrement(&refCount) == 0 && OnFinalRelease())
			{
				destructor(1);
			}
		}	
	};
	
	class CCustomMaterial: public __IRefCounted
	{
	public:
		virtual IMaterial* GetMaterial() = 0;
		virtual void AddTexture(ICompositeTexture* pTexture) = 0;
		virtual ICompositeTexture* GetTexture(int nIndex) = 0;
		virtual bool IsValid() const = 0;
		virtual bool CheckRegenerate(int nSize) = 0;
		virtual const char* GetBaseMaterialName() = 0;

		CMaterialReference m_Material;
		CUtlVector< ICompositeTexture* > m_pTextures;

		bool m_bValid;
		int m_nModelMaterialIndex;
		KeyValues* m_pVMTKeyValues;
		const char* m_szBaseMaterialName;

		static int m_nMaterialCount;
	};

	class CCustomMaterialManager
	{
	public:
		virtual int Process() = 0;//0
		virtual CCustomMaterial* GetOrCreateCustomMaterial(KeyValues* pKeyValues, const CUtlVector< SCompositeTextureInfo >& vecTextureInfos, bool bIgnorePicMip = false) = 0; //unk usage
		virtual int DebugGetNumActiveCustomMaterials() = 0;
		virtual bool GetVMTKeyValues(const char* name, KeyValues* res) = 0;
		virtual ~CCustomMaterialManager() = 0;

		CConCommandMemberAccessor< CCustomMaterialManager > m_ReloadAllMaterials;
		CConCommandMemberAccessor< CCustomMaterialManager > m_Usage;
		CConCommandMemberAccessor< CCustomMaterialManager > m_ReloadVmtCache;
		CUtlVector< CCustomMaterial* > m_pCustomMaterials;
		CUtlMap< const char*, KeyValues* > m_mapVMTKeyValues;
	};

	DECLARE_POINTER_HANDLE1(MaterialLock_t);
	class IMaterialSystem
	{		
		typedef void(__thiscall* oUncacheUnusedMaterials)(void*);
		typedef void(__thiscall* oUncacheAllMaterials)(void*, bool);
		typedef void(__thiscall* oCacheUsedMaterials)(void*);
		typedef void(__thiscall* oReloadTextures)(void*);
		typedef void(__thiscall* oReloadMaterials)(void*, const char* pSubString);
		typedef IMatRenderContext* (__thiscall* oGetRenderContext)(void*);
		typedef MaterialLock_t(__thiscall* oLock)(void*);
		typedef void(__thiscall* oUnlock)(PVOID, MaterialLock_t);
		typedef IMaterial*(__thiscall* oCreateMaterial)(void*, const char*, KeyValues*);
		typedef IMaterial*(__thiscall* oFindMaterial)(void*, char const*, const char*, bool, const char*);
		typedef CCustomMaterialManager*(__thiscall* oCCustomMaterialManager)(void*);
	public:
		//	virtual void				UncacheAllMaterials( ) = 0; 115

		void UncacheUnusedMaterials()
		{
			return CallVFunction<oUncacheUnusedMaterials>(this, 77)(this);
		}

		void UncacheAllMaterials(bool bRecomputeStateSnapshots = false)
		{
			return CallVFunction<oUncacheAllMaterials>(this, 78)(this, bRecomputeStateSnapshots);
		}

		void CacheUsedMaterials()
		{
			return CallVFunction<oCacheUsedMaterials>(this, 80)(this);
		}

		void ReloadTextures()
		{
			return CallVFunction<oReloadTextures>(this, 81)(this);
		}

		void ReloadMaterials(const char* pSubString = nullptr)
		{
			return CallVFunction<oReloadMaterials>(this, 82)(this, pSubString);
		}

		IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues)
		{
			return CallVFunction<oCreateMaterial>(this, 83)(this, pMaterialName, pVMTKeyValues);
		}

		IMatRenderContext* GetRenderContext()
		{
			return CallVFunction<oGetRenderContext>(this, 115)(this);
		}

		MaterialLock_t Lock()
		{
			return CallVFunction<oLock>(this, 118)(this);
		}

		void Unlock(MaterialLock_t mutex)
		{
			return CallVFunction<oUnlock>(this, 119)(this, mutex);
		}

		IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = nullptr)
		{
			return CallVFunction<oFindMaterial>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
		}

		IMaterial* FindProceduralMaterial(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues)
		{
			return CallVFunction<IMaterial*(__thiscall*)(void*, const char*, const char*, KeyValues*)>(this, 124)(this, pMaterialName, pTextureGroupName, pVMTKeyValues);
		}
		CCustomMaterialManager* GetCustomMaterialManager()
		{
			return CallVFunction<oCCustomMaterialManager>(this, 152)(this);
		}
		void* GetCompositeTextureGenerator()
		{
			//153
			return nullptr;
		}
	};
}
#pragma once
