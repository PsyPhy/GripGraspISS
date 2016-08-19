// CodaVisibility.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include <WinSock2.h>

#include <windows.h>
#include <mmsystem.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"

// A device that records 3D marker positions.
PsyPhy::CodaRTnetDaemonTracker codaTracker;

using namespace PsyPhy;

int _tmain(int argc, _TCHAR* argv[])
{
	
	// Make sure that the GraspTrackerDaemon has time to bind its socket.
	Sleep( 500 );

	MarkerFrame localFrame[2];
	int which_marker = 0;

	fprintf( stderr, "Initializing CodaRTnetDaemonTracker ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Starting acquisition ... " );
	codaTracker.StartAcquisition( 600.0 );
	fprintf( stderr, "OK.\n\n" );

	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < 2; unit++ ) {
			codaTracker.GetCurrentMarkerFrameUnit( localFrame[unit], unit );
			fprintf( stderr, "U%d: %6.3f ", unit, localFrame[unit].time );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( localFrame[unit].marker[mrk].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				if ( ((mrk+1) % 8) == 0 ) fprintf( stderr, "   " );
			}
			fprintf( stderr, "  " );
		}
		fprintf( stderr, "Algt: " );
		for ( int mrk = 0; mrk < 24; mrk++ ) {
			int count = 0;
			double sum = 0.0;
			if ( localFrame[0].marker[mrk].visibility && localFrame[1].marker[mrk].visibility ) {
				Vector3 delta;
				codaTracker.SubtractVectors( delta, localFrame[0].marker[mrk].position, localFrame[1].marker[mrk].position );
				double distance = codaTracker.VectorNorm( delta );
				if ( distance < 2.0 ) fprintf( stderr, "O" );
				else if ( distance < 5.0 ) fprintf( stderr, "+" );
				else fprintf( stderr, "_" );
				sum += distance;
				count++;
			}
			else {
				fprintf( stderr, "." );
			}
			if ( ((mrk+1) % 8) == 0 ) {
				fprintf( stderr, " (%3.0lf) ", ( count ? sum / (double) count : 999.0 ) );
				sum = 0.0;
				count = 0;
			}
		}
		fprintf( stderr, "%s %s\n", codaTracker.vstr( localFrame[0].marker[which_marker].position), codaTracker.vstr( localFrame[1].marker[which_marker].position ));

	}
	int key = _getch(); // Clear the _kbhit().
	codaTracker.AbortAcquisition();

	fprintf( stderr, "\nCodaVisibility terminated. Press <Return> to close window.\n" );
	_getch();

	return 0;
}

