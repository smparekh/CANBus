/*********************************************************************
 *
 *   This file contains CAN drivers for PIC18Fxx8 devices
 *   
 *********************************************************************
 * FileName:        CAN.C
 * Dependencies:    CAN.H, PIC18.H or P18CXXX.H
 * Processor:       PIC18FXX8
 * Compiler:        MCC18 v2.20 or higher
 *                  HITECH PICC-18 v8.20PL4 or higher
 * Linker:          
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company뭩
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
#include "can.h"

#if defined(HI_TECH_C)
    #if defined(_MPC_)
        #define HITECH_C18
    #else
        #error "Unknown part is selected."
    #endif
#else
    #define MCHP_C18
#endif

#if defined(MCHP_C18) && defined(HITECH_C18)
#error "Invalid Compiler selection."
#endif

#if !defined(MCHP_C18) && !defined(HITECH_C18)
#error "Compiler not supported."
#endif


#if defined(HITECH_C18)
#include <pic18.h>

#elif defined(MCHP_C18)
#include <p18cxxx.h>

#define TRISB2 TRISBbits.TRISB2
#define TRISB3 TRISBbits.TRISB3
#define TXB0REQ TXB0CONbits.TXREQ
#define RXB0FUL RXB0CONbits.RXFUL
#define RXB0RTRRO RXB0CONbits.RXRTRRO

#define RXB0IF PIR3bits.RXB0IF
#define RXB1IF PIR3bits.RXB1IF
#define TXB0IF PIR3bits.TXB0IF
#define TXB1IF PIR3bits.TXB1IF
#define TXB2IF PIR3bits.TXB2IF
#define ERRIF PIR3bits.ERRIF
#define WAKIF PIR3bits.WAKIF

#define RXB0IE PIE3bits.RXB0IE
#define RXB1IE PIE3bits.RXB1IE
#define TXB0IE PIE3bits.TXB0IE
#define TXB1IE PIE3bits.TXB1IE
#define TXB2IE PIE3bits.TXB2IE
#define ERRIE PIE3bits.ERRIE
#define WAKIE PIE3bits.WAKIE


#endif



#define CONFIG_MODE 0x9F
#define LISTEN_MODE 0x7F 
#define LOOPBACK_MODE 0x5F 
#define DISABLE_MODE 0x3F  
#define NORMAL_MODE 0x1F
#define MODE_MASK 0xE0

#define NoInterrupt 0x00
#define ErrorInterrupt 0x02
#define TXB2Interrupt 0x04
#define TXB1Interrupt 0x06
#define TXB0Interrupt 0x08
#define RXB1Interrupt 0x0A
#define RXB0Interrupt 0x0C
#define WakeUpInterrupt 0x0E


#ifdef CAN_ERROR_HANDLER_ENABLE
extern void CANErrorHandler(void);
#define CAN_INT_BITS 0x3F	//CAN interrupts which should be enabled, simply what PIE3 is to be loaded with (Note all TXB IE will be set regardless of this value)
#else
#define CAN_INT_BITS 0x1F	//CAN interrupts which should be enabled, simply what PIE3 is to be loaded with (Note all TXB IE will be set regardless of this value)
#endif

union RXBuffer {				//Receive buffer structure
	unsigned char Data[14];		//It has to be a bit messy/strict for
	struct {					//it to go trough the PICC18 compiler
		union {
			unsigned char Byte;
			struct {
				unsigned FILHIT0:1;
				unsigned JTOFF:1;
				unsigned RXB0DBEN:1;
				unsigned RXRTRRO:1;
				unsigned :1;
				unsigned RXM0:1;
				unsigned RXM1:1;
				unsigned RXFUL:1;
			} Bits;
		} RXBCON;
		union {
			unsigned char Byte;
		} RXBSIDH;
		union {
			unsigned char Byte;
			struct {
				unsigned EID16:1;
				unsigned EID17:1;
				unsigned :1;
				unsigned EXID:1;
				unsigned SRR:1;
				unsigned SID0:1;
				unsigned SID1:1;
				unsigned SID2:1;
			} Bits;
		} RXBSIDL;
		union {
			unsigned char Byte;
		} RXBEIDH;
		union {
			unsigned char Byte;
		} RXBEIDL;
		union {
			unsigned char Byte;
			struct {
				unsigned DLC0:1;
				unsigned DLC1:1;
				unsigned DLC2:1;
				unsigned DLC3:1;
				unsigned RB0:1;
				unsigned RB1:1;
				unsigned RXRTR:1;
				unsigned :1;
			} Bits;
		} RXBDLC;
		union {
			unsigned char Array[8];
			struct {
				unsigned char RXBD0;
				unsigned char RXBD1;
				unsigned char RXBD2;
				unsigned char RXBD3;
				unsigned char RXBD4;
				unsigned char RXBD5;
				unsigned char RXBD6;
				unsigned char RXBD7;
			} Bytes;
		} RXBD;
	} Specific;
};

union TXBuffer {					//Transmit buffer structure
	unsigned char Data[14];
	struct {
		union {
			unsigned char Byte;
			struct {
				unsigned TXPRI0:1;
				unsigned TXPRI1:1;
				unsigned :1;
				unsigned TXREQ:1;
				unsigned TXERR:1;
				unsigned TXLARB:1;
				unsigned TXABT:1;
			} Bits;
		} TXBCON;
		union {
			unsigned char Byte;
		} TXBSIDH;
		union {
			unsigned char Byte;
			struct {
				unsigned EID16:1;
				unsigned EID17:1;
				unsigned :1;
				unsigned EXIDE:1;
				unsigned :1;
				unsigned SID0:1;
				unsigned SID1:1;
				unsigned SID2:1;
			} Bits;
		} TXBSIDL;
		union {
			unsigned char Byte;
		} TXBEIDH;
		union {
			unsigned char Byte;
		} TXBEIDL;
		union {
			unsigned char Byte;
			struct {
				unsigned DLC0:1;
				unsigned DLC1:1;
				unsigned DLC2:1;
				unsigned DLC3:1;
				unsigned :1;
				unsigned :1;
				unsigned TXRTR:1;
				unsigned :1;
			} Bits;
		} TXBDLC;
		union {
			unsigned char Array[8];
			struct {
				unsigned char TXBD0;
				unsigned char TXBD1;
				unsigned char TXBD2;
				unsigned char TXBD3;
				unsigned char TXBD4;
				unsigned char TXBD5;
				unsigned char TXBD6;
				unsigned char TXBD7;
			} Bytes;
		} TXBD;
	} Specific;
};
union RXBuffer RXMessage[RXBUF];		//Received messages FIFO buffer
union TXBuffer TXMessage[TXBUF];		//Pending messages to transmit FIFO buffer
char RXRPtr = 0;						//Read pointer for RXMessage buffer
char RXWPtr = 0;						//Write pointer for RXMessage buffer
char TXRPtr = 0;						//Read pointer for TXMessage buffer
char TXWPtr = 0;						//Write pointer for TXMessage buffer


/*********************************************************************
 * Function:        void CANGetMessage(void)
 *
 * PreCondition:    <WIN2:WIN0> in the CANCON register has to set
 *                  to reflect the desired RXB registers
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Will modify the RX FIFO Write pointer (RXWPtr)
 *
 * Overview:        Gets the registers for a RXB and puts them in the
 *                  CAN Receive buffer
 *
 * Note:            Care is not taken if buffer is full
 ********************************************************************/
void CANGetMessage(void);

/*********************************************************************
 * Function:        char CANPutMessage(void)
 *
 * PreCondition:    <WIN2:WIN0> in the CANCON register has to set
 *                  to reflect the desired TXB registers
 *
 * Input:           None
 *
 * Output:          0 -> A new message has been put in the transmit queue
 *                  1 -> There was no messages in the TX buffer to send
 *
 * Side Effects:    Will modify the TX buffer큦 Read pointer (TXRPtr)
 *
 * Overview:        Checks if there is any messages to transmit and if so
 *                  place it in the registers reflected by <WIN2:WIN0> 
 *
 * Note:            None
 ********************************************************************/
 char CANPutMessage(void);

/*********************************************************************
 * Function:        char CANOpen(void)
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
char CANOpen(unsigned char CONFIG1, unsigned char CONFIG2, unsigned char CONFIG3)
{
	TRISB2 = 0;
	TRISB3 = 1;
	PIE3 = 0;										//Disable all CAN interrupts
	PIR3 = 0;										//and clear all CAN interrupt flags
	CANCON = (CONFIG_MODE & MODE_MASK) | (CANCON & 0x3F);	//Set configuration mode
	while((CANSTAT & MODE_MASK) != (CONFIG_MODE & MODE_MASK));	//Wait until config mode is set

	BRGCON1 = CONFIG1;
	BRGCON2 = CONFIG2;
	BRGCON3 = CONFIG3;
	
	RXB0CON = 0x04;					//Receive all valid messages receive buffer overflow 
	RXB1CON = 0x00;					//writes to RXB1
	
	//Set the acceptance filters for all the filters
#ifdef ACCEPT_STANDARD_FILTER_0
	RXF0SIDL = (unsigned char)(ACCEPTANCE_FILTER_0 << 5);
	RXF0SIDH = (unsigned char)(ACCEPTANCE_FILTER_0 >> 3);
#else
	RXF0EIDL = (unsigned char)(ACCEPTANCE_FILTER_0 & 0xFF);	
	RXF0EIDH = (unsigned char)((ACCEPTANCE_FILTER_0 >> 8) & 0xFF);
	RXF0SIDL = (unsigned char)((ACCEPTANCE_FILTER_0 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_0 >> 13) & 0xE0) | 0x08;
	RXF0SIDH = (unsigned char)((ACCEPTANCE_FILTER_0 >> 21) & 0xFF);
#endif
#ifdef ACCEPT_STANDARD_FILTER_1
	RXF1SIDL = (unsigned char)(ACCEPTANCE_FILTER_1 << 5);
	RXF1SIDH = (unsigned char)(ACCEPTANCE_FILTER_1 >> 3);
#else
	RXF1EIDL = (unsigned char)(ACCEPTANCE_FILTER_1 & 0xFF);	
	RXF1EIDH = (unsigned char)((ACCEPTANCE_FILTER_1 >> 8) & 0xFF);
	RXF1SIDL = (unsigned char)((ACCEPTANCE_FILTER_1 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_1 >> 13) & 0xE0) | 0x08;
	RXF1SIDH = (unsigned char)((ACCEPTANCE_FILTER_1 >> 21) & 0xFF);
#endif

#ifdef ACCEPT_STANDARD_FILTER_2
	RXF2SIDL = (unsigned char)(ACCEPTANCE_FILTER_2 << 5);
	RXF2SIDH = (unsigned char)(ACCEPTANCE_FILTER_2 >> 3);
#else
	RXF2EIDL = (unsigned char)(ACCEPTANCE_FILTER_2 & 0xFF);	
	RXF2EIDH = (unsigned char)((ACCEPTANCE_FILTER_2 >> 8) & 0xFF);
	RXF2SIDL = (unsigned char)((ACCEPTANCE_FILTER_2 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_2 >> 13) & 0xE0) | 0x08;
	RXF2SIDH = (unsigned char)((ACCEPTANCE_FILTER_2 >> 21) & 0xFF);
#endif

#ifdef ACCEPT_STANDARD_FILTER_3
	RXF3SIDL = (unsigned char)(ACCEPTANCE_FILTER_3 << 5);
	RXF3SIDH = (unsigned char)(ACCEPTANCE_FILTER_3 >> 3);
#else
	RXF3EIDL = (unsigned char)(ACCEPTANCE_FILTER_3 & 0xFF);	
	RXF3EIDH = (unsigned char)((ACCEPTANCE_FILTER_3 >> 8) & 0xFF);
	RXF3SIDL = (unsigned char)((ACCEPTANCE_FILTER_3 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_3 >> 13) & 0xE0) | 0x08;
	RXF3SIDH = (unsigned char)((ACCEPTANCE_FILTER_3 >> 21) & 0xFF);
#endif

#ifdef ACCEPT_STANDARD_FILTER_4
	RXF4SIDL = (unsigned char)(ACCEPTANCE_FILTER_4 << 5);
	RXF4SIDH = (unsigned char)(ACCEPTANCE_FILTER_4 >> 3);
#else
	RXF4EIDL = (unsigned char)(ACCEPTANCE_FILTER_4 & 0xFF);	
	RXF4EIDH = (unsigned char)((ACCEPTANCE_FILTER_4 >> 8) & 0xFF);
	RXF4SIDL = (unsigned char)((ACCEPTANCE_FILTER_4 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_4 >> 13) & 0xE0) | 0x08;
	RXF4SIDH = (unsigned char)((ACCEPTANCE_FILTER_4 >> 21) & 0xFF);
#endif

#ifdef ACCEPT_STANDARD_FILTER_5
	RXF5SIDL = (unsigned char)(ACCEPTANCE_FILTER_5 << 5);
	RXF5SIDH = (unsigned char)(ACCEPTANCE_FILTER_5 >> 3);
#else
	RXF5EIDL = (unsigned char)(ACCEPTANCE_FILTER_5 & 0xFF);	
	RXF5EIDH = (unsigned char)((ACCEPTANCE_FILTER_5 >> 8) & 0xFF);
	RXF5SIDL = (unsigned char)((ACCEPTANCE_FILTER_5 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_4 >> 13) & 0xE0) | 0x08;
	RXF5SIDH = (unsigned char)((ACCEPTANCE_FILTER_5 >> 21) & 0xFF);
#endif

//Set the acceptance masks
	RXM0EIDL = (unsigned char)(ACCEPTANCE_MASK_0_1 & 0xFF);	
	RXM0EIDH = (unsigned char)((ACCEPTANCE_MASK_0_1 >> 8) & 0xFF);
	RXM0SIDL = (unsigned char)((ACCEPTANCE_MASK_0_1 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_MASK_0_1 >> 13) & 0xE0) | 0x08;
	RXM0SIDH = (unsigned char)((ACCEPTANCE_MASK_0_1 >> 21) & 0xFF);
	RXM1EIDL = (unsigned char)(ACCEPTANCE_MASK_2_5 & 0xFF);	
	RXM1EIDH = (unsigned char)((ACCEPTANCE_MASK_2_5 >> 8) & 0xFF);
	RXM1SIDL = (unsigned char)((ACCEPTANCE_MASK_2_5 >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_MASK_2_5 >> 13) & 0xE0) | 0x08;
	RXM1SIDH = (unsigned char)((ACCEPTANCE_MASK_2_5 >> 21) & 0xFF);

	CIOCON = 0x20;						//Drive TX pin Vdd when recessive, disable capture

#ifdef CAN_LOW_INT
	IPR3 = 0x00;
#else
	IPR3 = 0xFF;
#endif

#ifdef USE_LOOPBACK_MODE
	CANCON = (LOOPBACK_MODE & MODE_MASK) | (CANCON & (MODE_MASK ^ 0xFF));					//Set loopback mode for debug purposes only
	while((CANSTAT & MODE_MASK) != (LOOPBACK_MODE & MODE_MASK));	//Wait until loopback mode is set
#else
	CANCON = (NORMAL_MODE & MODE_MASK) | (CANCON & (MODE_MASK ^ 0xFF));					//Set normal mode
	while((CANSTAT & MODE_MASK) != (NORMAL_MODE & MODE_MASK));		//Wait until normal mode is set
#endif
	PIE3 = CAN_INT_BITS & 0xE3;			//Enable CAN interrupts except TX interrupts
	PIR3 = 0x18;						//Set TXB1 & TXB2 int flags

//Send my address to notify our presence
#ifdef MY_ADDRESS_IS_STANDARD
	TXB0SIDL = (unsigned char)(MY_ADDRESS_IDENTIFIER << 5);
	TXB0SIDH = (unsigned char)(MY_ADDRESS_IDENTIFIER >> 3);		//Load address with MY_ADDRESS_IDENTIFIER standard identifier
	TXB0DLC = 0x00;												//0 data bytes to send
	TXB0CON = 0x00;												//Initiate transmission
	TXB0REQ = 1;
#else
	TXB0EIDL = (unsigned char)(MY_ADDRESS_IDENTIFIER & 0xFF);	//Load address with MY_ADDRESS_IDENTIFIER extended identifier
	TXB0EIDH = (unsigned char)((MY_ADDRESS_IDENTIFIER >> 8) & 0xFF);
	TXB0SIDL = (unsigned char)((MY_ADDRESS_IDENTIFIER >> 16) & 0x03) | (unsigned char)((ACCEPTANCE_FILTER_2 >> 13) & 0xE0) | 0x08;
	TXB0SIDH = (unsigned char)((MY_ADDRESS_IDENTIFIER >> 21) & 0xFF);
	TXB0DLC = 0x00;												//0 data bytes to send
	TXB0CON = 0x00;												//Initiate transmission
	TXB0REQ = 1;
#endif
	
	return 0;
}


/*********************************************************************
 * Function:        void CANISR(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Will modify the RX/TX interrupt flags 
 *                  and interrupt enable bits
 *
 * Overview:        Checks if a CAN reception/transmission was complete 
 *                  and if so write/read to the CAN RX/TX FIFO buffers
 *
 * Note:            This function is supposed to be called from the ISR
 ********************************************************************/
void CANISR(void)
{
	unsigned char TempCANCON;

	if(PIR3 & PIE3)
	{
		TempCANCON = CANCON;
		
		if(RXB0IF && RXB0IE)
		{
			RXB0IF = 0;		//Clear interrupt flag
			CANCON = CANCON & 0xF0 | RXB0Interrupt;
			CANGetMessage();
		}
		else if(RXB1IF && RXB1IE)
		{
			RXB1IF = 0;		//Clear interrupt flag
			CANCON = CANCON & 0xF0 | RXB1Interrupt;
			CANGetMessage();
		}
		else if(TXB0IF && TXB0IE)
		{
			CANCON = CANCON & 0xF0 | TXB0Interrupt;
			if(CANPutMessage())
				TXB0IE = 0;
			else
				TXB0IF = 0;
		}
		else if(TXB1IF && TXB1IE)
		{
			CANCON = CANCON & 0xF0 | TXB1Interrupt;
			if(CANPutMessage())
				TXB1IE = 0;
			else
				TXB1IF = 0;
		}
		else if(TXB2IF && TXB2IE)
		{
			CANCON = CANCON & 0xF0 | TXB2Interrupt;
			if(CANPutMessage())			//if there wasn't any more messages to send
				TXB2IE = 0;				//disable interrupts for TXB2 and leave TXB2IF 
			else						//still on so PutCAN() can reenable the interrupt and instantly vector to ISR
				TXB2IF = 0;				//message was sent, just clear the interrupt flag.
		}
		else if(ERRIF && ERRIE)
		{
			ERRIF = 0;			//Clear interrupt flag
#ifdef CAN_ERROR_HANDLER_ENABLE
			CANErrorHandler();
#endif			
								//No error handler implemented!!!
		}
		else if(WAKIF && WAKIE)
		{
			WAKIF = 0;			//Clear interrupt flag
		}

		CANCON = TempCANCON;
	}
}


/*********************************************************************
 * Function:        void CANGetMessage(void)
 *
 * PreCondition:    <WIN2:WIN0> in the CANCON register has to set
 *                  to reflect the desired RXB registers
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Will modify the RX FIFO Write pointer (RXWPtr)
 *
 * Overview:        Gets the registers for a RXB and puts them in the
 *                  CAN Receive buffer
 *
 * Note:            Care is not taken if buffer is full
 ********************************************************************/
void CANGetMessage(void)
{
	unsigned char i;
	if(++RXWPtr >= RXBUF)		//If pointer overflowed
		RXWPtr = 0;				//Clear it
	
	for(i=0;i<14;i++)
	{
		RXMessage[RXWPtr].Data[i] = *(unsigned char *)(0xF60+i);
	}

	RXB0FUL = 0;
}


/*********************************************************************
 * Function:        char CANPutMessage(void)
 *
 * PreCondition:    <WIN2:WIN0> in the CANCON register has to set
 *                  to reflect the desired TXB registers
 *
 * Input:           None
 *
 * Output:          0 -> A new message has been put in the transmit queue
 *                  1 -> There was no messages in the TX buffer to send
 *
 * Side Effects:    Will modify the TX buffer큦 Read pointer (TXRPtr)
 *
 * Overview:        Checks if there is any messages to transmit and if so
 *                  place it in the registers reflected by <WIN2:WIN0> 
 *
 * Note:            None
 ********************************************************************/
char CANPutMessage(void)
{
	unsigned char i;
	if(TXWPtr != TXRPtr)						//If there are any more data to send
	{
		if(++TXRPtr >= TXBUF)					//then increment the TX read pointer
			TXRPtr = 0;
		for(i=0;i<14;i++)
			*(unsigned char *)(0xF60+i) = TXMessage[TXRPtr].Data[i];
		RXB0RTRRO = 1;							//TXREQ = 1;
		return 0;
	}
	else
		return 1;
}


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
 * Side Effects:    Will modify the TX Buffer register큦 Write pointer
 *
 * Overview:        Initially checks if at least one buffer slot is available
 *                  and if so push the requested message in the buffer.
 *                  Checks if the TX modules are idle and if they are, reactivate one.
 *
 * Note:            None
 ********************************************************************/
char CANPut(struct CANMessage Message)
{
	unsigned char TempPtr, i;
	if(TXWPtr == TXRPtr-1 || (TXWPtr == TXBUF-1 && TXRPtr == 0))	//if all transmit buffers are full return 1
		return 1;
		
	//Do not modify the TX pointer until the message has been successfully copied so the CANISR don't
	//send a message that isn't ready
	if(TXWPtr >= TXBUF-1)					//check if transmit write pointer will overflow
	{
		TempPtr = 0;							//and post clear write pointer
	}
	else
	{
		TempPtr = TXWPtr+1;							//and postincrement write pointer
	}
	
	if(Message.NoOfBytes > 8)					//Ensure we don't handle more than 8 bytes
		Message.NoOfBytes = 8;	
		
	TXMessage[TempPtr].Specific.TXBDLC.Byte = Message.NoOfBytes;				//Set the data count

	if(!Message.Remote)			//If dataframe
	{
		TXMessage[TempPtr].Specific.TXBDLC.Bits.TXRTR = 0;								//Clear the Remote Transfer Request bit
		
		for(i = 0; i < Message.NoOfBytes; i++)	//Load data registers
		{
			TXMessage[TempPtr].Specific.TXBD.Array[i] = Message.Data[i];
		}
	}
	else											//Remote frame
	{
		TXMessage[TempPtr].Specific.TXBDLC.Bits.TXRTR = 1;								//Set TXRTR bit
	}
	if(Message.Ext)						//Extended identifier
	{
		TXMessage[TempPtr].Specific.TXBEIDL.Byte = (unsigned char)(Message.Address & 0xFF);			// Put address <7:0> in EIDL
		TXMessage[TempPtr].Specific.TXBEIDH.Byte = (unsigned char)((Message.Address >> 8) & 0xFF);	// Put address <15:8> in EIDH
		TXMessage[TempPtr].Specific.TXBSIDL.Byte = (unsigned char)((Message.Address >> 16) & 0x03) | (unsigned char)((Message.Address >> 13) & 0xE0);
		TXMessage[TempPtr].Specific.TXBSIDH.Byte = (unsigned char)((Message.Address >> 21) & 0xFF);
		TXMessage[TempPtr].Specific.TXBSIDL.Bits.EXIDE = 1;
	}
	else											//Standard identifier
	{
		TXMessage[TempPtr].Specific.TXBSIDL.Byte = (unsigned char)((Message.Address << 5) & 0xFF);	//Put address <2:0> in SIDL
		TXMessage[TempPtr].Specific.TXBSIDH.Byte = (unsigned char)((Message.Address >> 3) & 0xFF);	//Put address <10:3> in SIDH
		TXMessage[TempPtr].Specific.TXBSIDL.Bits.EXIDE = 0;
	}
	TXMessage[TempPtr].Specific.TXBCON.Byte = Message.Priority & 0x03;	//Set the internal priority of the data frame
	
	TXWPtr = TempPtr;

	//Reenable an interrupt if it is idle, it doesn't matter if another TX source caught the pending message
	//before the source that is enabled here does since the interrupt CHECKS if there are any messages pending
	//and if the buffer is empty, it will disable itself again.
	if(!TXB0IE)			//TXB0 is idle
		TXB0IE = 1;		//enable TXB0 to fire an interrupt to send the pending message
	else if(!TXB1IE)		//else if TXB1 is idle
		TXB1IE = 1;		//same procedure
	else if(!TXB2IE)		//finally try TXB2
		TXB2IE = 1;	

	return 0;
}


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
char CANRXMessageIsPending(void)
{
	if(RXWPtr != RXRPtr)
		return 1;
	else
		return 0;
}



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
 * Side Effects:    Will modify the RX Buffer register큦 Read pointer
 *
 * Overview:        Pops the the first message of the RX buffer
 *
 * Note:            None
 ********************************************************************/
struct CANMessage CANGet(void)
{
	struct CANMessage ReturnMessage;
	unsigned char TempPtr, i;

	/*Do not modify the RX pointer until the message has been successfully
	  copied to prevent ISR to overwrite it (Note. this is not implemented in the ISR yet)*/
	if(RXRPtr >= RXBUF-1)					//Check if pointer will overflow
	{
		TempPtr = 0;
	}
	else
	{
		TempPtr = RXRPtr+1;
	}
	
	ReturnMessage.NoOfBytes = RXMessage[TempPtr].Specific.RXBDLC.Byte & 0x0F;					//copy data count
	if(RXMessage[TempPtr].Specific.RXBCON.Bits.RXRTRRO)	//Remote frame request
	{
		ReturnMessage.Remote = 1;
	}
	else				//Data frame 
	{
		ReturnMessage.Remote = 0;									//Clear remote flag
		for(i = 0; i < ReturnMessage.NoOfBytes; i++)				//copy data content
			ReturnMessage.Data[i] = RXMessage[TempPtr].Specific.RXBD.Array[i];
	}
	CANCON = CANCON & 0xF0;
	
	ReturnMessage.Address = (unsigned int)(RXMessage[TempPtr].Specific.RXBSIDH.Byte) << 3;			//Load the standard identifier into the address
	ReturnMessage.Address |= (RXMessage[TempPtr].Specific.RXBSIDL.Byte >> 5);

	if(RXMessage[TempPtr].Specific.RXBSIDL.Bits.EXID)				//Extended identifier
	{
		ReturnMessage.Ext = 1;										//Set the extended identifier flag
		ReturnMessage.Address = (ReturnMessage.Address << 2) | (RXMessage[TempPtr].Specific.RXBSIDL.Byte & 0x03);
		ReturnMessage.Address = ReturnMessage.Address << 16;		//and copy the extended address 
		ReturnMessage.Address |= (unsigned int)(RXMessage[TempPtr].Specific.RXBEIDH.Byte) << 8;
		ReturnMessage.Address |= RXMessage[TempPtr].Specific.RXBEIDL.Byte;		
	}
	else								//Standard identifier
	{
		ReturnMessage.Ext = 0;		//clear the extended identifier flag
	}
	
	RXRPtr = TempPtr;
	return ReturnMessage;		
}


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
 *                  the mode has been set.
 *
 * Note:            If USE_LOOPBACK_MODE is defined the requested
 *                  mode will be loopback mode if the input is
 *                  Normal mode
 ********************************************************************/
void CANSetMode(unsigned char Mode)
{
#ifdef USE_LOOPBACK_MODE
	if(Mode == NORMAL_MODE)
	{
		CANCON = (LOOPBACK_MODE & MODE_MASK) | (CANCON & (MODE_MASK ^ 0xFF));
		while((CANSTAT & MODE_MASK) != (LOOPBACK_MODE & MODE_MASK));	
	}
	else
#endif
	{
		CANCON = (Mode & MODE_MASK) | (CANCON & (MODE_MASK ^ 0xFF));
		while((CANSTAT & MODE_MASK) != (Mode & MODE_MASK));
	}

}
