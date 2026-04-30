#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include "multiboot2.h"

#define PAGE_SIZE 4096

void pmm_init(struct multiboot_info* mb_info);
void* pmm_alloc_frame();
void pmm_free_frame(void* frame);

#endif
