#include <msp430.h>

/**
 * main.c
 * Author: Ryan Baker
 * Date: 10/17/18
 */

char size = 0;
char count = 0;

void main(void)
{
   // disable watchdog timer
   WDTCTL = WDTPW + WDTHOLD;        // watchdog timer stopped by enabling bit 7 (WDTHOLD) and using
                                    // the password (WDTPW)

   // P1.6 => red, P2.1 => green, P2.4 => blue
   // enable output to LED and turn it off
   P1SEL |= 0x40;                   // enable peripheral function on P1.6...
   P1SEL2 &= ~0x40;                 // ...by setting both of these bits
   P1DIR |= 0x40;                   // set P1.6 as an output
   P1OUT &= ~0x40;                  // set P1.6 low
   P2SEL |= 0x12;                   // enable peripheral function on P2.1 and P2.4...
   P2SEL2 &= ~0x12;                 // ...by settings both of these bits
   P2DIR |= 0x12;                   // set P2.1 and P2.4 as outputs
   P2OUT &= ~0x12;                  // set P2.1 and P2.4 low

   // enable RXD/TXD
   P1SEL |= 0x06;                   // enable RXD/TXD function on P1.1 and P1.2...
   P1SEL2 |= 0x06;                  // ...by settings both of these bits

   // configure timer A0
   TA0CCTL1 = OUTMOD_3;             // set to OUTMOD_3 for pwm set/reset
   TA0CCR0 = 255;                   // set to 255
   TA0CCR1 = 0;                     // set to 0
   TA0CTL = TASSEL_2 + MC_1 + ID_2 + TACLR; // set to use SMCLK (TASSEL_2) and count to CCR0 in up mode
                                    // (MC_1) divided by 4 (ID_2)

   // configure timer A1
   TA1CCTL1 = OUTMOD_3;             // set to OUTMOD_3 for pwm set/reset
   TA1CCTL2 = OUTMOD_3;             // set to OUTMOD_3 for pwm set/reset
   TA1CCR0 = 255;                   // set to 255
   TA1CCR1 = 0;                     // set to 0
   TA1CCR2 = 0;                     // set to 0
   TA1CTL = TASSEL_2 + MC_1 + ID_2 + TACLR; // set to use SMCLK (TASSEL_2) and count to CCR0 in up mode
                                    // (MC_1) divided by 4 (ID_2)

   // configure UART and set baud rate... (UCA0CTL1)/(UCA0BR0 + UCA0BR1 x 256) = baud rate
   UCA0CTL1 |= UCSSEL_2;            // set to use SMCLK (UCSSEL_2)
   UCA0BR0 = 104;                   // set to 104
   UCA0BR1 = 0;                     // set to 0
   UCA0MCTL = UCBRS_2;              // set modulation pattern to high on bit 1 & 5
   UCA0CTL1 &= ~UCSWRST;            // initialize USCI
   UC0IE |= UCA0RXIE;               // enable USCI_A0 RX interrupt

   // put into low power mode and enable global interrupts
   _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=USCIAB0RX_VECTOR
// interrupt when receiving data
__interrupt void USCI0RX_ISR(void)
{
   UC0IE |= UCA0RXIE;               // enable USCI_A0 RX interrupt
   char byte = UCA0RXBUF;           // store the byte from UART in a register temporarily

   // if this is the first byte received
   if(count == 0)
   {
      size = byte + 1;                  // the number bytes expected is set to the value received
      count = byte + 1;                 // keeps track of number of bytes processed/remaining
      // if there is enough data for the next node
      if(byte >= 6)
      {
         UCA0TXBUF = byte - 3;      // send the number of bytes the next node should expect
      }
      else
      {
          UCA0TXBUF = 0x00;         // send the number of bytes the next node should expect
      }
   }
   // if this is the second byte received
   else if((size - count) == 1)
   {
      TA0CCR1 = byte;               // set the duty cycle of red to this value
   }
   // if this is the third byte received
   else if((size - count) == 2)
   {
      TA1CCR1 = byte;               // set the duty cycle of green to this value
   }
   // if this is the fourth byte received
   else if((size - count) == 3)
   {
      TA1CCR2 = byte;               // set the duty cycle of blue to this value
   }
   // if it is any other byte
   else
   {
      // and there are enough bytes to send to the next node
      if(size >= 6)
      {
         UCA0TXBUF = byte;          // send the remaining bytes to the next node
      }
      else
      {
          size = byte + 1;                  // the number bytes expected is set to the value received
          count = byte + 1;                 // keeps track of number of bytes processed/remaining
          // if there is enough data for the next node
          if(byte >= 6)
          {
             UCA0TXBUF = byte - 3;      // send the number of bytes the next node should expect
          }
          else
          {
             UCA0TXBUF = 0x00;          // send the number of bytes the next node should expect
          }
      }
   }
   count -= 1;                      // keep track of number of bytes processed/remaining
}

#pragma vector=USCIAB0TX_VECTOR
// interrupt when transmitting data
__interrupt void USCI0TX_ISR(void)
{
   UC0IE &= ~UCA0RXIE;              // disable USCI_A0 RX interrupt
}
