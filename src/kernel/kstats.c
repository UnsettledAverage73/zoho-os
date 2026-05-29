#include "kstats.h"
#include "klog.h"
#include "pmm.h"
#include "task.h"

static kernel_stats_t current_stats;

void kstats_init(void) {
    klog(LOG_INFO, "KSTATS", "Initializing kernel statistics...");
    current_stats.uptime_ms = 0;
    current_stats.total_memory = pmm_get_total_frames() * PAGE_SIZE;
    current_stats.used_memory = (pmm_get_total_frames() - pmm_get_free_count()) * PAGE_SIZE;
    current_stats.active_tasks = 0; // Will be updated by scheduler
    current_stats.page_faults = 0;
    klog(LOG_INFO, "KSTATS", "Kernel statistics initialized.");
}

kernel_stats_t kstats_get(void) {
    return current_stats;
}

void kstats_update(void) {
    // This would be called by a timer interrupt or on demand
    current_stats.used_memory = (pmm_get_total_frames() - pmm_get_free_count()) * PAGE_SIZE;
    // active_tasks and page_faults would be updated by their respective subsystems
}
