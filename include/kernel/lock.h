#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef struct {
    volatile int locked;
} spinlock_t;

void spin_init(spinlock_t* lock);
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);

// Interrupt-safe locking
uint64_t spin_lock_irqsave(spinlock_t* lock);
void spin_unlock_irqrestore(spinlock_t* lock, uint64_t flags);

#endif
