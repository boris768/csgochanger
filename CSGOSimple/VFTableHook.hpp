#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>

namespace detail
{
	class protect_guard
	{
	public:
		protect_guard(void* base, size_t len, std::uint32_t flags) noexcept;
		~protect_guard();
		protect_guard(protect_guard&&) = delete;
		protect_guard(const protect_guard&) = delete;
		protect_guard& operator=(const protect_guard&) = delete;
	private:
		void*         base_;
		size_t        length_;
		std::uint32_t old_{};
	};
}

class vf_hook
{
public:
	constexpr explicit  vf_hook() noexcept
		: class_base_(nullptr), vftbl_len_(0), new_vftbl_(nullptr), old_vftbl_(nullptr)
	{
	}
	//we dont use this
	//constexpr vf_hook(void* base) noexcept
	//	:class_base_(base), vftbl_len_(0), new_vftbl_(nullptr), old_vftbl_(nullptr)
	//{
	//}

	vf_hook(const vf_hook&) = delete;
	vf_hook(vf_hook&&) = delete;
	vf_hook& operator=(const vf_hook&) = delete;
	
	~vf_hook();

	bool setup(LPVOID base_of_dll, void * base) noexcept;
	template <typename T>
	void hook_index(T fun, const int index) noexcept
	{
		assert(index >= 0 && index <= static_cast<int>(vftbl_len_));
		new_vftbl_[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
	}

	void unhook_index(size_t index) const noexcept;

	void unhook_all() noexcept;
	template <typename T>
	T get_original(int index) noexcept
	{
		return T(old_vftbl_[index]);
	}

	static uintptr_t* search_free_data_page(void* base_of_dll, const size_t vmt_size);
	static size_t estimate_vftbl_length(const std::uintptr_t* vftbl_start);

private:
	static bool table_is_hooked(void* base_of_dll, void* table) noexcept;

	//Modified code from exphck https://www.unknowncheats.me/forum/2128832-post43.html

	void* class_base_;
	size_t vftbl_len_;
	uintptr_t* new_vftbl_;
	uintptr_t* old_vftbl_;
	LPCVOID search_base_ = nullptr;
	bool was_allocated_ = false;
};

