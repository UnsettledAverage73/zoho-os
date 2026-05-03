#ifndef APIC_H
#define APIC_H

#include <stdint.h>

void lapic_init();
uint8_t lapic_get_id();
void lapic_eoi();
void lapic_timer_init(uint32_t hz);
void lapic_send_ipi(uint8_t apic_id, uint32_t vector);

#endif
