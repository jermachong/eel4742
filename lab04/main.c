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
void config_ACLK_to_32KHz_crystal()
{
  // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
  // Reroute pins to LFXIN/LFXOUT functionality
  PJSEL1 &= ~BIT4;
  PJSEL0 |= BIT4;
  // Wait until the oscillator fault flags remain cleared
  CSCTL0 = CSKEY; // Unlock CS registers

  do
  {
    CSCTL5 &= ~LFXTOFFG; // Local fault flag
    SFRIFG1 &= ~OFIFG;   // Global fault flag
  } while ((CSCTL5 & LFXTOFFG) != 0);
  CSCTL0_H = 0; // Lock CS registers
  return;
}

// Part 1: flash the LEDs in continuous mode using interrupt
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
// int main(void) {

//   WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//   PM5CTL0 &= ~LOCKLPM5;

//   // Configure GPIO
//   P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
//   P1OUT &= ~redLED;   // Turn LED Off

//   // configure ACLK to the 32KHz crystal
//   config_ACLK_to_32KHz_crystal();

//   // Configure Channel 0 for up mode with interrupts
//   TA0CCR0 = 32768; // 1 second @ 32 KHz, for 0.5 s set to 16384, for 0.1 s set to 3276
//   TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
//   TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

//   //Configure Timer_A: divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero
//   TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR ; // use ACLK, divide by 1, continous mode, clear TAR
//   // TAIE is not set to 1 because we are using a different flag (CCIE, which compares TAR to TACCR0)

//   // Enable the global interrupt bit
//   __enable_interrupt();

//   // infinite loop, waits between interrupts
//   for(;;){}

// }

// // ** Writing the ISR **
// #pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
// __interrupt void T0A0_ISR() {
//   // Interrupt response goes here
//   P1OUT ^= redLED; // toggle LED
//   // hardware clears the flag
// }

// Part 3: Push Button with Interrupt
// int main(void) {

//   WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//   PM5CTL0 &= ~LOCKLPM5;

//   // Configure GPIO
//   P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
//   P1OUT &= ~redLED;   // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   P1DIR &= ~(BUT1|BUT2);  //0: Direct pin as input
//   P1REN |= (BUT1|BUT2);   //1: Enable built-in resistor
//   P1OUT |= (BUT1|BUT2);   //1: Set resistor as pull-up
//   P1IES |= (BUT1|BUT2);   //1: interrupt on falling edge (0 for rising edge)
//   P1IFG &= ~(BUT1|BUT2);  //0: clear the interrupt flags
//   P1IE  |= (BUT1|BUT2);   //1: enable the interrupts

//   // Enable the global interrupt bit
//   __enable_interrupt();

//   // infinite loop, waits between interrupts
//   for(;;){}

// }

// // ** Writing the ISR **
// #pragma vector = PORT1_VECTOR // Link the ISR to the vector
// __interrupt void P1_ISR() {
//   // Detect button 1 interrupt flag
//   if((P1IFG & BUT1) !=0 ) {
//     P1OUT ^= redLED; // toggle LED
//     P1IFG &= ~BUT1; // clear button 1 interrupt flag
//     __delay_cycles(200000);
//   }
//   // Detect button 2 interrupt flag
//   if((P1IFG & BUT2) !=0 ) {
//     P9OUT ^= greenLED; // toggle LED
//     P1IFG &= ~BUT2; // clear button 2 interrupt flag
//     __delay_cycles(200000);
//   }
// }

// Part 4: Low-Power Modes
// int main(void) {

//   WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//   PM5CTL0 &= ~LOCKLPM5;

//   // Configure GPIO
//   P1DIR |= redLED; // Clear P1.0 output latch for a defined power-on state
//   P1OUT &= ~redLED;   // Turn LED Off
//   P9DIR |= greenLED;
//   P9OUT &= ~greenLED;

//   P1DIR &= ~(BUT1|BUT2);  //0: Direct pin as input
//   P1REN |= (BUT1|BUT2);   //1: Enable built-in resistor
//   P1OUT |= (BUT1|BUT2);   //1: Set resistor as pull-up
//   P1IES |= (BUT1|BUT2);   //1: interrupt on falling edge (0 for rising edge)
//   P1IFG &= ~(BUT1|BUT2);  //0: clear the interrupt flags
//   P1IE  |= (BUT1|BUT2);   //1: enable the interrupts

//   // engage low power mode
//   _low_power_mode_4(); // auto enables interrupt

// }

// // ** Writing the ISR **
// #pragma vector = PORT1_VECTOR // Link the ISR to the vector
// __interrupt void P1_ISR() {
//   // Detect button 1 interrupt flag
//   if((P1IFG & BUT1) !=0 ) {
//     P1OUT ^= redLED; // toggle LED
//     P1IFG &= ~BUT1; // clear button 1 interrupt flag
//     __delay_cycles(200000);
//   }
//   // Detect button 2 interrupt flag
//   if((P1IFG & BUT2) !=0 ) {
//     P9OUT ^= greenLED; // toggle LED
//     P1IFG &= ~BUT2; // clear button 2 interrupt flag
//     __delay_cycles(200000);
//   }
// }

// Part 5: Application: Crawler Guidance System
volatile int left = 0, right = 0;
int_fast8_t redEnabled = 1, greenEnabled = 1;
int main(void)
{

  WDTCTL = WDTPW | WDTHOLD; // Stop WDT
  PM5CTL0 &= ~LOCKLPM5;

  // Configure GPIO
  P1DIR |= redLED;  // Clear P1.0 output latch for a defined power-on state
  P1OUT &= ~redLED; // Turn LED Off
  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  P1DIR &= ~(BUT1 | BUT2); // 0: Direct pin as input
  P1REN |= (BUT1 | BUT2);  // 1: Enable built-in resistor
  P1OUT |= (BUT1 | BUT2);  // 1: Set resistor as pull-up
  P1IES |= (BUT1 | BUT2);  // 1: interrupt on falling edge (0 for rising edge)
  P1IFG &= ~(BUT1 | BUT2); // 0: clear the interrupt flags
  P1IE |= (BUT1 | BUT2);   // 1: enable the interrupts

  // configure ACLK to the 32KHz crystal
  config_ACLK_to_32KHz_crystal();

  // Configure Channel 0 for up mode with interrupts
  TA0CCR0 = 32768;    // 1 second @ 32 KHz
  TA0CCTL0 |= CCIE;   // Enable Channel 0 CCIE bit
  TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

  // Configure Timer_A: divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero
  TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR; // use ACLK, divide by 1, continous mode, clear TAR
  // TAIE is not set to 1 because we are using a different flag (CCIE, which compares TAR to TACCR0)

  // engage low power mode 3 (Leaves ACLK on)
  _low_power_mode_3(); // auto enables interrupt
}

// ** Writing the ISRs **
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR()
{
  // Interrupt response goes here
  if (redEnabled)
    P1OUT ^= redLED; // toggle LED
  if (greenEnabled)
    P9OUT ^= greenLED; // toggle LED
}

#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void P1_ISR()
{
  uint8_t flags = P1IFG;
  P1IFG &= ~(BUT1 | BUT2); // clear buttons interrupt flag

  // Detect button 1 interrupt flag
  // left steering
  if ((flags & BUT1) != 0)
  {
    if (right > 0) // right is activated, let's move back to neutral
    {
      right--;
      if (right == 0)
      {
        greenEnabled = 1; // re-enable right
        redEnabled = 1;
        TA0CCR0 = 32768; // speed up timer
        P1OUT &= ~redLED;
        P9OUT &= ~greenLED;
      }
      else if (right == 1)
        TA0CCR0 = 16384;
      else if (right == 2)
        TA0CCR0 = 16384 / 2;
      TA0R = 0;
    }
    else // steering left
    {
      greenEnabled = 0;   // disable right (greenLED)
      P9OUT &= ~greenLED; // force LED off
      redEnabled = 1;     // make sure left is still  blinking
      if (left == 0)      // first left (R+)
      {
        TA0CCR0 = 16384; // speed up timer
        left = 1;
      }
      else if (left == 1) // second left (R++)
      {
        TA0CCR0 = 16384 / 2; // speed up timer
        left = 2;
      }
      else if (left == 2) // third left (R+++)
      {
        // speed up timer to max freq
        TA0CCR0 = 16384 / 4; // speed up timer
        left = 3;            // max reached
      }
      TA0R = 0; // Ensure the new speed takes effect immediately
    }
    __delay_cycles(500000);
  }
  // Detect button 2 interrupt flag
  // right steering
  if ((flags & BUT2) != 0)
  {
    if (left > 0) // left is activated
    {
      left--;
      if (left == 0)
      {
        greenEnabled = 1; // re-enable right
        redEnabled = 1;
        TA0CCR0 = 32768; // speed up timer
        P1OUT &= ~redLED;
        P9OUT &= ~greenLED;
      }
      else if (left == 1) // second left (R++)
        TA0CCR0 = 16384;
      else if (left == 2) // third left (R+++)
        TA0CCR0 = 16384 / 2;
      TA0R = 0; // reset timer
    }
    else // steering right
    {
      redEnabled = 0;
      P1OUT &= ~redLED;
      greenEnabled = 1; // make sure right is still blinking
      if (right == 0)   // first right (G+)
      {
        TA0CCR0 = 16384; // speed up timer
        right = 1;
      }
      else if (right == 1) // second right (G++)
      {
        TA0CCR0 = 16384 / 2; // speed up timer
        right = 2;
      }
      else if (right == 2) // third right (G+++)
      {
        // speed up timer to max freq
        TA0CCR0 = 16384 / 4; // speed up timer
        right = 3;           // max reached
      }
      TA0R = 0; // Ensure the new speed takes effect immediately
    }
    __delay_cycles(500000);
  }
  // __delay_cycles(250000);
}
