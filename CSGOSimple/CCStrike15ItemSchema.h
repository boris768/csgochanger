#pragma once
#include <memory>
#include <vector>
#include "SourceEngine/CRC.hpp"
#include "SourceEngine/UtlVector.hpp"
#include "SourceEngine/UtlString.hpp"
#include "SourceEngine/Vector.hpp"
#include "CUtlRBTree.hpp"
#include "SourceEngine/QAngle.hpp"
#include <string>

#include "ISteamGameCoordinator.h"
#include "SourceEngine/Definitions.hpp"

namespace SourceEngine
{
	typedef unsigned __int64	itemid_t;
	typedef unsigned __int16	item_definition_index_t;
	typedef unsigned __int16	attrib_definition_index_t;
	typedef unsigned __int32	attrib_value_t;
	typedef int		econ_tag_handle_t;
	typedef	unsigned __int16	equipped_class_t;
	typedef unsigned __int16	equipped_slot_t;
	typedef unsigned __int8	equipped_preset_t;
#define INVALID_ECON_TAG_HANDLE	((econ_tag_handle_t)-1)
#define INVALID_EQUIPPED_SLOT	((equipped_slot_t)-1)
#define INVALID_EQUIPPED_SLOT_BITPACKED 0x3F
#define INVALID_STYLE_INDEX		((style_index_t)-1)
#define INVALID_PRESET_INDEX	((equipped_preset_t)-1)

	template <typename T>
	class CDefLess
	{
	public:
		CDefLess() {}
		CDefLess(int i) {}
		bool operator()(const T& lhs, const T& rhs) const { return (lhs < rhs); }
		bool operator!() const { return false; }
	};
	
	struct SOID_t
	{
		SOID_t()
			: m_id(0)
			, m_type(0)
			, m_padding(0)
		{
		}

		SOID_t(unsigned __int32 type, unsigned __int64 id)
			: m_id(id)
			, m_type(type)
			, m_padding(0)
		{
		}
		
		//initializes the soid fields
		void Init(unsigned __int32 type, unsigned __int64 id)
		{
			m_type = type;
			m_id = id;
		}

		unsigned __int64 ID() const
		{
			return m_id;
		}

		unsigned __int32 Type() const
		{
			return m_type;
		}

		bool IsValid()
		{
			return m_type != 0;
		}

		bool operator==(const SOID_t& rhs) const
		{
			return m_type == rhs.m_type && m_id == rhs.m_id;
		}

		bool operator!=(const SOID_t& rhs) const
		{
			return m_type != rhs.m_type || m_id != rhs.m_id;
		}

		bool operator<(const SOID_t& rhs) const
		{
			if (m_type == rhs.m_type)
			{
				return m_id < rhs.m_id;
			}
			return m_type < rhs.m_type;
		}

		unsigned __int64 m_id;
		unsigned __int32 m_type;
		unsigned __int32 m_padding; // so structure is 16 bytes
	};
	
	class CAttribute_String
	{
	public:
		virtual ~CAttribute_String() = 0;
	 
		int32_t FieldSet{};
		char** Value{};
	 
	private:
		int32_t pad0[2]{};
	};
	 
	union attribute_data_union_t
	{
		bool bValue;
		int iValue;
		float fValue;
		Vector vValue[3];
		CAttribute_String* szValue;
	};

	class CEconItemAttributeDefinition;
	class ISchemaAttributeType
	{
	public:
		virtual ~ISchemaAttributeType() = 0;
		virtual int GetTypeUniqueIdentifier() = 0;
		virtual void LoadEconAttributeValue() = 0;
		virtual void ConvertEconAttributeValueToByteStream() = 0;
		virtual bool BConvertStringToEconAttributeValue( const CEconItemAttributeDefinition*, const char*, attribute_data_union_t* ) = 0;
	};

	struct attr_type_t
	{
		char* name;
		ISchemaAttributeType* type;
	};

	struct AlternateIconData_t
	{
	private:
		int32_t pad0;
	 
	public:
		CUtlString icon_path;
		CUtlString icon_path_large;
	 
	private:
		CUtlString pad1[2];
	};

	struct kill_eater_score_type_t
	{
		int id;
		char* type_name;
		char* model_attribute;
		char* level_data;
		bool use_level_data;
	};
 
	class CEconItemRarityDefinition
	{
	public:
		int value;
		int color;
		CUtlString name;
		CUtlString loc_key;
		CUtlString loc_key_weapon;
		CUtlString loot_list;
		CUtlString recycle_list;
		CUtlString drop_sound;
		CUtlString next_rarity;
	 
	private:
		int32_t pad0[2];
	 
	public:
		float weight;
	};
	 
	class CEconItemQualityDefinition
	{
	public:
		int value;
		char* name;
		int weight;
		bool canSupportSet;
		char* hexColor;
	};
	
	class KeyValues;

	class CEconItemAttributeDefinition
	{
	public:
		virtual ~CEconItemAttributeDefinition() = 0;
	 
		KeyValues* kv;
		uint16_t id;
		ISchemaAttributeType* attribute_type;
		bool hidden;
		bool force_output_description;
		bool stored_as_integer;
		bool instance_data;
		int asset_class_export;
		int asset_class_bucket;
		int effect_type;
		int description_format;
		char* description_string;
		char* description_tag;
		char* armory_desc;
		int score;
		char* name;
		char* attribute_class;
	 
	private:
		int32_t pad0;
	};
	 
	class CEconCraftingRecipeDefinition
	{
	public:
		virtual ~CEconCraftingRecipeDefinition() = 0;
	 
		int definitionindex; // Don't ask me why Valve sometimes uses dword and sometimes word.
		CUtlString name;
		CUtlString n_A;
		CUtlString desc_inputs;
		CUtlString desc_outputs;
		CUtlString di_A;
		CUtlString di_B;
		CUtlString di_C;
		CUtlString do_A;
		CUtlString do_B;
		CUtlString do_C;
		wchar_t name_localized[64];
		wchar_t requires_produces_localized[512];
		bool all_same_class;
		bool all_same_slot;
		int add_class_usage_to_output;
		int add_slot_usage_to_output;
		int add_set_to_output;
		bool always_known;
		bool premium_only;
		int category;
		int filter;
	 
	private: // Some vectors and other things
		int32_t pad0[47];
	};

	struct item_list_entry_t
	{
		int32_t item; //0x0000
		int32_t paintkit; //0x0004
		int32_t N00000051; //0x0008
		int32_t N00000052; //0x000C
		int32_t N00000053; //0x0010
		int32_t N00000054; //0x0014
		bool unk_flag; //0x0018
		char pad_0019[3]; //0x0019
	};
	 
	struct itemset_attrib_t;
	class CEconItemSetDefinition
	{
	public:
		void* vtable;//virtual ~CEconItemSetDefinition();
	 
		char* key_name;
		char* name;
		char* unlocalized_name;
		char* set_description;
		CUtlVector<item_list_entry_t> items;
		int store_bundle;
		bool is_collection;
		bool is_hidden_set;
		CUtlVector<itemset_attrib_t*> attributes;//here is no *
	};

	
	class CEconLootListDefinition
	{
	public:
		virtual ~CEconLootListDefinition() = 0;
	 
	private:
		int32_t pad0[5]; // a vector
	 
	public:
		char* name;
		CUtlVector<item_list_entry_t> items;
		int hero;
		bool public_list_contents;
		bool contains_stickers_autographed_by_proplayers;
		bool will_produce_stattrak;
		float totalweights;
		CUtlVector<float> weights;
		
	private:
		int32_t pad1[6]; // a vector + something else
	};



	class CPaintKit;

	struct offsets_CCStrike15ItemSchema
	{
		ptrdiff_t CUtlHashMapLarge__Find{};

		using AddPaintKitDefinition_t = int(__thiscall*)(void*, int, CPaintKit*);
		AddPaintKitDefinition_t AddPaintKitDefinition{};
		using GetPaintKitDefinitionByName_t = CPaintKit*(__thiscall*)(void*, const char*);
		GetPaintKitDefinitionByName_t GetPaintKitDefinitionByName{};
		using GetPaintKitDefinition_t = CPaintKit*(__thiscall*)(void*, unsigned int);
		GetPaintKitDefinition_t GetPaintKitDefinition{};
		ptrdiff_t alternative_icons2_offset{};
		ptrdiff_t paint_kits_rarity_offset{};
		ptrdiff_t Items{};
		ptrdiff_t s_nCurrentPaintKitId{};
		offsets_CCStrike15ItemSchema();
	};
	extern offsets_CCStrike15ItemSchema* offsets_cc_strike15_item_schema;

	class CCStrike15ItemSchema;
	//class KeyValues;

	class CStickerKit // I came up with this some months ago, still seems to be up-to-date...
	{
	public:
		CStickerKit(): nID(0), nRarity(1), m_nEventID(0), m_nEventTeamID(0), m_nPlayerID(0),
		               bMaterialPathIsAbsolute(false),
		               flRotateStart(0),
		               flRotateEnd(0), flScaleMin(1.0f),
		               flScaleMax(1.0f),
		               flWearMin(0), flWearMax(1.f),
		               m_pKVItem(nullptr)
		{
		}
		bool InitFromKeyValues(KeyValues* pKVEntry);
		int nID;
		int nRarity;
		CUtlString sName;
		CUtlString sDescriptionString;
		CUtlString sItemName;
		CUtlString sMaterialPath;
		CUtlString sMaterialPathNoDrips;
		CUtlString m_strInventoryImage;

		int m_nEventID;
		int m_nEventTeamID;
		int m_nPlayerID;

		bool bMaterialPathIsAbsolute;

		float flRotateStart;
		float flRotateEnd;

		float flScaleMin;
		float flScaleMax;

		float flWearMin;
		float flWearMax;
		CUtlString m_sIconURLSmall;
		CUtlString m_sIconURLLarge;
		KeyValues* m_pKVItem;
	};

	class CEconColorDefinition
	{
	public:
		char* name;
		char* color_name;
		char* hex_color;
	};

	class CPaintKit
	{
	public:
		int PaintKitID;
		CUtlString name;
		CUtlString description_string;
		CUtlString description_tag;
		CUtlString same_name_family_aggregate;
		CUtlString pattern;
		CUtlString normal;
		CUtlString logo_material;
		int unk1;
		int rarity;
		int style;
		uint8_t color[4][4];
		uint8_t logocolor[4][4];
		float wear_default;
		float wear_remap_min;
		float wear_remap_max;
		int8_t seed;
		int8_t phongexponent;
		int8_t phongalbedoboost;
		int8_t phongintensity;
		float pattern_scale;
		float pattern_offset_x_start;
		float pattern_offset_x_end;
		float pattern_offset_y_start;
		float pattern_offset_y_end;
		float pattern_rotate_start;
		float pattern_rotate_end;
		float logo_scale;
		float logo_offset_x;
		float logo_offset_y;
		float logo_rotation;
		bool ignore_weapon_size_scale;
		char pad2[3];
		int view_model_exponent_override_size;
		bool only_first_material;
		bool use_normal;
		char pad[2];
		float pearlescent;
		CUtlString vmt_path;
		KeyValues* vmt_overrides;

		CPaintKit() : PaintKitID(0), unk1(0), rarity(1), style(0), color{ 0,0,0,0 }, logocolor{ 0,0,0,0 },
					 wear_default(0.f),
		             wear_remap_min(0.f),
		             wear_remap_max(1.f),
		             seed(0),
		             phongexponent(16),
		             phongalbedoboost(0),
		             phongintensity(-1),
		             pattern_scale(0.5f),
		             pattern_offset_x_start(0.f),
		             pattern_offset_x_end(0.f),
		             pattern_offset_y_start(0.f),
		             pattern_offset_y_end(0.f),
		             pattern_rotate_start(0.f), pattern_rotate_end(0.f),
		             logo_scale(1.8f),
		             logo_offset_x(1.f), logo_offset_y(2.f),
		             logo_rotation(0),
		             ignore_weapon_size_scale(false), pad2{},
		             view_model_exponent_override_size(256),
		             only_first_material(false), use_normal(false), pad{},
		             pearlescent(0.f), vmt_overrides(nullptr)
		{
		}

		int BInitFromKV(KeyValues* kv, CPaintKit* default_paintkit);
	}; //Size=0x00DC

	class CEconGraffitiTintDefinition
	{
	public:
		int id;
		char* name;
		char* hex_color;
	 
		uint8_t b;
		uint8_t g;
		uint8_t r;
	};

	struct StickerData_t_source
	{
		char viewmodel_geometry[128]; //0x0000 
		char viewmodel_material[128]; //0x0080 
		Vector worldmodel_decal_pos; //0x0100 
		Vector worldmodel_decal_end; //0x010C 
		char worldmodel_decal_bone[32]; //0x0118 
	};

	class CEconMusicDefinition
	{
	public:
		int id;
		char* name;
		char* loc_name;
		char* loc_description;
		char* pedestal_display_model;
		char* image_inventory;
	 
	private:
		int32_t pad0_[2]{0, 0};
	};

	class CEconQuestDefinition
	{
	public:
		int id;
		char* name;
		char* mapgroup;
		char* map;
		char* gamemode;
		char* quest_reward;
		char* expression;
		char* expr_bonus;
		CCopyableUtlVector<unsigned int> points;
	 
		enum
		{
			is_an_event = 1,
			event_allow_individual_maps
		} flags;
	 
		int difficulty;
		int operational_points;
		int xp_reward;
		int xp_bonus_percent;
		int target_team;
		int required_event;
	 
	private:
		int32_t pad0;
	 
	public:
		int gametype; // gamemode & 15 | ( mapgroup << 8 )
		char* loc_name;
		char* loc_shortname;
		char* loc_description;
		char* loc_huddescription;
		KeyValues* string_tokens;
		char* loc_bonus;
		char* quest_icon;
	};
	 
	class CEconCampaignDefinition
	{
	public:
		int id;
	 
	private:
		int32_t pad0;
	 
	public:
		char* loc_name;
		char* loc_description;
	 
		class CEconCampaignNodeDefinition
		{
		public:
			class CEconCampaignNodeStoryBlockDefinition
			{
			public:
				char* content_file;
				char* character_name;
				char* expression;
				char* description;
			};
			CUtlVector<CEconCampaignNodeStoryBlockDefinition*> story_blocks;
	 
			int id;
			int quest_index;
			int parentid;
			CUtlVector<unsigned int> questunlocks;
		};
		CUtlHashMapLarge<int, CEconCampaignNodeDefinition> quests;
		CUtlHashMapLarge<int, CEconCampaignNodeDefinition> quests2; // ???
	 
		int season_number;
	};
	 
	class CSkirmishModeDefinition
	{
	public:
		int id;
		char* name;
		char* gamemode;
		int igamemode;
		char* server_exec;
		char* loc_name;
		char* loc_rules;
		char* loc_description;
		char* loc_details;
		char* icon;
	};
	 
	class CProPlayerData
	{
	public:
		KeyValues* kv;
		int id;
		CUtlString name;
		CUtlString code;
		int dob;
		CUtlString geo;
	};
	 
	class CItemLevelingDefinition
	{
	public:
		int id;
		int score;
		char* rank_name;
	 
	private:
		int32_t pad0[2];
	};
	 
	class CPipRankData
	{
	public:
		class PipRankInfo
		{
			int32_t pad0;
	 
		public:
			int8_t id;
			int8_t pips;
			int8_t winstreak;
			int8_t loss;
		};
	 
		CUtlVector<PipRankInfo> rank_info;
	};
	 
	struct static_attrib_t // Get a CEconItemAttributeDefinition* with m_Attributes[static_attrib_t.id]
	{
		uint16_t id;
		attribute_data_union_t value;
		bool force_gc_to_generate;
	};

	struct codecontrolledbodygroupdata_t;
	struct attachedmodel_t;
	struct attachedparticle_t;
	class CEconStyleInfo;
	class AssetInfo
	{
		int32_t pad0;
	 
	public:
		CUtlMap<const char*, int, unsigned short> player_bodygroups;
		CUtlMap<const char*, codecontrolledbodygroupdata_t, unsigned short> code_controlled_bodygroup;
		int skin;
		bool use_per_class_bodygroups;
		CUtlVector<attachedmodel_t*> attached_models;//here is no *
		CUtlVector<attachedparticle_t*> attached_particles;//here is no *
		char* custom_sounds[10];
		char* material_override;
		int muzzle_flash;
	 
	private:
		int32_t pad1;
	 
	public:
		char* particle_effect;
		char* particle_snapshot;
		char* sounds[18];
		char* primary_ammo;
		char* secondary_ammo;
		char* weapon_type;
		char* addon_location;
		char* eject_brass_effect;
		char* muzzle_flash_effect_1st_person;
		char* muzzle_flash_effect_1st_person_alt;
		char* muzzle_flash_effect_3rd_person;
		char* muzzle_flash_effect_3rd_person_alt;
		char* heat_effect;
		char* player_animation_extension;
		Vector grenade_smoke_color;
		int vm_bodygroup_override;
		int vm_bodygroup_state_override;
		int wm_bodygroup_override;
		int wm_bodygroup_state_override;
		bool skip_model_combine;
		CUtlVector<const char*> animation_modifiers;
		CUtlVector<CEconStyleInfo*> styles;
	};
	 
	struct WeaponPaintableMaterial_t
	{
		char Name[128];
		char OrigMat[128];
		char FolderName[128];
		int ViewmodelDim;
		int WorldDim;
		float WeaponLength;
		float UVScale;
		bool BaseTextureOverride;
		bool MirrorPattern;
	};
	 
	class IEconTool;
	struct LightDesc_t;

	class CCStrike15ItemDefinition //TODO: reclass it fully
	{
	public:

		//		virtual bool BInitFromKV(KeyValues* kv, CCStrike15ItemSchema* item_schema, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>* error_log) = 0;

		virtual uint16_t GetDefinitionIndex() const = 0;
		virtual const char* GetPrefabName() const = 0;
		virtual const char* GetItemBaseName() const = 0;
		virtual const char* GetItemTypeName() const = 0;
		virtual const char* GetItemDesc() const = 0;
		virtual const char* GetInventoryImage() const = 0;
		virtual const char* GetBasePlayerDisplayModel() const = 0;
		virtual const char* GetWorldDisplayModel() const = 0;
		virtual const char* GetExtraWearableModel() const = 0;
		virtual int GetLoadoutSlot() const = 0;
		virtual KeyValues* GetRawDefinition() const = 0;
		virtual int GetHeroID() const = 0;
		virtual int GetRarity() const = 0;
		virtual CUtlVector<int>* GetItemSets() const = 0;
		virtual int GetBundleItemCount() const = 0;
		virtual void* GetBundleItem( int ) const = 0;
		virtual bool IsBaseItem() const = 0;
		virtual bool IsPublicItem() const = 0;
		virtual bool IsBundle() const = 0;
		virtual bool IsPackBundle() const = 0;
		virtual bool IsPackItem() const = 0;
		virtual void* BInitVisualBlockFromKV( KeyValues*, void*, void* ) = 0;
		virtual void* BInitFromKV( KeyValues*, void*, void* ) = 0;
		virtual void* BInitFromTestItemKVs( int, KeyValues*, void* ) = 0;
		virtual void* GeneratePrecacheModelStrings( bool, void* ) const = 0;
		virtual void* GeneratePrecacheSoundStrings( void* ) const = 0;
		virtual void* GeneratePrecacheEffectStrings( void* ) const = 0;
		virtual void* CopyPolymorphic( const CCStrike15ItemDefinition* ) = 0;
		virtual int GetItemTypeID() const = 0;
		virtual bool IsDefaultSlotItem() const = 0;
		virtual bool IsPreviewableInStore() const = 0;
		virtual int GetBundleItemPaintKitID( int ) const = 0;
		virtual const char* GetWorldDroppedModel() const = 0;
		virtual const char* GetHolsteredModel() const = 0;
		virtual const char* GetZoomInSound() const = 0;
		virtual const char* GetZoomOutSound() const = 0;
		virtual const char* GetIconDisplayModel() const = 0;
		virtual const char* GetBuyMenuDisplayModel() const = 0;
		virtual const char* GetPedestalDisplayModel() const = 0;
		virtual const char* GetMagazineModel() const = 0;
		virtual const char* GetScopeLensMaskModel() const = 0;
		virtual const char* GetUidModel() const = 0;
		virtual const char* GetStatTrakModelByType( unsigned int ) const = 0;
		virtual int GetNumSupportedStickerSlots() const = 0;
		virtual const char* GetStickerSlotModelBySlotIndex( unsigned int ) const = 0;
		virtual void* GetStickerSlotWorldProjectionStartBySlotIndex( unsigned int ) const = 0;
		virtual void* GetStickerSlotWorldProjectionEndBySlotIndex( unsigned int ) const = 0;
		virtual void* GetStickerWorldModelBoneParentNameBySlotIndex( unsigned int ) const = 0;
		virtual void* GetStickerSlotMaterialBySlotIndex( unsigned int ) const = 0;
		virtual const char* GetIconDefaultImage() const = 0;
		virtual void* GetParticleFile() const = 0;
		virtual void* GetParticleSnapshotFile() const = 0;
		virtual bool IsRecent() const = 0;
		virtual bool IsContentStreamable() const = 0;
		virtual void* IgnoreInCollectionView() const = 0;
		virtual void* GeneratePrecacheModelStrings( bool, void* ) = 0;

	KeyValues* kv;
	uint16_t id;
	CUtlVector<uint16_t> associated_items;
	bool m_bEnabled;
		const char* m_szPrefab;

		uint8_t min_ilevel;
		uint8_t max_ilevel;
		uint8_t item_rarity;
		uint8_t item_quality;
		uint8_t forced_item_quality;
		uint8_t default_drop_quality;
		uint8_t default_drop_quantity;
		CUtlVector<static_attrib_t> attributes;
		uint8_t popularity_seed;
		KeyValues* portraits;
		char* item_name;
		bool propername;
		char* item_type_name;
		int type_id; // crc32( item_type_name )
		char* item_description;
		int expiration_date;
		int creation_date;
		char* model_inventory;
		char* image_inventory;
		CUtlVector<const char*> image_inventory_overlay;
		int image_inventory_pos_x;
		int image_inventory_pos_y;
		int image_inventory_size_w;
		int image_inventory_size_h;
		char* m_pszBaseDisplayModel;
		bool m_bLoadOnDemand;
		bool m_bHasBeenLoaded;
		bool m_bHideBodyGroupsDeployedOnly;

		bool hide_bodygroups_deployed_only;
		char* model_world;
		char* model_dropped;
		char* model_holstered;
		char* extra_wearable;
		char m_szWorldDroppedModel[80];
	 
		CUtlVector<StickerData_t_source> stickers;
		char* icon_default_image;
		bool attach_to_hands;
		bool attach_to_hands_vm_only;
		bool flip_viewmodel;
		bool act_as_wearable;
		CUtlVector<int> item_sets;
		AssetInfo* visuals;
		int item_type;

		bool allow_purchase_standalone;
		char* brass_eject_model;
		char* zoom_in_sound;
		char* zoom_out_sound;

		IEconTool* tool;

	private:
		int32_t pad4[3];

	public:
		int sound_material;
		bool disable_style_selector;

	private:
		int32_t pad5[8];

	public:
		char* particle_file;
		char* particle_snapshot;
		char* loot_list_name;

	private:
		int32_t pad6[4];

	public:
		CUtlVector<WeaponPaintableMaterial_t> paint_data;

		struct
		{
			QAngle* camera_angles;
			Vector* camera_offset;
			float camera_fov;
			LightDesc_t* lights[4];

		private:
			int32_t pad0;
		} *inventory_image_data;

		char* m_pszItemClassname;
		char* m_pszItemLogClassname;
		char* m_pszItemIconClassname;
		char* m_pszDefinitionName;
		bool hidden;
		bool show_in_armory;
		bool baseitem;
		bool default_slot_item;
		bool import;
		bool one_per_account_cdkey;

	private:
		int32_t pad7;

	public:
		char* armory_desc;
		CCStrike15ItemDefinition* armory_remapdef;
		CCStrike15ItemDefinition* store_remapdef;
		char* armory_remap;
		char* store_remap;
		char* class_token_id;
		char* slot_token_id;
		int drop_type;
		int holiday_restriction;
		int subtype;

	private:
		int32_t pad8[4];

	public:
		CUtlMap<unsigned int, const char*, unsigned short> alternate_icons;

	private:
		int32_t pad9[9];

	public:
		bool not_developer;
		bool ignore_in_collection_view;

		// This is where CCStrike15ItemDefinition begins
		int item_sub_position;
		int item_gear_slot;
		int item_gear_slot_position;
		int m_iAnimationSlot;
		char* m_pszPlayerDisplayModel[4];
		int class_usage[4];
	 
	private:
		int32_t pad10[2];
	}; //Size=0x0270

	template <typename T>
	struct  CUtlSomeThing
	{
		CUtlMemory<T> m_Items;
		int field_C;
		int field_10;
		signed __int64 field_14;
		uint8_t gap_1C[4];
		void* field_20;
		CUtlMemory<T> field_24;
		signed int field_30;
		signed int field_34;
		signed int m_nSize;
		int field_3C;
		signed int field_40;
	};

	struct mass
	{
		int id;
		CCStrike15ItemDefinition* item;
		int pad;
	};

	class IEconItemSchema
	{
	public:
		// Schema Parsing
		virtual bool BInit(const char* fileName, const char* pathID, CUtlVector<CUtlString>* pVecErrors = NULL) = 0;
		virtual bool BInitLootLists(KeyValues* pKVLootLists, KeyValues* pKVRandomAttributeTemplates, CUtlVector<CUtlString>* pVecErrors, bool bServerLists) = 0;

//		virtual KeyValues* GetRawDefinition() const = 0;
//
//		virtual int GetItemDefinitionCount() = 0;
//		virtual CCStrike15ItemDefinition* GetItemDefinitionInterface(int iDefIndex) = 0;
//		virtual CCStrike15ItemDefinition* GetDefaultItemDefinitionInterface() = 0;
//
//		virtual int GetLoadoutSlotCount() = 0;
//		virtual const char* GetLoadoutSlotName(int iSlot) = 0;
//		virtual int GetLoadoutSlot(const char* pszSlotName) = 0;
//		virtual int GetCharacterSlotType(int iCharacter, int iSlot) = 0;
//		virtual int GetCharacterID(const char* pszCharacterName) = 0;
//		virtual int GetCharacterCount(void) = 0;
//		virtual const char* GetCharacterName(int iCharacter) = 0;
//		virtual const char* GetCharacterModel(int iCharacter) = 0;
//
//		// Particle Systems
//		virtual void* GetAttributeControlledParticleSystem(int id) = 0;
//		virtual void* GetAttributeControlledParticleSystemByIndex(int id) = 0;
//		virtual void* FindAttributeControlledParticleSystem(const char* pchSystemName, int* outID = NULL) = 0;
//		virtual int GetNumAttributeControlledParticleSystems() const = 0;
//
//		// Quality
//		virtual int GetQualityDefinitionCount() = 0;
//		virtual const char* GetQualityName(uint8_t iQuality) = 0;
//		virtual int GetQualityIndex(const char* pszQuality) = 0;
//
//		// Rarity
//		virtual int GetRarityDefinitionCount() const = 0;
//		virtual const char* GetRarityName(uint8_t iRarity) = 0;
//		virtual const char* GetRarityLocKey(uint8_t iRarity) = 0;
//		virtual const char* GetRarityColor(uint8_t iRarity) = 0;
//		virtual const char* GetRarityLootList(uint8_t iRarity) = 0;
//		virtual int GetRarityIndex(const char* pszRarity) = 0;
//
//		// Attributes
//		virtual const IEconItemAttributeDefinition* GetAttributeDefinitionInterface(int iAttribIndex) const = 0;
//
//		// Item Sets
//		virtual int GetItemSetCount(void) const = 0;
//		virtual const IEconItemSetDefinition* GetItemSet(int iIndex) const = 0;
//
//		// Loot Lists
//		virtual void PurgeLootLists(void) = 0;
//		virtual const IEconLootListDefinition* GetLootListInterfaceByName(const char* pListName, int* out_piIndex = NULL) = 0;
//		virtual const IEconLootListDefinition* GetLootListInterfaceByIndex(int iIdx) const = 0;
//		virtual const int GetLootListIndex(const char* pListName) = 0;
//		virtual const int GetLootListInterfaceCount(void) const = 0;
//
//		virtual KeyValues* FindDefinitionPrefabByName(const char* pszPrefabName) const = 0;
//
//		// Schema Init, for Editing
//		virtual attachedparticlesystem_t GetAttachedParticleSystemInfo(KeyValues* pParticleSystemKV, int32 nItemIndex) const = 0;
//		virtual bool BInitAttributeControlledParticleSystems(KeyValues* pKVParticleSystems, CUtlVector<CUtlString>* pVecErrors) = 0;
//		virtual bool BInitItems(KeyValues* pKVAttributes, CUtlVector<CUtlString>* pVecErrors) = 0;
//		virtual bool BInitItemSets(KeyValues* pKVItemSets, CUtlVector<CUtlString>* pVecErrors) = 0;
//
//		virtual bool DeleteItemDefinition(int iDefIndex) = 0;
//
//		// Iterating over the item definitions. 
//		virtual CEconItemDefinition* GetItemDefinitionByName(const char* pszDefName) = 0;
//		virtual const CEconItemDefinition* GetItemDefinitionByName(const char* pszDefName) const = 0;
//
//#if defined(CLIENT_DLL) || defined(GAME_DLL) || defined(CLIENT_EDITOR_DLL)
//		virtual void ItemTesting_CreateTestDefinition(int iCloneFromItemDef, int iNewDef, KeyValues* pNewKV) = 0;
//		virtual void ItemTesting_DiscardTestDefinition(int iDef) = 0;
//#endif
//
//		// sound materials
//		virtual const char* GetSoundMaterialNameByID(int nSoundMaterialID) = 0;
//		virtual int GetSoundMaterialID(const char* pszSoundMaterial) = 0;
//
//		// iterating sound materials
//		virtual int GetSoundMaterialCount(void) = 0;
//		virtual int GetSoundMaterialIDByIndex(int nIndex) = 0;			// returns the Nth sound material ID
	};
	
	class IEconItemSystem
	{
	public:
		virtual IEconItemSchema* GetItemSchemaInterface() = 0;

		// Item Reservations
		virtual void RequestReservedItemDefinitionList() = 0;
		virtual void ReserveItemDefinition(uint32_t nDefIndex, const char* pszUserName) = 0;
		virtual void ReleaseItemDefReservation(uint32_t nDefIndex, const char* pszUserName) = 0;
	};
	
	class CCStrike15ItemSchema :IEconItemSchema
	{
	public:
		
		virtual bool BInit(const char* fileName, const char* pathID, CUtlVector<CUtlString>* pVecErrors = NULL) = 0;
		virtual bool BInitLootLists(KeyValues* pKVLootLists, KeyValues* pKVRandomAttributeTemplates, CUtlVector<CUtlString>* pVecErrors, bool bServerLists) = 0;
		virtual int GetRawDefinition() = 0;
		virtual int GetItemDefinitionCount() = 0;
		virtual int GetItemDefinitionMutable() = 0;
		virtual void GetDefaultItemDefinitionInterface(int) = 0;
		virtual void unused1() = 0;
		virtual void unused2() = 0;
		virtual void unused3() = 0;
		virtual void zero() = 0;
		virtual void unused4() = 0;
		virtual void unused5() = 0;
		virtual void unused6() = 0;
		virtual void unused7() = 0;
		virtual int GetAttributeControlledParticleSystem(int) = 0;
		virtual int GetAttributeControlledParticleSystemByIndex(int) = 0;
		virtual int FindAttributeControlledParticleSystem(char const*, int*) = 0;
		virtual int GetNumAttributeControlledParticleSystems() = 0;
		virtual int GetQualityDefinitionCount() = 0;
		virtual char* GetQualityName(int) = 0;
		virtual int GetQualityIndex(char const*) = 0;
		virtual char* GetItemTypeName() = 0;
		virtual char* sub_59DFE0() = 0;
		virtual char* GetRarityName(int) = 0;
		virtual char* GetRarityLocKey(int) = 0;
		virtual char* GetRarityLootList(int) = 0;
		virtual int GetRarityIndex(const char*) = 0;
		virtual void* GetAttributeDefinition(int) = 0;
		virtual int GetItemSetCount() = 0;
		virtual CEconItemSetDefinition* GetItemSet(int) = 0;
		virtual void PurgeLootLists() = 0;
		virtual int GetLootListInterfaceByName(char const*, int*) = 0;
		virtual int GetLootListInterfaceByIndex(int) = 0;
		virtual int GetLootListIndex(char const*) = 0;
		virtual int GetLootListInterfaceCount() = 0;
		virtual int FindDefinitionPrefabByName(char const*) = 0;
		virtual int GetAttachedParticleSystemInfo(KeyValues*, int) = 0;
		virtual bool BInitAttributeControlledParticleSystems(KeyValues*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*) = 0;
		virtual bool BInitItems(KeyValues*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*) = 0;
		virtual bool BInitItemSets(KeyValues*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*) = 0;
		virtual bool DeleteItemDefinition(int) = 0;
		virtual CCStrike15ItemDefinition* GetItemDefinitionByName_0(char const*) = 0;
		virtual CCStrike15ItemDefinition* GetItemDefinitionByName(char const*) = 0;
		virtual int ItemTesting_CreateTestDefinition(int, int item_def_index, KeyValues*) = 0;
		virtual void ItemTesting_DiscardTestDefinition(int) = 0;
		virtual char* GetSoundMaterialNameByID(int) = 0;
		virtual int GetSoundMaterialID(char const*) = 0;
		virtual int GetSoundMaterialCount() = 0;
		virtual int GetSoundMaterialIDByIndex(int) = 0;
		virtual ~CCStrike15ItemSchema() = 0;
		virtual int GetToolType(char const*) = 0;
		virtual int GetNumPrefabs() = 0;
		virtual char* GetPrefabName(int) = 0;
		virtual void unused8(char const*) = 0;
		virtual CCStrike15ItemDefinition* CreateEconItemDefinition() = 0;
		virtual void* CreateCraftingRecipeDefinition() = 0;
		virtual void* CreateEconStyleInfo() = 0;
		virtual void* CreateEconToolImpl(char const*, char const*, char const*, void*, KeyValues*) = 0;
		virtual void Reset() = 0;
		virtual int CalculateNumberOfConcreteItems(CCStrike15ItemDefinition const*) = 0;
		virtual bool BInitItems_0() = 0;
		virtual bool BInitBundles(CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*) = 0;
		virtual bool BInitPaymentRules(CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*) = 0;
		virtual bool BInitSchema(KeyValues* pKVRawDefinition, CUtlVector<CUtlString>* pVecErrors = NULL);


		bool m_bSchemaUpdatesEnabled;
		uint32_t m_unResetCount;
		private:
			int32_t pad0[8];

		public:
			KeyValues* kv;

		private:
			int32_t pad1;
	public:
			int m_unVersion;

			uint16_t first_valid_class;
			uint16_t last_valid_class;
			uint16_t first_valid_item_slot;
			uint16_t last_valid_item_slot;
			int num_item_presets;
			int item_level_min;
			int item_level_max;

		//private:
		//	int32_t pad2[2];

			CUtlVector<attr_type_t> attribute_types;
			// Contains the list of rarity definitions
			CUtlMap<int, CEconItemRarityDefinition, int, CDefLess<int> >		m_mapRarities;

			// Contains the list of quality definitions read in from all data files.
			CUtlMap<int, CEconItemQualityDefinition, int, CDefLess<int> >		m_mapQualities;

		//private:
		//	int32_t pad3[18];

		//public:
			//CUtlHashMapLarge<int, CCStrike15ItemDefinition*> items;
			CUtlSomeThing< CCStrike15ItemDefinition*> items;
			CCStrike15ItemDefinition* m_DefaultItem; // The first item
			CUtlVector<CEconItemAttributeDefinition*> attributes;
			CUtlHashMapLarge<int, CEconCraftingRecipeDefinition*> recipes;
			CUtlMap<const char*, CEconItemSetDefinition> item_sets;
			CUtlHashMapLarge<int, const char*> revolving_loot_lists;
			CUtlHashMapLarge<int, const char*> quest_reward_loot_lists;
			CUtlMap<const char*, CEconLootListDefinition> client_loot_lists;

		private:
			int32_t pad4[14];

		public:
			CUtlHashMapLarge<int, AlternateIconData_t> alternate_icons2;

		private:
			int32_t pad5[18];

		public:
			CUtlHashMapLarge<int, CPaintKit*> paint_kits;
			CUtlHashMapLarge<int, CStickerKit*> sticker_kits;
			CUtlMap<const char*, CStickerKit*> sticker_kits_namekey;

		private:
			int32_t pad6[24];

		public:
			CUtlMap<const char*, KeyValues*> prefabs;
			CUtlVector<CEconColorDefinition*> colors;
			CUtlVector<CEconGraffitiTintDefinition*> graffiti_tints;

		private:
			int32_t pad7[20];

		public:
			CUtlHashMapLarge<int, CEconMusicDefinition*> music_definitions;
			CUtlHashMapLarge<int, CEconQuestDefinition*> quest_definitions;
			CUtlHashMapLarge<int, CEconCampaignDefinition*> campaign_definitions;

		private:
			int32_t pad8[27];

		public:
			CUtlHashMapLarge<int, CSkirmishModeDefinition*> skirmish_modes;
			CUtlHashMapLarge<unsigned int, CProPlayerData*> pro_players;

		private:
			int32_t pad9[53];

		public:
			CUtlMap<unsigned int, kill_eater_score_type_t, unsigned short> kill_eater_score_types;
			CUtlMap<const char*, CUtlVector<CItemLevelingDefinition>*> item_levels;
			CPipRankData skirmish_rank_info;

		private:
			int32_t pad10[43];

		public: // This is where CCStrike15ItemSchema begins
			CUtlVector<const char*> m_ClassUsabilityStrings;
			CUtlVector<const char*> m_LoadoutStrings;
			CUtlVector<const char*> m_LoadoutStringsSubPositions;
			CUtlVector<const char*> m_LoadoutStringsForDisplay;

		private:
			int32_t pad[5];

		public:
			CUtlVector<const char*> m_LoadoutStringsOrder; // idk how to name this. 1st = primary, 2nd = secondary, 3rd = melee, 4th = grenade, 5th = item

		CUtlHashMapLarge<int, CPaintKit*>* GetPaintKits()
		{
			return (CUtlHashMapLarge<int, CPaintKit*>*)((uintptr_t)this + 0x288);
		}

		mass* GetItemDefinitions()
		{
			return *reinterpret_cast<mass**>(reinterpret_cast<uintptr_t>(this) + offsets_cc_strike15_item_schema->Items);
		}

		int FindItemDefinition(int *index)
		{
			return reinterpret_cast<int(__thiscall*)(void*, int*)>(offsets_cc_strike15_item_schema->CUtlHashMapLarge__Find)((this + 136), index);
		}

		int AddPaintKitDefinition(int stub, CPaintKit* new_paint_kit)
		{
			return offsets_cc_strike15_item_schema->AddPaintKitDefinition(this, stub, new_paint_kit);
		}

		CPaintKit* GetPaintKitDefinitionByName(const char* name)
		{
			return offsets_cc_strike15_item_schema->GetPaintKitDefinitionByName(this, name);
		}

		CPaintKit* GetPaintKitDefinition(unsigned int paintKitID)
		{
			return offsets_cc_strike15_item_schema->GetPaintKitDefinition(this, paintKitID);
		}

		CUtlMap<const char*, CEconItemSetDefinition>* GetItemSets()
		{
			return (CUtlMap<const char*, CEconItemSetDefinition>*)((uintptr_t)this + 0x144);
		}

		void* GetAlternativeIcons()
		{
			return (void*)((uintptr_t)this + offsets_cc_strike15_item_schema->alternative_icons2_offset);
		}
		CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>* GetPaintKitsRarity()
		{
			return (CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>*)((uintptr_t)this + offsets_cc_strike15_item_schema->paint_kits_rarity_offset);
			
		}

		CUtlMap<int, CStickerKit*, int, CDefLess<int> > * GetStickerKits()
		{
			return reinterpret_cast<CUtlMap<int, CStickerKit*, int, CDefLess<int> > *>(((uintptr_t)this + 0x2AC));
		}
		//<CUtlMap<int, CPaintKit *, int, CDefLess<int>>::Node_t, int, CUtlMap<int, CPaintKit *, int, CDefLess<int>>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CPaintKit *, int, CDefLess<int>>::Node_t, int>, int>>
		//char pad_0x0000[0x34]; //0x0000

		//CUtlRBTree<CUtlMap<int, CCStrike15ItemDefinition *, int>::Node_t, int, CUtlMap<int, CCStrike15ItemDefinition *, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CCStrike15ItemDefinition *, int>::Node_t, int>, int>> Item_definitions;

		//CUtlRBTree<CUtlMap<int, CPaintKit *, int>::Node_t, int, CUtlMap<int, CPaintKit *, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CPaintKit *, int>::Node_t, int>, int> > PaintKits; //0x0264 

		//char pad_0x0288[0x430]; //0x0288
	};
	enum ItemQuality : unsigned short
	{
		ITEM_QUALITY_DEFAULT,
		ITEM_QUALITY_GENUINE,
		ITEM_QUALITY_VINTAGE,
		ITEM_QUALITY_UNUSUAL,
		ITEM_QUALITY_SKIN,
		ITEM_QUALITY_COMMUNITY,
		ITEM_QUALITY_DEVELOPER,
		ITEM_QUALITY_SELFMADE,
		ITEM_QUALITY_CUSTOMIZED,
		ITEM_QUALITY_STRANGE,
		ITEM_QUALITY_COMPLETED,
		ITEM_QUALITY_UNK2,
		ITEM_QUALITY_TOURNAMENT
	};

	enum ItemRarity : unsigned short
	{
		ITEM_RARITY_DEFAULT = 0,
		ITEM_RARITY_COMMON = 1,
		ITEM_RARITY_UNCOMMON = 2,
		ITEM_RARITY_RARE = 3,
		ITEM_RARITY_MYTHICAL = 4,
		ITEM_RARITY_LEGENDARY = 5,
		ITEM_RARITY_ANCIENT = 6,
		ITEM_RARITY_IMMORTAL = 7,
		ITEM_RARITY_UNUSUAL = 99
	};
	enum EStickerAttributeType
	{
		k_EStickerAttribute_ID,
		k_EStickerAttribute_Wear,
		k_EStickerAttribute_Scale,
		k_EStickerAttribute_Rotation,
		k_EStickerAttribute_Count,
	};
	
	class IEconItemInterface
	{
	public:
		virtual ~IEconItemInterface() { }

		// Is an attribute present? We neither know nor care anything about the attribute
		// value stored.
		///*bool FindAttribute(const CEconItemAttributeDefinition* pAttrDef) const
		//{
		//	return ::FindAttribute(this, pAttrDef);
		//}*/

		// If an attribute is present, it will copy the value into out_pValue and return true.
		// If the attribute is not present, it will return false and not touch the value in
		// out_pValue. If a T is passed in that is not a type the attribute system understands,
		// this function will fail to compile.
		//template < typename T >
		//bool FindAttribute(const CEconItemAttributeDefinition* pAttrDef, T* out_pValue) const
		//{
		//	return ::FindAttribute(this, pAttrDef, out_pValue);
		//}

		virtual int GetCustomPaintKitIndex() const;
		virtual int GetCustomPaintKitSeed() const;
		virtual float GetCustomPaintKitWear(float flWearDefault = 0.0f) const;

		virtual float GetStickerAttributeBySlotIndexFloat(int nSlotIndex, EStickerAttributeType type, float flDefault) const;
		virtual unsigned int GetStickerAttributeBySlotIndexInt(int nSlotIndex, EStickerAttributeType type, unsigned int uiDefault) const;

		// IEconItemInterface common implementation.
		virtual bool IsTradable() const;
		virtual bool IsMarketable() const;
		virtual bool IsCommodity() const;
		virtual bool IsUsableInCrafting() const;
		virtual bool IsHiddenFromDropList() const;

		virtual RTime32 GetExpirationDate() const;

		// IEconItemInterface interface.
		virtual const void* GetItemDefinition() const = 0;

		virtual unsigned int			GetAccountID() const = 0;
		virtual unsigned __int64			GetItemID() const = 0;
		virtual int			GetQuality() const = 0;
		virtual unsigned int			GetRarity() const = 0;
		virtual unsigned char			GetFlags() const = 0;
		virtual int GetOrigin() const = 0;
		virtual unsigned short			GetQuantity() const = 0;
		virtual unsigned int			GetItemLevel() const = 0;
		virtual bool			GetInUse() const = 0;			// is this item in use somewhere in the backend? (ie., cross-game trading)

		virtual const char* GetCustomName() const = 0;		// get a user-generated name, if present, otherwise NULL; return value is UTF8
		virtual const char* GetCustomDesc() const = 0;		// get a user-generated flavor text, if present, otherwise NULL; return value is UTF8

		virtual int				GetItemSetIndex() const = 0;

		// IEconItemInterface attribute iteration interface. This is not meant to be used for
		// attribute lookup! This is meant for anything that requires iterating over the full
		// attribute list.
		virtual void IterateAttributes(/*IEconItemAttributeIterator**/ void* pIterator) const = 0;

		// We can check whether the item is potentially tradable in future if enough time
		// passes, and if yes can get the exact date when it becomes tradable
		bool IsPotentiallyTradable() const;

		RTime32 GetCacheRefreshDateTime() const;

		RTime32 GetTradableAfterDateTime() const;
		RTime32 GetUseAfterDateTime() const;
	};


	class CSharedObject
	{
		typedef CSharedObject* (__cdecl* SOCreationFunc_t)();
		struct SharedObjectInfo_t
		{
			int					m_nID;
			unsigned			m_unFlags;
			SOCreationFunc_t	m_pFactoryFunction;
			const char* m_pchClassName;
			const char* m_pchBuildCacheSubNodeName;
			const char* m_pchUpdateNodeName;
			const char* m_pchCreateNodeName;
		};
	};

	class CEconItem;
	
	class CSharedObjectTypeCache
	{	
	public:
		virtual ~CSharedObjectTypeCache() = 0;
		virtual void AddObject(CSharedObject* obj) = 0;
		virtual void AddObjectClean(CSharedObject* obj) = 0;
		virtual void RemoveObject(CSharedObject* obj) = 0;
		virtual void RemoveAllObjectsWithoutDeleting() = 0;
		virtual void EnsureCapacity(unsigned int nItems) = 0;
		virtual void Dump() = 0;
		std::vector<CEconItem*> GetEconItems();
		typedef CUtlVectorFixedGrowable<CSharedObject*, 1> CSharedObjectVec;

		CSharedObjectVec m_vecObjects;
		int m_nTypeID;
	};

	class CSharedObjectCache
	{
	public:
		CSharedObjectCache();
		virtual ~CSharedObjectCache();

		virtual SOID_t GetOwner() const = 0;

		virtual bool AddObject(CSharedObject* pSharedObject);
		virtual bool AddObjectClean(CSharedObject* pSharedObject);
		virtual CSharedObject* RemoveObject(const CSharedObject& soIndex);
		virtual bool RemoveAllObjectsWithoutDeleting();

		//called to find the type cache for the specified class ID. This will return NULL if one does not exist
		const CSharedObjectTypeCache* FindBaseTypeCache(int nClassID) const;
		//called to find the type cache for the specified class ID. This will return NULL if one does not exist
		CSharedObjectTypeCache* FindBaseTypeCache(int nClassID);
		//called to create the specified class ID. If one exists, this is the same as find, otherwise one will be constructed
		CSharedObjectTypeCache* CreateBaseTypeCache(int nClassID);


		CSharedObject* FindSharedObject(const CSharedObject& soIndex);
		const CSharedObject* FindSharedObject(const CSharedObject& soIndex) const;

		void SetVersion(unsigned __int64 ulVersion) { m_ulVersion = ulVersion; }
		unsigned __int64 GetVersion() const { return m_ulVersion; }
		virtual void MarkDirty() {}

		virtual void Dump() const;

	protected:
		virtual CSharedObjectTypeCache* AllocateTypeCache(int nClassID) const = 0;
		CSharedObjectTypeCache* GetTypeCacheByIndex(int nIndex) { return m_CacheObjects.IsValidIndex(nIndex) ? m_CacheObjects.Element(nIndex) : NULL; }
		int GetTypeCacheCount() const { return m_CacheObjects.Count(); }

		int FirstTypeCacheIndex() const { return m_CacheObjects.Count() > 0 ? 0 : m_CacheObjects.InvalidIndex(); }
		int NextTypeCacheIndex(int iCurrent) const { return (iCurrent + 1 < m_CacheObjects.Count()) ? iCurrent + 1 : m_CacheObjects.InvalidIndex(); }
		int InvalidTypeCacheIndex() const { return m_CacheObjects.InvalidIndex(); }

		unsigned __int64 m_ulVersion;
	private:
		CUtlVector< CSharedObjectTypeCache* > m_CacheObjects;
	};

	class CEconItem //: public CSharedObject, public IEconItemInterface
	{
		unsigned short* GetEconItemData();
		//void UpdateEquippedState(unsigned int state);
	public:

		using SetDynamicAttributeValue_t = int(__thiscall*)(CEconItem*, unsigned long, void*);
		uint32_t* GetInventory();
		uint32_t* GetAccountID() ;
		uint16_t* GetDefIndex();
		uint64_t* GetItemID();
		uint64_t* GetOriginalID();
		unsigned char* GetFlags();
		void SetQuality(ItemQuality quality);
		void SetRarity(ItemRarity rarity);
		void SetOrigin(int origin);
		void SetLevel(int level);
		void SetInUse(bool in_use);
		void SetCustomName(const char* name);
		void SetCustomDesc(const char* name);
		void SetPaintSeed(float seed);
		void SetPaintKit(float kit);
		void SetPaintWear(float wear);
		void SetStatTrak(int val);
		void AddSticker(int index, int kit, float wear, float scale, float rotation);
		static CCStrike15ItemSchema* getItemSchema();
		template<typename TYPE>
		void SetAttributeValue(int index, TYPE val)
		{
			auto item_schema = (DWORD*)getItemSchema();
			auto v16 = *(DWORD*)(item_schema[72] + 4 * index);

			getSetDynamicAttributeValue()(this, v16, &val);
		}
		struct attribute_t
		{
			attrib_definition_index_t m_unDefinitionIndex;			// stored as ints here for memory efficiency on the GC
			attribute_data_union_t m_value;

			void operator=(const attribute_t& rhs) = delete;
		};

		struct EquippedInstance_t
		{
			EquippedInstance_t() : m_unEquippedClass(0), m_unEquippedSlot(INVALID_EQUIPPED_SLOT) {}
			EquippedInstance_t(equipped_class_t unClass, equipped_slot_t unSlot) : m_unEquippedClass(unClass), m_unEquippedSlot(unSlot) {}
			equipped_class_t m_unEquippedClass;
			equipped_slot_t m_unEquippedSlot;
		};
		
		struct CustomDataOptimizedObject_t
		{
			// This is preceding the attributes block, attributes tightly packed and follow immediately
			// 25% of CS:GO items have no attributes and don't allocate anything here
			// one attribute case = [uint16] + [uint16+uint32] = 8 bytes (SBH bucket 8)
			// 2 attributes case = [uint16]+2x[uint16+uint32] = 14 bytes (SBH bucket 16)
			// 3 attributes case = [uint16]+3x[uint16+uint32] = 20 bytes (SBH bucket 24)  (60% of CS:GO items)
			// 4 attributes case = [uint16]+4x[uint16+uint32] = 26 bytes (SBH bucket 32)
			// 5 attributes case = [uint16]+4x[uint16+uint32] = 26 bytes (SBH bucket 32)

			uint16_t m_equipInstanceSlot1 : 6;			// Equip Instance Slot									#6 // if equipped
			uint16_t m_equipInstanceClass1 : 3;			// Equip Instance Class									#9
			uint16_t m_equipInstanceClass2Bit : 1;		// Whether the item is equipped for complementary class	#10
			uint16_t m_numAttributes : 6;					// Length of following attributes						#16

			// 32-bit GC size = 2 bytes!

			attribute_t* GetAttribute(uint32_t j) { return (reinterpret_cast<attribute_t*>(this + 1)) + j; }
			[[nodiscard]] const attribute_t* GetAttribute(uint32_t j) const { return (reinterpret_cast<const attribute_t*>(this + 1)) + j; }

			CustomDataOptimizedObject_t() = delete;
			CustomDataOptimizedObject_t(const CustomDataOptimizedObject_t&) = delete;
			~CustomDataOptimizedObject_t() = delete;
			CustomDataOptimizedObject_t& operator=(const CustomDataOptimizedObject_t&) = delete;
			void FreeObjectAndAttrMemory();
			static CustomDataOptimizedObject_t* Alloc(uint32_t numAttributes);			// allocates enough memory to include attributes
			static attribute_t* AddAttribute(CustomDataOptimizedObject_t*& rptr);	// reallocs the ptr
		};
		bool IsEquipped() const;
		bool IsEquippedForClass(equipped_class_t unClass) const;
		equipped_slot_t GetEquippedPositionForClass(equipped_class_t unClass) const;
		void UpdateEquippedState(EquippedInstance_t equipInstance);
		
		void* vtable1;
		void* vtable2;
		uint64_t m_ulID; //0x0008
		uint64_t m_ulOriginalID; //0x0010
		CustomDataOptimizedObject_t* m_pCustomDataOptimizedObject;
		uint32_t m_unAccountID; //0x001C
		uint32_t m_unInventory; //0x0020
		uint16_t m_unDefIndex; //0x0024
		union
		{
			uint16_t EconItemData; //0x0026
			struct
			{
				uint16_t m_unOrigin : 5;					// Origin (eEconItemOrigin) (0-31)					#5
				uint16_t m_nQuality : 4;					// Item quality (0-14)								#9
				uint16_t m_unLevel : 2;						// Item Level (0-1-2-3)								#11
				uint16_t m_nRarity : 4;						// Item rarity	(0-14)								#15
				uint16_t m_dirtybitInUse : 1;				// In Use
			};
		};
		//char pad_0028[8]; //0x0028
		__int16 m_iItemSet;				// packs it tightly with defindex field
		int  m_bSOUpdateFrame;
		uint8_t m_unFlags; //0x0030
		//char pad_0031[11]; //0x0031

		static SetDynamicAttributeValue_t getSetDynamicAttributeValue();
	};
	
	class C_EconItemView;

	class CPlayerInventory
	{
		/// Enumerate different events that might trigger a callback to an ISharedObjectListener
		enum ESOCacheEvent
		{

			/// Dummy sentinel value
			eSOCacheEvent_None = 0,

			/// We received a our first update from the GC and are subscribed
			eSOCacheEvent_Subscribed = 1,

			/// We lost connection to GC or GC notified us that we are no longer subscribed.
			/// Objects stay in the cache, but we no longer receive updates
			eSOCacheEvent_Unsubscribed = 2,

			/// We received a full update from the GC on a cache for which we were already subscribed.
			/// This can happen if connectivity is lost, and then restored before we realized it was lost.
			eSOCacheEvent_Resubscribed = 3,

			/// We received an incremental update from the GC about specific object(s) being
			/// added, updated, or removed from the cache
			eSOCacheEvent_Incremental = 4,

			/// A lister was added to the cache
			/// @see CGCClientSharedObjectCache::AddListener
			eSOCacheEvent_ListenerAdded = 5,

			/// A lister was removed from the cache
			/// @see CGCClientSharedObjectCache::RemoveListener
			eSOCacheEvent_ListenerRemoved = 6,
		};
	public:
		
		virtual void SOCreated(SOID_t owner, const CSharedObject* pObject, ESOCacheEvent eEvent) = 0;
		virtual void SOUpdated(SOID_t owner, const CSharedObject* pObject, ESOCacheEvent eEvent) = 0;
		virtual void SODestroyed(SOID_t owner, const CSharedObject* pObject, ESOCacheEvent eEvent) = 0;
		virtual void SOCacheSubscribed(SOID_t owner, ESOCacheEvent eEvent) = 0;
		virtual void* SOCacheUnsubscribed(SOID_t owner, ESOCacheEvent eEvent) = 0;
		virtual ~CPlayerInventory() = 0;
		virtual bool CanPurchaseItems(int) =  0;
		virtual int GetMaxItemCount() = 0;
		virtual C_EconItemView* GetItemInLoadout(int team, int slot) = 0; // 8
			// Made public for an inventory helper func
		//int					m_nTargetRecipe;
		SOID_t m_OwnerID;

		CUtlVector< C_EconItemView*> m_aDefaultEquippedDefinitionItems;

		typedef CUtlMap< itemid_t, C_EconItemView*, int, CDefLess< itemid_t > > ItemIdToItemMap_t;
		// Wrap different container types so they all stay in sync with inventory state
		class CItemContainers
		{
		public:
			void Add(C_EconItemView* pItem);
			void Remove(itemid_t ullItemID);
			void Purge();
			bool DbgValidate() const; // Walk all containers and confirm content matches

			const CUtlVector< C_EconItemView*>& GetItemVector() const { return m_vecInventoryItems; }
			const ItemIdToItemMap_t& GetItemMap() const { return m_mapItemIDToItemView; }

		private:
			// The items the player has in his inventory, received from steam.
			CUtlVector< C_EconItemView*> m_vecInventoryItems;

			// Copies of pointers in vector for faster lookups
			ItemIdToItemMap_t m_mapItemIDToItemView;
		};

		CItemContainers	m_Items;

		int			m_iPendingRequests;
		bool		m_bGotItemsFromSteamAtLeastOnce, m_bCurrentlySubscribedToSteam;

		void* m_pSOCache;

		CUtlVector<void*> m_vecListeners;

		int					m_nActiveQuestID;

		itemid_t		m_LoadoutItems[4][57];
		static CEconItem* CreateEconItem();

		C_EconItemView* GetInventoryItemByItemID(int64_t ID);

		void RemoveItem(uint64_t ID);

		void RemoveItem(CEconItem* item);

		void ClearInventory();

		CSharedObjectTypeCache* GetBaseTypeCache();

		uint32_t GetSteamID() const;

		CUtlVector<C_EconItemView*>* GetInventoryItems();

		bool AddEconItem(CEconItem* item, bool bUpdateAckFile, bool bWriteAckFile, bool bCheckForNewItems);
	};
	
	class CSInventoryManager
	{
	public:
		C_EconItemView* FindOrCreateReferenceEconItem(int64_t ID);
		CPlayerInventory* GetLocalPlayerInventory();
		bool EquipItemInLoadout(int team, int slot, unsigned long long itemID, bool bSwap = false);
	};

}

struct paint_kit_inject_request
{
	paint_kit_inject_request(const char* pk, SourceEngine::CRC32_t _kv, SourceEngine::CRC32_t _tex):paintkit_kv_file(pk), kv_crc32(_kv), texture_crc32(_tex){}
	paint_kit_inject_request(std::string pk, SourceEngine::CRC32_t _kv, SourceEngine::CRC32_t _tex):paintkit_kv_file(std::move(pk)), kv_crc32(_kv), texture_crc32(_tex) {}
	std::string paintkit_kv_file;
	SourceEngine::CRC32_t kv_crc32;
	SourceEngine::CRC32_t texture_crc32;
};

void inject_all_paint_kits(std::vector<paint_kit_inject_request>& requested);

void patch_vmtcache_file();

void InjectPaintKit(const char* paintkit_kv_file, SourceEngine::CRC32_t kv_crc32, SourceEngine::CRC32_t texture_crc32);
void InjectStickerkit(const char* stickerkit_kv_file, SourceEngine::CRC32_t texture_crc32);

class CustomSkinWeapon;
class CustomSkinKnife;
class CustomSkinWearable;

SourceEngine::CEconItem* InjectItem(SourceEngine::TeamID team, CustomSkinWeapon& weapon);
SourceEngine::CEconItem* InjectKnife(SourceEngine::TeamID team, CustomSkinKnife& weapon);
SourceEngine::CEconItem* InjectWearable(SourceEngine::TeamID team, CustomSkinWearable& weapon);
void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	const std::vector<CustomSkinWeapon>::iterator& config);
void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	CustomSkinKnife& config);
void UpdateItem(
	SourceEngine::TeamID team,
	SourceEngine::CEconItem* item,
	CustomSkinWearable& config);

void LoadInventory();

SourceEngine::TeamID GetAvailableClassID(int definition_index);
int GetSlotID(int definition_index);

constexpr auto size_check = sizeof(SourceEngine::CUtlHashMapLarge<int, SourceEngine::CCStrike15ItemDefinition*>);

struct ss
{
	const void* field_4;
	void* field_8;
	int field_C;
	int field_10;
	signed int field_14;
	int field_18;
	signed int field_1C;
	signed int field_20;
	int field_24;
	uint8_t gap_28[4];

};

constexpr auto size_check2 = sizeof(SourceEngine::CUtlHashMapLarge<int, SourceEngine::CEconItemRarityDefinition>);
constexpr auto size_check3 = sizeof(SourceEngine::CUtlHashMapLarge<int, SourceEngine::CCStrike15ItemDefinition*>);

inline uint64_t CombinedItemIdMakeFromDefIndexAndPaint(uint16_t iDefIndex, uint16_t iPaint, uint8_t ub1 = 0)
{
	if (iDefIndex == 0 && iPaint == 0)
		return 0;
	return 0xF000000000000000ull | (uint64_t(ub1) << 32) | (uint64_t(iPaint) << 16) | (uint64_t(iDefIndex));
}

// Returns whether the itemID in question is really an encoded base item
inline bool CombinedItemIdIsDefIndexAndPaint(uint64_t ullItemId)
{
	return ullItemId >= 0xF000000000000000ull;
}

// Extracts the definition index from an encoded itemID
inline uint16_t CombinedItemIdGetDefIndex(uint64_t ullItemId)
{
	return uint16_t(ullItemId);
}

// Extracts the paint index from an encoded itemID
inline uint16_t CombinedItemIdGetPaint(uint64_t ullItemId)
{
	return uint16_t(ullItemId >> 16);
}

// Extracts the extra byte from an encoded itemID
inline uint16_t CombinedItemIdGetUB1(uint64_t ullItemId)
{
	return uint8_t(ullItemId >> 32);
}


struct offsets_ItemSchema
{
	using GetItemSchemaFn1 = SourceEngine::CCStrike15ItemSchema * (__stdcall*)();
	GetItemSchemaFn1 GetItemSchema;

	using SetDynamicAttributeValue_t = int(__thiscall*)(SourceEngine::CEconItem*, DWORD, void*);
	SetDynamicAttributeValue_t SetDynamicAttributeValue;

	using AddEconItem_t = bool (__thiscall*)(void*, SourceEngine::CEconItem*, int, int, char);
	AddEconItem_t AddEconItem;

	using GetInventoryItemByItemID_t = SourceEngine::C_EconItemView * (__thiscall*)(void*, int64_t);
	GetInventoryItemByItemID_t GetInventoryItemByItemID;

	using CreateSharedObjectSubclass_EconItem_t = SourceEngine::CEconItem * (__stdcall*)();
	CreateSharedObjectSubclass_EconItem_t CreateSharedObjectSubclass_EconItem = nullptr;

	using SetCustomName_t = SourceEngine::CEconItem * (__thiscall*)(void*, const char*);
	SetCustomName_t SetCustomName;
	SetCustomName_t SetCustomDesc;

	using UpdateEquippedState_t = int(__thiscall*)(SourceEngine::CEconItem*, unsigned int);
	UpdateEquippedState_t UpdateEquippedState;

	using EquipItemInLoadout_t = bool(__thiscall*)(void*, int, int, unsigned long long, bool);
	EquipItemInLoadout_t EquipItemInLoadout = nullptr;

	using FindOrCreateReferenceEconItem_t = SourceEngine::C_EconItemView * (__thiscall*)(void*, int64_t);
	FindOrCreateReferenceEconItem_t FindOrCreateReferenceEconItem;

	using GCSDK_CGCClient_FindSOCache_t = uintptr_t(__thiscall*)(uintptr_t, uint64_t, bool);
	GCSDK_CGCClient_FindSOCache_t GCSDK_CGCClient_FindSOCache;

	using GCSDK_CSharedObjectCache_CreateBaseTypeCache_t = SourceEngine::CSharedObjectTypeCache * (__thiscall*)(uintptr_t, int);
	GCSDK_CSharedObjectCache_CreateBaseTypeCache_t GCSDK_CSharedObjectCache_CreateBaseTypeCache;
	
	DWORD find_so_cache;
	DWORD create_base_type_cache;
	DWORD gc_cs;

	uintptr_t so_cache_map;
	uintptr_t g_GCClientSystem;

	using RemoveItem_t = int(__thiscall*)(void*, int64_t);
	RemoveItem_t RemoveItem;
	offsets_ItemSchema();
};

extern offsets_ItemSchema* offsets_itemschema;