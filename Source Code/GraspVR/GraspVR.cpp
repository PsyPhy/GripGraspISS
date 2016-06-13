// This is the main DLL file.

#include "stdafx.h"
#include "../Useful/fMessageBox.h"

#include "../OculusInterface/OculusInterface.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaRTnetTracker.h"

#include "../OculusInterface/MousePoseTrackers.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

#include "GraspVR.h"

using namespace Grasp;
using namespace PsyPhy;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Trackers 
//

void GraspVR::InitializeTrackers( void ) {
	trackers->Initialize();
}

void GraspVR::UpdateTrackers( void ) {

	trackers->Update();

	// Get the position and orientation of the head and update the viewpoint accordingly.
	// Note that if the tracker returns false, meaning that the tracker does not have a valid new value,
	// the viewpoint offset and attitude are left unchanged, effectively using the last valid tracker reading.
	TrackerPose headPose;
	if ( !trackers->hmdTracker->GetCurrentPose( headPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TrackerPose mousePose;
		viewpoint->SetPose( headPose.pose );
		renderer->hud->SetPose( headPose.pose );
		if ( !trackers->mouseTracker->GetCurrentPose( mousePose ) ) {
			static int pose_error_counter = 0;
			fOutputDebugString( "Error reading mouse pose tracker (%03d).\n", ++pose_error_counter );
		}
		else renderer->vTool->SetAttitude( mousePose.pose.orientation );
	}

	// Track movements of the hand marker array.
	TrackerPose handPose;
	if ( !trackers->handTracker->GetCurrentPose( handPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading hand pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		renderer->hand->SetPose( handPose.pose );
	}

}

void GraspVR::ReleaseTrackers( void ) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// VR 
//

void GraspVR::InitializeVR( HINSTANCE hinst ) {

	ovrResult result;

	// Decide if we are in full screen mode or not.
	// To avoid losing focus by clicking outside the desktop window it is best to be in fullscreen mode.
	static const bool fullscreen = true;

	// Initializes LibOVR, and the Rift
#ifdef USE_OCULUS_O_8
	OVR::System::Init();
#endif
	result = ovr_Initialize( nullptr );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "GraspVR", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
	result = oculusDisplay->InitWindow( hinst, L"GraspVR", fullscreen );
	fAbortMessageOnCondition(   OVR_FAILURE( result ), "GraspVR", "Failed to open window." );

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper->Initialize( oculusDisplay, true, fullscreen );
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
	viewpoint = new OculusViewpoint( oculusMapper, 6.0, 1.0, 10000.0 );

	// Set the player's position and orientation to the null position and orientation.
	// Note that the head tracking will be applied relative to this pose.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create all the necessary VR rendering objects.
	renderer = new GraspGLObjects();
	renderer->CreateVRObjects();
	renderer->PlaceVRObjects();

	// Initialize state of the objects.
	renderer->starrySky->Enable();
	renderer->darkSky->Disable();
	renderer->room->Enable();
	renderer->response->Disable();
	renderer->orientationTarget->Disable();
	renderer->positionOnlyTarget->Disable();
	renderer->tiltPrompt->Disable();
	renderer->successIndicator->Disable();
	renderer->timeoutIndicator->Disable();
	renderer->headMisalignIndicator->Disable();
	renderer->vTool->Disable();
	renderer->kTool->Disable();
	renderer->kkTool->Disable();
	renderer->projectiles->Disable();

	currentProjectileState = cocked;

}

void GraspVR::Release( void ) {

	// Shutdown the trackers.
	ReleaseTrackers();

	// Shutdown the Rift.
	ovr_Shutdown();
#ifdef USE_OCULUS_0_8
	OVR::System::Destroy();
#endif
	oculusDisplay->CloseWindow();
	oculusDisplay->ReleaseDevice();

}

ProjectileState GraspVR::TriggerProjectiles( void ) {
	if ( currentProjectileState == running ) return( currentProjectileState );
	// Position the projectiles where the tool is now.
	renderer->projectiles->SetPosition( renderer->hand->position );
	renderer->projectiles->SetOrientation( renderer->hand->orientation );
	// Make the projectiles visible.
	renderer->projectiles->Enable();
	return( currentProjectileState = running );
}

ProjectileState GraspVR::HandleProjectiles( void ) {

	switch ( currentProjectileState ) {

	case running:
		if ( renderer->projectiles->position[Z] < renderer->darkSky->position[Z] ) {
			// Normally here we would decide if it is a hit or a miss and set the state 
			// accordingly. For the moment, everything is a miss.
			currentProjectileState = miss;
			renderer->projectiles->Disable();
		}
		else {
			// If the projectiles have been triggered and have not reached their destination, move them forward in depth.
			Vector3 aim, new_position;
			MultiplyVector( aim, kVector, renderer->hand->orientation );
			ScaleVector( aim, aim, -20.0 );
			AddVectors( new_position, renderer->projectiles->position, aim );
			renderer->projectiles->SetPosition( new_position );
			currentProjectileState = running;
		}
		break;

	case miss:
	case hit:
		// We might want to animate the hit or the miss situations, but for now we just 
		// hide the projectiles and transition to the cocked state, ready for the next shot.
		currentProjectileState = cocked;
		break;

	case cocked:
		// We are waiting for the trigger, so just stay in this state.
		currentProjectileState = cocked;
		break;

	default:
		fAbortMessage( "GraspVR", "Unrecognized projectile state: %d", currentProjectileState );
		break;

	}
	return( currentProjectileState );

}

double GraspVR::SetDesiredHeadRoll( double desired_roll_angle, double tolerance ) {
	desiredHeadRoll = desired_roll_angle;
	desiredHeadRollTolerance = tolerance;
	SetQuaternion( desiredHeadOrientation, desiredHeadRoll, kVector );
	return( desiredHeadRoll );
}
bool GraspVR::HandleHeadAlignment( void ) {
	static int good_countdown = CYCLES_TO_BE_GOOD;
	// If the head is aligned CYCLES_TO_BE_GOOD cycles in a row, return that the head orientation is good.
	if ( renderer->ColorGlasses( desiredHeadRoll ) ) return( --good_countdown <= 0 );
	// If head alignment has been lost, reset the counter and return false.
	else {
		good_countdown = CYCLES_TO_BE_GOOD;
		return false;
	}
}

double GraspVR::SetDesiredHandRoll( double roll_angle ) {
	desiredHandRoll = roll_angle;
	return( desiredHandRoll );
}
bool GraspVR::HandleHandAlignment( void ) {
	return( renderer->ColorKK( desiredHandRoll ) );
}

void GraspVR::HandleSpinningPrompts( void ) {
	static double angle = 39.0;
	renderer->timeoutIndicator->SetOrientation( angle, 0.0, 0.0 );
	angle += 1.0;
}
double  GraspVR::SetTargetOrientation( double roll_angle ) {
	renderer->orientationTarget->SetOrientation( roll_angle, 0.0, 0.0 );
	return( roll_angle );
}


void GraspVR::Render( void ) {

	// Prepare the GL graphics state for drawing in a way that is compatible 
	//  with OpenGLObjects. I am doing this each time we get ready to DrawObjects in 
	//  case other GL stuff is going on elsewhere. Otherwise, we could probably
	//  do this just once at the beginning, e.g. in CreateObjects.
	glUsefulPrepareRendering();
	for (int eye = 0; eye < 2; ++eye) {

		// Get ready to draw into one of the eyes.
		oculusMapper->SelectEye( eye );

		// Set up the viewing transformations.
		viewpoint->Apply( eye );
		// Draw the objects in the world.
		renderer->DrawVR();

		// Take care of an Oculus bug.
		oculusMapper->DeselectEye( eye );

	}

	// Do distortion rendering, Present and flush/sync
	ovrResult result = oculusMapper->BlastIt();
	// fOutputDebugString( "BlastIt() returns: %d\n", result );
}

// A rendering loop that allows one to toggle on and off the various VR objects.
void GraspVR::DebugLoop( void ) {

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay->HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Boresight the HMD tracker on 'B'.
		if ( oculusDisplay->Key['B'] ) trackers->hmdTracker->Boresight();
		if ( oculusDisplay->Key['U'] ) trackers->hmdTracker->Unboresight();

		// Handle triggering and moving the projectiles.
		if ( ( oculusDisplay->Key[VK_RETURN] || oculusDisplay->Button[MOUSE_LEFT] ) && currentProjectileState == cocked ) TriggerProjectiles();
		HandleProjectiles();

		// Prompt the subject to achieve the desired head orientation.
		HandleHeadAlignment();

		//
		// Handle other key presses.
		//

		// Disable drawing of all objects.
		if ( oculusDisplay->Key[VK_SPACE] ) {
			renderer->orientationTarget->Disable();
			renderer->positionOnlyTarget->Disable();
			renderer->kkTool->Disable();
			renderer->vTool->Disable();
			renderer->kTool->Disable();
			renderer->tiltPrompt->Disable();
			renderer->projectiles->Disable();
			renderer->response->Disable();
			renderer->starrySky->Enable();
			renderer->darkSky->Disable();
			renderer->room->Disable();
		}
		// Show the room.
		if ( oculusDisplay->Key['R'] ) {
			renderer->room->Enable();
		}
		// Show the target and the target-specific sky behind it.
		if ( oculusDisplay->Key['T'] ) {
			renderer->orientationTarget->Enable();
			renderer->starrySky->Disable();
			renderer->darkSky->Enable();
		}
		// Show the hand/tool.
		if ( oculusDisplay->Key['H'] ) {
			renderer->vTool->Enable();
			renderer->kTool->Disable();
			renderer->kkTool->Disable();
		}
		if ( oculusDisplay->Key['K'] ) {
			renderer->vTool->Disable();
			renderer->kTool->Disable();
			renderer->kkTool->Enable();
		}
		if ( oculusDisplay->Key['J'] ) {
			renderer->vTool->Disable();
			renderer->kTool->Enable();
			renderer->kkTool->Disable();
		}

		// Show the tilt prompt.
		if ( oculusDisplay->Key['P'] ) renderer->tiltPrompt->Enable();

		Render();
	}

}

