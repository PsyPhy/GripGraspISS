// CodaVisibility.cpp : Defines the entry point for the console application.
//

// A simple program to show CODA realtime monitoring.

#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
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
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaLegacyPolledTracker.h"

using namespace PsyPhy;

int nUnits = 0;

int main(int argc, char *argv[])
{

	Vector3	offsets[MAX_UNITS];
	Matrix3x3 rotations[MAX_UNITS];
	
	// Show the position of one marker on the screen.
	int which_marker = 20;


	bool use_daemon = false;
	bool use_legacy = false;
	for ( int i = 0; i < argc; i++ ) {
		if ( !strcmp( "--daemon", argv[i] ) ) use_daemon = true;
		else if ( !strcmp( "--legacy", argv[i] ) ) use_legacy = true;
	}

	// A device that records 3D marker positions.
	PsyPhy::Tracker *codaTracker;
	// Run GraspTrackerDaemon and connect to it to get CODA data.
	if ( use_daemon ) codaTracker = new PsyPhy::CodaRTnetDaemonTracker();
	// Use the old method to access the CODA. Used mostly by Michele.
	else if ( use_legacy ) codaTracker = new PsyPhy::CodaLegacyPolledTracker();
	// In the default case, start up the CODA in continuous flux mode.
	else codaTracker = new PsyPhy::CodaRTnetContinuousTracker();

	// Make sure that the GraspTrackerDaemon has time to bind its socket.
	Sleep( 500 );

	MarkerFrame localFrame[2];

	fprintf( stderr, "Initializing coda tracker ..." );
	codaTracker->Initialize( "CodaRTnet.ini" );
	fprintf( stderr, "OK.\n" );

	nUnits = codaTracker->nUnits;

	fprintf( stderr, "Transformations:\n" );
	codaTracker->GetAlignmentTransforms( offsets, rotations );
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) fprintf( stderr, " Unit: %d  Offset: %s  Rotation: %s\n" , unit, codaTracker->vstr( offsets[unit] ), codaTracker->mstr( rotations[unit] ));

	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < nUnits; unit++ ) {
			codaTracker->GetCurrentMarkerFrameUnit( localFrame[unit], unit );
			fprintf( stderr, "U%d: %6.3f ", unit, localFrame[unit].time );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( localFrame[unit].marker[mrk].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				if ( ((mrk+1) % 8) == 0 ) fprintf( stderr, "   " );
			}
			fprintf( stderr, "  " );
		}
		// Indicate the amount of misalignment between XYZ coordinates taken by CODA 0 vs 1;
		if ( nUnits > 1 ) {
			fprintf( stderr, "Algt: " );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				int count = 0;
				double sum = 0.0;
				if ( localFrame[0].marker[mrk].visibility && localFrame[1].marker[mrk].visibility ) {
					Vector3 delta;
					codaTracker->SubtractVectors( delta, localFrame[0].marker[mrk].position, localFrame[1].marker[mrk].position );
					double distance = codaTracker->VectorNorm( delta );
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
			fprintf( stderr, "%s %s\n", codaTracker->vstr( localFrame[0].marker[which_marker].position), codaTracker->vstr( localFrame[1].marker[which_marker].position ));
		}
		else  fprintf( stderr, "%s\n", codaTracker->vstr( localFrame[0].marker[which_marker].position) );
		Sleep( 40 );
	}

	// Runs until there is a key hit.
	int key = _getch(); // Clear the _kbhit().
	codaTracker->AbortAcquisition();
	codaTracker->Quit();

	fprintf( stderr, "\nCodaVisibility terminated. Press <Return> to close window.\n" );
	_getch();

	return 0;
}

