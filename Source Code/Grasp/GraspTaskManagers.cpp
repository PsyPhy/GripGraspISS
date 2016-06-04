// Grasp.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "../OculusInterface/OculusInterface.h"

#include "../Useful/Timers.h"
#include "../GraspVR/GraspVR.h"
#include "GraspTaskManagers.h"

using namespace Grasp;
using namespace PsyPhy;

//
// A state machine to run through the different state of a block of trials.
// On entry, the current state is compared to the previous state.
// If the are different, the machine calls the EnterXXX() method for the current state.
// Then, in either case, it calls the UpdateXXX() method, which returns the subsequent state.
// If the subsequent state is not the same as the current state, the ExitXXX() method is called for the current state.
//
bool GraspTaskManager::UpdateStateMachine( void ) {

	switch ( currentState ) {

	case StraightenHead:

		if ( currentState != previousState ) EnterStraightenHead();
		nextState = UpdateStraightenHead();
		if ( nextState != currentState ) ExitStraightenHead();
		break;
	
	case PresentTarget:

		if ( currentState != previousState ) EnterPresentTarget();
		nextState = UpdatePresentTarget();
		if ( nextState != currentState ) ExitPresentTarget();
		break;

	case TiltHead:

		if ( currentState != previousState ) EnterTiltHead();
		nextState = UpdateTiltHead();
		if ( nextState != currentState ) ExitTiltHead();
		break;

	case ObtainResponse:

		if ( currentState != previousState ) EnterObtainResponse();
		nextState = UpdateObtainResponse();
		if ( nextState != currentState ) ExitObtainResponse();
		break;


	}
	
	if ( nextState == ExitStateMachine ) return( true );
	else {
		previousState = currentState;
		currentState = nextState;
		return( false );
	}

}


int GraspTaskManager::RunTrialBlock( void ) {

	// Load the trial parameters.
	SetDesiredHandRoll( -35.0 );
	SetTargetOrientation( 17.5 );

	// Initialize the state machine.
	previousState = NullState;
	nextState = StraightenHead;

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay->HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Boresight the HMD tracker on 'B'.
		if ( oculusDisplay->Key['B'] ) trackers->hmdTracker->Boresight();
		if ( oculusDisplay->Key['U'] ) trackers->hmdTracker->Unboresight();

		// Render the scene to the HMD.
		Render();

		// Update the state machine. If it returns true it means that we have 
		//  finished the current block of trials.
		if ( UpdateStateMachine() ) break;

	}

	return 0;

}


// 
// Now define the handlers for each state.
//

//
// StraightenHead
// The subject is guided to align the head with the body axis.
void GraspTaskManager::EnterStraightenHead( void ) { 
	// Align with the local reference frame of the subject.
	// Here we just align with the absolute refererence frame, but
	// in Quasi-Freefloating we will want to align the visual reference frame
	// in the HMD to the orientation defined by the chest markers.
	trackers->hmdTracker->Boresight();
	// The desired orientation of the head is upright (0°).
	SetDesiredHeadRoll( 0.0 );
	// Set a time limit to achieve the desired head orientation.
	TimerSet( stateTimer, 5.0 ); 
}
GraspTrialState GraspTaskManager::UpdateStraightenHead( void ) { 
	// Here we just update the visual feedback about the head orientation
	// with resppect to the desired head orientation. 
	HandleHeadAlignment();
	// Had the fixed time run out? If so, move on to present the target.
	// We should probably check if the head is properly aligned and do
	// do something different if not.
	if ( TimerTimeout( stateTimer ) ) return( PresentTarget ); 
	else return( StraightenHead );
}
void GraspTaskManager::ExitStraightenHead( void ) {}

// PresentTarget
// The target is diplayed to the subejct.
void GraspTaskManager::EnterPresentTarget( void ) {
	TimerSet( stateTimer, 3.0 ); 
	renderer->orientationTarget->Enable();
}
GraspTrialState GraspTaskManager::UpdatePresentTarget( void ) { 
	// Update the visual feedback about the head tilt.
	// Should also check that the tilt is maintained and signal
	// an error if not, but that is not yet implemented.
	HandleHeadAlignment();
	// Stay in this state a fixed time.
	if ( TimerTimeout( stateTimer ) ) return( TiltHead ); 
	else return( PresentTarget );
}
void  GraspTaskManager::ExitPresentTarget( void ) {
		renderer->orientationTarget->Disable();
}

// TiltHead
// The subject is guided to a new tilt orientation of the head.
// Conflict may be added during the motion.
void GraspTaskManager::EnterTiltHead( void ) {
	TimerSet( stateTimer, 5.0 ); 
	SetDesiredHeadRoll( -35.0 );
	renderer->tiltPrompt->Enable();
}
GraspTrialState GraspTaskManager::UpdateTiltHead( void ) { 
	// Update the visual feedback about the head tilt.
	// Should also check that the tilt is maintained and signal
	// an error if not, but that is not yet implemented.
	// Also need to remove the tilt prompt when close to the desired tilt.
	HandleHeadAlignment();
	// Stay in this state a fixed time.
	if ( TimerTimeout( stateTimer ) ) return( ObtainResponse ); 
	else return( TiltHead );
}
void  GraspTaskManager::ExitTiltHead( void ) {
	// Remove the prompt about which way to tilt.
	renderer->tiltPrompt->Disable();
}

// ObtainResponse
// The subject reproduces the remembered orientation.
void GraspTaskManager::EnterObtainResponse( void ) {
	// Set a timeout timer to limit the time allowed to respond.
	TimerSet( stateTimer, 20.0 ); 
	// Show where to aim without showing the target orientation.
	renderer->positionOnlyTarget->Enable();
	// Show the hand orientation as in V-V or during training.
	renderer->vTool->Enable();
}
GraspTrialState GraspTaskManager::UpdateObtainResponse( void ) { 
	// Update the visual feedback about the head tilt.
	// Should also check that the tilt is maintained and signal
	// an error if not, but that is not yet implemented.
	HandleHeadAlignment();
	// Handle triggering and moving the projectiles.
	if ( ( oculusDisplay->Key[VK_RETURN] || oculusDisplay->Button[MOUSE_LEFT] ) ) {
		renderer->vTool->Disable();
		TriggerProjectiles();
	}
	ProjectileState pstate = HandleProjectiles();
	// If the projectile has been launched and reaches the end of its trajectory,
	// terminate the current trial and move on to the next.
	if ( pstate == hit || pstate == miss ) return( StraightenHead );
	// If no resonse, then exit the block of trials.
	if ( TimerTimeout( stateTimer ) ) return( ExitStateMachine ); 
	// Otherwise, continue in this state.
	return( ObtainResponse );
}
void  GraspTaskManager::ExitObtainResponse( void ) {
	renderer->vTool->Disable();
	renderer->positionOnlyTarget->Disable();
}