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

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/NullPoseTracker.h"
#include "../OculusInterface/OculusPoseTracker.h"

// OpenGL rendering based on PsyPhy OpenGLObjects.
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../PsyPhyOculusDemo/PsyPhyRendering.h"

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

static bool usePsyPhy = true;

/*****************************************************************************/

ovrResult MainLoop( OculusDisplayOGL *platform )
{
	ovrResult result;

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper.Initialize( platform, true );
	if ( OVR_FAILURE ( result ) ) return result;

	// Create a pose tracker that uses only the Oculus.
	PsyPhy::PoseTracker *oculusPoseTracker = new PsyPhy::OculusPoseTracker( &oculusMapper );
	fAbortMessageOnCondition( !oculusPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing OculusPoseTracker." );

	// Create a null tracker. This can be used to work in egocentric coordinates.
	PsyPhy::PoseTracker *nullPoseTracker = new PsyPhy::NullPoseTracker();
	fAbortMessageOnCondition( !nullPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing NullPoseTracker." );

	// Pick which PoseTracker to use.
	PsyPhy::PoseTracker *headPoseTracker = oculusPoseTracker;

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
		if ( platform->Key['B'] ) headPoseTracker->Boresight();
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

		if ( platform->Key['O'] ) headPoseTracker = oculusPoseTracker;
		if ( platform->Key['N'] ) headPoseTracker = nullPoseTracker;
															
		// Place the viewpoint at the height specified by the Oculus configuration manager.
		// If the origin of our tracker reference frame is at zero height, this is not necessary
		//  because the tracker will put the viewpoint at the proper, measured height.
		// PlayerPosition.y = ovr_GetFloat( oculusMapper.HMD, OVR_KEY_EYE_HEIGHT, PlayerPosition.y);

		// Animate the sphere.
		static double toolP = 0.0;
		tool->SetOrientation( toolP, toolP, - toolP );
		toolP += 1.0;

		// Render the world using the PsyPhy 3D modelling routines.
		if ( isVisible && usePsyPhy )
        {

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
			if ( !headPoseTracker->GetCurrentPose( headPose ) ) {
				static int pose_error_counter = 0;
				fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
			}
			else viewpoint->SetPose( headPose.pose );

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
				viewpoint->Apply( (PsyPhy::Eye) eye );
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

   // Initializes LibOVR, and the Rift
#ifdef USE_OCULUS_O_8
	OVR::System::Init();
#endif
    ovrResult result = ovr_Initialize( nullptr );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
    fAbortMessageOnCondition( !oculusDisplay.InitWindow( hinst, L"GraspOnOculus", false ), "PsyPhyOculus", "Failed to open window." );

    // Call the main loop.
	// Pass a pointer to Platform to give access to the HandleMessages() method and other parameters.
	result = MainLoop( &oculusDisplay );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "PsyPhyOculus",  "An error occurred setting up the GL graphics window.\nIs the Oculus connected?");

	// Shutdown the Rift.
	// I shut it down before halting the CODA just so that the HMD goes dark while the 
	// CODA frames are being retrieved.
    ovr_Shutdown();
  #ifdef USE_OCULUS_0_8
	OVR::System::Destroy();
#endif
	oculusDisplay.CloseWindow();
	oculusDisplay.ReleaseDevice();

    return(0);
}
