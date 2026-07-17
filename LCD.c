#include <lpc21xx.h>
#include "LCD_defines.h"
#include "defines.h"
#include "delay.h"
#include "types.h"
void writeLCD(u8 data)
{
	SCLRBIT(IOCLR0,RW);//RW=0 write operation
	WBYTE(IOPIN1,LCD_DATA,data);
	SSETBIT(IOSET0,EN);//EN=1
	delay_us(1);
	SCLRBIT(IOCLR0,EN);//EN=0
	delay_ms(2);//internal process 
}
void cmdLCD(u8 cmd)
{
	SCLRBIT(IOCLR0,RS);//RS=0,command reg selected
	writeLCD(cmd);
}
void charLCD(u8 ascii)
{
	SSETBIT(IOSET0,RS);//RS=1,data reg selected
	writeLCD(ascii);
}
void InitLCD()
{
	WBYTE(IODIR1,LCD_DATA,255);
	SETBIT(IODIR0,RS);//p0.16 as output
	SETBIT(IODIR0,RW);//p0.17 as output
	SETBIT(IODIR0,EN);//p0.18 as output
	
	delay_ms(15);
	cmdLCD(MODE_8BIT_1LINE);
	delay_ms(5);
	cmdLCD(MODE_8BIT_1LINE);
	delay_us(100);
	cmdLCD(MODE_8BIT_1LINE);
	cmdLCD(MODE_8BIT_2LINE);
	cmdLCD(DISP_ON_CUR_BLINK);
	cmdLCD(0x01);
	//cmdLCD(0x10);
	//cmdLCD(0x06);
}
void strLCD(s8 *p)
{
	while(*p)
	{
		charLCD(*p);
		p++;
	}
}
void U32LCD(u32 n)
{
	u8 a[10];
	s32 i=0;
	if(n==0)
	{
		charLCD('0');
	}
	else 
	{
		while(n)
		{
			a[i++]=(n%10)+48;
			n/=10;
		}
		for(--i;i>=0;i--)
		charLCD(a[i]);
	}
}
void s32LCD(s32 n)
{
	if(n<0)
	{
		charLCD('-');
		n=-n;
	}	
		U32LCD(n);

}
void f32LCD(f32 fnum,u8 nDP)
{
	u32 n;
	s32 i;
	if(fnum<0)
	{
		charLCD('-');
	}
	n=fnum;
	U32LCD(n);
	charLCD('.');

	for(i=0;i<nDP;i++)
	{
		fnum=(fnum-n)*10;
		n=fnum;
		charLCD(n+48);
	}
}
void HEXLCD(u32 n)
{
	u8 a[8],rem;
	s32 i=0;
	if(n==0)
	{
		charLCD('0');
	}
	else
	{
		while(n)
		{
			rem=n%16;
			(rem<10)?(rem+=48):(rem+=55);
			a[i++]=rem;
			n/=16;
		}
		for(--i;i>=0;i--)
		{
			charLCD(a[i]);
		}
	}
}
void BinLCD(u32 n,u8 nbd)
{
	s32 i;
	for(i=(nbd-1);i>=0;i--)
	{
		charLCD(((n>>i)&1)+48);
	}
}
void BuildCGRAM(u8* p,u8 nb)
{
	s32 i;
	cmdLCD(GOTO_CGRAM);
	for(i=0;i<=nb;i++)
	{
		charLCD(p[i]);
	}
	cmdLCD(GOTO_LINE1_POS0);
}

