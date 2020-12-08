#pragma once
#include "KeyValues.h"
#include "IMaterialSystem.h"

struct WeaponPaintableMaterial_t
{
	char m_szName[128];
	char m_szOriginalMaterialName[128];
	char m_szFolderName[128];
	int m_nViewModelSize;						// texture size
	int m_nWorldModelSize;						// texture size
	float m_flWeaponLength;
	float m_flUVScale;
	bool m_bBaseTextureOverride;
	bool m_bMirrorPattern;
};

class IVisualsDataCompare
{
public:
	virtual void FillCompareBlob() = 0;
	virtual const SourceEngine::CUtlBuffer& GetCompareBlob() const = 0;
	virtual bool Compare(const SourceEngine::CUtlBuffer& pOther) = 0;
};

class IVisualsDataProcessor : public SourceEngine::__IRefCounted
{
public:
	IVisualsDataProcessor() {}

	virtual SourceEngine::KeyValues* GenerateCustomMaterialKeyValues() = 0;
	virtual SourceEngine::KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId) = 0;
	virtual IVisualsDataCompare* GetCompareObject() = 0;
	virtual bool HasCustomMaterial() const = 0;
	virtual const char* GetOriginalMaterialName() const = 0;
	virtual const char* GetOriginalMaterialBaseName() const = 0;
	virtual const char* GetPatternVTFName() const = 0;
	virtual void Refresh() = 0;

protected:
	virtual ~IVisualsDataProcessor() {}
};

template< class T >
class CBaseVisualsDataProcessor : public IVisualsDataProcessor
{
public:
	CBaseVisualsDataProcessor() {}

	virtual bool Compare(const SourceEngine::CUtlBuffer& otherBuf) { return GetCompareObject()->Compare(otherBuf); }
	IVisualsDataCompare* GetCompareObject() override { return &m_compareObject; }
	const char* GetPatternVTFName() const override { return nullptr; }

protected:
	T m_compareObject;
};

enum MaterialParamID_t
{
	MATERIAL_PARAM_ID_BASE_DIFFUSE_TEXTURE = 0,
	MATERIAL_PARAM_ID_PHONG_EXPONENT_TEXTURE,
	MATERIAL_PARAM_ID_BUMP_MAP,
	MATERIAL_PARAM_ID_ANISOTROPY_MAP,
	MATERIAL_PARAM_ID_MASKS1_MAP,

	MATERIAL_PARAM_ID_COUNT
};

enum VisualsDataPaintStyle_t
{
	VISUALS_DATA_PAINTSTYLE_ORIGINAL = 0,
	VISUALS_DATA_PAINTSTYLE_SOLID,			// solid color blocks
	VISUALS_DATA_PAINTSTYLE_HYDROGRAPHIC,	// hydrodipped
	VISUALS_DATA_PAINTSTYLE_SPRAY,			// spraypainted
	VISUALS_DATA_PAINTSTYLE_ANODIZED,		// anodized color blocks
	VISUALS_DATA_PAINTSTYLE_ANO_MULTI,		// anodized with a design
	VISUALS_DATA_PAINTSTYLE_ANO_AIR,		// anodized and spraypainted with dyes
	VISUALS_DATA_PAINTSTYLE_CUSTOM,			// custom paintjob
	VISUALS_DATA_PAINTSTYLE_ANTIQUED,		// antiqued
	VISUALS_DATA_PAINTSTYLE_GUNSMITH,		// half custom paintjob half antiqued

	VISUALS_DATA_PAINTSTYLE_COUNT
};

struct CCSSWeaponVisualsData_t
{
	char szOrigDiffuseVTFName[MAX_PATH];
	char szOrigExpVTFName[MAX_PATH];
	char szMaskVTFName[MAX_PATH];
	char szPosVTFName[MAX_PATH];
	char szAOVTFName[MAX_PATH];
	char szSurfaceVTFName[MAX_PATH];
	char szPatternVTFName[MAX_PATH];
	char szOrigVMTName[MAX_PATH];

	VisualsDataPaintStyle_t nStyle;
	bool bUsesPattern;

	SourceEngine::Vector col0;
	SourceEngine::Vector col1;
	SourceEngine::Vector col2;
	SourceEngine::Vector col3;

	int nPhongAlbedoBoost;
	int nPhongExponent;
	int nPhongIntensity;

	float flPhongAlbedoFactor;

	float flWearProgress;

	float flPatternScale;
	float flPatternOffsetX;
	float flPatternOffsetY;
	float flPatternRot;

	float flWearScale;
	float flWearOffsetX;
	float flWearOffsetY;
	float flWearRot;

	float flGrungeScale;
	float flGrungeOffsetX;
	float flGrungeOffsetY;
	float flGrungeRot;

	char szGrungeVTFName[MAX_PATH];
	char szWearVTFName[MAX_PATH];
};

class CBaseVisualsDataCompare : public IVisualsDataCompare
{
public:
	CBaseVisualsDataCompare() = default;

	CBaseVisualsDataCompare(CBaseVisualsDataCompare&& moveFrom) = default;

	CBaseVisualsDataCompare& operator=(CBaseVisualsDataCompare&& moveFrom) = default;
	
	virtual void FillCompareBlob()
	{
		SerializeToBuffer(m_compareBlob);
	}
	virtual const SourceEngine::CUtlBuffer& GetCompareBlob() const
	{
		return m_compareBlob;
	}

	virtual bool Compare(const SourceEngine::CUtlBuffer& otherBuf)
	{
		return (m_compareBlob.TellPut() == otherBuf.TellPut() && memcmp(otherBuf.Base(), m_compareBlob.Base(), m_compareBlob.TellPut()) == 0);
	}

	int m_nIndex;
	int m_nSeed;
	float m_flWear;
	int m_nLOD;
	int m_nModelID;  // for weapons this is CSWeaponID, for clothing this is ClothingDefinitionSlotId_t

protected:
	virtual void SerializeToBuffer(SourceEngine::CUtlBuffer& buf)
	{
		//buf.Clear();
		//Serialize(buf, m_nIndex);
		//Serialize(buf, m_nSeed);
		//Serialize(buf, m_flWear);
		//Serialize(buf, m_nLOD);
		//Serialize(buf, m_nModelID);
	}

private:
	SourceEngine::CUtlBuffer m_compareBlob;
};

class CCSWeaponVisualsDataCompare : public CBaseVisualsDataCompare
{
public:
	CCSWeaponVisualsDataCompare() = default;
	CCSWeaponVisualsDataCompare(CCSWeaponVisualsDataCompare&& moveFrom) = default;

	CCSWeaponVisualsDataCompare& operator= (CCSWeaponVisualsDataCompare&& moveFrom) = default;

	virtual void SerializeToBuffer(SourceEngine::CUtlBuffer& buf);

	float m_flWeaponLength;
	float m_flUVScale;
};


class CCSWeaponVisualsDataProcessor : public CBaseVisualsDataProcessor< CCSWeaponVisualsDataCompare >
{

public:
	CCSWeaponVisualsDataProcessor(CCSWeaponVisualsDataCompare&& compareObject, const WeaponPaintableMaterial_t* pWeaponPaintableMaterialData, const char* pCompositingShaderName = NULL)
		: m_pWeaponPaintableMaterialData(pWeaponPaintableMaterialData)
		, m_pCompositingShaderName(NULL)
		, m_flPhongAlbedoFactor(1.0f)
		, m_nPhongIntensity(0)
		, m_bIgnoreWeaponSizeScale(false)
	{
		m_compareObject = std::move(compareObject);
		m_compareObject.FillCompareBlob();
		CCSWeaponVisualsDataProcessor::SetVisualsData(pCompositingShaderName);
	}

	virtual void SetVisualsData(const char* pCompositingShaderName = NULL);
	virtual SourceEngine::KeyValues* GenerateCustomMaterialKeyValues();
	virtual SourceEngine::KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId);
	virtual bool HasCustomMaterial() const;
	virtual const char* GetOriginalMaterialName() const;
	virtual const char* GetOriginalMaterialBaseName() const;
	virtual const char* GetPatternVTFName() const { return m_visualsData.szPatternVTFName; }
	virtual void Refresh();
	virtual ~CCSWeaponVisualsDataProcessor();

	const WeaponPaintableMaterial_t* m_pWeaponPaintableMaterialData;

	char* m_pCompositingShaderName;
	CCSSWeaponVisualsData_t m_visualsData;
	float m_flPhongAlbedoFactor;
	int m_nPhongIntensity;
	bool m_bIgnoreWeaponSizeScale;
};


struct CCSClothingData_t
{
	bool bHasBump;
	bool bHasMasks1;

	char szOrigNormalVTFName[MAX_PATH];
	char szOrigMasks1VTFName[MAX_PATH];
	char m_szOrigVMTName[MAX_PATH];
	char m_szOrigVMTBaseName[MAX_PATH];
	char m_szSkinVMTName[MAX_PATH];

	float flWearProgress;

	float flPatternScaleX;
	float flPatternScaleY;
	float flPatternOffsetX;
	float flPatternOffsetY;
	float flPatternRot;

	float flGrungeScale;
	float flGrungeOffsetX;
	float flGrungeOffsetY;
	float flGrungeRot;

	bool bBaseAlphaPhongMask;
	bool bBaseAlphaEnvMask;
	bool bBumpAlphaEnvMask;

	float flFlipFixup;

	SourceEngine::KeyValues* pVMTOverrideValues;
};

class CCSClothingVisualsDataCompare : public CBaseVisualsDataCompare
{
public:
	// Need to declare default constructor here since we have declared a move constructor
	CCSClothingVisualsDataCompare() = default;

	// Unfortunately, VS2013 doesn't support default declaration of move functions.
	CCSClothingVisualsDataCompare(CCSClothingVisualsDataCompare&& moveFrom) = default;
	CCSClothingVisualsDataCompare& operator=(CCSClothingVisualsDataCompare&& moveFrom) = default;

	int m_nTeamId;
	bool m_bMirrorPattern;
	int m_nMaterialId;

	virtual void SerializeToBuffer(SourceEngine::CUtlBuffer& buf);
};


class CCSClothingVisualsDataProcessor : public CBaseVisualsDataProcessor< CCSClothingVisualsDataCompare >
{
public:
	CCSClothingVisualsDataProcessor(CCSClothingVisualsDataCompare&& compareObject, const WeaponPaintableMaterial_t* pWeaponPaintableMaterialDat, const char* szCompositingShaderName = NULL);

	virtual SourceEngine::KeyValues* GenerateCustomMaterialKeyValues();
	virtual SourceEngine::KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId);
	virtual bool HasCustomMaterial() const;
	virtual const char* GetOriginalMaterialName() const;
	virtual const char* GetOriginalMaterialBaseName() const;
	virtual const char* GetSkinMaterialName() const;
	virtual void Refresh();

	virtual void SetVisualsData(const char* pCompositingShaderName = NULL);

private:
	virtual ~CCSClothingVisualsDataProcessor();

	const WeaponPaintableMaterial_t* m_pWeaponPaintableMaterialData;

	CCSClothingData_t m_visualsData;

	char* m_szCompositingShaderName;
};

//	__const:0000000000F3FBB8                 dq offset __ZN31CCSClothingVisualsDataProcessorD0Ev; CCSClothingVisualsDataProcessor::~CCSClothingVisualsDataProcessor()
//	__const:0000000000F3FBC0                 dq offset __ZN20CRefCountServiceBaseILb1E6CRefMTE14OnFinalReleaseEv; CRefCountServiceBase<true, CRefMT>::OnFinalRelease(void)
//	__const:0000000000F3FBC8                 dq offset __ZN31CCSClothingVisualsDataProcessor31GenerateCustomMaterialKeyValuesEv; CCSClothingVisualsDataProcessor::GenerateCustomMaterialKeyValues(void)
//	__const:0000000000F3FBD0                 dq offset __ZN31CCSClothingVisualsDataProcessor34GenerateCompositeMaterialKeyValuesEi; CCSClothingVisualsDataProcessor::GenerateCompositeMaterialKeyValues(int)
//	__const:0000000000F3FBD8                 dq offset __ZN25CBaseVisualsDataProcessorI29CCSClothingVisualsDataCompareE16GetCompareObjectEv; CBaseVisualsDataProcessor<CCSClothingVisualsDataCompare>::GetCompareObject(void)
//	__const:0000000000F3FBE0                 dq offset __ZNK31CCSClothingVisualsDataProcessor17HasCustomMaterialEv; CCSClothingVisualsDataProcessor::HasCustomMaterial(void)
//	__const:0000000000F3FBE8                 dq offset __ZNK31CCSClothingVisualsDataProcessor23GetOriginalMaterialNameEv; CCSClothingVisualsDataProcessor::GetOriginalMaterialName(void)
//	__const:0000000000F3FBF0                 dq offset __ZNK31CCSClothingVisualsDataProcessor27GetOriginalMaterialBaseNameEv; CCSClothingVisualsDataProcessor::GetOriginalMaterialBaseName(void)
//	__const:0000000000F3FBF8                 dq offset __ZNK25CBaseVisualsDataProcessorI29CCSClothingVisualsDataCompareE17GetPatternVTFNameEv; CBaseVisualsDataProcessor<CCSClothingVisualsDataCompare>::GetPatternVTFName(void)
//	__const:0000000000F3FC00                 dq offset __ZN31CCSClothingVisualsDataProcessor7RefreshEv; CCSClothingVisualsDataProcessor::Refresh(void)
//	__const:0000000000F3FC08                 dq offset __ZN25CBaseVisualsDataProcessorI29CCSClothingVisualsDataCompareE7CompareERK10CUtlBuffer; CBaseVisualsDataProcessor<CCSClothingVisualsDataCompare>::Compare(CUtlBuffer const&)
//	__const:0000000000F3FC10                 dq offset __ZNK31CCSClothingVisualsDataProcessor19GetSkinMaterialNameEv; CCSClothingVisualsDataProcessor::GetSkinMaterialName(void)
//	__const:0000000000F3FC18                 dq offset __ZN31CCSClothingVisualsDataProcessor14SetVisualsDataEPKc; CCSClothingVisualsDataProcessor::SetVisualsData(char const*)
