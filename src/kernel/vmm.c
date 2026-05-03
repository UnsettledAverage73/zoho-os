#include "vmm.h"
#include "pmm.h"
#include "lock.h"
#include <stdint.h>
#include <stddef.h>

#define PML4_INDEX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PD_INDEX(addr)   (((addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)   (((addr) >> 12) & 0x1FF)

static uint64_t* current_pml4;
static spinlock_t vmm_lock;

void vmm_init() {
    spin_init(&vmm_lock);
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    current_pml4 = (uint64_t*)cr3;
}

void* vmm_create_address_space() {
    uint64_t flags = spin_lock_irqsave(&vmm_lock);
    uint64_t* new_pml4 = (uint64_t*)pmm_alloc_frame();
    for (int i = 0; i < 512; i++) new_pml4[i] = 0;
    
    new_pml4[0] = current_pml4[0];
    spin_unlock_irqrestore(&vmm_lock, flags);
    
    return new_pml4;
}

void vmm_switch_address_space(void* pml4) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pml4));
}

void vmm_destroy_address_space(void* pml4_ptr) {
    uint64_t flags = spin_lock_irqsave(&vmm_lock);
    uint64_t* pml4 = (uint64_t*)pml4_ptr;
    for (int i = 1; i < 512; i++) {
        if (pml4[i] & PAGE_PRESENT) {
            uint64_t* pdpt = (uint64_t*)(pml4[i] & ~0xFFFULL);
            for (int j = 0; j < 512; j++) {
                if (pdpt[j] & PAGE_PRESENT) {
                    uint64_t* pd = (uint64_t*)(pdpt[j] & ~0xFFFULL);
                    for (int k = 0; k < 512; k++) {
                        if (pd[k] & PAGE_PRESENT) {
                            uint64_t* pt = (uint64_t*)(pd[k] & ~0xFFFULL);
                            for (int l = 0; l < 512; l++) {
                                if (pt[l] & PAGE_PRESENT) {
                                    pmm_free_frame((void*)(pt[l] & ~0xFFFULL));
                                }
                            }
                            pmm_free_frame(pt);
                        }
                    }
                    pmm_free_frame(pd);
                }
            }
            pmm_free_frame(pdpt);
        }
    }
    pmm_free_frame(pml4);
    spin_unlock_irqrestore(&vmm_lock, flags);
}

static uint64_t* get_next_table(uint64_t* table, uint64_t index, int alloc) {
    if (table[index] & PAGE_PRESENT) {
        return (uint64_t*)(table[index] & ~0xFFFULL);
    } else {
        if (!alloc) return NULL;
        uint64_t* next_table = (uint64_t*)pmm_alloc_frame();
        if (!next_table) return NULL;
        
        for (int i = 0; i < 512; i++) next_table[i] = 0;
        
        table[index] = (uint64_t)next_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
        return next_table;
    }
}

void vmm_map(void* pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t irq_flags = spin_lock_irqsave(&vmm_lock);
    uint64_t* pdpt = get_next_table((uint64_t*)pml4, PML4_INDEX(virt), 1);
    uint64_t* pd   = get_next_table(pdpt, PDPT_INDEX(virt), 1);
    uint64_t* pt   = get_next_table(pd, PD_INDEX(virt), 1);
    
    pt[PT_INDEX(virt)] = (phys & ~0xFFFULL) | flags | PAGE_PRESENT;
    spin_unlock_irqrestore(&vmm_lock, irq_flags);
}

void vmm_unmap(void* pml4, uint64_t virt) {
    uint64_t irq_flags = spin_lock_irqsave(&vmm_lock);
    uint64_t* pdpt = get_next_table((uint64_t*)pml4, PML4_INDEX(virt), 0);
    if (!pdpt) { spin_unlock_irqrestore(&vmm_lock, irq_flags); return; }
    uint64_t* pd   = get_next_table(pdpt, PDPT_INDEX(virt), 0);
    if (!pd) { spin_unlock_irqrestore(&vmm_lock, irq_flags); return; }
    uint64_t* pt   = get_next_table(pd, PD_INDEX(virt), 0);
    if (!pt) { spin_unlock_irqrestore(&vmm_lock, irq_flags); return; }
    
    pt[PT_INDEX(virt)] = 0;
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
    spin_unlock_irqrestore(&vmm_lock, irq_flags);
}
