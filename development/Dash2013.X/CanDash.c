//Returns an int ( 1 representing that the can is not in config mode , 0 the can is in config mode.)
#include <p18cxxx.h>
#include "CanDash.h"
#include "can.h"



int GetCanMode(int canMode)
{
    if(canMode == CAN_CONFIG_MODE)
    {
    if(CANSTATbits.OPMODE2 != 1)
        return 0;
    if(CANSTATbits.OPMODE1 != 0)
        return 0;
    if( CANSTATbits.OPMODE0 != 0)
        return 0;

     return 1;
    }
    if(canMode == CAN_NORMAL_MODE)
    {
    if(CANSTATbits.OPMODE2 != 0)
        return 0;
    if(CANSTATbits.OPMODE1 != 0)
        return 0;
    if( CANSTATbits.OPMODE0 != 0)
        return 0;

     return 1;


    }
}
void SetCanConfigMode(int configmode)
{
 //Set bits
 if(configmode == CAN_CONFIG_MODE)
 {
 CANSTATbits.OPMODE2 = 1;
 CANSTATbits.OPMODE1 = 0;
 CANSTATbits.OPMODE0 = 0;
 while(!GetCanMode(CAN_CONFIG_MODE));
 }
 else if(configmode == CAN_NORMAL_MODE)
 {
 CANSTATbits.OPMODE2 = 0;
 CANSTATbits.OPMODE1 = 0;
 CANSTATbits.OPMODE0 = 0;
 while(!GetCanMode(CAN_NORMAL_MODE));
 }
 //While loop

}
void initialize_can(void)
{
     CANOpen(0x00,0xbc,0x01); //set it to 1Mbits per sec
     CANISR();
     //Set Interupts
     SetCanConfigMode(CAN_NORMAL_MODE);
}

