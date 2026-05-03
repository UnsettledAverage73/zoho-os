#include "window.h"
#include "kmalloc.h"
#include "graphics.h"
#include "mouse.h"
#include "lock.h"

static window_t* window_list = NULL;
static uint32_t next_window_id = 1;
static spinlock_t window_lock;
static uint8_t scene_dirty = 1;

void window_init() {
    window_list = NULL;
    spin_init(&window_lock);
    scene_dirty = 1;
}

window_t* window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t bg_color) {
    uint64_t flags = spin_lock_irqsave(&window_lock);
    window_t* win = kmalloc(sizeof(window_t));
    win->id = next_window_id++;
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->bg_color = bg_color;
    win->title_color = 0xFF555555;
    win->buffer = NULL;
    win->owner_pid = 0;
    win->dirty = 1;
    win->event_head = 0;
    win->event_tail = 0;

    win->next = window_list;
    window_list = win;
    scene_dirty = 1;
    spin_unlock_irqrestore(&window_lock, flags);
    return win;
}

void window_mark_dirty(window_t* win) {
    if (!win) return;
    uint64_t flags = spin_lock_irqsave(&window_lock);
    win->dirty = 1;
    scene_dirty = 1;
    spin_unlock_irqrestore(&window_lock, flags);
}

int window_needs_redraw() {
    return scene_dirty != 0;
}

void window_push_event(window_t* win, gui_event_t event) {
    // Note: We assume the caller or the window structure 
    // handles atomic queue access if needed.
    // For now, let's keep it simple as events are per-window.
    uint8_t next = (win->event_tail + 1) % 16;
    if (next == win->event_head) return; 
    win->event_queue[win->event_tail] = event;
    win->event_tail = next;
}

window_t* window_get_by_id(uint32_t id) {
    uint64_t flags = spin_lock_irqsave(&window_lock);
    window_t* curr = window_list;
    while (curr) {
        if (curr->id == id) {
            spin_unlock_irqrestore(&window_lock, flags);
            return curr;
        }
        curr = curr->next;
    }
    spin_unlock_irqrestore(&window_lock, flags);
    return NULL;
}

void window_destroy_by_pid(uint64_t pid) {
    uint64_t flags = spin_lock_irqsave(&window_lock);
    window_t* curr = window_list;
    window_t* prev = NULL;

    while (curr) {
        if (curr->owner_pid == pid) {
            if (prev) prev->next = curr->next;
            else window_list = curr->next;

            window_t* to_free = curr;
            curr = curr->next;
            if (to_free->buffer) kfree(to_free->buffer);
            kfree(to_free);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    spin_unlock_irqrestore(&window_lock, flags);
}

static window_t* dragging_window = NULL;
static int32_t drag_off_x, drag_off_y;

void window_update() {
    int32_t mx = mouse_get_x();
    int32_t my = mouse_get_y();
    uint8_t mb = mouse_get_buttons();

    static uint8_t last_mb = 0;
    static int32_t last_mx = 0, last_my = 0;

    uint64_t flags = spin_lock_irqsave(&window_lock);

    if (mb & 1) { // Left click
        if (!dragging_window) {
            window_t* curr = window_list;
            while (curr) {
                if (mx >= (int32_t)curr->x && mx < (int32_t)(curr->x + curr->w)) {
                    if (my >= (int32_t)curr->y && my < (int32_t)(curr->y + 25)) { 
                        dragging_window = curr;
                        drag_off_x = mx - curr->x;
                        drag_off_y = my - curr->y;
                        
                        if (curr != window_list) {
                            window_t* prev = window_list;
                            while(prev && prev->next != curr) prev = prev->next;
                            if (prev) {
                                prev->next = curr->next;
                                curr->next = window_list;
                                window_list = curr;
                                scene_dirty = 1;
                            }
                        }
                        break;
                    } else if (my >= (int32_t)curr->y + 25 && my < (int32_t)(curr->y + curr->h)) {
                        if (!(last_mb & 1)) {
                            gui_event_t ev = {GUI_EVENT_MOUSE_CLICK, (uint32_t)(mx - curr->x), (uint32_t)(my - curr->y - 25), 1};
                            window_push_event(curr, ev);
                            scene_dirty = 1;
                        }
                        break;
                    }
                }
                curr = curr->next;
            }
        } else {
            dragging_window->x = mx - drag_off_x;
            dragging_window->y = my - drag_off_y;
            dragging_window->dirty = 1;
            scene_dirty = 1;
        }
    } else {
        dragging_window = NULL;
    }

    if (mx != last_mx || my != last_my) {
        window_t* curr = window_list;
        while (curr) {
            if (mx >= (int32_t)curr->x && mx < (int32_t)(curr->x + curr->w) &&
                my >= (int32_t)curr->y + 25 && my < (int32_t)(curr->y + curr->h)) {
                gui_event_t ev = {GUI_EVENT_MOUSE_MOVE, (uint32_t)(mx - curr->x), (uint32_t)(my - curr->y - 25), 0};
                window_push_event(curr, ev);
                scene_dirty = 1;
                break;
            }
            curr = curr->next;
        }
    }

    if (mb != last_mb) scene_dirty = 1;

    last_mb = mb;
    last_mx = mx;
    last_my = my;
    spin_unlock_irqrestore(&window_lock, flags);
}

void window_draw_all() {
    graphics_clear(0xFF112233);

    uint64_t flags = spin_lock_irqsave(&window_lock);
    window_t* stack[16];
    int count = 0;
    window_t* curr = window_list;
    while(curr && count < 16) {
        stack[count++] = curr;
        curr = curr->next;
    }

    for (int i = count - 1; i >= 0; i--) {
        curr = stack[i];
        graphics_draw_rect(curr->x + 5, curr->y + 5, curr->w, curr->h, 0xAA000000);
        if (curr->buffer) {
            for (uint32_t wy = 0; wy < curr->h - 25; wy++) {
                for (uint32_t wx = 0; wx < curr->w; wx++) {
                    graphics_put_pixel(curr->x + wx, curr->y + 25 + wy, curr->buffer[wy * curr->w + wx]);
                }
            }
        } else {
            graphics_draw_rect(curr->x, curr->y + 25, curr->w, curr->h - 25, curr->bg_color);
        }
        graphics_draw_rect(curr->x, curr->y, curr->w, 25, curr->title_color);
        graphics_draw_rect(curr->x, curr->y, curr->w, 1, 0xFFFFFFFF);
        graphics_draw_rect(curr->x, curr->y + curr->h - 1, curr->w, 1, 0xFFFFFFFF);
        graphics_draw_rect(curr->x, curr->y, 1, curr->h, 0xFFFFFFFF);
        graphics_draw_rect(curr->x + curr->w - 1, curr->y, 1, curr->h, 0xFFFFFFFF);
        curr->dirty = 0;
    }
    scene_dirty = 0;
    spin_unlock_irqrestore(&window_lock, flags);

    graphics_draw_cursor(mouse_get_x(), mouse_get_y());
    graphics_swap();
}
