/* 
 * File:   can_func.h
 * Author: carbon
 *
 * Created on June 20, 2012, 7:00 PM
 */

#ifndef CAN_FUNC_H
#define	CAN_FUNC_H

#include "GenericTypeDefs.h"
#include <xc.h>
#include <sys/kmem.h>
#include "plib.h"

#define SYSTEM_FREQ 80000000
#define CAN_BUS_SPEED 1000000

#define CAN1_BRPVAL 0x3

BYTE CAN1MessageFifoArea[2 * 8 * 16];	/* This is the CAN1 FIFO message area.	*/

void CAN1Init(void);

void CAN1RxMsgProcess(void);


#endif	/* CAN_FUNC_H */

