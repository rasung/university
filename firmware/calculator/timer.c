#include "timer.h"

static uint32_t TIMER1_TICK = 0;
static uint32_t FND_DIGIT = 0;    //?
static uint32_t keypad_state = 1;

void TIMER1_IRQHandler(void)
{
  uint32_t scan_code;
  TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);

  TIMER1_TICK = (TIMER1_TICK + 1) % 999;

  if ((TIMER1_TICK % 20) == 0) {
    scan_code = scan_keypad();
    if (scan_code != 0xffffffff) {
      if (keypad_state == 1) {
        keypad_state = 0;
        key_enqueue(scan_code);
      }
      else if (scan_code == 0xffffffff) {
        keypad_state == 1;
      }
    }
  }
  fnd_digit(FND_DIGIT);
  FND_DIGIT = (FND_DIGIT + 1) % 8;
}

void init_timer1(void)
{
  TIM_TIMERCFG_Type TIM_ConfigStruct_1;
  TIM_MATCHCFG_Type TIM_MatchConfigStruct_11;

  TIM_ConfigStruct_1.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct_1.PrescaleValue = 1;
  TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct_1);

  TIM_MatchConfigStruct_11.MatchChannel = 1;
  TIM_MatchConfigStruct_11.IntOnMatch = ENABLE;
  TIM_MatchConfigStruct_11.StopOnMatch = DISABLE;
  TIM_MatchConfigStruct_11.ResetOnMatch = ENABLE;
  TIM_MatchConfigStruct_11.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
  TIM_MatchConfigStruct_11.MatchValue = 999;

  TIM_ConfigMatch(LPC_TIM1, &TIM_MatchConfigStruct_11);

  TIM_Cmd(LPC_TIM1,ENABLE);

  NVIC_EnableIRQ(TIMER1_IRQn);
}
