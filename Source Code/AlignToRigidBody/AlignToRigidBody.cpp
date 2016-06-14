// AlignToRigidBody.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Some convenient routines for outputing debug messages.
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaPoseTracker.h"

using namespace PsyPhy;

// A device that records 3D marker positions that will drive the 6dof pose trackers.
// This has to be a static variable because otherwise it overflows the stack.
CodaRTnetTracker codaTracker;

// How long to sample data.
float duration = 1.0f;

// Rigid body model file.
char *body_file = "Bdy\\Hand.bdy";

int _tmain(int argc, char *argv[])
{

	MarkerFrame avgFrame;
	TrackerPose pose;

	// Can take a single command line argument which is the name of the rigid body file.
	if ( argc > 1 ) body_file = argv[1];
	fprintf( stderr, "AlignToRigidBody.exe\n\nRigid Body Filename: %s\n\n", body_file );

	fprintf( stderr, "Initializing CODA ... " );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Starting acquisition ... " );
	codaTracker.StartAcquisition( duration );
	fprintf( stderr, "OK.\nAcquiring ... " );
	while ( codaTracker.GetAcquisitionState() ) Sleep( 20 );
	codaTracker.StopAcquisition();
	fprintf( stderr, "OK.\n\n" );
	codaTracker.Quit();

	for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {

		// Compute the average marker positions for the acquired sample.
		codaTracker.ComputeAverageMarkerFrame( avgFrame, codaTracker.recordedMarkerFrames[unit], codaTracker.nFrames );
		// Set the position of the model markers from the average acquired sample.
		CodaPoseTracker *codaPoseTracker = new CodaPoseTracker( &avgFrame );
		codaPoseTracker->ReadModelMarkerPositions( body_file );
		codaPoseTracker->GetCurrentPose( pose );
		PsyPhy::Matrix3x3 rotation_matrix;
		codaPoseTracker->QuaternionToMatrix( rotation_matrix, pose.pose.orientation );
		fprintf( stderr, "Unit: %d Position: %s\n", unit, codaPoseTracker->vstr( pose.pose.position ));
		fprintf( stderr, "         Orientation: %s\n", codaPoseTracker->qstr( pose.pose.orientation ));
		fprintf( stderr, "         Orientation: %s\n", codaPoseTracker->mstr( rotation_matrix ));

	}

	system( "bin\\WinSCP.com /command \"open ftp://administrator:dex@10.80.12.103\" \"cd CodaMotion\" \"cd RTNet\" \"ls\" \"cd Binaries\" \"ls\" \"rm codaRTModuleCX1-Alignment.dat\" \"ls\" \"exit\" & pause" );
	 
	fprintf( stderr, "\nAlignToRigidBody.exe terminated. Press <Return> to close window.\n" );
	_getch();

	return 0;
}

