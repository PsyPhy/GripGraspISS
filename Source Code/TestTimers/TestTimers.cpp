// TestTimers.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "../Useful/Timers.h"

int main(int argc, char *argv[])
{
	Timer tm;

	TimerSet( tm, 5.0 );

	while ( !TimerTimeout( tm ) ) {
		fprintf( stderr, "Elapsed: %f   Remaining: %f\n", TimerElapsedTime( tm ), TimerRemainingTime( tm ) );
		int hit = _kbhit();
		if ( hit ) {
			int key = _getch( );
			if ( key == 'p' ) {
				TimerPause( tm );
				fprintf( stderr, "p\n" );
			}
			else if ( key != 0 ) {
					 TimerResume( tm );
			}
		}
	}

	return 0;
}

