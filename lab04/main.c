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

int main(void) {

  WDTCTL = WDTPW | WDTHOLD; // Stop WDT
  PM5CTL0 &= ~LOCKLPM5;

  // Configure GPIO
  P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
  P1OUT &= ~redLED;   // Turn LED Off
  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  P1DIR &= ~BUT1; // Direct pin as input
  P1REN |= BIT1;        // Enable built-in resistor
  P1OUT |= BIT1;    // Set resistor as pull-up
  P1DIR &= ~BUT2; // Direct pin as input
  P1REN |= BIT2;        // Enable built-in resistor
  P1OUT |= BIT2;    // Set resistor as pull-up
}
