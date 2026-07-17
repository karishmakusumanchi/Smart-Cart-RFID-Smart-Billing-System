#include<lpc21xx.h>
#include<string.h>
#include<stdio.h>
#include"types.h"
#include"delay.h"
#include"lcd.h"
#include"lcd_defines.h"
#include"kpm.h"
#include"uart0_uart1.h"
#include"major_proj.h"
s8 buff[30],rfid[30],rx_buff[50],msg[30],item_name[30];
s8 op;
s32 entry_flag,delete_flag,exit_flag,scan_flag,recv_flag,flag,i,retry,done;
s32 cost,qty,totalamt,cash,PIN,attempt_cnt;
void U0_send(char *buff)
{
	scan_flag=0;
	U0_Txstring(buff);
	U0_TxByte('\0');	
}
void U0_recv()
{
	recv_flag=0;
	sscanf(rx_buff,"%[^,],%d,%d",item_name,&cost,&qty);
}
void item_details()
{
	cmdLCD(CLEAR_LCD);
	strLCD(item_name);
	charLCD(' ');
	strLCD("RQTY:");
	U32LCD(qty);
	cmdLCD(GOTO_LINE2_POS0);
	strLCD("TAmt:");
	s32LCD(totalamt);	
}
void cash_payment()
{
	cmdLCD(CLEAR_LCD);
	strLCD("Enter amount:");
	cash=Readnum();
	if(cash==totalamt)
	{
		retry=0;
		done=1;
		sprintf(msg,"S%d$",totalamt);
		U0_send(msg);
		cmdLCD(CLEAR_LCD);
		strLCD("Payment Success");
		cmdLCD(GOTO_LINE2_POS0);
		strLCD("Visit Again");
		delay_s(2);
		/*cmdLCD(CLEAR_LCD);
		strLCD("PRESS Entry Key");
		cmdLCD(GOTO_LINE2_POS0);
		strLCD("To Begin Shopping");*/
		totalamt=0;
		delay_s(2);
	}
	else retry=1;
}
void card_payment()
{
	cmdLCD(CLEAR_LCD);
	strLCD("place ATM card");
	scan_flag=0;
	while(scan_flag==0);
	snprintf(rfid,sizeof(rfid),"B%sA%d$",buff,totalamt);
	U0_send(rfid);
	while(recv_flag==0); 
	recv_flag=0;
	if(strcmp(rx_buff,"NOT FOUND")==0)
	{
		retry=1;
		cmdLCD(CLEAR_LCD);
		strLCD("CARD Not Found");
		delay_s(2);
	}
	else
	{
		attempt_cnt=3;
		while(1)
		{
			cmdLCD(CLEAR_LCD);
			strLCD("Enter ATM PIN");
			cmdLCD(GOTO_LINE2_POS0);
			PIN=ReadPIN();
			snprintf(msg,sizeof(msg),"P%dA%d$",PIN,totalamt);
			U0_send(msg);
			while(recv_flag==0); 
			recv_flag=0;
			U0_recv();
								 
			if(strcmp(rx_buff,"SUCCESS")==0)	
			{
				retry=0;
				done=1;
				delay_ms(500);
				sprintf(msg,"S%d$",totalamt);
				U0_send(msg);
				cmdLCD(CLEAR_LCD);
				strLCD("payment success");
				cmdLCD(GOTO_LINE2_POS0);	
				strLCD("Visit Again");
				delay_s(2);
				/*cmdLCD(CLEAR_LCD);
				strLCD("PRESS Entry Key");
				cmdLCD(GOTO_LINE2_POS0);
				strLCD("To Begin Shopping");*/
				totalamt=0;
				break;
			}
	 		else if(strcmp(rx_buff,"WRONGPIN")==0)
			{	
				--attempt_cnt;
				cmdLCD(CLEAR_LCD);
				strLCD("wrong PIN");
				cmdLCD(GOTO_LINE2_POS0);
				strLCD("YOu have");
				U32LCD(attempt_cnt);
				strLCD("chances");
				delay_s(2);
			}	
			else if(strcmp(rx_buff,"LOWBAL")==0)
			{
				retry=1;
				cmdLCD(CLEAR_LCD);
				strLCD("Low Balance");
				cmdLCD(GOTO_LINE2_POS0);
				strLCD("payment failed");
				delay_s(2);
				cmdLCD(CLEAR_LCD);
				strLCD("Try Another Way");
				delay_s(2);
				break;
		}	 
			if(attempt_cnt==0)
			{
				retry=1;
				cmdLCD(CLEAR_LCD);
				strLCD("Lost chances");
				cmdLCD(GOTO_LINE2_POS0);
				strLCD("payment failed");
				delay_s(2);
				break;
			}
		}		
	}					
}
int main()
{
	init();
	strLCD("smart cart RFID");
	delay_s(3);
	cmdLCD(CLEAR_LCD);
	strLCD("PRESS Entry Key");
	cmdLCD(GOTO_LINE2_POS0);
	strLCD("& Begin Shopping");
	while(1)
	{
		/*cmdLCD(CLEAR_LCD);
		strLCD("PRESS Entry Key");
		cmdLCD(GOTO_LINE2_POS0);
		strLCD("To Begin Shopping");*/
		if(entry_flag)
		{
			entry_flag=0;
			//buff={0};
			memset(buff,0,sizeof(buff));
			scan_flag=0;
			recv_flag=0;
			delete_flag=0;
			exit_flag=0;
			cmdLCD(CLEAR_LCD);
			strLCD("Hello,Place card");
			cmdLCD(GOTO_LINE2_POS0);
			strLCD("to Add Items");
			//strLCD("waiting...");
			while(1)
			{				
				if(scan_flag)
				{
					U0_send(buff);
					IOSET0=1<<2;
					delay_ms(1000);
					IOCLR0=1<<2;
					cmdLCD(CLEAR_LCD);
					strLCD("card scanned");
					cmdLCD(GOTO_LINE2_POS0);
					strLCD(buff);
				}
			
				if(recv_flag)
				{
					U0_recv();
					totalamt+=cost;
					item_details();
				}
				if(delete_flag)
				{
					delete_flag=0;
					if(totalamt==0)
					{
						cmdLCD(CLEAR_LCD);
						strLCD("No items to");
						cmdLCD(GOTO_LINE2_POS0);
						strLCD("delete Add items");
						delay_s(2);
						strLCD("Place card and");
						cmdLCD(GOTO_LINE2_POS0);
						strLCD("Add items");
					}
					else
					{
						cmdLCD(CLEAR_LCD);
						strLCD("Place card to");
						cmdLCD(GOTO_LINE2_POS0);
						strLCD("delete");
						while(1)
						{	
							if(scan_flag)
							{
								//scan_flag=0;
								snprintf(rfid,sizeof(rfid),"D%s$",buff);
								//U0_Txstring(rfid);
								//U0_TxByte('\0');
								U0_send(rfid);
								while(recv_flag==0);
								if(strcmp(rx_buff,"NO item")==0)
								{
									recv_flag=0;
									cmdLCD(CLEAR_LCD);
									strLCD("No such item to");
									cmdLCD(GOTO_LINE2_POS0);
									strLCD("delete Add item");
									delay_s(2);
									if(totalamt==0)
									{	
									cmdLCD(CLEAR_LCD);
									strLCD("Press Entry key");
									cmdLCD(GOTO_LINE2_POS0);
									strLCD("and add items");
									}
									else
									{
										cmdLCD(CLEAR_LCD);
										strLCD("TAmt:");
										s32LCD(totalamt);
										cmdLCD(GOTO_LINE2_POS0);
									}
								}
								else
								{
									U0_recv();
									totalamt-=cost;	
									cmdLCD(CLEAR_LCD);
									strLCD("one ");
									strLCD(item_name);
									cmdLCD(GOTO_LINE2_POS0);
									strLCD(" item deleted");
									delay_s(2);
									item_details();
								}
							}
							else if(entry_flag==1)
							{
								entry_flag=0;
								cmdLCD(CLEAR_LCD);
								if(totalamt>0)
								{
								strLCD("totalamt:");
								s32LCD(totalamt);
								cmdLCD(GOTO_LINE2_POS0);
								strLCD("Add MOre items");
								
								}
								else
								{
								strLCD("Place card and");
								cmdLCD(GOTO_LINE2_POS0);
								strLCD("Add items");
								}
								break;
							}
						}
					}
				}
				if(exit_flag)
				{
					exit_flag=0;
					while(1)
					{
						Again:cmdLCD(CLEAR_LCD);
						strLCD("Select payment");
						cmdLCD(GOTO_LINE2_POS0);
						strLCD("     Method");
						delay_s(2);
						if(retry==0)
						{
							cmdLCD(CLEAR_LCD);
							strLCD("1.Cash  2.Card");
							op=keyscan();
							switch(op)
							{
								case '1':cmdLCD(CLEAR_LCD);
										cash_payment();
										break;
								case '2':card_payment(); 
										break;
								default:cmdLCD(CLEAR_LCD);
										strLCD("Select Valid");
										cmdLCD(GOTO_LINE2_POS0);
										strLCD("     Option");
										delay_s(2);
										goto Again;
							}
							if(retry==0)
							{
								cmdLCD(CLEAR_LCD);
								strLCD("PRESS Entry Key");
								cmdLCD(GOTO_LINE2_POS0);
								strLCD("To Begin Shopping");
								break;
							}
						}
						else 
						{
							cmdLCD(CLEAR_LCD);
							strLCD("1.Cash  2.Card");
							cmdLCD(GOTO_LINE2_POS0);
							strLCD("3.Cancel");
							op=keyscan();
							switch(op)
							{
								case '1':cmdLCD(CLEAR_LCD);
										cash_payment();
										 break;
								case '2':card_payment(); 
										 break;
								case '3':U0_send("C$");
										 retry=0;
										 done=1;
										 totalamt=0;
										 cmdLCD(CLEAR_LCD);
										 strLCD("CANCELED BUYING!");
										 cmdLCD(GOTO_LINE2_POS0);
										 delay_s(2);
										 break;
								default:cmdLCD(CLEAR_LCD);
										strLCD("Select Valid");
										cmdLCD(GOTO_LINE2_POS0);
										strLCD("     Option");
										delay_s(2);
										goto Again;
							}
							if(retry==0)
							{
								cmdLCD(CLEAR_LCD);
								strLCD("PRESS Entry Key");
								cmdLCD(GOTO_LINE2_POS0);
								strLCD("To Begin Shopping");
								break;
							}
						}
					}
				}
				if(done==1)
				{
					done=0;
					break;
				}
			}
		}
		//cmdLCD(CLEAR_LCD);
		//strLCD("smart cart RFID");
		//delay_ms(2000);
	}
}
