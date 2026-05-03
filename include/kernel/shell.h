#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include "window.h"

void shell_init();
void shell_input(char c);
void shell_set_window(window_t* win);

#endif
