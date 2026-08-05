#include<stdio.h>        // Standard input/output functions
#include<stdlib.h>       // Memory allocation and exit()
#include<stdint.h>       // Standard integer types
#include<stdarg.h>       // Variable argument functions
#include<string.h>       // String handling functions
#include<unistd.h>       // POSIX functions
#include "uart.h"        // UART communication functions
#include<errno.h>        // Error handling
#include<stdio_ext.h>    // __fpurge() function
#include<time.h>         // Time functions
#include<signal.h>       // Signal handling

//#define TEST_MODE       // Uncomment to enable test mode

char manager_card[10]="12345678";   // Manager RFID card number

// Structure to store shopping cart details
struct cart
{
    char name[50];      // Item name
    char card[10];      // RFID card ID
    int qty;            // Available quantity
    int price;          // Item price
    int count;          // Quantity added into cart
};

struct cart *cart_items=NULL;   // Dynamic array for cart items
int total_items=5;              // Initial total items

// Structure to store bank account details
struct bank
{
    char card[10];      // Bank card number
    int pin;            // PIN number
    int bal;            // Account balance
}bank_details;

void display_bank(struct bank b);

int cart_count=0;       // Number of items in cart
int total=0;            // Total bill amount
int fd;                 // UART file descriptor
FILE *fp;               // File pointer

// Send data through UART
void uart_send(char *p)
{
    printf("send\n");

    while(*p)
    {
        serialPutchar(fd,*p);   // Send one character
        ++p;
    }
}

// Check whether string contains only digits
int checkint(char *p)
{
    while(*p)
    {
        if(!(*p>='0' && *p<='9'))
            return 1;           // Invalid input

        ++p;
    }

    return 0;                   // Valid numeric string
}

// Open file and load complete contents into memory
void *openfile(char *filename,char *mode,char *file_buff)
{
    fp=fopen(filename,mode);

    if(fp==NULL)
    {
        perror("fopen");
        exit(0);
    }

    fseek(fp,0,SEEK_END);       // Move to end of file
    int size=ftell(fp)+1;       // Get file size

    rewind(fp);                 // Move back to beginning

    file_buff=calloc(size,1);   // Allocate memory

    fread(file_buff,size-1,1,fp);   // Read file contents

    rewind(fp);

    file_buff[size-1]='\0';     // Null terminate string

    return file_buff;
}
// Manager menu for stock maintenance
void manager_fun()
{
    ...
    while(1)
    {
        // Display manager options
        puts("------MENU------");

        printf("1.Entry\n");
        printf("2.Update price\n");
        printf("3.Update Quantity\n");
        printf("4.Back\n");

        // Read manager choice
        scanf("%c",&c);

        switch(c)
        {
            case '1':

                // Open stock file in append mode
                fp=fopen("stock.csv","a");

                // Read new item details
                printf("Enter item name:");

                // Validate RFID card number
                printf("Enter Card Number:");

                // Validate quantity
                printf("Enter Quantity:");

                // Validate price
                printf("Enter Item Price:");

                // Save new item into stock.csv
                fprintf(fp,"%s,%s,%d,%d\n",
                        item_name,item_card,qty,price);

                fclose(fp);
                break;

            case '2':

                // Load stock file into memory
                file_buff=openfile("stock.csv","r+",file_buff);

                // Search item using RFID card
                p=strstr(file_buff,item_card);

                // Update price field
                // Resize memory if new price length changes

                fprintf(fp,"%s",file_buff);

                fclose(fp);
                break;

            case '3':

                // Load stock file
                file_buff=openfile("stock.csv","r+",file_buff);

                // Search item card

                // Update quantity field

                fprintf(fp,"%s",file_buff);

                fclose(fp);
                break;

            case '4':

                // Return to main menu
                return;
        }
    }
}
// Process scanned RFID item card
void item_card(char *card)
{
    char line[100],qty_buf[12],tx_buf[80];

    // Read stock database
    file_buff=openfile("stock.csv","r+",file_buff);

    rewind(fp);

    // Search item in stock.csv
    while(fgets(line,sizeof(line),fp))
    {
        if(strstr(line,card))
        {
            // Check whether item already exists in cart
            for(int i=0;i<cart_count;i++)
            {
                if(strcmp(cart_items[i].card,card)==0)
                {
                    // Read item details from stock file
                    sscanf(line,"%[^,],%[^,],%d,%d",
                           cart_items[i].name,
                           cart_items[i].card,
                           &cart_items[i].qty,
                           &cart_items[i].price);

                    // Check stock availability
                    if(cart_items[i].qty==0)
                    {
                        printf("%s is out of stock\n",
                               cart_items[i].name);

                        return;
                    }

                    // Reduce available stock by one
                    --cart_items[i].qty;

                    // Increase cart quantity
                    ++cart_items[i].count;

                    // Update total bill
                    total+=cart_items[i].price;

                    // Prepare UART message for LCD
                    sprintf(tx_buf,
                    "#%s,%d,%d$",
                    cart_items[i].name,
                    cart_items[i].price,
                    cart_items[i].qty);

                    flag=1;
                    break;
                }
            }

            // Item scanned first time
            if(flag==0)
            {
                // Increase cart size
                ++cart_count;

                // Allocate memory for new item
                cart_items=realloc(cart_items,
                        cart_count*sizeof(struct cart));

                // Read item information
                sscanf(line,"%[^,],%[^,],%d,%d",
                       cart_items[cart_count-1].name,
                       cart_items[cart_count-1].card,
                       &cart_items[cart_count-1].qty,
                       &cart_items[cart_count-1].price);

                // Check stock availability
                if(cart_items[cart_count-1].qty==0)
                {
                    printf("%s is out of stock\n",
                           cart_items[cart_count-1].name);

                    return;
                }

                // Reduce stock by one
                --cart_items[cart_count-1].qty;

                // First item count
                cart_items[cart_count-1].count=1;

                // Add price to total bill
                total+=cart_items[cart_count-1].price;

                // Prepare UART message
                sprintf(tx_buf,
                "#%s,%d,%d$",
                cart_items[cart_count-1].name,
                cart_items[cart_count-1].price,
                cart_items[cart_count-1].qty);
            }
        }
    }

    // Locate quantity field in file buffer
    p=strstr(file_buff,card);

    if(p==NULL)
    {
        printf("Oops! Item not found\n");
        return;
    }

    // Move pointer to quantity value
    p=p+9;

    // Update quantity in stock.csv
}
// Check if new quantity length is same as old quantity length
if(strlen(qty_buf)==qty_len)
{
    // Replace quantity directly
    strncpy(p,qty_buf,strlen(qty_buf));
}
else
{
    // Shift data when quantity length changes
    memmove(p,p+1,strlen(p+1)+1);

    // Store current position
    posn=p-file_buff;

    // Reallocate memory for updated data
    file_buff=realloc(file_buff,(strlen(file_buff))+1);

    // Copy new quantity into buffer
    memcpy(file_buff+posn,qty_buf,strlen(qty_buf));
}

// Write updated stock to file
rewind(fp);
fprintf(fp,"%s",file_buff);
rewind(fp);

// Send updated item details to LPC2148 through UART
uart_send(tx_buf);

fclose(fp);

// Display customer cart details
cart:
puts("\n------ITEMS taken by Customer------\n");

// Print cart heading
puts("ITEM\t\t\tPRICE\t\titems in cart\t\tREMAINING STOCK\n");

// Display each cart item
for(int i=0;i<cart_count;i++)
{
    printf("%-25s%-20d%-20d%d\n\n",
           cart_items[i].name,
           cart_items[i].price,
           cart_items[i].count,
           cart_items[i].qty);
}

// Display total bill amount
printf("---------------------------Total Amount :%d-------------------------\n",total);
puts("\n");
// Remove scanned item from customer cart
void delete_item(char *card)
{
    // Buffers for quantity, price and UART message
    char *itemname=NULL,price_buf[12],qty_buf[12],tx_buf[100];

    int flag=0,qty_len;

    // Search item in cart
    for(int i=0;i<cart_count;i++)
    {
        if(strcmp(cart_items[i].card,card)==0)
        {
            // Reduce item count in cart
            --cart_items[i].count;

            // Increase available stock
            ++cart_items[i].qty;

            // Prepare updated quantity
            sprintf(qty_buf,"%d",cart_items[i].qty);

            // Prepare UART message
            sprintf(tx_buf,"#%s,%d,%d$",
                    cart_items[i].name,
                    cart_items[i].price,
                    cart_items[i].qty);

            // Remove item if count becomes zero
            if(cart_items[i].count==0)
            {
                total-=cart_items[i].price;

                // Shift remaining cart items
                memmove(&cart_items[i],
                        &cart_items[i+1],
                        (cart_count-i-1)*sizeof(struct cart));

                --cart_count;
            }

            flag=1;
            break;
        }
    }

    // Display updated cart
    if(flag)
    {
        puts("\n=========== CART DETAILS =========\n");

        int total_price=0;

        printf("Total Cart Items:%d\n",cart_count);

        // Print all cart items
        for(int i=0;i<cart_count;i++)
        {
            printf("Item Name : %s\n",cart_items[i].name);
            printf("Item Price : %d\n",cart_items[i].price);
            printf("Remaining Qty : %d\n",cart_items[i].qty);
            printf("Items in Cart : %d\n",cart_items[i].count);

            // Calculate total amount
            total_price+=cart_items[i].price*cart_items[i].count;
        }

        printf("\nTotal Amount:%d\n",total_price);

        // Open stock file for updating quantity
        file_buff=openfile("stock.csv","r+",file_buff);

        // Find item in stock database
        p=strstr(file_buff,card);

        // Update stock quantity
        fprintf(fp,"%s",file_buff);

        fclose(fp);

        // Send updated details to LCD
        uart_send(tx_buf);
    }
    else
    {
        // Item not found in cart
        printf("NO such item found\n");

        uart_send("#NO item$");
    }
}
// Verify customer bank RFID card
void verify_Bankcard(char *cardbuff)
{
    // Open bank database
    fp=fopen("bank.csv","r");

    // Search card in database
    while(fgets(line,sizeof(line),fp))
    {
        if(strstr(line,cardbuff))
        {
            // Read bank details
            sscanf(line,"%[^,],%d,%d",
                   bank_details.card,
                   &bank_details.pin,
                   &bank_details.bal);

            card_present=1;
            break;
        }
    }

    fclose(fp);

    // Send verification result
    if(card_present)
    {
        uart_send("#FOUND$");
        display_bank(bank_details);
    }
    else
    {
        uart_send("#NOT FOUND$");
    }
}
// Verify PIN and perform payment
void verify_pin(int pin,int amt)
{
    // Open bank database and temporary file
    fp=fopen("bank.csv","r");
    FILE *temp_fp=fopen("temp.csv","w");

    // Read each account
    while(fgets(line,sizeof(line),fp))
    {
        if(strstr(line,bank_details.card))
        {
            // Check PIN
            if(pin==bank_details.pin)
            {
                // Check balance
                if(bank_details.bal>=amt)
                {
                    // Deduct amount
                    bank_details.bal-=amt;

                    uart_send("#SUCCESS$");
                }
                else
                {
                    uart_send("#LOWBAL$");
                }
            }
            else
            {
                uart_send("#WRONGPIN$");
            }

            // Save updated account
            fprintf(temp_fp,"%s,%d,%d\n",
                    bank_details.card,
                    bank_details.pin,
                    bank_details.bal);
        }
        else
        {
            // Copy remaining records
            fputs(line,temp_fp);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    // Replace old bank database
    remove("bank.csv");
    rename("temp.csv","bank.csv");
}
// Handle payment success or cancellation
void transaction_result(char *msg)
{
    // Payment successful
    if(msg[0]=='S')
    {
        // Get current date and time
        time(&t);
        tm_info=localtime(&t);

        // Convert received amount
        int amt=atoi(msg+1);

        // Format date and time
        strftime(datetime,sizeof(datetime),
                 "%d-%m-%y,%H:%M:%S",
                 tm_info);

        // Store income transaction
        fp=fopen("income.csv","a");
        fprintf(fp,"%s,%d\n",datetime,amt);
        fclose(fp);

        // Clear customer cart
        free(cart_items);
        cart_items=NULL;
        cart_count=0;
    }

    // Transaction cancelled
    else if(msg[0]=='C')
    {
        // Restore stock quantity if payment is cancelled
        fp=fopen("stock.csv","r");

        // Restore stock quantities when payment is cancelled
while(fgets(line,sizeof(line),fp))
{
    flag=1;

    // Check whether current stock item exists in cart
    for(int i=0;i<cart_count;i++)
    {
        if(strstr(line,cart_items[i].card))
        {
            // Restore original stock quantity
            fprintf(t_fp,"%s,%s,%d,%d\n",
                    cart_items[i].name,
                    cart_items[i].card,
                    (cart_items[i].qty + cart_items[i].count),
                    cart_items[i].price);

            flag=0;
            break;
        }
    }

    // Copy unchanged stock records
    if(flag)
        fputs(line,t_fp);
}

// Free cart memory
free(cart_items);
cart_items=NULL;
cart_count=0;

// Close files
fclose(fp);
fclose(t_fp);

// Replace old stock file with updated stock
remove("stock.csv");
rename("temp.csv","stock.csv");

// Display cancellation message
puts("PURCHASE CANCELED");
// Receive data from UART
int uart_receive(char *buff)
{
    char rx;
    int i=0;

    // Open UART communication
    if((fd = serialOpen("/dev/ttyUSB0",9600)) < 0)
    {
        fprintf(stderr,
                "Unable to open serial device: %s\n",
                strerror(errno));
        exit(0);
    }

    // Receive characters until NULL character
    while(1)
    {
        rx=serialGetchar(fd);

        if(rx=='\0')
        {
            // End of received data
            buff[i]='\0';
            break;
        }
        else
        {
            // Store received character
            buff[i++]=rx;
        }
    }
}
// Function to receive RFID card data
void get_data(char *buff)
{
#ifdef TEST_MODE
    // Test mode: enter card number manually from keyboard
    printf("Test Mode\nEnter Card Number Manually:");

    // Clear input buffer
    __fpurge(stdin);

    // Read card number from user
    fgets(buff,30,stdin);

    // Remove newline character
    buff[strlen(buff)-1]='\0';

#else
    // Normal mode: clear input buffer
    __fpurge(stdin);

    // Receive RFID data through UART
    uart_receive(buff);
#endif
}
// Function to display all available stock items
void display_stock(void)
{
    int i;
    char line[100], *p = NULL;

    // Open stock database file in read mode
    fp = fopen("stock.csv", "r");

    // Check whether file exists
    if(fp == NULL)
    {
        puts("stock.csv not found");
        return;
    }

    // Display stock table heading
    printf("\n==============================================================================================\n");
    printf("                             AVAILABLE STOCK ITEMS\n\n\n");
    printf("ITEM\t\t\tID\t\t\tStock\t\t\tPrice\n");
    printf("=================================================================================================\n");

    // Read each record from stock.csv
    while(fgets(line, 100, fp))
    {
        p = line;
        char *q = p;

        // Split CSV fields using comma delimiter
        while(*p)
        {
            if(*p == ',')
            {
                *p = '\0';              // Replace comma with null character
                printf("%-20s\t", q);   // Print current field
                q = p + 1;              // Move to next field
            }
            ++p;
        }

        // Print last field (price)
        printf("%s\n", q);
    }

    // Close stock file
    fclose(fp);
}
// Signal handler for Ctrl+C
void sigint_handler()
{
    // Restore stock if transaction is interrupted
    transaction_result("C");

    // Exit the program
    exit(0);
}
// Register SIGINT signal handler
void init_signal()
{
    signal(SIGINT,sigint_handler);
}
// Display bank account information
void display_bank(struct bank b)
{
    printf("\n--------------- Bank Database----------\n");

    // Display card ID, PIN and balance
    printf("CARD ID\t\tBalance\n");
    printf("%s\t\t%d\t\t%d\n",
            b.card,
            b.pin,
            b.bal);
}
// Main function
int main()
{
    char buff[30],temp[12];
    int display=0;

    struct bank b;

    // Initialize signal handling
    init_signal();

    while(1)
    {
        // Display stock only once at startup
        if(display==0)
        {
            display_stock();
            display=1;
        }

        // Wait for RFID data
        puts("Ready To Receive...");

        get_data(buff);

        // Manager RFID card scanned
        if(strcmp(buff,manager_card)==0)
        {
            manager_fun();
        }

        // Delete item from cart
        else if(buff[0]=='D' && buff[9]=='$')
        {
            buff[strlen(buff)-1]='\0';
            delete_item(buff+1);
        }

        // Bank card scanned
        else if(buff[0]=='B')
        {
            char *p=buff;

            buff[strlen(buff)-1]='\0';

            // Separate card number and amount
            while(*p!='A')
                ++p;

            *p='\0';

            // Verify bank card
            verify_Bankcard(buff+1);
        }

        // PIN received from customer
        else if(buff[0]=='P')
        {
            char *p=buff;

            buff[strlen(buff)-1]='\0';

            // Separate PIN and amount
            while(*p!='A')
                ++p;

            *p='\0';

            // Verify PIN and process payment
            verify_pin(atoi(buff+1),atoi(p+1));
        }

        // Payment success or cancellation
        else if(buff[0]=='S' || buff[0]=='C')
        {
            buff[strlen(buff)-1]='\0';

            transaction_result(buff);
        }

        // Exit application
        else if(strcmp(buff,"exit")==0)
        {
            // Save total income
            fp=fopen("income.csv","a");

            fprintf(fp,"%s,,%d\n","TOTAL",total);

            fclose(fp);

            exit(0);
        }

        // RFID item card scanned
        else if(buff[0]=='R')
        {
            printf("Item card\n");

            // Add item to customer cart
            item_card(buff+1);
        }
    }
}
			
			
