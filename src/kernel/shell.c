#include "shell.h"
#include "vga.h"
#include "serial.h"
#include "pmm.h"
#include "pit.h"
#include "vfs.h"
#include "task.h"
#include "kmalloc.h"
#include "ata.h"
#include "string.h"
#include "graphics.h"
#include <stddef.h>

#define MAX_BUFFER 256
static char input_buffer[MAX_BUFFER];
static int buffer_idx = 0;

static window_t* shell_window = NULL;
static uint32_t term_row = 0;
static uint32_t term_col = 0;

void shell_set_window(window_t* win) {
    shell_window = win;
    term_row = 0;
    term_col = 0;
    window_mark_dirty(shell_window);
}

static void shell_putchar(char c) {
    if (c == '\n') {
        vga_print("\n");
        serial_print("\n");
        term_col = 0;
        term_row++;
        if (shell_window && term_row * 8 >= shell_window->h - 25) {
            // Very basic scrolling: just clear and reset for now
            for (uint32_t i = 0; i < shell_window->w * (shell_window->h - 25); i++) shell_window->buffer[i] = 0;
            term_row = 0;
            window_mark_dirty(shell_window);
        }
    } else if (c == '\b') {
        if (term_col > 0) {
            term_col--;
            if (shell_window && shell_window->buffer) {
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        shell_window->buffer[(term_row * 8 + i) * shell_window->w + (term_col * 8 + j)] = 0;
                    }
                }
                window_mark_dirty(shell_window);
            }
        }
    } else {
        char s[2] = {c, '\0'};
        vga_print(s);
        serial_print(s);
        if (shell_window && shell_window->buffer) {
            graphics_draw_char(shell_window->buffer, shell_window->w, term_col * 8, term_row * 8, c, 0xFFFFFFFF);
            window_mark_dirty(shell_window);
        }
        term_col++;
        if (shell_window && term_col * 8 >= shell_window->w) {
            term_col = 0;
            term_row++;
            if (term_row * 8 >= shell_window->h - 25) {
                for (uint32_t i = 0; i < shell_window->w * (shell_window->h - 25); i++) shell_window->buffer[i] = 0;
                term_row = 0;
                window_mark_dirty(shell_window);
            }
        }
    }
}

static void shell_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        shell_putchar(str[i]);
    }
}

static void shell_execute(char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        shell_print("Available commands: help, ls, cat <file>, write <f> <t>, exec <f>, free, clear, ticks, gui, stress_proc, stress_disk\n");
    } else if (strcmp(cmd, "ls") == 0) {
        char name[100];
        int i = 0;
        while (vfs_readdir(i++, name) == 0) {
            shell_print(name);
            shell_print("\n");
        }
    } else if (memcmp(cmd, "cat ", 4) == 0) {
        char* filename = cmd + 4;
        int fd = vfs_open(filename);
        if (fd < 0) {
            shell_print("File not found.\n");
        } else {
            uint32_t sz = vfs_size(fd);
            char* buf = (char*)kmalloc(sz + 1);
            vfs_read(fd, buf, sz);
            buf[sz] = '\0';
            shell_print(buf);
            shell_print("\n");
            kfree(buf);
            vfs_close(fd);
        }
    } else if (memcmp(cmd, "exec ", 5) == 0) {
        char* filename = cmd + 5;
        if (task_exec_file(filename)) {
            shell_print("Process started.\n");
        } else {
            shell_print("Failed to start process.\n");
        }
    } else if (strcmp(cmd, "gui") == 0) {
        extern void user_gui_app();
        task_create_user(user_gui_app);
        shell_print("GUI Process started.\n");
    } else if (memcmp(cmd, "write ", 6) == 0) {
        char* rest = cmd + 6;
        char* filename = rest;
        char* data = NULL;
        for (int i = 0; rest[i] != '\0'; i++) {
            if (rest[i] == ' ') {
                rest[i] = '\0';
                data = rest + i + 1;
                break;
            }
        }
        if (data) {
            int fd = vfs_open(filename);
            if (fd < 0) {
                shell_print("File not found.\n");
            } else {
                vfs_write(fd, data, strlen(data));
                vfs_close(fd);
                shell_print("File written.\n");
            }
        } else {
            shell_print("Usage: write <file> <text>\n");
        }
    } else if (strcmp(cmd, "free") == 0) {
        shell_print("Free frames: ");
        serial_print_hex(pmm_get_free_count());
        shell_print("\n");
    } else if (strcmp(cmd, "stress_proc") == 0) {
        shell_print("Starting Process Storm (50 cycles)...\n");
        for (int i = 0; i < 50; i++) {
            extern void user_gui_app();
            task_create_user(user_gui_app);
            for (volatile int j = 0; j < 5000000; j++);
        }
        shell_print("Stress test complete.\n");
    } else if (strcmp(cmd, "stress_disk") == 0) {
        shell_print("Starting Disk Hammer...\n");
        uint8_t buf[512];
        for (int i = 0; i < 512; i++) {
            memset(buf, (uint8_t)i, 512);
            ata_write_sector(5000 + i, buf);
            memset(buf, 0, 512);
            ata_read_sector(5000 + i, buf);
            if (buf[0] != (uint8_t)i) {
                shell_print("DISK CORRUPTION DETECTED!\n");
                break;
            }
        }
        shell_print("Disk stress test complete.\n");
    } else if (strcmp(cmd, "ticks") == 0) {
        shell_print("System Ticks: ");
        serial_print_hex(pit_get_ticks());
        shell_print("\n");
    } else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
        if (shell_window && shell_window->buffer) {
            for (uint32_t i = 0; i < shell_window->w * (shell_window->h - 25); i++) shell_window->buffer[i] = 0;
        }
        term_row = 0;
        term_col = 0;
        window_mark_dirty(shell_window);
    } else if (cmd[0] != '\0') {
        if (task_exec_file(cmd)) {
             shell_print("Process started.\n");
        } else {
            shell_print("Unknown command or file: ");
            shell_print(cmd);
            shell_print("\n");
        }
    }
    shell_print("zoho> ");
}

void shell_init() {
    shell_print("\nZoho OS Interactive Shell\n");
    shell_print("zoho> ");
}

void shell_input(char c) {
    if (c == '\n' || c == '\r') {
        shell_putchar('\n');
        input_buffer[buffer_idx] = '\0';
        if (buffer_idx > 0) {
            shell_execute(input_buffer);
        } else {
            shell_print("zoho> ");
        }
        buffer_idx = 0;
    } else if (c == '\b') {
        if (buffer_idx > 0) {
            buffer_idx--;
            shell_putchar('\b');
        }
    } else {
        if (buffer_idx < MAX_BUFFER - 1) {
            input_buffer[buffer_idx++] = c;
            shell_putchar(c);
        }
    }
}
