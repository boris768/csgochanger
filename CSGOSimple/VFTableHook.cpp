#include "VFTableHook.hpp"
#include "Utils.hpp"
#include "CustomWinAPI.h"
#include "XorStr.hpp"

detail::protect_guard::protect_guard(void* base, const size_t len, const std::uint32_t flags) noexcept: base_(base),length_(len)
{
	g_pWinApi->dwVirtualProtect(base, len, flags, reinterpret_cast<PDWORD>(&old_));
}

detail::protect_guard::~protect_guard()
{
	g_pWinApi->dwVirtualProtect(base_, length_, old_, reinterpret_cast<PDWORD>(&old_));
}

vf_hook::~vf_hook()
{
	unhook_all();
	if (was_allocated_)
		delete[] new_vftbl_;
}

bool vf_hook::setup(void* const base_of_dll, void* base) noexcept
{
	if (Utils::IsCodePtr(base_of_dll))
		return false;
	if (base != nullptr && !Utils::IsCodePtr(base))
		class_base_ = base;

	if (class_base_ == nullptr)
		return false;

	old_vftbl_ = *static_cast<std::uintptr_t**>(class_base_);
	vftbl_len_ = estimate_vftbl_length(old_vftbl_) * sizeof(std::uintptr_t);

	if (vftbl_len_ == 0)
		return false;


	//if (base_of_dll)// If provided a module name then we will find a place in that module				
	//{
	//	new_vftbl_ = search_free_data_page(base_of_dll, vftbl_len_ + sizeof(std::uintptr_t));
	//	if (!new_vftbl_)
	//	{
	//		g_pWinApi->dwMessageBoxW(nullptr, XorStr(L"Could not write to memory!"), XorStr(L"Error!"), MB_OK | MB_ICONWARNING);
	//		return false;
	//	}
	//	//std::memset(new_vftbl_, NULL, vftbl_len_ + sizeof(std::uintptr_t));

	//	auto guard1 = detail::protect_guard(&new_vftbl_[1], vftbl_len_ + sizeof(std::uintptr_t), PAGE_READWRITE);
	//	std::memcpy(&new_vftbl_[1], old_vftbl_, vftbl_len_);
	//	new_vftbl_[0] = old_vftbl_[-1];
	//	try
	//	{
	//		auto guard = detail::protect_guard( class_base_, sizeof(std::uintptr_t), PAGE_READWRITE );

	//		*static_cast<std::uintptr_t**>(class_base_) = &new_vftbl_[1];
	//		was_allocated_ = false;
	//		if (table_is_hooked(base_of_dll, base))
	//		{
	//			g_pWinApi->dwBeep(500, 500);
	//		}
	//	}
	//	catch (...)
	//	{
	//		delete[] new_vftbl_;
	//		return false;
	//	}
	//}
	//else // If not then we do regular vmthooking
	//{
		new_vftbl_ = new std::uintptr_t[vftbl_len_ + 1]();
		std::memcpy(&new_vftbl_[1], old_vftbl_, vftbl_len_);
		try
		{
			auto guard = detail::protect_guard{ class_base_, sizeof(std::uintptr_t), PAGE_READWRITE };
			new_vftbl_[0] = old_vftbl_[-1];
			*static_cast<std::uintptr_t**>(class_base_) = &new_vftbl_[1];
			was_allocated_ = true;
		}
		catch (...)
		{
			delete[] new_vftbl_;
			return false;
		}
	//}
	_driver_request request(
		static_cast<unsigned long long>(g_pWinApi->dwGetCurrentProcessId()), 
		reinterpret_cast<unsigned long long>(&new_vftbl_[1]), 
		reinterpret_cast<unsigned long long>(old_vftbl_));
	constexpr size_t s = sizeof(_driver_request);
	g_pWinApi->dwNtReadVirtualMemory(reinterpret_cast<HANDLE>(0xFACE94), nullptr, &request, s, nullptr);

	return true;
}

std::size_t vf_hook::estimate_vftbl_length(const std::uintptr_t* vftbl_start)
{
	MEMORY_BASIC_INFORMATION memInfo;
	auto size = -1;
	do
	{
		size++;
		g_pWinApi->dwVirtualQuery(reinterpret_cast<LPCVOID>(vftbl_start[size]), &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
	}
	while (memInfo.Protect == PAGE_EXECUTE_READ || memInfo.Protect == PAGE_EXECUTE_READWRITE);
	return size;
}

bool vf_hook::table_is_hooked(void* base_of_dll, void* table) noexcept
{
	// Because of this probable check you can make a VEH Hook by forcing the pointer to 0
	if (table == nullptr) // Not initialized or invalid table to check
		return false;

	// Get module end
	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(base_of_dll);
	const auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(base_of_dll) + dos_header->e_lfanew);

	const auto module_end = reinterpret_cast<std::uintptr_t>(base_of_dll) + nt_headers->OptionalHeader.SizeOfImage - sizeof(std::uintptr_t);

	const auto table_dst = *reinterpret_cast<std::uintptr_t*>(table);

	// Check the destination of the table, if the destination of the table is outside the region of the module, it is because it has been changed(hooked)
	return table_dst < reinterpret_cast<std::uintptr_t>(base_of_dll) || table_dst > module_end;
}

uintptr_t* vf_hook::search_free_data_page(void* base_of_dll, const size_t vmt_size)
{
	const auto check_data_section = [&](const void* const address, const std::size_t _vmt_size)
	{
		constexpr DWORD data_protection = (PAGE_EXECUTE_READWRITE | PAGE_READWRITE);
		MEMORY_BASIC_INFORMATION mbi;

		if (g_pWinApi->dwVirtualQuery(address, &mbi, sizeof(mbi)) == sizeof(mbi) && mbi.AllocationBase && mbi.BaseAddress &&
			mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS)
		{
			if ((mbi.Protect & data_protection) && mbi.RegionSize >= _vmt_size)
			{
				return static_cast<bool>(mbi.Protect & data_protection);
			}
		}
		return false;
	};

	if (base_of_dll == nullptr)
		return nullptr;

	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(base_of_dll);
	const auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(base_of_dll) +
		dos_header->e_lfanew);

	const auto module_end = reinterpret_cast<std::uintptr_t>(base_of_dll) + nt_headers->OptionalHeader.SizeOfImage -
		sizeof(std::uintptr_t);

	for (auto current_address = module_end; current_address > reinterpret_cast<DWORD>(base_of_dll); current_address -=
	     sizeof(std::uintptr_t))
	{
		if (*reinterpret_cast<std::uintptr_t*>(current_address) == 0 && check_data_section(
			reinterpret_cast<LPCVOID>(current_address), vmt_size))
		{
			bool is_good_vmt = true;
			auto page_count = 0u;

			for (; page_count < vmt_size && is_good_vmt; page_count += sizeof(std::uintptr_t))
			{
				if (*reinterpret_cast<std::uintptr_t*>(current_address + page_count) != 0)
					is_good_vmt = false;
			}

			if (is_good_vmt && page_count >= vmt_size)
				return reinterpret_cast<uintptr_t*>(current_address);
		}
	}
	return nullptr;
}

void vf_hook::unhook_index(const size_t index) const noexcept
{
	new_vftbl_[index] = old_vftbl_[index];
}

void vf_hook::unhook_all() noexcept
{

	if (old_vftbl_ != nullptr)
	{
		auto guard = detail::protect_guard{ class_base_, sizeof(std::uintptr_t), PAGE_READWRITE };
		*static_cast<std::uintptr_t**>(class_base_) = old_vftbl_;
		old_vftbl_ = nullptr;
	}

}
