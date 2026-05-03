#include "task.h"
#include "kmalloc.h"
#include "klog.h"
#include "vga.h"
#include "serial.h"
#include "vmm.h"
#include "gdt.h"
#include "syscall.h"
#include "elf.h"
#include "lock.h"
#include "cpu.h"
#include "window.h"
#include "vfs.h"
#include <stddef.h>

static uint64_t next_id = 1;
static spinlock_t id_lock;

void task_init_global() {
    next_id = 1;
    spin_init(&id_lock);
}

static void enqueue_task(runqueue_t* rq, task_t* task) {
    uint64_t flags = spin_lock_irqsave(&rq->lock);
    task->next = NULL;
    if (rq->tail) {
        rq->tail->next = task;
        rq->tail = task;
    } else {
        rq->head = rq->tail = task;
    }
    rq->count++;
    spin_unlock_irqrestore(&rq->lock, flags);
}

static task_t* dequeue_task(runqueue_t* rq) {
    uint64_t flags = spin_lock_irqsave(&rq->lock);
    if (!rq->head) {
        spin_unlock_irqrestore(&rq->lock, flags);
        return NULL;
    }
    task_t* task = rq->head;
    rq->head = task->next;
    if (!rq->head) rq->tail = NULL;
    rq->count--;
    spin_unlock_irqrestore(&rq->lock, flags);
    return task;
}

static cpu_t* pick_least_loaded_cpu() {
    cpu_lock_all();
    int count = cpu_get_count_unlocked();
    cpu_t* least = cpu_get_by_index_unlocked(0);
    
    // load = tasks in runqueue + (1 if current task is not idle)
    int min_load = least->runqueue.count;
    if (least->current_task && least->current_task->id != 0) min_load++;
    
    for (int i = 1; i < count; i++) {
        cpu_t* cpu = cpu_get_by_index_unlocked(i);
        int load = cpu->runqueue.count;
        if (cpu->current_task && cpu->current_task->id != 0) load++;
        
        if (load < min_load) {
            min_load = load;
            least = cpu;
        }
    }
    cpu_unlock_all();
    return least;
}

void task_init_per_cpu() {
    cpu_t* cpu = get_cpu();
    spin_init(&cpu->runqueue.lock);
    cpu->runqueue.head = cpu->runqueue.tail = NULL;
    cpu->runqueue.count = 0;
    
    task_t* idle_task = kmalloc(sizeof(task_t));
    idle_task->id = 0;
    idle_task->state = TASK_RUNNING;
    idle_task->cpu_id = cpu->id;
    idle_task->next = NULL;
    idle_task->rsp = 0; 
    idle_task->kernel_rsp = 0;
    idle_task->stack_bottom = NULL;
    idle_task->user_stack_bottom = NULL;
    
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    idle_task->pml4 = (void*)cr3;

    cpu->idle_task = idle_task;
    cpu->current_task = idle_task;
    
    klog(LOG_INFO, "TASK", "Task system initialized for CPU");
}

task_t* task_create(void (*entry)()) {
    task_t* new_task = kmalloc(sizeof(task_t));
    
    uint64_t id_flags = spin_lock_irqsave(&id_lock);
    new_task->id = next_id++;
    spin_unlock_irqrestore(&id_lock, id_flags);
    
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    new_task->pml4 = (void*)cr3;

    size_t stack_size = 4096 * 4;
    new_task->stack_bottom = kmalloc(stack_size);
    new_task->user_stack_bottom = NULL;
    uint64_t* stack = (uint64_t*)((uint8_t*)new_task->stack_bottom + stack_size);
    new_task->kernel_rsp = (uint64_t)stack;
    
    *(--stack) = 0x10;            // SS
    *(--stack) = (uint64_t)stack + 8; // RSP
    *(--stack) = 0x202;           // RFLAGS
    *(--stack) = 0x08;            // CS
    *(--stack) = (uint64_t)entry; // RIP
    
    *(--stack) = 0; 
    *(--stack) = 0;

    for (int i = 0; i < 15; i++) *(--stack) = 0;

    new_task->rsp = (uint64_t)stack;
    new_task->state = TASK_READY;
    
    cpu_t* target = pick_least_loaded_cpu();
    new_task->cpu_id = target->id;

    enqueue_task(&target->runqueue, new_task);

    return new_task;
}

task_t* task_create_user(void (*entry)()) {
    task_t* new_task = kmalloc(sizeof(task_t));
    
    uint64_t id_flags = spin_lock_irqsave(&id_lock);
    new_task->id = next_id++;
    spin_unlock_irqrestore(&id_lock, id_flags);

    new_task->pml4 = vmm_create_address_space();

    size_t ustack_size = 4096 * 4;
    new_task->user_stack_bottom = kmalloc(ustack_size);
    uint64_t* ustack = (uint64_t*)((uint8_t*)new_task->user_stack_bottom + ustack_size);

    size_t kstack_size = 4096 * 4;
    new_task->stack_bottom = kmalloc(kstack_size);
    uint64_t* kstack = (uint64_t*)((uint8_t*)new_task->stack_bottom + kstack_size);
    new_task->kernel_rsp = (uint64_t)kstack;
    
    uint64_t* stack = kstack;

    *(--stack) = 0x1B;            // SS (User Data)
    *(--stack) = (uint64_t)ustack; // RSP
    *(--stack) = 0x202;           // RFLAGS
    *(--stack) = 0x23;            // CS (User Code)
    *(--stack) = (uint64_t)entry; // RIP
    
    *(--stack) = 0; 
    *(--stack) = 0;

    for (int i = 0; i < 15; i++) *(--stack) = 0;

    new_task->rsp = (uint64_t)stack;
    new_task->state = TASK_READY;
    
    cpu_t* target = pick_least_loaded_cpu();
    new_task->cpu_id = target->id;

    enqueue_task(&target->runqueue, new_task);

    return new_task;
}

task_t* task_exec(void* elf_data) {
    void* entry;
    void* pml4;
    if (!elf_load(elf_data, &entry, &pml4)) {
        return NULL;
    }

    task_t* new_task = kmalloc(sizeof(task_t));
    
    uint64_t id_flags = spin_lock_irqsave(&id_lock);
    new_task->id = next_id++;
    spin_unlock_irqrestore(&id_lock, id_flags);

    new_task->pml4 = pml4;

    size_t ustack_size = 4096 * 4;
    new_task->user_stack_bottom = kmalloc(ustack_size);
    uint64_t* ustack = (uint64_t*)((uint8_t*)new_task->user_stack_bottom + ustack_size);

    size_t kstack_size = 4096 * 4;
    new_task->stack_bottom = kmalloc(kstack_size);
    uint64_t* kstack = (uint64_t*)((uint8_t*)new_task->stack_bottom + kstack_size);
    new_task->kernel_rsp = (uint64_t)kstack;
    
    uint64_t* stack = kstack;

    *(--stack) = 0x1B;            // SS (User Data)
    *(--stack) = (uint64_t)ustack; // RSP
    *(--stack) = 0x202;           // RFLAGS
    *(--stack) = 0x23;            // CS (User Code)
    *(--stack) = (uint64_t)entry; // RIP
    
    *(--stack) = 0; 
    *(--stack) = 0;

    for (int i = 0; i < 15; i++) *(--stack) = 0;

    new_task->rsp = (uint64_t)stack;
    new_task->state = TASK_READY;
    
    cpu_t* target = pick_least_loaded_cpu();
    new_task->cpu_id = target->id;

    enqueue_task(&target->runqueue, new_task);

    return new_task;
}

static void task_reap(task_t* task) {
    if (task->pml4) {
        vmm_destroy_address_space(task->pml4);
    }
    window_destroy_by_pid(task->id);
    if (task->stack_bottom) kfree(task->stack_bottom);
    if (task->user_stack_bottom) kfree(task->user_stack_bottom);
    kfree(task);
}

task_t* task_exec_file(const char* path) {
    int fd = vfs_open(path);
    if (fd < 0) return NULL;

    uint32_t size = vfs_size(fd);
    void* buf = kmalloc(size);
    vfs_read(fd, buf, size);
    vfs_close(fd);

    task_t* task = task_exec(buf);
    kfree(buf);
    return task;
}

static task_t* task_steal() {
    cpu_t* self = get_cpu();
    int count = cpu_get_count();
    for (int i = 0; i < count; i++) {
        cpu_t* victim = cpu_get_by_index(i);
        if (victim == self) continue;
        
        // Attempt to steal from victim's runqueue
        task_t* task = dequeue_task(&victim->runqueue);
        if (task) {
            task->cpu_id = self->id;
            return task;
        }
    }
    return NULL;
}

uint64_t task_schedule(uint64_t current_rsp) {
    cpu_t* cpu = get_cpu();
    if (!cpu->current_task) return current_rsp;

    cpu->current_task->rsp = current_rsp;
    
    // If the current task was running, put it back in the runqueue (unless it's the idle task)
    if (cpu->current_task->state == TASK_RUNNING) {
        cpu->current_task->state = TASK_READY;
        if (cpu->current_task->id != 0) {
            enqueue_task(&cpu->runqueue, cpu->current_task);
        }
    }

    // Pick next task from local runqueue
    task_t* next = NULL;
    while (1) {
        next = dequeue_task(&cpu->runqueue);
        
        // If local queue is empty, try to steal from others
        if (!next) {
            next = task_steal();
            if (!next) break; // Nothing to steal
        }

        if (next->state == TASK_EXITED) {
            task_reap(next);
            continue;
        }

        if (next->state == TASK_READY) {
            break;
        }
        
        enqueue_task(&cpu->runqueue, next);
    }

    if (!next) {
        next = cpu->idle_task;
    }

    cpu->current_task = next;
    cpu->current_task->state = TASK_RUNNING;

    tss_set_rsp0(cpu->current_task->kernel_rsp);
    syscall_set_kernel_stack(cpu->current_task->kernel_rsp);
    vmm_switch_address_space(cpu->current_task->pml4);

    return cpu->current_task->rsp;
}

void task_yield() {
    __asm__ volatile ("int $32"); 
}
