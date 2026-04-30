#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_EXITED
} task_state_t;

typedef struct task {
    uint64_t id;
    uint64_t rsp; // Current stack pointer
    void* stack_bottom;
    task_state_t state;
    struct task* next;
} task_t;

void task_init();
task_t* task_create(void (*entry)());
void task_yield();
uint64_t task_schedule(uint64_t current_rsp);

#endif
