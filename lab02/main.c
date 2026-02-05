/*
* EEL 4742
* Jeremy Achong
* Lab 2 - Experimentation with using the board's buttons
*/

#include <driverlib.h>
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1
#define BUT2 BIT2 // Button S2 at P1.2

// 2.1: Turning on the red LED while button BUT1 is pushed 
// int main(void) {

//     volatile uint32_t i;

//     WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//     PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance

//     // Configure and initialize LEDs
//     P1DIR |= redLED; // Direct pin as output
//     P9DIR |= greenLED; // Direct pin as output
//     P1OUT &= ~redLED; // Turn LED Off
//     P9OUT &= ~greenLED; // Turn LED Off

//     // Configure buttons
//     P1DIR &= ~BUT1; // Direct pin as input
//     P1REN |= BIT1;        // Enable built-in resistor
//     P1OUT |= BIT1;    // Set resistor as pull-up

//     // Polling the button in an infinite loop
//     for (;;) {
//       // Rewrite the pseudocode below into C code
//         if ((P1IN * BUT1)==0) { // button is pushed
//             P1OUT ^= redLED;    
//         }
//         else {
//             P1OUT &= ~redLED; // Turn LED Off
//         }
//     }
// }


// 2.2: Using Two Push Buttons
// int main(void) {

//     volatile uint32_t i;

//     WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//     PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance

//     // Configure and initialize LEDs
//     P1DIR |= redLED; // Direct pin as output
//     P9DIR |= greenLED; // Direct pin as output
//     P1OUT &= ~redLED; // Turn LED Off
//     P9OUT &= ~greenLED; // Turn LED Off

//     // Configure buttons
//     P1DIR &= ~BUT1; // Direct pin as input
//     P1REN |= BIT1;        // Enable built-in resistor
//     P1OUT |= BIT1;    // Set resistor as pull-up
//     P1DIR &= ~BUT2; // Direct pin as input
//     P1REN |= BIT2;        // Enable built-in resistor
//     P1OUT |= BIT2;    // Set resistor as pull-up

//     // Polling the button in an infinite loop
//     for (;;) {

//         if ((P1IN * BUT1)==0) { // button is pushed
//             P1OUT ^= redLED;    
//         }
//         else {
//             P1OUT &= ~redLED; // Turn LED Off
//         }
//         if ((P1IN * BUT2)==0) { // button is pushed
//             P1OUT ^= greenLED;        
//         }
//         else {
//             P1OUT &= ~greenLED; // Turn LED Off
//         }
//     }
// }

// 2.3: Electric Load Generator (v1)
// first come first serve, don't run at the same time
// LED is on as long as button is pressed
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
    P1DIR &= ~BUT2; // Direct pin as input
    P1REN |= BIT2;        // Enable built-in resistor
    P1OUT |= BIT2;    // Set resistor as pull-up

    // Polling the button in an infinite loop
    for (;;) {
        if ((P1IN * BUT1)==0) { // button is pushed
            P1OUT |= redLED;    
        }
        else if ((P1IN * BUT2)==0) { // button is pushed
            P9OUT |= greenLED;        
        }
        else {
            P1OUT &= ~redLED; // Turn LED Off
            P9OUT &= ~greenLED; // Turn LED Off
        }
    }
}
