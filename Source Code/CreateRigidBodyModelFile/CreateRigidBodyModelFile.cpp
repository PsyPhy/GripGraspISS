// CreateRigidBodyModelFile.cpp : Defines the entry point for the console application.
//

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include "../Useful/fMessageBox.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaPoseTracker.h"

using namespace PsyPhy;

// A device that records 3D marker positions that will drive the 6dof pose trackers.
// This has to be a static variable because otherwise it overflows the stack.
CodaRTnetTracker codaTracker;

int _tmain(int argc, _TCHAR* argv[])
{
	int mrk_list[MAX_MARKERS];
	int mrk_list_length = 0;

	bool confirm = false;

	for ( int arg = 1; arg < argc; arg++ ) {
		if ( !strcmp( argv[arg], "--confirm" ) ) confirm = true;
		else {
			sscanf( argv[arg], "%d", &mrk_list[mrk_list_length] );
			mrk_list_length++;
		}
	}

	fprintf( stderr, "\n%s started.\n", argv[0] );

	// Initialize the connection to the CODA tracking system.
	fprintf( stderr, "Initializing CODA system ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );

	// Acquire the current marker positions for the head tracker.
	MarkerFrame avgFrame;
	fprintf( stderr, "Acquiring marker positions [" );
	for ( int mrk = 0; mrk < mrk_list_length; mrk++ ) {
		// Show which markers.
		fprintf( stderr, " %d", mrk_list[mrk] );
	}
	fprintf( stderr, " ] ... " );
	// Acquire a sample for 5 seconds.
	codaTracker.StartAcquisition( 5.0 );
	while ( codaTracker.GetAcquisitionState() ) Sleep( 10 );
	codaTracker.StopAcquisition();
	fprintf( stderr, "OK.\n" );

	// Compute the average marker positions for the acquired sample.
	codaTracker.ComputeAverageMarkerFrame( avgFrame, codaTracker.recordedMarkerFrames[1], codaTracker.nFrames );
	// Set the position of the model markers from the average acquired sample.
	CodaPoseTracker *codaPoseTracker = new CodaPoseTracker( nullptr );
	codaPoseTracker->SetModelMarkerPositions( mrk_list_length, mrk_list, &avgFrame );
	// Output the model to a file via stdout.
	codaPoseTracker->WriteModelMarkerPositions( stdout );

	if ( confirm ) fMessageBox( MB_OK, "CreateRigidBodyModelFile", "Model Creation Succeeded.\nMarkers = %d\n\n", mrk_list_length );

	return 0;
}

