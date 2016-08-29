///
/// Module:	GraspTrackers
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		03 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#include "stdafx.h"
#include "GraspTrackers.h"

// #define BACKGROUND_GET_DATA

using namespace PsyPhy;
using namespace Grasp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Parameters that can be set to tune the tracking behavior.
//

// How much the tool will turn for a given displacement of the mouse or trackball.(
double GraspTrackers::mouseGain = - 0.001;
// Where to place the tool when in V response mode.
Pose GraspTrackers::handPoseV = {{0.0, 0.0, -500.0}, {0.0, 0.0, 0.0, 1.0}};
// Where to place the tool when in K response mode.
Pose GraspTrackers::handPoseK = {{100.0, -100.0, -500.0}, {0.02, 0.02, 0.0, sqrt( 1.0 - (0.02 * 0.02 + 0.02 * 0.02 ))}};
// How much the torso will turn for each press of an arrow key.
double GraspTrackers::arrowGain = - 0.01;
// Simulate the position of the torso of the subject.
Pose GraspTrackers::chestPoseSim = {{0.0, -200.0, 0.0}, {0.0, 0.0, 0.0, 1.0}};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraspTrackers::Update( void ) {
	// Perform any periodic updating that the trackers might require.
	fAbortMessageOnCondition( !hmdTracker->Update(), "PsyPhyOculusDemo", "Error updating head pose tracker." );
	fAbortMessageOnCondition( !handTracker->Update(), "PsyPhyOculusDemo", "Error updating hand pose tracker." );
	fAbortMessageOnCondition( !chestTracker->Update(), "PsyPhyOculusDemo", "Error updating chest pose tracker." );
	fAbortMessageOnCondition( !rollTracker->Update(), "PsyPhyOculusDemo", "Error updating mouse pose tracker." );
}

void GraspTrackers::Release( void ) {
	hmdTracker->Release();
	handTracker->Release();
	chestTracker->Release();
	rollTracker->Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspDexTrackers uses trackers based on the Oculus and DEX/CODA tracker.
void GraspDexTrackers::InitializeCodaTrackers( void ) {

	// Initialize the connection to the CODA tracking system.
	codaTracker->Initialize();

	// Start continuous acquisition of Coda marker data for a maximum duration.
	// This is needed if we use the classical CodaRTnet tracker, but is not necessary
	// if we use the continuous tracker.
	//codaTracker.StartAcquisition( 600.0 );

#ifdef BACKGROUND_GET_DATA
	// Initiate real-time retrieval of CODA marker frames in a background thread 
	// so that waiting for the frame to come back from the CODA does not slow down
	// the rendering loop.
	requestSharedMemoryParent = false;
	requestSharedMemoryChild = false;
	parentHasPriority = false;

	stopMarkerGrabs = false;
	threadHandle = CreateThread( NULL, 0, GetCodaMarkerFramesInBackground, this, 0, &threadID );
	// SetThreadPriority( threadHandle, THREAD_PRIORITY_HIGHEST );
#endif

	// Create PoseTrackers that combine data from multiple CODAs.
	hmdCascadeTracker = new CascadePoseTracker();
	handCascadeTracker = new CascadePoseTracker();
	chestCascadeTracker = new CascadePoseTracker();

	// for ( int unit = 0; unit < nCodaUnits; unit++ ) {
	for ( int unit = nCodaUnits - 1; unit >= 0 ; unit -- ) {

		hmdCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
		fAbortMessageOnCondition( !hmdCodaPoseTracker[unit]->Initialize(), "GraspVR", "Error initializing hmdCodaPoseTracker[%d].", unit );
		hmdCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );
		hmdCascadeTracker->AddTracker( hmdCodaPoseTracker[unit] );

		handCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
		fAbortMessageOnCondition( !handCodaPoseTracker[unit]->Initialize(), "GraspVR", "Error initializing toolCodaPoseTracker[%d].", unit );
		handCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );
		handCascadeTracker->AddTracker( handCodaPoseTracker[unit] );

		chestCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
		fAbortMessageOnCondition( !chestCodaPoseTracker[unit]->Initialize(), "GraspVR", "Error initializing torsoCodaPoseTracker[%d].", unit );
		chestCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\Chest.bdy" );
		chestCascadeTracker->AddTracker( chestCodaPoseTracker[unit] );
	
	}

	/////////////////////////////////////// TO BE TESTED ////////////////////////////////////////////////////////////
	fAbortMessageOnCondition( !hmdCascadeTracker->Initialize(), "GraspVR", "Error initializing hmdCascadeTracker." );
	fAbortMessageOnCondition( !handCascadeTracker->Initialize(), "GraspVR", "Error initializing handCascadeTracker." );
	fAbortMessageOnCondition( !chestCascadeTracker->Initialize(), "GraspVR", "Error initializing chestCascadeTracker." );
	/////////////////////////////////////// TO BE TESTED ////////////////////////////////////////////////////////////

	// Create pose trackers that simply filter the coda trackers.
	hmdFilteredTracker = new PoseTrackerFilter( hmdCascadeTracker, 2.0 );
	hmdFilteredTracker->Initialize();
	handFilteredTracker = new PoseTrackerFilter( handCascadeTracker, 2.0 );
	handFilteredTracker->Initialize();
	chestFilteredTracker = new PoseTrackerFilter( chestCascadeTracker, 10.0 );
	chestFilteredTracker->Initialize();

}

void GraspDexTrackers::GetMarkerData( void ) {
#ifdef BACKGROUND_GET_DATA
	// Get the current position of the CODA markers.
	for ( int unit = 0; unit < nCodaUnits; unit++ ) {
		GetMarkerFrameFromBackground( unit, &markerFrame[unit] );
	}
#else 
	for ( int unit = 0; unit < nCodaUnits; unit++ ) {
		codaTracker->GetCurrentMarkerFrameUnit( markerFrame[unit], unit );
	}
#endif
}
void GraspDexTrackers::UpdatePoseTrackers( void ) {
	// The base class does this nicely.
	GraspTrackers::Update();
}

void GraspDexTrackers::Update( void ) {
		
	// Check if the thread is still running.
	// I don't know why it should stop, but it seems to be happening in Release mode.
	if ( threadHandle ) {
		int result = WaitForSingleObject( threadHandle, 0 );
		if ( result == WAIT_OBJECT_0 ) fAbortMessage( "GraspTrackers", "Retrieval thread has unexpectedly terminated!" );
	}

	// Retrieve the marker data from the CODA.
	GetMarkerData();
	// Compute the poses.
	UpdatePoseTrackers();
}

// Construct a number that indicates the number of visible markers in each structure.
unsigned int GraspDexTrackers::GetTrackerStatus( void ) {

	unsigned int status = 0;
	int group, id, mrk, unit;
	for ( group = 0, id = 0; group < 3 && id < MAX_MARKERS; group ++ ) {
		int group_count = 0;
		for ( mrk = 0; mrk < 8 && id < MAX_MARKERS; mrk++ , id++ ) {
			bool visibility = false;
			for ( unit = 0; unit < nCodaUnits; unit++ ) {
				if ( markerFrame[unit].marker[id].visibility ) visibility = true;
			}
			if ( visibility ) group_count++;
		}
		status = status + pow(10.0,group) * group_count;
	}
	return( status );
}

void GraspDexTrackers::Release( void ) {

	// Do what all flavors of GraspTrackers do.
	GraspTrackers::Release();

	// Halt the Coda real-time frame acquisition that is occuring in a background thread.
	if ( threadHandle ) {
		stopMarkerGrabs = true;
		WaitForSingleObject( threadHandle, INFINITE );
	}
	// Halt the continuous Coda acquisition.
	codaTracker->AbortAcquisition();
	codaTracker->Quit();

}

void GraspDexTrackers::WriteDataFiles( char *filename_root ) {

	// Output the CODA data to a file.
	char filename[MAX_PATH];
	strncpy( filename, filename_root, sizeof( filename ) );
	strncat( filename, ".mrk", sizeof( filename ) - strlen( ".mrk" ));
	fOutputDebugString( "Writing CODA data to %s.\n", filename );
	FILE *fp = fopen( filename, "w" );
	if ( !fp ) fMessageBox( MB_OK, "File Error", "Error opening %s for write.", filename );

	fprintf( fp, "%s\n", filename );
	fprintf( fp, "Tracker Units: %d\n", codaTracker->GetNumberOfUnits() );
	fprintf( fp, "Frame\tTime" );
	for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
		for ( int unit = 0; unit < codaTracker->GetNumberOfUnits(); unit++ ) {
			fprintf( fp, "\tM%02d.%1d.V\tM%02d.%1d.X\tM%02d.%1d.Y\tM%02d.%1d.Z", mrk, unit, mrk, unit, mrk, unit, mrk, unit  );
		}
	}
	fprintf( fp, "\n" );

	for ( unsigned int frm = 0; frm < codaTracker->nFrames; frm++ ) {
		fprintf( fp, "%05d %9.3f", frm, codaTracker->recordedMarkerFrames[0][frm].time );
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			for ( int unit = 0; unit < codaTracker->GetNumberOfUnits(); unit++ ) {
				fprintf( fp, " %1d",  codaTracker->recordedMarkerFrames[unit][frm].marker[mrk].visibility );
				for ( int i = 0; i < 3; i++ ) fprintf( fp, " %9.3f",  codaTracker->recordedMarkerFrames[unit][frm].marker[mrk].position[i] );
			}
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
	fOutputDebugString( "File %s closed.\n", filename );
}

void GraspDexTrackers::Initialize( void ) {

	// Initialize all of the low-level coda-based pose trackers.
	InitializeCodaTrackers();

	//// Create a tracker to control the roll orientation via the mouse.
	//OculusRemotePoseTracker *oculusRemoteRollTracker;
	//oculusRemoteRollTracker = new PsyPhy::OculusRemotePoseTracker( mouseGain );
	//fAbortMessageOnCondition( !oculusRemoteRollTracker->Initialize(), "GraspVR", "Error initializing OculusRemotePoseTracker." );

	// Now assign the trackers to each body part.
	hmdTracker = hmdFilteredTracker;
	handTracker = handFilteredTracker;
	chestTracker = chestFilteredTracker;
	
	// Place the hand at a constant position relative to the origin.
	rollTracker->OffsetTo( handPoseV );
	//handTracker->OffsetTo( handPoseK );

	// Give the trackers a chance to get started.
	Sleep( 200 );
	// Allow them to update.
	Update();
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspOculusCodaTrackers is a version of GraspTrackers that uses a combination
//  of Coda and Oculus trackers.

void GraspOculusCodaTrackers::Initialize( void ) {

	// Initialize all of the low-leve coda-based pose trackers.
	InitializeCodaTrackers();

	// Create a pose tracker that combines Coda and Oculus data for head tracking.
	oculusCodaPoseTracker = new PsyPhy::OculusCodaPoseTracker( oculusMapper, hmdFilteredTracker );
	fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "GraspVR", "Error initializing oculusCodaPoseTracker." );

	// Create a tracker to control the roll orientation via the mouse.
	// Here we use the mouse tracker tied to the OculusMapper. 
	MouseRollPoseTracker *mouseRollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "GraspVR", "Error initializing oculusCodaPoseTracker." );

	// Now assign the trackers to each body part.
	hmdTracker = oculusCodaPoseTracker;
	handTracker = handFilteredTracker;
	chestTracker = chestFilteredTracker;
	rollTracker = mouseRollTracker;
	
	// Place the hand at a constant position relative to the origin.
	rollTracker->OffsetTo( handPoseV );
	//handTracker->OffsetTo( handPoseK );

	// Give the trackers a chance to get started.
	Sleep( 200 );
	// Allow them to update.
	Update();
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspSIM is a version of GraspVR that uses only the Oculus and simulated trackers.

void GraspSimTrackers::Initialize( void ) {

	// Create a pose tracker that uses only the Oculus.
	
	// Pick one of the two Oculus-only hmd trackers.
	// This one uses the full Oculus tracker, incuding drift compensation with gravity.
	 hmdTracker = new PsyPhy::OculusPoseTracker( oculusMapper );
	// The next one uses our own inertial implementation, but we do not give it an absolute tracker for
	//  drift compensation. It works pretty well when it's only for orientation tracking.
	//hmdTracker = new PsyPhy::OculusCodaPoseTracker( &oculusMapper, nullptr );
	
	 fAbortMessageOnCondition( !hmdTracker->Initialize(), "GraspSimTrackers", "Error initializing OculusPoseTracker." );

	// The hand tracker is a mouse tracker to simulate the other protocols.
	// If rollTrackers is also a mouse tracker, they will move in parallel to each other, 
	// but  since they are never used together this should be OK.
	handTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !handTracker->Initialize(), "GraspSimTrackers", "Error initializing MouseRollPoseTracker for the hand tracker." );
	// Set the default position and orientation of the hand.
	// The MouseRollPoseTracker will then rotate the tool around this constant position.
	handTracker->OffsetTo( handPoseK );

	// Create a arrow key tracker to simulate movements of the chest.
	chestTrackerRaw = new PsyPhy::KeyboardPoseTracker( oculusMapper );
	fAbortMessageOnCondition( !chestTrackerRaw->Initialize(), "GraspSimTrackers", "Error initializing ArrowRollPoseTracker." );
	// Set the position and orientation of the chest wrt the origin when in V-V mode.
	// The ArrowsRollPoseTracker will then rotate the tool around this constant position.
	chestTrackerRaw->OffsetTo( chestPoseSim );
	// Filter the chest pose.
	chestTracker = new PoseTrackerFilter( chestTrackerRaw, 2.0 );
	fAbortMessageOnCondition( !chestTracker->Initialize(), "GraspSimTrackers", "Error initializing PoseTrackerFilter." );
	
	// Create a tracker to control roll movements of the hand for the toV responses.
	rollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !rollTracker->Initialize(), "GraspSimTrackers", "Error initializing MouseRollPoseTracker for the mouse tracker." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The rollTracker will then rotate the tool around this constant position.
	rollTracker->OffsetTo( handPoseV );

}


