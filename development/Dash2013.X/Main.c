/* 
 * File:   Main.c
 * Author: David
 *
 * Created on March 19, 2013, 8:42 PM
 */
#include "interrupts.h"
#include "CanDash.h"
#include "can.h"
#include "CANDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>


// PIC18F2480 Configuration Bit Settings

#include <p18F2480.h>

// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = BOHW     // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config BBSIZ = 1024     // Boot Block Size Select bit (1K words (2K bytes) boot block)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)



void main() {
    struct CANMessage CANRXMsg;
    unsigned char byte1, byte2;
    int battv;
    float batt;
    char batts[50];

    TRISBbits.RB3 = 1;
    TRISBbits.RB4 = 0;
    TRISBbits.RB5 = 0;
    LATBbits.LATB4 = 0;
    LATBbits.LATB5 = 0;
    //OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT &
    //        USART_CONT_RX & USART_BRGH_LOW, 0x33);


    PIE3bits.RXB0IE = 1;
    PORTBbits.RB4 = 0;
    PORTBbits.RB5 = 0;

    CANInit();
    
    enable_interrupts();
   
    while(1)
    {
        PORTBbits.RB4 = 1;
        if(CANRXMessageIsPending())
        {
            
            CANRXMsg = CANGet();
            if (CANRXMsg.Address == 1520)
            {
                byte1 = CANRXMsg.Data[4];
                byte2 = CANRXMsg.Data[5];
                battv = (byte1 << 8) + byte2;
                batt = battv * 0.1;
                //sprintf(batts, "Bat: %f V.", batt);
                //putsUSART(batts);
            }

        }
//        else
//            PORTBbits.RB5 = 1;
    }

}

