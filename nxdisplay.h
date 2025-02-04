#pragma once

#ifndef NXDISPLAY_H
#define NXDISPLAY_H

#include <Arduino.h>

extern void display_start(int start_delay = 0);
extern void display_clear();
extern void display_display();

extern void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
extern void display_value(float value, int size, int x, int y);
extern void display_str(String value, int size, int x, int y);

extern void display_status(String value);

extern void display_add(int size, int x, int y, String item_name, String value_name);

#endif