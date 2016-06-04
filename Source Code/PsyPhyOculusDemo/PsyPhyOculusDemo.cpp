/*****************************************************************************

This is the starting point for the GRASP protocol running on Oculus Rift.
The starting point is the sample program "OculusRoomTiny (GL)" provided by Oculus.
See Notes.txt for the original header comments from OculusRoomTiny.

Joe McIntyre 

*****************************************************************************/

// Flags to set the operating mode.
bool useOVR = false;		// OVR style rendering.
bool usePsyPhy = true;	// PsyPhy style rendering.
bool useCoda = false;	// Do we have a Coda?

#define _CRT_SECURE_NO_WARNINGS

#include <time.h>
#include <stdio.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

// OpenGL rendering functions for Room Tiny.
#include "TinyRoomSceneRenderer.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"

// OpenGL rendering based on PsyPhy OpenGLObjects.
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "PsyPhyRendering.h"

// A device that records 3D marker positions.
// Those marker positions will also drive the 6dof pose trackers.
PsyPhy::CodaRTnetTracker codaTracker;

using namespace OVR;
using namespace PsyPhy;

// Interface to OpenGL windows and HMD.
OculusDisplayOGL oculusDisplay;

// Mapping and rendering in Oculus.
OculusMapper oculusMapper;

void ViewpointSetPose ( PsyPhy::OculusViewpoint *viewpoint, ovrPosef pose ) {
	PsyPhy::Vector3 pos;
	PsyPhy::Quaternion ori;
	pos[X] = pose.Position.x;
	pos[Y] = pose.Position.y;
	pos[Z] = pose.Position.z;
	ori[X] = pose.Orientation.x;
	ori[Y] = pose.Orientation.y;
	ori[Z] = pose.Orientation.z;
	ori[M] = pose.Orientation.w;
	viewpoint->SetOffset( pos );
	viewpoint->SetAttitude( ori );
};

// CodaRTnetTracker can handle up to 28 at 200 Hz. But Grasp only uses 24.
int nMarkers = 24;

// Nominally, we have two CODA cx1 sensing units, but we can support up to 8.
#define MAX_CODA_UNITS	8
int nCodaUnits = 2;

/*****************************************************************************/

// Polling the CODA in the rendering loop can cause non-smooth updating.
// Here we use a thread to get the CODA pose data in the background.
bool lockCoda[MAX_CODA_UNITS] = { false, false, false, false, false, false, false, false };
bool stopMarkerGrabs = false;
MarkerFrame codaBuffer[MAX_CODA_UNITS];
HANDLE threadHandle;

void GetMarkerFrameFromBackground( int unit, MarkerFrame *destination ) {
	while ( lockCoda[unit] );
	lockCoda[unit] = true;
	memcpy( destination, &codaBuffer[unit], sizeof( *destination ) );
	lockCoda[unit] = false;
}

DWORD WINAPI GetCodaMarkerFramesInBackground( LPVOID prm ) {

	PsyPhy::CodaRTnetTracker *coda = (PsyPhy::CodaRTnetTracker *)prm;
	MarkerFrame localFrame;

	while ( !stopMarkerGrabs ) {
		for ( int unit = 0; unit < nCodaUnits; unit++ ) {
			coda->GetCurrentMarkerFrameUnit( localFrame, unit );
			while ( lockCoda[unit] );
			lockCoda[unit] = true;
			memcpy( &codaBuffer[unit], &localFrame, sizeof( codaBuffer[unit] ) );
			lockCoda[unit] = false;
		}
	}
	OutputDebugString( "GetCodaMarkerFramesInBackground() thread exiting.\n" );
	return NULL;
}


/*****************************************************************************/

ovrResult MainLoop( OculusDisplayOGL *platform )
{
	ovrResult result;

	// Create a null tracker to be used when other trackers are not available.
	PsyPhy::PoseTracker *nullPoseTracker = new PsyPhy::NullPoseTracker();
	fAbortMessageOnCondition( !nullPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing NullPoseTracker." );

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper.Initialize( platform, true, false );
	if ( OVR_FAILURE ( result ) ) return result;

	// Create a pose tracker that uses only the Oculus.
	PsyPhy::PoseTracker *oculusPoseTracker = new PsyPhy::OculusPoseTracker( &oculusMapper );
	fAbortMessageOnCondition( !oculusPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing OculusPoseTracker." );

	// Buffers to hold the most recent frame of marker data.
	MarkerFrame markerFrame[MAX_CASCADED_TRACKERS];

	// CodaPoseTrackers compute the pose from a frame of Coda data and a rigid body model.
	// We use one for each Coda unit, but we could use more.
	CodaPoseTracker *hmdCodaPoseTracker[MAX_CASCADED_TRACKERS];
	CodaPoseTracker *handCodaPoseTracker[MAX_CASCADED_TRACKERS];
	CodaPoseTracker *chestCodaPoseTracker[MAX_CASCADED_TRACKERS];

	// CascadePoseTrackers group together multiple PoseTrackers for the same entity.
	// It sequentially tries to get the pose from one after another, stopping as soon as it gets one.
	// We use this mechanism to combine the pose info from each coda unit, because it it better to
	// compute the pose from markers positions measured by the same coda unit. Priority is therefore
	// given to the first coda in the list. Others are used only as needed.
	CascadePoseTracker *hmdCascadeTracker;
	CascadePoseTracker *handCascadeTracker;
	CascadePoseTracker *chestCascadeTracker;

	// For the HMD we can combine pose information from both the HMD and a Coda tracker.
	OculusCodaPoseTracker *oculusCodaPoseTracker;

	// Pointers to the PoseTrackers that are actually selected.
	PoseTracker *hmdTracker;
	PoseTracker *handTracker;
	PoseTracker *chestTracker;

	if ( useCoda ) {

		// Create PoseTrackers that 
		hmdCascadeTracker = new CascadePoseTracker();
		handCascadeTracker = new CascadePoseTracker();
		chestCascadeTracker = new CascadePoseTracker();
		for ( int unit = 0; unit < nCodaUnits; unit++ ) {
			hmdCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
			fAbortMessageOnCondition( !hmdCodaPoseTracker[unit]->Initialize(), "PsyPhyOculusDemo", "Error initializing hmdCodaPoseTracker[%d].", unit );
			hmdCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );
			hmdCascadeTracker->AddTracker( hmdCodaPoseTracker[unit] );

			handCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
			fAbortMessageOnCondition( !handCodaPoseTracker[unit]->Initialize(), "PsyPhyOculusDemo", "Error initializing toolCodaPoseTracker[%d].", unit );
			handCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );
			handCascadeTracker->AddTracker( handCodaPoseTracker[unit] );

			chestCodaPoseTracker[unit] = new CodaPoseTracker( &markerFrame[unit] );
			fAbortMessageOnCondition( !chestCodaPoseTracker[unit]->Initialize(), "PsyPhyOculusDemo", "Error initializing torsoCodaPoseTracker[%d].", unit );
			chestCodaPoseTracker[unit]->ReadModelMarkerPositions( "Bdy\\Chest.bdy" );
			chestCascadeTracker->AddTracker( chestCodaPoseTracker[unit] );
		}
		// Create a pose tracker that combines Coda and Oculus data.
		oculusCodaPoseTracker = new PsyPhy::OculusCodaPoseTracker( &oculusMapper, hmdCascadeTracker );
		fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing oculusCodaPoseTracker." );
		// Pick which PoseTrackers to use.
		hmdTracker = oculusCodaPoseTracker;
		handTracker = handCascadeTracker;
		chestTracker = chestCascadeTracker;
	}
	else {
		// If we are not using the Coda, we can still use the Oculus to measure HMD movements.
		hmdTracker = oculusPoseTracker;
		// For now, we don't use any tracker other than the Coda-based trackers for the hand and torso.
		handTracker = nullPoseTracker;
		chestTracker =nullPoseTracker;
	}

    // Make the scene based on the OculusRoomTiny example.
	// Call with 'true' to include GPU intensive object, 'false' to keep it simple.
	// The intensive object appears to be a partition wall of some sort that cuts
	//  the room -- and the table -- in two. On the Perspectives machine it causes the 
	//  table to flicker.
    Scene *roomScene = new Scene( false );

	// Create the scene and objects for the PsyPhy rendering system.
	CreatePsyPhyObjects();

    // Main loop
    bool isVisible = true;
    while ( platform->HandleMessages() )
    {
		
		// Yaw is the nominal orientation (straight ahead) for the player in the horizontal plane.
        static float Yaw( 3.14159f );  

		// Boresight the Oculus tracker on 'B'.
		// This will only affect the PsyPhy rendering.
		if ( platform->Key['B'] ) hmdTracker->Boresight();
		if ( platform->Key['U'] ) hmdTracker->Unboresight();

		// Keyboard inputs to adjust player orientation in the horizontal plane.
        if ( platform->Key[VK_LEFT] )  Yaw += 0.02f;
        if ( platform->Key[VK_RIGHT] ) Yaw -= 0.02f;

        // Keyboard inputs to adjust player position. The forward, backward, leftward and rightward steps
		//  are taken relative to the current viewing orientation of the subjec.
        static OVR::Vector3f PlayerPosition( 0.0f, 0.0f, 0.0f );
        if ( platform->Key['W'] || platform->Key[VK_UP] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( 0, 0, -0.05f) );
        if ( platform->Key['S'] || platform->Key[VK_DOWN] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( 0, 0, +0.05f) );
        if ( platform->Key['D'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( +0.05f, 0, 0) );
        if ( platform->Key['A'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( -0.05f, 0, 0) );

		if ( platform->Key['O'] ) hmdTracker = oculusPoseTracker;
		if ( platform->Key['N'] ) hmdTracker = nullPoseTracker;
		if ( platform->Key['C'] && useCoda ) hmdTracker = hmdTracker;
		if ( platform->Key['K'] && useCoda ) hmdTracker = oculusCodaPoseTracker;
		if ( platform->Key['H'] && useCoda ) hmdTracker = handTracker;
		if ( platform->Key['T'] && useCoda ) hmdTracker = chestTracker;

		if ( platform->Key['V'] ) useOVR = true, usePsyPhy = false;
		if ( platform->Key['P'] ) usePsyPhy = true, useOVR = false;
															
		// Place the viewpoint at the height specified by the Oculus configuration manager.
		// If the origin of our tracker reference frame is at zero height, this is not necessary
		//  because the tracker will put the viewpoint at the proper, measured height.
		// PlayerPosition.y = ovr_GetFloat( oculusMapper.HMD, OVR_KEY_EYE_HEIGHT, PlayerPosition.y);

		// Animate the cube.
        static float cubeClock = 0;
        roomScene->Models[0]->Pos = OVR::Vector3f(9 * sin( cubeClock ), 3, 9 * cos( cubeClock += 0.015f ) );

		// Animate the sphere.
		static double toolP = 0.0;
		tool->SetOrientation( toolP, toolP, - toolP );
		toolP += 1.0;

        if ( isVisible && useOVR )
        {
			// Read the predicted state of the Oculus head tracker.
			ovrPosef ovrHeadPose = oculusMapper.ReadHeadPose();

            for (int eye = 0; eye < 2; ++eye)
            {
				Matrix4f	view;
				Matrix4f	projection;

				// Compute the player's orientation transform in the world.
				// The reference point for the viewing pose is determined by the 
				//  player's yaw angle and position that are controlled by the keyboard.
                Matrix4f PlayerOrientation = Matrix4f::RotationY( Yaw );

				// Get ready to draw into one of the eyes.
 				oculusMapper.SelectEye( eye );

				// Get the viewing pose and projection matrices based on the programmed player's
				//  position and orientation and the sensed position and orientation of the HMD.
				oculusMapper.GetEyeProjections( eye, PlayerPosition,  PlayerOrientation, &view, &projection );

                // Render the world using the Oculus rendering routines.
				roomScene->Render( view, projection );

				// There is some cleaning up that has to be done after the view for each eye is drawn.
				oculusMapper.DeselectEye( eye );
 			}
		}

		// Render the world using the PsyPhy 3D modelling routines.
		if ( isVisible && usePsyPhy )
        {

			// Get the current position of the CODA markers.
			// codaTracker.GetCurrentMarkerFrameUnit( primaryMarkerFrame, 0 );
			// codaTracker.GetCurrentMarkerFrameUnit( secondaryMarkerFrame, 1 );
			// GetMarkerFrameFromBackground( 0, &primaryMarkerFrame );
			// GetMarkerFrameFromBackground( 0, &secondaryMarkerFrame );
			for ( int unit = 0; unit < nCodaUnits; unit++ ) {
				GetMarkerFrameFromBackground( unit, &markerFrame[unit] );
			}
			// Perform any periodic updating that the trackers might require.
			fAbortMessageOnCondition( !hmdTracker->Update(), "PsyPhyOculusDemo", "Error updating hmd pose tracker." );
			fAbortMessageOnCondition( !handTracker->Update(), "PsyPhyOculusDemo", "Error updating hand pose tracker." );
			fAbortMessageOnCondition( !chestTracker->Update(), "PsyPhyOculusDemo", "Error updating chest pose tracker." );

			// Set the baseline orientation of the viewpoint to the player's position.
			PsyPhy::Vector3 pos;
			pos[X] = PlayerPosition.x;
			pos[Y] = PlayerPosition.y;
			pos[Z] = PlayerPosition.z;
			viewpoint->SetPosition( pos );
			viewpoint->SetOrientation( viewpoint->ToDegrees( Yaw ), viewpoint->jVector );

			// Get the position and orientation of the head and add them to the Player position and orientation.
			// Note that if the tracker returns false, meaning that the tracker does not have a valid new value,
			// the viewpoint offset and attitude are left unchanged, effectively using the last valid tracker reading.
			PsyPhy::TrackerPose headPose;
			if ( !hmdTracker->GetCurrentPose( headPose ) ) {
				static int pose_error_counter = 0;
				fOutputDebugString( "Error reading hmd pose tracker (%03d).\n", ++pose_error_counter );
			}
			else {
				// The position and orientation of the viewpoint is first set by the player position.
				// So we use the offset and attitude to turn the viewpoint according to the tracker
				//  with respect to the player's position.
				viewpoint->SetOffset( headPose.pose.position );
				viewpoint->SetAttitude( headPose.pose.orientation );
			}

			// Prepare the GL graphics state for drawing in a way that is compatible 
			//  with OpenGLObjects. I am doing this each time we get ready to DrawObjects in 
			//  case other GL stuff is going on elsewhere. Otherwise, we could probably
			//  do this just once at the beginning, e.g. in CreateObjects.
			glUsefulPrepareRendering();

			for (int eye = 0; eye < 2; ++eye)
            {

				// Get ready to draw into one of the eyes.
 				oculusMapper.SelectEye( eye );
				// Set up the viewing transformations.
				viewpoint->Apply( eye );
				// Draw the objects in the world.
				DrawPsyPhyObjects();
				// Take care of an Oculus bug.
				oculusMapper.DeselectEye( eye );
 
          }
        }

        // Do distortion rendering, Present and flush/sync
		result = oculusMapper.BlastIt();
        isVisible = (result == ovrSuccess);

   }

    delete roomScene;
    oculusMapper.Release();

    // Return a flag saying whether a retry should be attempted or not.
	// If retryCreate is true on entry, then signal for retry in all cases.
	// If the result is not success, but is ovrError_DisplayLost, then signal for a retry as well.
	// Otherwise, return a flag signalling what error occured.
    return  result;

}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{

	// Initialize the connection to the CODA tracking system.
	if ( useCoda ) codaTracker.Initialize();

   // Initializes LibOVR, and the Rift
    OVR::System::Init();
    ovrResult result = ovr_Initialize( nullptr );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
    fAbortMessageOnCondition( !oculusDisplay.InitWindow( hinst, L"GraspOnOculus", false ), "PsyPhyOculus", "Failed to open window." );

	// Start an acquisition on the CODA.
	if ( useCoda ) {
		
		// Start continuous acquisition of Coda marker data for a maximum duration.
		codaTracker.StartAcquisition( 600.0 );

		// Initiate real-time retrieval of CODA marker frames in a background thread 
		// so that waiting for the frame to come back from the CODA does not slow down
		// the rendering loop.
		DWORD threadID;
		stopMarkerGrabs = false;
		threadHandle = CreateThread( NULL, 0, GetCodaMarkerFramesInBackground, &codaTracker, 0, &threadID );
	}

    // Call the main loop.
	// Pass a pointer to oculusDisplay to give access to the HandleMessages() method and other parameters.
	result = MainLoop( &oculusDisplay );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus",  "An error occurred setting up the GL graphics window.\nIs the Oculus connected?");

	// We exited the main loop, so shutdown the Rift.
	// I shut it down before halting the CODA just so that the HMD goes dark while the 
	// CODA frames are being retrieved.
    ovr_Shutdown();
    OVR::System::Destroy();
	oculusDisplay.CloseWindow();
	oculusDisplay.ReleaseDevice();

	if ( useCoda ) {

		// Halt the Coda real-time frame acquisition that is occuring in a background thread.
		stopMarkerGrabs = true;
		WaitForSingleObject( threadHandle, INFINITE );
		// Halt the continuous Coda acquisition.
		codaTracker.StopAcquisition();

		// Output the CODA data to a file.
		char *filename = "Log\\PsyPhyOculusDemo.mrk";
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

    return(0);
}
