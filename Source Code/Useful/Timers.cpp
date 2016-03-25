/*****************************************************************************/
/*                                                                           */
/*                               DexTimers.cpp                               */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Timers.h"

// 
// Scale factor to produce slow motion.
// Set it to 1.0 to produce normal speed movements.
// Note that all the timers share this common factor.
//

static double dex_slow_motion = 1.0;

/*****************************************************************************/

void TimerSetSlowmotion( float factor ) {
	dex_slow_motion = factor;
}


/*****************************************************************************/

/*
 * Implement a means of timing actions - version Win32.
 */

/****************************************************************************/

/*
 * Simply start the timer without setting an alarm.
 * Used to measure elapsed time rather than to wait for a timeout.
 */

void TimerStart ( Timer &timer ) {

	LARGE_INTEGER	li;

	QueryPerformanceFrequency( &li );
	timer.frequency = (double) li.QuadPart;

	QueryPerformanceCounter( &li );
	timer.mark = li.QuadPart;

	timer.split = timer.mark;

}
	
/*****************************************************************************/

double TimerElapsedTime ( Timer &timer ) {
	
	LARGE_INTEGER	li;
	__int64			current_time;
	double			duration;
	
	/* Compute the true time interval since the timer was started. */

	QueryPerformanceCounter( &li );
	current_time = li.QuadPart;
	duration = (double) (current_time - timer.mark) / timer.frequency / dex_slow_motion;

	return( duration );

}

double TimerRemainingTime( Timer &timer ) {
	return( timer.alarm - TimerElapsedTime( timer ) );
}

/****************************************************************************/

double TimerSplitTime ( Timer &timer ) {
	
	LARGE_INTEGER	li;
	__int64			current_time;
	double			duration;
	
	/* Compute the true time interval since the last split was set. */

	QueryPerformanceCounter( &li );
	current_time = li.QuadPart;
	duration = (double) (current_time - timer.split) / timer.frequency / dex_slow_motion;

	return( duration );

}

/****************************************************************************/

void TimerSet( Timer &timer, double seconds ) {

	timer.alarm = seconds;
	TimerStart( timer );

}

int	TimerTimeout( Timer &timer ) {
	return( TimerElapsedTime( timer ) >= timer.alarm );
}
