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
// If the are different, the machine calls the EnterXXX() method for the current state, where XXX is one of the possible GraspTrialStates.
// Then, in either case, it calls the UpdateXXX() method, which returns the subsequent state.
// If the subsequent state is not the same as the current state, the ExitXXX() method is called for the current state before leaving the cycle.
//
bool GraspTaskManager::UpdateStateMachine( void ) {

	// Here I implement a rather brute force implementation in which each state has it's own 
	// case in the switch statement and each case calls explicitly the required methods. If 
	// you want to add a state, you also have to edit here to handle the state and you have to 
	// add the state handlers to the definition of GraspTaskManager. Also, it is not possible for
	// a derived task to extend the possible states. It would be necessary to duplicate everything that
	// is here in the UpdateStateMachine() method of the derived class, plus whatever extra is needed.
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

	case ProvideFeedback:

		if ( currentState != previousState ) EnterProvideFeedback();
		nextState = UpdateProvideFeedback();
		if ( nextState != currentState ) ExitProvideFeedback();
		break;

	case TrialCompleted:

		if ( currentState != previousState ) EnterTrialCompleted();
		nextState = UpdateTrialCompleted();
		if ( nextState != currentState ) ExitTrialCompleted();
		break;

	case TrialInterrupted:

		if ( currentState != previousState ) EnterTrialInterrupted();
		nextState = UpdateTrialInterrupted();
		if ( nextState != currentState ) ExitTrialInterrupted();
		break;


	}
	
	// Quit the state machine if that is what the current state said to do.
	if ( nextState == ExitStateMachine ) return( true );
	// Otherwise, move on to the next state.
	else {
		previousState = currentState;
		currentState = nextState;
		return( false );
	}

}


int GraspTaskManager::RunTrialBlock( void ) {

	// Load the trial parameters.
	// We probably want to read a parameter file to define the sequence of stimuli.
	SetDesiredHandRoll( -35.0 );
	SetTargetOrientation( 17.5 );

	// Initialize the state machine.
	previousState = NullState;
	currentState = StraightenHead;

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay->HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Boresight the HMD tracker on 'B'.
		// This is here for debugging and should probably be removed.
		if ( oculusDisplay->Key['B'] ) trackers->hmdTracker->Boresight();
		if ( oculusDisplay->Key['U'] ) trackers->hmdTracker->Unboresight();

		// Render the scene to the HMD. This will draw all items that are
		// enabled. The state machine is responsible for enabling and disabling
		// the OpenGLObjects objects as needed.
		Render();

		// Update the state machine. If it returns true it means that we have 
		//  finished the current block of trials.
		if ( this->UpdateStateMachine() ) break;

	}

	return 0;

}


// 
// Now define the handlers for each state.
// Here we define the required methods for the base class GraspTaskManager.
// Then we will define new classes of GraspTaskManager and redefine only 
//  those methods that are different from the base class.
// For instance, in K-K the present target state will guide the subject to a 
//  defined kinesthetic target, rather than showing the orientated line of 
//  balls on the screen. But the other state handlers can probably stay the same.
//

//
// StraightenHead
// The subject is guided to align the head with the body axis.
void GraspTaskManager::EnterStraightenHead( void ) { 
	// Align with the local reference frame of the subject.
	// Here we just align with the absolute refererence frame, but
	// in Quasi-Freefloating we will want to align the visual reference frame
	// in the HMD to the orientation defined by the chest markers.

	// TODO: Need to establish a new CODA reference frame around the chest marker structure.
	// TODO: Decide what should be the visual conditions when the realignment occurs. Perhaps
	//  we need to avoid sudden jumps of the tunnel orientation.

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
	// do something different if not. Also, if the duration is fixed, it would 
	// be good to give to the subject a very clear indication when the correct head orientation 
	// has been achieved. Otherwise, there is a tendancy to try and improve the orientation until
	// you get a reaction.
	if ( TimerTimeout( stateTimer ) ) return( PresentTarget ); 
	else return( currentState );
}
void GraspTaskManager::ExitStraightenHead( void ) {}

// PresentTarget
// The target is diplayed to the subejct.
// Here don't actually display anything, but the derived classes will.
void GraspTaskManager::EnterPresentTarget( void ) {
	TimerSet( stateTimer, 3.0 ); 
}
GraspTrialState GraspTaskManager::UpdatePresentTarget( void ) { 
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed.
	if ( !HandleHeadAlignment() ) return( TrialInterrupted );
	// Stay in this state for a fixed time.
	// Nominally, the next step is to tilt the head prior to responding.
	if ( TimerTimeout( stateTimer ) ) return( TiltHead ); 
	// If we haven't returned based on some condition above, continue in
	// this state for the next cycle.
	return( currentState );
}
void  GraspTaskManager::ExitPresentTarget( void ) {
	// Hide the visible target.
	renderer->orientationTarget->Disable();
}

// TiltHead
// The subject is guided to a new tilt orientation of the head.
// Conflict may be added during the motion.
void GraspTaskManager::EnterTiltHead( void ) {
	// Set a timeout to accomplish the task.
	TimerSet( stateTimer, 5.0 ); 
	// Set the desired tilt of the head.
	// Normally this would come from the stimulus sequence.
	SetDesiredHeadRoll( -35.0 );
	// Show the arrow that indicates the required direction of 
	// head tilt from the current orientation.
	// This needs work, because it means measuring the current tilt of the
	// head and compare it to the desired tilt. This could perhaps be
	// built into HandleHeadAlignment().
	renderer->tiltPrompt->Enable();
}
GraspTrialState GraspTaskManager::UpdateTiltHead( void ) { 
	// Update the visual feedback about the head tilt.
	bool aligned = HandleHeadAlignment();
	// Stay in this state a fixed time.
	if ( TimerTimeout( stateTimer ) ) {
		if ( aligned ) return( ObtainResponse ); 
		else return( TrialInterrupted );
	}
	return( currentState );
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
}
GraspTrialState GraspTaskManager::UpdateObtainResponse( void ) { 
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed. Interrupt the trial if not.
	if ( !HandleHeadAlignment() ) return( TrialInterrupted );
	// Handle triggering and moving the projectiles.
	if ( oculusDisplay->Button[MOUSE_LEFT] ) return( ProvideFeedback );
	if ( TimerTimeout( stateTimer ) ) return( ExitStateMachine ); 
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitObtainResponse( void ) {
	renderer->positionOnlyTarget->Disable();
}

// ProvideFeedback
// The subject sees the result of his or her response.
void GraspTaskManager::EnterProvideFeedback( void ) {
	// Show the target orientation.
	renderer->orientationTarget->Enable();
	// Launch the projectiles from the current hand orientation (the response );
	TriggerProjectiles();
}
GraspTrialState GraspTaskManager::UpdateProvideFeedback( void ) { 
	// If the projectile has been launched and reaches the end of its trajectory,
	// terminate the current trial and move on to the next. For the moment we do 
	// not differentiate between hits and misses, but we could do some visual effects.
	ProjectileState pstate = HandleProjectiles();
	if ( pstate == hit || pstate == miss ) return( TrialCompleted );
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitProvideFeedback( void ) {
	renderer->orientationTarget->Disable();
}

// TrialCompleted
// Provide an indication that the trial was completed successfully.
// Then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialCompleted( void ) {
	// Show the success indicator.
	renderer->successIndicator->Enable();
	// Show it for a fixed time.
	TimerSet( stateTimer, 1.0 ); 
}
GraspTrialState GraspTaskManager::UpdateTrialCompleted( void ) { 
	// After timer runs out, move on to the next trial.
	// What we really should do is set the parameters for the next trial,
	// or signal to end the state machine if all trials are done.
	if ( TimerTimeout( stateTimer ) ) return( StraightenHead ); 
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitTrialCompleted( void ) {
	renderer->successIndicator->Disable();
}

// TrialInterrupted
// Provide an indication that the trial was completed successfully.
// Then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialInterrupted( void ) {
	// Show the success indicator.
	renderer->timeoutIndicator->Enable();
	// Show it for a fixed time.
	TimerSet( stateTimer, 1.0 ); 
}
GraspTrialState GraspTaskManager::UpdateTrialInterrupted( void ) { 
	// After timer runs out, move on to the next trial.
	// What we really should do is set the parameters for the next trial,
	// or signal to end the state machine if all trials are done.
	if ( TimerTimeout( stateTimer ) ) return( StraightenHead ); 
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitTrialInterrupted( void ) {
	renderer->timeoutIndicator->Disable();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// VtoV

void VtoV::EnterPresentTarget( void ) {
	// The target is displayed visually.
	renderer->orientationTarget->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}
void  VtoV::ExitPresentTarget( void ) {
	// Hide the visible target.
	renderer->orientationTarget->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitPresentTarget();
}

void VtoV::EnterObtainResponse( void ) {
	// Show the visual representation of the hand that is driven 
	//  by the mouse or buttons.
	renderer->vTool->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}

void VtoV::ExitObtainResponse( void ) {
	// Hide the hand.
	renderer->vTool->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitObtainResponse();
}
