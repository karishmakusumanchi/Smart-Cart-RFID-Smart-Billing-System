#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include<unistd.h>
#include "uart.h"
#include<errno.h>
#include<stdio_ext.h>
#include<time.h>
//#define TEST_MODE

char manager_card[10]="12608170";
struct cart
{
	char name[50];
	char card[10];
	int qty;
	int price;
	int count;
};
struct cart *cart_items=NULL;
struct bank
{
	char card[10];
	int pin;
	int bal;
}bank_details;
int cart_count=0;
int total;
int fd;
FILE *fp;
void uart_send(char *p)
{
	printf("send\n");
	while(*p)
	{
		serialPutchar (fd,*p);
		++p;
	}
}
int checkint(char *p)
{
	while(*p)
	{
		if(!(*p>='0'&&*p<='9'))
			return 1;
		++p;
	}
	return 0;
}
void *openfile(char *filename,char *mode,char *file_buff)
{
	fp=fopen(filename,mode);
	if(fp==NULL)
	{
		perror("fopen");
		//return 0;
		exit(0);
	}
	fseek(fp,0,2);
	int size=ftell(fp)+1;
	rewind(fp);
	file_buff=calloc(size,1);
	fread(file_buff,size-1,1,fp);
	rewind(fp);
	file_buff[size-1]='\0';
	return file_buff;
}
void manager_fun()
{
	char item_name[50],item_card[10];
	char *file_buff=NULL;
	char *p=NULL,*q=NULL,*k=NULL;
	int size,total_len,l1,l2,posn,i;
	int qty,price;
	char c;
	int flag=0;
	while(1)
	{
		__fpurge(stdin);
		puts("------MENU------");
		printf("1.Entry\n2.Update price\n3.Update Quantity\n4.Back\n");
		printf("select choice:");
		scanf("%c",&c);
		switch(c)
		{
			case '1':fp=fopen("stock.csv","a");
				if(fp==NULL)
				{
					perror("fopen\n");
					//return 0;
					exit(0);
				}
				__fpurge(stdin);
				printf("enter item name:");
				fgets(item_name,50,stdin);
				item_name[strlen(item_name)-1]='\0';
				card1:printf("Enter the Card Number:");
				__fpurge(stdin);
				fgets(item_card,10,stdin);
				item_card[strlen(item_card)-1]='\0';
				if((strlen(item_card)!=8)||checkint(item_card))
				{
					printf("Enter 8 digits Card number\n");
					goto card1;
				}
				qty1:printf("Enter the Quantity:");
				__fpurge(stdin);
				scanf("%d",&qty);
				if(qty<0)
				{
					printf("Enter Quantity Again\n");
					goto qty1;
				}
				price1:printf("Enter the item price:");
				__fpurge(stdin);
				scanf("%d",&price);
				if(price<0)
				{
					printf("Enter Price Again\n");
					goto price1;
				}
				fprintf(fp,"%s,%s,%d,%d\n",item_name,item_card,qty,price);
				fclose(fp);
				break;
			case '2':
				file_buff=openfile("stock.csv","r+",file_buff);
				//char card_buf[10];
				__fpurge(stdin);
				card2:printf("Enter card Number to Update price\n");
				fgets(item_card,10,stdin);
				item_card[strlen(item_card)-1]='\0';
				if((strlen(item_card)!=8)||checkint(item_card))
				{
					printf("Enter 8 digits Card number\n");
					goto card2;
				}
				p=strstr(file_buff,item_card);
				if(p==NULL)
				{
					printf("Item not found\n");
					exit(0);
					//return;
				}
				p=p+9;
				q=p;
				while(*q!=',')
				{
					++q;
				}
				++q;
				char price[11];
				__fpurge(stdin);
				price2:printf("Enter New Price:");
				fgets(price,11,stdin);
				price[strlen(price)-1]='\0';
				if((strlen(price)==0)||checkint(price))
				{
					printf("Enter price Again\n");
					goto price2;
				}
				total_len=0;l1=0;l2=0;
				k=q;
				while(*k!='\n')
				{
					++l1;
					++k;
				}
				l2=strlen(price);
				total_len=strlen(file_buff);
				posn=q-file_buff;
				if(l2>l1)
				{
					file_buff=realloc(file_buff,total_len+(l2-l1)+1);
					k=file_buff+posn;
					memmove(k+(l2-l1),k,strlen(k)+1);
				}
				else if(l2<l1)
				{
					k=file_buff+posn;
					memmove(k,k+(l1-l2),strlen(k+(l1-l2))+1);
					file_buff=realloc(file_buff,total_len-(l1-l2)+1);
				}
				q=file_buff+posn;
				i=0;
				while(price[i]!='\0')
				{
					*q=price[i++];
					++q;
				}
				fprintf(fp,"%s",file_buff);
				fclose(fp);
				//printf("%s\n",file_buff);
				break;
			case '3':
				file_buff=openfile("stock.csv","r+",file_buff);
                                //char card_buf[10];
                                __fpurge(stdin);
				card3:printf("Enter card Number to Update Quantity\n");
                                fgets(item_card,10,stdin);
                                item_card[strlen(item_card)-1]='\0';
				if((strlen(item_card)!=8)||checkint(item_card))
				{
					printf("Enter 8 digits Card number\n");
					goto card3;
				}
                                p=strstr(file_buff,item_card);
                                if(p==NULL)
                                {
                                        printf("Item not found\n");
                                        exit(0);
                                        //return;
                                }
                                p=p+9;
                                //char *q=p;
                                char qty[11];
                                __fpurge(stdin);
				qty2:printf("Enter New Quantity:");
                                fgets(qty,11,stdin);
                                qty[strlen(qty)-1]='\0';
				if((strlen(qty)==0)||checkint(qty))
				{
					printf("Enter quantity Again\n");
					goto qty2;
				}
                                total_len,l1=0,l2=0;
                                q=p;
                                while(*q!=',')
                                {
                                        ++l1;
                                        ++q;
                                }
                                l2=strlen(qty);
                                total_len=strlen(file_buff);
                                posn=p-file_buff;
                                if(l2>l1)
                                {
                                        file_buff=realloc(file_buff,total_len+(l2-l1)+1);
                                        q=file_buff+posn;
                                        memmove(q+(l2-l1),q,strlen(q)+1);
                                }
                                else if(l2<l1)
                                {
                                        q=file_buff+posn;
                                        memmove(q,q+(l1-l2),strlen(q+(l1-l2))+1);
                                        file_buff=realloc(file_buff,total_len-(l1-l2)+1);
                                }
                                q=file_buff+posn;
                                i=0;
                                while(qty[i]!='\0')
                                {
                                        *q=qty[i++];
                                        ++q;
                                }
                                fprintf(fp,"%s",file_buff);
                                fclose(fp);
                                //printf("%s\n",file_buff);
                                break;
			case '4':return;
		}
	}
}
void item_card(char *card)
{
	char line[100],qty_buf[12],tx_buf[80];
	char *p=NULL;
	int flag=0,qty_len=0,posn;
	char *file_buff=NULL;
	file_buff=openfile("stock.csv","r+",file_buff);
	rewind(fp);
	while(fgets(line,sizeof(line),fp))
	{
		if(strstr(line,card))
		{
			for(int i=0;i<cart_count;i++)
			{
				if(strcmp(cart_items[i].card,card)==0)
				{
				sscanf(line,"%[^,],%[^,],%d,%d",cart_items[i].name,cart_items[i].card,&cart_items[i].qty,&cart_items[i].price);
				qty_len=snprintf(NULL,0,"%d",cart_items[i].qty);
				--cart_items[i].qty;
				sprintf(qty_buf,"%d",cart_items[i].qty);
				++cart_items[i].count;
				sprintf(tx_buf,"#%s,%d,%d$",cart_items[i].name,cart_items[i].price,cart_items[i].qty);
				flag=1;
				break;
				}
			}
		if(flag==0)
		{
			++cart_count;
			cart_items=realloc(cart_items,cart_count*sizeof(struct cart));
			sscanf(line,"%[^,],%[^,],%d,%d",cart_items[cart_count-1].name,cart_items[cart_count-1].card,&cart_items[cart_count-1].qty,&cart_items[cart_count-1].price);
			qty_len=snprintf(NULL,0,"%d",cart_items[cart_count-1].qty);
			--cart_items[cart_count-1].qty;
			sprintf(qty_buf,"%d",cart_items[cart_count-1].qty);
			cart_items[cart_count-1].count=1;
			sprintf(tx_buf,"#%s,%d,%d$",cart_items[cart_count-1].name,cart_items[cart_count-1].price,cart_items[cart_count-1].qty);
		}
		}
	}
	p=strstr(file_buff,card);
	if(p==NULL)
	{
		printf("OOPs!Item not found\n\n");
		//exit(0);
		return;
	}
	p=p+9;
	if(strlen(qty_buf)==qty_len)
	{
		strncpy(p,qty_buf,strlen(qty_buf));
		//memmove(p,p+l,strlen(p+l)+1);
	}
	else
	{
		memmove(p,p+1,strlen(p+1)+1);
		posn=p-file_buff;
		file_buff=realloc(file_buff,(strlen(file_buff))+1);
		//strncpy(file_buff+posn,qty_buf,strlen(qty_buf));
		memcpy(file_buff+posn,qty_buf,strlen(qty_buf));
	}
	rewind(fp);
	fprintf(fp,"%s",file_buff);
	rewind(fp);
	uart_send(tx_buf);
	fclose(fp);
	puts("\n------CART ITEMS------\n");
	for(int i=0;i<cart_count;i++)
	{
		printf("Item name:%s Price:%d Remaining QTY:%d\n",cart_items[i].name,cart_items[i].price,cart_items[i].qty);
	}
	puts("\n");
	//printf("%s\n",file_buff);
}
void delete_item(char *card)
{
	char *itemname=NULL,price_buf[12],qty_buf[12],tx_buf[100];
	int flag=0,qty_len;
	for(int i=0;i<cart_count;i++)
	{
		if(strcmp(cart_items[i].card,card)==0)
		{
			--cart_items[i].count;
			qty_len=snprintf(NULL,0,"%d",cart_items[i].qty);
			++cart_items[i].qty;
			sprintf(qty_buf,"%d",cart_items[i].qty);
			sprintf(tx_buf,"#%s,%d,%d$",cart_items[i].name,cart_items[i].price,cart_items[i].qty);
			if(cart_items[i].count==0)
			{
				memmove(&cart_items[i],&cart_items[i+1],(cart_count-i-1)*sizeof(struct cart));
				--cart_count;
			}
			flag=1;
			break;
		}
	}
	if(flag)
	{
		puts("\n------CART ITEMS------\n");
		for(int i=0;i<cart_count;i++)
		{
		printf("Item name:%s Price:%d Remaining QTY:%d\n",cart_items[i].name,cart_items[i].price,cart_items[i].qty);
		}
		puts("\n");

		int posn;
		char *file_buff=NULL,*p=NULL;
		file_buff=openfile("stock.csv","r+",file_buff);
		p=strstr(file_buff,card);
		if(p==NULL)
		{
			printf("Item not found\n");
			//exit(0);
			return;
		}
		p=p+9;
		if(strlen(qty_buf)==qty_len)
		{
			strncpy(p,qty_buf,strlen(qty_buf));
			//memmove(p,p+l,strlen(p+l)+1);
		}
		else
		{
			posn=p-file_buff;
			file_buff=realloc(file_buff,(strlen(file_buff)+1)+1);
			p=file_buff+posn;
			memmove(p+1,p,strlen(p)+1);
			//file_buff=realloc(file_buff,(strlen(file_buff)+1)+1);
			//strncpy(file_buff+posn,qty_buf,strlen(qty_buf));
			memcpy(file_buff+posn,qty_buf,strlen(qty_buf));
		}
		fprintf(fp,"%s",file_buff);
		fclose(fp);
		//printf("%s\n",file_buff);	
		uart_send(tx_buf);
	}
	else
	{
		printf("NO such item found\n");
		uart_send("#NO item$");
	}
}
void verify_Bankcard(char *cardbuff)
{
	fp=fopen("bank.csv","r");
	char line[100];	
	int card_present=0;
	while(fgets(line,sizeof(line),fp))
	{
		if(strstr(line,cardbuff))
		{
			sscanf(line,"%[^,],%d,%d",bank_details.card,&bank_details.pin,&bank_details.bal);
			card_present=1;
			break;
		}
	}
	fclose(fp);
	if(card_present==1)
	{
		uart_send("#FOUND$");
		puts("CARD FOUND");
	}
	else 
	{
		uart_send("#NOT FOUND$");
		puts("NOT CARD FOUND");
	}
	//printf("card:%s pin:%d bal:%d\n",bank_details.card,bank_details.pin,bank_details.bal);
}
void verify_pin(int pin,int amt)
{
	char line[100];
	//static int cnt;
	fp=fopen("bank.csv","r");
	FILE *temp_fp=fopen("temp.csv","w");
	while(fgets(line,sizeof(line),fp))
	{
		if(strstr(line,bank_details.card))
		{
			if(pin==bank_details.pin)
			{
				if(bank_details.bal>=amt)
				{
					bank_details.bal-=amt;
					uart_send("#SUCCESS$");
					puts("PAYMENT SUCCESSFULL");
				}
				else {
					puts("INSUFFICIENT FUNDS IN ACCOUNT");
					uart_send("#LOWBAL$");
				}
			}
			else{
				uart_send("#WRONGPIN$");
				puts("WRONG PIN!!");
			}
			fprintf(temp_fp,"%s,%d,%d\n",bank_details.card,bank_details.pin,bank_details.bal);
		}
		else
			fputs(line,temp_fp);
	}
	fclose(fp);
	fclose(temp_fp);
	remove("bank.csv");
	rename("temp.csv","bank.csv");
}
void transaction_result(char *msg)
{
	int flag;
	char line[100];
	FILE *t_fp=fopen("temp.csv","w");
	if(msg[0]=='S')
	{
		time_t t;
		struct tm *tm_info;
		time(&t);
		tm_info=localtime(&t);
		int amt=atoi(msg+1);
		total+=amt;
		char datetime[30];
		printf("Total amt:%s\n",msg+1);
		strftime(datetime,sizeof(datetime),"%d-%m-%y,%H:%M:%S",tm_info);
		fp=fopen("income.csv","a");
		fprintf(fp,"%s,%d\n",datetime,amt);
		fclose(fp);
		free(cart_items);
		cart_items=NULL;
		 cart_count=0;
	}
	else if(msg[0]=='C')
	{
		printf("In cancel\n");
		fp=fopen("stock.csv","r");
		 while(fgets(line,sizeof(line),fp))
		 {
			 flag=1;
			for(int i=0;i<cart_count;i++)
			{
				if(strstr(line,cart_items[i].card))
				{
					fprintf(t_fp,"%s,%s,%d,%d\n",cart_items[i].name,cart_items[i].card,(cart_items[i].qty+cart_items[i].count),cart_items[i].price);
					flag=0;
					break;
				}
			}
			if(flag)
				fputs(line,t_fp);
		 }
		 free(cart_items);
		 cart_items=NULL;
		 cart_count=0;
		 fclose(fp);
		 fclose(t_fp);
		 remove("stock.csv");
		 rename("temp.csv","stock.csv");
		 puts("PURCHASE CANCELED");
	}
}
int uart_receive(char *buff)
{
	char rx;
	int i=0;
	if((fd = serialOpen ("/dev/ttyUSB0",9600)) < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
                return 1 ;
	}
	//puts("serial port is opened\n");
	while(1)
	{
		rx=serialGetchar(fd);
		if(rx=='\0')
		{
			buff[i]='\0';
			break;
		}
		else
		{
			buff[i++]=rx;
		}
	}
}
void get_data(char *buff)
{
#ifdef TEST_MODE
	printf("Test Mode\nEnter Card Number Manually:");
	__fpurge(stdin);
	fgets(buff,30,stdin);
	buff[strlen(buff)-1]='\0';
#else
	uart_receive(buff);
#endif
}
int main()
{
	char buff[30],temp[12];
	int i;
	//uart_receive(buff);
	while(1)
	{
		puts("Ready To Recieve...");
		get_data(buff);
		//printf("%s\n",buff);
		if(strcmp(buff,manager_card)==0)
		{
			//printf("manager card\n");
			manager_fun();
		}
		else if(buff[0]=='D'&&buff[9]=='$')
		{
			//printf("Delete item\n");
			buff[strlen(buff)-1]='\0';
			//printf("%s\n",buff+1);
			delete_item(buff+1);
		}
		else if(buff[0]=='B')
		{
			char *p=buff;
			//printf("Verify card\n");
			buff[strlen(buff)-1]='\0';
			while(*p!='A')
			{
				++p;
			}
			*p='\0';
			//printf("%s\n%s\n",buff+1,p+1);
			verify_Bankcard(buff+1);
		}
		else if(buff[0]=='P')
		{
			char *p=buff;
			buff[strlen(buff)-1]='\0';
			while(*p!='A')
			{
				++p;
			}
			*p='\0';
			verify_pin(atoi(buff+1),atoi(p+1));
			//printf("%s\n",buff+1);
		}
		else if(buff[0]=='S'||buff[0]=='C')
		{	
			buff[strlen(buff)-1]='\0';
			transaction_result(buff);
		}
		else if(strcmp(buff,"exit")==0)
		{
			fp=fopen("income.csv","a");
			fprintf(fp,"%s,,%d\n","TOTAL",total);
			fclose(fp);
			exit(0);
		}
		else 
		{	
			printf("item card\n");
			item_card(buff);
		}
	}
}
