// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CSGOStructs.hpp"	
#include "Hooks.hpp"
#include "Utils.hpp"
#include "XorStr.hpp"

netvars_C_BaseViewModel::netvars_C_BaseViewModel()
{
	//m_nModelIndex = GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_nModelIndex"));
	//m_hWeapon = GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_hWeapon"));
	//m_hOwner = GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_hOwner"));
	//m_nSequence = GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_nSequence"));

	const auto netvar_mgr = netvar_manager::get();
	m_nModelIndex = netvar_mgr.get_offset(FNV("CBaseViewModel->m_nModelIndex"));
	m_hWeapon = netvar_mgr.get_offset(FNV("CBaseViewModel->m_hWeapon"));
	m_hOwner = netvar_mgr.get_offset(FNV("CBaseViewModel->m_hOwner"));
	m_nSequence = netvar_mgr.get_offset(FNV("CBaseViewModel->m_nSequence"));
	m_bClientSideAnimation = netvar_mgr.get_offset(FNV("CBaseAnimating->m_bClientSideAnimation"));
	m_bClientSideFrameReset = netvar_mgr.get_offset(FNV("CBaseAnimating->m_bClientSideFrameReset"));

	//UpdateAllViewmodelAddons = Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 83 EC 2C 53 56 57 8B F9 8B 07"));
	UpdateAllViewmodelAddons =  reinterpret_cast<UpdateAllViewmodelAddons_t>(Utils::dwGetCallAddress(
		Utils::FindSignature(SourceEngine::memory.client(), XorStr("E8 ? ? ? ? 8B 16 8B CE 6A 20")))); //broken
	RemoveAttachmentModelForWearable = reinterpret_cast<RemoveAttachmentModelForWearable_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 57 8B B9")));
	CreateAttachmentModelForWearable = reinterpret_cast<CreateAttachmentModelForWearable_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 51 53 8B 5D 08 56 57 89")));
	GetSequenceActivity = reinterpret_cast<GetSequenceActivity_t>(Utils::dwGetCallAddress(Utils::FindSignature(SourceEngine::memory.client(), XorStr("E8 ? ? ? ? 33 C9 3B F0 5E"))));
}
netvars_C_BaseCombatWeapon::netvars_C_BaseCombatWeapon()
{
	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 1\n"));
	
	const auto netvar_mgr = netvar_manager::get();
	m_hOwnerEntity = netvar_mgr.get_offset(FNV("CBaseEntity->m_hOwnerEntity"));
	m_flNextPrimaryAttack = netvar_mgr.get_offset(FNV("CBaseCombatWeapon->m_flNextPrimaryAttack"));
	m_flTimeWeaponIdle = netvar_mgr.get_offset(FNV("CBaseCombatWeapon->m_flTimeWeaponIdle"));
	m_iClip1 = netvar_mgr.get_offset(FNV("CBaseCombatWeapon->m_iClip1"));
	m_bInReload = m_flNextPrimaryAttack + 109;
	
	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 2\n"));

	m_bClientSideAnimation = netvar_mgr.get_offset(FNV("CBaseAnimating->m_bClientSideAnimation"));
	m_hWeaponWorldModel = netvar_mgr.get_offset(FNV("CBaseCombatWeapon->m_hWeaponWorldModel"));
	m_flAnimTime = netvar_mgr.get_offset(FNV("CBaseEntity->m_flAnimTime"));

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 3\n"));

	m_bInitialized = netvar_mgr.get_offset(FNV("CEconEntity->m_bInitialized"));
	m_OriginalOwnerXuidLow = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_OriginalOwnerXuidLow"));
	m_OriginalOwnerXuidHigh = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_OriginalOwnerXuidHigh"));
	m_nFallbackPaintKit = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_nFallbackPaintKit"));
	m_nFallbackSeed = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_nFallbackSeed"));
	m_flFallbackWear = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_flFallbackWear"));
	m_nFallbackStatTrak = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_nFallbackStatTrak"));

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 4\n"));
	
	m_hOuter = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_hOuter"));
	m_iItemDefinitionIndex = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_iItemDefinitionIndex"));
	m_iItemIDHigh = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_iItemIDHigh"));
	m_iItemIDLow = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_iItemIDLow"));
	m_iAccountID = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_iAccountID"));
	m_iEntityQuality = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_iEntityQuality"));
	m_szCustomName = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_szCustomName"));

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 5\n"));
	
	m_Item = netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_Item"));
	m_AttributeManager = 0;// netvar_mgr.get_offset(FNV("CBaseAttributableItem->m_AttributeManager"));

	dwGetEconItemViewSomething = m_Item;
	//m_hOwnerEntity = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"));
	//m_flNextPrimaryAttack = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"));
	//m_flTimeWeaponIdle = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flTimeWeaponIdle"));
	//m_iClip1 = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"));
	//m_bInReload = m_flNextPrimaryAttack + 109;

	//m_bClientSideAnimation = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_bClientSideAnimation"));
	//m_hWeaponWorldModel = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_hWeaponWorldModel"));
	//m_flAnimTime = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("AnimTimeMustBeFirst"), XorStr("m_flAnimTime"));

	//m_OriginalOwnerXuidLow = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_OriginalOwnerXuidLow"));
	//m_OriginalOwnerXuidHigh = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_OriginalOwnerXuidHigh"));
	//m_nFallbackPaintKit = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackPaintKit"));
	//m_nFallbackSeed = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackSeed"));
	//m_flFallbackWear = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_flFallbackWear"));
	//m_nFallbackStatTrak = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackStatTrak"));
	//
	//m_hOuter = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_hOuter"));
	//m_iItemDefinitionIndex = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex"));
	//m_iItemIDHigh = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDHigh"));
	//m_iItemIDLow = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDLow"));
	//m_iAccountID = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iAccountID"));
	//m_iEntityQuality = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityQuality"));
	//m_szCustomName = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_szCustomName"));

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 6\n"));
	
	CheckCustomMaterial = reinterpret_cast<CheckCustomMaterial_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 EC 28 53 8B D9 56 57 80 ? ? ? ? ? ? 0F")));
	UpdateCustomMaterial = reinterpret_cast<UpdateCustomMaterial_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("56 8B F1 80 ? ? ? ? ? ? 75 70 83 ? ? ? ? ? ? 7F")));
	m_bCustomMaterialInitialized = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("C6 86 ? ? ? ? ? FF 50 04")) + 2);

	//CNetVar fast_hack_fix;
	//m_Item = 0;
	//m_AttributeManager = 0;
	//fast_hack_fix.QueueNetVar(XorStr("DT_AttributeContainer"), XorStr("m_Item"), &m_Item);
	//fast_hack_fix.QueueNetVar(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), &m_AttributeManager);
	//fast_hack_fix.Store();
	//assert(m_Item > 0);
	//assert(m_AttributeManager > 0);

	//55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 3C
	//C_EconItemView_Update = Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 3C")); //TODO: broken due fucking volvo

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 7\n"));
	
	C_EconItemView_Update = 0;
	dwSetWeaponModel = Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 56 8B F1 8B 4D 0C 57"));
	dwSetModelIndex = *reinterpret_cast<PDWORD>(Utils::FindSignature(XorStr("FF 92"), dwSetWeaponModel, dwSetWeaponModel + 0x1000) + 2) / 4;
	m_CustomMaterials = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("83 BE ? ? ? ? ? 7F 67")) + 2) - 12;
	WearableEquip = reinterpret_cast<WearableEquip_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9")));
	WearableUnEquip = reinterpret_cast<WearableEquip_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 53 6A 00 8B D9 E8 ? ? ? ? 6A 00")));
	InitializeAttributes = reinterpret_cast<InitializeAttributes_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86")));
	SetIdealActivity = reinterpret_cast<SetIdealActivity_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 53 56 8B 35 ? ? ? ? 57 8B F9 8B")));
	m_nIdealSequence = *reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("89 87 ? ? ? ? E8 ? ? ? ? 89 45 08")) + 2);
	m_nIdealActivity = m_nIdealSequence + 4;

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 8\n"));
	
	vtbl_index_SendViewModelAnim = (*reinterpret_cast<ptrdiff_t*>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("8B 07 8B CF FF 90 ? ? ? ? FF B7")) + 6)) / 4;

	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 9\n"));
	
	ClassIDs.insert_or_assign(XorStr("CAK47"), 0);
	ClassIDs.insert_or_assign(XorStr("CDEagle"), 0);
	ClassIDs.insert_or_assign(XorStr("CKnifeGG"), 0);
	ClassIDs.insert_or_assign(XorStr("CKnife"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponAug"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponAWP"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponBizon"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponElite"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponFamas"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponFiveSeven"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponG3SG1"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponGalil"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponGalilAR"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponGlock"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponHKP2000"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponM249"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponM4A1"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponMAC10"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponMag7"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponMP7"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponMP9"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponNegev"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponNOVA"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponP250"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponP90"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponSawedoff"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponSCAR20"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponSG556"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponSSG08"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponTec9"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponUMP45"), 0);
	ClassIDs.insert_or_assign(XorStr("CWeaponXM1014"), 0);
	
	Hooks::changer_log->msg(XorStr("netvars_C_BaseCombatWeapon: stage 10\n"));
}

netvars_CSGameRulesProxy::netvars_CSGameRulesProxy()
{
	GameRulesOffset = Utils::FindSignature(SourceEngine::memory.client(), XorStr("8B EC 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 FF 35 ? ? ? ? E8") ) + 18;
	GameRulesOffset = *reinterpret_cast<ptrdiff_t*>(GameRulesOffset);

	const auto netvar_mgr = netvar_manager::get();
	m_iRoundTime = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_iRoundTime"));
	m_bIsValveDS = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_bIsValveDS"));
	m_bIsQueuedMatchmaking = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_bIsQueuedMatchmaking"));
	m_bWarmupPeriod = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_bWarmupPeriod"));
	m_totalRoundsPlayed = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_totalRoundsPlayed"));
	m_fRoundStartTime = netvar_mgr.get_offset(FNV("CCSGameRulesProxy->m_fRoundStartTime"));

	//m_iRoundTime = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_iRoundTime"));
	//m_bIsValveDS = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bIsValveDS"));
	//m_bIsQueuedMatchmaking = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bIsQueuedMatchmaking"));
	//m_bWarmupPeriod = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bWarmupPeriod"));
	//m_totalRoundsPlayed = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_totalRoundsPlayed"));
	//m_fRoundStartTime = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_fRoundStartTime"));
}

netvars_C_CSPlayer::netvars_C_CSPlayer()
{
	const auto netvar_mgr = netvar_manager::get();

	m_hMyWearables = netvar_mgr.get_offset(FNV("CBaseCombatCharacter->m_hMyWearables"));
	m_hActiveWeapon = netvar_mgr.get_offset(FNV("CBaseCombatCharacter->m_hActiveWeapon"));
	m_hMyWeapons = netvar_mgr.get_offset(FNV("CBaseCombatCharacter->m_hMyWeapons"));

	m_iHealth = netvar_mgr.get_offset(FNV("CBasePlayer->m_iHealth"));
	m_hViewModel = netvar_mgr.get_offset(FNV("CBasePlayer->m_hViewModel[0]"));
	m_lifeState = netvar_mgr.get_offset(FNV("CBasePlayer->m_lifeState"));
	m_fFlags = netvar_mgr.get_offset(FNV("CBasePlayer->m_fFlags"));
	m_vecViewOffset = netvar_mgr.get_offset(FNV("CBasePlayer->m_vecViewOffset[0]"));
	m_viewPunchAngle = netvar_mgr.get_offset(FNV("CBasePlayer->m_viewPunchAngle"));
	m_aimPunchAngle = netvar_mgr.get_offset(FNV("CBasePlayer->m_aimPunchAngle"));
	m_iFOV = netvar_mgr.get_offset(FNV("CBasePlayer->m_iFOV"));
	m_nTickBase = netvar_mgr.get_offset(FNV("CBasePlayer->m_nTickBase"));
	m_hObserverTarget = netvar_mgr.get_offset(FNV("CBasePlayer->m_hObserverTarget"));
	m_iObserverMode = netvar_mgr.get_offset(FNV("CBasePlayer->m_iObserverMode"));

	m_iTeamNum = netvar_mgr.get_offset(FNV("CBaseEntity->m_iTeamNum"));
	m_vecOrigin = netvar_mgr.get_offset(FNV("CBaseEntity->m_vecOrigin"));
	m_vecMins = netvar_mgr.get_offset(FNV("CBaseEntity->m_vecMins"));
	m_vecMaxs = netvar_mgr.get_offset(FNV("CBaseEntity->m_vecMaxs"));

	m_bIsScoped = netvar_mgr.get_offset(FNV("CCSPlayer->m_bIsScoped"));
	m_ArmorValue = netvar_mgr.get_offset(FNV("CCSPlayer->m_ArmorValue"));
	m_bHasHelmet = netvar_mgr.get_offset(FNV("CCSPlayer->m_bHasHelmet"));
	m_bHasDefuser = netvar_mgr.get_offset(FNV("CCSPlayer->m_bHasDefuser"));
	m_bGunGameImmunity = netvar_mgr.get_offset(FNV("CCSPlayer->m_bGunGameImmunity"));
	m_iShotsFired = netvar_mgr.get_offset(FNV("CCSPlayer->m_iShotsFired"));
	m_hRagdoll = netvar_mgr.get_offset(FNV("CCSPlayer->m_hRagdoll"));
	m_unMusicID = netvar_mgr.get_offset(FNV("CCSPlayer->m_unMusicID"));
	m_bIsLookingAtWeapon = netvar_mgr.get_offset(FNV("CCSPlayer->m_bIsLookingAtWeapon"));
	m_bIsHoldingLookAtWeapon = netvar_mgr.get_offset(FNV("CCSPlayer->m_bIsHoldingLookAtWeapon"));
	
	rag_m_hPlayer = netvar_mgr.get_offset(FNV("CCSRagdoll->m_hPlayer"));

	m_nBody = netvar_mgr.get_offset(FNV("CBaseAnimating->m_nBody"));
	m_bIsControllingBot = netvar_mgr.get_offset(FNV("CCSPlayer->m_bIsControllingBot"));
	m_dwAnimState = 0; //unused;
	//m_hMyWearables = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWearables"));
	//m_hActiveWeapon = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon"));
	//m_hMyWeapons = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWeapons"));

	//m_iHealth = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iHealth"));
	//m_hViewModel = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_hViewModel[0]"));
	//m_lifeState = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_lifeState"));
	//m_fFlags = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_fFlags"));
	//m_vecViewOffset = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]"));
	//m_viewPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle"));
	//m_aimPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle"));
	//m_iFOV = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iFOV"));
	//m_nTickBase = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase"));
	//m_hObserverTarget = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_hObserverTarget"));
	//m_iObserverMode = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iObserverMode"));

	//m_iTeamNum = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_iTeamNum"));
	//m_vecOrigin = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_vecOrigin"));
	//m_vecMins = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_Collision"), XorStr("m_vecMins"));
	//m_vecMaxs = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_Collision"), XorStr("m_vecMaxs"));

	//m_bIsScoped = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsScoped"));
	//m_ArmorValue = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_ArmorValue"));
	//m_bHasHelmet = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bHasHelmet"));
	//m_bHasDefuser = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bHasDefuser"));
	//m_bGunGameImmunity = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bGunGameImmunity"));
	//m_iShotsFired = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("cslocaldata"), XorStr("m_iShotsFired"));
	//m_hRagdoll = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_hRagdoll"));
	//m_unMusicID = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_unMusicID"));

	//rag_m_hPlayer = GET_NETVAR(XorStr("DT_CSRagdoll"), XorStr("m_hPlayer"));

	//m_nBody = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_nBody"));

	m_bDormant = *(ptrdiff_t*)(Utils::FindSignature(SourceEngine::memory.client(), XorStr("8A 81 ? ? ? ? C3 32 C0")) + 2) + 8;//0xED;
	AttachWearableGibsFromPlayer = reinterpret_cast<AttachWearableGibsFromPlayer_t>(Utils::FindSignature(SourceEngine::memory.client(),
	                                                                                                     XorStr("55 8B EC 83 EC 18 53 89 4D F8 8B 4D 08 56 57")));
	SetAbsOriginFn = reinterpret_cast<SetAbsOrigin_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 E4 F8 51 53 56 57 8B F1")));
	UpdateCustomClothingModel = reinterpret_cast<UpdateCustomClothingModel_t>(Utils::FindSignature(SourceEngine::memory.client(), XorStr("55 8B EC 83 EC 28 53 56 57 8B 7D 08 8B D9 83")));
}

netvars_CSTeam::netvars_CSTeam()
{
	const auto netvar_mgr = netvar_manager::get();
	m_iTeamNum = netvar_mgr.get_offset(FNV("CTeam->m_iTeamNum"));
	m_bSurrendered = netvar_mgr.get_offset(FNV("CTeam->m_bSurrendered"));
	m_scoreTotal = netvar_mgr.get_offset(FNV("CTeam->m_scoreTotal"));

	//m_iTeamNum = GET_NETVAR(XorStr("DT_Team"), XorStr("m_iTeamNum"));
	//m_bSurrendered = GET_NETVAR(XorStr("DT_Team"), XorStr("m_bSurrendered"));
	//m_scoreTotal = GET_NETVAR(XorStr("DT_Team"), XorStr("m_scoreTotal"));
}

C_CSGameRulesProxy* C_CSGameRulesProxy::GetGameRulesProxy()
{
	return *reinterpret_cast<C_CSGameRulesProxy**>(netvars_csgame_rules_proxy->GameRulesOffset);
}

C_CSTeam* C_CSTeam::GetTeamT()
{
	if (!SourceEngine::interfaces.Engine()->IsInGame())
		return nullptr;

	auto entitylist = SourceEngine::interfaces.EntityList();
	C_CSTeam* team = nullptr;
	for (int i = 65; i < entitylist->GetHighestEntityIndex(); ++i)
	{
		team = reinterpret_cast<C_CSTeam*>(entitylist->GetClientEntity(i));
		if (!team)
			continue;
		SourceEngine::ClientClass * client_class = team->GetClientClass();
		if (!client_class)
			continue;
		if (strcmp(client_class->m_pNetworkName, XorStr("CCSTeam")) == 0)
		{
			if(team->GetTeamNum() == static_cast<int>(SourceEngine::TeamID::TEAM_TERRORIST))
				break;
		}
	}
	return team;
}

C_CSTeam* C_CSTeam::GetTeamCT()
{
	if (!SourceEngine::interfaces.Engine()->IsInGame())
		return nullptr;

	auto entitylist = SourceEngine::interfaces.EntityList();
	C_CSTeam* team = nullptr;
	for (int i = 65; i < entitylist->GetHighestEntityIndex(); ++i)
	{
		team = reinterpret_cast<C_CSTeam*>(entitylist->GetClientEntity(i));
		if (!team)
			continue;
		SourceEngine::ClientClass * client_class = team->GetClientClass();
		if (!client_class)
			continue;
		if (strcmp(client_class->m_pNetworkName, XorStr("CCSTeam")) == 0)
		{
			if (team->GetTeamNum() == static_cast<int>(SourceEngine::TeamID::TEAM_COUNTER_TERRORIST))
				break;
		}
	}
	return team;
}

C_CSTeam* C_CSTeam::GetTeamByNum(SourceEngine::TeamID teamid)
{
	if (!SourceEngine::interfaces.Engine()->IsInGame())
		return nullptr;

	auto entitylist = SourceEngine::interfaces.EntityList();
	C_CSTeam* team = nullptr;
	for (int i = 65; i < entitylist->GetHighestEntityIndex(); ++i)
	{
		team = reinterpret_cast<C_CSTeam*>(entitylist->GetClientEntity(i));
		if (!team)
			continue;
		SourceEngine::ClientClass * client_class = team->GetClientClass();
		if (!client_class)
			continue;
		if (strcmp(client_class->m_pNetworkName, XorStr("CCSTeam")) == 0)
		{
			if (team->GetTeamNum() == static_cast<int>(teamid))
				break;
		}
	}
	return team;
}

std::string C_BaseCombatWeapon::GetWeaponName(const SourceEngine::EItemDefinitionIndex weapon)
{
	switch (weapon)
	{
	case SourceEngine::weapon_mp5sd:
		return XorStr("mp5sd");
	case SourceEngine::weapon_ak47:
		return XorStr("ak47");
	case SourceEngine::weapon_deagle:
		return XorStr("deagle");
	case SourceEngine::weapon_aug:
		return XorStr("aug");
	case SourceEngine::weapon_fiveseven:
		return XorStr("fiveseven");
	case SourceEngine::weapon_m249:
		return XorStr("m249");
	case SourceEngine::weapon_m4a1:
		return XorStr("m4a1");
	case SourceEngine::weapon_awp:
		return XorStr("awp");
	case SourceEngine::weapon_bizon:
		return XorStr("bizon");
	case SourceEngine::weapon_cz75a:
		return XorStr("cz75");
	case SourceEngine::weapon_elite:
		return XorStr("elite");
	case SourceEngine::weapon_famas:
		return XorStr("famas");
	case SourceEngine::weapon_g3sg1:
		return XorStr("g3sg1");
	case SourceEngine::weapon_galilar:
		return XorStr("galilar");
	case SourceEngine::weapon_glock:
		return XorStr("glock");
	case SourceEngine::weapon_hkp2000:
		return XorStr("hkp2000");
	case SourceEngine::weapon_m4a1_silencer:
		return XorStr("m4a1-s");
	case SourceEngine::weapon_mac10:
		return XorStr("mac10");
	case SourceEngine::weapon_mag7:
		return XorStr("mag7");
	case SourceEngine::weapon_mp7:
		return XorStr("mp7");
	case SourceEngine::weapon_mp9:
		return XorStr("mp9");
	case SourceEngine::weapon_negev:
		return XorStr("negev");
	case SourceEngine::weapon_nova:
		return XorStr("nova");
	case SourceEngine::weapon_p250:
		return XorStr("p250");
	case SourceEngine::weapon_p90:
		return XorStr("p90");
	case SourceEngine::weapon_revolver:
		return XorStr("revolver");
	case SourceEngine::weapon_sawedoff:
		return XorStr("sawedoff");
	case SourceEngine::weapon_scar20:
		return XorStr("scar20");
	case SourceEngine::weapon_sg556:
		return XorStr("sg556");
	case SourceEngine::weapon_ssg08:
		return XorStr("ssg08");
	case SourceEngine::weapon_tec9:
		return XorStr("tec9");
	case SourceEngine::weapon_ump:
		return XorStr("ump");
	case SourceEngine::weapon_usp_silencer:
		return XorStr("usp");
	case SourceEngine::weapon_xm1014:
		return XorStr("xm1014");
	default:
		return XorStr("-");
	}
}
