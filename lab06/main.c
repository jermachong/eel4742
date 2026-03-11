/*
 * EEL 4742
 * Jeremy Achong
 * Lab 6 - Universal Asyncghro
 */
#include <msp430fr6989.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FLAGS UCA1IFG      // Contains the transmit & receive flags
#define RXFLAG UCRXIFG     // Receive flag
#define TXFLAG UCTXIFG     // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

#define redLED BIT0   // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1     // Button S1 at P1.1
#define BUT2 BIT2     // Button S2 at P1.2
#define exclaim BIT0  // exclamaition mark
#define timer BIT3    // timer icon
#define decimal BIT0  // decimal icon
#define colon BIT2;   // colon icon

void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);

void uart_write_uint16(unsigned int n);
void uart_write_string(char *str);

// Part 6.1: Transmitting Data over UART
// int main(void)
// {
// 	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
//     PM5CTL0 &= ~LOCKLPM5;
// 	char* arr = "0123456789";
// 	short int ARR_LEN = 10;
// 	int i = 0;
// 	char result = 0;
// 	// Configure pins to backchannel UART
// 	// Pins: (UCA1TXD / P3.4) (UCA1RXD / P3.5)
// 	// (P3SEL1=00, P3SEL0=11) (P2DIR=xx)
// 	P3SEL1 &= ~(BIT4|BIT5);
// 	P3SEL0 |= (BIT4|BIT5);
// 	P1DIR |= redLED; // Pins as output
//     P9DIR |= greenLED;
//     P1OUT &= ~redLED; // Red on
//     P9OUT &= ~greenLED; // Green off

// 	Initialize_UART();

// 	for(;;){
// 		P1OUT ^= redLED; // blink LED

// 		uart_write_char(arr[i]);

// 		result = uart_read_char(); // listen for input
// 		if (result == '1') // on green LED
// 			P9OUT |= greenLED;
// 		else if (result == '2') // off green LED
// 			P9OUT &= ~greenLED;

// 		i = (i + 1) % ARR_LEN;
// 		__delay_cycles(750000);
// 	}

// 	return 0;
// }

// Part 6.2 Transmitting Integers & Strings over UART
int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5;

  char *short_str = "hello world";
  char *long_str = "YXACV5VN0MI5GRMX0YAZGWQBB3FLUP9D";
  unsigned int short_num = 1337;
  unsigned int long_num = 65535; // will roll over if value is above 65545

  // Configure pins to backchannel UART
  // Pins: (UCA1TXD / P3.4) (UCA1RXD / P3.5)
  // (P3SEL1=00, P3SEL0=11) (P2DIR=xx)
  P3SEL1 &= ~(BIT4 | BIT5);
  P3SEL0 |= (BIT4 | BIT5);
  P1DIR |= redLED; // Pins as output
  P9DIR |= greenLED;
  P1OUT &= ~redLED;   // Red on
  P9OUT &= ~greenLED; // Green off

  Initialize_UART();

  uart_write_uint16(short_num);
  uart_write_uint16(long_num);

  uart_write_string(short_str);
  uart_write_string(long_str);

  for (;;) {
    P1OUT ^= redLED; // blink LED
    __delay_cycles(750000);
  }

  return 0;
}

// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control, oversampling reception
// Clock: SMCLK @ 1 MHz (1,000,000 Hz)
void Initialize_UART(void) {
  // Configure pins to UART functionality
  P3SEL1 &= ~(BIT4 | BIT5);
  P3SEL0 |= (BIT4 | BIT5);
  // Main configuration register
  UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
  // Most fields in this register, when set to zero, correspond to the
  // popular configuration
  UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
  // Configure the clock dividers and modulators (and enable oversampling)
  UCA1BRW = 6; // divider
  // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
  // UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
  UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;
  // Exit the reset state
  UCA1CTLW0 &= ~UCSWRST;
}

// transmits a byte over UART
void uart_write_char(unsigned char ch) {
  // Wait for any ongoing transmission to complete
  while ((FLAGS & TXFLAG) == 0) {
  }
  // Copy the byte to the transmit buffer
  TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
  return;
}

// The function returns the byte; if none received, returns null character
unsigned char uart_read_char(void) {
  unsigned char temp;
  // Return null character (ASCII=0) if no byte was received
  if ((FLAGS & RXFLAG) == 0)
    return 0;
  // Otherwise, copy the received byte (this clears the flag) and return it
  temp = RXBUFFER;
  return temp;
}

// Function transmits 16-bit unsigned integers
void uart_write_uint16(unsigned int n) {
  char str[6]; // 5 digits + null term
  int i = 0, j, k = 0;

  // manually convert to string
  if (n == 0) {
    str[i++] = '0';
  } else
    while (n > 0) {
      str[i++] = n % 10 + '0';
      n /= 10;
    }

  str[i] = '\0';

  // Reverse the string to get the correct order
  for (j = 0, k = i - 1; j < k; j++, k--) {
    char temp = str[j];
    str[j] = str[k];
    str[k] = temp;
  }

  uart_write_string(str); // output made string
}

// Function transmits each character of the string over UART.
void uart_write_string(char *str) {
  int i = 0;
  int n = strlen(str); // get length of input string
  if (n == 0)          // skip if empty
    return;
  for (i = 0; i < n; i++) { // loop through chars abnd print
    uart_write_char(str[i]);
  }
  uart_write_char("\n");
}