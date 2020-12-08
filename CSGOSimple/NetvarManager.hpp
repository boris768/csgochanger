#pragma once
#include "SourceEngine/Recv.hpp"
#include "fnv_hash.h"
#include <unordered_map>

class netvar_manager
{
	struct stored_data
	{
		SourceEngine::RecvProp* prop_ptr;
		std::uint16_t class_relative_offset;
	};

public:
	// Return a const instance, others shouldnt modify this.
	static const netvar_manager& get();

	[[nodiscard]] std::uint16_t get_offset(fnv::hash hash) const;

	[[nodiscard]] SourceEngine::RecvProp* get_prop(fnv::hash hash) const;

	// Prevent instruction cache pollution caused by automatic
	// inlining of `get` and get_offset every netvar usage when
	// there are a lot of netvars
	__declspec(noinline) static std::uint16_t get_offset_by_hash(fnv::hash hash);
	
	template<fnv::hash Hash>
	static auto get_offset_by_hash_cached() -> std::uint16_t
	{
		static auto offset = std::uint16_t(0);
		if (!offset)
			offset = get_offset_by_hash(Hash);
		return offset;
	}

	explicit netvar_manager();
	~netvar_manager() = default;
private:
	void dump_recursive(const char* base_class, SourceEngine::RecvTable* table, std::uint16_t offset);

	//SourceEngine::CUtlMap<fnv::hash, stored_data> *m_props;
	std::unordered_map<fnv::hash, stored_data> m_props;
};