// Function prototypes for initializing and writing to the Digilent PmodCLS LCD.
//
// Change History:
//
// Name					Date		Changes
//
// Cameron Patterson	8/9/11		Initial version.
// Shaishav Parekh		9/11/11		No changes.
// Shaishav Parekh		11/15/11	No changes.
// Shaishav Parekh		12/6/11		No changes.

#ifndef LCD_H
#define LCD_H

void initLCD();

void specChars();

BOOL lcdString(const char string[]);

BOOL lcdInstruction(const char string[]);

#endif
