/*
* EEL 4742
* Jeremy Achong
* Turning on the red LED while button S1 is pushed 
*/

#include <driverlib.h>
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1

int main(void) {

    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= ~BUT1; // Direct pin as input
    P1REN |= BIT1;        // Enable built-in resistor
    P1OUT |= BIT1;    // Set resistor as pull-up

    // Polling the button in an infinite loop
    for (;;) {
      // Rewrite the pseudocode below into C code
        if ((P1IN * BUT1)==0) { // button is pushed
            P1OUT ^= redLED;    
        }
        else {
            P1OUT &= ~redLED; // Turn LED Off
        }
    }
}
