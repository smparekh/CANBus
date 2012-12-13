// Functions used for writing to the Digilent PmodCLS LCD display over the
// Cerebot 32MX7 I2C channel 1.
// The I2C functions are copied from MicroChip's I2C peripheral library
// example code (i2c_master.c).
//
// Change History:
//
// Name					Date		Changes
//
// Cameron Patterson	8/9/11		Initial version.
// Shaishav Parekh		9/11/11		Included fixes for resetting lcd, transmitting the
//									to the lcd and fixes to the lcdInstruction function
// Shaishav Parekh		11/15/11	None.
// Shaishav Parekh		12/6/11		Added a function that creates custom characters.


#include <plib.h>
#include <string.h>
#include "I2C.h"
#include "HardwareProfile.h"
#include "LCD.h"

/*******************************************************************************
	Function:	
	  void specChars();

	Summary:
	  Creates four custom characters.
	
	Description:
		Creates a character occupy the top left, right and bottom left, right
		corners of one block on the lcd.
	
	Parameters:
		None.
	
	Returns:
		None.
*******************************************************************************/
void specChars() {

	lcdInstruction("24;24;0;0;0;0;0;0;1d");
	lcdInstruction("3;3;0;0;0;0;0;0;2d");
	lcdInstruction("0;0;0;0;0;0;24;24;3d");
	lcdInstruction("0;0;0;0;0;0;3;3;4d");
	lcdInstruction("3p");
}

/*******************************************************************************
  Function:
    BOOL lcdString(const char string[])

  Summary:
    Displays a character string on the PmodCLS LCD.

  Description:
    Each non-null byte in the character string is sent over the I2C bus to the
    LCD.  There is no length check on the string.

  Precondition:
    The LCD's I2C bus must have been initialized and enabled.

  Parameters:
    None.
    
  Returns:
    TRUE if the write succeeded, and FALSE otherwise.
    
  Example:
    <code>
    lcdString("Hello");
    </code>

  Remarks:
    All string characters are sent in a single I2C bus transaction,
    and there are no retries.
  *****************************************************************************/

BOOL lcdString(const char string[])
{
    BOOL success = TRUE;
	int length = strlen(string);


	if (I2Csend(LCD_ADDRESS, length, string))
	{
	success = TRUE;
	}
	else { success = FALSE; }
	
	return success;
}
/*******************************************************************************
  Function:
    BOOL lcdInstruction(const char[] string)

  Summary:
    Writes an instruction (command) to the PmodCLS LCD.

  Description:
    Each non-null byte in the character string is sent over the I2C bus to the
    LCD.  At most 28 bytes from the instruction string are sent.

  Precondition:
    The LCD's I2C bus must have been initialized and enabled.

  Parameters:
    A pointer to a null-terminated string of at most 28 characters.
    
  Returns:
    TRUE if the write succeeded, and FALSE otherwise.
    
  Example:
    <code>
    lcdInstruction("*");  // performs a reset
    </code>

  Remarks:
    The instruction escape sequence is prepended to the string.
  *****************************************************************************/

BOOL lcdInstruction(const char string[])
{
	char instruction[32] = "\x1B[";  // Instruction escape preamble.

	return lcdString(strncat(instruction, string, 28));
}

/*******************************************************************************
  Function:
    void initLCD(void)

  Summary:
    Enables the LCD's I2C bus, resets and clears the display.

  Description:
    This routine sets the I2C bus speed, enables the I2C bus, 
    and sends LCD commands to reset and clear the display.

  Precondition:
    None.

  Parameters:
    None.
    
  Returns:
    None.
    
  Example:
    <code>
    initLCD();
    </code>

  Remarks:
    Since the LCD is a write-only device, no error diagnostics are provided.
  *****************************************************************************/

void initLCD()
{
	const char resetDisplay[]  = "*";   // Equivalent to a cycling the power.
	const char enableDisplay[] = "1e";  // Enables display with the backlight off.
    
	// Reset and clear the display.
	lcdInstruction("*");
	lcdInstruction("j");
    
}






