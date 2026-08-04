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
#include<signal.h>
//-#define TEST_MODE

char manager_card[10]="12345678";
struct cart
{
        char name[50];
        char card[10];
        int qty;
        int price;
        int count;
};
struct cart *cart_items=NULL;
int total_items=5;
struct bank
{
        char card[10];
        int pin;
        int bal;
}bank_details;
void display_bank(struct bank b);

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
        char item_name[50],
        item_card[10];
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
                                        return;
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
                                break;
                        case '3':
                                file_buff=openfile("stock.csv","r+",file_buff);
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
                                if("Item not found\n");
                                        exit(0);
                                }
                                p=p+9;
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
                                break;
                        case '4':return;
                }
        }
}
void item_card(char *card)
{
        char line[100],qty_buf[12],tx_buf[80];
        char *p=NULL;
        int flag=0,qty_len=0,posn,total_amt;
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
                                if(cart_items[i].qty==0)
                                {
                                        if(cart_items[i].count==0)
                                        {
                                                --cart_count;
                                                return;
                                        }
                                        printf("%s is out of stock\n",cart_items[i].name);
                                        goto cart;
                                        return;
                                }
                                qty_len=snprintf(NULL,0,"%d",cart_items[i].qty);
                                --cart_items[i].qty;
                                sprintf(qty_buf,"%d",cart_items[i].qty);
                                ++cart_items[i].count;
                                sprintf(tx_buf,"#%s,%d,%d$",cart_items[i].name,cart_items[i].price,cart_items[i].qty);
                                total+=cart_items[i].price;
                                flag=1;
                                break;
                                }
                        }
                if(flag==0)
                {
                        ++cart_count;
                        cart_items=realloc(cart_items,cart_count*sizeof(struct cart));
                        sscanf(line,"%[^,],%[^,],%d,%d",cart_items[cart_count-1].name,cart_items[cart_count-1].card,&cart_items[cart_count-1].qty,&cart_items[cart_count-1].price);
                        if(cart_items[cart_count-1].qty==0)
                            {
                                        printf("%s is out of stock\n",cart_items[cart_count-1].name);
                                        if(cart_items[cart_count-1].count==0)
                                        {
                                                --cart_count;
                                                return;
                                        }
                                        goto cart;
                                        return;
                            }
                        qty_len=snprintf(NULL,0,"%d",cart_items[cart_count-1].qty);
                        --cart_items[cart_count-1].qty;
                        sprintf(qty_buf,"%d",cart_items[cart_count-1].qty);
                        cart_items[cart_count-1].count=1;
                        sprintf(tx_buf,"#%s,%d,%d$",cart_items[cart_count-1].name,cart_items[cart_count-1].price,cart_items[cart_count-1].qty);
                                total+=cart_items[cart_count-1].price;

                }
                }
        }
        p=strstr(file_buff,card);
        if(p==NULL)
        {
                printf("OOPs!Item not found\n\n");
                return;
        }
        p=p+9;
        if(strlen(qty_buf)==qty_len)
        {
                strncpy(p,qty_buf,strlen(qty_buf));
        }
-- INSERT --                                                                          
