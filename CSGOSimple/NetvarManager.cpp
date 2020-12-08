// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "NetvarManager.hpp"

#include <fstream>
#include "SourceEngine/SDK.hpp"


bool __cdecl less_function(const fnv::hash& a, const fnv::hash& b)
{
	if (a > b)
		return true;
	return false;
}

netvar_manager *instance;
const netvar_manager& netvar_manager::get()
{
	//mmap @no static initializers@ bypass
	if(!instance)
	{
		instance = new netvar_manager();
	}
	return reinterpret_cast<const netvar_manager &>(*instance);
}

std::uint16_t netvar_manager::get_offset(const fnv::hash hash) const
{
	return m_props.at(hash).class_relative_offset;
	//const auto it = m_props->Find(hash);
	//if(m_props->IsValidIndex(it))
	//{
	//	return m_props->Element(it).class_relative_offset;
	//}
	//assert(0); //hash for offset wasnt found
	//return 0;
}

SourceEngine::RecvProp* netvar_manager::get_prop(const fnv::hash hash) const
{
	return m_props.at(hash).prop_ptr;
	//const auto it = m_props->Find(hash);
	//if (m_props->IsValidIndex(it))
	//{
	//	return m_props->Element(it).prop_ptr;
	//}
	//assert(0); //hash for prop wasnt found
	//return nullptr;
}

std::uint16_t netvar_manager::get_offset_by_hash(const fnv::hash hash)
{
	return get().get_offset(hash);
}

#include <cctype>

#define DUMP_NETVARS

#ifdef DUMP_NETVARS
#define IF_DUMPING(...) __VA_ARGS__
#else
#define IF_DUMPING(...)
#endif

IF_DUMPING(static FILE* s_fp = nullptr;)

netvar_manager::netvar_manager()
{
	//m_props = new SourceEngine::CUtlMap<fnv::hash, stored_data>(5000, 5000, less_function);
	//m_props->SetLessFunc(less_function);
	auto pClient = SourceEngine::interfaces.Client();
	IF_DUMPING(fopen_s(&s_fp, "netvar_dump.txt", "w");)
		for (auto clazz = pClient->GetAllClasses(); clazz; clazz = clazz->m_pNext)
			if (clazz->m_pRecvTable)
				dump_recursive(clazz->m_pNetworkName, clazz->m_pRecvTable, 0);
	IF_DUMPING(if(s_fp) fclose(s_fp);)
}

auto netvar_manager::dump_recursive(const char* base_class, SourceEngine::RecvTable* table, const std::uint16_t offset) -> void
{
	for (auto i = 0; i < table->m_nProps; ++i)
	{
		const auto prop_ptr = &table->m_pProps[i];

		//Skip trash array items
		if (!prop_ptr || isdigit(prop_ptr->m_pVarName[0]))
			continue;

		//We dont care about the base class, we already know that
		if (fnv::hash_runtime(prop_ptr->m_pVarName) == FNV("baseclass"))
			continue;

		if (prop_ptr->m_RecvType == SourceEngine::SendPropType::DPT_DataTable &&
			prop_ptr->m_pDataTable != nullptr &&
			prop_ptr->m_pDataTable->m_pNetTableName[0] == 'D') // Skip shitty tables
		{
			dump_recursive(base_class, prop_ptr->m_pDataTable, std::uint16_t(offset + prop_ptr->m_Offset));
		}

		char hash_name[256];

		strcpy_s(hash_name, base_class);
		strcat_s(hash_name, "->");
		strcat_s(hash_name, prop_ptr->m_pVarName);

		const auto hash = fnv::hash_runtime(hash_name);
		const auto total_offset = std::uint16_t(offset + prop_ptr->m_Offset);

		IF_DUMPING(fprintf(s_fp, "%s\t0x%04X\t%s\n", base_class, total_offset, prop_ptr->m_pVarName);)

		/*stored_data save{
			prop_ptr,
			total_offset
		}*/;
		//m_props->Insert(hash, save);
		//m_props.insert(hash, save);
		
			m_props[hash] =
		{
			prop_ptr,
			total_offset
		};
	}
}
