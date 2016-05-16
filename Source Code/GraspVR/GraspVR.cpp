// This is the main DLL file.

#include "stdafx.h"
#include "../Useful/fMessageBox.h"
#include "../OculusInterface/OculusInterface.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../OculusInterface/MousePoseTrackers.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

#include "GraspVR.h"

using namespace Grasp;
using namespace PsyPhy;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Parameters that can be set to tune the stimuli.
//

// How much the tool will turn for a given displacement of the mouse or trackball.
double GraspVR::mouseGain = - 0.001;
// Where to place the tool when in V-V mode.
Pose GraspVR::handPoseVV = {{0.0, 0.0, -200.0}, {0.0, 0.0, 0.0, 1.0}};
// How much the torso will turn for each press of an arrow key.
double GraspVR::arrowGain = - 0.01;
// Simulate the position of the torso of the subject.
Pose GraspVR::chestPoseSim = {{0.0, -200.0, 0.0}, {0.0, 0.0, 0.0, 1.0}};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Hardware Interface
//

// I don't remember why these are here as static global variables, instead of as 
// static instance variables of the GraspVR class, but I don't want to spend time figuring it out.

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

	// Create a mouse tracker to simulate movements of the hand.
	handTracker = new PsyPhy::MouseRollPoseTracker( &oculusMapper, mouseGain );
	fAbortMessageOnCondition( !handTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing MouseRollPoseTracker." );
	// Set the position and orientation of the tool wrt the origin when in V-V mode.
	// The MouseRollPoseTracker will then rotate the tool around this constant position.
	handTracker->BoresightTo( handPoseVV );

	// Create a arrow key tracker to simulate movements of the hand.
	chestTracker = new PsyPhy::ArrowsRollPoseTracker( &oculusMapper, arrowGain );
	fAbortMessageOnCondition( !chestTracker->Initialize(), "PsyPhyOculusDemo", "Error initializing ArrowRollPoseTracker." );
	// Set the position and orientation of the chest wrt the origin when in V-V mode.
	// The ArrowsRollPoseTracker will then rotate the tool around this constant position.
	chestTracker->BoresightTo( chestPoseSim );


}


void GraspVR::UpdateTrackers( void ) {

		// Perform any periodic updating that the trackers might require.
		fAbortMessageOnCondition( !hmdTracker->Update(), "PsyPhyOculusDemo", "Error updating head pose tracker." );
		fAbortMessageOnCondition( !handTracker->Update(), "PsyPhyOculusDemo", "Error updating hand pose tracker." );

		// Get the position and orientation of the head and add them to the Player position and orientation.
		// Note that if the tracker returns false, meaning that the tracker does not have a valid new value,
		// the viewpoint offset and attitude are left unchanged, effectively using the last valid tracker reading.
		TrackerPose headPose;
		if ( !hmdTracker->GetCurrentPose( headPose ) ) {
			static int pose_error_counter = 0;
			fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
		}
		else {
			viewpoint->SetPose( headPose.pose );
		}

		// Track movements of the hand marker array.
		TrackerPose handPose;
		if ( !handTracker->GetCurrentPose( handPose ) ) {
			static int pose_error_counter = 0;
			fOutputDebugString( "Error reading hand pose tracker (%03d).\n", ++pose_error_counter );
		}
		else {
			renderer->tool->SetPose( handPose.pose );
		}

		// Boresight the Oculus tracker on 'B'.
		// This will only affect the PsyPhy rendering.
		if ( oculusDisplay.Key['B'] ) hmdTracker->Boresight();
		if ( oculusDisplay.Key['U'] ) hmdTracker->Unboresight();
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
	renderer = new GraspGLObjects();
	renderer->CreateVRObjects();
	renderer->PlaceVRObjects();

	// Initialize state of the objects.
	renderer->lightSky->Enable();
	renderer->darkSky->Disable();
	renderer->room->Enable();
	renderer->target->Disable();
	renderer->tiltPrompt->Disable();
	renderer->tool->Disable();
	renderer->projectiles->Disable();

}

void GraspVR::Release( void ) {

	// Shutdown the Rift.
	ovr_Shutdown();
    OVR::System::Destroy();
	oculusDisplay.CloseWindow();
	oculusDisplay.ReleaseDevice();

}

ProjectileState GraspVR::HandleProjectiles( void ) {
	
		// Trigger the projectiles, but only if it is not already triggered.
		if ( oculusDisplay.Key[VK_RETURN] || oculusDisplay.Button[MOUSE_LEFT] && !renderer->projectiles->enabled ) { 
			// Position the projectiles where the tool is now.
			renderer->projectiles->SetPosition( renderer->tool->position );
			renderer->projectiles->SetOrientation( renderer->tool->orientation );
			// Make the projectiles visible.
			renderer->projectiles->Enable();
			// Hide the tool.
			renderer->tool->Disable();
			return( running );
		}
		else if ( renderer->projectiles->position[Z] < renderer->darkSky->position[Z] ) {
			renderer->projectiles->Disable();
			return( miss );
		}
		else if ( renderer->projectiles->enabled ) {
			// If the projectiles have been triggered and have not reached their destination, move them forward in depth.
			renderer->projectiles->SetPosition( renderer->projectiles->position[X], renderer->projectiles->position[Y], renderer->projectiles->position[Z] - 10.0 );
			return( running );
		}
		else return( cocked );

}

double GraspVR::SetDesiredHeadRoll( double roll_angle ) {
	desiredHeadRoll = roll_angle;
	return( desiredHeadRoll );
}
double GraspVR::SetDesiredHandRoll( double roll_angle ) {
	desiredHandRoll = roll_angle;
	return( desiredHandRoll );
}

// A rendering loop that allows one to toggle on and off the various VR objects.
void GraspVR::DebugLoop( void ) {

	SetDesiredHeadRoll( 20.0 );
	SetDesiredHandRoll( -35.0 );
	
	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay.HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Handle triggering and moving the projectiles.
		HandleProjectiles();

		// Prompt the subject to achieve the desired head orientation.

		//
		// Handle other key presses.
		//

		// Disable drawing of all objects.
		if ( oculusDisplay.Key[VK_SPACE] ) {
			renderer->target->Disable();
			renderer->tool->Disable();
			renderer->tiltPrompt->Disable();
			renderer->projectiles->Disable();
			renderer->lightSky->Enable();
			renderer->darkSky->Disable();
		}
		// Show the target and the target-specific sky behind it.
		if ( oculusDisplay.Key['T'] ) {
			renderer->target->Enable();
			renderer->lightSky->Disable();
			renderer->darkSky->Enable();
		}
		// Show the hand/tool.
		if ( oculusDisplay.Key['H'] ) renderer->tool->Enable();
		// Show the tilt prompt.
		if ( oculusDisplay.Key['P'] ) renderer->tiltPrompt->Enable();

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
			renderer->DrawVR();
			// Take care of an Oculus bug.
			oculusMapper.DeselectEye( eye );
 
        }

        // Do distortion rendering, Present and flush/sync
		oculusMapper.BlastIt();
	}


}