
#include "stdafx.h"
#include "GraspVR.h"

using namespace Grasp;
using namespace PsyPhy;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Constants
//

// Number of cycles that the head alignment has to be within tolerance to be considered good.
int GraspVR::secondsToBeGood = 0.75;
int GraspVR::handSecondsToBeGood = 3.0;
int GraspVR::secondsToBeBad = 2.0;

// The following are tolerances expressed in degrees.
double	GraspVR::desiredHandRollTolerance = 5.0;
double	GraspVR::desiredHandRollSweetZone = 2.0;
double	GraspVR::desiredHeadRollTolerance = 5.0;
double	GraspVR::desiredHeadRollSweetZone = 2.0;

// The next three are angular tolerances as well, but
// the are expressed as the cosine of the tolerance angle.
double	GraspVR::armRaisedThreshold = 0.9;			// approx. 25°. Corresponds roughly to tunnel outline.
double	GraspVR::straightAheadThreshold = 0.995;	// approx. 2.5° Corresponds roughly to center target radius.
double  GraspVR::pointingThreshold = 0.98;


double GraspVR::interpupillary_distance = 60.0;
// Define how much the chest markers are ahead of or behind the HMD markers
//  in depth when the subject is looking level. Value is in mm. Positive if the
//  chest markers are in front of the HMD, negative if behind.
double GraspVR::chestOffset = 0.0;
// Define how much the eyes are behind the HMD tracker.
double GraspVR::viewpointOffset = 100.0;

double GraspVR::near_clipping = 1.0;
double GraspVR::far_clipping = 5500.0;

double GraspVR::projectile_speed = 20.0;
double GraspVR::prompt_spin_speed = - 0.75;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Trackers 
//

void GraspVR::InitializeTrackers( void ) {
	CopyVector( localAlignment.displacement, zeroVector );
	CopyQuaternion( localAlignment.rotation, nullQuaternion );
}

void GraspVR::UpdateTrackers( void ) {

	trackers->Update();

	// Get the position and orientation of the head and update the viewpoint accordingly.
	// Note that if the tracker returns false, meaning that the tracker does not have a valid new value,
	// the viewpoint offset and attitude are left unchanged, effectively using the last valid tracker reading.
	if ( !trackers->GetCurrentHMDPose( headPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading head pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		// Transform to the local reference frame centered on the initial HMD pose.
		TransformPose( headPose.pose, localAlignment, headPose.pose );

		// I shift the position backward towar the eyes.
		Vector3 shift;
		RotateVector( shift, headPose.pose.orientation, kVector );
		ScaleVector( shift, shift, viewpointOffset );
		AddVectors( headPose.pose.position, headPose.pose.position, shift );

		// Set the viewpoint according to the measured pose.
		viewpoint->SetPose( headPose.pose );
		// Place objects that move with the heads up display.
		renderer->hmd->SetPose( headPose.pose );
	}

	// Apply a counter rotation to the room as a means to generate conflict.
	 //renderer->room->SetOrientation( ( 1.0 - conflictGain ) * ObjectRollAngle( renderer->glasses ), 0.0, 0.0 );
	// Compute a counter roll that is opposite to the orientation of the head from zero.
	Quaternion conjugate, counter_roll;
	ComputeQuaternionConjugate( conjugate, headPose.pose.orientation );
	double lambda = conflictGain - 1.0;
	double alpha = 1 - lambda;
	double beta = 1 - alpha;
	for ( int i = 0; i < 4; i++ ) counter_roll[i] = alpha * nullQuaternion[i] + beta * conjugate[i]; 
	NormalizeQuaternion( counter_roll );
	renderer->room->SetOrientation( counter_roll );

	// Track movements of the hand marker array.
	if ( !trackers->GetCurrentHandPose( handPose ) ) {
		static int pose_error_counter = 0;
		//fOutputDebugString( "Error reading hand pose tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TransformPose( handPose.pose, localAlignment, handPose.pose );
		renderer->hand->SetPose( handPose.pose );
	}

	if ( !trackers->GetCurrentChestPose( chestPose ) ) {
		static int pose_error_counter = 0;
		// fOutputDebugString( "Error reading chest tracker (%03d).\n", ++pose_error_counter );
	}
	else {
		TransformPose( chestPose.pose, localAlignment, chestPose.pose );
	}

	// The vTool is a special case because it does not move with the hand. Instead,
	// it is attached to the HMD and moves with the gaze. Its orientation is set by
	// the rollTracker, which presumably changes only the roll angle.
	if ( !trackers->rollTracker->GetCurrentPose( rollPose ) ) {
		static int pose_error_counter = 0;
		fOutputDebugString( "Error reading roll tracker (%03d).\n", ++pose_error_counter );
	}
	else renderer->vTool->SetOrientation( rollPose.pose.orientation );


}

void GraspVR::ReleaseTrackers( void ) {
	trackers->Release();
}

void GraspVR::AlignToHMD( void ) {

	TrackerPose hmdPose;

	if ( !trackers->hmdTracker->GetCurrentPose( hmdPose ) ) {
		fMessageBox( MB_OK | MB_ICONEXCLAMATION, "GraspVR::AlignToHMD", "Error reading head pose tracker for alignment." );
		exit( -100 );
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

void GraspVR::AlignToCODA( void ) {
	CopyVector( localAlignment.displacement, zeroVector );
	CopyQuaternion( localAlignment.rotation, nullQuaternion );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// VR 
//

void GraspVR::InitializeVR( void ) {

	// Set up a default GL rendering context.
	glUsefulInitializeDefault();
	glUsefulDefaultSpecularLighting( 0.7 );
	glUsefulShinyMaterial();

	// Create a viewpoint into the scene, using default IPD, FOV and near/far culling.
	// Our basic units are millimeters, so set the near and far accordingly.
	viewpoint = display->CreateViewpoint( interpupillary_distance, near_clipping, far_clipping );

	// Set the player's position and orientation to the null position and orientation.
	// Note that the head tracking will be applied relative to this pose.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create all the necessary VR rendering objects.
	renderer->SetLighting();
	renderer->CreateVRObjects();
	renderer->positionOnlyTarget->radius = sqrt( 1 - pointingThreshold * pointingThreshold ) * renderer->room_length / 2.0;

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
	renderer->aimingErrorSphere->Disable();
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

	// Shutdown the display.
	display->Release();

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
	TimerSet( headGoodTimer, 0.5 );
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
	renderer->SetColorByRollError( renderer->glasses, angular_error, desiredHeadRollSweetZone, renderer->errorColorMapTransparency  );
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

/// Indicate when the subject is looking down the Z axis of the laboratory absolute coordinate frame.
AlignmentStatus GraspVR::HandleGazeDirection( void ) {
	Vector3 gaze;
	RotateVector( gaze, headPose.pose.orientation, kVector );
	if ( DotProduct( gaze, kVector ) > straightAheadThreshold ) {
		renderer->straightAheadTarget->SetColor( GREEN );
		return( aligned );
	}
	else {
		renderer->straightAheadTarget->SetColor( ORANGE );
		return( misaligned );
	}
}

/// Prompt the subject to redress the head on the shoulders and look straight ahead.
AlignmentStatus GraspVR::HandleHeadOnShoulders( bool use_arrow ) {

	// We do not have a lot of confidence about the positioning of the chest marker plate 
	// so we define 'straight ahead' as follows.
	// The centroid of the chest marker plate should be more reproducible than its orientation.
	// So we take the line from the centroid of the chest marker plate to the centroid of the HMD markers.
	Vector3 chest_to_eyes;
	Vector3 chest;
	CopyVector( chest, chestPose.pose.position );
	chest[Z] += chestOffset;

	SubtractVectors( chest_to_eyes, headPose.pose.position, chest );
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
	// Also set the color of the gaze laser to grey when off alignment and magenta when aligned.
	// Gaze is aligned with straight_ahead when the dot product between straight behind and the HMD k vector is 1.
	Vector3 gaze;
	double product;
	bool centered;
	RotateVector( gaze, headPose.pose.orientation, kVector );
	if ( (product = DotProduct( gaze, straight_behind )) < straightAheadThreshold ) {
		renderer->gazeLaser->SetColor( GRAY );
		renderer->straightAheadTarget->SetColor( ORANGE );
		centered = false;
	}
	else {
		renderer->gazeLaser->SetColor( MAGENTA );
		renderer->straightAheadTarget->SetColor( GREEN );
		centered = true;
	}
	// Now place the target far away along the straight ahead axis, at the level of the eyes.
	// Note the negative sign when scaling the straight_behind vector, 
	// so as to put it in front of the subject (GL coordinates).
	Vector3 look_at_location;
	ScaleVector( look_at_location, straight_behind, - renderer->room_length / 2.0 );
	AddVectors( look_at_location, look_at_location, headPose.pose.position );
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
	// If the roll angle of the head is zero, the arctan of the length of nose should be 90°.
	Vector3 nose;
	ComputeCrossProduct( nose, chest_to_eyes, ear_to_ear );
	product = VectorNorm( nose );
	double angular_error = ( product > 1.0 ? 90.0 : ToDegrees( asin( product ) ) ) - 90.0;

	// Set the color of the halo according to the angular error.
	renderer->SetColorByRollError( renderer->glasses, angular_error, desiredHeadRollSweetZone, renderer->errorColorMapTransparency  );
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
		// TimerSet( headGoodTimer, secondsToBeGood );
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


#if 1
	// Check if the hand is raised in front of the eyes. If not, it is shown in grey.
	Vector3 relativeHandPosition;
	SubtractVectors( relativeHandPosition, renderer->hand->position, renderer->hmd->position );
	double z = relativeHandPosition[Z];
	relativeHandPosition[Z] = 0;
	if ( z > -100.0 || VectorNorm( relativeHandPosition ) >= renderer->inner_visor_radius ) {
		renderer->kTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->kkTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->SetHandColor( renderer->vkTool, false );
		return( lowered );
	}
	else {
		renderer->kTool->SetColor( 0.0, 0.0, 1.0, 1.0 );	// kTool is always blue regardless of the orientation.
		renderer->SetHandColor( renderer->vkTool, true );
		return( raised );
	}
#else
	// Check if the hand is raised in front of the eyes. If not, it is shown in grey.
	Vector3 relativeHandPosition;
	SubtractVectors( relativeHandPosition, renderer->hmd->position, renderer->hand->position );
	NormalizeVector( relativeHandPosition );
	
	if ( DotProduct( relativeHandPosition, kVector ) < armRaisedThreshold ) {
		renderer->kTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->kkTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		renderer->SetHandColor( renderer->vkTool, false );
		return( lowered );
	}
	else {
		renderer->kTool->SetColor( 0.0, 0.0, 1.0, 1.0 );	// kTool is always blue regardless of the orientation.
//		renderer->kkTool->SetColor( 0.5, 0.5, 0.5, 1.0 );	// This should be overridden by a subsequent call to HandleHandOrientation.
		renderer->SetHandColor( renderer->vkTool, true );
		return( raised );
	}

#endif
}

double GraspVR::SetDesiredHandRoll( double desired_roll_angle, double tolerance ) {
	desiredHandRoll = desired_roll_angle;
	desiredHandRollTolerance = tolerance;
	TimerSet( handGoodTimer, handSecondsToBeGood );
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
		renderer->handRollPrompt->Disable();
		TimerSet( handGoodTimer, handSecondsToBeGood );
		TimerSet( handBadTimer, 0.0 );
		return( misaligned );
	}
	else {

		// Compute the roll angle of the hand.
		double object_roll_angle = ObjectRollAngle( renderer->kkTool );
		// Compute the error with respect to the desired roll angle.
		double angular_error =  desiredHandRoll - object_roll_angle;
		// Set the color of the tool according to the angular error.
		// Here we set the transparency to 1.0 (opaque) so that there are no surface cues.
		renderer->SetColorByRollError( renderer->kkTool, angular_error, desiredHandRollSweetZone, 1.0 );
		// Set the direction of the arrow according to the direction of the error.
		// This does not mean that the arrow is visible. That is determined below.
		if ( angular_error < 0.0 ) renderer->handRollPrompt->SetAttitude( 0.0, 0.0, 0.0 );
		if ( angular_error > 0.0 ) renderer->handRollPrompt->SetAttitude( 0.0, 0.0, 180.0 );

		// If the roll angle of the hand is aligned with the target do stuff depending on the time delays.
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
				TimerSet( handGoodTimer, handSecondsToBeGood );
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

void GraspVR::HandleLasers( void ) {

	// Check if the hand is raised in front of the eyes. If not, it is shown in grey.
	Vector3 relativeHandPosition;
	SubtractVectors( relativeHandPosition, renderer->hand->position, renderer->hmd->position );
	double z = relativeHandPosition[Z];
	relativeHandPosition[Z] = 0;
	if ( z > -100.0 || VectorNorm( relativeHandPosition ) >= renderer->inner_visor_radius ) {
		// Lasers should be visible only if the hand is in the field of view.
		// For 
		renderer->handLaser->SetColor( 0.0, 0.0, 0.0, 0.0 );
		renderer->handLaser->SetOffset( 0.0, 0.0, renderer->laser_distance );
		// If the hand is not raised, we cannot be on the target.
		renderer->positionOnlyTarget->SetColor( Translucid( GRAY ) );
	}
	else {

		// Allow the lasers to be drawn if the hand is raised to be in view.
		// This does not mean that they will be drawn. They are only drawn 
		//  if the corresponding hand is enabled.
		renderer->handLaser->SetColor( 1.0, 0.0, 1.0, 1.0 );
		renderer->handLaser->SetOffset( 0.0, 0.0, - renderer->laser_distance );

		// Lasers in the hand should become diffuse if they do not point down the tunnel.
		// The original version used the angle between the tunnel and hand k-vectors to 
		// assess how well the hand is aligned with the tunnel.
		Vector3 tunnel_axis, hand_axis;
		Vector3 laser_ray, tip_location;
		MultiplyVector( tunnel_axis, renderer->kVectorMinus, renderer->room->orientation );
		MultiplyVector( hand_axis, renderer->kVectorMinus, renderer->hand->orientation );
		// Early versions used only the pitch and yaw orientation of the hand to determine
		// how much diffusion to apply. But that allowed for cheating because one could simply
		// displace the hand, without tilting, to touch the walls of the tunnel. Here we approximate
		// the endpoint of the laser and use its position with respect to the tunnel cross-section
		// to determine the level of diffusion.  But we have a switch that allows us to revert
		// to the old method, if desired.
		if ( stopCheating ) {
			ScaleVector( laser_ray, hand_axis, renderer->laser_distance );
			AddVectors( tip_location, laser_ray, renderer->hand->position );
			NormalizeVector( hand_axis, tip_location );
		}
		double projection = renderer->DotProduct( hand_axis, tunnel_axis );
		renderer->handLaser->SetEccentricity( projection );

		// Change the color of the targeting sphere depending on if the hand alignment is good or not.
		if ( projection > pointingThreshold ) {
			renderer->positionOnlyTarget->SetColor( 0.0, 1.0, 1.0, 0.5 );
			if ( snuffLaser ) renderer->handLaser->Disable();
		}
		else renderer->positionOnlyTarget->SetColor( Translucid( GRAY ) );

		// As another way of avoiding cheating, we turn the laser off if the hand
		// has been properly aligned for at least an instant, but then use an expanding target
		// sphere to indicate when the hand orientation or displacement moves it out of the 
		// acceptable range at the center of the tunnel.
		if ( laserTargetingActive ) {
			if ( projection > pointingThreshold ) {
				laserTargetingAcquired = true;
				// The target sphere comes on to show that the hand is in the acceptable
				// zone in terms of position and orientation.
				renderer->positionOnlyTarget->Enable();
				renderer->aimingErrorSphere->Disable();
			}

			else {
				// If we haven´t yet achieved an acceptable pose of the hand, the aiming target
				// is visible but grey. The error sphere is hidden.
				if ( !laserTargetingAcquired ) {
					renderer->positionOnlyTarget->Enable();
					renderer->aimingErrorSphere->Disable();
				}
				// But if an acceptable pose has been achieved at least briefly and then
				// the hand exits the zone, the laser is turned off and the distance outside
				// the acceptable zone is signaled by an expanding sphere.
				else {
					renderer->aimingErrorSphere->Enable();
					renderer->positionOnlyTarget->Disable();
					// Compute how far we are outside the acceptable zone.
					double error = projection - pointingThreshold;
					// The error sphere inflates the greater the error.
					renderer->aimingErrorSphere->radius = ( 1.0 - error * balloonInflationRate ) * renderer->positionOnlyTarget->radius;
					// The error sphere gradually becomes more opaque as it expands. In this way the
					// walls of the tunnel are hidden if the user tries to put the hand in front of them.
					renderer->aimingErrorSphere->SetColor( 1.0, 0.0, 1.0, 0.5 - 20.0 * error );
				}
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
	HandleLasers();

	// Prepare the GL graphics state for drawing in a way that is compatible 
	//  with OpenGLObjects. I am doing this each time we get ready to DrawObjects in 
	//  case other GL stuff is going on elsewhere. Otherwise, we could probably
	//  do this just once at the beginning, e.g. in CreateObjects.
	display->Prepare();
	glUsefulPrepareRendering();

	for (int eye = 0; eye < 2; ++eye) {

		// Get ready to draw into one of the eyes.
		display->SelectEye( eye );

		// Set up the viewing transformations.
		display->ApplyViewpoint( viewpoint, (Eye) eye );

		// Draw the objects in the world.
		renderer->DrawVR();

		// Take care of an Oculus bug.
		display->DeselectEye( eye );

	}

	// Do distortion rendering, Present and flush/sync
	display->Present();

}

// A rendering loop that allows one to toggle on and off the various VR objects.
void GraspVR::DebugLoop( void ) {

	// Enter into the rendering loop and handle other messages.
	while ( display->HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Boresight the HMD tracker on 'B'.
		if ( display->KeyDown( VK_CONTROL ) && display->KeyDownEvents( 'B' ) ) trackers->hmdTracker->Boresight();
		if ( display->KeyDown( VK_CONTROL ) && display->KeyDownEvents( 'U' ) ) trackers->hmdTracker->Unboresight();

		// Handle triggering and moving the projectiles.
		if ( ( display->KeyDownEvents( VK_RETURN ) || display->KeyDownEvents( MOUSE_LEFT ) ) && currentProjectileState == cocked ) TriggerProjectiles();
		HandleProjectiles();

		// Prompt the subject to achieve the desired head orientation.
		HandleHeadAlignment( true );

		//
		// Handle other key presses.
		//

		// Disable drawing of all objects.
		if ( display->KeyDownEvents( VK_SPACE ) ) {
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
		if ( display->KeyDownEvents( 'R' ) ) {
			renderer->room->Enable();
		}
		// Show the target and the target-specific sky behind it.
		if ( display->KeyDownEvents( 'T' ) ) {
			renderer->orientationTarget->Enable();
			renderer->starrySky->Disable();
			renderer->darkSky->Enable();
		}
		// Show the hand/tool.
		if ( display->KeyDownEvents( 'H' ) ) {
			renderer->vTool->Enable();
			renderer->kTool->Disable();
			renderer->kkTool->Disable();
		}
		if ( display->KeyDownEvents( 'K' ) ) {
			renderer->vTool->Disable();
			renderer->kTool->Disable();
			renderer->kkTool->Enable();
		}
		if ( display->KeyDownEvents( 'J' ) ) {
			renderer->vTool->Disable();
			renderer->kTool->Enable();
			renderer->kkTool->Disable();
		}

		// Show the tilt prompt.
		if ( display->KeyDownEvents( 'P' ) ) renderer->headTiltPrompt->Enable();

		Render();
	}

}

