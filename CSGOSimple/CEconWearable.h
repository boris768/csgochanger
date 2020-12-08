#pragma once
#include "SourceEngine/IClientEntity.hpp"
#include "C_EconItemView.h"
#include "Options.hpp"

class C_CEconWearable : public SourceEngine::IClientEntity
{
	template<class T>
	T* GetFieldPointer(ptrdiff_t offset)
	{
		const ptrdiff_t address = reinterpret_cast<ptrdiff_t>(this) + offset;
		T* returning_value = (T*)address;
		return returning_value;
	}
public:
	bool* GetInitialized();
	int* GetFallbackStatTrack();
	int* GetFallbackPaintKit();
	int* GetFallbackSeed();
	float* GetFallbackWear();
	int* GetEntityQuality();
	int* GetItemIDHigh();
	int* GetAccountID();
	short* GetItemDefinitionIndex();
	void SetModelIndex(int index);
	SourceEngine::C_EconItemView* GetEconItemView();
	int Equip(IClientEntity* player);
	int UnEquip(IClientEntity* player);
	int InitializeAttributes();
};
