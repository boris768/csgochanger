#pragma once
#include "SourceEngine/IClientEntity.hpp"
#include "SourceEngine/UtlVector.hpp"
#include "SourceEngine/Definitions.hpp"

namespace SourceEngine
{
	EXCEPTION_DISPOSITION __stdcall viewmodel_handler(EXCEPTION_RECORD* ExceptionRecord, PLONG pEstablisherFrame, CONTEXT* ContextRecord, PLONG pDispatcherContext);

	class CAnimationLayer
	{
	public:
		char pad0[0x14];
		int m_nOrder;
		int m_nSequence;
		char pad1[4];
		float m_flWeight;
		char pad2[8];
		float m_flCycle;

	};

	class CSequenceTransitioner
	{
	public:
		void CheckForSequenceChange(
			// Describe the current animation state with these parameters.
			void *hdr,
			int nCurSequence,

			// Even if the sequence hasn't changed, you can force it to interpolate from the previous
			// spot in the same sequence to the current spot in the same sequence by setting this to true.
			bool bForceNewSequence,

			// Follows EF_NOINTERP.
			bool bInterpolate
		);

		void UpdateCurrent(
			// Describe the current animation state with these parameters.
			void *hdr,
			int nCurSequence,
			float flCurCycle,
			float flCurPlaybackRate,
			float flCurTime
		);

		void RemoveAll(void) { m_animationQueue.RemoveAll(); };

	public:
		CUtlVector <CAnimationLayer> m_animationQueue;
	};


	class IBaseViewModel : public IClientEntity
	{
		template<class T>
		T GetFieldValue(ptrdiff_t offset);
		template<class T>
		T* GetFieldPointer(ptrdiff_t offset);
	public:
		iActivity GetSequenceActivity(int);
		CSequenceTransitioner* GetSequenceTransitioner();
		void SetWeaponModel(const char* modelname, void* weapon);
		void SendViewModelMatchingSequence(int sequence);
		int* GetSequence();
		void UpdateAllViewmodelAddons();
		void RemoveAttachmentModelForWearable(IClientEntity*, IClientEntity*);
		bool CreateAttachmentModelForWearable(IClientEntity*, IClientEntity*);
		CBaseHandle GetOwner();
		CBaseHandle GetWeapon();
		void SetModelIndex(int index);
		float* GetAnimTime();
		bool* GetClientSideAnimation();
		bool* GetClientSideFrameReset();
	};

	template <class T>
	T IBaseViewModel::GetFieldValue(const ptrdiff_t offset)
	{
		const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
		return *(T*)address;
	}

	template <class T>
	T* IBaseViewModel::GetFieldPointer(const ptrdiff_t offset)
	{
		const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
		return (T*)address;
	}
}
