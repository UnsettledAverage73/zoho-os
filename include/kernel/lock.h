#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef struct {
    volatile int locked;
    const char* name;
    volatile uint64_t acquisitions;
    volatile uint64_t contentions;
    volatile uint64_t spin_loops;
} spinlock_t;

typedef struct {
    const char* name;
    uint64_t acquisitions;
    uint64_t contentions;
    uint64_t spin_loops;
} lock_stats_t;

void spin_init_named(spinlock_t* lock, const char* name);
#define spin_init(lock) spin_init_named((lock), #lock)
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);

// Interrupt-safe locking
uint64_t spin_lock_irqsave(spinlock_t* lock);
void spin_unlock_irqrestore(spinlock_t* lock, uint64_t flags);
uint32_t lock_get_registered_count(void);
int lock_get_stats(uint32_t index, lock_stats_t* out_stats);

#endif
