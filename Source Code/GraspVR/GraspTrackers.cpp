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

// At one point I used a background thread to get the data from the codas.
// This was to keep from blocking the refresh loop. But now we use the GraspTrackerDaemon
// that runs in a separate process to provide the most recent data with virtually zero
// delay when we ask for the data with the current process.
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
	nCodaUnits = codaTracker->nUnits;

	// Start continuous acquisition of Coda marker data for a maximum duration.
	// This is needed if we use the buffered CodaRTnet tracker, but is not necessary
	// if we use the continuous tracker. But we no longer know when we get here
	// which tracker was assigned, so we really shoudn't do this any more. I leave
	// it in comments here just until I get a chance to do a rigorous cleanup.
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
		
#ifdef BACKGROUND_GET_DATA
	// Check if the thread is still running.
	// I don't know why it should stop, but it seems to be happening in Release mode.
	if ( threadHandle ) {
		int result = WaitForSingleObject( threadHandle, 0 );
		if ( result == WAIT_OBJECT_0 ) fAbortMessage( "GraspTrackers", "Retrieval thread has unexpectedly terminated!" );
	}
#endif

	// Retrieve the marker data from the CODA.
	GetMarkerData();
	// Compute the poses.
	UpdatePoseTrackers();
}

// Construct a decimal number that indicates the number of visible markers in each structure.
// 100's digit shows structure 3, 10's digit shows structure 2, 1's digit shows structure 1.
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
		status = status + (int) pow(10.0, ( 2 - group )) * group_count;
	}
	return( status );
}

void GraspDexTrackers::Release( void ) {

	// Do what all flavors of GraspTrackers do.
	GraspTrackers::Release();

#ifdef BACKGROUND_GET_DATA

	// Halt the Coda real-time frame acquisition that is occuring in a background thread.
	if ( threadHandle ) {
		stopMarkerGrabs = true;
		WaitForSingleObject( threadHandle, INFINITE );
	}
#endif

	// Halt the continuous Coda acquisition.
	codaTracker->AbortAcquisition();
	codaTracker->Quit();

}


// The base class GraspTrackers takes care of writing out the pose data from each cycle.
// Derived classes are given the chance to add additional columns to the data file.
// When it is a Dex tracker, we use this feature to add the marker data to the file.
void GraspDexTrackers::WriteAdditionalColumnHeadings( FILE *fp ) {
	for ( int unit = 0; unit < codaTracker->nUnits; unit++ ) codaTracker->WriteColumnHeadings( fp, unit );
}
void GraspDexTrackers::WriteAdditionalTrackerData( FILE *fp ) {
	for ( int unit = 0; unit < codaTracker->nUnits; unit++ ) codaTracker->WriteMarkerData( fp, markerFrame[unit] );
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

	// Give the trackers a chance to get started.
	Sleep( 200 );
	// Allow them to update.
	Update();
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspOculusCodaTrackers is a version of GraspTrackers that uses a combination of Coda and Oculus trackers.

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

	// Give the trackers a chance to get started.
	Sleep( 200 );
	// Allow them to update.
	Update();
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspOculusOnlyTrackers is a version that uses only the Oculus and simulated trackers.

void GraspOculusTrackers::Initialize( void ) {

	// Create a pose tracker that uses only the Oculus.
	
	// This one uses the full Oculus tracker, incuding drift compensation with gravity.
	 hmdTracker = new PsyPhy::OculusHMDPoseTracker( oculusMapper );
	 fAbortMessageOnCondition( !hmdTracker->Initialize(), "GraspOculusTrackers", "Error initializing OculusPoseTracker." );

	// The hand tracker is based on the right hand Oculus Touch.
	handTracker = new PsyPhy::OculusRightHandPoseTracker( oculusMapper );
	fAbortMessageOnCondition( !handTracker->Initialize(), "GraspOculusTrackers", "Error initializing tracker for the hand tracker." );

	// Use the Left Oculus Touch for the chest.
	chestTrackerRaw = new PsyPhy::OculusLeftHandPoseTracker( oculusMapper );
	fAbortMessageOnCondition( !chestTrackerRaw->Initialize(), "GraspOculusTrackers", "Error initializing tracker for the chest." );
	// Filter the chest pose.
	chestTracker = new PoseTrackerFilter( chestTrackerRaw, 2.0 );
	fAbortMessageOnCondition( !chestTracker->Initialize(), "GraspOculusTrackers", "Error initializing PoseTrackerFilter." );
	
	// Create a tracker to control roll movements of the hand for the *-V responses.
	rollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !rollTracker->Initialize(), "GraspOculusTrackers", "Error initializing MouseRollPoseTracker for the mouse tracker." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The rollTracker will then rotate the tool around this constant position.
	rollTracker->OffsetTo( handPoseV );

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspOculusOnlyTrackers is a version that uses only the Oculus and simulated trackers.

void GraspOculusLiteTrackers::Initialize( void ) {

	// Create a pose tracker that uses only the Oculus.
	
	// Pick one of the two Oculus-only hmd trackers by commenting or uncommenting below:
	// This one uses the full Oculus tracker, incuding drift compensation with gravity.
	hmdTracker = new PsyPhy::OculusHMDPoseTracker( oculusMapper );
	// The next one uses our own inertial implementation, but we do not give it an absolute tracker for
	//  drift compensation. It works pretty well when it's only for orientation tracking.
	// hmdTracker = new PsyPhy::OculusCodaPoseTracker( oculusMapper, nullptr );
	fAbortMessageOnCondition( !hmdTracker->Initialize(), "GraspOculusLiteTrackers", "Error initializing OculusPoseTracker." );

	// The hand tracker is a mouse tracker to simulate the other protocols.
	// If rollTrackers is also a mouse tracker, they will move in parallel to each other, 
	// but  since they are never used together this should be OK.
	handTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !handTracker->Initialize(), "GraspOculusLiteTrackers", "Error initializing tracker for the hand tracker." );
	// Set the default position and orientation of the hand.
	// The MouseRollPoseTracker will then rotate the tool around this constant position.
	handTracker->OffsetTo( handPoseK );

	// Create a arrow key tracker to simulate movements of the chest.
	chestTrackerRaw = new PsyPhy::KeyboardPoseTracker( oculusMapper );
	fAbortMessageOnCondition( !chestTrackerRaw->Initialize(), "GraspOculusLiteTrackers", "Error initializing tracker for the chest." );
	// Set the position and orientation of the chest wrt the origin when in V-V mode.
	// The ArrowsRollPoseTracker will then rotate the tool around this constant position.
	// chestTrackerRaw->OffsetTo( chestPoseSim );
	// Filter the chest pose.
	chestTracker = new PoseTrackerFilter( chestTrackerRaw, 2.0 );
	fAbortMessageOnCondition( !chestTracker->Initialize(), "GraspOculusLiteTrackers", "Error initializing PoseTrackerFilter." );
	
	// Create a tracker to control roll movements of the hand for the *-V responses.
	rollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !rollTracker->Initialize(), "GraspOculusLiteTrackers", "Error initializing MouseRollPoseTracker for the mouse tracker." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The rollTracker will then rotate the tool around this constant position.
	rollTracker->OffsetTo( handPoseV );

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GraspSimulatedTrackers avoids all specific hardware. For now, it does no tracking at all.

void GraspSimulatedTrackers::Initialize( void ) {

	hmdTracker = new PsyPhy::WindowsArrowPoseTracker( window, mouseGain );
	fAbortMessageOnCondition( !hmdTracker->Initialize(), "GraspSimulatedTrackers", "Error initializing tracker for the Head." );
	// The hand tracker is a mouse tracker to simulate the other protocols.
	// If rollTrackers is also a mouse tracker, they will move in parallel to each other, 
	// but  since they are never used together this should be OK.
	handTracker = new PsyPhy::WindowsMouseRollPoseTracker( window, mouseGain );
	fAbortMessageOnCondition( !handTracker->Initialize(), "GraspSimulatedTrackers", "Error initializing tracker for the hand." );
	// Set the default position and orientation of the hand.
	// The MouseRollPoseTracker will then rotate the tool around this constant position.
	handTracker->OffsetTo( handPoseK );
	chestTracker = new PsyPhy::NullPoseTracker();
	rollTracker = new PsyPhy::NullPoseTracker();
	// Create a tracker to control roll movements of the hand for the *-V responses.
	rollTracker = new PsyPhy::WindowsMouseRollPoseTracker( window, mouseGain );
	fAbortMessageOnCondition( !rollTracker->Initialize(), "GraspSimulatedTrackers", "Error initializing tracker for hand roll." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The rollTracker will then rotate the tool around this constant position.
	rollTracker->OffsetTo( handPoseV );

}