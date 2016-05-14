#include "timer.h"

static uint32_t TIMER0_TICK = 0;
uint32_t test;

void TIMER0_IRQHandler(void) {
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

  TIMER0_TICK = (TIMER0_TICK + 1) % 1000;

  if ((TIMER0_TICK % 10) == 0) {
    test = get_SM_state();
    switch (test) {
      case SM_NOT:
			set_rotation_step(0);
      break;

      case SM_LEFT:
			GPIO_SetValue(1, 0xB0000000);
      SM_left_move(1);
			GPIO_ClearValue(1, 0xB0000000);
      break;

      case SM_RIGHT:
			GPIO_SetValue(2, 0x0000007C);
      SM_right_move(1);
			GPIO_ClearValue(2, 0x0000007C);
      break;
    }
  }
}

void init_timer0(void) {
  TIM_TIMERCFG_Type TIM_ConfigStruct_0;
  TIM_MATCHCFG_Type TIM_MatchConfigStruct_00;

  TIM_ConfigStruct_0.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct_0.PrescaleValue = 1;
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TIM_ConfigStruct_0);

  TIM_MatchConfigStruct_00.MatchChannel = 0;
  TIM_MatchConfigStruct_00.IntOnMatch = ENABLE;
  TIM_MatchConfigStruct_00.StopOnMatch = DISABLE;
  TIM_MatchConfigStruct_00.ResetOnMatch = ENABLE;
  TIM_MatchConfigStruct_00.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
  TIM_MatchConfigStruct_00.MatchValue = 999;

  TIM_ConfigMatch(LPC_TIM0, &TIM_MatchConfigStruct_00);

  TIM_Cmd(LPC_TIM0,ENABLE);

  NVIC_EnableIRQ(TIMER0_IRQn);
}
