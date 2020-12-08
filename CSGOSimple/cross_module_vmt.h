#pragma once
#include <cstdint>
#include <cassert>

/* Hooks an object in a remote module that csgo uses to call virtual functions from module A in module B
	EXAMPLE: FSN is called from engine.dll: but its vtable is in client.dll, engine.dll has a global that points to the
	VTable FSN resides in -> this way we can replace what this points to in order to hook all calls from engine into that specific VTable */
class cross_module_vmt
{
public:
	explicit cross_module_vmt(void* base_of_dll, void* ptr_to_class_object) noexcept;
	cross_module_vmt(cross_module_vmt&&) = delete;
	cross_module_vmt(const cross_module_vmt&) = delete;
	cross_module_vmt& operator=(const cross_module_vmt&) = delete;
	~cross_module_vmt() noexcept;

	template <typename T>
	void hook(T fun, const size_t index) noexcept
	{
		assert(index >= 0 && index <= m_size);
		//new_vftbl_[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
		*reinterpret_cast<uintptr_t*>(uintptr_t(m_jmp_table) + index * 11 + 7) = uintptr_t(fun) - uintptr_t(m_jmp_table) - (index + 1) * 11;
	}

	//void hook(void* fn, size_t index) const
	//{
	//	*reinterpret_cast<uintptr_t*>(uintptr_t(m_jmp_table) + index * 11 + 7) = uintptr_t(fn) - uintptr_t(m_jmp_table) - (index + 1) * 11;
	//}

	void unhook(size_t index) const noexcept
	{
		*reinterpret_cast<uintptr_t*>(uintptr_t(m_jmp_table) + index * 11 + 7) = m_original_vtable[index];
	}

	void unhook_table() const noexcept
	{
		*m_original_pointer = uintptr_t(m_class_obj);
	}

	template<typename T>
	T get_original(int index)
	{
		return reinterpret_cast<T>(m_original_vtable[index]);
	}

private:
	uintptr_t* m_original_pointer;
	uintptr_t** m_class_obj;
	uintptr_t* m_original_vtable;
	uintptr_t* m_new_vtable;
	void* m_jmp_table;
	size_t m_size;
};