#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

void initialize(void);
void change_LED(void);
uint32_t sw3; // switch 3
uint32_t count; // sw3 count 
uint32_t count2 = 0; // 이전 카운트 값 저장할 변수
PINSEL_CFG_Type pincfg = {2,10,1,0,0}; // P2.10의 기능을 /EINT0으로 설정

void EINT0_IRQHandler(void) // 인터럽트 서비스 루틴
{
	LPC_SC->EXTINT = 1; // Clear Interrupt
	count2 = count; // 이전 카운트 저장
	count = (count + 1) % 8; // 카운트 값 변경
}

int main(void)
{
	initialize();

	while (1) {
		if (count != count2){ // 카운트 값이 변경되었다면
			change_LED(); // change LED
		}
	}
}

void initialize(void)
{
	count = 0;
	GPIO_SetDir(1, 0xB0000000, 1); // GPIO LED 포트 설정
	GPIO_SetDir(2, 0x0000007C, 1);
	GPIO_SetDir(2, 0x00000400, 0); // SW3 포트 설정
	GPIO_ClearValue(1, 0xB0000000); // LED 초기화
	GPIO_ClearValue(2, 0x0000007C);
	change_LED();
	PINSEL_ConfigPin(&pincfg); // P2.10의 기능을 /EINT0으로 설정
	LPC_SC->EXTMODE = 0x1; 
	LPC_SC->EXTPOLAR  = 0x0; // /EINT0가 falling edge에서 인터럽트를 요청하도록 설정
	NVIC_EnableIRQ(EINT0_IRQn); // NVIC의 /EINT0을 인에이블
}

void change_LED()
{
	GPIO_ClearValue(1, 0xB0000000);
	GPIO_ClearValue(2, 0x0000007C);
	switch(count) {
		case 0: GPIO_SetValue(1, 1U<<28); break; 
		case 1: GPIO_SetValue(1, 1U<<29); break;
		case 2: GPIO_SetValue(1, 1U<<31); break;
		case 3: GPIO_SetValue(2, 1U<<2); break;
		case 4: GPIO_SetValue(2, 1U<<3);  break;
		case 5: GPIO_SetValue(2, 1U<<4);  break;
		case 6: GPIO_SetValue(2, 1U<<5);  break;
		case 7: GPIO_SetValue(2, 1U<<6);  break;
		default: break;
	}
}