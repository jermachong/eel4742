/*
 * EEL 4742
 * Jeremy Achong
 * Lab 3 - Timer Module
 */
#include <msp430fr6989.h>
#include <stdint.h>
#include <stdio.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1     // Button S1 at P1.1
#define BUT2 BIT2     // Button S2 at P1.2

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

// Part 1: Using ACLK, 32KHz crystal, continous mode

//int main(void) {
//  WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//
//  // Configure GPIO
//  P1DIR |= BIT0; // Clear P1.0 output latch for a defined power-on state
//  P1OUT &= ~redLED;   // Turn LED Off
//
//  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
//                        // to activate previously configured port settings
//
//  // Configure ACLK to the 32 KHz crystal
//  config_ACLK_to_32KHz_crystal();
//
//  // Configre Timer_A
//  // use ACLK, divide by 1, continous mode, clear TAR
//  TA0CTL = TASSEL_1 | ID_1 | MC_2 | TACLR;
//
//  // Ensure flag is cleared at the start
//  TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit
//
//  // build in RC oscillator running at a freq of 4.8 MHz / 128 = 37.5 KHz.
//
//  for(;;) {
//    // wait for flag to be raised
//    while((TA0CTL & TAIFG)==0){} // wait for flag to become 1
//    P1OUT ^= BIT0;
//    TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit
//  }
//}

//Part 2: Using Up Mode

//int main(void) {
//  WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//
//  // Configure GPIO
//  P1DIR |= BIT0; // Clear P1.0 output latch for a defined power-on state
//  P1OUT &= ~redLED;   // Turn LED Off
//
//  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
//                        // to activate previously configured port settings
//
//  // Configure ACLK to the 32 KHz crystal
//  config_ACLK_to_32KHz_crystal();
//
//  // Set timer period
//  TA0CCR0 = 3277;
//
//  // Configure Timer_A
//  // use ACLK, divide by 1, up mode, clear TAR
//  TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
//
//  // Ensure flag is cleared at the start
//  TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit
//
//  // build in RC oscillator running at a freq of 4.8 MHz / 128 = 37.5 KHz.
//
//  for(;;) {
//    // wait for flag to be raised
//    while((TA0CTL & TAIFG)==0){} // wait for flag to become 1
//    P1OUT ^= BIT0;
//    TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit
//  }
//}

//Part 3: Signal Repeater

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop WDT

  // Configure GPIO
  P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
  P1OUT &= ~redLED;   // Turn LED Off
  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
                        // to activate previously configured port settings

  P1DIR &= ~BUT1; // Direct pin as input
  P1REN |= BIT1;        // Enable built-in resistor
  P1OUT |= BIT1;    // Set resistor as pull-up
  P1DIR &= ~BUT2; // Direct pin as input
  P1REN |= BIT2;        // Enable built-in resistor
  P1OUT |= BIT2;    // Set resistor as pull-up

  // Configure ACLK to the 32 KHz crystal
  config_ACLK_to_32KHz_crystal();

  // Configure Timer_A
  // use ACLK, divide by 1, continous mode, clear TAR
  unsigned int limit = 65534; // max supported cycles
  // results in 2 second delay with div by 1
  // results in 4 second delay with div by 2
  TA0CTL = TASSEL_1 | ID_1 | MC_2 | TACLR;

  // Ensure flag is cleared at the start
  TA0CTL &= ~TAIFG; // AND with inverse of mask to clear the bit

  for(;;) {
    
    while((P1IN & BUT1)); // button not pushed yet
    TA0CTL |= TACLR; // reset timer
  
    while(!(P1IN & BUT1)){
      if(TA0R > limit) { // if timer exceeds 
        P9OUT |= greenLED; // turn greenLED on
        
        while(P1IN & BUT2);

          if(!(P1IN & BUT2)) { // wait until button 2 is pressed to reset
              P9OUT &= ~greenLED;
              TA0CTL |= TACLR; // reset TA0R
          }
          
      } // keep timer going until release
    }
    
    // button is released
    
    unsigned int duration = TA0R; // save the value that TAR reached

    P1OUT |= BIT0; // turn on LED
    TA0CTL |= TACLR; // start timer at 0

    while(TA0R <= duration){} // delay until TAR reaches TA0CCR0
    P1OUT &= ~BIT0; // turn off LED
    TA0CTL |= TACLR; // reset TA0R
    duration = 0;
    }
  }




