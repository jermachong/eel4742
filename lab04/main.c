/*
 * EEL 4742
 * Jeremy Achong
 * Lab 4 - Interrupts & Low-Power Modes
 */
#include <msp430fr6989.h>
#include <stdint.h>
#include <stdio.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1     // Button S1 at P1.1
#define BUT2 BIT2     // Button S2 at P1.2

// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
  // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
  // Reroute pins to LFXIN/LFXOUT functionality
  PJSEL1 &=~BIT4;
  PJSEL0 |= BIT4;
  // Wait until the oscillator fault flags remain cleared
  CSCTL0 = CSKEY; // Unlock CS registers
  
  do {
    CSCTL5 &= ~LFXTOFFG; // Local fault flag
    SFRIFG1 &= ~OFIFG;   // Global fault flag
  } while ((CSCTL5 & LFXTOFFG) != 0);
  CSCTL0_H = 0; // Lock CS registers
  return;
}

// // Part 1: flash the LEDs in continuous mode using interrupt
// int main(void) {

//   WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//   PM5CTL0 &= ~LOCKLPM5;

//   // Configure GPIO
//   P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
//   P1OUT &= ~redLED;   // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   // configure ACLK to the 32KHz crystal
//   config_ACLK_to_32KHz_crystal();

//   //Configure Timer_A: divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero
//   TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR | TAIE; // use ACLK, divide by 1, continous mode, clear TAR
//   // period of about 3 seconds
  
//   // Ensure flag is cleared at the start
//   TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit

//   // Enable the global interrupt bit
//   __enable_interrupt();

//   // infinite loop, waits between interrupts
//   for(;;){}

// }

// // ** Writing the ISR **
// #pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
// __interrupt void T0A1_ISR() {
//   // Interrupt response goes here
//   P1OUT ^= redLED; // toggle LED
//   // clear flag, otherwise LED will stay on as flag is never raised again
//   TA0CTL &= ~TAIFG; 

// }

// Part 2: Timer's Up Mode with Interrupt
int main(void) {

  WDTCTL = WDTPW | WDTHOLD; // Stop WDT
  PM5CTL0 &= ~LOCKLPM5;

  // Configure GPIO
  P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
  P1OUT &= ~redLED;   // Turn LED Off

  // configure ACLK to the 32KHz crystal
  config_ACLK_to_32KHz_crystal();
  
  // Configure Channel 0 for up mode with interrupts
  TA0CCR0 = 32768-1; // 1 second @ 32 KHz, for 0.5 s set to 16384, for 0.1 s set to 3276
  TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
  TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

  //Configure Timer_A: divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero
  TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR ; // use ACLK, divide by 1, continous mode, clear TAR
  // TAIE is not set to 1 because we are using a different flag (CCIE, which compares TAR to TACCR0)
  

  // Enable the global interrupt bit
  __enable_interrupt();

  // infinite loop, waits between interrupts
  for(;;){}

}

// ** Writing the ISR **
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
  // Interrupt response goes here
  P1OUT ^= redLED; // toggle LED
  // hardware clears the flag
}

// Part 3: Push Button with Interrupt

