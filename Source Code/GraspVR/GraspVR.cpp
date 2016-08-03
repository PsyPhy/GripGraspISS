// This is the main DLL file.

#include "stdafx.h"
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

// The following are tolerances expressed in degrees.
double	GraspVR::desiredHandRollTolerance = 5.0;
double	GraspVR::desiredHandRollSweetZone = 2.0;
double	GraspVR::desiredHeadRollTolerance = 5.0;
double	GraspVR::desiredHeadRollSweetZone = 2.0;

// The next two are angular tolerances as well, but
// the are expressed as the cosine of the tolerance angle.
double	GraspVR::armRaisedThreshold = 0.9;			// approx. 25°. Corresponds roughly to tunnel outline.
double	GraspVR::straightAheadThreshold = 0.999;	// approx. 2.5° Corresponds roughly to center target radius.

double GraspVR::handFilterConstant = 2.0;

double GraspVR::interpupillary_distance = 6.0;
double GraspVR::near_clipping = 1.0;
double GraspVR::far_clipping = 5000.0;

double GraspVR::projectile_speed = 20.0;
double GraspVR::prompt_spin_speed = - 0.75;


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

	// Apply a counter rotation to the room as a means to generate conflict.
	renderer->room->SetOrientation( ( 1.0 - conflictGain ) * ObjectRollAngle( renderer->glasses ), 0.0, 0.0 );

	// Track movements of the hand marker array.
	if ( !trackers->handTracker->GetCurrentPose( handPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading hand pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TransformPose( handPose.pose, localAlignment, handPose.pose );
		renderer->hand->SetPose( handPose.pose );
	}

	if ( !trackers->chestTracker->GetCurrentPose( chestPose ) ) {
		static int pose_error_counter = 0;
		// fOutputDebugString( "Error reading chest tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TransformPose( chestPose.pose, localAlignment, chestPose.pose );
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
	// Usually we mirror the Oculus display on the computer screen. But you may want to hide
	// what is going on in the HMD. To do so, set the following to false;
	static const bool mirror = true;

	// Initializes LibOVR, and the Rift
#ifdef USE_OCULUS_O_8
	OVR::System::Init();
#endif
	result = ovr_Initialize( nullptr );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspVR", "Error initializing VR Headset.\n - Failed to initialize libOVR (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

	// Initialize the Oculus-enabled Windows platform.
	result = oculusDisplay->InitWindow( hinst, L"GraspVR", fullscreen );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspVR", "Error initializing VR Headset.\n - Failed to initialize oculusDisplay (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper->Initialize( oculusDisplay, mirror );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspVR", "Error initializing VR Headset.\n - Failed to initialize oculusMapper (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

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
	viewpoint = new OculusViewpoint( oculusMapper, interpupillary_distance, near_clipping, far_clipping );

	// Set the player's position and orientation to the null position and orientation.
	// Note that the head tracking will be applied relative to this pose.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create all the necessary VR rendering objects.
	renderer = new GraspGLObjects();
	renderer->CreateVRObjects();
	renderer->PlaceVRObjects();

	// Initialize state of the objects.
	// Almost everything is off by default, except the room, the sky and the halo.
	renderer->room->Enable();
	renderer->starrySky->Enable();
	renderer->glasses->Enable();

	renderer->darkSky->Disable();
	renderer->response->Disable();
	renderer->orientationTarget->Disable();
	renderer->positionOnlyTarget->Disable();
	renderer->straightAheadTarget->Disable();
	renderer->headTiltPrompt->Disable();
	renderer->handRollPrompt->Disable();
	renderer->gazeLaser->Disable();
	renderer->successIndicator->Disable();
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
	ScaleVector( projectileDirection, projectileDirection, - projectile_speed );

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

// A common routine to compute the roll angle of an object. It is used to compute the roll angle of
// the head when guiding the head to the desired tilt and to compute the roll angle of the hand/tool
// when guiding the hand to a kinesthetic target.
double GraspVR::ObjectRollAngle( OpenGLObject * object ) {
	// This implementation is sensitive to pitch and yaw and will go crazy at 90°.
	// Note the negative sign on the Y parameter passed to atan2. I had to add this to make the target
	//  orientation here match the orientation of the visual target. But I don't know if this one was 
	//  wrong or the other one. We should check.

	// This computes the angle from the projection of the rotated iVector into the XY plane. 
	// It should fail when yaw is close to 90°.
	return( ToDegrees( atan2( - object->orientation[X][Y], object->orientation[X][X] )));
}

AlignmentStatus GraspVR::HandleHeadAlignment( bool use_arrow ) {

	// Compute the roll angle of the object that is being followed.
	// Note the negative sign on the Y parameter passed to atan2. I had to add this to make the target
	//  orientation here match the orientation of the visual target. But I don't know if this one was 
	//  wrong or the other one. We should check.
	double object_roll_angle = ObjectRollAngle( renderer->glasses );
	// Compute the error with respect to the desired roll angle.
	double angular_error =  desiredHeadRoll - object_roll_angle;
	// Set the color of the halo according to the angular error.
	renderer->SetColorByRollError( renderer->glasses, angular_error, desiredHeadRollSweetZone  );
	// Set the direction of the arrow according to the direction of the error.
	// This does not mean that the arrow is visible. That is determined below.
	if ( angular_error < 0.0 ) renderer->headTiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
	if ( angular_error > 0.0 ) renderer->headTiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

	// If the head is aligned do stuff depending on the time delays.
	if ( fabs( angular_error ) < desiredHeadRollTolerance ) {
		// Arrow gets turned off as soon as the angle is good.
		renderer->headTiltPrompt->Disable();
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
			if ( use_arrow ) renderer->headTiltPrompt->Enable();
			// Signal that we really are misaligned.
			return( misaligned );
		}
		// The alignment hasn't be bad for long enough to really be considered as bad.
		else return( transitioningToBad );
	}
}

/// Prompt the subject to redress the head on the shoulders and look straight ahead.
AlignmentStatus GraspVR::HandleHeadOnShoulders( bool use_arrow ) {

	// We do not have a lot of confidence about the positioning of the chest marker plate 
	// so we define 'straight ahead' as follows.
	// The centroid of the chest marker plate should be more reproducible than its orientation.
	// So we take the line from the centroid of the chest marker plate to the centroid of the HMD markers.
	Vector3 chest_to_eyes;
	SubtractVectors( chest_to_eyes, headPose.pose.position, chestPose.pose.position );
	NormalizeVector( chest_to_eyes );
	// The x-axis in the chest marker plate local reference will be in the plane of the plate and should 
	// be more-or-less perpendicular to the chest-to-eyes vector, which is all we need, then, to define 
	// the fronto-parallel plane with respect to the torso. This vector will be relatively unaffected by 
	// pitch rotations of the torso or of the chest marker plate and we don't care about roll for this vector.
	Vector3 left_to_right;
	RotateVector( left_to_right, chestPose.pose.orientation, iVector );
	// The straight ahead vector wrt the torso is perpendicular to the chest-to-eyes and left-to-right vectors.
	// We use straight behind instead because it is more directly comparable to the k vector of the HMD, which 
	//  points from front to back (GL reference frame).
	Vector3 straight_behind;
	ComputeCrossProduct( straight_behind, left_to_right, chest_to_eyes );
	NormalizeVector( straight_behind );
	// Set the color of the spherical target to green if it is centered in the field of view.
	// Gaze is aligned with straight_ahead when the dot product between straight behind and the HMD k vector is 1.
	Vector3 gaze;
	double product;
	bool centered;
	RotateVector( gaze, headPose.pose.orientation, kVector );
	if ( (product = DotProduct( gaze, straight_behind )) < straightAheadThreshold ) {
		centered = false;
		renderer->gazeLaser->SetColor( MAGENTA );
	}
	else {
		renderer->gazeLaser->SetColor( GREEN );
		centered = true;
	}
	// Now place the target at the end of the (not visible) tunnel.
	// Note the negative sign when scaling the straight_behind vector, 
	// so as to put it in front of the subject (GL coordinates).
	Vector3 look_at_location;
	ScaleVector( look_at_location, straight_behind, - renderer->room_length / 2.0 );
	renderer->straightAheadTarget->SetPosition( look_at_location );

	// Now we need to compute the roll angle of the head/hmd.
	// We can assume that the subject is looking straight ahead because they have to 
	// sight the target with the laser pointer in order to achieve the correct alignment.
	Vector3 ear_to_ear;
	RotateVector( ear_to_ear, headPose.pose.orientation, iVector );
	// As mentioned above, we are not confident that the chest marker plate is well aligned,
	// nor can we be sure that the head is not pitched up or down a bit.
	// So rather than comparing jVector to jVector between the HMD and the chest plate, we 
	// take the cross product of the vector from chest centroid to HMD centroid and the 
	// vector from ear to ear. I call this vector 'nose', for no particularly good reason.
	// If the roll angle of the head is zero, the artan of the length of nose should be 90°.
	Vector3 nose;
	ComputeCrossProduct( nose, chest_to_eyes, ear_to_ear );
	product = VectorNorm( nose );
	double angular_error = ( product > 1.0 ? 90.0 : ToDegrees( asin( product ) ) ) - 90.0;

	// Set the color of the halo according to the angular error.
	renderer->SetColorByRollError( renderer->glasses, angular_error, desiredHeadRollSweetZone  );
	// Set the direction of the arrow according to the direction of the error.
	// This does not mean that the arrow is visible. That is determined below.
	if ( angular_error < 0.0 ) renderer->headTiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
	if ( angular_error > 0.0 ) renderer->headTiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

	// The hand must be lowered to really be aligned. If it is still raised after a short
	// delay, we activate the prompt to remind the subject to lower the arm.
	ArmStatus hand_status = HandleHandElevation();
	if ( hand_status == raised ) {
		if ( TimerElapsedTime( headBadTimer ) > secondsToBeGood ) renderer->lowerHandPrompt->Enable();
		TimerSet( headGoodTimer, secondsToBeGood );
		return( misaligned );
	} 
	else renderer->lowerHandPrompt->Disable();
	// Must have the gaze centered to be aligned.
	if ( !centered ) {
		TimerSet( headGoodTimer, secondsToBeGood );
		return( misaligned );
	}
	// Now check for the actual head roll alignment, adding in some hysteresis.
	if ( fabs( angular_error ) < desiredHeadRollTolerance ) {
		// Arrow gets turned off as soon as the angle is good.
		renderer->headTiltPrompt->Disable();
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
			if ( use_arrow ) renderer->headTiltPrompt->Enable();
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
		renderer->SetHandColor( renderer->vkTool, false );
		return( lowered );
	}
	else {
		renderer->kTool->SetColor( 0.0, 0.0, 1.0, 1.0 );	// kTool is always blue regardless of the orientation.
		renderer->kkTool->SetColor( 0.5, 0.5, 0.5, 1.0 );	// This should be overridden by a subsequent call to HandleHandOrientation.
		renderer->SetHandColor( renderer->vkTool, true );
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
	renderer->handRollPrompt->SetPosition( handPose.pose.position );
	// Set its Z axis to align with the Z axis of the hand, but
	//  ignore the roll angle.
	Vector3 ray;
	RotateVector( ray, handPose.pose.orientation, kVector );
	Matrix3x3( rotation_matrix );
	SetRotationMatrix( rotation_matrix, ray, kVector );
	renderer->handRollPrompt->SetOrientation( rotation_matrix );

	// Check if the hand is raised properly. If not, it is shown in grey.
	if ( lowered == HandleHandElevation() ) {
		TimerSet( handGoodTimer, secondsToBeGood );
		TimerSet( handBadTimer, 0.0 );
		return( misaligned );
	}
	else {

		// Compute the roll angle of the hand.
		double object_roll_angle = ObjectRollAngle( renderer->kkTool );
		// Compute the error with respect to the desired roll angle.
		double angular_error =  desiredHandRoll - object_roll_angle;
		// Set the color of the tool according to the angular error.
		renderer->SetColorByRollError( renderer->kkTool, angular_error, desiredHandRollSweetZone );
		// Set the direction of the arrow according to the direction of the error.
		// This does not mean that the arrow is visible. That is determined below.
		if ( angular_error < 0.0 ) renderer->handRollPrompt->SetAttitude( 0.0, 0.0, 0.0 );
		if ( angular_error > 0.0 ) renderer->handRollPrompt->SetAttitude( 0.0, 0.0, 180.0 );

		// If the hand is aligned do stuff depending on the time delays.
		if ( fabs( angular_error ) < desiredHandRollTolerance ) {
			// Arrow gets turned off as soon as the angle is good.
			renderer->handRollPrompt->Disable();
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
				if ( use_arrow ) renderer->handRollPrompt->Enable();
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

	// Set an arbitrary starting orientation.
	static double angle = 39.0;
	renderer->spinners->SetAttitude( angle, 0.0, 0.0 );
	renderer->raiseHandIndicator->SetAttitude( angle, 0.0, 0.0 );
	angle += prompt_spin_speed;
}

double  GraspVR::SetTargetOrientation( double roll_angle ) {
	renderer->orientationTarget->SetOrientation( roll_angle, 0.0, 0.0 );
	return( roll_angle );
}


void GraspVR::Render( void ) {

	HandleSpinningPrompts();

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
			renderer->straightAheadTarget->Disable();
			renderer->kkTool->Disable();
			renderer->vTool->Disable();
			renderer->kTool->Disable();
			renderer->handRollPrompt->Disable();
			renderer->headTiltPrompt->Disable();
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
		if ( oculusDisplay->Key['P'] ) renderer->headTiltPrompt->Enable();

		Render();
	}

}

