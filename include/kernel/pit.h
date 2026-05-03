#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_init(uint32_t frequency);
uint64_t pit_get_ticks();
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void tsc_calibrate();
uint64_t rdtsc();

#endif
