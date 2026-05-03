#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init();
uint64_t serial_lock_all();
void serial_unlock_all(uint64_t flags);
void serial_putc(char c);
void serial_print(const char* str);
void serial_print_no_lock(const char* str);
void serial_print_hex(uint64_t val);
void serial_print_dec(uint64_t val);
void serial_print_dec_no_lock(uint64_t val);
void serial_handler();

#endif
