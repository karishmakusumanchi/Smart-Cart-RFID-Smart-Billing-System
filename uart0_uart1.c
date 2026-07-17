#include<Lpc21xx.h>
#include"types.h"
#include"uart_defines.h"
#include"project_defines.h"
#include"interrupt.h"
#include"uart0_uart1.h"

void Init_UART0(void)
{
	//cfg p0.0&p0.1 as TXD0&RXD0
	PINSEL0&=~(15<<0);
	//PINSEL0&=~(0xffffffff<<0);
	//update pin selection
	PINSEL0|=TxD0_PIN_FUNC|RxD0_PIN_FUNC;
	//cfg UXLCR for
	U0LCR|=(1<<DLAB_BIT)|WOR_LEN_SEL_;
	//cfg speed/freq/baudrate
	U0DLM=DIVISOR>>8;
	U0DLL=DIVISOR;
	//Reset DLAB bit to start comm
	U0LCR&=~(1<<DLAB_BIT);
	U0IER=(1<<0);
	VICIntEnable|=1<<UART0_VIC_CHNO;
	VICVectCntl1=1<<5|UART0_VIC_CHNO;
	VICVectAddr1=(u32)uart0_isr;
}

void U0_TxByte(u8 byte)
{
	//place byte to transmit in to UxTHR
	U0THR=byte;
	//monitor until transmission to complete
	while(((U0LSR>>TEMT_BIT)&1)==0);
}

void U0_Txstring(char *p)
{
	while(*p)
	{
		U0_TxByte(*p++);
	}
}

void Init_UART1(void)
{
	//cfg p0.0&p0.1 as TXD0&RXD0
	PINSEL0&=~(15<<16);
	//update pin selection
	PINSEL0|=TxD1_PIN_FUNC|RxD1_PIN_FUNC;
	//cfg UXLCR for
	U1LCR|=(1<<DLAB_BIT)|WOR_LEN_SEL_;
	//cfg speed/freq/baudrate
	U1DLM=DIVISOR>>8;
	U1DLL=DIVISOR;
	//Reset DLAB bit to start comm
	U1LCR&=~(1<<DLAB_BIT);
	U1IER=(1<<0);
	VICIntEnable|=1<<UART1_VIC_CHNO;
	VICVectCntl0=1<<5|UART1_VIC_CHNO;
	VICVectAddr0=(u32)uart1_isr;
}

void U1_TxByte(u8 byte)
{
	//place byte to transmit in to UxTHR
	U1THR=byte;
	//monitor until transmission to complete
	while(((U1LSR>>TEMT_BIT)&1)==0);
}

void U1_Txstring(char *p)
{
	while(*p)
	{
		U0_TxByte(*p++);
	}
}

