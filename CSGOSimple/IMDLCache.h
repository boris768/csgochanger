#pragma once
#include "IStudioRender.h"

namespace SourceEngine
{
	enum MDLCacheDataType_t
	{
		// Callbacks to get called when data is loaded or unloaded for these:
		MDLCACHE_STUDIOHDR = 0,
		MDLCACHE_STUDIOHWDATA,
		MDLCACHE_VCOLLIDE,

		// Callbacks NOT called when data is loaded or unloaded for these:
		MDLCACHE_ANIMBLOCK,
		MDLCACHE_VIRTUALMODEL,
		MDLCACHE_VERTEXES,
		MDLCACHE_DECODEDANIMBLOCK,
	};


	class IMDLCacheNotify
	{
	public:
		// Called right after the data is loaded
		virtual void OnDataLoaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;

		// Called right before the data is unloaded
		virtual void OnDataUnloaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
	};


	class IMDLCache : IAppSystem
	{
	public:
		virtual void SetCacheNotify(IMDLCacheNotify *pNotify) = 0;
		virtual MDLHandle_t FindMDL(const char *pMDLRelativePath) = 0; //10
	};
}