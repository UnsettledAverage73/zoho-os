#include "gdt.h"

static struct {
    struct gdt_entry entries[5];
    struct gdt_tss_entry tss;
} __attribute__((packed)) gdt;

static struct gdt_ptr gdt_ptr;
static struct tss_entry tss;

extern void gdt_load(struct gdt_ptr* ptr);
extern void tss_load();

static void gdt_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt.entries[i].base_low = (base & 0xFFFF);
    gdt.entries[i].base_middle = (base >> 16) & 0xFF;
    gdt.entries[i].base_high = (base >> 24) & 0xFF;
    gdt.entries[i].limit_low = (limit & 0xFFFF);
    gdt.entries[i].granularity = (limit >> 16) & 0x0F;
    gdt.entries[i].granularity |= gran & 0xF0;
    gdt.entries[i].access = access;
}

static void gdt_set_tss(int i, uint64_t base, uint32_t limit) {
    struct gdt_tss_entry* entry = &gdt.tss;
    entry->limit_low = (limit & 0xFFFF);
    entry->base_low = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->access = 0x89; // Present, Executable, Accessible, TSS
    entry->granularity = (limit >> 16) & 0x0F;
    entry->base_high = (base >> 24) & 0xFF;
    entry->base_higher = (base >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}

void gdt_init() {
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint64_t)&gdt;

    gdt_set_entry(0, 0, 0, 0, 0);                // Null
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xAF); // Kernel Code
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xAF); // Kernel Data
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xAF); // User Code
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xAF); // User Data

    // Setup TSS
    for (int i = 0; i < sizeof(tss); i++) ((char*)&tss)[i] = 0;
    tss.iopb_offset = sizeof(tss);
    
    gdt_set_tss(5, (uint64_t)&tss, sizeof(tss) - 1);

    gdt_load(&gdt_ptr);
    tss_load();
}
