#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_EXITED
} task_state_t;

struct task_context {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t rip;
};

typedef struct task {
    uint64_t id;
    void* stack_top;
    void* stack_bottom;
    struct task_context* context;
    task_state_t state;
    struct task* next;
} task_t;

void task_init();
task_t* task_create(void (*entry)());
void task_yield();

#endif
