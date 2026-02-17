/*
 * EEL 4742
 * Jeremy Achong
 * Lab 3 - Timer Module
 */
#include <msp430fr6989.h>
#include <stdint.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

//**********************************
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

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop WDT

  // Configure GPIO
  P1DIR |= BIT0; // Clear P1.0 output latch for a defined power-on state
  P1OUT &= ~redLED;   // Turn LED Off

  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
                        // to activate previously configured port settings

  // Configure ACLK to the 32 KHz crystal
  config_ACLK_to_32KHz_crystal();

  // Configre Timer_A
  // use ACLK, divide by 1, continous mode, clear TAR
  TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR;
  
  // Ensure flag is cleared at the start
  TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit
  
  // build in RC oscillator running at a freq of 4.8 MHz / 128 = 37.5 KHz. 

  for(;;) {
    // wait for flag to be raised 
    while((TA0CTL & TAIFG)==0){} // wait for flag to become 1
    P1OUT ^= BIT0;
    TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit


  }
}


