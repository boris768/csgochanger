#pragma once
#include "SourceEngine/IAppSystem.hpp"
#include "SourceEngine/QAngle.hpp"

namespace SourceEngine
{
	struct studiohdr_t;
#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

	class Quaternion;
	class VMatrix;
	struct Ray_t;
	//	class IMaterial;
	class ITexture;
	class matrix3x4_t;
	class Vector4D;
	//class Vector;
	class IMaterialSystem;
	class IMesh;
	class CFastTimer;
	class IPooledVBAllocator;
	struct mstudioanimdesc_t;
	struct mstudioseqdesc_t;
	struct mstudiobodyparts_t;
	struct mstudiotexture_t;
	struct virtualmodel_t;
	struct LightDesc_t;
	struct MaterialLightingState_t;
	struct StudioModelArrayInfo_t;
	struct StudioModelArrayInfo2_t;
	struct StudioArrayInstanceData_t;
	struct FlashlightState_t;
	struct GetTriangles_Output_t;
	struct StudioArrayData_t;
	struct MeshInstanceData_t;

	constexpr auto STUDIO_ACTIVITY = 0x1000;		// Has been updated at runtime to activity index
	enum
	{
		STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
		STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
		STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
		STUDIORENDER_DRAW_GROUP_MASK = 0x03,
		STUDIORENDER_DRAW_NO_FLEXES = 0x04,
		STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
		STUDIORENDER_DRAW_ACCURATETIME = 0x10,
		// Use accurate timing when drawing the model.
		STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
		STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
		STUDIORENDER_DRAW_WIREFRAME = 0x80,
		STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
		STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
		STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,
		STUDIORENDER_GENERATE_STATS = 0x8000,
	};

	enum LocalFlexController_t
	{
		// this isn't really an enum - its just a typed int. gcc will not accept it as a fwd decl, so we'll define one value
		DUMMY_FLEX_CONTROLLER = 0x7fffffff // make take 32 bits
	};


	class virtualgroup_t
	{
	public:
		virtualgroup_t()noexcept { cache = nullptr; };
		// tool dependant.  In engine this is a model_t, in tool it's a direct pointer
		void* cache;
		// converts cache entry into a usable studiohdr_t *
		studiohdr_t* GetStudioHdr() const
		{
			return (studiohdr_t*)cache;
		}

		CUtlVector< int > boneMap;				// maps global bone to local bone
		CUtlVector< int > masterBone;			// maps local bone to global bone
		CUtlVector< int > masterSeq;			// maps local sequence to master sequence
		CUtlVector< int > masterAnim;			// maps local animation to master animation
		CUtlVector< int > masterAttachment;	// maps local attachment to global
		CUtlVector< int > masterPose;			// maps local pose parameter to global
		CUtlVector< int > masterNode;			// maps local transition nodes to global
	};

	struct virtualsequence_t
	{
		int	flags;
		int activity;
		int group;
		int index;
	};

	struct virtualgeneric_t
	{
		int group;
		int index;
	};

	struct virtualmodel_t
	{
		void AppendSequences(int group, const studiohdr_t* pStudioHdr);
		void AppendAnimations(int group, const studiohdr_t* pStudioHdr);
		void AppendAttachments(int ground, const studiohdr_t* pStudioHdr);
		void AppendPoseParameters(int group, const studiohdr_t* pStudioHdr);
		void AppendBonemap(int group, const studiohdr_t* pStudioHdr);
		void AppendNodes(int group, const studiohdr_t* pStudioHdr);
		void AppendTransitions(int group, const studiohdr_t* pStudioHdr);
		void AppendIKLocks(int group, const studiohdr_t* pStudioHdr);
		void AppendModels(int group, const studiohdr_t* pStudioHdr);
		void UpdateAutoplaySequences(const studiohdr_t* pStudioHdr);

		virtualgroup_t* pAnimGroup(int animation) { return &m_group[m_anim[animation].group]; } // Note: user must manage mutex for this
		virtualgroup_t* pSeqGroup(int sequence)
		{
			// Check for out of range access that is causing crashes on some servers.
			// Perhaps caused by sourcemod bugs. Typical sequence in these cases is ~292
			// when the count is 234. Using unsigned math allows for free range
			// checking against zero.
			if ((unsigned)sequence >= (unsigned)m_seq.Count())
			{
				Assert(0);
				return 0;
			}
			return &m_group[m_seq[sequence].group];
		} // Note: user must manage mutex for this

		void* m_Lock;

		CUtlVector< virtualsequence_t > m_seq;
		CUtlVector< virtualgeneric_t > m_anim;
		CUtlVector< virtualgeneric_t > m_attachment;
		CUtlVector< virtualgeneric_t > m_pose;
		CUtlVector< virtualgroup_t > m_group;
		CUtlVector< virtualgeneric_t > m_node;
		CUtlVector< virtualgeneric_t > m_iklock;
		CUtlVector< unsigned short > m_autoplaySequences;
	};

	
	struct StudioRenderConfig_t
	{
		float fEyeShiftX; // eye X position
		float fEyeShiftY; // eye Y position
		float fEyeShiftZ; // eye Z position
		float fEyeSize; // adjustment to iris textures
		float fEyeGlintPixelWidthLODThreshold;

		int maxDecalsPerModel;
		int drawEntities;
		int skin;
		int fullbright;

		bool bEyeMove : 1; // look around
		bool bSoftwareSkin : 1;
		bool bNoHardware : 1;
		bool bNoSoftware : 1;
		bool bTeeth : 1;
		bool bEyes : 1;
		bool bFlex : 1;
		bool bWireframe : 1;
		bool bDrawNormals : 1;
		bool bDrawTangentFrame : 1;
		bool bDrawZBufferedWireframe : 1;
		bool bSoftwareLighting : 1;
		bool bShowEnvCubemapOnly : 1;
		bool bWireframeDecals : 1;

		// Reserved for future use
		int m_nReserved[4];
	};

	class CCycleCount
	{
		friend class CFastTimer;

	public:
		CCycleCount();
		CCycleCount(uint64_t cycles);

		void Sample();
		// Sample the clock. This takes about 34 clocks to execute (or 26,000 calls per millisecond on a P900).

		void Init(); // Set to zero.
		void Init(float initTimeMsec);
		void Init(double initTimeMsec) { Init(static_cast<float>(initTimeMsec)); }
		void Init(uint64_t cycles);
		bool IsLessThan(CCycleCount const& other) const; // Compare two counts.

		// Convert to other time representations. These functions are slow, so it's preferable to call them
		// during display rather than inside a timing block.
		unsigned long GetCycles() const;
		uint64_t GetLongCycles() const;

		unsigned long GetMicroseconds() const;
		uint64_t GetUlMicroseconds() const;
		double GetMicrosecondsF() const;
		void SetMicroseconds(unsigned long nMicroseconds);

		unsigned long GetMilliseconds() const;
		double GetMillisecondsF() const;

		double GetSeconds() const;

		CCycleCount& operator+=(CCycleCount const& other);

		// dest = rSrc1 + rSrc2
		static void Add(CCycleCount const& rSrc1, CCycleCount const& rSrc2, CCycleCount& dest);
		// Add two samples together.

		// dest = rSrc1 - rSrc2
		static void Sub(CCycleCount const& rSrc1, CCycleCount const& rSrc2, CCycleCount& dest);
		// Add two samples together.

		static uint64_t GetTimestamp();

		uint64_t m_Int64;
	};

	class CClockSpeedInit
	{
	public:
		CClockSpeedInit()
		{
			Init();
		}

		static void Init();
	};

	class CFastTimer
	{
	public:
		// These functions are fast to call and should be called from your sampling code.
		void Start();
		void End();

		const CCycleCount& GetDuration() const; // Get the elapsed time between Start and End calls.
		CCycleCount GetDurationInProgress() const; // Call without ending. Not that cheap.

		// Return number of cycles per second on this processor.
		static inline uint64_t GetClockSpeed();

	private:
		CCycleCount m_Duration;
#ifdef DEBUG_FASTTIMER
		bool m_bRunning;		// Are we currently running?
#endif
	};

	class RadianEuler
	{
	public:
		RadianEuler() : x(0), y(0), z(0)
		{
		}

		RadianEuler(float X, float Y, float Z)
		{
			x = X;
			y = Y;
			z = Z;
		}

		RadianEuler(Quaternion const& q); // evil auto type promotion!!!
		RadianEuler(QAngle const& angles); // evil auto type promotion!!!

		// Initialization
		void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
		{
			x = ix;
			y = iy;
			z = iz;
		}

		//	conversion to qangle
		QAngle ToQAngle() const;
		bool IsValid() const;
		void Invalidate();

		float* Base() { return &x; }
		const float* Base() const { return &x; }

		// array access...
		float operator[](size_t i) const;
		float& operator[](size_t i);

		float x, y, z;
	};

	class Quaternion // same data-layout as engine's vec4_t,
	{
		//		which is a float[4]
	public:
		Quaternion() : x(0), y(0), z(0), w(0)
		{
		}

		Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw)
		{
		}

		Quaternion(RadianEuler const& angle); // evil auto type promotion!!!

		void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f)
		{
			x = ix;
			y = iy;
			z = iz;
			w = iw;
		}

		bool IsValid() const;
		void Invalidate();

		bool operator==(const Quaternion& src) const;
		bool operator!=(const Quaternion& src) const;

		float* Base() { return (float*)this; }
		const float* Base() const { return (float*)this; }

		// array access...
		float operator[](size_t i) const;
		float& operator[](size_t i);

		float x, y, z, w;
	};


	struct mstudiobone_t
	{
		int sznameindex;
		char* pszName() const { return ((char *)this) + sznameindex; }
		int parent; // parent bone
		int bonecontroller[6]; // bone controller index, -1 == none
		// default values
		Vector pos;
		Quaternion quat;
		RadianEuler rot;
		// compression scale
		Vector posscale;
		Vector rotscale;

		matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int flags;
		int proctype;
		int procindex; // procedural rule
		mutable int physicsbone; // index into physically simulated bone
		void* pProcedure() const
		{
			if (procindex == 0) return nullptr;
			return static_cast<void *>(((byte *)this) + procindex);
		};
		int surfacepropidx; // index into string tablefor property name
		char* pszSurfaceProp() const { return ((char *)this) + surfacepropidx; }
		int GetSurfaceProp() const { return surfacepropLookup; }

		int contents; // See BSPFlags.h for the contents flags
		int surfacepropLookup; // this index must be cached by the loader, not saved in the file
		int unused[7]; // remove as appropriate
	};

	struct mstudiobonecontroller_t
	{
		int bone; // -1 == 0
		int type; // X, Y, Z, XR, YR, ZR, M
		float start;
		float end;
		int rest; // byte index value at rest
		int inputfield; // 0-3 user set controller, 4 mouth
		int unused[8];
	};

	struct mstudiobbox_t
	{
		int bone;
		int group; // intersection group
		Vector bbmin; // bounding box 
		Vector bbmax;
		int hitboxnameindex; // offset to the name of the hitbox.
		int pad[3];
		float radius;
		int pad2[4];

		char* getHitboxName()
		{
			return hitboxnameindex == 0 ? nullptr : ((char*)this) + hitboxnameindex;
		}
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;

		char* pszName() const
		{
			return ((char*)this) + sznameindex;
		}

		int numhitboxes;
		int hitboxindex;

		mstudiobbox_t* pHitbox(int i) const
		{
			return reinterpret_cast<mstudiobbox_t*>(((byte*)this) + hitboxindex) + i;
		}
	};

	struct mstudioposeparamdesc_t
	{
		int sznameindex;
		char* const pszName() const { return ((char*)this) + sznameindex; }
		int flags; // ????
		float start; // starting value
		float end; // ending value
		float loop; // looping range, 0 for no looping, 360 for rotations, etc.
	};

	struct mstudioanimblock_t
	{
		int datastart;
		int dataend;
	};

	struct mstudioanimsections_t
	{
		int animblock;
		int animindex;
	};

	struct mstudiomouth_t
	{
		int bone;
		Vector forward;
		int flexdesc;

	private:
		// No copy constructors allowed
		mstudiomouth_t(const mstudiomouth_t& vOther);
	};

	struct mstudioattachment_t
	{
		int sznameindex;

		char* pszName() const
		{
			return ((char*)this) + sznameindex;
		}

		unsigned int flags;
		int localbone;
		matrix3x4_t local; // attachment point
		int unused[8];
	};

	struct mstudioflexdesc_t
	{
		int szFACSindex;

		char* pszFACS() const
		{
			return ((char*)this) + szFACSindex;
		}
	};

	struct mstudioflexcontroller_t
	{
		int sztypeindex;
		char* pszType() const { return ((char*)this) + sztypeindex; }
		int sznameindex;
		char* pszName() const { return ((char*)this) + sznameindex; }
		mutable int localToGlobal; // remapped at load time to master list
		float min;
		float max;
	};

	struct mstudioflexop_t
	{
		int op;

		union
		{
			int index;
			float value;
		} d;
	};

	struct mstudioflexrule_t
	{
		int flex;
		int numops;
		int opindex;
		mstudioflexop_t* iFlexOp(int i) const { return (mstudioflexop_t*)(((byte*)this) + opindex) + i; };
	};

	struct mstudiomodelgroup_t
	{
		int szlabelindex; // textual name
		char* pszLabel() const { return ((char*)this) + szlabelindex; }
		int sznameindex; // file name
		char* pszName() const { return ((char*)this) + sznameindex; }
	};


	struct mstudioiklink_t
	{
		int bone;
		Vector kneeDir; // ideal bending direction (per link, if applicable)
		Vector unused0; // unused

	private:
		// No copy constructors allowed
		mstudioiklink_t(const mstudioiklink_t& vOther);
	};

	struct mstudioikchain_t
	{
		int sznameindex;
		char* pszName() const { return ((char*)this) + sznameindex; }
		int linktype;
		int numlinks;
		int linkindex;
		mstudioiklink_t* pLink(int i) const { return (mstudioiklink_t*)(((byte*)this) + linkindex) + i; };
		// FIXME: add unused entries
	};

	struct mstudioiklock_t
	{
		int chain;
		float flPosWeight;
		float flLocalQWeight;
		int flags;

		int unused[4];
	};


	//-----------------------------------------------------------------------------
	// The component of the bone used by mstudioboneflexdriver_t
	//-----------------------------------------------------------------------------
	enum StudioBoneFlexComponent_t
	{
		STUDIO_BONE_FLEX_INVALID = -1,
		// Invalid
		STUDIO_BONE_FLEX_TX = 0,
		// Translate X
		STUDIO_BONE_FLEX_TY = 1,
		// Translate Y
		STUDIO_BONE_FLEX_TZ = 2 // Translate Z
	};


	//-----------------------------------------------------------------------------
	// Component is one of Translate X, Y or Z [0,2] (StudioBoneFlexComponent_t)
	//-----------------------------------------------------------------------------
	struct mstudioboneflexdrivercontrol_t
	{
		int m_nBoneComponent; // Bone component that drives flex, StudioBoneFlexComponent_t
		int m_nFlexControllerIndex; // Flex controller to drive
		float m_flMin; // Min value of bone component mapped to 0 on flex controller
		float m_flMax; // Max value of bone component mapped to 1 on flex controller

		// No copy constructors allowed
		mstudioboneflexdrivercontrol_t(mstudioboneflexdrivercontrol_t&& v) = delete;
		mstudioboneflexdrivercontrol_t(const mstudioboneflexdrivercontrol_t& vOther) = delete;
		mstudioboneflexdrivercontrol_t& operator= (const mstudioboneflexdrivercontrol_t& v) = delete;
	};

	struct mstudioboneflexdriver_t
	{
		int m_nBoneIndex; // Bone to drive flex controller
		int m_nControlCount; // Number of flex controllers being driven
		int m_nControlIndex; // Index into data where controllers are (relative to this)

		mstudioboneflexdrivercontrol_t* pBoneFlexDriverControl(const int i) const
		{
			Assert(i >= 0 && i < m_nControlCount);
			return (mstudioboneflexdrivercontrol_t*)(((byte*)this) + m_nControlIndex) + i;
		}

		int unused[3];

		mstudioboneflexdriver_t(mstudioboneflexdriver_t&& v) = delete;
		mstudioboneflexdriver_t(const mstudioboneflexdriver_t& vOther) = delete;
		mstudioboneflexdriver_t& operator= (const mstudioboneflexdriver_t& v) = delete;
		
	};

	struct mstudioflexcontrollerui_t
	{
		int sznameindex;
		char* pszName() const { return ((char*)this) + sznameindex; }

		// These are used like a union to save space
		// Here are the possible configurations for a UI controller
		//
		// SIMPLE NON-STEREO:	0: control	1: unused	2: unused
		// STEREO:				0: left		1: right	2: unused
		// NWAY NON-STEREO:		0: control	1: unused	2: value
		// NWAY STEREO:			0: left		1: right	2: value

		int szindex0;
		int szindex1;
		int szindex2;

		const mstudioflexcontroller_t* pController(void) const
		{
			return !stereo ? (mstudioflexcontroller_t*)((char*)this + szindex0) : nullptr;
		}

		char* const pszControllerName() const { return !stereo ? pController()->pszName() : nullptr; }
		//int			controllerIndex(const CStudioHdr& cStudioHdr) const;

		const mstudioflexcontroller_t* pLeftController(void) const
		{
			return stereo ? (mstudioflexcontroller_t*)((char*)this + szindex0) : nullptr;
		}

		char* const pszLeftName(void) const { return stereo ? pLeftController()->pszName() : nullptr; }
		//inline int			leftIndex(const CStudioHdr& cStudioHdr) const;

		const mstudioflexcontroller_t* pRightController(void) const
		{
			return stereo ? (mstudioflexcontroller_t*)((char*)this + szindex1) : nullptr;
		}

		char* const pszRightName(void) const { return stereo ? pRightController()->pszName() : nullptr; }
		//inline int			rightIndex(const CStudioHdr& cStudioHdr) const;

		//inline const mstudioflexcontroller_t* pNWayValueController(void) const
		//{
		//	return remaptype == FLEXCONTROLLER_REMAP_NWAY ? (mstudioflexcontroller_t*)((char*)this + szindex2) : nullptr;
		//}
		//inline char* const	pszNWayValueName(void) const { return remaptype == FLEXCONTROLLER_REMAP_NWAY ? pNWayValueController()->pszName() : nullptr; }
		//inline int			nWayValueIndex(const CStudioHdr& cStudioHdr) const;

		// Number of controllers this ui description contains, 1, 2 or 3
		//inline int			Count() const { return (stereo ? 2 : 1) + (remaptype == FLEXCONTROLLER_REMAP_NWAY ? 1 : 0); }
		inline const mstudioflexcontroller_t* pController(int index) const;

		unsigned char remaptype; // See the FlexControllerRemapType_t enum
		bool stereo; // Is this a stereo control?
		byte unused[2];
	};

	struct mstudiolinearbone_t
	{
		int numbones;

		int flagsindex;

		int flags(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((int*)(((byte*)this) + flagsindex) + i);
		};

		int* pflags(int i)
		{
			Assert(i >= 0 && i < numbones);
			return ((int*)(((byte*)this) + flagsindex) + i);
		};

		int parentindex;

		int parent(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((int*)(((byte*)this) + parentindex) + i);
		};

		int posindex;

		Vector pos(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((Vector*)(((byte*)this) + posindex) + i);
		};

		int quatindex;

		Quaternion quat(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((Quaternion*)(((byte*)this) + quatindex) + i);
		};

		int rotindex;

		RadianEuler rot(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((RadianEuler*)(((byte*)this) + rotindex) + i);
		};

		int posetoboneindex;

		matrix3x4_t poseToBone(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((matrix3x4_t*)(((byte*)this) + posetoboneindex) + i);
		};

		int posscaleindex;

		Vector posscale(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((Vector*)(((byte*)this) + posscaleindex) + i);
		};

		int rotscaleindex;

		Vector rotscale(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((Vector*)(((byte*)this) + rotscaleindex) + i);
		};

		int qalignmentindex;

		Quaternion qalignment(int i) const
		{
			Assert(i >= 0 && i < numbones);
			return *((Quaternion*)(((byte*)this) + qalignmentindex) + i);
		};

		int unused[6];
		mstudiolinearbone_t(mstudiolinearbone_t&& v) = delete;
		mstudiolinearbone_t(const mstudiolinearbone_t& vOther) = delete;
		mstudiolinearbone_t& operator= (const mstudiolinearbone_t& v) = delete;
	};

	struct mstudiosrcbonetransform_t
	{
		int sznameindex;
		const char* pszName() const { return ((char*)this) + sznameindex; }
		matrix3x4_t pretransform;
		matrix3x4_t posttransform;
	};

	struct mstudioevent_t
	{
		float				cycle;
		int					event;
		int					type;
		const char* pszOptions() const { return options; }
		char				options[64];

		int					szeventindex;
		char* pszEventName() const { return ((char*)this) + szeventindex; }
	};

	struct mstudioactivitymodifier_t
	{
		int					sznameindex;
		char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : nullptr; }
	};
	
	struct mstudioautolayer_t
	{
		//private:
		short				iSequence;
		short				iPose;
		//public:
		int					flags;
		float				start;	// beginning of influence
		float				peak;	// start of full influence
		float				tail;	// end of full influence
		float				end;	// end of all influence
	};

	struct mstudiomovement_t
	{
		int					endframe;
		int					motionflags;
		float				v0;			// velocity at start of block
		float				v1;			// velocity at end of block
		float				angle;		// YAW rotation at end of this blocks movement
		Vector				vector;		// movement vector relative to this blocks initial angle
		Vector				position;	// relative to start of animation???

	private:
		// No copy constructors allowed
		mstudiomovement_t(const mstudiomovement_t& vOther);
	};

	struct mstudioanim_t
	{
		byte				bone;
		byte				flags;		// weighing options

		// valid for animating data only
		byte* pData() const { return (((byte*)this) + sizeof(struct mstudioanim_t)); };
		short				nextoffset;
		mstudioanim_t* pNext() const {
			if (nextoffset != 0) 
				return  (mstudioanim_t*)(((byte*)this) + nextoffset);
			return nullptr; };
	};

	struct mstudioikrule_t
	{
		int			index;

		int			type;
		int			chain;

		int			bone;

		int			slot;	// iktarget slot.  Usually same as chain.
		float		height;
		float		radius;
		float		floor;
		Vector		pos;
		Quaternion	q;

		int			compressedikerrorindex;
		//inline mstudiocompressedikerror_t* pCompressedError() const { return (mstudiocompressedikerror_t*)(((byte*)this) + compressedikerrorindex); };
		int			unused2;

		int			iStart;
		int			ikerrorindex;
		//inline mstudioikerror_t* pError(int i) const { return  (ikerrorindex) ? (mstudioikerror_t*)(((byte*)this) + ikerrorindex) + (i - iStart) : NULL; };

		float		start;	// beginning of influence
		float		peak;	// start of full influence
		float		tail;	// end of full influence
		float		end;	// end of all influence

		float		unused3;	// 
		float		contact;	// frame footstep makes ground concact
		float		drop;		// how far down the foot should drop when reaching for IK
		float		top;		// top of the foot box

		int			unused6;
		int			unused7;
		int			unused8;

		int			szattachmentindex;		// name of world attachment
		char* const pszAttachment(void) const { return ((char*)this) + szattachmentindex; }

		int			unused[7];

		mstudioikrule_t(mstudioikrule_t&& v) = delete;
		mstudioikrule_t(const mstudioikrule_t& vOther) = delete;
		mstudioikrule_t& operator= (const mstudioikrule_t& v) = delete;
	};
	
	struct mstudioanimdesc_t
	{
		int					baseptr;
		studiohdr_t* pStudiohdr() const { return (studiohdr_t*)(((byte*)this) + baseptr); }

		int					sznameindex;
		char* pszName() const { return ((char*)this) + sznameindex; }

		float				fps;		// frames per second	
		int					flags;		// looping/non-looping flags

		int					numframes;

		// piecewise movement
		int					nummovements;
		int					movementindex;
		mstudiomovement_t* pMovement(int i) const { return (mstudiomovement_t*)(((byte*)this) + movementindex) + i; };

		int					unused1[6];			// remove as appropriate (and zero if loading older versions)	

		int					animblock;
		int					animindex;	 // non-zero when anim data isn't in sections
		mstudioanim_t* pAnimBlock(int block, int index) const; // returns pointer to a specific anim block (local or external)
		mstudioanim_t* pAnim(int* piFrame, float& flStall) const; // returns pointer to data and new frame index
		mstudioanim_t* pAnim(int* piFrame) const; // returns pointer to data and new frame index

		int					numikrules;
		int					ikruleindex;	// non-zero when IK data is stored in the mdl
		int					animblockikruleindex; // non-zero when IK data is stored in animblock file
		mstudioikrule_t* pIKRule(int i) const;

		int					numlocalhierarchy;
		int					localhierarchyindex;
		//mstudiolocalhierarchy_t* pHierarchy(int i) const;

		int					sectionindex;
		int					sectionframes; // number of frames used in each fast lookup section, zero if not used
		mstudioanimsections_t* const pSection(int i) const { return (mstudioanimsections_t*)(((byte*)this) + sectionindex) + i; }

		short				zeroframespan;	// frames per span
		short				zeroframecount; // number of spans
		int					zeroframeindex;
		byte* pZeroFrameData() const { if (zeroframeindex) return (((byte*)this) + zeroframeindex); else return NULL; };
		mutable float		zeroframestalltime;		// saved during read stalls
		
		mstudioanimdesc_t(mstudioanimdesc_t&& v) = delete;
		mstudioanimdesc_t(const mstudioanimdesc_t& vOther) = delete;
		mstudioanimdesc_t& operator= (const mstudioanimdesc_t& v) = delete;
	};
	
	struct mstudioseqdesc_t
	{
		int					baseptr;
		studiohdr_t* pStudiohdr() const { return (studiohdr_t*)(((byte*)this) + baseptr); }

		int					szlabelindex;
		char* pszLabel() const { return ((char*)this) + szlabelindex; }

		int					szactivitynameindex;
		char* pszActivityName() const { return ((char*)this) + szactivitynameindex; }

		int					flags;		// looping/non-looping flags

		int					activity;	// initialized at loadtime to game DLL values
		int					actweight;

		int					numevents;
		int					eventindex;
		mstudioevent_t* pEvent(int i) const { Assert(i >= 0 && i < numevents); return (mstudioevent_t*)(((byte*)this) + eventindex) + i; };

		Vector				bbmin;		// per sequence bounding box
		Vector				bbmax;

		int					numblends;

		// Index into array of shorts which is groupsize[0] x groupsize[1] in length
		int					animindexindex;

		 int			anim(int x, int y) const
		{
			if (x >= groupsize[0])
			{
				x = groupsize[0] - 1;
			}

			if (y >= groupsize[1])
			{
				y = groupsize[1] - 1;
			}

			const int offset = y * groupsize[0] + x;
			short* blends = (short*)(((byte*)this) + animindexindex);
			const int value = (int)blends[offset];
			return value;
		}

		int					movementindex;	// [blend] float array for blended movement
		int					groupsize[2];
		int					paramindex[2];	// X, Y, Z, XR, YR, ZR
		float				paramstart[2];	// local (0..1) starting value
		float				paramend[2];	// local (0..1) ending value
		int					paramparent;

		float				fadeintime;		// ideal cross fate in time (0.2 default)
		float				fadeouttime;	// ideal cross fade out time (0.2 default)

		int					localentrynode;		// transition node at entry
		int					localexitnode;		// transition node at exit
		int					nodeflags;		// transition rules

		float				entryphase;		// used to match entry gait
		float				exitphase;		// used to match exit gait

		float				lastframe;		// frame that should generation EndOfSequence

		int					nextseq;		// auto advancing sequences
		int					pose;			// index of delta animation between end and nextseq

		int					numikrules;

		int					numautolayers;	//
		int					autolayerindex;
		mstudioautolayer_t* pAutolayer(int i) const { Assert(i >= 0 && i < numautolayers); return (mstudioautolayer_t*)(((byte*)this) + autolayerindex) + i; };

		int					weightlistindex;
		float* pBoneweight(int i) const { return ((float*)(((byte*)this) + weightlistindex) + i); };
		float		weight(int i) const { return *(pBoneweight(i)); };

		// FIXME: make this 2D instead of 2x1D arrays
		int					posekeyindex;
		float* pPoseKey(int iParam, int iAnim) const { return (float*)(((byte*)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
		float				poseKey(int iParam, int iAnim) const { return *(pPoseKey(iParam, iAnim)); }

		int					numiklocks;
		int					iklockindex;
		mstudioiklock_t* pIKLock(int i) const { Assert(i >= 0 && i < numiklocks); return (mstudioiklock_t*)(((byte*)this) + iklockindex) + i; };

		// Key values
		int					keyvalueindex;
		int					keyvaluesize;
		const char* KeyValueText() const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

		int					cycleposeindex;		// index of pose parameter to use as cycle index

		int					activitymodifierindex;
		int					numactivitymodifiers;
		mstudioactivitymodifier_t* pActivityModifier(int i) const { Assert(i >= 0 && i < numactivitymodifiers); return activitymodifierindex != 0 ? (mstudioactivitymodifier_t*)(((byte*)this) + activitymodifierindex) + i : NULL; };

		int					unused[5];		// remove/add as appropriate (grow back to 8 ints on version change!)

		mstudioseqdesc_t(mstudioseqdesc_t&& v) = delete;
		mstudioseqdesc_t(const mstudioseqdesc_t& vOther) = delete;
		mstudioseqdesc_t& operator= (const mstudioseqdesc_t& v) = delete;
	};

	struct studiohdr2_t
	{
		// NOTE: For forward compat, make sure any methods in this struct
		// are also available in studiohdr_t so no leaf code ever directly references
		// a studiohdr2_t structure
		int numsrcbonetransform;
		int srcbonetransformindex;

		int illumpositionattachmentindex;
		int IllumPositionAttachmentIndex() const { return illumpositionattachmentindex; }

		float flMaxEyeDeflection;
		float MaxEyeDeflection() const { return flMaxEyeDeflection != 0.0f ? flMaxEyeDeflection : 0.866f; }
		// default to cos(30) if not set

		int linearboneindex;

		mstudiolinearbone_t* pLinearBones() const
		{
			return (linearboneindex) ? (mstudiolinearbone_t*)(((byte*)this) + linearboneindex) : nullptr;
		}

		int sznameindex;
		char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : nullptr; }

		int m_nBoneFlexDriverCount;
		int m_nBoneFlexDriverIndex;

		mstudioboneflexdriver_t* pBoneFlexDriver(int i) const
		{
			Assert(i >= 0 && i < m_nBoneFlexDriverCount);
			return (mstudioboneflexdriver_t*)(((byte*)this) + m_nBoneFlexDriverIndex) + i;
		}

		int reserved[56];
	};

	struct studiohdr_t
	{
		int id;
		int version;
		long checksum; // this has to be the same in the phy and vtx files to load!
		const char* pszName() const { return name; }
		char name[64];
		int length;
		QAngle eyeposition; // ideal eye position
		QAngle illumposition; // illumination center
		QAngle hull_min; // ideal movement hull size
		QAngle hull_max;
		QAngle view_bbmin; // clipping bounding box
		QAngle view_bbmax;
		int flags;
		int numbones; // bones
		int boneindex;

		mstudiobone_t* pBone(size_t i) const
		{
			return (mstudiobone_t *)(((byte *)this) + boneindex + i);
		};
		int RemapSeqBone(int iSequence, int iLocalBone) const; // maps local sequence bone to global bone
		int RemapAnimBone(int iAnim, int iLocalBone) const; // maps local animations bone to global bone

		int numbonecontrollers; // bone controllers
		int bonecontrollerindex;
		//mstudiobonecontroller_t* pBonecontroller(int i) const { return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

		int numhitboxsets;
		int hitboxsetindex;

		// Look up hitbox set by index
		mstudiohitboxset_t* pHitboxSet(int i) const
		{
			return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex + i);
		};

		// Calls through to hitbox to determine size of specified set
		mstudiobbox_t* pHitbox(int i, int set) const
		{
			mstudiohitboxset_t const* s = pHitboxSet(set);
			if (!s)
				return nullptr;

			return s->pHitbox(i);
		};

		// Calls through to set to get hitbox count for set
		int iHitboxCount(int set) const
		{
			mstudiohitboxset_t const* s = pHitboxSet(set);
			if (!s)
				return 0;

			return s->numhitboxes;
		};

		// file local animations? and sequences
		//private:
		int numlocalanim; // animations/poses
		int localanimindex; // animation descriptions
		mstudioanimdesc_t* pLocalAnimdesc(int i) const
		{
			return (mstudioanimdesc_t*)(((byte*)this) + localanimindex) + i;
		};

		int numlocalseq; // sequences
		int localseqindex;

		mstudioseqdesc_t* pLocalSeqdesc(int i) const
		{
			return (mstudioseqdesc_t*)(((byte*)this) + localseqindex) + i;
		};

		//public:
		bool SequencesAvailable() const
		{
			if (numincludemodels == 0)
			{
				return true;
			}
			return (GetVirtualModel() != nullptr);
		}

		int GetNumSeq() const
		{
			if (numincludemodels == 0)
			{
				return numlocalseq;
			}

			const auto pVModel = (virtualmodel_t*)GetVirtualModel();
			Assert(pVModel);
			return pVModel->m_seq.Count();
		}
		mstudioanimdesc_t& pAnimdesc(int i) const
		{
			if (numincludemodels == 0)
			{
				return *pLocalAnimdesc(i);
			}
			virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
			assert(pVModel);

			virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_anim[i].group];
			const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
			assert(pStudioHdr);

			return *pStudioHdr->pLocalAnimdesc(pVModel->m_anim[i].index);
		}
		mstudioseqdesc_t& pSeqdesc(int i) const
		{
			if (numincludemodels == 0)
			{
				return *pLocalSeqdesc(i);
			}

			virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
			assert(pVModel);

			if (!pVModel)
			{
				return *pLocalSeqdesc(i);
			}

			virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_seq[i].group];
			const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
			assert(pStudioHdr);

			return *pStudioHdr->pLocalSeqdesc(pVModel->m_seq[i].index);
		}
		int iRelativeAnim(int baseseq, int relanim) const // maps seq local anim reference to global anim index
		{
			if (numincludemodels == 0)
			{
				return relanim;
			}

			virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
			Assert(pVModel);

			virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_seq[baseseq].group];

			return pGroup->masterAnim[relanim];
		}
		int iRelativeSeq(int baseseq, int relseq) const // maps seq local seq reference to global seq index
		{
			if (numincludemodels == 0)
			{
				return relseq;
			}

			virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
			Assert(pVModel);

			virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_seq[baseseq].group];

			return pGroup->masterSeq[relseq];
		}

		//private:
		mutable int activitylistversion; // initialization flag - have the sequences been indexed?
		mutable int eventsindexed;
		//public:
		int GetSequenceActivity(int iSequence);
		void SetSequenceActivity(int iSequence, int iActivity);
		int GetActivityListVersion();
		void SetActivityListVersion(int version) const;
		int GetEventListVersion();
		void SetEventListVersion(int version);

		// raw textures
		int numtextures;
		int textureindex;

		//mstudiotexture_t* pTexture(int i) const
		//{
		//	return (mstudiotexture_t*)(((byte*)this) + textureindex) + i;
		//};


		// raw textures search paths
		int numcdtextures;
		int cdtextureindex;

		char* pCdtexture(int i) const
		{
			return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i));
		};

		// replaceable textures tables
		int numskinref;
		int numskinfamilies;
		int skinindex;
		short* pSkinref(int i) const { return (short *)(((byte *)this) + skinindex) + i; };

		int numbodyparts;
		int bodypartindex;

		//mstudiobodyparts_t* pBodypart(int i) const
		//{
		//	return (mstudiobodyparts_t*)(((byte*)this) + bodypartindex) + i;
		//}

		int numlocalattachments;
		int localattachmentindex;

		mstudioattachment_t* pLocalAttachment(int i) const
		{
			Assert(i >= 0 && i < numlocalattachments);
			return (mstudioattachment_t*)(((byte*)this) + localattachmentindex) + i;
		};
		//public:
		int GetNumAttachments() const;
		const mstudioattachment_t& pAttachment(int i) const;
		int GetAttachmentBone(int i);
		// used on my tools in hlmv, not persistant
		void SetAttachmentBone(int iAttachment, int iBone);

		// animation node to animation node transition graph
		//private:
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;

		char* pszLocalNodeName(int iNode) const
		{
			Assert(iNode >= 0 && iNode < numlocalnodes);
			return (((char*)this) + *((int*)(((byte*)this) + localnodenameindex) + iNode));
		}

		byte* pLocalTransition(int i) const
		{
			Assert(i >= 0 && i < (numlocalnodes * numlocalnodes));
			return static_cast<byte*>(((byte*)this) + localnodeindex) + i;
		}

		int EntryNode(int iSequence);
		int ExitNode(int iSequence);
		char* pszNodeName(int iNode);
		int GetTransition(int iFrom, int iTo) const;

		int numflexdesc;
		int flexdescindex;

		mstudioflexdesc_t* pFlexdesc(int i) const
		{
			Assert(i >= 0 && i < numflexdesc);
			return (mstudioflexdesc_t*)(((byte*)this) + flexdescindex) + i;
		};

		int numflexcontrollers;
		int flexcontrollerindex;

		mstudioflexcontroller_t* pFlexcontroller(LocalFlexController_t i) const
		{
			Assert(numflexcontrollers == 0 || (i >= 0 && i < numflexcontrollers));
			return (mstudioflexcontroller_t*)(((byte*)this) + flexcontrollerindex) + i;
		};

		int numflexrules;
		int flexruleindex;

		mstudioflexrule_t* pFlexRule(int i) const
		{
			Assert(i >= 0 && i < numflexrules);
			return (mstudioflexrule_t*)(((byte*)this) + flexruleindex) + i;
		};

		int numikchains;
		int ikchainindex;

		mstudioikchain_t* pIKChain(int i) const
		{
			Assert(i >= 0 && i < numikchains);
			return (mstudioikchain_t*)(((byte*)this) + ikchainindex) + i;
		};

		int nummouths;
		int mouthindex;

		mstudiomouth_t* pMouth(int i) const
		{
			Assert(i >= 0 && i < nummouths);
			return (mstudiomouth_t*)(((byte*)this) + mouthindex) + i;
		};

		//private:
		int numlocalposeparameters;
		int localposeparamindex;

		mstudioposeparamdesc_t* pLocalPoseParameter(int i) const
		{
			Assert(i >= 0 && i < numlocalposeparameters);
			return (mstudioposeparamdesc_t*)(((byte*)this) + localposeparamindex) + i;
		};
		//public:
		int GetNumPoseParameters(void) const;
		const mstudioposeparamdesc_t& pPoseParameter(int i);
		int GetSharedPoseParameter(int iSequence, int iLocalPose) const;

		int surfacepropindex;
		char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropindex; }

		// Key values
		int keyvalueindex;
		int keyvaluesize;
		const char* KeyValueText() const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : nullptr; }

		int numlocalikautoplaylocks;
		int localikautoplaylockindex;

		mstudioiklock_t* pLocalIKAutoplayLock(int i) const
		{
			Assert(i >= 0 && i < numlocalikautoplaylocks);
			return (mstudioiklock_t*)(((byte*)this) + localikautoplaylockindex) + i;
		};

		int GetNumIKAutoplayLocks() const;
		const mstudioiklock_t& pIKAutoplayLock(int i);
		int CountAutoplaySequences() const;
		int CopyAutoplaySequences(unsigned short* pOut, int outCount) const;
		int GetAutoplayList(unsigned short** pOut) const;

		// The collision model mass that jay wanted
		float mass;
		int contents;

		// external animations, models, etc.
		int numincludemodels;
		int includemodelindex;

		mstudiomodelgroup_t* pModelGroup(int i) const
		{
			Assert(i >= 0 && i < numincludemodels);
			return (mstudiomodelgroup_t*)(((byte*)this) + includemodelindex) + i;
		};
		// implementation specific call to get a named model
		const studiohdr_t* FindModel(void** cache, char const* modelname) const;

		// implementation specific back pointer to virtual data
		mutable void* virtualModel;
		virtualmodel_t* GetVirtualModel() const
		{
			return (virtualmodel_t*)virtualModel;
		}

		// for demand loaded animation blocks
		int szanimblocknameindex;
		char* const pszAnimBlockName() const { return ((char*)this) + szanimblocknameindex; }
		int numanimblocks;
		int animblockindex;

		mstudioanimblock_t* pAnimBlock(int i) const
		{
			Assert(i > 0 && i < numanimblocks);
			return (mstudioanimblock_t*)(((byte*)this) + animblockindex) + i;
		};
		mutable void* animblockModel;
		byte* GetAnimBlock(int i) const;

		int bonetablebynameindex;
		const byte* GetBoneTableSortedByName() const { return (byte*)this + bonetablebynameindex; }

		// used by tools only that don't cache, but persist mdl's peer data
		// engine uses virtualModel to back link to cache pointers
		void* pVertexBase;
		void* pIndexBase;

		// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
		// this value is used to calculate directional components of lighting 
		// on static props
		byte constdirectionallightdot;

		// set during load of mdl data to track *desired* lod configuration (not actual)
		// the *actual* clamped root lod is found in studiohwdata
		// this is stored here as a global store to ensure the staged loading matches the rendering
		byte rootLOD;

		// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
		// to be set as root LOD:
		//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
		//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
		byte numAllowedRootLODs;

		byte unused[1];

		int unused4; // zero out if version < 47

		int numflexcontrollerui;
		int flexcontrolleruiindex;

		mstudioflexcontrollerui_t* pFlexControllerUI(int i) const
		{
			Assert(i >= 0 && i < numflexcontrollerui);
			return (mstudioflexcontrollerui_t*)(((byte*)this) + flexcontrolleruiindex) + i;
		}

		float flVertAnimFixedPointScale;

		float VertAnimFixedPointScale() const
		{
			constexpr auto STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE = 0x00200000;
			return (flags & STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE) ? flVertAnimFixedPointScale : 1.0f / 4096.0f;
		}

		int unused3[1];

		// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
		int studiohdr2index;
		studiohdr2_t* pStudioHdr2() const { return (studiohdr2_t*)(((byte*)this) + studiohdr2index); }

		// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
		int NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }

		const mstudiosrcbonetransform_t* SrcBoneTransform(int i) const
		{
			Assert(i >= 0 && i < NumSrcBoneTransforms());
			return (mstudiosrcbonetransform_t*)(((byte*)this) + pStudioHdr2()->srcbonetransformindex) + i;
		}

		int IllumPositionAttachmentIndex() const
		{
			return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0;
		}

		float MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; }
		// default to cos(30) if not set

		mstudiolinearbone_t* pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : nullptr; }

		int BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }

		const mstudioboneflexdriver_t* BoneFlexDriver(int i) const
		{
			Assert(i >= 0 && i < BoneFlexDriverCount());
			return studiohdr2index ? pStudioHdr2()->pBoneFlexDriver(i) : nullptr;
		}

		// NOTE: No room to add stuff? Up the .mdl file format version 
		// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
		// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
		int unused2[1];
	};


	struct studiomeshdata_t
	{
		int m_NumGroup;
		void* m_pMeshGroup;
	};

	struct studioloddata_t
	{
		// not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
		//int					m_NumMeshes; 
		studiomeshdata_t* m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
		float m_SwitchPoint;
		// one of these for each lod since we can switch to simpler materials on lower lods.
		int numMaterials;
		IMaterial** ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
		// hack - this needs to go away.
		int* pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */

		// For decals on hardware morphing, we must actually do hardware skinning
		// For this to work, we have to hope that the total # of bones used by
		// hw flexed verts is < than the max possible for the dx level we're running under
		int* m_pHWMorphDecalBoneRemap;
		int m_nDecalBoneCount;
	};

	struct studiohwdata_t
	{
		int m_RootLOD; // calced and clamped, nonzero for lod culling
		int m_NumLODs;
		studioloddata_t* m_pLODs;
		int m_NumStudioMeshes;

		static float LODMetric(float unitSphereSize)
		{
			return (fabs(unitSphereSize) > FLT_EPSILON) ? (100.0f / unitSphereSize) : 0.0f;
		}

		int GetLODForMetric(float lodMetric) const
		{
			if (!m_NumLODs)
				return 0;

			// shadow lod is specified on the last lod with a negative switch
			// never consider shadow lod as viable candidate
			int numLODs = (m_pLODs[m_NumLODs - 1].m_SwitchPoint < 0.0f) ? m_NumLODs - 1 : m_NumLODs;

			for (int i = m_RootLOD; i < numLODs - 1; i++)
			{
				if (m_pLODs[i + 1].m_SwitchPoint > lodMetric)
					return i;
			}

			return numLODs - 1;
		}
	};

	DECLARE_POINTER_HANDLE(StudioDecalHandle_t);
#define STUDIORENDER_DECAL_INVALID  ( (StudioDecalHandle_t)0 )

	enum
	{
		ADDDECAL_TO_ALL_LODS = -1
	};

	DECLARE_POINTER_HANDLE(MDLHandle_t);

	enum OverrideType_t
	{
		OVERRIDE_NORMAL = 0,
		OVERRIDE_BUILD_SHADOWS,
		OVERRIDE_DEPTH_WRITE,
		OVERRIDE_SSAO_DEPTH_WRITE,
	};


	struct DrawModelState_t
	{
		studiohdr_t* m_pStudioHdr;
		studiohwdata_t* m_pStudioHWData;
		void* m_pRenderable;
		const matrix3x4_t* m_pModelToWorld;
		StudioDecalHandle_t m_decals;
		int m_drawFlags;
		int m_lod;
	};

	struct DrawModelResults_t
	{
		int m_ActualTriCount;
		int m_TextureMemoryBytes;
		int m_NumHardwareBones;
		int m_NumBatches;
		int m_NumMaterials;
		int m_nLODUsed;
		int m_flLODMetric;
		void* m_Materials;
		CFastTimer m_RenderTime;
	};

	struct ColorMeshInfo_t
	{
		// A given color mesh can own a unique Mesh, or it can use a shared Mesh
		// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
		IMesh* m_pMesh;
		IPooledVBAllocator* m_pPooledVBAllocator;
		int m_nVertOffsetInBytes;
		int m_nNumVerts;
	};

	struct DrawModelInfo_t
	{
		studiohdr_t* m_pStudioHdr;
		studiohwdata_t* m_pHardwareData;
		StudioDecalHandle_t m_Decals;
		int m_Skin;
		int m_Body;
		int m_HitboxSet;
		void* m_pClientEntity;
		int m_Lod;
		ColorMeshInfo_t* m_pColorMeshes;
		bool m_bStaticLighting;
		void* m_LightingState;

		//IMPLEMENT_OPERATOR_EQUAL(DrawModelInfo_t);
	};


	class IStudioRender : public IAppSystem
	{
	public:
		virtual void BeginFrame() = 0; // 8
		virtual void EndFrame() = 0;
		virtual void Mat_Stub(IMaterialSystem* pMatSys) = 0;

		// Updates the rendering configuration 
		virtual void UpdateConfig(const StudioRenderConfig_t& config) = 0;
		virtual void GetCurrentConfig(StudioRenderConfig_t& config) = 0;

		// Load, unload model data
		virtual bool LoadModel(studiohdr_t* pStudioHdr, void* pVtxData, studiohwdata_t* pHardwareData) = 0;
		virtual void UnloadModel(studiohwdata_t* pHardwareData) = 0;

		// Refresh the studiohdr since it was lost...
		virtual void RefreshStudioHdr(studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData) = 0;

		// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
		virtual void SetEyeViewTarget(const studiohdr_t* pStudioHdr, int nBodyIndex, const Vector& worldPosition) = 0;

		// Methods related to lighting state
		// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
		// returned from GetNumAmbientLightSamples
		virtual int GetNumAmbientLightSamples() = 0;
		virtual const Vector* GetAmbientLightDirections() = 0;
		virtual void SetAmbientLightColors(const Vector4D* pAmbientOnlyColors) = 0;
		virtual void SetAmbientLightColors(const Vector* pAmbientOnlyColors) = 0; // 20
		virtual void SetLocalLights(int numLights, const LightDesc_t* pLights) = 0;

		// Sets information about the camera location + orientation
		virtual void SetViewState(const Vector& viewOrigin, const Vector& viewRight,
		                          const Vector& viewUp, const Vector& viewPlaneNormal) = 0;

		// LOD stuff
		virtual int GetNumLODs(const studiohwdata_t& hardwareData) const = 0;
		virtual float GetLODSwitchValue(const studiohwdata_t& hardwareData, int lod) const = 0;
		virtual void SetLODSwitchValue(studiohwdata_t& hardwareData, int lod, float switchValue) = 0;

		// Sets the color/alpha modulation
		virtual void SetColorModulation(float const* pColor) = 0;
		virtual void SetAlphaModulation(float flAlpha) = 0;

		// Draws the model
		virtual void DrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info,
		                       matrix3x4_t* pBoneToWorld, float* pFlexWeights, float* pFlexDelayedWeights,
		                       const Vector& modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

		// Methods related to static prop rendering
		virtual void DrawModelStaticProp(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld,
		                                 int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;
		virtual void DrawStaticPropDecals(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld) = 0;
		virtual void DrawStaticPropShadows(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld, int flags)
		= 0;

		// Causes a material to be used instead of the materials the model was compiled with
		virtual void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL) = 0;
		// 32

		// Create, destroy list of decals for a particular model
		virtual StudioDecalHandle_t CreateDecalList(studiohwdata_t* pHardwareData) = 0;
		virtual void DestroyDecalList(StudioDecalHandle_t handle) = 0;

		// Add decals to a decal list by doing a planar projection along the ray
		// The BoneToWorld matrices must be set before this is called
		virtual void AddDecal(StudioDecalHandle_t handle, studiohdr_t* pStudioHdr, matrix3x4_t* pBoneToWorld,
		                      const Ray_t& ray, const Vector& decalUp, IMaterial* pDecalMaterial, float radius,
		                      int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;

		// Compute the lighting at a point and normal
		virtual void ComputeLighting(const Vector* pAmbient, int lightCount,
		                             LightDesc_t* pLights, const Vector& pt, const Vector& normal,
		                             Vector& lighting) = 0;

		// Compute the lighting at a point, constant directional component is passed
		// as flDirectionalAmount
		virtual void ComputeLightingConstDirectional(const Vector* pAmbient, int lightCount,
		                                             LightDesc_t* pLights, const Vector& pt, const Vector& normal,
		                                             Vector& lighting, float flDirectionalAmount) = 0;

		// Shadow state (affects the models as they are rendered)
		virtual void AddShadow(IMaterial* pMaterial, void* pProxyData, FlashlightState_t* m_pFlashlightState = nullptr,
		                       VMatrix* pWorldToTexture = nullptr, ITexture* pFlashlightDepthTexture = nullptr) = 0;
		virtual void ClearAllShadows() = 0;

		// Gets the model LOD; pass in the screen size in pixels of a sphere 
		// of radius 1 that has the same origin as the model to get the LOD out...
		virtual int ComputeModelLod(studiohwdata_t* pHardwareData, float unitSphereSize, float* pMetric = nullptr) = 0;

		// Return a number that is usable for budgets, etc.
		// Things that we care about:
		// 1) effective triangle count (factors in batch sizes, state changes, etc)
		// 2) texture memory usage
		// Get Triangles returns the LOD used
		virtual void GetPerfStats(DrawModelResults_t* pResults, const DrawModelInfo_t& info, /*CUtlBuffer*/
		                          void* pSpewBuf = nullptr) const = 0;
		virtual void GetTriangles(const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld,
		                          GetTriangles_Output_t& out) = 0;

		// Returns materials used by a particular model
		virtual int GetMaterialList(studiohdr_t* pStudioHdr, int count, IMaterial** ppMaterials) = 0;
		virtual int GetMaterialListFromBodyAndSkin(MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials,
		                                           IMaterial** ppOutputMaterials) = 0;

		// no debug modes, just fastest drawing path
		virtual void DrawModelArrayStaticProp(const DrawModelInfo_t& drawInfo, int nInstanceCount,
		                                      const MeshInstanceData_t* pInstanceData,
		                                      ColorMeshInfo_t** pColorMeshes) = 0;

		// draw an array of models with the same state
		virtual void DrawModelArray(const StudioModelArrayInfo_t& drawInfo, int nCount,
		                            StudioArrayInstanceData_t* pInstanceData, int nInstanceStride,
		                            int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

		// draw an array of models with the same state
		virtual void DrawModelShadowArray(int nCount, StudioArrayData_t* pShadowData,
		                                  int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

		// draw an array of models with the same state
		virtual void DrawModelArray(const StudioModelArrayInfo2_t& info, int nCount, StudioArrayData_t* pArrayData,
		                            int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;
	};
}
