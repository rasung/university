#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"

#define UARTX	(LPC_UART_TypeDef *)LPC_UART1
#define TIMEOUT 10000

#define RxReady (UART_GetLineStatus(UARTX) & UART_LSR_RDR)
#define TxEmpty (UART_GetLineStatus(UARTX) & UART_LSR_THRE)

#define RxQSIZE 0x400
#define TxQSIZE 0x400

unsigned char RxQueue[RxQSIZE];
unsigned char TxQueue[TxQSIZE];

uint32_t RxQ_head, RxQ_tail;
uint32_t TxQ_head, TxQ_tail;

PINSEL_CFG_Type pincfg1={2,0,2,0,0};
PINSEL_CFG_Type pincfg2={2,1,2,0,0};

void init_UART()
{
	UART_CFG_Type UARTcfg;
	UART_FIFO_CFG_Type FIFOcfg;
	
	PINSEL_ConfigPin(&pincfg1);
	PINSEL_ConfigPin(&pincfg2);
	
	UARTcfg.Baud_rate = 115200;
	UARTcfg.Parity = UART_PARITY_NONE;
	UARTcfg.Databits = UART_DATABIT_8;
	UARTcfg.Stopbits = UART_STOPBIT_1;
	UART_Init(UARTX, &UARTcfg);
	
	FIFOcfg.FIFO_ResetRxBuf = ENABLE;
	FIFOcfg.FIFO_ResetTxBuf = ENABLE;
	FIFOcfg.FIFO_DMAMode = DISABLE;
	FIFOcfg.FIFO_Level = UART_FIFO_TRGLEV0;
	UART_FIFOConfig(UARTX, &FIFOcfg);
	
	UART_TxCmd(UARTX, ENABLE);
	
	// Enable Receive Data Available interrupt
	UART_IntConfig(UARTX, UART_INTCFG_RBR, ENABLE);
	RxQ_head = RxQ_tail = 0;
	
	UART_IntConfig(UARTX, UART_INTCFG_THRE, ENABLE);
	TxQ_head = TxQ_tail = 0;
	
	NVIC_EnableIRQ(UART1_IRQn);
}

uint8_t UART_RxByte(void)
{
	uint8_t ch = 0xFF;
	int watchdog = 0;
	
	while(!RxReady)
	{
		watchdog += 1;
		if(watchdog == TIMEOUT)
			break;
	}
	
	if(watchdog != TIMEOUT)
		ch = UART_ReceiveByte(UARTX);
	
	return ch;
}


void TxQ_enqueue(unsigned char ch)
{
	if((TxQ_head+1) != TxQ_tail)
	{
		TxQueue[TxQ_head] = ch;
		TxQ_head = (TxQ_head + 1) % TxQSIZE;
	}
}

void UART_TxByte(uint8_t ch)
{
	if(TxQ_head != TxQ_tail)
	{
		TxQ_enqueue(ch);
	}
	else
	{
		if(TxEmpty)
			UART_SendByte(UARTX, ch);
		else
			TxQ_enqueue(ch);
	}
}

void UART_TxData(char *str)
{
	while(*str != 0){
		UART_TxByte(*str++);
	}
}

void UART1_IRQHandler(void)
{
	// read interrupt status and clear interrupt
	uint32_t inttype = UART_GetIntId(UARTX) & 0x0F;
	
	switch(inttype){
		case UART_IIR_INTID_RDA:
			RxQueue[RxQ_head] = UART_ReceiveByte(UARTX);
			RxQ_head = (RxQ_head + 1) % RxQSIZE;
			break;
		case UART_IIR_INTID_THRE:
			if(TxQ_head != TxQ_tail)
			{
				UART_SendByte(UARTX, TxQueue[TxQ_tail]);
				TxQ_tail = (TxQ_tail + 1 ) % TxQSIZE;
			}
			break;
		default :
			break;
	}
}

uint8_t RxQ_dequeue()
{
	uint8_t rxd;
	if(RxQ_head == RxQ_tail)
		rxd = 0xFF; // queue empty
	else {
		rxd = RxQueue[RxQ_tail];
		RxQ_tail = (RxQ_tail + 1) % RxQSIZE;
	}
	return rxd;
}