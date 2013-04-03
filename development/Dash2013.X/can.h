/*********************************************************************
 *
 *                           This is the header for CAN.C
 *   
 *********************************************************************
 * FileName:        CAN.H
 * Dependencies:    CANDef.h
 * Processor:       PIC18FXX8
 * Compiler:        MCC18 v2.20 or higher
 *                  HITECH PICC-18 v8.20PL4 or higher
 * Linker:          
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company’s
 * customer, for use solely and exclusively with products manufactured
 * by the Company. 
 *
 * The software is owned by the Company and/or its supplier, and is 
 * protected under applicable copyright laws. All rights are reserved. 
 * Any use in violation of the foregoing restrictions may subject the 
 * user to criminal sanctions under applicable laws, as well as to 
 * civil liability for the breach of the terms and conditions of this 
 * license.
 
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Thomas Castmo        06/07/2003  Initial release
 * Thomas Castmo        07/07/2003  Cleared up a bit and implemented 
 *                                  the window function CANCON<3:1> 
 *                                  for interrupts
 * Thomas Castmo        16/07/2003  Added support for the Microchip
 *                                  MPLAB C18 compiler
 * 
 ********************************************************************/
#ifndef __CAN_H
#define __CAN_H

#include "candef.h"

#define CANInit() CANOpen(CAN_CONFIG_1, CAN_CONFIG_2, CAN_CONFIG_3)

#define MASK_ENABLE_ALL 0x00000000			//The mask to enable all incoming messages

#define ACCEPTANCE_MASK_0_1 RXMASK0			//Acceptance mask for filters 0 and 1
#define ACCEPTANCE_MASK_2_5 RXMASK1			//Acceptance mask for filters 2, 3, 4 and 5

//                          0x1FFFFFFF      Maximum extended identifier
//                          0x000007FF      Maximum standard identifier
#define ACCEPTANCE_FILTER_0 RXFILT0			//Acceptance filter 0
#define ACCEPTANCE_FILTER_1 RXFILT1			//Acceptance filter 1

//                          0x1FFFFFFF      Maximum extended identifier
//                          0x000007FF      Maximum standard identifier
#define ACCEPTANCE_FILTER_2 RXFILT2			//Acceptance filter 2
#define ACCEPTANCE_FILTER_3 RXFILT3			//Acceptance filter 3
#define ACCEPTANCE_FILTER_4 RXFILT4			//Acceptance filter 4
#define ACCEPTANCE_FILTER_5 RXFILT5			//Acceptance filter 5

//If a desired filter is to accept extended identifiers
//then comment the appropriate line out
#ifdef ST_FILTER_0
#define ACCEPT_STANDARD_FILTER_0
#endif
#ifdef ST_FILTER_1
#define ACCEPT_STANDARD_FILTER_1
#endif
#ifdef ST_FILTER_2
#define ACCEPT_STANDARD_FILTER_2
#endif
#ifdef ST_FILTER_3
#define ACCEPT_STANDARD_FILTER_3
#endif
#ifdef ST_FILTER_4
#define ACCEPT_STANDARD_FILTER_4
#endif
#ifdef ST_FILTER_5
#define ACCEPT_STANDARD_FILTER_5
#endif




//                            0x1FFFFFFF      Maximum extended identifier
//                            0x000007FF      Maximum standard identifier
#define MY_ADDRESS_IDENTIFIER MY_IDENT

//To use extended identifier for my address, comment the following line out
#ifdef STD_IDENT
#define MY_ADDRESS_IS_STANDARD
#endif


//If using normal mode, comment the following line out
#ifdef LPBACK
#define USE_LOOPBACK_MODE
#endif


//Size of RX buffer (14 bytes per buffer entry) has to be greater than or equal to 2
#define RXBUF RX_BUFFER

//Size of TX buffer (14 bytes per buffer entry) has to be greater than or equal to 2
#define TXBUF TX_BUFFER

#if TXBUF < 2 || RXBUF < 2
#error "The RXbuffer and TXbuffer has to greater than or equal to 2"
#endif
	
//CAN message structure (one message is 15 bytes wide)
struct CANMessage {
	unsigned long Address;
	unsigned char Data[8];
	unsigned char NoOfBytes;
	unsigned char Priority;
	unsigned Ext:1;
	unsigned Remote:1;
};

#define CAN_LISTEN_MODE 0x7F 
#define CAN_LOOPBACK_MODE 0x5F 
#define CAN_DISABLE_MODE 0x3F  
#define CAN_NORMAL_MODE 0x1F



/*********************************************************************
 * Function:        char CANOpen(unsigned char CONFIG1, unsigned char CONFIG2, unsigned char CONFIG3)
 *
 * PreCondition:    None
 *
 * Input:           Values to be written into BRGCON1 -> BRGCON3
 *
 * Output:          0 -> Initialasation succeeded
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the appropriate register for the device to act
 *                  as a CAN node
 *
 * Note:            Input values 0x03, 0xAA, 0x05 at Fosc = 16MHz works with 
 *                  the default firmware at nodeB on the CAN I/O expander board.
 ********************************************************************/
char CANOpen(unsigned char, unsigned char, unsigned char);	


/*********************************************************************
 * Function:        void CANISR(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Will modify the RX/TX Buffer registers´ Write/Read pointers
 *
 * Overview:        Checks if a CAN reception/transmission was complete 
 *                  and if so write/read to the CAN RX/TX FIFO buffers
 *
 * Note:            This function is supposed to be called from the ISR
 ********************************************************************/
void CANISR(void);


/*********************************************************************
 * Function:        char CANPut(struct CANMessage Message)
 *
 * PreCondition:    None
 *
 * Input:           A CAN message
 *
 * Output:          1 -> Failed to put a CAN on the buffer, buffer is full
 *                  0 -> The CAN message is put on the buffer and will be 
 *                       transmitted eventually 
 *
 * Side Effects:    Will modify the TX Buffer register´s Write pointer
 *
 * Overview:        Initially checks if at least one buffer slot is available
 *                  and if so push the requested message in the buffer
 *
 * Note:            None
 ********************************************************************/
char CANPut(struct CANMessage);


/*********************************************************************
 * Function:        char CANRXMessageIsPending(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          1 -> At least one received message is pending in the RX buffer
 *                  0 -> No received messages are pending
 *
 * Side Effects:    None
 *
 * Overview:        Checks if the RX Write pointer is equal to RX Read pointer and
 *                  if so returns 0, else returns 1
 *
 * Note:            Since no care is taken if the buffer overflow
 *                  this function has to be polled frequently to
 *                  prevent a software receive buffer overflow
 ********************************************************************/
char CANRXMessageIsPending(void);


/*********************************************************************
 * Function:        struct CANMessage CANGet(void)
 *
 * PreCondition:    An unread message has to be in the buffer
 *                  use RXCANMessageIsPending(void) prior to 
 *                  calling this function in order to determine
 *                  if an unread message is pending.
 *
 * Input:           None
 *
 * Output:          The received message
 *
 * Side Effects:    Will modify the RX Buffer register´s Read pointer
 *
 * Overview:        Pops the the first message of the RX buffer
 *
 * Note:            None
 ********************************************************************/
struct CANMessage CANGet(void);


/*********************************************************************
 * Function:        void CANSetMode(unsigned char Mode)
 *
 * PreCondition:    None
 *
 * Input:           Desired CAN Mode
 *                  (CAN_LISTEN_MODE, CAN_LOOPBACK_MODE
 *                   CAN_DISABLE_MODE, CAN_NORMAL_MODE)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Requests to set the desired mode and waits until
 *					the mode has been set.
 *
 * Note:            If USE_LOOPBACK_MODE is defined the requested
 *                  mode will be loopback mode if the input is
 *                  Normal mode
 ********************************************************************/
void CANSetMode(unsigned char);

#endif
