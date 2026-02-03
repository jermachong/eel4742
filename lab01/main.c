// EEL 4742
// Jeremy Achong
// Code that flashes the red LED
#include <msp430fr6989.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#include <stdbool.h>
#include <stdint.h>

//    void main(void) {
//        volatile unsigned int i;
//        WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//        PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power-on default high-impedance
//        mode

//        P1DIR |= redLED; // Direct pin as output
//        P1OUT &= ~redLED; // Turn LED Off

//        for(;;) {
//            // Delay loop
//            for(i=0; i<10000; i++) {}

//            P1OUT ^= redLED; // Toggle the LED
//        }
//    }

// Flashing in sync
//    void main(void) {
//        volatile unsigned int i;
//        WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//        PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power-on default high-impedance
//        mode
//
//        P1DIR |= redLED; // Direct pin as output
//        P1OUT &= ~redLED; // Turn LED Off
//        P9DIR |= greenLED;
//        P9OUT &= ~greenLED;
//
//
//        for(;;) {
//            // Delay loop
//            for(i=0; i<10000; i++) {}
//
//            P1OUT ^= redLED; // Toggle the LED
//            P9OUT ^= greenLED;
//        }
//    }

// long delay solutions
//    void main(void) {
////        volatile unsigned int i;
//        volatile uint32_t i; // unsigned int 32-bit type to hold larger int
//
//        WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//        PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power-on default high-impedance
//        mode
//
//        P1DIR |= redLED; // Direct pin as output
//        P1OUT &= ~redLED; // Turn LED Off
//        P9DIR |= greenLED;
//        P9OUT &= ~greenLED;
//
//
////        for(;;) {
////            // Delay loop solution 2 ( 3 seconds )
////            for(i=0; i<160000; i++) {}
////
////            P1OUT ^= redLED; // Toggle the LED
////            P9OUT ^= greenLED;
//
////        }
//
//        for(;;){
//            // Delay loop solution 3 ( 3 seconds)
//            _delay_cycles(2000000); // Creates a delay of 2000000 CPU cycles
//            P1OUT ^= redLED; // Toggle the LED
//            P9OUT ^= greenLED;
//        }
//    }

// // Firefighter pattern 1: fast four, slow three, fast four ...
// void main(void) {
//   volatile uint32_t i;
//   volatile unsigned int j, k, x;

//   WDTCTL = WDTPW | WDTHOLD;
//   PM5CTL0 &= ~LOCKLPM5;

//   P1DIR |= redLED;
//   P1OUT &= ~redLED;
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   for (;;) {
//     // Fast alternating (4 cycles)
//     for (j = 0; j < 4; j++) {
//       for (k = 0; k < 2; k++) {
//         for (x = 0; x < 16; x++) {
//           if (k == 0)
//             P1OUT ^= redLED;
//           else
//             P9OUT ^= greenLED;
//           for (i = 0; i < 1000; i++);
//         }
//         for (i = 0; i < 10000; i++);
//       }
//     }

//     // Slow alternating (3 cycles)
//     for (j = 0; j < 3; j++) {
//       for (k = 0; k < 2; k++) {
//         for (x = 0; x < 32; x++) {
//           if (k == 0)
//             P1OUT ^= redLED;
//           else
//             P9OUT ^= greenLED;
//           for (i = 0; i < 1000; i++);
//         }
//         for (i = 0; i < 20000; i++);
//       }
//     }
//   }
// }

// Firefighter pattern 2: slow alternate, fast alternate, simultaneous
void main(void) {
  volatile uint32_t i;
  volatile unsigned int j, k, x, y, z;

  WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
  PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power-on default high-impedance mode

  P1DIR |= redLED; // Direct pin as output
  P1OUT &= ~redLED; // Turn LED Off
  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  for(;;){
    _delay_cycles(10000);
    
    // Slow alternating pattern
    for(j = 0; j < 16; j++) {
      for(x = 0; x < 3; x++) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }
      _delay_cycles(200000);
      for(y = 0; y < 3; y++) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }
    
    // Fast alternating pattern
    for(k = 0; k < 7; k++) {
      for(x = 0; x < 3; x++) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }
      _delay_cycles(50000);
      for(y = 0; y < 3; y++) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }
    
    // Very fast alternating pattern
    for(k = 0; k < 5; k++) {
      for(x = 0; x < 3; x++) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }
      _delay_cycles(35000);
      for(y = 0; y < 3; y++) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }
    
    _delay_cycles(17000);
    
    // Simultaneous toggling pattern
    for(z = 0; z < 6; z++){
      _delay_cycles(10000);
      P1OUT ^= redLED;
      P9OUT ^= greenLED;
    }
    
    _delay_cycles(10000);
  }
}
