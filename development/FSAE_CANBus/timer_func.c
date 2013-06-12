#include "timer_func.h"
#include <plib.h>
static BOOL oneSecondUp;
static UINT32 msecCounter;


void Timer1Init(void)
{
	/* This function will intialize the Timer 1
	 * for basic timer operation. It will enable
	 * timer interrupt. The one second flag is
	 * initialized and the millisecond counter is
	 * initialized. */

	T1CON = 0x0;			/* Basic Timer Operation				*/
	PR1 = TIMER1_PERIOD;	/* Timer period in TimerFunctions.h 	*/

	IFS0CLR = 0x10;			/* Clear interrupt flag and enable		*/
	IEC0SET = 0x10;			/* timer 1 interrupt. 					*/
	IPC1bits.T1IP = 4;		/* Timer 1 priority is 4				*/

	oneSecondUp = FALSE;	/* Intialize the one second flag		*/
	msecCounter = 0;		/* and the millisecond counter.			*/

	T1CONSET = 0x8000;		/* Start the timer.						*/

}

BOOL IsOneSecondUp(void)
{
	/* This function will return TRUE if
	 * a second has expired since the last
	 * time the function had returned
	 * TRUE. If not then function returns
	 * FALSE. */

	BOOL result;

	if (oneSecondUp == TRUE)
	{
		/* If a second has expired
		 * then return true and reset
		 * the one second flag. */

		result = TRUE;
		oneSecondUp = FALSE;
	}
	else
	{
		result = FALSE;
	}

	return(result);
}

void __attribute__((vector(4), interrupt(ipl4), nomips16)) Timer1InterruptHandler(void)
{
	/* This is the Timer 1 ISR */

	IFS0CLR = 0x10; 	/* Clear the Interrupt Flag	*/

	msecCounter ++;		/* Increment millisecond counter.	*/

	if(msecCounter == ONE_SECOND_MSEC)
	{
		/* This means that one second
		 * has expired since the last time
		 * msecCounter was 0. */

		oneSecondUp = TRUE;	/* Indicate that one second is up	*/
		msecCounter = 0;	/* Reset the millisecond counter.	*/
                mPORTGToggleBits(BIT_14);
	}
        


}
