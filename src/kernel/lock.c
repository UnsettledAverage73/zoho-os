#include "lock.h"

void spin_init(spinlock_t* lock) {
    lock->locked = 0;
}

void spin_lock(spinlock_t* lock) {
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        __asm__ volatile ("pause");
    }
}

void spin_unlock(spinlock_t* lock) {
    __sync_lock_release(&lock->locked);
}

uint64_t spin_lock_irqsave(spinlock_t* lock) {
    uint64_t flags;
    __asm__ volatile (
        "pushfq\n"
        "pop %0\n"
        "cli"
        : "=rm"(flags)
        :
        : "memory"
    );
    spin_lock(lock);
    return flags;
}

void spin_unlock_irqrestore(spinlock_t* lock, uint64_t flags) {
    spin_unlock(lock);
    __asm__ volatile (
        "push %0\n"
        "popfq"
        :
        : "rm"(flags)
        : "memory"
    );
}
