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
unsigned char uart_read_char();
void increment_time(char input_time[]);
void update_time(char input_time[]);
int validate_time(char input_time[]);
void config_ACLK_to_32KHz_crystal();

char time[] = "12:00\t";
volatile int button_pressed = 0; // flag for button press
volatile int log_ready = 1;      // Start at 1 to log immediately on boot
volatile int timer_seconds = 0;  // Counts the 1-second timer ticks

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

  // *** Timer Module Setup ***
  // Configure Channel 0 for up mode with interrupts
  TA0CCR0 = 32768;    // 1 second @ 32 KHz
  TA0CCTL0 |= CCIE;   // Enable Channel 0 CCIE bit
  TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

  // Configure Timer_A: divide by 1, up mode, TAR cleared, enable interrupt for
  // rollback-to-zero
  TA0CTL = TASSEL_1 | ID_0 | MC_1 |
           TACLR; // use ACLK, divide by 1, up mode, clear TAR

  // RN=0111b=7 The LSB bit is worth 1.28
  // CT=0 Result produced in 100 ms
  // M=11b=3 Continuous readings
  // ME=1 Mask (hide) the Exponent from the result
  //  RN  CT M   ...        ME
  //   |  |  |              |
  //   v  v  v              v
  // 0111 0  11 0 0 0 0 1 0 1 00 = 0x7614
  i2c_write_word(0x44, 0x01, 0x7614);

  unsigned int light_data = 0;
  unsigned int lux = 0;
  i2c_read_word(0x44, 0x00, &light_data);
  lux = light_data * 1.28;
  unsigned int light_max = lux + 10;
  unsigned int light_min = 0;
  if ((light_data * 1.28) - 10 > 0)
    light_min = lux - 10;

  // Write header
  uart_write_string("*** Lux Logger ***\n\n");
  __enable_interrupt();
  config_ACLK_to_32KHz_crystal();

  for (;;) {

    // 1. Did the timer say it's time to log?
    if (log_ready == 1) {
      increment_time(time);
      // collect reading from sensor and convert to lux
      i2c_read_word(0x44, 0x00, &light_data);
      lux = light_data * 1.28;

      // print out timestamp and lux measurement
      uart_write_string(time);
      uart_write_uint16(lux);

      // check if boundaries were changed
      if (lux > light_max) {
        uart_write_string("\t<Up>");
        light_max = lux;
      } else if (lux < light_min) {
        uart_write_string("\t<Down>");
        light_min = lux;
      }
      uart_write_string("\r\n");

      log_ready = 0; // Reset flag until next timer cycle
    }

    if (button_pressed == 1) {
      update_time(time);
      button_pressed = 0; // Reset flag
    }

    // 3. (Optional) Go to sleep here!
    // Since everything is interrupt-driven now, you could put the
    // CPU into Low Power Mode here to save battery. The next ISR will wake it
    // up.
  }
  return 0;
}

// make sure the time string is in 12-hour format
int validate_time(char input_time[]) {
  if (input_time[0] > '1')
    return 0;
  if (input_time[1] > '2')
    return 0;
  if (input_time[3] > '5')
    return 0;

  return 1; // valid
}

// ask the user to input a new timestamp
void update_time(char input_time[]) {
  uart_write_string("Enter the time... (3 or 4 digits then hit Enter)\r\n");

  char temp_buf[4]; // Temporary bucket to hold up to 4 keystrokes
  int i = 0;
  char t;

  while (1) {
    t = uart_read_char(); // Read from UART

    if (t != 0) { // ONLY act if a real character was received

      // Check if the user hit Enter (Carriage Return or Line Feed)
      if (t == '\r' || t == '\n') {
        break; // Exit the while loop
      }

      // If it's a valid number, save it
      if (t >= '0' && t <= '9') {
        temp_buf[i] = t;
        i++;
      }

      // If they typed 5 numbers, automatically stop (no need to press enter)
      if (i > 4) {
        break;
      }
    }
  }
  if (i == 4) {
    input_time[0] = temp_buf[0];
    input_time[1] = temp_buf[1];
    input_time[3] = temp_buf[2];
    input_time[4] = temp_buf[3];
  } else {
    input_time[0] = '0';
    input_time[1] = temp_buf[0];
    input_time[3] = temp_buf[1];
    input_time[4] = temp_buf[2];
  }
  // if the time formatting is wrong, have user retype
  if (validate_time(input_time) == 0)
    update_time(input_time);
  uart_write_string("Time is set to "), uart_write_string(time);
  uart_write_string("\r\n");
  timer_seconds = 0;
}

// update time by 1 minute
void increment_time(char input_time[]) {
  // Increment the minute ones-digit directly
  input_time[4]++;

  // Cascade the carry-overs
  if (input_time[4] > '9') {
    input_time[4] = '0'; // Reset ones
    input_time[3]++;     // Increment tens

    if (input_time[3] > '5') {
      input_time[3] = '0'; // Reset minute tens
      input_time[1]++;     // Increment hour ones

      // Handle 09:59 -> 10:00
      if (input_time[1] > '9') {
        input_time[1] = '0';
        input_time[0]++;
      }

      // Handle 12:59 -> 01:00 (12-hour format logic)
      if (input_time[0] == '1' && input_time[1] == '3') {
        input_time[0] = '0';
        input_time[1] = '1';
      }
    }
  }
}

// ** ISR for Part 3 **
#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void P1_ISR() {
  // Check if S2 caused the interrupt
  if ((P1IFG & BUT2) != 0) {
    button_pressed = 1; // trigger update_time
    P1IFG &= ~BUT2;     // clear flag
  }
}

// ** ISR for Timer_A0 **
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void) {

  timer_seconds++; // Count 1 second

  // Check if we hit our target (e.g., 60 seconds)
  if (timer_seconds >= 60) {
    log_ready = 1;     // Ring the bell for the main loop!
    timer_seconds = 0; // Reset the counter
  }
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
  unsigned char byte1 = 0,
                byte2 = 0; // Intialize to ensure successful reading
  UCB1I2CSA = i2c_address; // Set address
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
  unsigned int i = 0, j, k = 0;

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

// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
  // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
  // Reroute pins to LFXIN/LFXOUT functionality
  PJSEL1 &= ~BIT4;
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
