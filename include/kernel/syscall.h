#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "window.h"

void syscall_init();
void syscall_set_kernel_stack(uint64_t stack);

uint32_t sys_create_window(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
int sys_get_event(uint32_t win_id, gui_event_t* out_event);
void sys_exit(int status);
int sys_open(const char* path);
int sys_read(int fd, void* buffer, uint32_t count);
int sys_write(int fd, const void* buffer, uint32_t count);
void sys_close(int fd);
uint32_t sys_exec(const char* path);
void sys_yield();
uint64_t sys_free_frames();

#endif
