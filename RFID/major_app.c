#include<lpc21xx.h>          // LPC2148 register definitions
#include<string.h>           // String handling functions
#include<stdio.h>            // Standard input/output functions
#include"types.h"            // User-defined data types
#include"delay.h"            // Delay functions
#include"lcd.h"              // LCD driver functions
#include"lcd_defines.h"      // LCD command definitions
#include"kpm.h"              // Keypad driver functions
#include"uart0_uart1.h"      // UART0 and UART1 functions
#include"major_proj.h"       // Project-specific function declarations

// Global buffers for UART communication and RFID data
s8 buff[30],rfid[30],rx_buff[50],msg[30],item_name[30];

// Variable to store keypad option
s8 op;

// Global control flags
s32 entry_flag,delete_flag,exit_flag,scan_flag,recv_flag,flag,i,retry,done;

// Variables for billing and payment
s32 cost,qty,totalamt,cash,PIN,attempt_cnt;
// Send data to Linux through UART0
void U0_send(char *buff)
{
    scan_flag=0;              // Reset scan flag

    U0_Txstring(buff);        // Send complete string

    U0_TxByte('\0');          // Send NULL character to indicate end of message
}
// Receive item information from Linux
void U0_recv()
{
    recv_flag=0;              // Clear receive flag

    // Extract item name, price and quantity
    sscanf(rx_buff,"%[^,],%d,%d",item_name,&cost,&qty);
}
// Display scanned item details on LCD
void item_details()
{
    cmdLCD(CLEAR_LCD);          // Clear LCD

    strLCD(item_name);          // Display item name

    charLCD(' ');

    strLCD("RQTY:");            // Display remaining quantity

    U32LCD(qty);

    cmdLCD(GOTO_LINE2_POS0);

    strLCD("TAmt:");            // Display total bill

    s32LCD(totalamt);
}
// Handle cash payment
void cash_payment()
{
    cmdLCD(CLEAR_LCD);

    strLCD("Enter amount:");

    // Read cash entered by customer
    cash=Readnum();

    // Check whether amount matches total bill
    if(cash==totalamt)
    {
        retry=0;               // Payment successful
        done=1;

        // Notify Linux about successful payment
        sprintf(msg,"S%d$",totalamt);

        U0_send(msg);

        // Display success message
        cmdLCD(CLEAR_LCD);

        strLCD("Payment Success");

        cmdLCD(GOTO_LINE2_POS0);

        strLCD("Visit Again");

        delay_s(2);

        totalamt=0;            // Reset total amount

        delay_s(2);
    }
    else
    {
        retry=1;               // Payment failed
    }
}

// Handle ATM card payment
void card_payment()
{
    // Clear LCD and ask customer to scan ATM card
    cmdLCD(CLEAR_LCD);
    strLCD("place ATM card");

    // Wait until ATM card is scanned
    scan_flag=0;
    while(scan_flag==0);

    // Prepare bank card message with total amount
    snprintf(rfid,sizeof(rfid),"B%sA%d$",buff,totalamt);

    // Send bank card details to Linux
    U0_send(rfid);

    // Wait for bank card verification
    while(recv_flag==0);

    recv_flag=0;

    // Check whether card exists
    if(strcmp(rx_buff,"NOT FOUND")==0)
    {
        retry=1;

        // Display card not found message
        cmdLCD(CLEAR_LCD);
        strLCD("CARD Not Found");
        delay_s(2);
    }
    else
    {
        // Allow maximum 3 PIN attempts
        attempt_cnt=3;

        while(1)
        {
            // Ask customer to enter ATM PIN
            cmdLCD(CLEAR_LCD);
            strLCD("Enter ATM PIN");

            cmdLCD(GOTO_LINE2_POS0);

            // Read PIN from keypad
            PIN=ReadPIN();

            // Prepare PIN verification message
            snprintf(msg,sizeof(msg),"P%dA%d$",PIN,totalamt);

            // Send PIN to Linux
            U0_send(msg);

            // Wait for verification result
            while(recv_flag==0);

            recv_flag=0;

            U0_recv();

            // Payment successful
            if(strcmp(rx_buff,"SUCCESS")==0)
            {
                retry=0;
                done=1;

                delay_ms(500);

                // Inform Linux about successful transaction
                sprintf(msg,"S%d$",totalamt);
                U0_send(msg);

                // Display success message
                cmdLCD(CLEAR_LCD);
                strLCD("payment success");

                cmdLCD(GOTO_LINE2_POS0);
                strLCD("Visit Again");

                delay_s(2);

                // Reset total bill
                totalamt=0;

                break;
            }

            // Wrong PIN entered
            else if(strcmp(rx_buff,"WRONGPIN")==0)
            {
                --attempt_cnt;

                cmdLCD(CLEAR_LCD);
                strLCD("wrong PIN");

                cmdLCD(GOTO_LINE2_POS0);

                // Display remaining attempts
                strLCD("You have ");
                U32LCD(attempt_cnt);
                strLCD(" chances");

                delay_s(2);
            }

            // Low account balance
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

            // All PIN attempts completed
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

// Main function
int main()
{
    // Initialize LCD, UART, keypad and GPIO
    init();

    // Display welcome message
    strLCD("smart cart RFID");
    delay_s(3);

    // Display shopping instruction
    cmdLCD(CLEAR_LCD);
    strLCD("PRESS Entry Key");
    cmdLCD(GOTO_LINE2_POS0);
    strLCD("& Begin Shopping");

    // Run continuously
    while(1)
    {
        // Check whether Entry key is pressed
        if(entry_flag)
        {
            // Clear entry flag
            entry_flag=0;

            // Clear RFID buffer
            memset(buff,0,sizeof(buff));

            // Reset all control flags
            scan_flag=0;
            recv_flag=0;
            delete_flag=0;
            exit_flag=0;

            // Ask customer to scan an item
            cmdLCD(CLEAR_LCD);
            strLCD("Hello,Place card");
            cmdLCD(GOTO_LINE2_POS0);
            strLCD("to Add Items");

            // Shopping loop
            while(1)
            {
                // Item RFID card scanned
                if(scan_flag)
                {
                    // Send RFID number to Linux
                    U0_send(buff);

                    // Turn ON buzzer/LED indication
                    IOSET0=1<<2;
                    delay_ms(1000);
                    IOCLR0=1<<2;

                    // Display scanned card
                    cmdLCD(CLEAR_LCD);
                    strLCD("card scanned");
                    cmdLCD(GOTO_LINE2_POS0);
                    strLCD(buff);
                }

                // Item details received from Linux
                if(recv_flag)
                {
                    // Extract item information
                    U0_recv();

                    // Add item price to total bill
                    totalamt += cost;

                    // Display item details
                    item_details();
                }

                // Delete key pressed
                if(delete_flag)
                {
                    delete_flag=0;

                    // Check whether cart is empty
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
                        // Ask customer to scan item for deletion
                        cmdLCD(CLEAR_LCD);
                        strLCD("Place card to");
                        cmdLCD(GOTO_LINE2_POS0);
                        strLCD("delete");
					                        // Wait for customer to scan item for deletion
                        while(1)
                        {
                            // Check whether an RFID card is scanned
                            if(scan_flag)
                            {
                                // Prepare delete request message
                                snprintf(rfid,sizeof(rfid),"D%s$",buff);

                                // Send delete request to Linux
                                U0_send(rfid);

                                // Wait for response
                                while(recv_flag==0);

                                // Item not found in cart
                                if(strcmp(rx_buff,"NO item")==0)
                                {
                                    recv_flag=0;

                                    cmdLCD(CLEAR_LCD);
                                    strLCD("No such item to");
                                    cmdLCD(GOTO_LINE2_POS0);
                                    strLCD("delete Add item");

                                    delay_s(2);

                                    // If cart becomes empty
                                    if(totalamt==0)
                                    {
                                        cmdLCD(CLEAR_LCD);
                                        strLCD("Press Entry key");
                                        cmdLCD(GOTO_LINE2_POS0);
                                        strLCD("and add items");
                                    }
                                    else
                                    {
                                        // Display current total amount
                                        cmdLCD(CLEAR_LCD);
                                        strLCD("TAmt:");
                                        s32LCD(totalamt);
                                        cmdLCD(GOTO_LINE2_POS0);
                                    }
                                }
                                else
                                {
                                    // Receive updated item details
                                    U0_recv();

                                    // Reduce total amount
                                    totalamt -= cost;

                                    // Display item deleted message
                                    cmdLCD(CLEAR_LCD);
                                    strLCD("one ");
                                    strLCD(item_name);

                                    cmdLCD(GOTO_LINE2_POS0);
                                    strLCD(" item deleted");

                                    delay_s(2);

                                    // Show updated bill details
                                    item_details();
                                }
                            }

                            // Entry key pressed again to continue shopping
                            else if(entry_flag==1)
                            {
                                entry_flag=0;

                                cmdLCD(CLEAR_LCD);

                                // Display current bill if cart has items
                                if(totalamt>0)
                                {
                                    strLCD("Total Amt:");
                                    s32LCD(totalamt);

                                    cmdLCD(GOTO_LINE2_POS0);
                                    strLCD("Add More Items");
                                }
                                else
                                {
                                    // Cart is empty
                                    strLCD("Place card and");
                                    cmdLCD(GOTO_LINE2_POS0);
                                    strLCD("Add items");
                                }

                                break;
                            }
                        }
                    }
                }