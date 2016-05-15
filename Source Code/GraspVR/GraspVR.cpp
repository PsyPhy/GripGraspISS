// This is the main DLL file.

#include "stdafx.h"
#include "../Useful/fMessageBox.h"
#include "../OculusInterface/OculusInterface.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

#include "GraspVR.h"

using namespace Grasp;
using namespace PsyPhy;

// Interface to OpenGL windows and HMD.
static OculusDisplayOGL oculusDisplay;

// Mapping and rendering in Oculus.
static OculusMapper oculusMapper;

void GraspVR::Initialize( HINSTANCE hinst ) {

	InitializeVR( hinst );
	InitializeTrackers();

}

void GraspVR::InitializeTrackers( void ) {

	// Create a pose tracker that uses only the Oculus.
	hmdTracker = new PsyPhy::OculusPoseTracker( &oculusMapper );
	fAbortMessageOnCondition( !hmdTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing OculusPoseTracker." );

}

void GraspVR::InitializeVR( HINSTANCE hinst ) {

	ovrResult result;

	// Decide if we are in full screen mode or not.
	// To avoid loosing focus by clicking outside the desktop window it is best to be in fullscreen mode.
	static const bool fullscreen = true;

   // Initializes LibOVR, and the Rift
    OVR::System::Init();
    result = ovr_Initialize( nullptr );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "GraspVR", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
	result = oculusDisplay.InitWindow( hinst, L"GraspVR", fullscreen );
    fAbortMessageOnCondition(   OVR_FAILURE( result ), "GraspVR", "Failed to open window." );

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper.Initialize( &oculusDisplay, fullscreen );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "GraspVR", "Failed to initialize libOVR." );

	// Set up a default GL rendering context.
	glUsefulInitializeDefault();

	// Tone down the intensity a little bit from the default lighting.
	// Auto lighting is used to avoid giving a reference from directional lighting.
	// I don't fully understand the lighting thing, because when I set the intensity to 0
	//  one can still see the objects. But nevertheless this works to reduce the intensity somewhat.
	glUsefulAutoLighting( 0.0 );
	glUsefulShinyMaterial();

	// Create a viewpoint into the scene, using default IPD, FOV and near/far culling.
	// Our basic units are millimeters, so set the near and far accordingly.
	viewpoint = new OculusViewpoint( &oculusMapper, 6.0, 106.0, 100.0, 10000.0 );

	// Set the player's position and orientation to the null position and orientation.
	// Note that the head tracking will be applied relative to this pose.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create all the necessary VR rendering objects.
	CreateObjects();

	// Set the objects where they belong.
	target->SetPosition( 0.0, 0.0, - room_length );
	tool->SetPosition( 0.0, 0.0, - 200.0 );
	tiltPrompt->SetPosition( 0.0, 0.0, - room_length / 2.0 );

	// Initialize state of the projectile.
	projectileCounter = -1;

}

void GraspVR::Release( void ) {

	// Shutdown the Rift.
	ovr_Shutdown();
    OVR::System::Destroy();
	oculusDisplay.CloseWindow();
	oculusDisplay.ReleaseDevice();

}

// Draw the objects that are used during VR rendering.
// Note that only those objects that are currently active are actually drawn.
void GraspVR::Draw( void ) {

	// I am still trying to get specular reflections to work.
	glUsefulDefaultSpecularLighting( 0.7 );
	glUsefulMatteMaterial();

	DrawSky();
	DrawDarkSky();
	DrawRoom();

	glUsefulShinyMaterial();
	DrawTarget();
	DrawTiltPrompt();
	DrawTool();
	DrawProjectiles();
}


// A rendering loop that allows one to toggle on and off the various VR objects.
void GraspVR::DebugLoop( void ) {
	
	// Create a pose tracker that uses only the Oculus.
	PsyPhy::PoseTracker *headPoseTracker = new PsyPhy::OculusPoseTracker( &oculusMapper );
	fAbortMessageOnCondition( !headPoseTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing OculusPoseTracker." );

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay.HandleMessages() ) {

		// Boresight the Oculus tracker on 'B'.
		// This will only affect the PsyPhy rendering.
		if ( oculusDisplay.Key['B'] ) hmdTracker->Boresight();
		if ( oculusDisplay.Key['U'] ) hmdTracker->Unboresight();

		// Disable drawing of all objects.
		if ( oculusDisplay.Key[VK_SPACE] ) {
			target->Disable();
			tool->Disable();
			tiltPrompt->Disable();
			dark_sky->Disable();
			projectiles->Disable();
		}
		// Show the target and the target-specific sky behind it.
		if ( oculusDisplay.Key['T'] ) {
			target->Enable();
			sky->Disable();
			dark_sky->Enable();
		}
		// Show the hand/tool.
		if ( oculusDisplay.Key['H'] ) tool->Enable();
		// Show the tilt prompt.
		if ( oculusDisplay.Key['P'] ) tiltPrompt->Enable();

		// Trigger the projectiles.
		if ( oculusDisplay.Key[VK_RETURN] || oculusDisplay.Button[MOUSE_LEFT] ) { // I used the spacebar because with this interface I cannot find the way to recover the mouse inputs yet
			projectiles->Enable();
			projectileCounter = 0;
			projectiles->SetPosition( tool->position );
		}
		// If the projectiles have been triggered, move them forward in depth.
		if ( projectileCounter >= 0 && projectileCounter <= 1000 ){
			projectileCounter++;
			projectiles->SetPosition( projectiles->position[X], projectiles->position[Y], projectiles->position[Z] - 10.0 );
		}
		// If the projectiles have reached the end of their trajectory, return them to the cocked state.
		else{
			projectileCounter = -1;
			projectiles->Disable();
		}

		// Perform any periodic updating that the head tracker might require.
		fAbortMessageOnCondition( !headPoseTracker->Update(), "PsyPhyOculusDemo", "Error updating head pose tracker." );
		
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
		for (int eye = 0; eye < 2; ++eye) {

			// Get ready to draw into one of the eyes.
 			oculusMapper.SelectEye( eye );
			// Set up the viewing transformations.
			viewpoint->Apply( eye );
			// Draw the objects in the world.
			Draw();
			// Take care of an Oculus bug.
			oculusMapper.DeselectEye( eye );
 
        }

        // Do distortion rendering, Present and flush/sync
		oculusMapper.BlastIt();
	}


}