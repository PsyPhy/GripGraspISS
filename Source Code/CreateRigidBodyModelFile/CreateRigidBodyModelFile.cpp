// CreateRigidBodyModelFile.cpp : Defines the entry point for the console application.
//

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
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
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaLegacyPolledTracker.h"
#include "../Trackers/CodaPoseTracker.h"

using namespace PsyPhy;

void usage ( void ) {
	fAbortMessage( "CreateRigidBodyModelFile", 
		"Usage:\n\n  CreateRigidBodyModelFile [--confirm] [--daemon OR --legacy OR none] [--unit=#] m1 m1 ... \n\nm1 ... mn are zero-based marker numbers.\n# is the zero-based CODA cx1 unit number.\nAttention: no spaces around '='." );
}

int _tmain(int argc, _TCHAR* argv[])
{
	int mrk_list[MAX_MARKERS];
	int mrk_list_length = 0;
	int coda_unit = 0;
	bool use_daemon = false;
	bool use_legacy = false;
	bool confirm = false;

	// A device that records 3D marker positions that will drive the 6dof pose trackers.
	Tracker *codaTracker;

	for ( int arg = 1; arg < argc; arg++ ) {
		if ( !strcmp( argv[arg], "--confirm" ) ) confirm = true;
		else if ( !strcmp( "--daemon", argv[arg] ) ) use_daemon = true;
		else if ( !strcmp( "--legacy", argv[arg] ) ) use_legacy = true;
		else if ( !strncmp( argv[arg], "--unit=", strlen( "--unit=" ) ) ) {
			int items = sscanf( argv[arg], "--unit=%d", &coda_unit );
			if ( items != 1 ) usage();
		}
		else if ( argv[arg][0] == '-' ) usage();
		else {
			sscanf( argv[arg], "%d", &mrk_list[mrk_list_length] );
			mrk_list_length++;
		}
	}

	fprintf( stderr, "%s started.\n", argv[0] );

	// Initialize the connection to the CODA tracking system.
	fprintf( stderr, "Initializing CODA system ..." );
	if ( use_daemon ) codaTracker = new PsyPhy::CodaRTnetDaemonTracker();
	else if ( use_legacy ) codaTracker = new PsyPhy::CodaLegacyPolledTracker();
	else codaTracker = new PsyPhy::CodaRTnetContinuousTracker();
	codaTracker->Initialize();
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
	codaTracker->StartAcquisition( 5.0 );
	while ( codaTracker->GetAcquisitionState() ) Sleep( 10 );
	codaTracker->StopAcquisition();
	fprintf( stderr, "OK.\n" );

	// Compute the average marker positions for the acquired sample.
	fprintf( stderr, "Computing model ... " );
	codaTracker->ComputeAverageMarkerFrame( avgFrame, codaTracker->recordedMarkerFrames[coda_unit], codaTracker->nFrames );
	// Set the position of the model markers from the average acquired sample.
	CodaPoseTracker *codaPoseTracker = new CodaPoseTracker( nullptr );
	int nmarkers = codaPoseTracker->SetModelMarkerPositions( mrk_list_length, mrk_list, &avgFrame );
	// Ouput to the file, even if nmarkers is 0. This will create an empty model file but with the time stamp.
	codaPoseTracker->WriteModelMarkerPositions( stdout );
	// Output the model to a file via stdout.
	if ( nmarkers > 0 ) {
		fprintf( stderr, "OK.\n" );
		fprintf( stderr, "Model Creation Succeeded.\n  CODA cx1 Unit = %d\n  Markers = %d\n\n", coda_unit, mrk_list_length );
		if ( confirm )  fMessageBox( MB_OK, "CreateRigidBodyModelFile", "Model Creation Succeeded.\n  CODA cx1 Unit = %d\n  Markers = %d\n\n", coda_unit, mrk_list_length );
	}
	else {
		fprintf( stderr, "aborted due to missing markers.\n" );
		fMessageBox( MB_OK, "CreateRigidBodyModelFile", "Model file is empty." );
	}
	
	return 0;
}

