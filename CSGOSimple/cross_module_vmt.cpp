#include "cross_module_vmt.h"
#include "CustomWinAPI.h"
#include "VFTableHook.hpp"

cross_module_vmt::cross_module_vmt(void* base_of_dll, void* ptr_to_class_object) noexcept :
	m_original_pointer(static_cast<uintptr_t*>(ptr_to_class_object)),
	m_class_obj(*static_cast<uintptr_t***>(ptr_to_class_object)),
	m_original_vtable(*m_class_obj),
	m_jmp_table(nullptr),
	m_size(0)
{
	m_size = vf_hook::estimate_vftbl_length(m_original_vtable);

	/* allocate new vtable */
	m_new_vtable = vf_hook::search_free_data_page(base_of_dll, m_size*11); //new uintptr_t[m_size];
	if (!m_new_vtable)
		return;

	auto guard1 = detail::protect_guard(&m_new_vtable[1], m_size + sizeof(std::uintptr_t), PAGE_READWRITE);

	m_jmp_table = g_pWinApi->dwVirtualAlloc(nullptr, m_size * 11, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//g_pWinApi->dwVirtualProtect(m_jmp_table, m_size * 11 + sizeof(std::uintptr_t), PAGE_EXECUTE_READWRITE, reinterpret_cast<PDWORD>(&old_));
	//auto guard2 = detail::protect_guard(m_jmp_table, m_size * 11  + sizeof(std::uintptr_t), PAGE_EXECUTE_READWRITE);

	for (size_t i = 0; i < m_size; i++)
	{
		/* write shellcode to set ecx to real thisptr and then jump to correct function:
		MOV ECX, CLASS_POINTER -> 8B 0D [r/m32]
		JMP ORIGINAL_FUNCTION -> E9 [rel32]
		*/
		const uintptr_t offset = i * 11;
		const auto jmp_table = uintptr_t(m_jmp_table);
		*reinterpret_cast<uint16_t*>(jmp_table + offset) = 0x0D8B; // set 8B 0D (mov ecx)
		*reinterpret_cast<uintptr_t*>(jmp_table + offset + 2) = uintptr_t(&m_class_obj);
		// write the normal class object (for this ptr) -> mov ecx, m_class_object
		*reinterpret_cast<BYTE*>(jmp_table + offset + 6) = 0xE9; // write JMP 
		*reinterpret_cast<uintptr_t*>(jmp_table + offset + 7) = m_original_vtable[i] - jmp_table - offset - 11;
		// write original function to jump to (rel32)

		/* make our allocated vtable jump to the correct shellcode */
		m_new_vtable[i] = uintptr_t(m_jmp_table) + offset;
	}

	/* finally replace the remote pointer to point at our new vtable to hook it */
	*reinterpret_cast<uintptr_t***>(m_original_pointer) = &m_new_vtable;
	_driver_request request(
		static_cast<unsigned long long>(g_pWinApi->dwGetCurrentProcessId()),
		reinterpret_cast<unsigned long long>(m_original_vtable),
		reinterpret_cast<unsigned long long>(&m_new_vtable));
	constexpr size_t s = sizeof(_driver_request);
	g_pWinApi->dwNtReadVirtualMemory(reinterpret_cast<HANDLE>(0xFACE94), nullptr, &request, s, nullptr);
}

cross_module_vmt::~cross_module_vmt() noexcept
{
	*m_original_pointer = uintptr_t(m_class_obj);

	g_pWinApi->dwVirtualFree(m_jmp_table, 0, MEM_RELEASE);
	//delete m_new_vtable;
}
