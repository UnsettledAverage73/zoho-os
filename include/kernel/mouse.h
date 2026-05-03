#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

void mouse_init();
void mouse_handler();
int32_t mouse_get_x();
int32_t mouse_get_y();
uint8_t mouse_get_buttons();

#endif
