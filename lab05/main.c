/*
 * EEL 4742
 * Jeremy Achong
 * Lab 5 - LCD Display
 */
#include <msp430fr6989.h>
#include <stdint.h>
#include <stdio.h>
#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1     // Button S1 at P1.1
#define BUT2 BIT2     // Button S2 at P1.2
#define exclaim BIT0  // exclamaition mark
#define timer BIT3	  // timer icon
#define decimal BIT0  // decimal icon
#define colon BIT2;   // colon icon

// The array has the shapes of the digits (0 to 9)
const unsigned char LCD_Shapes[10] = {
	0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE0, 0xFF, 0xF7 };
	
						// from right to left
uint8_t * LCDptrs[8] = {&LCDM8, &LCDM15, &LCDM19, &LCDM4, &LCDM6};

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


void lcd_disp_symbol(int status){

}

void lcd_write_uint16(unsigned int n ){
	int digit;
	// uint8_t * LCDptr = &LCDM0;
	int i = 0;
	do{
		digit = n % 10;
		*LCDptrs[i] = LCD_Shapes[digit];
		i++;
		n = n/10;
	}while(n>0);

	// clear the other digits
	while(i<8) {
		*LCDptrs[i] = 0x00;
		i++;
	}

	return;
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD() {
PJSEL0 = BIT4 | BIT5; // For LFXT
LCDCPCTL0 = 0xFFD0;
LCDCPCTL1 = 0xF83F;
LCDCPCTL2 = 0x00F8;
// Configure LFXT 32kHz crystal
CSCTL0_H = CSKEY >> 8; // Unlock CS registers
CSCTL4 &= ~LFXTOFF; // Enable LFXT
do {
CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
SFRIFG1 &= ~OFIFG;
}while (SFRIFG1 & OFIFG); // Test oscillator fault flag
CSCTL0_H = 0; // Lock CS registers
// Initialize LCD_C
// ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
// VLCD generated internally,
// V2-V4 generated internally, v5 to ground
// Set VLCD voltage to 2.60v
// Enable charge pump and select internal reference for it
LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled

LCDCMEMCTL = LCDCLRM; // Clear LCD memory
//Turn LCD on (do this at the end!)
LCDCCTL0 |= LCDON;
return;
}



// Part 1: Printing on the LCD Display & Displaying 16-bit Unsigned Number
// int main(void)
// {
// 	volatile unsigned int n;
// 	WDTCTL = WDTPW | WDTHOLD; // Stop WDT
// 	PM5CTL0 &= ~LOCKLPM5;
// 	P1DIR |= redLED; // Pins as output
// 	P1OUT |= redLED; // Red on

// 	// Initializes the LCD_C module
// 	Initialize_LCD();

// 	LCDCMEMCTL = LCDCLRM; // Clears all the segments

// 	// Display 430 on the rightmost three digits
// 	// LCDM19 = LCD_Shapes[7];
// 	// LCDM15 = LCD_Shapes[8];
// 	// LCDM8 = LCD_Shapes[9];

// 	// Display 16-bit Unsigned Number
// 	n = 1337;
// 	lcd_write_uint16(n);

// 	//Flash the red LED
// 	for(;;){
// 		for(n=0;n<=60000;n++) {}
// 		P1OUT ^= redLED;
// 	}
// 	return 0;
// }

// Part 2: Implementing a Counter
// unsigned int counter = 0;
// int main(void)
// {
// 	volatile unsigned int n;
// 	WDTCTL = WDTPW | WDTHOLD; // Stop WDT
// 	PM5CTL0 &= ~LOCKLPM5;
// 	P1DIR |= redLED; // Pins as output
// 	P9DIR |= greenLED;
// 	P1OUT |= redLED; // Red on
// 	P9OUT &= ~greenLED; // Green off

// 	// ** Button Setup **
// 	P1DIR &= ~(BUT1|BUT2);  //0: Direct pin as input
// 	P1REN |= (BUT1|BUT2);   //1: Enable built-in resistor
// 	P1OUT |= (BUT1|BUT2);   //1: Set resistor as pull-up
// 	P1IES |= (BUT1|BUT2);   //1: interrupt on falling edge (0 for rising edge)
// 	P1IFG &= ~(BUT1|BUT2);  //0: clear the interrupt flags
// 	P1IE  |= (BUT1|BUT2);   //1: enable the interrupts
	
// 	// *** Timer Module Setup ***
// 	// Configure Channel 0 for up mode with interrupts
// 	TA0CCR0 = 32768; // 1 second @ 32 KHz
// 	TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
// 	TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

// 	//Configure Timer_A: divide by 1, up mode, TAR cleared, enable interrupt for rollback-to-zero
// 	TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR ; // use ACLK, divide by 1, continous mode, clear TAR
// 	// TAIE is not set to 1 because we are using a different flag (CCIE, which compares TAR to TACCR0)

// 	// Enable the global interrupt bit
// 	__enable_interrupt();
	
// 	// configure ACLK to the 32KHz crystal
//  	config_ACLK_to_32KHz_crystal();

// 	// Initializes the LCD_C module
// 	Initialize_LCD();

// 	LCDCMEMCTL = LCDCLRM; // Clears all the segments

// 	//Flash the red LED
// 	for(;;){	
// 		// Display 16-bit Unsigned Number
// 		lcd_write_uint16(counter); // update num on display
// 	}

// }

// // ** ISR for Part 2**
// #pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
// __interrupt void T0A0_ISR() {
//   // Interrupt response goes here
//   counter++; // increase number on display by 1 every second
//   // hardware clears the flag
// }

// // ** ISR for Part 2 **
// #pragma vector = PORT1_VECTOR // Link the ISR to the vector
// __interrupt void P1_ISR() {
//   // If S1 is pressed, reset counter
//   if((P1IFG & BUT1) !=0 ) {
//     counter = 0;
// 	TA0R = 0;
//     P1IFG &= ~BUT1; // clear button 1 interrupt flag
//     __delay_cycles(500000);
//   }
//   // If S2 is pressed, add 1000
//   if((P1IFG & BUT2) !=0 ) {
//     counter += 1000;
//     P1IFG &= ~BUT2; // clear button 2 interrupt flag
//     __delay_cycles(500000);
//   }
// }

// Part 3: Utility Chronometer
unsigned int counter = 0;
int main(void)
{
	volatile unsigned int n;
	WDTCTL = WDTPW | WDTHOLD; // Stop WDT
	PM5CTL0 &= ~LOCKLPM5;
	P1DIR |= redLED; // Pins as output
	P9DIR |= greenLED;
	P1OUT |= redLED; // Red on
	P9OUT &= ~greenLED; // Green off

	// ** Button Setup **
	P1DIR &= ~(BUT1|BUT2);  //0: Direct pin as input
	P1REN |= (BUT1|BUT2);   //1: Enable built-in resistor
	P1OUT |= (BUT1|BUT2);   //1: Set resistor as pull-up
	P1IES |= (BUT1|BUT2);   //1: interrupt on falling edge (0 for rising edge)
	P1IFG &= ~(BUT1|BUT2);  //0: clear the interrupt flags
	P1IE  |= (BUT1|BUT2);   //1: enable the interrupts
	
	// *** Timer Module Setup ***
	// Configure Channel 0 for up mode with interrupts
	TA0CCR0 = 32768; // 1 second @ 32 KHz
	TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
	TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

	//Configure Timer_A: divide by 1, up mode, TAR cleared, enable interrupt for rollback-to-zero
	TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR ; // use ACLK, divide by 1, continous mode, clear TAR
	// TAIE is not set to 1 because we are using a different flag (CCIE, which compares TAR to TACCR0)

	// Enable the global interrupt bit
	__enable_interrupt();
	
	// configure ACLK to the 32KHz crystal
 	config_ACLK_to_32KHz_crystal();

	// Initializes the LCD_C module
	Initialize_LCD();
	LCDCMEMCTL = LCDCLRM; // Clears all the segments
	LCDM3 |= exclaim;
	LCDM3 |= timer;
	LCDM7 |= colon;
	//Flash the red LED
	for(;;){	
		// Display 16-bit Unsigned Number
		lcd_write_uint16(counter); // update num on display
	}

}

// ** ISR for Part 2**
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
  // Interrupt response goes here
  counter++; // increase number on display by 1 every second
  // hardware clears the flag
}

// ** ISR for Part 2 **
#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void P1_ISR() {
  // If S1 is pressed, reset counter
  if((P1IFG & BUT1) !=0 ) {
    counter = 0;
	TA0R = 0;
    P1IFG &= ~BUT1; // clear button 1 interrupt flag
    __delay_cycles(500000);
  }
  // If S2 is pressed, add 1000
  if((P1IFG & BUT2) !=0 ) {
    counter += 1000;
    P1IFG &= ~BUT2; // clear button 2 interrupt flag
    __delay_cycles(500000);
  }
}



