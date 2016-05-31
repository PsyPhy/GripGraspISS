// CharnwoodAlignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Some convenient routines for outputing debug messages.
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

using namespace PsyPhy;

// A device that records 3D marker positions that will drive the 6dof pose trackers.
// This has to be a static variable because otherwise it overflows the stack.
CodaRTnetTracker codaTracker;


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

	//
	// Run a loop to show the visibility of the markers.
	// 
	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < 2; unit++ ) {
			codaTracker.GetCurrentMarkerFrameUnit( localFrame[unit], unit );
			fprintf( stderr, "Unit %d: ", unit );
			if ( localFrame[unit].marker[origin].visibility ) fprintf( stderr, "O" );
			else fprintf( stderr, "." );
			fprintf( stderr, "  " );
			if ( localFrame[unit].marker[x_minus].visibility ) fprintf( stderr, "O" );
			else fprintf( stderr, "." );
			if ( localFrame[unit].marker[x_plus].visibility ) fprintf( stderr, "O" );
			else fprintf( stderr, "." );
			fprintf( stderr, "  " );
			if ( localFrame[unit].marker[y_minus].visibility ) fprintf( stderr, "O" );
			else fprintf( stderr, "." );
			if ( localFrame[unit].marker[y_plus].visibility ) fprintf( stderr, "O" );
			else fprintf( stderr, "." );
			fprintf( stderr, "    " );
		}
		fprintf( stderr, "\n" );

	}
	int key = _getch(); // Clear the _kbhit().
	codaTracker.AbortAcquisition();

	// Perform the alignment if the key was an 'A'.
	if ( key == 'a' || key == 'A' ) {
		fprintf( stderr, "Performing alignment O: %d X: %d->%d Y: %d->%d ... ", origin, x_minus, x_plus, y_minus, y_plus );
		codaTracker.PerformAlignment( origin, x_minus, x_plus, y_minus, y_plus, true );
		fprintf( stderr, "OK.\n" );
	}

	fprintf( stderr, "\nCharnwoodAlignment terminated. Press <Return> to close window.\n" );
	_getch();
	return 0;

}

