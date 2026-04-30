#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "pmm.h"
#include "vmm.h"
#include "shell.h"
#include "klog.h"
#include "pit.h"
#include "kmalloc.h"
#include "task.h"

void task_test_1() {
    while(1) {
        klog(LOG_INFO, "TASK1", "Running thread A");
        for(volatile int i=0; i<100000000; i++);
        // No yield! Preemption handles it.
    }
}

void task_test_2() {
    while(1) {
        klog(LOG_INFO, "TASK2", "Running thread B");
        for(volatile int i=0; i<100000000; i++);
    }
}

void kmain(struct multiboot_info* mb_info) {
    serial_init();
    vga_clear();
    klog(LOG_INFO, "KERNEL", "Zoho OS Booted successfully in 64-bit Long Mode!");
    
    klog(LOG_INFO, "GDT", "Initializing GDT/TSS...");
    gdt_init();
    
    klog(LOG_INFO, "IDT", "Initializing IDT/Exceptions/PIC...");
    idt_init();

    klog(LOG_INFO, "PMM", "Initializing PMM...");
    pmm_init(mb_info);

    klog(LOG_INFO, "VMM", "Initializing VMM...");
    vmm_init();

    klog(LOG_INFO, "VMM", "Testing VMM mapping...");
    void* frame = pmm_alloc_frame();
    vmm_map(0xDEADBEEF000, (uint64_t)frame, PAGE_WRITABLE);
    uint64_t* ptr = (uint64_t*)0xDEADBEEF000;
    *ptr = 0xCAFEBABE;
    if (*ptr == 0xCAFEBABE) {
        klog(LOG_INFO, "VMM", "VMM Mapping Test: SUCCESS");
    }

    klog(LOG_INFO, "PIT", "Initializing PIT...");
    pit_init(100);

    klog(LOG_INFO, "KMALLOC", "Initializing kmalloc...");
    kmalloc_init();

    klog(LOG_INFO, "TASK", "Initializing Task System...");
    task_init();
    task_create(task_test_1);
    task_create(task_test_2);

    klog(LOG_INFO, "SHELL", "Initializing Shell...");
    shell_init();

    klog(LOG_INFO, "KERNEL", "System components initialized.");

    while(1) {
        task_yield();
    }
}
