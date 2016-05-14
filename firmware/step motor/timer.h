#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include "stepmotor.h"
#include "lpc17xx_timer.h"

void TIMER0_IRQHandler(void);
void init_timer0(void);

#endif
