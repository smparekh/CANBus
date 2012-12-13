// Function prototypes for initializing, sending
// and receving data from the I2C Bus
//
// Change History:
//
// Name					Date		Changes
//
// Shaishav Parekh		11/1/11		Initial Version.
// Shaishav Parekh		12/6/11		None.

#ifndef I2C_H
#define I2C_H

void initI2C();

BOOL I2Csend(UINT8 address, int length, const UINT8* buffer);

BOOL I2Crecv(UINT8 address, int length, UINT8* buffer);

#endif
