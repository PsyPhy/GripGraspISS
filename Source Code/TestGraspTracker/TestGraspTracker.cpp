// TestGraspTracker.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include "../OculusInterface/OculusInterface.h"


OculusDisplayOGL	window;
OculusMapper		mapper;

#include "../GraspVR/GraspTrackers.h"

using namespace Grasp;

int main( int argc, char *argv[] )
{

	bool use_coda = false;

	GraspTrackers *trackers;

	if ( use_coda ) {
		CodaRTnetTracker *codaTracker = new CodaRTnetDaemonTracker();
		trackers = new GraspOculusCodaTrackers( &mapper, codaTracker );
	}
	else trackers = new GraspSimTrackers( &mapper );

	ovrResult result = ovr_Initialize( nullptr );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
	fAbortMessageOnCondition( !window.InitWindow( NULL, L"GraspOnOculus", false ), "PsyPhyOculus", "Failed to open window." );

	mapper.Initialize( &window, true );
	trackers->Initialize();

	while ( window.HandleMessages() ) {
		TrackerPose chest_pose;
		trackers->Update();
		trackers->chestTracker->GetCurrentPose( chest_pose );
		printf( "Chest: %s %s\n", trackers->vstr( chest_pose.pose.position ), trackers->qstr( chest_pose.pose.orientation ) );
		Sleep( 500 );
	}



	return 0;
}

