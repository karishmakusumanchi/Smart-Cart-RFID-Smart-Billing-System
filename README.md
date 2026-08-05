# SmartCart – RFID Smart Billing System

## Overview

SmartCart is an embedded systems project that automates the retail billing process using RFID technology. The system enables customers to scan RFID-tagged products, automatically retrieve product information, calculate the total bill, and manage inventory through database integration. It reduces manual billing time, minimizes human errors, and provides a faster and more efficient checkout experience. The project integrates an LPC2148 ARM7 microcontroller with an RFID reader, LCD, keypad, UART communication, and a Linux-based backend application.

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

1. The user scans an RFID-tagged product.
2. The LPC2148 reads the RFID card ID.
3. The card ID is transmitted to the Linux application through UART.
4. The Linux application retrieves product details from the database.
5. Product name, price, and remaining stock are sent back to the microcontroller.
6. The LCD displays the product information and updates the total bill.
7. Users can add or remove products before checkout.
8. Payment is completed using either cash or RFID-based card authentication with PIN verification.

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
