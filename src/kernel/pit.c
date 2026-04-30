#include "pit.h"
#include "klog.h"

static uint64_t ticks = 0;

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193182 / frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);

    klog(LOG_INFO, "PIT", "Timer initialized at 100Hz");
}

void pit_handler() {
    ticks++;
}

uint64_t pit_get_ticks() {
    return ticks;
}
