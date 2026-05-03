#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)

void vmm_init();
void* vmm_create_address_space();
void vmm_destroy_address_space(void* pml4);
void vmm_switch_address_space(void* pml4);
void vmm_map(void* pml4, uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_unmap(void* pml4, uint64_t virt);

#endif
