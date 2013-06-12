#include "GenericTypeDefs.h"
#include <xc.h>
#include "I2C.h"
#include "LCD.h"
#include "can_func.h"
#include "timer_func.h"
#include <plib.h>
#ifndef OVERRIDE_CONFIG_BITS

    #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_8         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx1      // ICE/ICD Comm Channel Select
    #pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit
    #pragma config FCANIO   = OFF
#endif // OVERRIDE_CONFIG_BITS

int main()
{
    INTEnableSystemMultiVectoredInt();
    initI2C();
    initLCD();
    
//    CAN1Init();
    initUART();
    Timer1Init();
    mPORTGDirection(0x0);
    mPORTGClearBits(BIT_12);
    mPORTGClearBits(BIT_13);
    mPORTGClearBits(BIT_14);
    mPORTGClearBits(BIT_15);
    
    while (1)
    {
//        CAN1RxMsgProcess();
    }
    return 0;
}