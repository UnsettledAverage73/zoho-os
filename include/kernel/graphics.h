#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "multiboot2.h"

typedef struct {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} rect_t;

void graphics_init(struct multiboot_tag_framebuffer* tag);
void graphics_swap();
void graphics_swap_rects(const rect_t* rects, uint32_t count);
void graphics_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void graphics_clear(uint32_t color);
void graphics_clear_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void graphics_draw_line_buffer(uint32_t x, uint32_t y, uint32_t w, uint32_t* buffer);
void graphics_draw_cursor(uint32_t x, uint32_t y);
void graphics_draw_char(uint32_t* buf, uint32_t w, uint32_t x, uint32_t y, char c, uint32_t color);
void graphics_draw_string(uint32_t* buf, uint32_t w, uint32_t x, uint32_t y, const char* str, uint32_t color);
uint32_t graphics_get_width();
uint32_t graphics_get_height();
uint64_t graphics_get_fb_addr();

#endif
