// Grasp.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "../OculusInterface/OculusInterface.h"

#include "../Useful/Timers.h"
#include "../DexServices/DexServices.h"
#include "../GraspVR/GraspVR.h"
#include "GraspTaskManagers.h"

using namespace Grasp;
using namespace PsyPhy;

// Defined constants
const double GraspTaskManager::indicatorDisplayDuration = 1.0;	// Time to show success and error indicators.

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

	case StartBlock:

		if ( currentState != previousState ) EnterStartBlock();
		nextState = UpdateStartBlock();
		if ( nextState != currentState ) ExitStartBlock();
		break;
	
	case StartTrial:

		if ( currentState != previousState ) EnterStartTrial();
		nextState = UpdateStartTrial();
		if ( nextState != currentState ) ExitStartTrial();
		break;
	
	case StraightenHead:

		if ( currentState != previousState ) EnterStraightenHead();
		nextState = UpdateStraightenHead();
		if ( nextState != currentState ) ExitStraightenHead();
		break;
	
	case AlignHead:

		if ( currentState != previousState ) EnterAlignHead();
		nextState = UpdateAlignHead();
		if ( nextState != currentState ) ExitAlignHead();
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

	case BlockCompleted:

		if ( currentState != previousState ) EnterBlockCompleted();
		nextState = UpdateBlockCompleted();
		if ( nextState != currentState ) ExitBlockCompleted();
		break;

	case TrialInterrupted:

		if ( currentState != previousState ) EnterTrialInterrupted();
		nextState = UpdateTrialInterrupted();
		if ( nextState != currentState ) ExitTrialInterrupted();
		break;

	case Timeout:

		if ( currentState != previousState ) EnterTimeout();
		nextState = UpdateTimeout();
		if ( nextState != currentState ) ExitTimeout();
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

int GraspTaskManager::LoadTrialParameters( char *filename ) {

	char line[2048];
	FILE *fp = fopen( filename, "r" );
	fAbortMessageOnCondition( !fp, "GraspTaskManager", "Error opening trial sequence file %s for read.", filename );
	while ( fgets( line, sizeof( line ), fp ) ) {

		int items = sscanf( line, "%lf; %lf; %lf; %lf; %lf; %lf;  %lf; %lf; %lf; %lf; %lf; %d",
			&trialParameters[nTrials].targetHeadTilt,
			&trialParameters[nTrials].targetHeadTiltTolerance,
			&trialParameters[nTrials].targetHeadTiltDuration,
			&trialParameters[nTrials].targetOrientation,
			&trialParameters[nTrials].hapticTargetOrientationTolerance,
			&trialParameters[nTrials].targetPresentationDuration,
			&trialParameters[nTrials].responseHeadTilt,
			&trialParameters[nTrials].responseHeadTiltTolerance,
			&trialParameters[nTrials].responseHeadTiltDuration,
			&trialParameters[nTrials].responseTimeout,
			&trialParameters[nTrials].conflictGain,
			&trialParameters[nTrials].provideFeedback );

		if ( items == 12 ) {
			if ( nTrials >= MAX_GRASP_TRIALS ) {
				fOutputDebugString( "Max number of trials (%d) exceeded in file %s\n", MAX_GRASP_TRIALS, filename );
			}
			else {
				// A valid set of paramters. Add it to the list of trials.
				nTrials++;
			}
		}
		else if ( items > 0 ) {
			fOutputDebugString( "Error reading parameter list after item %d.\n", items );
			fOutputDebugString( line );
		}
		else {
			fOutputDebugString( "Comment line: %s", line );
		}
	}
	fclose( fp );
	return( nTrials );
}

// The following may be called if there is an anomaly during a trial so as 
// to repreat it at the end of the block. 
void GraspTaskManager::RepeatTrial( int trial ) {
	// Don't repeat more than allowed.
	if ( retriesRemaining <= 0 ) {
		fOutputDebugString( "Max number of retries exceeded.\n" );
		return;
	}
	// If there is no room in the list, don't repeat any more either.
	if ( nTrials >= MAX_GRASP_TRIALS ) {
		fOutputDebugString( "Repeat would exceed max number of trials (%d).\n", MAX_GRASP_TRIALS );
		return;
	}
	// Copy the parameters of the specified trial to the end of the list.
	memcpy( (void *) &trialParameters[nTrials], (void *) &trialParameters[trial], sizeof( trialParameters[nTrials] ) );
	// Show that the list has been extended.
	nTrials++;
	// Count down how many more retries are allowed.
	retriesRemaining--;
	fOutputDebugString( "Trial %d will be repeated. Retries remaining: %d\n", trial, retriesRemaining );
}

int GraspTaskManager::RunTrialBlock( char *sequence_filename, char *output_filename_root ) {

	// Load the trial parameters.
	int trials = LoadTrialParameters( sequence_filename );
	fAbortMessageOnCondition( ( trials <= 0 ), "GraspTaskManager", "No valid trials loaded from file %s.", sequence_filename );
	fOutputDebugString( "Loaded paramters for %d trials from file %s.\n", trials, sequence_filename );
	currentTrial = 0;

	// Open a file for storing the responses.
	sprintf( responseFilename, "%s.rsp", output_filename_root );
	response_fp = fopen( responseFilename, "w" );
	fAbortMessageOnCondition( !response_fp, "GraspTaskManager", "Error opening file %s for writing.", responseFilename );
	// Ouput a header.
	fprintf( response_fp, "trial; targetHeadTilt; targetHeadTiltTolerance; targetHeadTiltDuration; targetOrientation; hapticTargetOrientationTolerance; targetPresentationDuration; responseHeadTilt; responseHeadTiltTolerance; responseHeadTiltDuration; responseTimeout; conflictGain; feedback (0 or 1); time; response\n" );

	// Open a file for storing the tracker poses.
	sprintf( poseFilename, "%s.pse", output_filename_root );
	pose_fp = fopen( poseFilename, "w" );
	fAbortMessageOnCondition( !pose_fp, "GraspTaskManager", "Error opening file %s for writing.", poseFilename );
	// Ouput a header.
	fprintf( pose_fp, "trial; time; head.time; head.visible; head.position; head.orientation; hand.time; hand.visible; hand.position; hand.orientation; chest.time; chest.visible; chest.position; chest.orientation; roll.time; roll.visible; roll.position; roll.orientation\n" );

	// Call the paradigm-specific preparation, if any.
	Prepare();

	// Initialize the state machine.
	previousState = NullState;
	currentState = StartBlock;

	// Measure the time since this block was initiated.
	TimerStart( blockTimer );

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay->HandleMessages() ) {

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();

		// Output the poses to a file.
		if ( pose_fp ) {
			fprintf( pose_fp, "%3d; %6.3f; %6.3f; %1d; %s; %s; %6.3f; %1d; %s; %s; %6.3f; %1d; %s; %s; %6.3f; %1d; %s; %s\n", 
				currentTrial, TimerElapsedTime( blockTimer ), 
				headPose.time, headPose.visible, vstr( headPose.pose.position ), qstr( headPose.pose.orientation ), 
				handPose.time, handPose.visible, vstr( handPose.pose.position ), qstr( handPose.pose.orientation ), 
				chestPose.time, chestPose.visible, vstr( chestPose.pose.position), qstr( chestPose.pose.orientation ),
				rollPose.time, rollPose.visible, vstr( rollPose.pose.position), qstr( rollPose.pose.orientation )
				);
		}

		// Boresight the HMD tracker on 'B' or align to the HMD no 'A'.
		// This is here for debugging and should probably be removed.
		if ( oculusDisplay->KeyDownEvents['A'] ) AlignToHMD();
		if ( oculusDisplay->KeyDownEvents['B'] ) trackers->hmdTracker->Boresight();
		if ( oculusDisplay->KeyDownEvents['U'] ) trackers->hmdTracker->Unboresight();

		// Update the state machine. If it returns true it means that we have 
		//  finished the current block of trials.
		if ( this->UpdateStateMachine() ) break;

		// Render the scene to the HMD. This will draw all items that are
		// enabled. The state machine is responsible for enabling and disabling
		// the OpenGLObjects objects as needed.
		Render();

		// Clear any accumulated keydown events in preparation for the next cycle.
		oculusDisplay->ClearKeyDownEvents();
	}
	if ( response_fp ) fclose( response_fp );
	response_fp = NULL;
	if ( pose_fp ) fclose( pose_fp );
	pose_fp = NULL;

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

// StartBlock
// Wait until the subject is in the HMD and ready to start.
void GraspTaskManager::EnterStartBlock( void ) {
	// The desired orientation of the head is upright (0°). This is not essential, but it allows us to show the colors already.
	SetDesiredHeadRoll( trialParameters[0].targetHeadTilt, trialParameters[0].targetHeadTiltTolerance );
	// Show the "Press to continue." indicator.
	renderer->readyToStartIndicator->Enable();
}
GraspTrialState GraspTaskManager::UpdateStartBlock( void ) { 
	// Modulate the halo color, even though it does not matter, so
	// that the subject gets into that mode of operation.
	HandleHeadAlignment( false );
	// Make the prompt spin to avoid creating a reference frame.
	HandleSpinningPrompts();
	// When the subject clicks a button, we move on to start the first trial
	if ( Validate() ) return( StartTrial );
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitStartBlock( void ) {
	renderer->readyToStartIndicator->Disable();
}

//
// StartTrial
// Set the new trial parameters. This is where conflict should get turned off or on.
void GraspTaskManager::EnterStartTrial( void ) { 

	// Align with the local reference frame of the subject.
	// Here we just align with the absolute refererence frame, but
	// in Quasi-Freefloating we will want to align the visual reference frame
	// in the HMD to the orientation defined by the chest markers.

	// TODO: Need to establish a new CODA reference frame around the chest marker structure.
	// TODO: Decide what should be the visual conditions when the realignment occurs. Perhaps
	//  we need to avoid sudden jumps of the tunnel orientation.

	// Output the parameters of this trial to the response file.
	fprintf( response_fp, "%d;  %5.2f; %5.2f; %5.2f;   %6.2f; %5.2f; %5.2f;   %6.2f; %5.2f; %5.2f; %5.2f;   %4.2f; %d;",
		currentTrial,

		trialParameters[currentTrial].targetHeadTilt,
		trialParameters[currentTrial].targetHeadTiltTolerance,
		trialParameters[currentTrial].targetHeadTiltDuration,

		trialParameters[currentTrial].targetOrientation,
		trialParameters[currentTrial].hapticTargetOrientationTolerance,
		trialParameters[currentTrial].targetPresentationDuration,

		trialParameters[currentTrial].responseHeadTilt,
		trialParameters[currentTrial].responseHeadTiltTolerance,
		trialParameters[currentTrial].responseHeadTiltDuration,
		trialParameters[currentTrial].responseTimeout,

		trialParameters[currentTrial].conflictGain,
		trialParameters[currentTrial].provideFeedback );

	// Make sure that this information gets written right away to facilitate degugging if we crash.
	// Note that we do not output a \n in the above. The line will be completed by the response.
	fflush( response_fp );

	// Indicate which trial is about to begin. For now we simply show this on the debug text window.
	// In the future it should send this info to ground.
	fOutputDebugString( "Starting Trial: %d  Target: %6.1f  Head Tilt: %6.1f  Conflict: %4.2f\n",
		currentTrial, trialParameters[currentTrial].targetOrientation, trialParameters[currentTrial].targetHeadTilt, trialParameters[currentTrial].conflictGain );

}
GraspTrialState GraspTaskManager::UpdateStartTrial( void ) { 
	return( StraightenHead ); 
}
void GraspTaskManager::ExitStartTrial( void ) {
	char tag[32];
	sprintf( tag, "StartT%03d", currentTrial );
	dexServer->SnapPicture( tag );
}

//
// StraightenHead
// Set the new trial parameters. This is where conflict should get turned off or on.
void GraspTaskManager::EnterStraightenHead( void ) { 

	// Turn the starry sky back on, even if it won't be visible right away because the room will be off.
	renderer->starrySky->Enable();
	renderer->darkSky->Disable();
	// Turn off lots of visual cues in preparation for starting up the conflict.
	renderer->room->Disable();
	// Make sure that the head tilt prompt is not still present.
	renderer->tiltPrompt->Disable();
	// The desired orientation of the head to zero in preparation for applying conflict (if any).
	SetDesiredHeadRoll( 0.0, trialParameters[currentTrial].targetHeadTiltTolerance );
	renderer->positionOnlyTarget->Enable();

}
GraspTrialState GraspTaskManager::UpdateStraightenHead( void ) { 
	// Update the feedback about the head orientation wrt the desired head orientation.
	// If the head alignment is satisfactory, move on to the next state.
	if ( aligned == HandleHeadOnShoulders( false ) ) return( AlignHead ); 
	else return( currentState );
}
void GraspTaskManager::ExitStraightenHead( void ) {
	// Set the conflict gain.

	// Align to the current position of the HMD. 
	AlignToHMD();
	renderer->room->Enable();
}

//
// AlignHead
// The subject is guided to align the head in preparation for observing the target.
// For the ISS, the head tilt will most likely be zero for target acquisition for
// all trials, but the sequence entries allow one to specify a different orientation if desired.
void GraspTaskManager::EnterAlignHead( void ) { 
	// The desired orientation of the head to the specified head orientation.
	SetDesiredHeadRoll( trialParameters[currentTrial].targetHeadTilt, trialParameters[currentTrial].targetHeadTiltTolerance );
	// Set a time limit to achieve the desired head orientation.
	TimerSet( stateTimer,  trialParameters[currentTrial].targetHeadTiltDuration ); 
}
GraspTrialState GraspTaskManager::UpdateAlignHead( void ) { 
	// Update the feedback about the head orientation wrt the desired head orientation.
	// If the head alignment is satisfactory, move on to the next state.
	if ( aligned == HandleHeadAlignment( true ) ) return( PresentTarget ); 
	else if ( TimerTimeout( stateTimer ) ) return( Timeout ); 
	else return( currentState );
}
void GraspTaskManager::ExitAlignHead( void ) {}

// PresentTarget
// The target is diplayed to the subejct.
// Here don't actually display anything, but the derived classes will.
void GraspTaskManager::EnterPresentTarget( void ) {
	renderer->orientationTarget->SetOrientation( trialParameters[currentTrial].targetOrientation, 0.0, 0.0 );
	TimerSet( stateTimer, trialParameters[currentTrial].targetPresentationDuration ); 
	char tag[32];
	sprintf( tag, "Target%03d", currentTrial );
	dexServer->SnapPicture( tag );
}
GraspTrialState GraspTaskManager::UpdatePresentTarget( void ) { 
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed.
	if ( aligned != HandleHeadAlignment( false ) ) return( TrialInterrupted );
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
	// Change the sky background to remove the visual reference that it provides.
	renderer->starrySky->Disable();
	renderer->darkSky->Enable();
}

// TiltHead
// The subject is guided to a new tilt orientation of the head.
// Conflict may be added during the motion.
void GraspTaskManager::EnterTiltHead( void ) {
	// Set a fixed duration for this state.
	// Even if the desired head tilt is achieved, we stay here until the time is up.
	TimerSet( stateTimer, trialParameters[currentTrial].responseHeadTiltDuration ); 
	// Set the desired tilt of the head.
	// Normally this would come from the stimulus sequence.
	SetDesiredHeadRoll( trialParameters[currentTrial].responseHeadTilt, trialParameters[currentTrial].responseHeadTiltTolerance );
}
GraspTrialState GraspTaskManager::UpdateTiltHead( void ) { 
	// Update the visual feedback about the head tilt.
	AlignmentStatus status = HandleHeadAlignment( true );
	// Stay in this state a fixed time.
	if ( TimerTimeout( stateTimer ) ) {
		if ( status != misaligned ) return( ObtainResponse );
		else return( TrialInterrupted ); 
	}
	// Allow an operator to force a move forward. This can be used in a training situation
	//  where the time allowed to tilt the head is very long but we don't want to wait if the subject
	//  succeeds in a short amount of time.
	if ( oculusDisplay->KeyDownEvents['G'] && status == aligned ) return( ObtainResponse ); 
	return( currentState );
}
void  GraspTaskManager::ExitTiltHead( void ) {
	// Remove the prompt about which way to tilt.
}

// ObtainResponse
// The subject reproduces the remembered orientation.
void GraspTaskManager::EnterObtainResponse( void ) {
	// Set a timeout timer to limit the time allowed to respond.
	TimerSet( stateTimer, trialParameters[currentTrial].responseTimeout ); 
	// Show where to aim without showing the target orientation.
	renderer->positionOnlyTarget->Enable();
}
GraspTrialState GraspTaskManager::UpdateObtainResponse( void ) { 
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed. Interrupt the trial if not.
	if ( misaligned == HandleHeadAlignment( false ) ) return( TrialInterrupted );
	// Handle triggering and moving the projectiles.
	if ( Validate() ) {
		// Record the response.
		fprintf( response_fp, "%8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		fOutputDebugString( "Response: %8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		return( ProvideFeedback );
	}
	if ( TimerTimeout( stateTimer ) ) return( Timeout ); 
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitObtainResponse( void ) {
	// Make sure that the head tilt prompt is not still present.
	renderer->tiltPrompt->Disable();
	renderer->positionOnlyTarget->Disable();
	char tag[32];
	sprintf( tag, "Respns%03d", currentTrial );
	dexServer->SnapPicture( tag );
}

// ProvideFeedback
// The subject sees the result of his or her response.
void GraspTaskManager::EnterProvideFeedback( void ) {
	// Show the target.
	if ( trialParameters[currentTrial].provideFeedback == 1 ) renderer->orientationTarget->Enable();
	else renderer->positionOnlyTarget->Enable();
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
	renderer->positionOnlyTarget->Disable();
	renderer->projectiles->Disable();
}

// TrialCompleted
// Provide an indication that the trial was completed successfully.
// Then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialCompleted( void ) {
	// Put the target head orientation back to upright so that the subject can start moving there already.
	SetDesiredHeadRoll( 0.0, trialParameters[currentTrial].targetHeadTiltTolerance );
	// Show the success indicator.
	// renderer->successIndicator->Enable();
	// Show it for a fixed time.
	//TimerSet( stateTimer, indicatorDisplayDuration ); 
	TimerSet( stateTimer, 0.0 ); 
}
GraspTrialState GraspTaskManager::UpdateTrialCompleted( void ) { 
	// After timer runs out, move on to the next trial or exit.
	if ( TimerTimeout( stateTimer ) ) {
		currentTrial++;
		if ( currentTrial < nTrials ) return( StartTrial ); 
		else return( BlockCompleted );
	}
	// Otherwise, continue in this state.
	HandleHeadAlignment( false );
	return( currentState );
}
void  GraspTaskManager::ExitTrialCompleted( void ) {
	renderer->successIndicator->Disable();
}

// BlockCompleted
// Provide an indication that the block was completed successfully.
// Promt the subject to doff the HMD.
void GraspTaskManager::EnterBlockCompleted( void ) {
	// Show the success indicator.
	renderer->blockCompletedIndicator->Enable();
	SetDesiredHeadRoll( trialParameters[currentTrial].targetHeadTilt, trialParameters[currentTrial].targetHeadTiltTolerance );
}
GraspTrialState GraspTaskManager::UpdateBlockCompleted( void ) { 
	// After timer runs out, move on to the next trial or exit.
	if ( Validate() ||  oculusDisplay->KeyDownEvents['\r'] ) return( ExitStateMachine );
	// Otherwise, continue in this state.
	HandleHeadAlignment( false );
	HandleSpinningPrompts();
	return( currentState );
}
void  GraspTaskManager::ExitBlockCompleted( void ) {
	renderer->blockCompletedIndicator->Disable();
}

// TrialInterrupted
// Occurs when the required head tilt is not maintained.
// Show a message, then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialInterrupted( void ) {
	fprintf( response_fp, "%f; interrupted\n", 0.0 );
	// Show the success indicator.
	renderer->headMisalignIndicator->Enable();
	char tag[32];
	sprintf( tag, "Intrpt%03d", currentTrial );
	dexServer->SnapPicture( tag );
}
GraspTrialState GraspTaskManager::UpdateTrialInterrupted( void ) { 
	// Show the message until the subject presses a button.
	if ( Validate() ) {		
		// If retry count has not been exceeded and if there is room
		//  copy the current trial paramters to the end of the list
		//  of trials to be performed.
		RepeatTrial( currentTrial ); 
		// Move on to the next trial.
		currentTrial++;
		if ( currentTrial < nTrials ) return( StartTrial ); 
		else return( ExitStateMachine );
	}
	// Otherwise, continue in this state.
	HandleHeadAlignment( false );
	HandleSpinningPrompts();
	return( currentState );
}
void  GraspTaskManager::ExitTrialInterrupted( void ) {
	renderer->headMisalignIndicator->Disable();
}

// Timeout
// Provide an indication that the subject took too long to respond.
// Wait for a button press, then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTimeout( void ) {
	fprintf( response_fp, "%f; timeout\n", 0.0 );
	// Show the success indicator.
	renderer->timeoutIndicator->Enable();
	SetDesiredHeadRoll( trialParameters[currentTrial].targetHeadTilt, trialParameters[currentTrial].targetHeadTiltTolerance );
}
GraspTrialState GraspTaskManager::UpdateTimeout( void ) { 
	// When the subject presses a button, move on..
	if ( Validate() ) {
		// If retry count has not been exceeded and if there is room
		//  copy the current trial paramters to the end of the list
		//  of trials to be performed.
		RepeatTrial( currentTrial ); 
		// Move on to the next trial.
		currentTrial++;
		if ( currentTrial < nTrials ) return( StartTrial ); 
		else return( ExitStateMachine );
	}
	// Otherwise, continue in this state.
	HandleHeadAlignment( false );
	HandleSpinningPrompts();
	return( currentState );
}
void  GraspTaskManager::ExitTimeout( void ) {
	renderer->timeoutIndicator->Disable();
}
