#ifndef KTRACE_H
#define KTRACE_H

#include <stdint.h>

void ktrace_init(void);
void ktrace_log(const char* event);

#endif
