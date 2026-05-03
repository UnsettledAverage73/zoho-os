#include "syscall.h"
#include "klog.h"
#include "vga.h"
#include "cpu.h"
#include "window.h"
#include "pmm.h"
#include "vmm.h"
#include "string.h"
#include "kmalloc.h"
#include "vfs.h"
#include "task.h"

#define MSR_EFER         0xC0000080
#define MSR_STAR         0xC0000081
#define MSR_LSTAR        0xC0000082
#define MSR_SFMASK       0xC0000084
#define MSR_GS_BASE      0xC0000101
#define MSR_KERNEL_GS_BASE 0xC0000102

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

extern void syscall_entry();

uint32_t sys_create_window(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    window_t* win = window_create(x, y, w, h, 0xFFFFFFFF);
    cpu_t* cpu = get_cpu();
    win->owner_pid = cpu->current_task->id;

    uint32_t client_h = h - 25;
    uint32_t size = w * client_h * 4;
    uint32_t num_pages = (size + 4095) / 4096;

    uint32_t* kernel_buf = kmalloc(num_pages * 4096);
    memset(kernel_buf, 0, num_pages * 4096);
    win->buffer = kernel_buf;

    uint64_t user_addr = 0xA0000000 + (win->id * 0x1000000);
    for (uint32_t i = 0; i < num_pages; i++) {
        vmm_map(cpu->current_task->pml4, user_addr + i * 4096, (uint64_t)kernel_buf + i * 4096, PAGE_USER | PAGE_WRITABLE);
    }

    return win->id;
}

int sys_get_event(uint32_t win_id, gui_event_t* out_event) {
    window_t* win = window_get_by_id(win_id);
    if (!win) return -1;
    if (win->event_head == win->event_tail) return 0;

    memcpy(out_event, &win->event_queue[win->event_head], sizeof(gui_event_t));
    win->event_head = (win->event_head + 1) % 16;
    return 1;
}

void sys_exit(int status) {
    (void)status;
    cpu_t* cpu = get_cpu();
    cpu->current_task->state = TASK_EXITED;
    task_yield();
}

int sys_open(const char* path) {
    return vfs_open(path);
}

int sys_read(int fd, void* buffer, uint32_t count) {
    return vfs_read(fd, buffer, count);
}

int sys_write(int fd, const void* buffer, uint32_t count) {
    (void)fd; // For now, fd ignored, always write to console
    const char* buf = (const char*)buffer;
    for (uint32_t i = 0; i < count; i++) {
        vga_print_char(buf[i]);
    }
    return count;
}

void sys_close(int fd) {
    vfs_close(fd);
}

uint32_t sys_exec(const char* path) {
    task_t* task = task_exec_file(path);
    if (task) return task->id;
    return 0;
}

void sys_yield() {
    task_yield();
}

uint64_t sys_free_frames() {
    return pmm_get_free_count();
}

uint64_t syscall_handler_c(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    (void)arg5;
    switch(num) {
        case 1: return sys_create_window((uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4);
        case 2: return sys_get_event((uint32_t)arg1, (gui_event_t*)arg2);
        case 3: sys_exit((int)arg1); return 0;
        case 4: return sys_open((const char*)arg1);
        case 5: return sys_read((int)arg1, (void*)arg2, (uint32_t)arg3);
        case 6: return sys_write((int)arg1, (const void*)arg2, (uint32_t)arg3);
        case 7: sys_close((int)arg1); return 0;
        case 8: return sys_exec((const char*)arg1);
        case 9: sys_yield(); return 0;
        case 10: return sys_free_frames();
        default: return 0xFFFFFFFFFFFFFFFF;
    }
}

void syscall_set_kernel_stack(uint64_t stack) {
    get_cpu()->kernel_stack = stack;
}

void syscall_init() {
    uint64_t efer = rdmsr(MSR_EFER);
    wrmsr(MSR_EFER, efer | 1);
    wrmsr(MSR_STAR, ((uint64_t)0x10 << 48) | ((uint64_t)0x08 << 32));
    wrmsr(MSR_LSTAR, (uint64_t)syscall_entry);
    wrmsr(MSR_SFMASK, 0x200);
    klog(LOG_INFO, "SYSCALL", "Syscall system initialized");
}
