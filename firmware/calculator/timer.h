#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "keyscan.h"
#include "fnd.h"
#include "queue.h"
#include "calculator.h"


void init_timer1(void);
static void TIMER1_IRQHandler(void);

#endif
