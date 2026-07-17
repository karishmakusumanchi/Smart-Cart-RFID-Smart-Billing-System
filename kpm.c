#include "types.h"																	       
#include "defines.h"
#include <lpc21xx.h>
#include "kpm_defines.h"
#include "lcd.h"
#include"delay.h"

//u32 kpmLUT[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};

u8 kpmLUT[4][4]={{'1','2','3','A'},
				 {'4','5','6','B'},
				 {'7','8','9','C'},
				 {'*','0','#','D'}}; 
void Initkpm()
{
	WNIBBLE(IODIR1,ROW0,15);
}
u32 colscan()
{
	return (RNIBBLE(IOPIN1,COL0)<15?0:1);
}
u32 Rowcheck()
{
	u32 rno;
	for(rno=0;rno<4;rno++)
	{
		WNIBBLE(IOPIN1,ROW0,~(1<<rno));
		if(colscan()==0)
		{
			break;
		}
	}
		//make rows as default
		WNIBBLE(IOPIN1,ROW0,0X0);
		return rno;
}
u32 Colcheck()
{
	u32 cno;
	for(cno=0;cno<4;cno++)
	{
		if(RBIT(IOPIN1,(COL0+cno))==0)
			{
				break;
			}
	}
	return cno;
}
u8 keyscan()
{
	u8 keyv;
	u32 rno,cno;
	//waits for switch press
	while(colscan());
	//find the rno
	rno=Rowcheck();
	//find the cno
	cno=Colcheck();
	//get the key value using KPMLUT
	keyv=kpmLUT[rno][cno];
	//wait for switch released
	while(!colscan());
	return keyv;
}
u32 Readnum()
{
	u8 key;
	u32 sum=0;
	cmdLCD(0xc0);
	while(1)
	{
		key=keyscan();
		delay_ms(100);

		if(key>='0'&&key<='9')
		{
			charLCD(key);
			sum=(sum*10)+key-'0';
		}
		else if(key=='#')
		{
			break;
		}
		else if(key=='C')
		{
			/*cmdLCD(0xc0);
			strLCD("          ");
			cmdLCD(0xc0);
			sum=0;*/
			sum/=10;
			cmdLCD(0xc0);
			strLCD("          ");
			cmdLCD(0xc0);
			U32LCD(sum);
			if(sum==0)
			{
				cmdLCD(0xc0);
				strLCD("          ");
				cmdLCD(0xc0);
			}

		}
	}
	return sum;
}
u32 ReadPIN()
{
	u8 key;
	u32 sum=0;
	s32 i=0;
	cmdLCD(0xc0);
	while(1)
	{
		key=keyscan();
		delay_ms(100);

		if((key>='0'&&key<='9')&&i!=4)
		{
			charLCD('*');
			sum=(sum*10)+key-'0';
			++i;
		}
		else if((key=='C')&&i!=0)
		{
			sum/=10;
			--i;
			cmdLCD(0xc0+(i));
			charLCD(' ');
			cmdLCD(0xc0+(i));
		}
		else if((key=='#'))
		{
			return sum;
			//break;
		}
	}
//	return sum;
}
