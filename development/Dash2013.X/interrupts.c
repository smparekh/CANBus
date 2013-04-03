#include "can.h"
#include "interrupts.h"
#include <p18f2480.h>

void enable_interrupts(void)
{
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 0;
    return;
}

int in_high_int() {
    return(!INTCONbits.GIEH);
}

int in_low_int() {
    if (INTCONbits.GIEL == 1)
    {
        return 0;
    } else if (in_high_int())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void InterruptHandlerHigh();
void InterruptHandlerLow();

#pragma code InterruptVectorLow = 0x18

void InterruptVectorLow(void) {
    _asm
    goto InterruptHandlerLow
    _endasm
}

#pragma code InterruptVectorHigh = 0x08

void InterruptVectorHigh(void)
{
    _asm
    goto InterruptHandlerHigh
    _endasm
}

#pragma code
#pragma interrupt InterruptHandlerHigh

void InterruptHandlerHigh()
{
    CANISR();
}

#pragma code
#pragma interrupt InterruptHandlerLow
void InterruptHandlerLow()
{
}