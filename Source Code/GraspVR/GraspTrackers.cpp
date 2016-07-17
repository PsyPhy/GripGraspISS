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

#include "../OculusInterface/OculusInterface.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaRTnetTracker.h"

#include "../OculusInterface/MousePoseTrackers.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "GraspTrackers.h"

using namespace PsyPhy;
using namespace Grasp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Parameters that can be set to tune the tracking behavior.
//

// How much the tool will turn for a given displacement of the mouse or trackball.
double GraspTrackers::mouseGain = - 0.001;
// Where to place the tool when in V response mode.
Pose GraspTrackers::handPoseV = {{0.0, 0.0, -350.0}, {0.0, 0.0, 0.0, 1.0}};
// Where to place the tool when in K response mode.
Pose GraspTrackers::handPoseK = {{0.0, 0.0, -500.0}, {0.0, 0.0, 0.0, 1.0}};
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

// GraspDexTrackers is any version of GraspVR that uses the coda. 
// NB It does not define the hmd, hand and chest tracker. That should be done in another flavor built on this one.
void GraspDexTrackers::Initialize( void ) {

	// Initialize the connection to the CODA tracking system.
	codaTracker.Initialize();

	// Start continuous acquisition of Coda marker data for a maximum duration.
	codaTracker.StartAcquisition( 600.0 );

	// Initiate real-time retrieval of CODA marker frames in a background thread 
	// so that waiting for the frame to come back from the CODA does not slow down
	// the rendering loop.
	for ( int i = 0; i < nCodaUnits; i++ ) lockSharedMarkerFrame[i] = false;
	stopMarkerGrabs = false;
	threadHandle = CreateThread( NULL, 0, GetCodaMarkerFramesInBackground, this, 0, &threadID );

	// Create PoseTrackers that combine data from multiple CODAs.
	hmdCascadeTracker = new CascadePoseTracker();
	handCascadeTracker = new CascadePoseTracker();
	chestCascadeTracker = new CascadePoseTracker();

	for ( int unit = 0; unit < nCodaUnits; unit++ ) {
	//for ( int unit = nCodaUnits; unit >= 0 ; unit-- ) {
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

	// Create a pose tracker that combines Coda and Oculus data.
	oculusCodaPoseTracker = new PsyPhy::OculusCodaPoseTracker( oculusMapper, hmdCascadeTracker );
	fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "GraspVR", "Error initializing oculusCodaPoseTracker." );

	// Create a tracker to control the roll orientation via the mouse.
	mouseRollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "GraspVR", "Error initializing oculusCodaPoseTracker." );

	// Now assign the trackers to each body part.
	hmdTracker = oculusCodaPoseTracker;
	handTracker = handCascadeTracker;
	chestTracker = chestCascadeTracker;
	rollTracker = mouseRollTracker;
	
	// Place the hand at a constant position relative to the origin.
	rollTracker->OffsetTo( handPoseV );
	//handTracker->OffsetTo( handPoseK );

	// Give the trackers a chance to get started.
	Sleep( 200 );
	// Allow them to update.
	Update();

}

void GraspDexTrackers::Update( void ) {

	// Get the current position of the CODA markers.
	for ( int unit = 0; unit < nCodaUnits; unit++ ) {
		GetMarkerFrameFromBackground( unit, &markerFrame[unit] );
	}

	// Do what all flavors of GraspTrackers do.
	GraspTrackers::Update();

}

void GraspDexTrackers::Release( void ) {

	// Do what all flavors of GraspTrackers do.
	GraspTrackers::Release();

	// Halt the Coda real-time frame acquisition that is occuring in a background thread.
	stopMarkerGrabs = true;
	WaitForSingleObject( threadHandle, INFINITE );
	// Halt the continuous Coda acquisition.
	codaTracker.AbortAcquisition();

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
	fprintf( fp, "Tracker Units: %d\n", codaTracker.GetNumberOfUnits() );
	fprintf( fp, "Frame\tTime" );
	for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
		for ( int unit = 0; unit < codaTracker.GetNumberOfUnits(); unit++ ) {
			fprintf( fp, "\tM%02d.%1d.V\tM%02d.%1d.X\tM%02d.%1d.Y\tM%02d.%1d.Z", mrk, unit, mrk, unit, mrk, unit, mrk, unit  );
		}
	}
	fprintf( fp, "\n" );

	for ( int frm = 0; frm < codaTracker.nFrames; frm++ ) {
		fprintf( fp, "%05d %9.3f", frm, codaTracker.recordedMarkerFrames[0][frm].time );
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			for ( int unit = 0; unit < codaTracker.GetNumberOfUnits(); unit++ ) {
				fprintf( fp, " %1d",  codaTracker.recordedMarkerFrames[unit][frm].marker[mrk].visibility );
				for ( int i = 0; i < 3; i++ ) fprintf( fp, " %9.3f",  codaTracker.recordedMarkerFrames[unit][frm].marker[mrk].position[i] );
			}
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
	fOutputDebugString( "File %s closed.\n", filename );
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
	chestTracker = new PsyPhy::ArrowsRollPoseTracker( oculusMapper, arrowGain );
	fAbortMessageOnCondition( !chestTracker->Initialize(), "GraspSimTrackers", "Error initializing ArrowRollPoseTracker." );
	// Set the position and orientation of the chest wrt the origin when in V-V mode.
	// The ArrowsRollPoseTracker will then rotate the tool around this constant position.
	chestTracker->OffsetTo( chestPoseSim );

	// Create a tracker to control roll movements of the hand for the toV responses.
	rollTracker = new PsyPhy::MouseRollPoseTracker( oculusMapper, mouseGain );
	fAbortMessageOnCondition( !rollTracker->Initialize(), "GraspSimTrackers", "Error initializing MouseRollPoseTracker for the mouse tracker." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The rollTracker will then rotate the tool around this constant position.
	rollTracker->OffsetTo( handPoseV );

}


