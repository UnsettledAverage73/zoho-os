#include "task.h"
#include "kmalloc.h"
#include "klog.h"
#include "vmm.h"
#include <stddef.h>

extern void switch_context(void** old_rsp, void* new_rsp);

static task_t* current_task = NULL;
static task_t* task_list = NULL;
static uint64_t next_id = 1;

void task_init() {
    // Create the "main" kernel task representing the current execution
    current_task = kmalloc(sizeof(task_t));
    current_task->id = 0;
    current_task->state = TASK_RUNNING;
    current_task->next = NULL;
    task_list = current_task;
    
    klog(LOG_INFO, "TASK", "Task system initialized");
}

task_t* task_create(void (*entry)()) {
    task_t* new_task = kmalloc(sizeof(task_t));
    new_task->id = next_id++;
    
    // Allocate 16KB stack
    size_t stack_size = 4096 * 4;
    new_task->stack_bottom = kmalloc(stack_size);
    new_task->stack_top = (uint8_t*)new_task->stack_bottom + stack_size;
    
    // Setup initial context on the stack
    uint64_t* stack = (uint64_t*)new_task->stack_top;
    
    // Push return address (entry point)
    *(--stack) = (uint64_t)entry;
    
    // Push dummy values for callee-saved registers (rbp, rbx, r12-r15)
    *(--stack) = 0; // rbp
    *(--stack) = 0; // rbx
    *(--stack) = 0; // r12
    *(--stack) = 0; // r13
    *(--stack) = 0; // r14
    *(--stack) = 0; // r15
    
    new_task->context = (struct task_context*)stack;
    new_task->state = TASK_READY;
    
    // Add to task list
    task_t* curr = task_list;
    while (curr->next) curr = curr->next;
    curr->next = new_task;
    new_task->next = NULL;

    klog(LOG_INFO, "TASK", "Task created");
    return new_task;
}

void task_yield() {
    task_t* old_task = current_task;
    task_t* next_task = current_task->next;
    
    if (!next_task) next_task = task_list; // Round robin
    
    // Skip exited tasks (simplified)
    while (next_task->state == TASK_EXITED && next_task != old_task) {
        next_task = next_task->next;
        if (!next_task) next_task = task_list;
    }

    if (next_task == old_task) return;

    current_task = next_task;
    current_task->state = TASK_RUNNING;
    if (old_task->state == TASK_RUNNING) old_task->state = TASK_READY;

    switch_context((void**)&old_task->context, next_task->context);
}
