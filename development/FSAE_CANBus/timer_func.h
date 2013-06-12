/* 
 * File:   timer_func.h
 * Author: carbon
 *
 * Created on July 23, 2012, 5:58 PM
 */

#ifndef _TIMER_FUNC_H_
#define _TIMER_FUNC_H_

#include "GenericTypeDefs.h"
#include <xc.h>

#define TIMER1_PERIOD 	0x1F40	/* For a 0.1 millisecond interval	*/
#define ONE_SECOND_MSEC	0x03E8	/* For a 1 second timeout in terms 	*/
								/* of 0.1 milliseconds	*/

/****************************************************************************
  Function:
    void IsOneSecondUp(void);

  Description:
    This function will return true if one second has expired
	since the last time the function returned true.

  Precondition:
	None.

  Parameters:
	None.

  Return Values:
    TRUE - If one second has expired.
    FALSE - If one second has not expired.

  Remarks:
  	None.

  Example:
  	if(IsOneSecondUp() == TRUE)
	{
		foo();
	}
  ***************************************************************************/

BOOL IsOneSecondUp(void);

/****************************************************************************
  Function:
    void Timer1Init(void);

  Description:
    This function will intialize and start Timer 1 for period
	specified by the TIMER1_PERIOD macro. It will enable
	timer interrupt and will intialize the internal one
	second counter.

  Precondition:
	None.

  Parameters:
	None.

  Return Values:
	None.

  Remarks:
  	None.

  Example:
  	Timer1Init();
  ***************************************************************************/

void Timer1Init(void);


#endif

