#pragma once

#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "IBaseWorldModel.h"
//#include "Utils.hpp"

class C_BaseCombatWeapon;
class C_CSPlayer;
//class SourceEngine::C_EconItemView;

//this hack used to setup compatibility with manual mapping

class netvars_C_BaseViewModel
{
public:
	ptrdiff_t m_nModelIndex;
	ptrdiff_t m_hWeapon;
	ptrdiff_t m_hOwner;
	ptrdiff_t m_nSequence;
	ptrdiff_t m_bClientSideAnimation;
	ptrdiff_t m_bClientSideFrameReset;
	
	using UpdateAllViewmodelAddons_t = void(__thiscall*)(SourceEngine::IClientEntity*);
	UpdateAllViewmodelAddons_t UpdateAllViewmodelAddons;


	using GetSequenceActivity_t = SourceEngine::iActivity(__thiscall*)(void*, int);
	GetSequenceActivity_t GetSequenceActivity;

	using RemoveAttachmentModelForWearable_t = void(__thiscall*)(SourceEngine::IClientEntity*,
	                                                             SourceEngine::IClientEntity*,
	                                                             SourceEngine::IClientEntity*);
	RemoveAttachmentModelForWearable_t RemoveAttachmentModelForWearable;

	using CreateAttachmentModelForWearable_t = bool(__thiscall*)(SourceEngine::IClientEntity*,
	                                                             SourceEngine::IClientEntity*,
	                                                             SourceEngine::IClientEntity*);
	CreateAttachmentModelForWearable_t CreateAttachmentModelForWearable;

	netvars_C_BaseViewModel();
};

class netvars_C_BaseCombatWeapon
{
public:
	ptrdiff_t m_hOwnerEntity;
	ptrdiff_t m_flNextPrimaryAttack;
	ptrdiff_t m_iClip1;
	ptrdiff_t m_bInReload;
	ptrdiff_t m_flTimeWeaponIdle;
	ptrdiff_t m_bClientSideAnimation;

	ptrdiff_t m_hOuter;
	ptrdiff_t m_iItemDefinitionIndex;
	ptrdiff_t m_bInitialized;
	ptrdiff_t m_OriginalOwnerXuidLow;
	ptrdiff_t m_OriginalOwnerXuidHigh;
	ptrdiff_t m_nFallbackStatTrak;
	ptrdiff_t m_nFallbackPaintKit;
	ptrdiff_t m_nFallbackSeed;
	ptrdiff_t m_flFallbackWear;

	ptrdiff_t m_iItemIDHigh;
	ptrdiff_t m_iEntityQuality;
	ptrdiff_t m_iItemIDLow;
	ptrdiff_t m_szCustomName;
	ptrdiff_t m_hWeaponWorldModel;

	ptrdiff_t dwGetEconItemViewSomething;
	ptrdiff_t dwSetWeaponModel;
	ptrdiff_t m_iAccountID;

	ptrdiff_t m_flAnimTime;
	ptrdiff_t C_EconItemView_Update;
	ptrdiff_t m_CustomMaterials;
	ptrdiff_t m_bCustomMaterialInitialized;

	ptrdiff_t m_nIdealActivity;
	ptrdiff_t m_nIdealSequence;

	ptrdiff_t vtbl_index_SendViewModelAnim;
	
	using SetIdealActivity_t =	bool(__thiscall*)(::C_BaseCombatWeapon*, SourceEngine::iActivity);
	SetIdealActivity_t SetIdealActivity;

	using CheckCustomMaterial_t = void(__thiscall*)(C_BaseCombatWeapon*);
	CheckCustomMaterial_t CheckCustomMaterial;

	using UpdateCustomMaterial_t = void(__thiscall*)(C_BaseCombatWeapon*);
	UpdateCustomMaterial_t UpdateCustomMaterial;

	using WearableEquip_t = int(__thiscall*)(SourceEngine::IClientEntity*, SourceEngine::IClientEntity*);
	WearableEquip_t WearableEquip;
	WearableEquip_t WearableUnEquip;

	using InitializeAttributes_t = int(__thiscall*)(void*);
	InitializeAttributes_t InitializeAttributes;
	
	DWORD m_Item;
	DWORD m_AttributeManager;

	std::unordered_map<std::string, int> ClassIDs;

	size_t dwSetModelIndex;

	netvars_C_BaseCombatWeapon();
};

class netvars_CSGameRulesProxy
{
public:
	ptrdiff_t GameRulesOffset;
	ptrdiff_t m_iRoundTime;
	ptrdiff_t m_bIsValveDS;
	ptrdiff_t m_bIsQueuedMatchmaking;
	ptrdiff_t m_bWarmupPeriod;
	ptrdiff_t m_totalRoundsPlayed;
	ptrdiff_t m_fRoundStartTime;

	netvars_CSGameRulesProxy();
};

class netvars_C_CSPlayer
{
public:
	ptrdiff_t m_bDormant;
	ptrdiff_t m_hActiveWeapon;
	ptrdiff_t m_hMyWeapons;
	ptrdiff_t m_bIsScoped;
	ptrdiff_t m_iHealth;
	ptrdiff_t m_ArmorValue;
	ptrdiff_t m_bHasHelmet;
	ptrdiff_t m_bHasDefuser;
	ptrdiff_t m_lifeState;
	ptrdiff_t m_iTeamNum;
	ptrdiff_t m_fFlags;
	ptrdiff_t m_vecViewOffset;
	ptrdiff_t m_vecOrigin;
	ptrdiff_t m_viewPunchAngle;
	ptrdiff_t m_aimPunchAngle;
	ptrdiff_t m_vecMins;
	ptrdiff_t m_vecMaxs;
	ptrdiff_t m_bGunGameImmunity;
	ptrdiff_t m_nTickBase;
	ptrdiff_t m_iFOV;
	ptrdiff_t m_iShotsFired;
	ptrdiff_t m_hObserverTarget;
	ptrdiff_t m_iObserverMode;
	ptrdiff_t m_hViewModel;
	ptrdiff_t m_hMyWearables;
	ptrdiff_t m_hRagdoll;
	ptrdiff_t m_unMusicID;
	ptrdiff_t rag_m_hPlayer;
	ptrdiff_t m_dwAnimState;
	ptrdiff_t m_nBody;
	ptrdiff_t m_bIsControllingBot;
	ptrdiff_t m_bIsLookingAtWeapon;
	ptrdiff_t m_bIsHoldingLookAtWeapon;
	
	using AttachWearableGibsFromPlayer_t = void(__thiscall*)(SourceEngine::IClientEntity*, SourceEngine::IClientEntity*);
	AttachWearableGibsFromPlayer_t AttachWearableGibsFromPlayer;
	using SetAbsOrigin_t = void(__thiscall*)(SourceEngine::IClientEntity*, const SourceEngine::Vector&);
	SetAbsOrigin_t SetAbsOriginFn;
	using UpdateCustomClothingModel_t = bool(__thiscall*)(C_CSPlayer*, int , bool );
	UpdateCustomClothingModel_t UpdateCustomClothingModel;
	netvars_C_CSPlayer();
};

class netvars_CSTeam
{
public:
	ptrdiff_t m_iTeamNum;
	ptrdiff_t m_bSurrendered;
	ptrdiff_t m_scoreTotal;

	netvars_CSTeam();
};


extern netvars_C_BaseCombatWeapon* netvars_c_base_combat_weapon;
extern netvars_C_CSPlayer* netvars_c_cs_player;
extern netvars_C_BaseViewModel* netvars_c_base_view_model;
extern netvars_CSGameRulesProxy* netvars_csgame_rules_proxy;
extern netvars_CSTeam* netvars_cs_team;

class C_CSGameRulesProxy : public SourceEngine::IClientEntity
{
	template <class T>
	T GetFieldValue(ptrdiff_t offset);
public:
	static C_CSGameRulesProxy* GetGameRulesProxy();

	int GetRoundTime()
	{
		return GetFieldValue<int>(netvars_csgame_rules_proxy->m_iRoundTime);
	}

	bool IsValveDS()
	{
		return GetFieldValue<bool>(netvars_csgame_rules_proxy->m_bIsValveDS);
	}

	bool IsQueuedMatchmaking()
	{
		return GetFieldValue<bool>(netvars_csgame_rules_proxy->m_bIsQueuedMatchmaking);
	}

	bool IsWarmupPeriod()
	{
		return GetFieldValue<bool>(netvars_csgame_rules_proxy->m_bWarmupPeriod);
	}

	bool GettotalRoundsPlayed()
	{
		return GetFieldValue<int>(netvars_csgame_rules_proxy->m_totalRoundsPlayed);
	}

	float GetRoundStartTime()
	{
		return GetFieldValue<float>(netvars_csgame_rules_proxy->m_fRoundStartTime);
	}
};

class C_CSTeam : public SourceEngine::IClientEntity
{
	template <class T>
	T GetFieldValue(const ptrdiff_t offset)
	{
		const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
		return *(T*)address;
	}

public:
	static C_CSTeam* GetTeamT();
	static C_CSTeam* GetTeamCT();
	static C_CSTeam* GetTeamByNum(SourceEngine::TeamID);

	int GetTeamNum()
	{
		return GetFieldValue<int>(netvars_cs_team->m_iTeamNum);
	}

	int GetscoreTotal()
	{
		return GetFieldValue<int>(netvars_cs_team->m_scoreTotal);
	}

	int IsSurrended()
	{
		return GetFieldValue<bool>(netvars_cs_team->m_bSurrendered);
	}
};

class C_CSRagdoll : public SourceEngine::IClientEntity
{
	template <class T>
	T GetFieldValue(ptrdiff_t offset);
public:
	void AttachWearableGibsFromPlayer(IClientEntity* player)
	{
		netvars_c_cs_player->AttachWearableGibsFromPlayer(this, player);
	}

	IClientEntity* get_owner_player()
	{
		using namespace SourceEngine;
		return static_cast<IClientEntity*>(interfaces.EntityList()->GetClientEntityFromHandle(
			GetFieldValue<CHandle<IClientEntity>>(netvars_c_cs_player->rag_m_hPlayer)));
	}
};


class C_BaseCombatWeapon : public SourceEngine::IClientEntity
{
	template <class T>
	T GetFieldValue(ptrdiff_t offset);

	template <class T>
	T* GetFieldPointer(ptrdiff_t offset);
public:

	SourceEngine::IClientNetworkable* GetIClientNetworkable()
	{
		return reinterpret_cast<SourceEngine::IClientNetworkable*>(reinterpret_cast<DWORD>(this) + 8);
	}

	int GetIndex()
	{
		return GetIClientNetworkable()->EntIndex();
	}

	void SetModelIndex(int index)
	{
		using oSetModelIndex = void(__thiscall*)(void*, int);
		SourceEngine::CallVFunction<oSetModelIndex>(this, netvars_c_base_combat_weapon->dwSetModelIndex)(this, index); // Reverse SetWeaponModel.
	}

	//SkinChanger stuff
	void CheckCustomMaterial()
	{
		netvars_c_base_combat_weapon->CheckCustomMaterial(this);
	}

	void UpdateCustomMaterial()
	{
		netvars_c_base_combat_weapon->UpdateCustomMaterial(this);
	}

	SourceEngine::CUtlVector<SourceEngine::CCustomMaterial* >* GetCustomMaterials()
	{
		return GetFieldPointer<SourceEngine::CUtlVector<SourceEngine::CCustomMaterial* >>(netvars_c_base_combat_weapon->m_CustomMaterials);
	}

	SourceEngine::IBaseWorldModel* GetWeaponWorldModel() const
	{
		return reinterpret_cast<SourceEngine::IBaseWorldModel*>(SourceEngine::interfaces.EntityList()->
		                                                                                 GetClientEntityFromHandleOffset(
			                                                                                 this, netvars_c_base_combat_weapon->
			                                                                                 m_hWeaponWorldModel));
	}

	int* GetIdealSequence()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nIdealSequence);
	}
	
	int* GetIdealActivity()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nIdealActivity);
	}
	
	bool SetIdealActivity(SourceEngine::iActivity iActivity)
	{
		return netvars_c_base_combat_weapon->SetIdealActivity(this, iActivity);
	}
	
	bool* GetClientSideAnimation()
	{
		return GetFieldPointer<bool>(netvars_c_base_combat_weapon->m_bClientSideAnimation);
	}

	int* GetModelIndex()
	{
		return GetFieldPointer<int>(netvars_c_base_view_model->m_nModelIndex);
	}

	int* GetFallbackStatTrak()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackStatTrak);
	}

	int* GetFallbackPaintKit()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackPaintKit);
	}

	int* GetFallbackSeed()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_nFallbackSeed);
	}

	float* GetFallbackWear()
	{
		return GetFieldPointer<float>(netvars_c_base_combat_weapon->m_flFallbackWear);
	}

	int* GetEntityQuality()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_iEntityQuality);
	}

	char* GetCustomName()
	{
		return GetFieldPointer<char>(netvars_c_base_combat_weapon->m_szCustomName);
	}

	uint32_t* GetItemIDHigh()
	{
		return GetFieldPointer<uint32_t>(netvars_c_base_combat_weapon->m_iItemIDHigh);
	}

	uint32_t* GetItemIDLow()
	{
		return GetFieldPointer<uint32_t>(netvars_c_base_combat_weapon->m_iItemIDLow);
	}

	uint32_t* GetAccountID()
	{
		return GetFieldPointer<uint32_t>(netvars_c_base_combat_weapon->m_iAccountID);
	}

	int* GetOriginalOwnerXuidLow()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_OriginalOwnerXuidLow);
	}

	int* GetOriginalOwnerXuidHigh()
	{
		return GetFieldPointer<int>(netvars_c_base_combat_weapon->m_OriginalOwnerXuidHigh);
	}

	short* GetItemDefinitionIndex()
	{
		return GetFieldPointer<short>(netvars_c_base_combat_weapon->m_iItemDefinitionIndex);
	}

	bool* GetCustomMaterialInitialized()
	{
		return GetFieldPointer<bool>(netvars_c_base_combat_weapon->m_bCustomMaterialInitialized);
	}

	static std::string GetWeaponName(const SourceEngine::EItemDefinitionIndex weapon);

	SourceEngine::C_EconItemView* GetEconItemView()
	{
		return reinterpret_cast<SourceEngine::C_EconItemView*>(reinterpret_cast<DWORD>(this) + netvars_c_base_combat_weapon->dwGetEconItemViewSomething);
	}

	void SendViewModelAnim(const int nextSequence)
	{
		using oSendViewModelAnim = void(__thiscall*)(void*, int);
		return SourceEngine::CallVFunction<oSendViewModelAnim>(this, netvars_c_base_combat_weapon->vtbl_index_SendViewModelAnim)(this, nextSequence);
	}

	bool SendWeaponAnim(const int iActivity)
	{
		using oSendWeaponAnim = bool(__thiscall*)(void*, int);
		return SourceEngine::CallVFunction<oSendWeaponAnim>(this, 294)(this, iActivity); //  ??? - 354
	}

	float* GetAnimTime()
	{
		return GetFieldPointer<float>(netvars_c_base_combat_weapon->m_flAnimTime);
	}

	//------------------

	C_CSPlayer* GetOwner()
	{
		using namespace SourceEngine;
		return reinterpret_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntityFromHandle(
			GetFieldValue<CHandle<C_CSPlayer>>(netvars_c_base_combat_weapon->m_hOwnerEntity)));
	}

	float NextPrimaryAttack()
	{
		return GetFieldValue<float>(netvars_c_base_combat_weapon->m_flNextPrimaryAttack);
	}

	float* GetTimeWeaponIdle()
	{
		return GetFieldPointer<float>(netvars_c_base_combat_weapon->m_flTimeWeaponIdle);
	}

	unsigned int GetAmmo()
	{
		return GetFieldValue<unsigned int>(netvars_c_base_combat_weapon->m_iClip1);
	}

	bool GetInReload()
	{
		return GetFieldValue<bool>(netvars_c_base_combat_weapon->m_bInReload);
	}

	bool IsWeaponByClass()
	{
		SourceEngine::ClientClass* Class = this->GetClientClass();
		if (!Class)
			return false;
		const SourceEngine::EClassIds ClassID = Class->m_ClassID;
		//auto class_base = netvars_c_base_combat_weapon->ClassIDs.begin();
		for (auto& it : netvars_c_base_combat_weapon->ClassIDs)
		{
			if (ClassID == it.second)
			{
				return true;
			}
		}
		return false;
	}

	enum econ_class
	{
		none = 0,
		weapon,
		knife,
	};

	econ_class GetEconClass()
	{
		return GetEconClass(*this->GetItemDefinitionIndex());
	}

	static econ_class GetEconClass(const int itemdefinitionindex) noexcept//-V2008
	{
		using namespace SourceEngine;
		switch (itemdefinitionindex)
		{
		case weapon_mp5sd:
		case weapon_xm1014:
		case weapon_mag7:
		case weapon_mac10:
		case weapon_sawedoff:
		case weapon_ak47:
		case weapon_aug:
		case weapon_famas:
		case weapon_galilar:
		case weapon_m249:
		case weapon_m4a1:
		case weapon_m4a1_silencer:
		case weapon_nova:
		case weapon_p90:
		case weapon_ump:
		case weapon_bizon:
		case weapon_negev:
		case weapon_mp7:
		case weapon_mp9:
		case weapon_sg556:
		case weapon_awp:
		case weapon_g3sg1:
		case weapon_scar20:
		case weapon_ssg08:
		case weapon_deagle:
		case weapon_elite:
		case weapon_fiveseven:
		case weapon_glock:
		case weapon_tec9:
		case weapon_hkp2000:
		case weapon_usp_silencer:
		case weapon_p250:
		case weapon_cz75a:
		case weapon_revolver:
			return weapon;
		case weapon_knifegg:
		case weapon_knife_ct:
		case weapon_knife_t:
		case weapon_knife_bayonet:
		case weapon_knife_css:
		case weapon_knife_flip:
		case weapon_knife_gut:
		case weapon_knife_karambit:
		case weapon_knife_m9_bayonet:
		case weapon_knife_tactical:
		case weapon_knife_falchion:
		case weapon_knife_survival_bowie:
		case weapon_knife_butterfly:
		case weapon_knife_push:
		case weapon_knife_cord:
		case weapon_knife_canis:
		case weapon_knife_ursus:
		case weapon_knife_gypsy_jackknife:
		case weapon_knife_outdoor:
		case weapon_knife_stiletto:
		case weapon_knife_widowmaker:
		case weapon_knife_skeleton:
			return knife;
		default:
			return none;
		}
	}
};

class C_CSPlayer : public SourceEngine::IClientEntity
{
	template <class T>
	T GetFieldValue(ptrdiff_t offset);

	template <class T>
	T* GetFieldPointer(ptrdiff_t offset);
public:
	static C_CSPlayer* GetLocalPlayer()
	{
		return static_cast<C_CSPlayer*>(SourceEngine::interfaces.EntityList()->GetClientEntity(
			SourceEngine::interfaces.Engine()->GetLocalPlayer()));
	}

	C_BaseCombatWeapon* GetActiveWeapon()
	{
		using namespace SourceEngine;
		return static_cast<C_BaseCombatWeapon*>(interfaces.EntityList()->GetClientEntityFromHandle(
			GetFieldValue<CHandle<IClientEntity>>(netvars_c_cs_player->m_hActiveWeapon)));
	}

	SourceEngine::CHandle<IClientEntity>* GetWeapons()
	{
		return GetFieldPointer<SourceEngine::CHandle<IClientEntity>>(netvars_c_cs_player->m_hMyWeapons);
	}

	bool UpdateCustomClothingModel(int loadout_position, bool flag = 0)
	{
		return netvars_c_cs_player->UpdateCustomClothingModel(this, loadout_position, flag);
	}
	
	C_CSRagdoll* GetRagdoll()
	{
		using namespace SourceEngine;
		return static_cast<C_CSRagdoll*>(interfaces.EntityList()->GetClientEntityFromHandle(
			GetFieldValue<CHandle<IClientEntity>>(netvars_c_cs_player->m_hRagdoll)));
	}

	SourceEngine::Vector GetBonePosition(const SourceEngine::ECSPlayerBones& Bone)
	{
		SourceEngine::matrix3x4_t boneMatrix[128];

		if (!this->SetupBones(boneMatrix, 128, 0x00000100, SourceEngine::interfaces.Engine()->GetLastTimeStamp()))
			return SourceEngine::Vector();

		SourceEngine::matrix3x4_t hitbox = boneMatrix[Bone];

		return SourceEngine::Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}

	static int GetBoneByName(C_CSPlayer* player, const char* boneName)
	{
		SourceEngine::studiohdr_t* pStudioModel = SourceEngine::interfaces.ModelInfo()->GetStudioModel(player->GetModel());
		if (!pStudioModel)
			return -1;

		SourceEngine::matrix3x4_t pBoneToWorldOut[128];
		if (!player->SetupBones(pBoneToWorldOut, 128, 256, 0))
			return -1;

		for (size_t i = 0; i < size_t(pStudioModel->numbones); ++i)
		{
			SourceEngine::mstudiobone_t* pBone = pStudioModel->pBone(i);
			if (!pBone)
				continue;

			if (pBone->pszName() && strcmp(pBone->pszName(), boneName) == 0)
				return i;
		}

		return -1;
	}

	bool IsLookingAtWeapon()
	{
		return GetFieldValue<bool>(netvars_c_cs_player->m_bIsLookingAtWeapon);
	}
	bool IsHoldingLookAtWeapon()
	{
		return GetFieldValue<bool>(netvars_c_cs_player->m_bIsHoldingLookAtWeapon);
	}
	
	bool IsControllingBot()
	{
		return GetFieldValue<bool>(netvars_c_cs_player->m_bIsControllingBot);
	}
	
	SourceEngine::CBaseHandle* GetMyWearables()
	{
		return GetFieldPointer<SourceEngine::CBaseHandle>(netvars_c_cs_player->m_hMyWearables);
	}

	unsigned* GetMusicID()
	{
		return GetFieldPointer<unsigned>(netvars_c_cs_player->m_unMusicID);
	}

	SourceEngine::CBaseHandle GetViewModelHandle(const int& index)
	{
		return GetFieldValue<SourceEngine::CBaseHandle>(netvars_c_cs_player->m_hViewModel + 4 * index);
	}

	SourceEngine::IClientEntity* GetViewModel(const int index = 0)
	{
		const auto handle = this->GetViewModelHandle(index);
		return SourceEngine::interfaces.EntityList()->GetClientEntity(handle.GetEntryIndex());
	}

	SourceEngine::ObserverMode_t GetObserverMode()
	{
		return GetFieldValue<SourceEngine::ObserverMode_t>(netvars_c_cs_player->m_iObserverMode);
	}

	C_CSPlayer* GetObserverTarget()
	{
		using namespace SourceEngine;
		return static_cast<C_CSPlayer*>(interfaces.EntityList()->GetClientEntityFromHandle(
			GetFieldValue<CHandle<IClientEntity>>(netvars_c_cs_player->m_hObserverTarget)));
	}

	bool IsScoped()
	{
		return GetFieldValue<bool>(netvars_c_cs_player->m_bIsScoped);
	}

	int GetHealth()
	{
		return GetFieldValue<int>(netvars_c_cs_player->m_iHealth);
	}

	int GetArmor()
	{
		return GetFieldValue<int>(netvars_c_cs_player->m_ArmorValue);
	}

	bool HasHelmet()
	{
		return (GetFieldValue<int>(netvars_c_cs_player->m_bHasHelmet) == 1);
	}

	bool HasDefuser()
	{
		return (GetFieldValue<int>(netvars_c_cs_player->m_bHasDefuser) == 1);
	}

	bool IsAlive()
	{
		return GetFieldValue<int>(netvars_c_cs_player->m_lifeState) == SourceEngine::LifeState::LIFE_ALIVE;
	}

	SourceEngine::TeamID GetTeamNum()
	{
		return GetFieldValue<SourceEngine::TeamID>(netvars_c_cs_player->m_iTeamNum);
	}

	int GetFlags()
	{
		return GetFieldValue<int>(netvars_c_cs_player->m_fFlags);
	}

	SourceEngine::Vector GetViewOffset()
	{
		return GetFieldValue<SourceEngine::Vector>(netvars_c_cs_player->m_vecViewOffset);
	}

	SourceEngine::Vector GetOrigin()
	{
		return GetFieldValue<SourceEngine::Vector>(netvars_c_cs_player->m_vecOrigin);
	}

	SourceEngine::Vector GetEyePos()
	{
		return GetOrigin() + GetViewOffset();
	}

	SourceEngine::Vector* ViewPunch()
	{
		return GetFieldPointer<SourceEngine::Vector>(netvars_c_cs_player->m_viewPunchAngle);
	}

	SourceEngine::Vector* AimPunch()
	{
		return GetFieldPointer<SourceEngine::Vector>(netvars_c_cs_player->m_aimPunchAngle);
	}

	SourceEngine::Vector* GetOBBMins()
	{
		return GetFieldPointer<SourceEngine::Vector>(netvars_c_cs_player->m_vecMins);
	}

	SourceEngine::Vector* GetOBBMaxs()
	{
		return GetFieldPointer<SourceEngine::Vector>(netvars_c_cs_player->m_vecMaxs);
	}

	bool GetDormant()
	{
		//static int m_bDormant = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_bDormant"));
		return GetFieldValue<bool>(netvars_c_cs_player->m_bDormant);
	}
	int* Body()
	{
		return GetFieldPointer<int>(netvars_c_cs_player->m_nBody);
	}

	bool GetImmune()
	{
		return GetFieldValue<bool>(netvars_c_cs_player->m_bGunGameImmunity);
	}

	unsigned int GetTickBase()
	{
		return GetFieldValue<unsigned int>(netvars_c_cs_player->m_nTickBase);
	}

	int* GetFOV()
	{
		return GetFieldPointer<int>(netvars_c_cs_player->m_iFOV);
	}

	int GetShotsFired()
	{
		return GetFieldValue<int>(netvars_c_cs_player->m_iShotsFired);
	}

	bool IsJumping()
	{
		const auto flags = this->GetFlags();
		return !(flags == 1 || flags == 3 || flags == 5 || flags == 7);
	}
};

inline EXCEPTION_DISPOSITION __stdcall test_handler(EXCEPTION_RECORD* ExceptionRecord, PLONG pEstablisherFrame,
                                                    CONTEXT* ContextRecord, PLONG pDispatcherContext)
{
	ContextRecord->Eip = ContextRecord->Ebx;
	return ExceptionContinueExecution;
}

extern std::vector<ptrdiff_t> bad_adresses;

//#pragma optimize( "", off ) 
template <class T>
T C_CSGameRulesProxy::GetFieldValue(const ptrdiff_t offset)
{
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T)0;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T returning_value = *(T*)address;
	//	goto exit;
	//error_handled:
	//	returning_value = (T)0;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;
}

template <class T>
T C_CSRagdoll::GetFieldValue(const ptrdiff_t offset)
{
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T)0;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T returning_value = *(T*)address;
	//	goto exit;
	//error_handled:
	//	returning_value = (T)0;
	//	//std::cout << "C_BaseCombatWeapon::GetFieldValue: " << address << " caller: " << caller << std::endl;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;
}

template <class T>
T C_BaseCombatWeapon::GetFieldValue(const ptrdiff_t offset)
{
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T)0;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T returning_value = *(T*)address;
	//	goto exit;
	//error_handled:
	//	returning_value = (T)0;
	//	//std::cout << "C_BaseCombatWeapon::GetFieldValue: " << address << " caller: " << caller << std::endl;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;
	//__try
	//{
	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if(IsBadReadPtr((void*)address, sizeof(T)))
	//	return (T)0;
	//return *(T*)(address);
	//}
	//__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	return (T)0;
	//}
	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if (HeapValidate(SourceEngine::interfaces.MemAlloc()->hHeap, NULL, (void*)address))
	//{
	//	return *(T*)(address);
	//}
	//return (T)0;
}

//std::cout << "CClientState::GetFieldPointer
template <class T>
T* C_BaseCombatWeapon::GetFieldPointer(const ptrdiff_t offset)
{
	assert(this);
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T*)nullptr;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T* returning_value = (T*)address;
	//volatile T check = *returning_value;
	//	goto exit;
	//error_handled:
	//	returning_value = nullptr;
	//	//std::cout << "C_BaseCombatWeapon::GetFieldPointer: " << address << " caller: " << caller << std::endl;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;


	//if (IsBadWritePtr((void*)address, sizeof(T)))
	//	return (T*)nullptr;
	//return (T*)(address);
	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if(Utils::isNotOKToWriteMemory(reinterpret_cast<void*>(address)) != FALSE)
	//{
	//	return (T*)nullptr;
	//}
	//return (T*)(address);
	//return (T*)(reinterpret_cast<ptrdiff_t>(this) + offset);
}


template <class T>
T C_CSPlayer::GetFieldValue(const ptrdiff_t offset)
{
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T)0;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T returning_value = *(T*)address;
	//	goto exit;
	//error_handled:
	//	returning_value = (T)0;
	//	//std::cout << "C_CSPlayer::GetFieldValue: " << address << " caller: " << caller << std::endl;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;

	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if (IsBadReadPtr((void*)address, sizeof(T)))
	//	return (T)0;
	//return *(T*)(address);

	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if (Utils::isNotOKToReadMemory((void*)address))
	//{
	//	return (T)0;
	//}
	//return *(T*)(address);
}

template <class T>
T* C_CSPlayer::GetFieldPointer(const ptrdiff_t offset)
{
	//DWORD old_EBX;
	const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//auto end = bad_adresses.end();
	//for (auto it = bad_adresses.begin(); it != bad_adresses.end(); ++it)
	//{
	//	if (*it == address)
	//	{
	//		return (T*)nullptr;
	//	}
	//}
	//__asm
	//{
	//	mov old_EBX, ebx
	//	mov ebx, error_handled
	//	push test_handler
	//	push dword ptr fs : [0]
	//	mov dword ptr fs : [0], esp
	//}
	T* returning_value = (T*)address;
	//	volatile T check = *returning_value;
	//	goto exit;
	//error_handled:
	//	returning_value = nullptr;
	//	//std::cout << "C_CSPlayer::GetFieldValue: " << address << " caller: " << caller << std::endl;
	//	bad_adresses.emplace_back(address);
	//exit:
	//	__asm
	//	{
	//		mov ebx, old_EBX
	//		mov esp, dword ptr fs : [0]
	//		pop dword ptr fs : [0]
	//		add esp, 4
	//	}
	return returning_value;

	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;
	//if (IsBadReadPtr((void*)address, sizeof(T)))
	//	return (T*)nullptr;
	//return (T*)(address);

	//auto address = reinterpret_cast<ptrdiff_t>(this) + offset;

	//MEMORY_BASIC_INFORMATION info;
	//if(VirtualQuery((void*)address, &info, sizeof(info)) != sizeof(info) && (info.Protect != PAGE_READWRITE || info.State != MEM_COMMIT))
	//if (IsBadReadPtr((void*)address, sizeof(T)) || IsBadWritePtr((void*)address, sizeof(T)))
	//if(Utils::isNotOKToWriteMemory((void*)address))
	//{
	//	return (T*)nullptr;
	//}
	//return (T*)(address);
	//return (T*)(reinterpret_cast<ptrdiff_t>(this) + offset);
}

//#pragma optimize( "", on)
