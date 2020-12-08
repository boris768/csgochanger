// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CMaterialReference.h"
#include "CSGOStructs.hpp"

SourceEngine::CMaterialReference::CMaterialReference() noexcept : m_pMaterial(nullptr)
{
}

SourceEngine::CMaterialReference::~CMaterialReference()
{
	Shutdown();
}

//SourceEngine::IMaterial* SourceEngine::CMaterialReference::GetMaterial() const
//{
//	m_pMaterial->Refresh();
//	return m_pMaterial;
//}

void SourceEngine::CMaterialReference::Init(IMaterial* pMaterial) noexcept
{
	if (m_pMaterial != pMaterial)
	{
		Shutdown();

		m_pMaterial = pMaterial;
		if (m_pMaterial)
		{
			m_pMaterial->IncrementReferenceCount();//родня нахуй
		}
	}
}

void SourceEngine::CMaterialReference::Shutdown() noexcept
{
	if (m_pMaterial && interfaces.MaterialSystem())
	{
		m_pMaterial->DecrementReferenceCount();
		m_pMaterial = nullptr;
	}
}
