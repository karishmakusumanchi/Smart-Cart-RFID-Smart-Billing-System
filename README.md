# SmartCart – RFID Smart Billing System

<img width="1402" height="1122" alt="image" src="https://github.com/user-attachments/assets/a788d1fb-9e45-44f8-b759-8556c81a6356" />


## Overview

SmartCart is an embedded systems project that automates the retail billing process using RFID technology. The system enables customers to scan RFID-tagged products, automatically retrieve product information, calculate the total bill, and manage inventory through database integration. It reduces manual billing time, minimizes human errors, and provides a faster and more efficient checkout experience. The project integrates an LPC2148 ARM7 microcontroller with an RFID reader, LCD, keypad, UART communication, and a Linux-based backend application.
![Uploading image.png…]()


## Features

* RFID-based automatic product identification
* Real-time billing and total amount calculation
* Inventory management with database integration
* Manager card support for stock updates
* Add and remove item functionality
* Cash and card payment options
* ATM PIN verification for card payments
* UART communication between LPC2148 and Linux
* LCD display for user interaction
* Keypad-based menu navigation

## Hardware Used

* LPC2148 (ARM7 Microcontroller)
* RFID Reader
* RFID Cards
* 16×2 LCD Display
* 4×4 Matrix Keypad
* MAX232
* USB-to-UART Converter
* Push Buttons/Switches

## Software Used

* Embedded C
* Keil uVision
* Flash Magic
* Linux System Programming
* UART Communication
* Text-file based Database Management
## Working

The SmartCart RFID Smart Billing System automates the shopping and billing process using RFID technology and an LPC2148 ARM7 microcontroller. The complete workflow is as follows:

1. **Customer Authentication**
   - The customer scans the RFID customer card.
   - The system verifies the customer details and displays the available account balance.

2. **Product Scanning**
   - Each product is scanned using its RFID tag.
   - The RFID reader reads the unique tag ID and sends it to the LPC2148 microcontroller.

3. **Data Communication**
   - The LPC2148 transmits the RFID tag ID to the Linux application through UART communication.
   - The Linux application searches the product database (`stock.csv`) for the matching product information.

4. **Product Display**
   - The Linux application sends the product details, including product name, price, and available quantity, back to the microcontroller.
   - The 16×2 LCD displays the product information.
   - The selected product is added to the customer's shopping cart.

5. **Cart Management**
   - Customers can continue scanning products to add multiple items.
   - Products can also be removed from the cart if required.
   - The system updates the total bill after every operation.

6. **Payment Process**
   - After shopping is completed, the customer selects the payment option.
   - For card payment, the customer enters the PIN using the keypad.
   - The system verifies the PIN and checks the account balance stored in `bank.csv`.

7. **Transaction Completion**
   - If sufficient balance is available, the payment is processed successfully.
   - The customer account balance is updated.
   - The product quantity in `stock.csv` is reduced accordingly.
   - The LCD displays a **Payment Successful** message.

8. **Manager Mode**
   - The manager scans a manager RFID card to access stock management.
   - The manager can add new products or update the stock quantity in the database.

## Applications

* Supermarkets
* Retail Stores
* Smart Shopping Trolleys
* Automated Billing Counters
* Inventory Management Systems

## Future Enhancements

* IoT-based cloud database integration
* Mobile application support
* QR code and barcode integration
* Digital payment gateway integration
* Online inventory monitoring
* Sales analytics dashboard

## Skills Demonstrated

* Embedded C Programming
* ARM7 (LPC2148)
* RFID Technology
* UART Communication
* Linux System Programming
* Database Integration
* Embedded Hardware Interfacing
* Debugging and Testing

## Project Outcome

This project demonstrates the implementation of an RFID-enabled smart billing system that automates product identification, billing, and inventory management. It showcases practical knowledge of embedded systems, ARM7 microcontrollers, Linux programming, serial communication, and hardware-software integration to build a real-world retail automation solution.
