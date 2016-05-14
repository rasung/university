#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdint.h>
#include "lpc17xx_gpio.h"


void init_joystick(void);
uint32_t joyleft (void);
uint32_t joyright (void);

#endif
