#include "lpc17xx_gpio.h"
#include "queue.h"

uint32_t FNDVALUE = 0x00000000; // FND 출력 초기 값

uint32_t fndmap[0x10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e}; // 연산 FND OFF출력
uint32_t binmap[0x08] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void fnd_digit(uint32_t digit);
void fnd_init(void);
void fnd_portout(uint32_t value);
void clock_pin(uint8_t port, uint32_t pin);
uint32_t get_key(uint8_t ch);

void fnd_digit(uint32_t digit)
{
	uint32_t value;
	
	fnd_init();
	GPIO_ClearValue(0,0x01<<4);
	GPIO_ClearValue(0,0x01<<20);
	
	fnd_portout(0);
	clock_pin(0,10);
	
	value = (FNDVALUE >> (digit*4)) & 0x0F;
	fnd_portout(fndmap[value]);
	clock_pin(0,5);
	
	fnd_portout(binmap[digit]);
	clock_pin(0,10);
	GPIO_SetValue(0,0x01<<20);
}

void fnd_portout(uint32_t value)
{
	if(value & 0x01) GPIO_SetValue(0,1<<23);
	else GPIO_ClearValue(0,1<<23);
	if(value & 0x02) GPIO_SetValue(0,1<<24);
	else GPIO_ClearValue(0,1<<24);
	if(value & 0x04) GPIO_SetValue(1,1<<21);
	else GPIO_ClearValue(1,1<<21);
	if(value & 0x08) GPIO_SetValue(2,1<<11);
	else GPIO_ClearValue(2,1<<11);
	if(value & 0x10) GPIO_SetValue(2,1<<12);
	else GPIO_ClearValue(2,1<<12);
	if(value & 0x20) GPIO_SetValue(2,1<<13);
	else GPIO_ClearValue(2,1<<13);
	if(value & 0x40) GPIO_SetValue(3,1<<25);
	else GPIO_ClearValue(3,1<<25);
	if(value & 0x80) GPIO_SetValue(3,1<<26);
	else GPIO_ClearValue(3,1<<26);
}

void clock_pin(uint8_t port, uint32_t pin)
{
	GPIO_SetValue(port, 1<<pin);
	GPIO_ClearValue(port, 1<<pin);
}

void fnd_init()
{
	GPIO_SetDir(0, 1<<4, 1);
	GPIO_SetDir(0, 0x07<<19, 1);
	
	GPIO_SetDir(0, 0x03<<23, 1);
	GPIO_SetDir(1, 0x01<<21, 1);
	GPIO_SetDir(2, 0x07<<11, 1);
	GPIO_SetDir(3, 0x03<<25, 1);
	
	GPIO_SetDir(0, 0x01<<5, 1);
	GPIO_SetDir(0, 0x01<<10, 1);
	
	GPIO_SetValue(0, 0x07<<19);
	GPIO_ClearValue(0, 0x01<<5);
	GPIO_ClearValue(0, 0x01<<10);
}

uint32_t get_key(uint8_t ch)
{
	uint32_t key = 0xFFFFFFFF;
	switch(ch){
		case '1' : key = 1; break;
		case '2' : key = 2; break;
		case '3' : key = 3; break;
		case '4' : key = 4; break;
		case '5' : key = 5; break;
		case '6' : key = 6; break;
		case '7' : key = 7; break;
		case '8' : key = 8; break;
		case '9' : key = 9; break;
		case '0' : key = 0; break;
		default: break;
	}
	return key;
}