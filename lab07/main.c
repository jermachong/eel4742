/*
 * EEL 4742
 * Jeremy Achong
 * Lab 7 - Inter-Integrated Circuit (I2C) Communication
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
#define BUT1 BIT1          // Button S1 at P1.1
#define BUT2 BIT2          // Button S2 at P1.2

void Initialize_I2C();
void Initialize_UART();
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg,
                  unsigned int *data);
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg,
                   unsigned int data);
void uart_write_uint16(unsigned int n);
void uart_write_char(unsigned char ch);
void uart_write_string(char *str);

// Part 7.1: Reading the Manufacturer ID and Device ID Registers
// int main() {
//   WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;

//   Initialize_UART();
//   Initialize_I2C();

//   // Reading two bytes from register 0x50 on I2C device 0x22
//   unsigned int man_data = 0, dev_data = 0;

//   uart_write_char('t'); // test

//   for (;;) {
//     i2c_read_word(0x44, 0x7E, &man_data);
//     i2c_read_word(0x44, 0x7F, &dev_data);
//     uart_write_uint16(man_data);
//     uart_write_uint16(dev_data);
//     __delay_cycles(125000);
//   }

//   return 0;
// }

// Part 7.2 Reading Measurments from the Light Sensor
// int main() {
//   WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
//   PM5CTL0 &= ~LOCKLPM5;

//   Initialize_UART();
//   Initialize_I2C();

//   unsigned int light_data = 0;

//   // RN=0111b=7 The LSB bit is worth 1.28
//   // CT=0 Result produced in 100 ms
//   // M=11b=3 Continuous readings
//   // ME=1 Mask (hide) the Exponent from the result
//   //  RN  CT M   ...        ME
//   //   |  |  |              |
//   //   v  v  v              v
//   // 0111 0  11 0 0 0 0 1 0 1 00 = 0x7614
//   i2c_write_word(0x44, 0x01, 0x7614);

//   for (;;) {
//     i2c_read_word(0x44, 0x00, &light_data);
//     uart_write_uint16(light_data * 1.28); // multiply by 1.28 for the lux
//     value
//     __delay_cycles(1000000);
//   }

//   return 0;
// }

// Part 7.3: Lux Logger
int main() {
  WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5;

  Initialize_UART();
  Initialize_I2C();

  // ** Button Setup **
  P1DIR &= ~(BUT2); // 0: Direct pin as input
  P1REN |= (BUT2);  // 1: Enable built-in resistor
  P1OUT |= (BUT2);  // 1: Set resistor as pull-up
  P1IES |= (BUT2);  // 1: interrupt on falling edge (0 for rising edge)
  P1IFG &= ~(BUT2); // 0: clear the interrupt flags
  P1IE |= (BUT2);   // 1: enable the interrupts

  // RN=0111b=7 The LSB bit is worth 1.28
  // CT=0 Result produced in 100 ms
  // M=11b=3 Continuous readings
  // ME=1 Mask (hide) the Exponent from the result
  //  RN  CT M   ...        ME
  //   |  |  |              |
  //   v  v  v              v
  // 0111 0  11 0 0 0 0 1 0 1 00 = 0x7614
  i2c_write_word(0x44, 0x01, 0x7614);

  unsigned int light_data = 0, lux = 0;
  i2c_read_word(0x44, 0x00, &light_data);
  lux = light_data * 1.28;
  unsigned int light_max = lux + 10, light_min = lux - 10;

  char *time = "12:00\t";
  uart_write_string("*** Lux Logger ***\n\n");

  for (;;) {
    i2c_read_word(0x44, 0x00, &light_data); // get reading
    lux = light_data * 1.28;
    uart_write_string(time);
    uart_write_uint16(lux);

    if (lux > light_max) {
      uart_write_string("\t<Up>");
      light_max = lux;
    } else if (lux < light_min) {
      uart_write_string("\t<Down>");
      light_min = lux;
    }
    uart_write_char('\n');

    __delay_cycles(1000000);
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

void Initialize_I2C() {
  // Configure the MCU in Master mode
  // Configure pins to I2C functionality
  // (UCB1SDA same as P4.0) (UCB1SCL same as P4.1)
  // (P4SEL1=11, P4SEL0=00) (P4DIR=xx)
  P4SEL1 |= (BIT1 | BIT0);
  P4SEL0 &= ~(BIT1 | BIT0);
  // Enter reset state and set all fields in this register to zero
  UCB1CTLW0 = UCSWRST;
  // Fields that should be nonzero are changed below
  // (Master Mode: UCMST) (I2C mode: UCMODE_3) (Synchronous mode: UCSYNC)
  // (UCSSEL 1:ACLK, 2,3:SMCLK)
  UCB1CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_3;
  // Clock frequency: SMCLK/8 = 1 MHz/8 = 125 KHz
  UCB1BRW = 8;
  // Chip Data Sheet p. 53 (Should be 400 KHz max)
  // Exit the reset mode at the end of the configuration
  UCB1CTLW0 &= ~UCSWRST;
}

////////////////////////////////////////////////////////////////////
// Read a word (2 bytes) from I2C (address, register)
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg,
                  unsigned int *data) {
  unsigned char byte1 = 0, byte2 = 0; // Intialize to ensure successful reading
  UCB1I2CSA = i2c_address;            // Set address
  UCB1IFG &= ~UCTXIFG0;
  // Transmit a byte (the intßernal register address)
  UCB1CTLW0 |= UCTR;
  UCB1CTLW0 |= UCTXSTT;
  while ((UCB1IFG & UCTXIFG0) == 0) {
  } // Wait for flag to raise
  UCB1TXBUF = i2c_reg; // Write in the TX buffer
  while ((UCB1IFG & UCTXIFG0) == 0) {
  } // Buffer copied to shift register; Tx in progress; set Stop bit
  // Repeated Start
  UCB1CTLW0 &= ~UCTR;
  UCB1CTLW0 |= UCTXSTT;
  // Read the first byte
  while ((UCB1IFG & UCRXIFG0) == 0) {
  } // Wait for flag to raise
  byte1 = UCB1RXBUF;
  // Assert the Stop signal bit before receiving the last byte
  UCB1CTLW0 |= UCTXSTP;
  // Read the second byte
  while ((UCB1IFG & UCRXIFG0) == 0) {
  } // Wait for flag to raise
  byte2 = UCB1RXBUF;
  while ((UCB1CTLW0 & UCTXSTP) != 0) {
  }
  while ((UCB1STATW & UCBBUSY) != 0) {
  }
  *data = (byte1 << 8) | (byte2 & (unsigned int)0x00FF);
  return 0;
}

////////////////////////////////////////////////////////////////////
// Write a word (2 bytes) to I2C (address, register)
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg,
                   unsigned int data) {
  unsigned char byte1, byte2;
  UCB1I2CSA = i2c_address;    // Set I2C address
  byte1 = (data >> 8) & 0xFF; // MSByte
  byte2 = data & 0xFF;        // LSByte
  UCB1IFG &= ~UCTXIFG0;
  // Write 3 bytes
  UCB1CTLW0 |= (UCTR | UCTXSTT);
  while ((UCB1IFG & UCTXIFG0) == 0) {
  }
  UCB1TXBUF = i2c_reg;
  while ((UCB1IFG & UCTXIFG0) == 0) {
  }
  UCB1TXBUF = byte1;
  while ((UCB1IFG & UCTXIFG0) == 0) {
  }
  UCB1TXBUF = byte2;
  while ((UCB1IFG & UCTXIFG0) == 0) {
  }
  UCB1CTLW0 |= UCTXSTP;
  while ((UCB1CTLW0 & UCTXSTP) != 0) {
  }
  while ((UCB1STATW & UCBBUSY) != 0) {
  }
  return 0;
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
}

// ** ISR for Part 3 **
#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void P1_ISR() {
  // If S2 is pressed, set time
  if ((P1IFG & BUT2) != 0) {

    __delay_cycles(500000);
  }
}
