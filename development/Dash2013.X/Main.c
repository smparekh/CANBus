/* 
 * File:   Main.c
 * Author: David
 *
 * Created on March 19, 2013, 8:42 PM
 */
#include "CanDash.h"
#include "can.h"
#include "CANDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <p18cxxx.h>

/*
 * 
 */
void main() {
    initialize_can();
    while(1)
    {
     struct CANMessage canmessage;
     canmessage = CANGet();

    }

}

