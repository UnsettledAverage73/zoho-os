#ifndef KSTATS_H
#define KSTATS_H

#include <stdint.h>

typedef struct {
    uint64_t uptime_ms;
    uint64_t total_memory;
    uint64_t used_memory;
    uint32_t active_tasks;
    uint64_t page_faults;
} kernel_stats_t;

void kstats_init(void);
kernel_stats_t kstats_get(void);
void kstats_update(void);

#endif
