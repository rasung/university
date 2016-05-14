#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <stdint.h>
#include "lpc17xx_gpio.h"
#include "queue.h"
#include "keyscan.h"
#include "fnd.h"


uint32_t decimal (int n);
uint32_t decimalize_fndvalue (void);
uint32_t calculate (void);
void keypad_operand(uint32_t value);
void keypad_result(void);
void keypad_operator(uint32_t value);

#endif
