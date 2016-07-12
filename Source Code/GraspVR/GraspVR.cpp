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
// Constants
//

// Number of cycles that the head alignment has to be within tolerance to be considered good.
const int GraspVR::secondsToBeGood = 2.0;
const int GraspVR::secondsToBeBad = 2.0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Trackers 
//

void GraspVR::InitializeTrackers( const char *filename_root ) {
	trackers->Initialize();
	CopyVector( localAlignment.displacement, zeroVector );
	CopyQuaternion( localAlignment.rotation, nullQuaternion );

}

void GraspVR::UpdateTrackers( void ) {

	trackers->Update();

	// Get the position and orientation of the head and update the viewpoint accordingly.
	// Note that if the tracker returns false, meaning that the tracker does not have a valid new value,
	// the viewpoint offset and attitude are left unchanged, effectively using the last valid tracker reading.
	if ( !trackers->hmdTracker->GetCurrentPose( headPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		// Transform to the local reference frame centered on the initial HMD pose.
		TransformPose( headPose.pose, localAlignment, headPose.pose );
		// Set the viewpoint according to the measured pose.
		viewpoint->SetPose( headPose.pose );
		// Place objects that move with the heads up display.
		renderer->hmd->SetPose( headPose.pose );
	}


	// Track movements of the hand marker array.
	if ( !trackers->handTracker->GetCurrentPose( handPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading hand pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TransformPose( handPose.pose, localAlignment, handPose.pose );
		// Filter the hand position somewhat.
		// TODO: Constants need to be defined rather than hard-coded values.
		Pose filtered;
		ScaleVector( filtered.position, handPose.pose.position, 2.0 );
		AddVectors ( filtered.position, filtered.position, renderer->hand->position );
		ScaleVector( filtered.position, filtered.position, 1.0 / 3.0 );
		Quaternion q;
		MatrixToQuaternion( q, renderer->hand->orientation );
		for ( int i = 0; i < 4; i++ ) q[i] += ( 2.0 *  handPose.pose.orientation[i] );
		NormalizeQuaternion( q );
		CopyQuaternion( filtered.orientation, q );
		renderer->hand->SetPose( filtered );
	}

	if ( !trackers->chestTracker->GetCurrentPose( chestPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading chest tracker (%03d).\n", ++pose_error_counter );
	}

	// The vTool is a special case because it does not move with the hand. Instead,
	// it is attached to the HMD and moves with the gaze. It's roll attitude is set by
	// the roll tracker. We use the attitude property, rather than the orientation property,
	// because the position and orientation properties are used to position and orient the
	// visual object in 3D space.
	if ( !trackers->rollTracker->GetCurrentPose( rollPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading roll tracker (%03d).\n", ++pose_error_counter );
	}
	else renderer->vTool->SetAttitude( rollPose.pose.orientation );


}

void GraspVR::ReleaseTrackers( void ) {
	trackers->Release();
}

void GraspVR::AlignToHMD( void ) {

	TrackerPose hmdPose;

	if ( !trackers->hmdTracker->GetCurrentPose( hmdPose ) ) {
		fOutputDebugString( "Error reading head pose tracker for alignment.\n" );
	}
	else {
		// Local alignment is the conjugate of the head pose orientation.
		localAlignment.rotation[X] = - hmdPose.pose.orientation[X];
		localAlignment.rotation[Y] = - hmdPose.pose.orientation[Y];
		localAlignment.rotation[Z] = - hmdPose.pose.orientation[Z];
		localAlignment.rotation[M] =   hmdPose.pose.orientation[M];
		Vector3 rotated_position;
		RotateVector( rotated_position, localAlignment.rotation, hmdPose.pose.position );
		ScaleVector( localAlignment.displacement, rotated_position, -1.0 );
	}
}




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
	renderer->blockCompletedIndicator->Disable();
	renderer->headMisalignIndicator->Disable();
	renderer->readyToStartIndicator->Disable();
	renderer->vTool->Disable();
	renderer->kTool->Disable();
	renderer->vkTool->Disable();
	renderer->kkTool->Disable();
	renderer->projectiles->Disable();
	renderer->wristZone->Disable();

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
	// Since I set the object to be at the same place as the head tracker and then use the offset 
	//  parameter to place it in front of the eyes, I need to take into account both the tools
	//  position and orientation and its offset and attitude. I should probably make a more
	//  general function to compute the world position and orientation of compound objects.
	renderer->projectiles->SetOrientation( renderer->selectedTool->orientation );
	Vector3 offset, world;
	MultiplyVector( offset, renderer->selectedTool->offset, renderer->selectedTool->orientation );
	AddVectors( world, renderer->selectedTool->position, offset );
	renderer->projectiles->SetPosition( world );
	// If the projectiles have been triggered and have not reached their destination, move
	//  them forward along the line projecting out along the axis of the hand.
	MultiplyVector( projectileDirection, kVector, renderer->selectedTool->orientation );
	ScaleVector( projectileDirection, projectileDirection, -20.0 );

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
			Vector3 new_position;
			AddVectors( new_position, renderer->projectiles->position, projectileDirection );
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
	TimerSet( headGoodTimer, secondsToBeGood);
	TimerSet( headBadTimer, 0.0 );
	return( desiredHeadRoll );
}
AlignmentStatus GraspVR::HandleHeadAlignment( bool use_arrow ) {

	// Make the tilt prompt follow movements of the head, i.e. as part of the heads up display.
	renderer->tiltPrompt->SetPosition( renderer->hmd->position );
	renderer->tiltPrompt->SetOrientation( renderer->hmd->orientation );
	renderer->tiltPrompt->SetOffset( 0.0, 0.0, -150.0 );

	// Compute the roll angle of the object that is being followed.
	// Note the negative sign on the Y parameter passed to atan2. I had to add this to make the target
	//  orientation here match the orientation of the visual target. But I don't know if this one was 
	//  wrong or the other one. We should check.
	double object_roll_angle = ToDegrees( atan2( - renderer->glasses->orientation[0][1], renderer->glasses->orientation[0][0] ) );
	// Compute the error with respect to the desired roll angle.
	double angular_error =  desiredHeadRoll - object_roll_angle;
	// Set the color of the halo according to the angular error.
	renderer->SetColorByRollError( renderer->glasses, angular_error, desiredHeadRollSweetZone  );
	// Set the direction of the arrow according to the direction of the error.
	// This does not mean that the arrow is visible. That is determined below.
	if ( angular_error < 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
	if ( angular_error > 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

	// If the head is aligned do stuff depending on the time delays.
	if ( fabs( angular_error ) < desiredHeadRollTolerance ) {
		// Arrow gets turned off as soon as the angle is good.
		renderer->tiltPrompt->Disable();
		if ( TimerTimeout( headGoodTimer )) {
			// We made it to good. So to be bad again one must be bad for the requisite number of seconds.
			TimerSet( headBadTimer, secondsToBeBad );
			// Indicate that the alignment is good.
			return( aligned );
		}
		// The alignment hasn't be good for long enough to really be considered as good.
		else return( transitioningToGood );
	}
	// If head alignment has been lost, reset the counter and return false.
	else {
		if ( TimerTimeout( headBadTimer )) {
			// Made it to bad. To be good again you have to be good for a while.
			TimerSet( headGoodTimer, secondsToBeGood );
			// Turn on the arrow because we've been bad for a while.
			if ( use_arrow ) renderer->tiltPrompt->Enable();
			// Signal that we really are misaligned.
			return( misaligned );
		}
		// The alignment hasn't be bad for long enough to really be considered as bad.
		else return( transitioningToBad );
	}
}

ArmStatus GraspVR::HandleHandElevation( void ) {

	// Check if the hand is raised in front of the eyes. If not, it is shown in grey.
	Vector3 relativeHandPosition;
	SubtractVectors( relativeHandPosition,  renderer->hmd->position, renderer->hand->position );
	NormalizeVector( relativeHandPosition );
	if ( DotProduct( relativeHandPosition, kVector ) < armRaisedThreshold ) {
		renderer->kTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->kkTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->vkTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		return( lowered );
	}
	else {
		renderer->kTool->SetColor( 0.0, 0.0, 1.0, 1.0 );	// kTool is always blue regardless of the orientation.
		renderer->vkTool->SetColor( PARENT_COLOR );			// The color will depend on the color of each finger.
		renderer->vkTool->SetColor( 0.5, 0.5, 0.5, 1.0 );	// This should be overridden by a subsequent call to HandHandOrientation.
		return( raised );
	}

}

double GraspVR::SetDesiredHandRoll( double desired_roll_angle, double tolerance ) {
	desiredHandRoll = desired_roll_angle;
	desiredHandRollTolerance = tolerance;
	TimerSet( handGoodTimer, secondsToBeGood );
	TimerSet( handBadTimer, 0.0 );
	return( desiredHandRoll );
}
AlignmentStatus GraspVR::HandleHandAlignment( bool use_arrow ) {

	// Place the tilt prompt around the hand.
	renderer->tiltPrompt->SetPosition( renderer->kkTool->position );
	renderer->tiltPrompt->SetOrientation( renderer->kkTool->orientation );
	renderer->tiltPrompt->SetOffset( 0.0, 0.0, 0.0 );

	// Check if the hand is raised properly. If not, it is shown in grey.
	if ( lowered == HandleHandElevation() ) {
		TimerSet( handGoodTimer, secondsToBeGood );
		TimerSet( handBadTimer, 0.0 );
		return( misaligned );
	}
	else {

		// Compute the roll angle of the object that is being followed.
		// Note the negative sign on the Y parameter passed to atan2. I had to add this to make the target
		//  orientation here match the orientation of the visual target. But I don't know if this one was 
		//  wrong or the other one. We should check.
		double object_roll_angle = ToDegrees( atan2( - renderer->kkTool->orientation[0][1], renderer->kkTool->orientation[0][0] ) );
		// Compute the error with respect to the desired roll angle.
		double angular_error =  desiredHeadRoll - object_roll_angle;
		// Set the color of the tool according to the angular error.
		renderer->SetColorByRollError( renderer->kkTool, angular_error, desiredHandRollSweetZone );
		// Set the direction of the arrow according to the direction of the error.
		// This does not mean that the arrow is visible. That is determined below.
		if ( angular_error < 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
		if ( angular_error > 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

		// If the hand is aligned do stuff depending on the time delays.
		if ( fabs( angular_error ) < desiredHandRollTolerance ) {
			// Arrow gets turned off as soon as the angle is good.
			renderer->tiltPrompt->Disable();
			// We made it into the good zone. So to be bad again one must be bad for the requisite number of seconds.
			TimerSet( handBadTimer, secondsToBeBad );
			// Restart the timer in case it was paused by a brief exit from the good zone.
			TimerResume( handGoodTimer );
			// But only say that we are aligned if we have been good for the requisite amount of time.
			if ( TimerTimeout( handGoodTimer )) return( aligned );
			// The alignment hasn't be good for long enough to really be considered as good.
			else return( transitioningToGood );
		}
		// If hand alignment has been lost, do other stuff depending on the delays.
		else {
			if ( TimerTimeout( handBadTimer )) {
				// Made it to bad. 
				// Turn on the arrow because we've been bad for a while.
				if (use_arrow ) renderer->tiltPrompt->Enable();
				// If the error has endured long enough to trigger the arrow prompt, then
				// reset the time required to be good again to the full duration.
				// MICHELE: What do you think? 
				TimerSet( handGoodTimer, secondsToBeGood );
				// Signal that we really are misaligned.
				return( misaligned );
			}
			// The alignment hasn't be bad for long enough to really be considered as bad.
			else {
				TimerPause( handGoodTimer );
				return( transitioningToBad );
			}
		}
	}
}


void GraspVR::HandleSpinningPrompts( void ) {
	static double angle = 39.0;
	renderer->timeoutIndicator->SetAttitude( angle, 0.0, 0.0 );
	renderer->headMisalignIndicator->SetAttitude( angle, 0.0, 0.0 );
	renderer->readyToStartIndicator->SetAttitude( angle, 0.0, 0.0 );
	renderer->blockCompletedIndicator->SetAttitude( angle, 0.0, 0.0 );
	angle -= 1.0;
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
		HandleHeadAlignment( true );

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

