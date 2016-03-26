/*****************************************************************************

This is the starting point for the GRASP protocol running on Oculus Rift.
The starting point is the sample program "OculusRoomTiny (GL)" provided by Oculus.
See Notes.txt for the original header comments from OculusRoomTiny.

Joe McIntyre 

*****************************************************************************/

// Include the Oculus SDK

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "Kernel/OVR_System.h"
#include "OVR_CAPI_GL.h"

// Interface to Windows
#include "OculusWIN32.h"
#include "OculusMapper.h"

// OpenGL rendering functions for Room Tiny.
#include "TinyRoomSceneRenderer.h"


using namespace OVR;

/*****************************************************************************/

ovrResult MainLoop( OculusDisplayOGL *platform )
{
	ovrResult result;

	result = oculusMapper.Initialize( platform );
	if ( OVR_FAILURE ( result ) ) return result;

    // Make scene - can simplify further if needed
    Scene *roomScene = new Scene(false);

    // Main loop
    bool isVisible = true;
    while ( platform->HandleMessages() )
    {
        // Keyboard inputs to adjust player orientation
        static float Yaw( 3.141592f );  
        if ( platform->Key[VK_LEFT] )  Yaw += 0.02f;
        if ( platform->Key[VK_RIGHT] ) Yaw -= 0.02f;

        // Keyboard inputs to adjust player position
        static Vector3f PlayerPosition( 0.0f, 1.6f, -5.0f );
        if ( platform->Key['W'] || platform->Key[VK_UP] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( 0, 0, -0.05f) );
        if ( platform->Key['S'] || platform->Key[VK_DOWN] )	PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( 0, 0, +0.05f) );
        if ( platform->Key['D'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( +0.05f, 0, 0) );
        if ( platform->Key['A'] )							PlayerPosition += Matrix4f::RotationY( Yaw ).Transform( Vector3f( -0.05f, 0, 0) );
															
		// Place the viewpoint at the height specified by the Oculus configuration manager.
		// If the origin of our tracker reference frame is at zero height, this is not necessary
		//  because the tracker will put the viewpoint at the proper, measured height.
		PlayerPosition.y = ovr_GetFloat( oculusMapper.HMD, OVR_KEY_EYE_HEIGHT, PlayerPosition.y);

		// Animate the cube
        static float cubeClock = 0;
        roomScene->Models[0]->Pos = Vector3f(9 * sin( cubeClock ), 3, 9 * cos( cubeClock += 0.015f ) );

        if ( isVisible )
        {
			oculusMapper.PrepareViewpoints();
            for (int eye = 0; eye < 2; ++eye)
            {
				Matrix4f	view;
				Matrix4f	projection;

				// Get ready to draw into one of the eyes.
 				oculusMapper.SelectEye( eye );

				// Compute the player's orientation transform in the world.
				// The reference point for the viewing pose is determined by the 
				//  player's yaw angle and position that are controlled by the keyboard.
                Matrix4f PlayerOrientation = Matrix4f::RotationY( Yaw );

				// Get the viewing pose and projection matrices based on the programmed player's
				//  position and orientation and the sensed position and orientation of the HMD.
				oculusMapper.GetEyeProjections( eye, PlayerPosition,  PlayerOrientation, &view, &projection );

                // Render the world
				roomScene->Render( view, projection );

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

	// If we get here it means that the previous pass through MainLoop exited either normally
	//  or perhaps with an ovrError_DisplayLost status. If the user asked to quit, MainLoop
	//  kills the program, so we would not be here. So if we get here, we try to run
    do {
		// Be sure to handle any messages while waiting for the main loop to run again.
        if ( !oculusDisplay.HandleMessages() ) break;
		// Sleep a bit before retrying to reduce CPU load while the HMD is disconnected
        Sleep(10);
		// Try the main loop again.
		result = MainLoop( &oculusDisplay );
    } while ( OVR_SUCCESS( result ) || result == ovrError_DisplayLost );


	// Shutdown the Rift.
    ovr_Shutdown();
    OVR::System::Destroy();

    return(0);
}
