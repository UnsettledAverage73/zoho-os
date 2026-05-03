#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include "graphics.h"

#define GUI_EVENT_MOUSE_CLICK 1
#define GUI_EVENT_MOUSE_MOVE  2
#define GUI_EVENT_KEY_PRESS   3

typedef struct {
    uint32_t type;
    uint32_t x, y;
    uint32_t data;
} gui_event_t;

typedef struct window {
    uint32_t id;
    uint32_t x, y;
    uint32_t w, h;
    uint32_t title_color;
    uint32_t bg_color;
    uint32_t* buffer;      // Shared buffer (kernel side pointer)
    uint64_t owner_pid;
    uint8_t dirty;
    
    gui_event_t event_queue[16];
    uint8_t event_head;
    uint8_t event_tail;

    struct window* next;
} window_t;

void window_init();
window_t* window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t bg_color);
window_t* window_get_by_id(uint32_t id);
void window_destroy_by_pid(uint64_t pid);
void window_update();
void window_draw_all();
void window_mark_dirty(window_t* win);
int window_needs_redraw();

#endif
