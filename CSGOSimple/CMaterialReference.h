#pragma once
#include "IMaterial.h"

namespace SourceEngine
{
	class CMaterialReference
	{
	public:
		CMaterialReference() noexcept;
		~CMaterialReference();
		[[nodiscard]] IMaterial* GetMaterial() const noexcept { return m_pMaterial; }
		void Init(IMaterial* pMaterial) noexcept;
		void Shutdown() noexcept;
		
	private:
		IMaterial* m_pMaterial;
	};
}
