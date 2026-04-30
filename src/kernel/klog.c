#include "klog.h"
#include "vga.h"
#include "serial.h"

static const char* level_strings[] = {
    "[DEBUG]",
    "[INFO ]",
    "[WARN ]",
    "[ERROR]"
};

void klog(log_level_t level, const char* module, const char* fmt) {
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
}
