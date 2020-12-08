#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include "KeyValues.h"
#include "XorStrW.h"
#include "CMaterialReference.h"
#include "CCStrike15ItemSchema.h"

struct WeaponInfo_t;
struct WearableInfo_t;
struct SkinInfo_t;
struct StickerInfo_t;
struct PlayerInfo_t;

typedef std::vector<StickerInfo_t> vecStickerInfo;
typedef std::vector<SkinInfo_t> vecSkinInfo;
typedef std::vector<WeaponInfo_t> vecWeaponInfo;
typedef std::vector<WearableInfo_t> vecWearableInfo;
typedef std::vector<PlayerInfo_t> vecPlayerInfo;

struct WeaponSkin;
struct WearableSkin;
struct KnifeSkin;

extern std::vector<WeaponSkin> g_vWeaponSkins;
extern std::vector<WearableSkin> g_vWearablesSkins;
extern std::vector<KnifeSkin> g_vKnifesSkins;
extern vecStickerInfo g_vStickers;
extern vecStickerInfo g_vSprays;
extern std::vector<PlayerInfo_t> g_v_T_TeamSkins;
extern std::vector<PlayerInfo_t> g_v_CT_TeamSkins;
extern std::vector<PlayerInfo_t> g_v_All_TeamSkins;

struct WeaponInfo_t
{
	WeaponInfo_t(
		int itemdefinitionindex,
		int maxstickers,
		std::string stickermodel,
		std::string shortweaponname,
		std::wstring weaponname,
		std::string viewmodel,
		std::string worldmodel,
		std::string droppedmodel,
		SourceEngine::CCStrike15ItemDefinition* __game_item_definition,
		bool isknife
	);

	int iItemDefinitionIndex;
	int iMaxStickers;
	std::string szStickerModel;
	std::string szShortWeaponName;
	std::wstring szWeaponName;
	std::string szViewModel;
	std::string szWorldModel;
	std::string szDroppedModel;
	SourceEngine::CCStrike15ItemDefinition* game_item_definition;
	bool bIsKnife;
	std::string szWeaponNameAsU8;
};

struct WearableInfo_t
{
	WearableInfo_t(short itemdefinitionindex, std::string shortwearablename, std::wstring wearablename,
	               std::string viewmodel, std::string worldmodel,
	               SourceEngine::CCStrike15ItemDefinition* _game_item_definition);

	short iItemDefinitionIndex;

	std::string szShortWeaponName;
	std::wstring szWeaponName;
	std::string szViewModel;
	std::string szWorldModel;
	SourceEngine::CCStrike15ItemDefinition* game_item_definition;
	std::string szWeaponNameAsU8;
};

struct PlayerInfo_t
{
	PlayerInfo_t(short itemdefinitionindex, std::string _model_player, std::string _short_name,
	             std::wstring _item_name);
	std::string model_player;
	std::string short_name;
	std::wstring item_name;
	short iItemDefinitionIndex;
	std::string item_name_as_u8;
};

struct SkinInfo_t
{
	SkinInfo_t(int paintkitid, std::wstring skinname, std::string shortskinname,
	           SourceEngine::CPaintKit* paintkit_def = nullptr);;

	SkinInfo_t(SourceEngine::CPaintKit* paintkit_def);

	int iPaintKitID;
	std::string szShortSkinName;
	std::wstring szSkinName;
	std::string szSkinNameAsU8;
	SourceEngine::CPaintKit* m_pPaintKitDefinition;

	static bool compare(const SkinInfo_t& lhs, const SkinInfo_t& rhs)
	{
		return lhs.szSkinName < rhs.szSkinName;
	}

	bool operator==(const int PaintKit) const
	{
		return this->iPaintKitID == PaintKit;
	}
};

struct StickerInfo
{
	StickerInfo(StickerInfo_t *stickermaterial, unsigned int position)
		:
		sticker_info(stickermaterial),
		iPosition((position))
	{
	}

	StickerInfo_t *sticker_info;
	unsigned int iPosition;
};

struct StickerInfo_t
{
	StickerInfo_t(size_t stickerid, std::wstring stickername, std::string stickermaterial, bool _is_patch = false,
	              bool _is_spray = false);

	int iStickerKitID;
	std::wstring szStickerName;
	std::string szStickerNameAsU8;
	std::string szStickerMaterial;
	bool is_patch;
	bool is_spray;
	
	bool operator==(const int& _iStickerKitID) const
	{
		return _iStickerKitID == this->iStickerKitID;
	}

	static StickerInfo_t* GetSticker(const int _StickerKitID)
	{
		StickerInfo_t* result = nullptr;
		StickerInfo_t* end = g_vStickers.data() + g_vStickers.size();
		for (StickerInfo_t* it = g_vStickers.data(); it != end; ++it)
		{
			if (it->iStickerKitID == _StickerKitID)
			{
				result = it;
				break;
			}
		}
		return result;
	}
};

class StickerCompare
{
public:
	bool operator()(const StickerInfo_t& lhs, const StickerInfo_t& rhs) const
	{
		return compare(lhs, rhs);
	}

	static bool compare(const StickerInfo_t& lhs, const StickerInfo_t& rhs)
	{
		std::wstring szL = lhs.szStickerName, szR = rhs.szStickerName;
		transform(szL.begin(), szL.end(), szL.begin(), tolower);
		transform(szR.begin(), szR.end(), szR.begin(), tolower);

		return szL < szR;
	}
};

class WeaponInfoCompare
{
public:
	bool operator()(const WeaponInfo_t& lhs, const WeaponInfo_t& rhs) const
	{
		return compare(lhs, rhs);
	}

	bool operator()(std::vector<WeaponInfo_t>::const_iterator& lhs, std::vector<WeaponInfo_t>::const_iterator& rhs) const
	{
		return compare(*lhs, *rhs);
	}

	static bool compare(const WeaponInfo_t& lhs, const WeaponInfo_t& rhs)
	{
		return lhs.szWeaponName < rhs.szWeaponName;
	}
};

class WearableCompare
{
public:
	bool operator()(const WearableInfo_t& lhs, const WearableInfo_t& rhs) const
	{
		return compare(lhs, rhs);
	}

	static bool compare(const WearableInfo_t& lhs, const WearableInfo_t& rhs)
	{
		return lhs.szWeaponName < rhs.szWeaponName;
	}
};

struct stickerMaterialReference_t
{
	SourceEngine::CMaterialReference FirstPerson;
	SourceEngine::CMaterialReference ThirdPerson;
	unsigned int iPosition;
};

typedef std::map<WeaponInfo_t, vecSkinInfo, WeaponInfoCompare> mapSkinInfo;
typedef std::map<WearableInfo_t, vecSkinInfo, WearableCompare> mapWearableSkinInfo;

class Skin_loader
{
public:
	Skin_loader()
	{
		_skininfo.clear();
		_stickerinfo.clear();
		_sprayinfo.clear();
		_wearbleskininfo.clear();
		vec_weapon_info.clear();
		vec_wearable_info.clear();
		pks.clear();
		kv_values = nullptr;
		_stickerinfo.reserve(4500);
		_sprayinfo.reserve(200);
		/*status = σσ ρκΰ*/
		Load();

#ifdef _DEBUG
		Dump();
#endif
	}

	static void Dump();
	~Skin_loader()
	{
		delete kv_values;
	}

	const vecStickerInfo& GetStickerInfo() const
	{
		return _stickerinfo;
	}

	const vecStickerInfo& GetSprayInfo() const
	{
		return _sprayinfo;
	}

	const mapSkinInfo& GetSkinInfo() const
	{
		return _skininfo;
	}

	const mapWearableSkinInfo& GetWearableSkinInfo() const
	{
		return _wearbleskininfo;
	}
	bool add_to_base(SourceEngine::CPaintKit* new_paint_kit)
	{
		while(true)
		{
			auto found = pks.find(new_paint_kit->PaintKitID);
			if (found != pks.end())
			{
				if (found->second->name == new_paint_kit->name)
					return false;
				++new_paint_kit->PaintKitID;
				break;
			}
			break;
		}
		pks.insert_or_assign(new_paint_kit->PaintKitID, new_paint_kit);
		return true;
	}
	bool Load();

private:
	void LoadItems(const SourceEngine::KeyValues* pItems);
	void LoadStickerKits(const std::vector<SourceEngine::KeyValues*>& kvStickerKits);
	void LoadPaintKits(const std::vector<SourceEngine::KeyValues*>& kvPaintKits,
	                   const std::vector<SourceEngine::KeyValues*>& kvItemSets);
	void SortData();
	mapSkinInfo _skininfo; // Holds Skinsinformation
	vecStickerInfo _stickerinfo;
	vecStickerInfo _sprayinfo;
	mapWearableSkinInfo _wearbleskininfo;
	vecWeaponInfo vec_weapon_info;
	vecWearableInfo vec_wearable_info;
	vecPlayerInfo vec_ct_player_info;
	vecPlayerInfo vec_t_player_info;
	SourceEngine::KeyValues* kv_values;
	std::map <int, SourceEngine::CPaintKit*> pks;
};

struct WeaponSkin
{
	short iItemDefinitionIndex{};
	size_t iMaxStickers{};
	std::vector<SkinInfo_t> vSkins;
	std::string szStickerModel;
	std::wstring szWeaponName;
	std::string szWeaponNameAsU8;
	SourceEngine::CCStrike15ItemDefinition* game_item_definition;
	WeaponSkin();

	static WeaponSkin* GetWeaponSkinByItemDefinitionIndex(const short& iItemDefinitionIndex)
	{
		const auto end = g_vWeaponSkins.end();
		for (auto it = g_vWeaponSkins.begin(); it != end; ++it)
		{
			if (it->iItemDefinitionIndex == iItemDefinitionIndex)
				return &*it;
		}

		return nullptr;
	}

	static int iGetWeaponIndexByItemDefinitionIndex(const short& iItemDefinitionIndex)
	{
		for (auto i = 0U; i < g_vWeaponSkins.size(); ++i)
			if (g_vWeaponSkins[i].iItemDefinitionIndex == iItemDefinitionIndex)
				return static_cast<short>(i);

		return -1;
	}

	WeaponSkin(const mapWearableSkinInfo::value_type& in);

	WeaponSkin(const mapSkinInfo::value_type& in)
	{
		init_weapon(in);
	}

	bool operator()(const mapSkinInfo::value_type& in)
	{
		return init_weapon(in);
	}

	bool init_weapon(const mapSkinInfo::value_type& in);
};

struct KnifeSkin : WeaponSkin
{
	std::string szViewModel;
	std::string szWorldModel;
	std::string szDroppedModel;
	std::string szShortWeaponName;
	bool is_sequences_inited = false;
	std::vector<std::vector<int>> remap_sequences;
	KnifeSkin() = default;

	static bool bIsItemDefinitionIndexKnife(const int& iItemDefinitionIndex)
	{
		if (iItemDefinitionIndex == SourceEngine::weapon_knifegg ||
			iItemDefinitionIndex == SourceEngine::weapon_knife_ct ||
			iItemDefinitionIndex == SourceEngine::weapon_knife_t)
			return true;

		const auto end = g_vKnifesSkins.end();
		for (auto i = g_vKnifesSkins.begin(); i != end; ++i)
		{
			if (i->iItemDefinitionIndex == iItemDefinitionIndex)
				return true;
		}

		return false;
	}

	static KnifeSkin* GetKnifeSkinByItemDefinitionIndex(const int& iItemDefinitionIndex)
	{
		const auto end = g_vKnifesSkins.end();
		for (auto i = g_vKnifesSkins.begin(); i != end; ++i)
		{
			if (i->iItemDefinitionIndex == iItemDefinitionIndex)
				return &*i;
		}

		return nullptr;
	}

	static int iGetKnifeIndexByItemDefinitionIndex(const short& iItemDefinitionIndex)
	{
		for (auto i = 0U; i < g_vKnifesSkins.size(); ++i)
			if (g_vKnifesSkins[i].iItemDefinitionIndex == iItemDefinitionIndex)
				return i;

		return -1;
	}

	KnifeSkin(const mapSkinInfo::value_type& in): WeaponSkin(in)
	{
		init(in);
	}

	bool operator()(const mapSkinInfo::value_type& in)
	{
		return init(in);
	}

	bool init(const mapSkinInfo::value_type& in)
	{
		this->iMaxStickers = 0;
		this->szViewModel = in.first.szViewModel;
		this->szWorldModel = in.first.szWorldModel;
		this->szDroppedModel = in.first.szDroppedModel;
		this->szShortWeaponName = in.first.szShortWeaponName;
		this->szShortWeaponName.replace(0, 7, "");
		for (int i = 0; i < 13; ++i)
			remap_sequences.emplace_back();
		return true;
	}
};

struct WearableSkin : WeaponSkin
{
	std::string szViewModel;
	std::string szWorldModel;
	std::string szShortWeaponName;

	WearableSkin()
	{
		szWeaponName = L"default";
	}

	static bool bIsItemDefinitionIndexWearable(const int& iItemDefinitionIndex)
	{
		const auto end = g_vWearablesSkins.end();
		for (auto i = g_vWearablesSkins.begin(); i != end; ++i)
		{
			if (i->iItemDefinitionIndex == iItemDefinitionIndex)
				return true;
		}

		return false;
	}

	static WearableSkin* GetWearableSkinByItemDefinitionIndex(const int& iItemDefinitionIndex)
	{
		const auto end = g_vWearablesSkins.end();
		for (auto i = g_vWearablesSkins.begin(); i != end; ++i)
		{
			if (i->iItemDefinitionIndex == iItemDefinitionIndex)
				return &*i;
		}

		return nullptr;
	}

	static int iGetWearableIndexByItemDefinitionIndex(const int& iItemDefinitionIndex)
	{
		for (size_t i = 0; i < g_vWearablesSkins.size(); ++i)
			if (g_vWearablesSkins[i].iItemDefinitionIndex == iItemDefinitionIndex)
				return static_cast<int>(i);

		return -1;
	}

	explicit WearableSkin(const mapWearableSkinInfo::value_type& in): WeaponSkin(in)
	{
		init(in);
	}

	bool operator()(const mapWearableSkinInfo::value_type& in)
	{
		return init(in);
	}

	bool init(const mapWearableSkinInfo::value_type& in)
	{
		this->szViewModel = in.first.szViewModel;
		this->szWorldModel = in.first.szWorldModel;
		this->szShortWeaponName = in.first.szShortWeaponName;
		return true;
	}
};

struct Skins
{
	static SkinInfo_t* GetSkinByShortName(const std::string& str, std::vector<SkinInfo_t>& vSkins)
	{
		const auto end = vSkins.end();
		for (auto it = vSkins.begin(); it != end; ++it)
		{
			if (it->szShortSkinName == str)
				return &*it;
		}

		return nullptr;
	}

	static size_t GetSkinIndexBySkinInfo_t(const SkinInfo_t* skin, std::vector<SkinInfo_t>& vSkins)
	{
		if (!skin)
			return 0;

		for (size_t i = 0; i < vSkins.size(); ++i)
		{
			if (skin->iPaintKitID == vSkins.at(i).iPaintKitID)
				return i;
		}
		return 0;
	}
};

struct Stickers
{
	static int iGetStickerIndexByID(const int& iID)
	{
		for (size_t i = 0; i < g_vStickers.size(); ++i)
			if (iID == g_vStickers[i].iStickerKitID)
				return i;
		return -1;
	}
};

struct Sprays
{
	static int iGetSprayIndexByID(const int& iID)
	{
		for (size_t i = 0; i < g_vSprays.size(); ++i)
			if (iID == g_vSprays[i].iStickerKitID)
				return i;
		return -1;
	}
};
