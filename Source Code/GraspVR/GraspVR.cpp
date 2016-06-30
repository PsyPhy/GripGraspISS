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
const int GraspVR::secondsToBeGood = 3.0;
const int GraspVR::secondsToBeBad = 2.0;

// Transparency of objects that change color according to roll angle.
// It is important that the halo (glasses) be transparent. The kkTool will be transparent
//  too as a side effect, but that's not so important.
const double GraspVR::errorColorMapTransparency = 0.5;

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

void GraspVR::ReleaseTrackers( void ) {
	trackers->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// VR 
//

void GraspVR::InitializeVR( HINSTANCE hinst ) {

	ovrResult result;

	// Decide if we are in full screen mode or not.
	// To avoid losing focus by clicking outside the desktop window it is best to be in fullscreen mode.
	static const bool fullscreen = false;

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

// Modulate the color of an object according to it's roll angle wrt a specified desired roll angle.
double GraspVR::SetColorByRollError( OpenGLObject *object, double desired_roll_angle, double sweet_zone, double tolerance, bool use_arrow ) {

	// Compute the roll angle of the object that is being followed.
	// I got this off of a web site. It seems to work for gaze close to straight ahead,
	// but I'm not sure that it will work well for pitch or yaw far from zero.
	// In fact, I am pretty sure it is the same thing as what I did above by rotating the iVector.
	// We need a reliable method for computing the roll angle independent from pitch and yaw.

	// Note the negative sign on the Y parameter passed to atan2. I had to add this to make the target
	//  orientation here match the orientation of the visual target. But I don't know if this one was 
	//  wrong or the other one. We should check.
	double object_roll_angle = ToDegrees( atan2( - object->orientation[0][1], object->orientation[0][0] ) );
	// Compute the error with respect to the desired roll angle.
	double direction =  desired_roll_angle - object_roll_angle;
	double magnitude = fabs( direction );

#if 0
	// fOutputDebugString( "Desired: %6.2f  Measured: %6.2f  Error: %6.2f\n", desired_roll_angle, object_roll_angle, error );


	// Michele's formula for the color transitions.
	//
	// There are a number of things that I do not know how to explain:
	//  Why epsilon * 0.2 and not simply epsilon to be in the good zone?
	//  Why (2*epsilon - epsilon) and not just epsilon?
	//  Why + 0.0?
	//  Why all the divides by 255? 
	if ( error < epsilon * 0.2 ){//GREEN
		object->SetColor( 0.0/255.0, 255.0/255.0,  200.0/255.0, 0.5 );
		return(true);
	} else {
		if ( error > ( 2 * epsilon ) ){//RED
			object->SetColor(255.0/255.0, 0.0/255.0,  0.0/255.0, 0.5);
		} else {
			if ( error > epsilon ){//Yellow->red
				object->SetColor(200.0/255.0, (200.0*(1-(error-epsilon)/(2*epsilon-epsilon))+0.0)/255.0,  0.0/255.0, 0.5);
			} 
			else { //green->yellow
				object->SetColor((200.0*(error-epsilon*0.20)/(epsilon-epsilon*0.2)+0.0)/255.0, 200.0/255.0,  0.0/255.0, 0.5);
			}
		}
		return(false);
	}
#endif

	if ( magnitude < sweet_zone ){
		// If we are within the sweet zone, the color is more cyan than green.
		// This makes it easier for the subject to recognize when the head is properly aligned.
		// But one need not be in the sweet zone for the orientation to be considered good.
		// If the orientation is within tolerance, it is good as well. This means that the color
		// starts to change before leaving the tolerance zone (assuming that tolerance > sweet ).
		object->SetColor( 0.0, 1.0,  0.5, errorColorMapTransparency );
	} else {
		// Colors will change as one moves farther from the center.
		if ( ( magnitude - sweet_zone ) < tolerance ) {
			// Go from green at the center to yellow at the limit of the tolerance zone.
			double fade = ( magnitude - sweet_zone ) / tolerance;
			object->SetColor( fade, 1.0, 0.0, errorColorMapTransparency );
		}
		else if ( ( magnitude - sweet_zone ) < 2.0 * tolerance ) {
			// Go from yellow to red.
			double fade = ( magnitude - sweet_zone - tolerance ) / tolerance;
			object->SetColor( 1.0, 1.0 - fade, 0.0, errorColorMapTransparency );
		}
		// If more than 2 epsilons, the color is red.
		else object->SetColor( 1.0, 0.0, 0.0, errorColorMapTransparency );
	}

	if ( magnitude - sweet_zone < tolerance ) return ( 0.0 );
	else return ( direction );

}

double GraspVR::SetDesiredHeadRoll( double desired_roll_angle, double tolerance ) {
	desiredHeadRoll = desired_roll_angle;
	desiredHeadRollTolerance = tolerance;
	TimerSet( headGoodTimer, secondsToBeGood);
	TimerSet( headBadTimer, 0.0 );
	return( desiredHeadRoll );
}
AlignmentStatus GraspVR::HandleHeadAlignment( bool use_arrow ) {

	renderer->tiltPrompt->SetPosition( renderer->hud->position );
	renderer->tiltPrompt->SetOrientation( renderer->hud->orientation );
	renderer->tiltPrompt->SetOffset( 0.0, 0.0, -150.0 );

	double alignment = SetColorByRollError( renderer->glasses, desiredHeadRoll, desiredHeadRollSweetZone, desiredHeadRollTolerance, use_arrow );
	if ( alignment < 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
	if ( alignment > 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

	// If the head is aligned do stuff depending on the time delays.
	if ( 0.0 == alignment ) {
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

	if ( renderer->hand->position[Y] < -150.0 ) {
		renderer->kkTool->SetColor( 0.0, 0.0, 0.0, 0.85 );
		return( misaligned );
	}
	else {

		double alignment = SetColorByRollError( renderer->kkTool, desiredHandRoll, desiredHandRollSweetZone, desiredHandRollTolerance, use_arrow );
		if ( alignment < 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 0.0 );
		if ( alignment > 0.0 ) renderer->tiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );

		// If the hand is aligned do stuff depending on the time delays.
		if ( 0.0 == alignment ) {
			// Arrow gets turned off as soon as the angle is good.
			renderer->tiltPrompt->Disable();
			if ( TimerTimeout( handGoodTimer )) {
				// We made it to good. So to be bad again one must be bad for the requisite number of seconds.
				TimerSet( handBadTimer, secondsToBeBad );
				// Indicate that the alignment is good.
				return( aligned );
			}
			// The alignment hasn't be good for long enough to really be considered as good.
			else return( transitioningToGood );
		}
		// If head alignment has been lost, do other stuff depending on the delays.
		else {
			if ( TimerTimeout( handBadTimer )) {
				// Made it to bad. To be good again you have to be good for a while.
				TimerSet( handGoodTimer, secondsToBeGood );
				// Turn on the arrow because we've been bad for a while.
				renderer->tiltPrompt->Enable();
				// Signal that we really are misaligned.
				return( misaligned );
			}
			// The alignment hasn't be bad for long enough to really be considered as bad.
			else return( transitioningToBad );
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

