#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "task.h"
#include "gdt.h"

typedef struct cpu {
    uint64_t user_stack;    // 0x00
    uint64_t kernel_stack;  // 0x08
    struct cpu* self;       // 0x10
    task_t* current_task;   // 0x18
    uint8_t id;             // 0x20
    uint8_t need_resched;   // 0x21
    uint8_t pad[6];

    struct {
        struct gdt_entry entries[5];
        struct gdt_tss_entry tss;
    } __attribute__((packed)) gdt;
    struct tss_entry tss_entry;
    struct gdt_ptr gdt_ptr;
    runqueue_t runqueue;
    task_t* idle_task;
} __attribute__((packed)) cpu_t;

cpu_t* get_cpu();
void cpu_early_init();
void cpu_init(uint8_t apic_id);
int cpu_get_count();
int cpu_get_count_unlocked();
cpu_t* cpu_get_by_index(int index);
cpu_t* cpu_get_by_index_unlocked(int index);
void cpu_lock_all();
void cpu_unlock_all();

#endif
