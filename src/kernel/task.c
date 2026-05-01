#include "task.h"
#include "kmalloc.h"
#include "klog.h"
#include "vmm.h"
#include "lock.h"
#include <stddef.h>

static task_t* current_task = NULL;
static task_t* task_list = NULL;
static uint64_t next_id = 1;
static spinlock_t task_lock;

void task_init() {
    spin_init(&task_lock);
    current_task = kmalloc(sizeof(task_t));
    current_task->id = 0;
    current_task->state = TASK_RUNNING;
    current_task->next = NULL;
    current_task->rsp = 0; // Will be set on first preemption
    task_list = current_task;
    
    klog(LOG_INFO, "TASK", "Task system initialized");
}

task_t* task_create(void (*entry)()) {
    task_t* new_task = kmalloc(sizeof(task_t));
    new_task->id = next_id++;
    
    size_t stack_size = 4096 * 4;
    new_task->stack_bottom = kmalloc(stack_size);
    uint64_t* stack = (uint64_t*)((uint8_t*)new_task->stack_bottom + stack_size);
    
    // Initial interrupt frame for iretq
    *(--stack) = 0x10;            // SS
    *(--stack) = (uint64_t)stack + 8; // RSP (not really used by iretq for same-privilege return, but good for completeness)
    *(--stack) = 0x202;           // RFLAGS (Interrupts enabled)
    *(--stack) = 0x08;            // CS
    *(--stack) = (uint64_t)entry; // RIP
    
    // Dummy error code and int number
    *(--stack) = 0; 
    *(--stack) = 0;

    // Dummy registers for pop all
    for (int i = 0; i < 15; i++) *(--stack) = 0;

    new_task->rsp = (uint64_t)stack;
    new_task->state = TASK_READY;
    
    // Add to task list
    task_t* curr = task_list;
    while (curr->next) curr = curr->next;
    curr->next = new_task;
    new_task->next = NULL;

    return new_task;
}

uint64_t task_schedule(uint64_t current_rsp) {
    if (!current_task) return current_rsp;

    // Save current task's state
    current_task->rsp = current_rsp;
    if (current_task->state == TASK_RUNNING) current_task->state = TASK_READY;

    // Pick next task
    current_task = current_task->next;
    if (!current_task) current_task = task_list;

    while (current_task->state == TASK_EXITED) {
        current_task = current_task->next;
        if (!current_task) current_task = task_list;
    }

    current_task->state = TASK_RUNNING;
    return current_task->rsp;
}

void task_yield() {
    __asm__ volatile ("int $32"); // Trigger PIT interrupt manually
}
