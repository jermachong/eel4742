// EEL 4742
// Jeremy Achong
// Code that flashes the red LED
#include <msp430fr6989.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#include <stdbool.h>
#include <stdint.h>

// void main(void) {
//   volatile unsigned int i;
//   WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance
//   mode

//       P1DIR |= redLED; // Direct pin as output
//   P1OUT &= ~redLED;    // Turn LED Off

//   for (;;) {
//     // Delay loop
//     for (i = 0; i < 10000; i++) {
//     }

//     P1OUT ^= redLED; // Toggle the LED
//   }
// }

// Flashing in sync void main(void) {
//   volatile unsigned int i;
//   WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance
//   mode

//       P1DIR |= redLED; // Direct pin as output
//   P1OUT &= ~redLED;    // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   for (;;) {
//     // Delay loop
//     for (i = 0; i < 10000; i++) {
//     }

//     P1OUT ^= redLED; // Toggle the LED
//     P9OUT ^= greenLED;
//   }
// }

// long delay solutions void main(void) {
//   //        volatile unsigned int i;
//   volatile uint32_t i; // unsigned int 32-bit type to hold larger int

//   WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance
//   mode

//       P1DIR |= redLED; // Direct pin as output
//   P1OUT &= ~redLED;    // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   //        for(;;) {
//   //            // Delay loop solution 2 ( 3 seconds )
//   //            for(i=0; i<160000; i++) {}
//   //
//   //            P1OUT ^= redLED; // Toggle the LED
//   //            P9OUT ^= greenLED;

//   //        }

//   for (;;) {
//     // Delay loop solution 3 ( 3 seconds)
//     _delay_cycles(2000000); // Creates a delay of 2000000 CPU cycles
//     P1OUT ^= redLED;        // Toggle the LED
//     P9OUT ^= greenLED;
//   }
// }

// // Firefighter pattern 1 : fast four, slow three, fast four... 
// void main(void) {
//   volatile uint32_t i; // unsigned int 32-bit type to hold larger int
//   int short_red = 0;
//   int short_green = 0;
//   volatile unsigned int j;
//   volatile unsigned int k;
//   volatile unsigned int x;
//   volatile unsigned int y;
//   volatile unsigned int z;

//   WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance mode

//   P1DIR |= redLED; // Direct pin as output
//   P1OUT &= ~redLED;    // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   for (;;) {

//     // faster alternating
//     for (j = 0; j < 4; j++) {

//       // Fast Red Blinks (4 times = 8 toggles)
//       for (short_red = 0; short_red < 16; short_red++) {
//         P1OUT ^= redLED;
//         for (i = 0; i < 1000; i++)
//           ; // Simple delay
//       }

//       // Small gap between colors
//       for (i = 0; i < 10000; i++)
//         ;

//       // Fast Green Blinks (4 times = 8 toggles)
//       for (short_green = 0; short_green < 16; short_green++) {
//         P9OUT ^= greenLED;
//         for (i = 0; i < 1000; i++)
//           ; // Simple delay
//       }

//       // Small gap between colors
//       for (i = 0; i < 10000; i++)
//         ;
//     }

//     // slower alternating
//     for (j = 0; j < 3; j++) {
//       // Fast Red Blinks (4 times = 8 toggles)
//       for (short_red = 0; short_red < 32; short_red++) {
//         P1OUT ^= redLED;
//         for (i = 0; i < 1000; i++)
//           ; // Simple delay
//       }

//       // longer gap between colors
//       for (i = 0; i < 20000; i++)
//         ;

//       // Fast Green Blinks (4 times = 8 toggles)
//       for (short_green = 0; short_green < 32; short_green++) {
//         P9OUT ^= greenLED;
//         for (i = 0; i < 1000; i++)
//           ; // Simple delay
//       }

//       // longer gap between colors
//       for (i = 0; i < 20000; i++)
//         ;
//     }
//   }
// }

// Firefighter pattern 2: Refactored to count down
void main(void) {
  volatile uint32_t i; 

  volatile unsigned int j;
  volatile unsigned int k;
  volatile unsigned int x;
  volatile unsigned int y;
  volatile unsigned int z;

  WDTCTL = WDTPW | WDTHOLD; 
  PM5CTL0 &= ~LOCKLPM5;     

  P1DIR |= redLED;  
  P1OUT &= ~redLED; 
  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  for (;;) {
    _delay_cycles(20000);

    // alternate
    for (j = 16; j > 0; j--) {
      for (x = 3; x > 0; x--) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }

      _delay_cycles(200000);

      for (y = 3; y > 0; y--) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }

    ////////////////////////
    // faster alternate
    for (k = 7; k > 0; k--) {
      for (x = 3; x > 0; x--) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }

      _delay_cycles(50000);

      for (y = 3; y > 0; y--) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }

    ///////////////////////
    // really fast alternate
    for (k = 7; k > 0; k--) {
      for (x = 3; x > 0; x--) {
        _delay_cycles(10000);
        P1OUT ^= redLED;
      }

      _delay_cycles(3900);

      for (y = 3; y > 0; y--) {
        _delay_cycles(10000);
        P9OUT ^= greenLED;
      }
    }

    _delay_cycles(17000);

    // simultaneous
    for (z = 6; z > 0; z--) {
      for (i = 10000; i > 0; i--) {
        // Manual delay loop counting down
      }

      P1OUT ^= redLED; 
      P9OUT ^= greenLED;
    }

    _delay_cycles(280000);
  }
}
