#ifndef _KEYSCAN_H_
#define _KEYSCAN_H_

#include <stdint.h>
#include "lpc17xx_gpio.h"
#include "fnd.h"

static void put_scancode(uint32_t scan_x);
static uint32_t get_scancode(void);
uint32_t get_key1(uint32_t code);
static void keypad_Init(void);
uint32_t scan_keypad(void);

#endif
