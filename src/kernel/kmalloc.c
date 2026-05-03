#include "kmalloc.h"
#include "vmm.h"
#include "pmm.h"
#include "klog.h"
#include "lock.h"

#define HEAP_START 0x1000000 // 16MB
#define HEAP_SIZE  0x1000000 // 16MB

struct kmalloc_header {
    size_t size;
    int is_free;
    struct kmalloc_header* next;
};

static struct kmalloc_header* free_list_head = NULL;
static spinlock_t kmalloc_lock;

void kmalloc_init() {
    spin_init(&kmalloc_lock);
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    for (uint64_t addr = HEAP_START; addr < HEAP_START + HEAP_SIZE; addr += PAGE_SIZE) {
        void* frame = pmm_alloc_frame();
        vmm_map((void*)cr3, addr, (uint64_t)frame, PAGE_WRITABLE);
    }

    free_list_head = (struct kmalloc_header*)HEAP_START;
    free_list_head->size = HEAP_SIZE - sizeof(struct kmalloc_header);
    free_list_head->is_free = 1;
    free_list_head->next = NULL;

    klog(LOG_INFO, "KMALLOC", "Kernel heap initialized at 16MB (Size: 16MB)");
}

void* kmalloc(size_t size) {
    uint64_t flags = spin_lock_irqsave(&kmalloc_lock);
    // Align size to 8 bytes
    size = (size + 7) & ~7;

    struct kmalloc_header* curr = free_list_head;
    while (curr) {
        if (curr->is_free && curr->size >= size) {
            // Can we split this block?
            if (curr->size >= size + sizeof(struct kmalloc_header) + 8) {
                struct kmalloc_header* new_block = (struct kmalloc_header*)((uint8_t*)curr + sizeof(struct kmalloc_header) + size);
                new_block->size = curr->size - size - sizeof(struct kmalloc_header);
                new_block->is_free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->is_free = 0;
            spin_unlock_irqrestore(&kmalloc_lock, flags);
            return (void*)((uint8_t*)curr + sizeof(struct kmalloc_header));
        }
        curr = curr->next;
    }

    klog(LOG_ERROR, "KMALLOC", "Out of memory!");
    spin_unlock_irqrestore(&kmalloc_lock, flags);
    return NULL;
}

void kfree(void* ptr) {
    if (!ptr) return;
    uint64_t flags = spin_lock_irqsave(&kmalloc_lock);

    struct kmalloc_header* header = (struct kmalloc_header*)((uint8_t*)ptr - sizeof(struct kmalloc_header));
    header->is_free = 1;

    // Coalesce free blocks
    struct kmalloc_header* curr = free_list_head;
    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += curr->next->size + sizeof(struct kmalloc_header);
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
    spin_unlock_irqrestore(&kmalloc_lock, flags);
}
