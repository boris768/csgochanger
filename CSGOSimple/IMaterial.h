#pragma once

#include <cstdint>
#include "SourceEngine/Vector.hpp"

namespace SourceEngine
{
	class IMaterialVar;

	typedef uint64_t VertexFormat_t;
	enum VertexFormatFlags_t : uint64_t
	{
		// Indicates an uninitialized VertexFormat_t value
		VERTEX_FORMAT_INVALID = 0xFFFFFFFFFFFFFFFFU,

		VERTEX_POSITION = 0x0001,
		VERTEX_NORMAL = 0x0002,
		VERTEX_COLOR = 0x0004,
		VERTEX_SPECULAR = 0x0008,

		VERTEX_TANGENT_S = 0x0010,
		VERTEX_TANGENT_T = 0x0020,
		VERTEX_TANGENT_SPACE = VERTEX_TANGENT_S | VERTEX_TANGENT_T,

		// Indicates we're using wrinkle
		VERTEX_WRINKLE = 0x0040,

		// Indicates we're using bone indices
		VERTEX_BONE_INDEX = 0x0080,

		// Indicates this is a vertex shader
		VERTEX_FORMAT_VERTEX_SHADER = 0x0100,

		// Indicates this format shouldn't be bloated to cache align it
		// (only used for VertexUsage)
		VERTEX_FORMAT_USE_EXACT_FORMAT = 0x0200,

		// Indicates that compressed vertex elements are to be used (see also VertexCompressionType_t)
		VERTEX_FORMAT_COMPRESSED = 0x400,

		// Update this if you add or remove bits...
		VERTEX_LAST_BIT = 10,

		VERTEX_BONE_WEIGHT_BIT = VERTEX_LAST_BIT + 1,
		USER_DATA_SIZE_BIT = VERTEX_LAST_BIT + 4,
		TEX_COORD_SIZE_BIT = VERTEX_LAST_BIT + 7,

		VERTEX_BONE_WEIGHT_MASK = (0x7 << VERTEX_BONE_WEIGHT_BIT),
		USER_DATA_SIZE_MASK = (0x7 << USER_DATA_SIZE_BIT),

		VERTEX_FORMAT_FIELD_MASK = 0x0FF,

		// If everything is off, it's an unknown vertex format
		VERTEX_FORMAT_UNKNOWN = 0,
	};
	enum ImageFormat
	{
		IMAGE_FORMAT_UNKNOWN = -1,
		IMAGE_FORMAT_RGBA8888 = 0,
		IMAGE_FORMAT_ABGR8888,
		IMAGE_FORMAT_RGB888,
		IMAGE_FORMAT_BGR888,
		IMAGE_FORMAT_RGB565,
		IMAGE_FORMAT_I8,
		IMAGE_FORMAT_IA88,
		IMAGE_FORMAT_P8,
		IMAGE_FORMAT_A8,
		IMAGE_FORMAT_RGB888_BLUESCREEN,
		IMAGE_FORMAT_BGR888_BLUESCREEN,
		IMAGE_FORMAT_ARGB8888,
		IMAGE_FORMAT_BGRA8888,
		IMAGE_FORMAT_DXT1,
		IMAGE_FORMAT_DXT3,
		IMAGE_FORMAT_DXT5,
		IMAGE_FORMAT_BGRX8888,
		IMAGE_FORMAT_BGR565,
		IMAGE_FORMAT_BGRX5551,
		IMAGE_FORMAT_BGRA4444,
		IMAGE_FORMAT_DXT1_ONEBITALPHA,
		IMAGE_FORMAT_BGRA5551,
		IMAGE_FORMAT_UV88,
		IMAGE_FORMAT_UVWQ8888,
		IMAGE_FORMAT_RGBA16161616F,
		IMAGE_FORMAT_RGBA16161616,
		IMAGE_FORMAT_UVLX8888,
		IMAGE_FORMAT_R32F, // Single-channel 32-bit floating point
		IMAGE_FORMAT_RGB323232F, // NOTE: D3D9 does not have this format
		IMAGE_FORMAT_RGBA32323232F,
		IMAGE_FORMAT_RG1616F,
		IMAGE_FORMAT_RG3232F,
		IMAGE_FORMAT_RGBX8888,

		IMAGE_FORMAT_NULL, // Dummy format which takes no video memory

		// Compressed normal map formats
		IMAGE_FORMAT_ATI2N, // One-surface ATI2N / DXN format
		IMAGE_FORMAT_ATI1N, // Two-surface ATI1N format

		IMAGE_FORMAT_RGBA1010102, // 10 bit-per component render targets
		IMAGE_FORMAT_BGRA1010102,
		IMAGE_FORMAT_R16F, // 16 bit FP format

		// Depth-stencil texture formats
		IMAGE_FORMAT_D16,
		IMAGE_FORMAT_D15S1,
		IMAGE_FORMAT_D32,
		IMAGE_FORMAT_D24S8,
		IMAGE_FORMAT_LINEAR_D24S8,
		IMAGE_FORMAT_D24X8,
		IMAGE_FORMAT_D24X4S4,
		IMAGE_FORMAT_D24FS8,
		IMAGE_FORMAT_D16_SHADOW, // Specific formats for shadow mapping
		IMAGE_FORMAT_D24X8_SHADOW, // Specific formats for shadow mapping

		// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
		IMAGE_FORMAT_LINEAR_BGRX8888,
		IMAGE_FORMAT_LINEAR_RGBA8888,
		IMAGE_FORMAT_LINEAR_ABGR8888,
		IMAGE_FORMAT_LINEAR_ARGB8888,
		IMAGE_FORMAT_LINEAR_BGRA8888,
		IMAGE_FORMAT_LINEAR_RGB888,
		IMAGE_FORMAT_LINEAR_BGR888,
		IMAGE_FORMAT_LINEAR_BGRX5551,
		IMAGE_FORMAT_LINEAR_I8,
		IMAGE_FORMAT_LINEAR_RGBA16161616,

		IMAGE_FORMAT_LE_BGRX8888,
		IMAGE_FORMAT_LE_BGRA8888,

		NUM_IMAGE_FORMATS
	};


	enum Collision_Group_t
	{
		COLLISION_GROUP_NONE = 0,
		COLLISION_GROUP_DEBRIS, // Collides with nothing but world and static stuff
		COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
		COLLISION_GROUP_INTERACTIVE_DEBRIS, // Collides with everything except other interactive debris or debris
		COLLISION_GROUP_INTERACTIVE, // Collides with everything except interactive debris or debris
		COLLISION_GROUP_PLAYER,
		COLLISION_GROUP_BREAKABLE_GLASS,
		COLLISION_GROUP_VEHICLE,
		COLLISION_GROUP_PLAYER_MOVEMENT, // For HL2, same as Collision_Group_Player, for
		// TF2, this filters out other players and CBaseObjects
		COLLISION_GROUP_NPC, // Generic NPC group
		COLLISION_GROUP_IN_VEHICLE, // for any entity inside a vehicle
		COLLISION_GROUP_WEAPON, // for any weapons that need collision detection
		COLLISION_GROUP_VEHICLE_CLIP, // vehicle clip brush to restrict vehicle movement
		COLLISION_GROUP_PROJECTILE, // Projectiles!
		COLLISION_GROUP_DOOR_BLOCKER, // Blocks entities not permitted to get near moving doors
		COLLISION_GROUP_PASSABLE_DOOR, // Doors that the player shouldn't collide with
		COLLISION_GROUP_DISSOLVING, // Things that are dissolving are in this group
		COLLISION_GROUP_PUSHAWAY, // Nonsolid on client and server, pushaway in player code

		COLLISION_GROUP_NPC_ACTOR, // Used so NPCs in scripts ignore the player.
		COLLISION_GROUP_NPC_SCRIPTED, // USed for NPCs in scripts that should not collide with each other
		COLLISION_GROUP_PZ_CLIP,


		COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE, // Only collides with bullets

		LAST_SHARED_COLLISION_GROUP
	};

	enum MaterialVarFlags_t
	{
		MATERIAL_VAR_DEBUG = (1 << 0),
		MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
		MATERIAL_VAR_NO_DRAW = (1 << 2),
		MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

		MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
		MATERIAL_VAR_VERTEXALPHA = (1 << 5),
		MATERIAL_VAR_SELFILLUM = (1 << 6),
		MATERIAL_VAR_ADDITIVE = (1 << 7),
		MATERIAL_VAR_ALPHATEST = (1 << 8),
		//	MATERIAL_VAR_UNUSED					  = (1 << 9),
		MATERIAL_VAR_ZNEARER = (1 << 10),
		MATERIAL_VAR_MODEL = (1 << 11),
		MATERIAL_VAR_FLAT = (1 << 12),
		MATERIAL_VAR_NOCULL = (1 << 13),
		MATERIAL_VAR_NOFOG = (1 << 14),
		MATERIAL_VAR_IGNOREZ = (1 << 15),
		MATERIAL_VAR_DECAL = (1 << 16),
		MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
		//	MATERIAL_VAR_UNUSED					  = (1 << 18),
		MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
		MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
		MATERIAL_VAR_TRANSLUCENT = (1 << 21),
		MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
		MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
		MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
		MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
		MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
		MATERIAL_VAR_HALFLAMBERT = (1 << 27),
		MATERIAL_VAR_WIREFRAME = (1 << 28),
		MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
		MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
		MATERIAL_VAR_VERTEXFOG = (1 << 31),

		// NOTE: Only add flags here that either should be read from
		// .vmts or can be set directly from client code. Other, internal
		// flags should to into the flag enum in IMaterialInternal.h
	};

	enum MaterialPropertyTypes_t
	{
		MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0, // bool
		MATERIAL_PROPERTY_OPACITY, // int (enum MaterialPropertyOpacityTypes_t)
		MATERIAL_PROPERTY_REFLECTIVITY, // vec3_t
		MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS // bool
	};

	enum PreviewImageRetVal_t
	{
		MATERIAL_PREVIEW_IMAGE_BAD = 0,
		MATERIAL_PREVIEW_IMAGE_OK,
		MATERIAL_NO_PREVIEW_IMAGE,
	};

	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	class IMaterial
	{
	public:
		// Get the name of the material.  This is a full path to 
		// the vmt file starting from "hl2/materials" (or equivalent) without
		// a file extension.
		virtual const char* GetName() const = 0;
		virtual const char* GetTextureGroupName() const = 0;

		// Get the preferred size/bitDepth of a preview image of a material.
		// This is the sort of image that you would use for a thumbnail view
		// of a material, or in WorldCraft until it uses materials to render.
		// separate this for the tools maybe
		virtual PreviewImageRetVal_t GetPreviewImageProperties(int* width, int* height, ImageFormat* imageFormat, bool* isTranslucent) const = 0;

		// Get a preview image at the specified width/height and bitDepth.
		// Will do resampling if necessary.(not yet!!! :) )
		// Will do color format conversion. (works now.)
		virtual PreviewImageRetVal_t GetPreviewImage(unsigned char* data, int width, int height, ImageFormat imageFormat) const = 0;
		// 
		virtual int GetMappingWidth() = 0;
		virtual int GetMappingHeight() = 0;

		virtual int GetNumAnimationFrames() = 0;

		// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
		virtual bool InMaterialPage() = 0;
		virtual void GetMaterialOffset(float* pOffset) = 0;
		virtual void GetMaterialScale(float* pScale) = 0;
		virtual IMaterial* GetMaterialPage() = 0;

		// find a vmt variable.
		// This is how game code affects how a material is rendered.
		// The game code must know about the params that are used by
		// the shader for the material that it is trying to affect.
		virtual IMaterialVar* FindVar(const char* varName, bool* found, bool complain = true) = 0;

		// The user never allocates or deallocates materials.  Reference counting is
		// used instead.  Garbage collection is done upon a call to 
		// IMaterialSystem::UncacheUnusedMaterials.
		virtual void IncrementReferenceCount() = 0;
		virtual void DecrementReferenceCount() = 0;

		void AddRef() { IncrementReferenceCount(); }
		void Release() { DecrementReferenceCount(); }

		// Each material is assigned a number that groups it with like materials
		// for sorting in the application.
		virtual int GetEnumerationID() const = 0;

		virtual void GetLowResColorSample(float s, float t, float* color) const = 0;

		// This computes the state snapshots for this material
		virtual void RecomputeStateSnapshots() = 0;

		// Are we translucent?
		virtual bool IsTranslucent() = 0;

		// Are we alphatested?
		virtual bool IsAlphaTested() = 0;

		// Are we vertex lit?
		virtual bool IsVertexLit() = 0;

		// Gets the vertex format
		virtual VertexFormat_t GetVertexFormat() const = 0;

		// returns true if this material uses a material proxy
		virtual bool HasProxy() const = 0;

		virtual bool UsesEnvCubemap() = 0;

		virtual bool NeedsTangentSpace() = 0;

		virtual bool NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
		virtual bool NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

		// returns true if the shader doesn't do skinning itself and requires
		// the data that is sent to it to be preskinned.
		virtual bool NeedsSoftwareSkinning() = 0;

		// Apply constant color or alpha modulation
		virtual void AlphaModulate(float alpha) = 0;
		virtual void ColorModulate(float r, float g, float b) = 0;

		// Material Var flags...
		virtual void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
		virtual bool GetMaterialVarFlag(MaterialVarFlags_t flag) = 0;

		// Gets material reflectivity
		virtual void GetReflectivity(Vector& reflect) = 0;

		// Gets material property flags
		virtual bool GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

		// Is the material visible from both sides?
		virtual bool IsTwoSided() = 0;

		// Sets the shader associated with the material
		virtual void SetShader(const char* pShaderName) = 0;

		// Can't be const because the material might have to precache itself.
		virtual int GetNumPasses() = 0;

		// Can't be const because the material might have to precache itself.
		virtual int GetTextureMemoryBytes() = 0;

		// Meant to be used with materials created using CreateMaterial
		// It updates the materials to reflect the current values stored in the material vars
		virtual void Refresh() = 0;

		// GR - returns true is material uses lightmap alpha for blending
		virtual bool NeedsLightmapBlendAlpha() = 0;

		// returns true if the shader doesn't do lighting itself and requires
		// the data that is sent to it to be prelighted
		virtual bool NeedsSoftwareLighting() = 0;

		// Gets at the shader parameters
		virtual int ShaderParamCount() const = 0;
		virtual IMaterialVar** GetShaderParams() = 0;

		// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
		// the material can't be found.
		virtual bool IsErrorMaterial() const = 0;

		virtual void Unused() = 0;

		// Gets the current alpha modulation
		virtual float GetAlphaModulation() = 0;
		virtual void GetColorModulation(float* r, float* g, float* b) = 0;

		// Is this translucent given a particular alpha modulation?
		virtual bool IsTranslucentUnderModulation(float fAlphaModulation = 1.0F) const = 0;

		// fast find that stores the index of the found var in the string table in local cache
		virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;

		// Sets new VMT shader parameters for the material
		virtual void SetShaderAndParams(void* pKeyValues) = 0;
		virtual const char* GetShaderName() const = 0;

		virtual void DeleteIfUnreferenced() = 0;

		virtual bool IsSpriteCard() = 0;

		virtual void CallBindProxy(void* proxyData) = 0;

		virtual void RefreshPreservingMaterialVars() = 0;

		virtual bool WasReloadedFromWhitelist() = 0;

		virtual bool SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;

		virtual int GetReferenceCount() const = 0;

	};
}
