#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "serial.h"
#include "timer.h"
#include "fnd.h"

#define ENTER '\r'

#define STATE_OP1 0
#define STATE_OP2 1

int state = STATE_OP1;
int value,value2;
char cal;
void initialize(void);
void calculator(char cal); // 연산자 계산
char buffer[81];
void buffer_init(void); // 버퍼 값 초기화
uint32_t key;
int StringToHexa(const char *parm_string); // 10진수를 문자 그대로 16진수로 변환

int main()
{
	uint8_t ch;
	int bufptr = 0;
	
	initialize();

	while(1) {
		ch = RxQ_dequeue();
		if (ch != 0xFF) {
			key = get_key(ch); // 입력 숫자 값 32비트로 변환
			if (cal == '=') {FNDVALUE = 0x00000000;} // 재시작 할 때 FND 출력 값 초기화
			FNDVALUE = (FNDVALUE << 4) + key; // 입력 숫자 값 FND 출력
			switch(state) {
				case STATE_OP1:
					if((ch == '+') | (ch == '-') | (ch == '*') | (ch == '/')) { // 연산자 입력 시
						UART_TxByte(ch);
						if(ch == '+'){cal='+';} // 연산자 저장
						else if(ch == '-'){cal='-';}
						else if(ch == '*'){cal='*';}
						else if(ch == '/'){cal='/';}
						buffer[bufptr++] = 0;
						value = atoi(buffer); // 첫 입력 받은 값을 10진수로 변환하여 저장
						FNDVALUE = 0x00000000;
						bufptr = 0; // 버퍼 초기화
						state = STATE_OP2; // 상태 변경
						break;
					}
					else if(ch == 27) { // ESC
						buffer_init(); bufptr = 0; // 버퍼 초기화
						UART_TxData("\r\n");
						FNDVALUE = 0x00000000;
						break;
					}
					else { // 숫자 입력받기
						UART_TxByte(ch);
						buffer[bufptr++] = ch;
						cal = NULL; // 재시작 시 연산자 저장 값 초기화
						break;
					}

				case STATE_OP2:
					if((ch == '+') | (ch == '-') | (ch == '*') | (ch == '/')) { // 연산자 입력 시
						UART_TxByte(ch);
						buffer[bufptr++] = 0;
						value2 = atoi(buffer); // 입력 받은 숫자를 10진수로 변환하여 저장
						bufptr = 0; // 버퍼 초기화
						FNDVALUE = 0x00000000;

						if(cal == '+'){calculator(cal);} // 이전 연산자 계산
						else if(cal == '-'){calculator(cal);}
						else if(cal == '*'){calculator(cal);}
						else if(cal == '/'){calculator(cal);}
						
						if(ch == '+'){cal='+';} // 새 연산자 저장
						else if(ch == '-'){cal='-';}
						else if(ch == '*'){cal='*';}
						else if(ch == '/'){cal='/';}
						break;
					}
					else if (ch == '=') {
						UART_TxByte(ch);
						buffer[bufptr++] = 0;
						value2 = atoi(buffer); // 입력 받은 숫자를 10진수로 변환하여 저장
						bufptr = 0;
						
						if(cal == '+'){calculator(cal);} // 연산자 계산
						else if(cal == '-'){calculator(cal);}
						else if(cal == '*'){calculator(cal);}
						else if(cal == '/'){calculator(cal);}
						buffer_init(); // 버퍼 초기화
						sprintf(buffer, "%d", value); // 연산 결과 값을 buffer에 한 숫자씩 넣음
						UART_TxData(buffer);	// 결과 값 출력
						FNDVALUE = StringToHexa(buffer); // 결과 값을 16진수로 변환하여
						UART_TxData("\r\n");             //  FND 출력
						state = STATE_OP1; // 상태 변경
						cal = '='; // 연산 재시작 표시(FND 출력 초기화 위해)
						break;
					}
					else if (ch == 27) { // ESC
						buffer_init(); bufptr = 0; // 버퍼 초기화
						UART_TxData("\r\n");
						FNDVALUE = 0x00000000;
						state = STATE_OP1;
						break;
					}
					else { // 두 번째 값 입력받기
						UART_TxByte(ch);
						buffer[bufptr++] = ch;
						break;
					}
		}
		}
	}
}


void initialize() // 초기화
{
	init_UART();
	init_timer1();
}

void calculator(char cal) // 연산자 계산
{
	if(cal == '+'){value = value + value2;}
	else if(cal == '-'){value = value - value2;}
	else if(cal == '*'){value = value * value2;}
	else if(cal == '/'){value = value / value2;}
}

void buffer_init() // buffer에 저장된 값 초기화
{
	int i;
	for(i=0;i<81;i++) {
		buffer[i] = NULL;
	}
}

int StringToHexa(const char *parm_string)
// 16진수형태로 입력된 문자열 형식의 숫자값을 정수화하는 함수
{ 
    int count = strlen(parm_string), i = 0; 
    int hexa = 0; 
    for(i = 0; i < count; i++){ 
        if(*parm_string >= '0' && *parm_string <= '9') hexa = hexa*16 + *parm_string - '0'; 
        else if(*parm_string >= 'A' && *parm_string <= 'F') hexa = hexa*16 + *parm_string - 'A' + 10; 
        else if(*parm_string >= 'a' && *parm_string <= 'f') hexa = hexa*16 + *parm_string - 'a' + 10; 
        parm_string++; 
    } 
    return hexa; 
}