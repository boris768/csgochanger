#pragma once
#include "SourceEngine/IClientEntity.hpp"

namespace SourceEngine
{
	class IBaseWorldModel : public IClientEntity
	{
	public:
		void SetModelIndex(int index);
	};
}