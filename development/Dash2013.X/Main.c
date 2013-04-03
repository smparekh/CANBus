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
#include <p18cxxx.h>
#include <usart.h>
#include <stdio.h>

/*
 * 
 */
void main() {
    struct CANMessage CANRXMsg;
    unsigned char byte1, byte2;
    int battv;
    float batt;
    char batts[50];

    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT &
            USART_CONT_RX & USART_BRGH_LOW, 0x33);


    PIE3bits.RXB0IE = 1;

    CANInit();
    enable_interrupts();
    while(1)
    {
        if(CANRXMessageIsPending())
        {
            CANRXMsg = CANGet();
            if (CANRXMsg.Address == 1523)
            {
                byte1 = CANRXMsg.Data[4];
                byte2 = CANRXMsg.Data[5];
                battv = (byte1 << 8) + byte2;
                batt = battv * 0.1;
                sprintf(batts, "Bat: %f V.", batt);
                putsUSART(batts);
            }
        }
    }

}

