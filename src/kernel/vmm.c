#include "vmm.h"
#include "pmm.h"
#include <stdint.h>
#include <stddef.h>

#define PML4_INDEX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PD_INDEX(addr)   (((addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)   (((addr) >> 12) & 0x1FF)

static uint64_t* current_pml4;

void vmm_init() {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    current_pml4 = (uint64_t*)cr3;
}

static uint64_t* get_next_table(uint64_t* table, uint64_t index, int alloc) {
    if (table[index] & PAGE_PRESENT) {
        return (uint64_t*)(table[index] & ~0xFFFULL);
    } else {
        if (!alloc) return NULL;
        uint64_t* next_table = (uint64_t*)pmm_alloc_frame();
        if (!next_table) return NULL;
        
        // Zero the new table
        for (int i = 0; i < 512; i++) next_table[i] = 0;
        
        table[index] = (uint64_t)next_table | PAGE_PRESENT | PAGE_WRITABLE;
        return next_table;
    }
}

void vmm_map(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t* pdpt = get_next_table(current_pml4, PML4_INDEX(virt), 1);
    uint64_t* pd   = get_next_table(pdpt, PDPT_INDEX(virt), 1);
    uint64_t* pt   = get_next_table(pd, PD_INDEX(virt), 1);
    
    pt[PT_INDEX(virt)] = (phys & ~0xFFFULL) | flags | PAGE_PRESENT;
    
    // Invalidate TLB
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

void vmm_unmap(uint64_t virt) {
    uint64_t* pdpt = get_next_table(current_pml4, PML4_INDEX(virt), 0);
    if (!pdpt) return;
    uint64_t* pd   = get_next_table(pdpt, PDPT_INDEX(virt), 0);
    if (!pd) return;
    uint64_t* pt   = get_next_table(pd, PD_INDEX(virt), 0);
    if (!pt) return;
    
    pt[PT_INDEX(virt)] = 0;
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}
