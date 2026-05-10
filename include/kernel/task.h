#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>
#include "lock.h"

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_EXITED
} task_state_t;

typedef struct task {
    uint64_t id;
    uint64_t rsp; // Current stack pointer
    void* stack_bottom;
    void* user_stack_bottom;
    uint64_t kernel_rsp; // Kernel stack pointer for Ring 3 -> Ring 0
    void* pml4;
    task_state_t state;
    int cpu_id; 
    uint32_t timeslice;
    struct task* next;
} task_t;

typedef struct {
    spinlock_t lock;
    task_t* head;
    task_t* tail;
    uint32_t count;
} runqueue_t;

void task_init_global();
void task_init_per_cpu();
task_t* task_create(void (*entry)());
task_t* task_create_user(void (*entry)());
task_t* task_exec(void* elf_data);
task_t* task_exec_file(const char* path);
void task_yield();
void task_timer_tick();
int task_needs_schedule();
void task_request_reschedule();
uint64_t task_schedule(uint64_t current_rsp);

#endif
