#include "lpc17xx_timer.h"
#include "fnd.h"
 
 uint32_t FNDDIGIT; // 표시할 자릿수
 
void TIMER1_IRQHandler(void)
{
	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

	fnd_digit(FNDDIGIT);
	FNDDIGIT = (FNDDIGIT + 1) % 8;
}

void init_timer1()
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;
	
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue = 1;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct);
	
	TIM_MatchConfigStruct.MatchChannel = 0;
	TIM_MatchConfigStruct.IntOnMatch = ENABLE;
	TIM_MatchConfigStruct.StopOnMatch = DISABLE;
	TIM_MatchConfigStruct.ResetOnMatch = ENABLE;
	TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_MatchConfigStruct.MatchValue = 1000;
	TIM_ConfigMatch(LPC_TIM1, &TIM_MatchConfigStruct);
	
	TIM_Cmd(LPC_TIM1, ENABLE);
	NVIC_EnableIRQ(TIMER1_IRQn);
}