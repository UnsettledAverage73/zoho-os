#ifndef KLOG_H
#define KLOG_H

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

void klog(log_level_t level, const char* module, const char* fmt);

#endif
