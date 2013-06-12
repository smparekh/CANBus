/* 
 * File:   HardwareProfile.h
 * Author: carbon
 *
 * Created on July 23, 2012, 6:45 PM
 */

#ifndef HARDWAREPROFILE_H
#define	HARDWAREPROFILE_H

#ifdef	__cplusplus
extern "C" {
#endif
// Clock Constants
#define SYS_CLOCK_FREQ 80000000
#define GetSystemClock() (SYS_CLOCK_FREQ)
#define GetPeripheralClock() (SYS_CLOCK_FREQ/8)
#define GetInstructionClock() (SYS_CLOCK_FREQ)

#define LCD_I2C_BUS           I2C1             // Channel 1
#define LCD_ADDRESS           0x48             // LCD address
#define I2C_CLOCK_FREQ        100000

#define UART_BAUD_RATE        9600

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWAREPROFILE_H */

