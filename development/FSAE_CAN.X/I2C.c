// Functions and data used to provide the communication in the
// I2C.
// The targeted board is a Digilent Cerebot 32MX7.
// 
// Change History:
//
// Name					Date		Changes
//
// Shaishav Parekh		12/5/11		No changes.


#include <plib.h>
#include "I2C.h"
#include "HardwareProfile.h"


/*******************************************************************************
Function: void initI2C()

Summary: Sets the I2C frequency and enables the bus

Parameters: NONE

Returns: NONE
*******************************************************************************/

void initI2C()
{
	// Set the I2C baudrate.
    I2CSetFrequency(LCD_I2C_BUS, GetPeripheralClock(), I2C_CLOCK_FREQ);

	// Enable the I2C bus.
    I2CEnable(LCD_I2C_BUS, TRUE);
}

/*******************************************************************************
  Function:
    BOOL StartTransfer( BOOL restart )

  Summary:
    Starts (or restarts) a transfer to/from the EEPROM.

  Description:
    This routine starts (or restarts) a transfer to/from the LCD, waiting (in
    a blocking loop) until the start (or re-start) condition has completed.

  Precondition:
    The I2C module must have been initialized.

  Parameters:
    restart - If FALSE, send a "Start" condition
            - If TRUE, send a "Restart" condition
    
  Returns:
    TRUE    - If successful
    FALSE   - If a collision occured during Start signaling
    
  Example:
    <code>
    StartTransfer(FALSE);
    </code>

  Remarks:
    This is a blocking routine that waits for the bus to be idle and the Start
    (or Restart) signal to complete.
  *****************************************************************************/

BOOL StartTransfer( BOOL restart )
{
    I2C_STATUS  status;

    // Send the Start (or Restart) signal
    if(restart)
    {
        I2CRepeatStart(LCD_I2C_BUS);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        while( !I2CBusIsIdle(LCD_I2C_BUS) );

        if(I2CStart(LCD_I2C_BUS) != I2C_SUCCESS)
        {
            DBPRINTF("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(LCD_I2C_BUS);

    } while ( !(status & I2C_START) );

    return TRUE;
}


/*******************************************************************************
  Function:
    BOOL TransmitOneByte( UINT8 data )

  Summary:
    This transmits one byte to the LCD.

  Description:
    This transmits one byte to the LCD, and reports errors for any bus
    collisions.

  Precondition:
    The transfer must have been previously started.

  Parameters:
    data    - Data byte to transmit

  Returns:
    TRUE    - Data was sent successfully
    FALSE   - A bus collision occured

  Example:
    <code>
    TransmitOneByte(0xAA);
    </code>

  Remarks:
    This is a blocking routine that waits for the transmission to complete.
  *****************************************************************************/

BOOL TransmitOneByte( UINT8 data )
{
    // Wait for the transmitter to be ready
    while(!I2CTransmitterIsReady(LCD_I2C_BUS));

    // Transmit the byte
    if(I2CSendByte(LCD_I2C_BUS, data) == I2C_MASTER_BUS_COLLISION)
    {
        DBPRINTF("Error: I2C Master Bus Collision\n");
        return FALSE;
    }

    // Wait for the transmission to finish
    while(!I2CTransmissionHasCompleted(LCD_I2C_BUS));

    return TRUE;
}


/*******************************************************************************
  Function:
    void StopTransfer( void )

  Summary:
    Stops a transfer to the LCD.

  Description:
    This routine Stops a transfer to the LCD, waiting (in a 
    blocking loop) until the Stop condition has completed.

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    None.
    
  Returns:
    None.
    
  Example:
    <code>
    StopTransfer();
    </code>

  Remarks:
    This is a blocking routine that waits for the Stop signal to complete.
  *****************************************************************************/

void StopTransfer( void )
{
    I2C_STATUS  status;

    // Send the Stop signal
    I2CStop(LCD_I2C_BUS);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(LCD_I2C_BUS);

    } while ( !(status & I2C_STOP) );
}

/*******************************************************************************
  Function:
    BOOL ReceiveOneByte(UINT8* data, BOOL ack)

  Summary:
    Receives one byte from data.

  Description:
    This routine receives a byte from a buffer, data, with a bool ack .

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    UINT8* data, BOOL ack
    
  Returns:
    TRUE or FALSE.
    
  *****************************************************************************/

BOOL ReceiveOneByte(UINT8* data, BOOL ack)
{
	if(I2CReceiverEnable(LCD_I2C_BUS, TRUE)==I2C_SUCCESS)
	{
		while (!(I2CReceivedDataIsAvailable(LCD_I2C_BUS)));		
		*data = I2CGetByte(LCD_I2C_BUS);
		I2CAcknowledgeByte(LCD_I2C_BUS, ack);
		while (!(I2CAcknowledgeHasCompleted(LCD_I2C_BUS))); 
		return TRUE;
	}
	else 
	{
	return FALSE;
	}
}

/*******************************************************************************
  Function:
    BOOL I2Csend(UINT8 address, int length, const UINT8* buffer)

  Summary:
    Sends buffer of given length to the address.

  Parameters:
    UINT8 adress, int length, const UINT8* buffer
    
  Returns:
    TRUE if sent sucessfully, FALSE if unsuccessful send.
    
  *****************************************************************************/

BOOL I2Csend(UINT8 address, int length, const UINT8* buffer)
{
	BOOL success = TRUE;
	
	    // Start the transfer for writing data to the LCD.
	    if (StartTransfer(FALSE)) {
	    	I2C_7_BIT_ADDRESS SlaveAddress;
	    	I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, address, I2C_WRITE);
			if (TransmitOneByte(SlaveAddress.byte)) {
				const char* cp;
	            // Call TransmitOneByte() for each non-null character in the string.
	            // Stop with success set to false if TransmitOneByte() returns failure.
				int index = 0;
				while (index < length) {
					success = TransmitOneByte(*buffer);
					index++;
					buffer++;
				}	
				if (success) {
					StopTransfer();
					}
				}
			else {
				success = FALSE;
	    	}
		}
		else {
			success = FALSE;
		}	
	return success;
}

/*******************************************************************************
  Function:
    BOOL I2Crecv(UINT8 address, int length, UINT8* buffer)

  Summary:
    Receives data from address of given length and stores into buffer.

  Parameters:
    UINT8 address, int length, UINT8* buffer
    
  Returns:
    TRUE if received successfully, FALSE if receiving failed.
    
  *****************************************************************************/

BOOL I2Crecv(UINT8 address, int length, UINT8* buffer)
{
	BOOL success = TRUE;

    // Start the transfer for writing data to the LCD.
    if (StartTransfer(FALSE)) {
    	I2C_7_BIT_ADDRESS SlaveAddress;
    	I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, address, I2C_READ);
		if (TransmitOneByte(SlaveAddress.byte)) {
			const char* cp;
            // Call TransmitOneByte() for each non-null character in the string.
            // Stop with success set to false if TransmitOneByte() returns failure.
			int index = 0;
			while (index < length) {
				if (index == (length-1)) {
					success = ReceiveOneByte(buffer, FALSE);
					index++;
					buffer++;
				}
				else {
					success = ReceiveOneByte(buffer, TRUE);
					index++;
					buffer++;
				}
			}	
			if (success) {
				StopTransfer();
				}
			}
		else {
			success = FALSE;
    	}
	}
	else {
		success = FALSE;
	}	
	return success;
}
