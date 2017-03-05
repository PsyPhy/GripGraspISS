// CharnwoodAlignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

using namespace PsyPhy;

// A device that records 3D marker positions that will drive the 6dof pose trackers.
// This has to be a static variable because otherwise it overflows the stack.
CodaRTnetTracker codaTracker;


int main( int argc,  char *argv[] )
{
	int response;

	MarkerFrame localFrame[2];

	int origin = 7;
	int x_minus = 7;
	int x_plus = 17;
	int y_minus = 7;
	int y_plus = 14;

	//int origin = 0;
	//int x_minus = 2;
	//int x_plus = 0;
	//int y_minus = 1;
	//int y_plus = 0;

	//int origin = 3;
	//int x_minus = 8;
	//int x_plus = 3;
	//int y_minus = 8;
	//int y_plus = 14;

	for ( int i = 0; i < argc; i++ ) {
		if ( !strncmp( argv[i], "--origin=", strlen( "--origin=" ) ) ) sscanf( argv[i], "--origin=%d", &origin );
		if ( !strncmp( argv[i], "--x_minus=", strlen( "--x_minus=" ) ) ) sscanf( argv[i], "--x_minus=%d", &x_minus );
		if ( !strncmp( argv[i], "--x_plus=", strlen( "--x_plus=" ) ) ) sscanf( argv[i], "--x_plus=%d", &x_plus );
		if ( !strncmp( argv[i], "--y_minus=", strlen( "--y_minus=" ) ) ) sscanf( argv[i], "--y_minus=%d", &y_minus );
		if ( !strncmp( argv[i], "--y_plus=", strlen( "--y_plus=" ) ) ) sscanf( argv[i], "--y_plus=%d", &y_plus );
	}
	
	response = fMessageBox( MB_OKCANCEL, "CharnwoodAlignment", "Origin: %d\nX Axis: %d -> %d\nY Axis: %d -> %d", origin, x_minus, x_plus, y_minus, y_plus );	
	if ( response == IDCANCEL ) exit( -1 );

	// 
	// Loop until the alignment is successful or the user aborts.
	//
	while (1) {

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
				fprintf( stderr, "Unit %d: Origin (%d):", unit, origin );
				if ( localFrame[unit].marker[origin].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				fprintf( stderr, "  X (%d->%d):", x_minus, x_plus );
				if ( localFrame[unit].marker[x_minus].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				fprintf( stderr, "->" );
				if ( localFrame[unit].marker[x_plus].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				fprintf( stderr, "  Y (%d->%d):", y_minus, y_plus );
				if ( localFrame[unit].marker[y_minus].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				fprintf( stderr, "->" );
				if ( localFrame[unit].marker[y_plus].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				fprintf( stderr, "    " );
			}
			fprintf( stderr, "\n" );

		}
		int key = _getch(); // Clear the _kbhit().

		// Stop the stream of acquisitions so that the alignment procedure can be run.
		codaTracker.AbortAcquisition();

		// Perform the alignment if the key was an 'A'.
		if ( key == 'a' || key == 'A' ) {
			fprintf( stderr, "Performing alignment O: %d X: %d->%d Y: %d->%d ... ", origin, x_minus, x_plus, y_minus, y_plus );
			// Perform the alignment. The true at the end says to show the results popup even if the alignment is successfull.
			response = codaTracker.PerformAlignment( origin, x_minus, x_plus, y_minus, y_plus, true );
			if ( response == IDOK ) fprintf( stderr, "OK.\n" );
			else fprintf( stderr, "terminated with %s.\n", ( response == IDRETRY ? "Retry" : ( response == IDABORT ? "Abort" : ( response == IDIGNORE ? "Ignore" : "unrecognized response button" ))));
		}
		// If the user did not request the alignment, just exit.
		else {
			response = IDABORT;
			fprintf( stderr, "Exiting without attempting to peform alignment.\n" );
		}

		// Stop the tracker because in the current state of the code we cannot start a new acquisition if we don't.
		codaTracker.Quit();

		// Break out of the retry loop on anything but a Retry response in the popup.
		// Note that if the alignment was successful, Retry is not an option given to the user.
		// so a successfull alignment will necessarily cause the loop and program to exit.
		// Also, if the user did not press 'a' or 'A' they did not even see a popup and Abort is assumed.
		if ( response != IDRETRY ) break;
	}

	if ( response != IDOK ) MessageBox( NULL, "WARNING!\nCODA Alignment was not performed!", "CharnwoodAlignment", MB_OK | MB_ICONEXCLAMATION );

}

