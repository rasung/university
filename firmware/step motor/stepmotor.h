#ifndef _STEPMOTOR_H_
#define _STEPMOTOR_H_

#include <stdint.h>
#include "lpc17xx_gpio.h"

#define SM_NOT 0
#define SM_LEFT 1
#define SM_RIGHT 2

void init_stepmotor (void);
void SM_left_move (int n);
void SM_right_move (int n);
void input_stepmotor(uint32_t value);
void set_SM_state (uint32_t value);
uint32_t get_SM_state (void);
void set_rotation_step (int value);

#endif
