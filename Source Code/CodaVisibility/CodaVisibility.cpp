// CodaVisibility.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>

#include <windows.h>
#include <mmsystem.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

// A device that records 3D marker positions.
PsyPhy::CodaRTnetTracker codaTracker;

using namespace PsyPhy;

int _tmain(int argc, _TCHAR* argv[])
{
	
	MarkerFrame localFrame[2];

	int origin = 1;
	int x_minus = 1;
	int x_plus = 0;
	int y_minus = 2;
	int y_plus = 0;


	fprintf( stderr, "Initializing CODA ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Starting acquisition ... " );
	codaTracker.StartAcquisition( 600.0 );
	fprintf( stderr, "OK.\n\n" );

	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < 2; unit++ ) {
			codaTracker.GetCurrentMarkerFrameUnit( localFrame[unit], unit );
			fprintf( stderr, "U%d: ", unit );
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
				fOutputDebugString( "xxx.xx " );
			}
			if ( ((mrk+1) % 8) == 0 ) {
				fprintf( stderr, " (%3.0lf) ", ( count ? sum / (double) count : 999.0 ) );
				sum = 0.0;
				count = 0;
			}
		}
		Vector3 xdiff;
		Vector3 ydiff;
		codaTracker.SubtractVectors( xdiff, localFrame[0].marker[x_plus].position, localFrame[0].marker[x_minus].position );
		codaTracker.SubtractVectors( ydiff, localFrame[0].marker[y_plus].position, localFrame[0].marker[y_minus].position );
		fprintf( stderr, " %s %s\n", codaTracker.vstr( xdiff ), codaTracker.vstr( ydiff ) );

	}
	int key = getch(); // Clear the _kbhit().
	codaTracker.AbortAcquisition();

	if ( key == 'a' || key == 'A' ) {
		fprintf( stderr, "Performing alignment O: %d X: %d->%d Y: %d->%d ... ", origin, x_minus, x_plus, y_minus, y_plus );
		codaTracker.PerformAlignment( origin, x_minus, x_plus, y_minus, y_plus, true );
		fprintf( stderr, "OK.\n" );
	}

	fprintf( stderr, "\nCodaVisibility terminated. Press <Return> to close window.\n" );
	getch();

	return 0;
}

