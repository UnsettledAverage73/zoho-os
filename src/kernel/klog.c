#include "klog.h"
#include "vga.h"
#include "serial.h"
#include "lock.h"

static const char* level_strings[] = {
    "[DEBUG]",
    "[INFO ]",
    "[WARN ]",
    "[ERROR]"
};

static spinlock_t klog_lock;
static int klog_init_done = 0;

void klog(log_level_t level, const char* module, const char* fmt) {
    if (!klog_init_done) {
        spin_init(&klog_lock);
        klog_init_done = 1;
    }

    uint64_t flags = spin_lock_irqsave(&klog_lock);
    
    // Print to VGA
    vga_print(level_strings[level]);
    vga_print(" ");
    vga_print(module);
    vga_print(": ");
    vga_print(fmt);
    vga_print("\n");

    // Print to Serial
    serial_print(level_strings[level]);
    serial_print(" ");
    serial_print(module);
    serial_print(": ");
    serial_print(fmt);
    serial_print("\n");

    spin_unlock_irqrestore(&klog_lock, flags);
}
