#include<lpc21xx.h>
#include"lcd.h"
#include"kpm.h"
#include"project_defines.h"
#include"interrupt.h"
#include"types.h"
#include"uart0_uart1.h"
void init()
{
		IODIR0=1<<2;
        InitLCD();
        Initkpm();
		Init_UART0();
		Init_UART1();
      //  PINSEL0&=(u32)~3<<6|(u32)~3<<14;
        PINSEL1&=(u32)~3<<0;
        PINSEL0|=EINT1_IP_PIN|EINT2_IP_PIN;
        PINSEL1|=EINT0_IP_PIN;
        VICIntSelect=0;
        VICIntEnable|=1<<EINT0_VIC_CHNO|1<<EINT1_VIC_CHNO|1<<EINT2_VIC_CHNO;
	VICVectCntl2=1<<5|EINT0_VIC_CHNO;
	VICVectAddr2=(u32)eint0_isr;
	VICVectCntl3=1<<5|EINT1_VIC_CHNO;
	VICVectAddr3=(u32)eint1_isr;
	VICVectCntl4=1<<5|EINT2_VIC_CHNO;
	VICVectAddr4=(u32)eint2_isr;
				
	EXTINT=0;
	EXTMODE=(1<<0)|(1<<1)|(1<<2);
	EXTPOLAR=0;
				
}

