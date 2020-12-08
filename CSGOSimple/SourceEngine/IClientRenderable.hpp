#pragma once

#include "QAngle.hpp"
#include "Vector.hpp"

namespace SourceEngine
{

    typedef unsigned short ClientShadowHandle_t;
    typedef unsigned short ClientRenderHandle_t;
    typedef unsigned short ModelInstanceHandle_t;
    typedef unsigned char uint8;
	enum
	{
		MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
	};

    class matrix3x4_t;
    class IClientUnknown;
	struct model_t
	{
		void* fnHandle; //0x0000
		char name[260]; //0x0004
		int nLoadFlags; //0x0108
		int nServerCount; //0x010C
		int type; //0x0110
		int flags; //0x0114
		Vector mins; //0x0118
		Vector maxs; //0x0124
		float radius; //0x0130
		char pad_0134[28]; //0x0134
	};

    class IClientRenderable
    {
    public:
        virtual IClientUnknown*            GetIClientUnknown() = 0;
        virtual Vector const&              GetRenderOrigin() = 0;
        virtual QAngle const&              GetRenderAngles() = 0;
        virtual bool                       ShouldDraw() = 0;
        virtual int                        GetRenderFlags() = 0; // ERENDERFLAGS_xxx
        virtual void                       Unused() const {}
        virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
        virtual ClientRenderHandle_t&      RenderHandle() = 0;
        virtual const model_t*             GetModel() const = 0;
        virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/ &instance) = 0;
        virtual int                        GetBody() = 0;
        virtual void                       GetColorModulation(float* color) = 0;
        virtual bool                       LODTest() = 0;
        virtual bool                       SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
        virtual void                       SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights) = 0;
        virtual void                       DoAnimationEvents() = 0;
        virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
        virtual void                       GetRenderBounds(Vector& mins, Vector& maxs) = 0;
        virtual void                       GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
        virtual void                       GetShadowRenderBounds(Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType) = 0;
        virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
        virtual bool                       GetShadowCastDistance(float *pDist, int /*ShadowType_t*/ shadowType) const = 0;
        virtual bool                       GetShadowCastDirection(Vector *pDirection, int /*ShadowType_t*/ shadowType) const = 0;
        virtual bool                       IsShadowDirty() = 0;
        virtual void                       MarkShadowDirty(bool bDirty) = 0;
        virtual IClientRenderable*         GetShadowParent() = 0;
        virtual IClientRenderable*         FirstShadowChild() = 0;
        virtual IClientRenderable*         NextShadowPeer() = 0;
        virtual int /*ShadowType_t*/       ShadowCastType() = 0;
        virtual void                       CreateModelInstance() = 0;
        virtual ModelInstanceHandle_t      GetModelInstance() = 0;
        virtual const matrix3x4_t&         RenderableToWorldTransform() = 0;
        virtual int                        LookupAttachment(const char *pAttachmentName) = 0;
        virtual   bool                     GetAttachment(int number, Vector &origin, QAngle &angles) = 0;
        virtual bool                       GetAttachment(int number, matrix3x4_t &matrix) = 0;
        virtual float*                     GetRenderClipPlane() = 0;
        virtual int                        GetSkin() = 0;
        virtual void                       OnThreadedDrawSetup() = 0;
        virtual bool                       UsesFlexDelayedWeights() = 0;
        virtual void                       RecordToolMessage() = 0;
        virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
        virtual uint8                      OverrideAlphaModulation(uint8 nAlpha) = 0;
        virtual uint8                      OverrideShadowAlphaModulation(uint8 nAlpha) = 0;

		//bool SetupBonesHack(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
		//{
		//	typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4_t*, int, int, float);
		//	return CallVFunction< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		//}
    };

	// subclassed by things which wish to add per-leaf data managed by the client leafsystem
	class CClientLeafSubSystemData
	{
	public:
		virtual ~CClientLeafSubSystemData(void)
		{
		}
	};
	struct SetupRenderInfo_t
	{
		int undefined = 0;
	};

	class IClientLeafSystem
	{
	public:
		virtual void CreateRenderableHandle(IClientRenderable* pRenderable, bool bRenderWithViewModels, int nType, int nModelType, unsigned int nSplitscreenEnabled = 0xFFFFFFFF) = 0; // = RENDERABLE_MODEL_UNKNOWN_TYPE ) = 0;
		virtual void RemoveRenderable(ClientRenderHandle_t handle) = 0;
		virtual void AddRenderableToLeaves(ClientRenderHandle_t renderable, int nLeafCount, unsigned short *pLeaves) = 0;
		virtual void SetTranslucencyType(ClientRenderHandle_t handle, int nType) = 0;

		/*RenderInFastReflections(unsigned short, bool)
		DisableShadowDepthRendering(unsigned short, bool)
		DisableCSMRendering(unsigned short, bool)*/
		virtual void pad0() = 0;
		virtual void pad1() = 0;
		virtual void pad2() = 0;

		virtual void AddRenderable(IClientRenderable* pRenderable, bool IsStaticProp, int Type, int nModelType, unsigned int nSplitscreenEnabled = 0xFFFFFFFF) = 0; //7
		virtual bool IsRenderableInPVS(IClientRenderable *pRenderable) = 0; //8
		virtual void SetSubSystemDataInLeaf(int leaf, int nSubSystemIdx, CClientLeafSubSystemData *pData) = 0;
		virtual CClientLeafSubSystemData *GetSubSystemDataInLeaf(int leaf, int nSubSystemIdx) = 0;
		virtual void SetDetailObjectsInLeaf(int leaf, int firstDetailObject, int detailObjectCount) = 0;
		virtual void GetDetailObjectsInLeaf(int leaf, int& firstDetailObject, int& detailObjectCount) = 0;
		virtual void DrawDetailObjectsInLeaf(int leaf, int frameNumber, int& nFirstDetailObject, int& nDetailObjectCount) = 0;
		virtual bool ShouldDrawDetailObjectsInLeaf(int leaf, int frameNumber) = 0;
		virtual void RenderableChanged(ClientRenderHandle_t handle) = 0;
		virtual void BuildRenderablesList(const SetupRenderInfo_t &info) = 0;
		virtual void CollateViewModelRenderables(void*) = 0;
		virtual void DrawStaticProps(bool enable) = 0;
		virtual void DrawSmallEntities(bool enable) = 0;
		virtual int AddShadow(ClientShadowHandle_t userId, unsigned short flags) = 0;
		virtual void RemoveShadow(int h) = 0;
		virtual void ProjectShadow(int handle, int nLeafCount, const int *pLeafList) = 0;
		virtual void ProjectFlashlight(int handle, int nLeafCount, const int *pLeafList) = 0;
	};
}