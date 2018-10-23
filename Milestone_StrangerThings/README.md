# MILESTONE 1
### Written by Ryan Baker
### Last editted on Oct 18, 2018
## Description
This project is written for the MSP430G2553. An addressable RGB LED node is created that receives bytes over UART to set the duty cycles for each color of the RGB LED. The node all passes the remaining bytes to the next node so the next node can set its own colors.
## Files
The code is contained in main.c.
## Execution
To run this code, create a new project in Code Composer that uses the MSP430G2553. I used CCS 8.2.0 for this. Add the code to your project by adding the file directly or copying and pasting the contents of the file into a new file. Debug and let CCS flash the code to your processor. If there are any updates, update your board. Once the code is running on your board, you can send your board data over UART to change the color of the RGB LED.
