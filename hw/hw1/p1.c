/*
    J. Achong
    HW1
    For the questions below, write the code using the masks that are pre-defined in the header file. (E.g:
    BIT0:0000 0001, BIT1:0000 0010, …, BIT7:1000 0000).
    Perform the operations below on the 8-bit variable (uint_8t data).
*/
#include <stdint.h>
#include <stdio.h>

// #define BIT0 0x01  // 0000 0001
#define BIT1 0b00000010 // 0000 0010
#define BIT2 0b00000100 // 0000 0100
#define BIT5 0b00100000 // 0010 0000
#define BIT6 0b01000000 // 0100 0000
#define BIT7 0b10000000 // 1000 0000

// int main() {
//     uint8_t data = 0b00000000; // Initial value

//     data |= BIT5;  // set bit 5

//     data &= ~BIT5; // clear bit 5

//     data ^= BIT5;  // toggle bit 5

//     return 0;
// }

// int main() {
//     uint8_t data = 0b00000000; // Initial value

//     data |= BIT6;  // set bit 6 and 7
//     data |= BIT7;

//     data &= ~BIT6; // clear bit 6 and 7
//     data &= ~BIT7;

//     data ^= BIT6;  // toggle bit 6 and 7
//     data ^= BIT7;

//     data |= BIT6; // set bit 6
//     data &= ~BIT7; // clear bits 7

//     // Print the final value of data
//     printf("Final value of data: %d\n", data);

//     return 0;
// }

int main()
{
    uint8_t data = 0b00000000; // Initial value

    if ((data & BIT2) == 0) // check if bit 2 is 0
        printf("Bit 2 is 0\n");
    else
        printf("Bit 2 is 1\n");

    // check if bit 5 and 6 are 1
    if (((data & BIT5) == 1) && ((data & BIT6) == 1))
        printf("Bit 5 and Bit 6 are 1\n");
    else
        printf("Bit 5 and Bit 6 are not both 1\n");

    if ((data & BIT5) == 0) // check if bit 5 is 0
    {
        if ((data & BIT6) == 1) // check if bit 6 is 1
            printf("Bit 5 is 0 and Bit 6 is 1\n");
        else
            printf("Bit 5 is 0 and Bit 6 is 0\n");
    }

    return 0;
}