/*****************************************************************************

This is the starting point for the GRASP protocol running on Oculus Rift.
The starting point is the sample program "OculusRoomTiny (GL)" provided by Oculus.
See Notes.txt for the original header comments from OculusRoomTiny.

Joe McIntyre 

*****************************************************************************/

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
#include "../Trackers/CodaPoseTracker.h"
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

static bool useOVR = false;
static bool usePsyPhy = true;
static bool useCoda = true;

// CodaRTnetTracker can handle up to 28 at 200 Hz.
int nMarkers = 24;
/*****************************************************************************/

// Polling the CODA in the rendering loop can cause non-smooth updating.
// Here we use a thread to get the CODA pose data in the background.
#define N_CODAS 2
bool lockCoda[N_CODAS] = { false, false };
bool stopMarkerGrabs = false;
MarkerFrame codaBuffer[N_CODAS];
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
		for ( int unit = 0; unit < N_CODAS; unit++ ) {
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

	// A buffer to hold the most recent frame of marker data.
	MarkerFrame primaryMarkerFrame, secondaryMarkerFrame;
	PsyPhy::CodaPoseTracker *primaryCodaPoseTracker, *secondaryCodaPoseTracker;

	if ( useCoda ) {
		// Create a PoseTracker that will compute the pose based on marker data and a rigid body model.
		primaryCodaPoseTracker = new PsyPhy::CodaPoseTracker( &primaryMarkerFrame );
		fAbortMessageOnCondition( !primaryCodaPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing primaryCodaPoseTracker." );
		primaryCodaPoseTracker->ReadModelMarkerPositions( "HMD.bdy" );
		// Create a second PoseTracker that uses data from the secondary CODA unit.
		secondaryCodaPoseTracker = new PsyPhy::CodaPoseTracker( &secondaryMarkerFrame );
		fAbortMessageOnCondition( !secondaryCodaPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing secondaryCodaPoseTracker." );
		secondaryCodaPoseTracker->ReadModelMarkerPositions( "HMD.bdy" );
}

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper.Initialize( platform );
	if ( OVR_FAILURE ( result ) ) return result;

	// Create a pose tracker that uses only the Oculus.
	PsyPhy::PoseTracker *oculusPoseTracker = new PsyPhy::OculusPoseTracker( &oculusMapper );
	fAbortMessageOnCondition( !oculusPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing OculusPoseTracker." );

	// Create a pose tracker that combines Coda and Oculus data.
	// For the moment OculusCodaPoseTracker is still in development and uses only Oculus gyro data.
	// So we can go ahead and create it even if the useCoda flag is false.
	PsyPhy::PoseTracker *oculusCodaPoseTracker = new PsyPhy::OculusCodaPoseTracker( &oculusMapper, primaryCodaPoseTracker, secondaryCodaPoseTracker );
	fAbortMessageOnCondition( !oculusCodaPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing oculusCodaPoseTracker." );

	// Create a null tracker. This can be used to work in egocentric coordinates.
	PsyPhy::PoseTracker *nullPoseTracker = new PsyPhy::NullPoseTracker();
	fAbortMessageOnCondition( !nullPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing NullPoseTracker." );

	// Pick which PoseTracker to use.
	PsyPhy::PoseTracker *headPoseTracker = oculusCodaPoseTracker;

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
		if ( platform->Key['B'] ) headPoseTracker->BoresightCurrent();
		if ( platform->Key['U'] ) headPoseTracker->Unboresight();

		// Keyboard inputs to adjust player orientation in the horizontal plane.
        if ( platform->Key[VK_LEFT] )  Yaw += 0.02f;
        if ( platform->Key[VK_RIGHT] ) Yaw -= 0.02f;

        // Keyboard inputs to adjust player position. The forward, backward, leftward and rightward steps
		//  are taken relative to the current viewing orientation of the subjec.
        static OVR::Vector3f PlayerPosition( 0.0f, 0.0f, -5.0f );
        if ( platform->Key['W'] || platform->Key[VK_UP] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( 0, 0, -0.05f) );
        if ( platform->Key['S'] || platform->Key[VK_DOWN] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( 0, 0, +0.05f) );
        if ( platform->Key['D'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( +0.05f, 0, 0) );
        if ( platform->Key['A'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( OVR::Vector3f( -0.05f, 0, 0) );

		if ( useCoda && platform->Key['C'] ) headPoseTracker = primaryCodaPoseTracker;
		if ( useCoda && platform->Key['T'] ) headPoseTracker = secondaryCodaPoseTracker;
		if ( platform->Key['O'] ) headPoseTracker = oculusPoseTracker;
		if ( platform->Key['N'] ) headPoseTracker = nullPoseTracker;
		if ( platform->Key['K'] ) headPoseTracker = oculusCodaPoseTracker;

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
			GetMarkerFrameFromBackground( 0, &primaryMarkerFrame );
			GetMarkerFrameFromBackground( 0, &secondaryMarkerFrame );

			// Perform any periodic updating that the head tracker might require.
			fAbortMessageOnCondition( !headPoseTracker->Update(), "PsyPhyOculusDemo", "Error updating head pose tracker." );

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
			if ( !headPoseTracker->GetCurrentPose( &headPose ) ) {
				static int pose_error_counter = 0;
				fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
			}
			else {
				viewpoint->SetOffset( headPose.position );
				viewpoint->SetAttitude( headPose.orientation );
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
    fAbortMessageOnCondition( !oculusDisplay.InitWindow( hinst, L"GraspOnOculus"), "PsyPhyOculus", "Failed to open window." );

	// Start an acquisition on the CODA.
	if ( useCoda ) codaTracker.StartAcquisition( 600.0 );

	DWORD threadID;
	threadHandle = CreateThread( NULL, 0, GetCodaMarkerFramesInBackground, &codaTracker, 0, &threadID );


    // Call the main loop.
	// Pass a pointer to Platform to give access to the HandleMessages() method and other parameters.
	result = MainLoop( &oculusDisplay );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus",  "An error occurred setting up the GL graphics window.");

	stopMarkerGrabs = true;
	WaitForSingleObject( threadHandle, INFINITE );

	// Shutdown the Rift.
	// I shut it down before halting the CODA just so that the HMD goes dark while the 
	// CODA frames are being retrieved.
    ovr_Shutdown();
    OVR::System::Destroy();
	oculusDisplay.CloseWindow();
	oculusDisplay.ReleaseDevice();

	// Halt the Coda acquisition.
	if (useCoda ) codaTracker.StopAcquisition();

	// Output the CODA data to a file.
	char *filename = "PsyPhyOculusDemo.mrk";
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
    return(0);
}
