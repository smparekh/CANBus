/* 
 * File:   UART.h
 * Author: VTM
 *
 * Created on June 11, 2013, 6:25 PM
 */
#define MAXUARTBUF 1

#ifndef UART_H
#define	UART_H

typedef struct __uart_comm {
	unsigned char buffer[MAXUARTBUF];
	unsigned char	buflen;
} uart_comm;

void initUART();

//void UART_int_handler(void);

#endif	/* UART_H */

