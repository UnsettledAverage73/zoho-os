#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef struct {
    volatile int locked;
} spinlock_t;

void spin_init(spinlock_t* lock);
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);

#endif
