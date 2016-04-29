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
	
	MarkerFrame localFrame;

	fprintf( stderr, "Initializing CODA ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Starting acquisition ... " );
	codaTracker.StartAcquisition( 600.0 );
	fprintf( stderr, "OK.\n\n" );

	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < 2; unit++ ) {
			codaTracker.GetCurrentMarkerFrameUnit( localFrame, unit );
			fprintf( stderr, "Unit %d: ", unit );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( localFrame.marker[mrk].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				if ( ((mrk+1) % 8) == 0 ) fprintf( stderr, " " );
			}
			fprintf( stderr, "  " );
		}
		fprintf( stderr, "                                                     \r" );
	}
	codaTracker.StopAcquisition();

	return 0;
}

