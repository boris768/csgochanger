#pragma once

#include "IStudioRender.h"
#include "SourceEngine/IClientRenderable.hpp"

//VModelInfoClient

namespace SourceEngine
{
	class IVModelInfo
	{
		enum RenderableTranslucencyType_t
		{
			RENDERABLE_IS_OPAQUE = 0,
			RENDERABLE_IS_TRANSLUCENT,
			RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
		};
	public:
		virtual							~IVModelInfo() { }
		virtual const model_t			*GetModel(int modelindex) const = 0; //1
																			 // Returns index of model by name
		virtual int						GetModelIndex(const char *name) const = 0;
		// Returns name of model
		virtual const char				*GetModelName(const model_t *model) const = 0;//3
		virtual void					*GetVCollide(const model_t *model) const = 0;
		virtual void					*GetVCollide(int modelindex) const = 0;
		virtual void					GetModelBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
		virtual	void					GetModelRenderBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
		virtual int						GetModelFrameCount(const model_t *model) const = 0;
		virtual int						GetModelType(const model_t *model) const = 0;
		virtual void					*GetModelExtraData(const model_t *model) = 0;
		virtual bool					ModelHasMaterialProxy(const model_t *model) const = 0;
		virtual bool					IsTranslucent(model_t const* model) const = 0;
		virtual bool					IsTranslucentTwoPass(const model_t *model) const = 0;
		virtual void					Unused0() {};//14
		virtual RenderableTranslucencyType_t ComputeTranslucencyType(const model_t *model, int nSkin, int nBody) = 0;
		virtual int						GetModelMaterialCount(const model_t* model) const = 0;
		virtual void					GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterials) = 0;
		virtual bool					IsModelVertexLit(const model_t *model) const = 0;//18
		virtual const char				*GetModelKeyValueText(const model_t *model) = 0;
		virtual bool					GetModelKeyValue(const model_t *model, /*CUtlBuffer*/ void* buf) = 0; // supports keyvalue blocks in submodels
		virtual float					GetModelRadius(const model_t *model) = 0;

		virtual const studiohdr_t		*FindModel(const studiohdr_t *pStudioHdr, void **cache, const char *modelname) const = 0;
		virtual const studiohdr_t		*FindModel(void *cache) const = 0;
		virtual	virtualmodel_t			*GetVirtualModel(const studiohdr_t *pStudioHdr) const = 0;
		virtual byte					*GetAnimBlock(const studiohdr_t *pStudioHdr, int iBlock) const = 0;
		virtual bool					HasAnimBlockBeenPreloaded(studiohdr_t const*, int) const = 0;//26

		// Available on client only!!!
		virtual void					GetModelMaterialColorAndLighting(const model_t *model, Vector const& origin,
			QAngle const& angles, void* pTrace,
			Vector& lighting, Vector& matColor) = 0;
		virtual void					GetIlluminationPoint(const model_t *model, void *pRenderable, Vector const& origin,
			QAngle const& angles, Vector* pLightingCenter) = 0;

		virtual int						GetModelContents(int modelIndex) const = 0;
		virtual studiohdr_t				*GetStudioModel(const model_t *mod) = 0;//30
		virtual int						GetModelSpriteWidth(const model_t *model) const = 0;
		virtual int						GetModelSpriteHeight(const model_t *model) const = 0;

		// Sets/gets a map-specified fade range (client only)
		virtual void					SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
		virtual void					GetLevelScreenFadeRange(float *pMinArea, float *pMaxArea) const = 0;

		// Sets/gets a map-specified per-view fade range (client only)
		virtual void					SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;

		// Computes fade alpha based on distance fade + screen fade (client only)
		virtual unsigned char			ComputeLevelScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
		virtual unsigned char			ComputeViewScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;

		// both client and server
		virtual int						GetAutoplayList(const studiohdr_t *pStudioHdr, unsigned short **pAutoplayList) const = 0;

		// Gets a virtual terrain collision model (creates if necessary)
		// NOTE: This may return NULL if the terrain model cannot be virtualized
		virtual void					*GetCollideForVirtualTerrain(int index) = 0;
		virtual bool					IsUsingFBTexture(const model_t *model, int nSkin, int nBody, void /*IClientRenderable*/ *pClientRenderable) const = 0;
		virtual const model_t			*FindOrLoadModel(const char *name) const = 0;
		virtual MDLHandle_t				GetCacheHandle(const model_t *model) const = 0;
		// Returns planes of non-nodraw brush model surfaces
		virtual int						GetBrushModelPlaneCount(const model_t *model) const = 0;
		virtual void					GetBrushModelPlane(const model_t *model, int nIndex, void* plane, Vector *pOrigin) const = 0;
		virtual int						GetSurfacepropsForVirtualTerrain(int index) = 0;
		virtual bool					UsesEnvCubemap(const model_t *model) const = 0;
		virtual bool					UsesStaticLighting(const model_t *model) const = 0;

		studiohdr_t* hack_GetStudioModel(const model_t* mod);
	};

	inline studiohdr_t* IVModelInfo::hack_GetStudioModel(const model_t* mod)
	{
		using GetStudioModel_t = studiohdr_t * (__thiscall*) (void*, const model_t*);
		return SourceEngine::CallVFunction<GetStudioModel_t>(this, 32)(this, mod); // Reverse SetWeaponModel.
	}
}
