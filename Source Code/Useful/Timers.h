/*****************************************************************************/
/*                                                                           */
/*                                   Timers.h                                */
/*                                                                           */
/*****************************************************************************/

/* Timers */

#pragma once

#ifdef __cplusplus 
extern "C" {
#endif

#include "Windows.h"

typedef struct {

#ifdef WIN32		/* Windows NT */

	__int64 mark;
	__int64 split;
	double frequency;
    double	alarm;

#else				/* SGI Irix */

    timespec_t	mark;
    timespec_t	split;
    double		alarm;

#endif

 

} Timer;

void	TimerStart ( Timer &timer );
double	TimerElapsedTime ( Timer &timer );
double	TimerRemainingTime( Timer &timer );
double	TimerSplitTime ( Timer &timer );
void	TimerSet( Timer &timer, double seconds );
int	    TimerTimeout( Timer &timer );
void	TimerSetSlowmotion( float factor );

#ifdef __cplusplus 
}
#endif
