/*****************************************************************************

This is the starting point for the GRASP protocol running on Oculus Rift.
The starting point is the sample program "OculusRoomTiny (GL)" provided by Oculus.
See Notes.txt for the original header comments from OculusRoomTiny.

Joe McIntyre 

*****************************************************************************/

// Include the Oculus SDK


// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

// OpenGL rendering functions for Room Tiny.
#include "TinyRoomSceneRenderer.h"

// OpenGL rendering based on PsyPhy OpenGLObjects.
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "OpenGLObjectsRenderer.h"

using namespace OVR;

// Interface to OpenGL windows and HMD.
static OculusDisplayOGL oculusDisplay;

// Mapping and rendering in Oculus.
static OculusMapper oculusMapper;

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

/*****************************************************************************/

ovrResult MainLoop( OculusDisplayOGL *platform )
{
	ovrResult result;

	result = oculusMapper.Initialize( platform );
	if ( OVR_FAILURE ( result ) ) return result;

    // Make scene 
	// Call with 'true' to include GPU intensive object, 'false' to keep it simple.
	// The intensive object appears to be a partition wall of some sort that cuts
	//  the room -- and the table -- in two. On the Perspectives machine it causes the 
	//  table to flicker.
    Scene *roomScene = new Scene( false );
	CreateObjects();

    // Main loop
    bool isVisible = true;
    while ( platform->HandleMessages() )
    {
		// Yaw is the nominal orientation (straight ahead) for the player in the horizontal plane.
        static float Yaw( 3.14159f );  

		// Keyboard inputs to adjust player orientation in the horizontal plane.
        if ( platform->Key[VK_LEFT] )  Yaw += 0.02f;
        if ( platform->Key[VK_RIGHT] ) Yaw -= 0.02f;

        // Keyboard inputs to adjust player position. The forward, backward, leftward and rightward steps
		//  are taken relative to the current viewing orientation of the subjec.
        static Vector3f PlayerPosition( 0.0f, 0.0f, -5.0f );
        if ( platform->Key['W'] || platform->Key[VK_UP] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( 0, 0, -0.05f) );
        if ( platform->Key['S'] || platform->Key[VK_DOWN] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( 0, 0, +0.05f) );
        if ( platform->Key['D'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( +0.05f, 0, 0) );
        if ( platform->Key['A'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( -0.05f, 0, 0) );
															
		// Place the viewpoint at the height specified by the Oculus configuration manager.
		// If the origin of our tracker reference frame is at zero height, this is not necessary
		//  because the tracker will put the viewpoint at the proper, measured height.
		// PlayerPosition.y = ovr_GetFloat( oculusMapper.HMD, OVR_KEY_EYE_HEIGHT, PlayerPosition.y);

		// Animate the cube.
        static float cubeClock = 0;
        roomScene->Models[0]->Pos = Vector3f(9 * sin( cubeClock ), 3, 9 * cos( cubeClock += 0.015f ) );

		// Animate the sphere.
		static double toolP = 0.0;
		tool->SetOrientation( toolP, toolP, - toolP );
		toolP += 1.0;

        if ( isVisible )
        {
			// Read the predicted state of the Oculus head tracker.
			ovrPosef headPose = oculusMapper.ReadHeadPose();
			// Prepare the viewpoints using info about the subject's IPD
			//  as managed by the Oculus Configuration Utility.
			oculusMapper.PrepareViewpoints( headPose );
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
				// roomScene->Render( view, projection );

				// Render the world using the PsyPhy 3D modelling routines.

				// Set the baseline orientation of the viewpoint to the player's position.
				PsyPhy::Vector3 pos;
				pos[X] = PlayerPosition.x;
				pos[Y] = PlayerPosition.y;
				pos[Z] = PlayerPosition.z;
				viewpoint->SetPosition( pos );
				viewpoint->SetOrientation( viewpoint->ToDegrees( - Yaw ), viewpoint->jVector );

				// Get the orientation of the head as measured by the Oculus and convert to 
				//  a PsyPhy position and orientation quaternion.
				ViewpointSetPose( viewpoint, headPose );
				DrawObjects( eye );

				// There is some cleaning up that has to be done after the view for each eye is drawn.
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

    // Initializes LibOVR, and the Rift
    OVR::System::Init();
    ovrResult result = ovr_Initialize(nullptr);
    VALIDATE( OVR_SUCCESS(result), "Failed to initialize libOVR." );
	// Initialize the Oculus-enabled Windows platform.
    VALIDATE( oculusDisplay.InitWindow(hinst, L"GraspOnOculus"), "Failed to open window." );

    // Call Main Loop the first time. 
	// We will eject with an error message if the main loop fails the first time.
	// Pass a pointer to Platform to give access to the HandleMessages() method and other parameters.
	result = MainLoop( &oculusDisplay );
    VALIDATE( OVR_SUCCESS( result ), "An error occurred setting up the GL graphics window.");


	// Shutdown the Rift.
    ovr_Shutdown();
    OVR::System::Destroy();

    return(0);
}
