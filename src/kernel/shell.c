#include "shell.h"
#include "vga.h"
#include "serial.h"
#include "pmm.h"
#include "pit.h"
#include <stddef.h>

#define MAX_BUFFER 256
static char input_buffer[MAX_BUFFER];
static int buffer_idx = 0;

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static void shell_execute(char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        vga_print("Available commands: help, meminfo, panic, clear, echo, ticks, crash\n");
        serial_print("Available commands: help, meminfo, panic, clear, echo, ticks, crash\n");
    } else if (strcmp(cmd, "meminfo") == 0) {
        vga_print("Physical Memory Info:\n");
        serial_print("Physical Memory Info:\n");
        // ...
    } else if (strcmp(cmd, "ticks") == 0) {
        vga_print("System Ticks: ");
        vga_print_hex(pit_get_ticks());
        vga_print("\n");
        serial_print("System Ticks: ");
        serial_print_hex(pit_get_ticks());
        serial_print("\n");
    } else if (strcmp(cmd, "crash") == 0) {
        vga_print("Crashing kernel via null pointer dereference...\n");
        serial_print("Crashing kernel via null pointer dereference...\n");
        volatile int* p = (volatile int*)0;
        *p = 123;
    } else if (strcmp(cmd, "panic") == 0) {
        __asm__ volatile ("int $3");
    } else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
    } else if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
        vga_print(cmd + 5);
        vga_print("\n");
        serial_print(cmd + 5);
        serial_print("\n");
    } else if (cmd[0] != '\0') {
        vga_print("Unknown command: ");
        vga_print(cmd);
        vga_print("\n");
        serial_print("Unknown command: ");
        serial_print(cmd);
        serial_print("\n");
    }
    vga_print("zoho> ");
    serial_print("zoho> ");
}

void shell_init() {
    vga_print("\nZoho OS Kernel Debug Shell\n");
    vga_print("Type 'help' for a list of commands.\n");
    vga_print("zoho> ");
    serial_print("\nZoho OS Kernel Debug Shell\n");
    serial_print("zoho> ");
}

void shell_input(char c) {
    if (c == '\n') {
        vga_print("\n");
        serial_print("\n");
        input_buffer[buffer_idx] = '\0';
        shell_execute(input_buffer);
        buffer_idx = 0;
    } else if (c == '\b') {
        if (buffer_idx > 0) {
            buffer_idx--;
        }
    } else {
        if (buffer_idx < MAX_BUFFER - 1) {
            input_buffer[buffer_idx++] = c;
            char s[2] = {c, '\0'};
            vga_print(s);
            serial_print(s);
        }
    }
}
