#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)
#define PAGE_COW (1ULL << 9) // Available bit in page table entry
#define PAGE_HUGE (1ULL << 7) // Page Size bit in PDE

void vmm_init();
void* vmm_create_address_space();
void* vmm_clone_address_space(void* src_pml4);
void vmm_destroy_address_space(void* pml4);
void vmm_switch_address_space(void* pml4);
void vmm_map(void* pml4, uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_map_range(void* pml4, uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags);
void vmm_unmap(void* pml4, uint64_t virt);
void vmm_unmap_range(void* pml4, uint64_t virt, uint64_t size);
uint64_t vmm_get_phys(void* pml4, uint64_t virt);
uint64_t vmm_get_flags(void* pml4, uint64_t virt);

#endif
