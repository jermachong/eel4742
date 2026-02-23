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

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1)
    {
        // Toggle P1.0 output
        GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
			GPIO_PIN0
			);

        // Delay
        for(i=10000; i>0; i--);
    }
}
