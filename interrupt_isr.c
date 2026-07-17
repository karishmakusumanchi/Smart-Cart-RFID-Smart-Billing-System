#include<lpc21xx.h>
#include<string.h>
#include"types.h"
extern s8 buff[30],rx_buff[50];
extern s32 entry_flag,delete_flag,exit_flag,scan_flag,recv_flag;
s8 data;
s32 index;
void uart1_isr(void) __irq
{
	data=U1RBR;
	if(data==0x02)	
	{
		index=0;
		memset(buff,0,sizeof(buff));
	}
	else if(data==0x03)
	{
		buff[index]='\0';
		scan_flag=1;
	}
	else
	{
	 	buff[index++]=data;
	}
	VICVectAddr=0;
}
void uart0_isr(void) __irq
{
	data=U0RBR;
	if(data=='#')
	{
		index=0;
		memset(rx_buff,0,sizeof(rx_buff));
	}		
	else if(data=='$')
	{
		rx_buff[index]='\0';
		recv_flag=1;
	}
	else
	{
		rx_buff[index++]=data;
	}
	VICVectAddr=0;
}
void eint0_isr()__irq
{
	entry_flag=1;
	EXTINT=1<<0;
	VICVectAddr=0;
}
void eint1_isr()__irq
{	
	delete_flag=1;
	EXTINT=1<<1;
	VICVectAddr=0;
}
void eint2_isr()__irq
{
	exit_flag=1;
	EXTINT=1<<2;
	VICVectAddr=0;	
}
