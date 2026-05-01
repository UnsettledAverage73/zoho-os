#include "lock.h"

void spin_init(spinlock_t* lock) {
    lock->locked = 0;
}

void spin_lock(spinlock_t* lock) {
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        // Spin until we get the lock
        __asm__ volatile ("pause");
    }
}

void spin_unlock(spinlock_t* lock) {
    __sync_lock_release(&lock->locked);
}
