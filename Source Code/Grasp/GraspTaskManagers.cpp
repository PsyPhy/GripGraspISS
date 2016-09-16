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

// Amount of time to show a visual target.
double	GraspTaskManager::targetPresentationDuration = 3.0;

// Time to show the trial success indicator.
// If it is set to 0 there is no presentation of the success indicator.
double GraspTaskManager::indicatorDisplayDuration = 1.0;	

// Time limits to accomplish different actions.
double GraspTaskManager::alignHeadTimeout = 30.0;
double GraspTaskManager::tiltHeadTimeout = 5.0;
double GraspTaskManager::responseTimeout = 10.0;
double GraspTaskManager::alignHandTimeout = 30.0;

// Required precision for aligning the head.
// I am setting these values to what I think they should be for training.
// The head tilt tolerances are rather large.
double	GraspTaskManager::targetHeadTiltTolerance = 10.0;
double	GraspTaskManager::responseHeadTiltTolerance = 10.0;
double	GraspTaskManager::hapticTargetOrientationTolerance = 10.0;

// It is easy to forget to raise and lower the hand, but it is annoying to be prompted each time.
// The following parameter set delays before displaying the prompt.
double GraspTaskManager::handPromptDelay = 2.0;
double GraspTaskManager::handErrorDelay = 2.0;

int GraspTaskManager::maxRetries = 9;

bool GraspTaskManager::manualStraightenHead = true;
//
// A state machine to run through the different state of a block of trials.
// On entry, the current state is compared to the previous state.
// If the are different, the machine calls the EnterXXX() method for the current state, where XXX is one of the possible GraspTrialStates.
// Then, in either case, it calls the UpdateXXX() method, which returns the subsequent state.
// If the subsequent state is not the same as the current state, the ExitXXX() method is called for the current state before leaving the cycle.
//
bool GraspTaskManager::UpdateStateMachine( void ) {

	// If the state has changed, tell the ground where we are.
	// This happens automatically for any state change.
	// Specific state handlers might also add additional information.
	if ( currentState != previousState ) ShowProgress( currentState );

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

		int items = sscanf( line, "%lf; %lf; %lf; %lf; %d",
			&trialParameters[nTrials].targetHeadTilt,
			&trialParameters[nTrials].targetOrientation,
			&trialParameters[nTrials].responseHeadTilt,
			&trialParameters[nTrials].conflictGain,
			&trialParameters[nTrials].provideFeedback );

		if ( items == 5 ) {
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

void GraspTaskManager::ShowProgress( int phase, int characteristic ) {
	int info = ( nTrials - currentTrial ) * 100 + phase + characteristic;
	dexServices->SendSubstep( info );
}

int GraspTaskManager::RunTrialBlock( char *sequence_filename, char *output_filename_root ) {

	// Load the trial parameters.
	int trials = LoadTrialParameters( sequence_filename );
	fAbortMessageOnCondition( ( trials <= 0 ), "GraspTaskManager", "No valid trials loaded from file %s.", sequence_filename );
	fOutputDebugString( "Loaded paramters for %d trials from file %s.\n", trials, sequence_filename );
	currentTrial = 0;

	// Initiliaze the counter that limits the number of retries.
	retriesRemaining = maxRetries;

	// Open a file for storing the responses and output a header.
	sprintf( responseFilename, "%s.rsp", output_filename_root );
	response_fp = fopen( responseFilename, "w" );
	fAbortMessageOnCondition( !response_fp, "GraspTaskManager", "Error opening file %s for writing.", responseFilename );
	fprintf( response_fp, "trial; type; targetHeadTilt; targetHeadTiltTolerance; targetHeadTiltDuration; targetOrientation; hapticTargetOrientationTolerance; targetPresentationDuration; responseHeadTilt; responseHeadTiltTolerance; responseHeadTiltDuration; responseTimeout; conflictGain; feedback (0 or 1); time; response\n" );

	// Open a file for storing the tracker poses and output a header.
	sprintf( poseFilename, "%s.pse", output_filename_root );
	pose_fp = fopen( poseFilename, "w" );
	fAbortMessageOnCondition( !pose_fp, "GraspTaskManager", "Error opening file %s for writing.", poseFilename );
	fprintf( pose_fp, "trial; time; head.time; head.visible; head.position; head.orientation; hand.time; hand.visible; hand.position; hand.orientation; chest.time; chest.visible; chest.position; chest.orientation; roll.time; roll.visible; roll.position; roll.orientation\n" );

	// Call the paradigm-specific preparation, if any.
	Prepare();

	// Initialize the state machine.
	previousState = NullState;
	currentState = StartBlock;

	// Tell the ground what we are about to start doing.
	ShowProgress( StartBlock, GetParadigm() );

	// Measure the time since this block was initiated.
	TimerStart( blockTimer );

	// Enter into the rendering loop and handle other messages.
	while ( oculusDisplay->HandleMessages() ) {

		static int cycle_counter = 0;

		if ( cycle_counter % 1000 == 0 ) fOutputDebugString( "GraspTaskManager cycle: %d  %f\n", cycle_counter, TimerElapsedTime( blockTimer ) );
		cycle_counter++;

		// Update pose of tracked objects, including the viewpoint.
		UpdateTrackers();
		dexServices->SendTrackerStatus( trackers->GetTrackerStatus() );

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

		// Boresight the HMD tracker on 'B' or align to the HMD on 'A'.
		// This is here for debugging and should probably be removed.
		if ( oculusDisplay->KeyDownEvents['A'] ) AlignToHMD();
		if ( oculusDisplay->KeyDownEvents['B'] ) trackers->hmdTracker->Boresight();
		if ( oculusDisplay->KeyDownEvents['U'] ) trackers->hmdTracker->Unboresight();

		// Update the state machine. If it returns true it means that we have 
		//  finished the current block of trials.
		if ( UpdateStateMachine() ) break;

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
	// The desired orientation of the head is upright (0°). 
	// This is not essential, but it allows us to show the colors already.
	SetDesiredHeadRoll( trialParameters[0].targetHeadTilt, targetHeadTiltTolerance );
	// Show the "Press to continue." indicator.
	renderer->readyToStartIndicator->Enable();
	// Show the hand, just to allow the subject to play a little.
	renderer->vkTool->Enable();
}
GraspTrialState GraspTaskManager::UpdateStartBlock( void ) { 
	// Modulate the halo color, even though it does not matter, so
	// that the subject gets into that mode of operation.
	HandleHeadAlignment( false );
	// When the subject clicks a button, we move on to start the first trial
	if ( Validate() ) return( StartTrial );
	// Otherwise, continue in this state.
	return( currentState );
}
void  GraspTaskManager::ExitStartBlock( void ) {
	renderer->readyToStartIndicator->Disable();
	renderer->vkTool->Disable();
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
	fprintf( response_fp, "%d;  %s; %5.2f; %5.2f; %5.2f;   %6.2f; %5.2f; %5.2f;   %6.2f; %5.2f; %5.2f; %5.2f;   %4.2f; %d;",
		currentTrial,
		tag,

		trialParameters[currentTrial].targetHeadTilt,
		targetHeadTiltTolerance,
		alignHeadTimeout,

		trialParameters[currentTrial].targetOrientation,
		hapticTargetOrientationTolerance,
		targetPresentationDuration,

		trialParameters[currentTrial].responseHeadTilt,
		responseHeadTiltTolerance,
		tiltHeadTimeout,
		responseTimeout,

		trialParameters[currentTrial].conflictGain,
		trialParameters[currentTrial].provideFeedback );

	// Make sure that this information gets written right away to facilitate degugging if we crash.
	// Note that we do not output a \n in the above. The line will be completed by the response.
	fflush( response_fp );

	// Indicate which trial is about to begin. For now we simply show this on the debug text window.
	// In the future it should send this info to ground.
	fOutputDebugString( "Starting Trial: %d / %d Target: %6.1f  Head Tilt: %6.1f  Conflict: %4.2f\n",
		currentTrial, nTrials, trialParameters[currentTrial].targetOrientation, trialParameters[currentTrial].targetHeadTilt, trialParameters[currentTrial].conflictGain );
	// Tell the ground how many retries are remaining.
	ShowProgress( StartTrial, retriesRemaining );
}
GraspTrialState GraspTaskManager::UpdateStartTrial( void ) { 
	return( StraightenHead ); 
}
void GraspTaskManager::ExitStartTrial( void ) {
	char tag[32];
	sprintf( tag, "StartT%03d", currentTrial );
	dexServices->SnapPicture( tag );
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
	renderer->headTiltPrompt->Disable();
	if ( manualStraightenHead ) {
		renderer->straightenHeadIndicator->Enable();
		renderer->glasses->SetColor( 0.0f, 0.1f, 1.0f, 0.35f );
	}
	else {
		// The desired orientation of the head to zero in preparation for applying conflict (if any).
		SetDesiredHeadRoll( 0.0, targetHeadTiltTolerance );
		// Show a central target and a laser pointer that moves with the head to facilitate straight-ahead gaze.
		renderer->straightAheadTarget->Enable();
		renderer->gazeLaser->Enable();
	}
	// The hand must be lowered during this phase. Show the position of the hand to remind the subject.
	renderer->kTool->Enable();
	// Set a timeout to wait for head at proper alignment.
	TimerSet( straightenHeadTimer, alignHeadTimeout );
}
GraspTrialState GraspTaskManager::UpdateStraightenHead( void ) { 
	// Check for time limit exceeded.
	if ( TimerTimeout( straightenHeadTimer ) ) {
		interruptCondition = HEAD_ALIGNMENT_TIMEOUT;
		return( TrialInterrupted );
	}
	if ( Validate() ) return( AlignHead );
	if ( raised == HandleHandElevation() && TimerElapsedTime( straightenHeadTimer ) > handPromptDelay ) {
		renderer->lowerHandPrompt->Enable();
	}
	else renderer->lowerHandPrompt->Disable();
	// Update the feedback about the head orientation wrt the desired head orientation.
	// If the head alignment is satisfactory, move on to the next state.
	// if ( aligned == HandleHeadOnShoulders( false ) ) return( AlignHead ); 
	return( currentState );
}
void GraspTaskManager::ExitStraightenHead( void ) {
	renderer->straightAheadTarget->Disable();
	// Set the conflict gain.
	conflictGain = trialParameters[currentTrial].conflictGain;
	// Align to the current position of the HMD. 
	AlignToHMD();
	renderer->straightAheadTarget->Disable();
	renderer->gazeLaser->Disable();
	// Remove the prompts about lowering the hand, if any.
	renderer->lowerHandIndicator->Disable();
	renderer->lowerHandPrompt->Disable();
	renderer->kTool->Disable();
	renderer->straightenHeadIndicator->Disable();
	// Logically one might want to enable rendering of the room before leaving this state.
	// But that causes an artifact because the room gets drawn once at the 'old' alignment.
	// So I move enabling of the room to the next state.
	// renderer->room->Enable();
}

//
// AlignHead
// The subject is guided to align the head in preparation for observing the target.
// For the ISS, the head tilt will most likely be zero for target acquisition for
// all trials, but the sequence entries allow one to specify a different orientation if desired.
void GraspTaskManager::EnterAlignHead( void ) { 
	// See above.
	renderer->room->Enable();
	// The desired orientation of the head to the specified head orientation.
	SetDesiredHeadRoll( trialParameters[currentTrial].targetHeadTilt, targetHeadTiltTolerance );
	// Set a time limit to achieve the desired head orientation.
	TimerSet( alignHeadTimer,  alignHeadTimeout ); 
}
GraspTrialState GraspTaskManager::UpdateAlignHead( void ) { 
	// Update the feedback about the head orientation wrt the desired head orientation.
	// If the head alignment is satisfactory and the hand is not raised, move on to the next state.
	if ( aligned == HandleHeadAlignment( true ) ) return( PresentTarget ); 
	else if ( TimerTimeout( alignHeadTimer ) ) {
		renderer->lowerHandPrompt->Disable();
		interruptCondition = HEAD_ALIGNMENT_TIMEOUT;
		return( TrialInterrupted );
	}
	else return( currentState );
}
void GraspTaskManager::ExitAlignHead( void ) {}

// PresentTarget
// The target is diplayed to the subject.
// Here don't actually display anything, but the derived classes will.
void GraspTaskManager::EnterPresentTarget( void ) {
	char tag[32];
	sprintf( tag, "Target%03d", currentTrial );
	dexServices->SnapPicture( tag );
	renderer->orientationTarget->SetOrientation( trialParameters[currentTrial].targetOrientation, 0.0, 0.0 );
}
GraspTrialState GraspTaskManager::UpdatePresentTarget( void ) { 
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed.
	if ( misaligned == HandleHeadAlignment( false ) ) {
		interruptCondition = HEAD_MISALIGNMENT;
		return( TrialInterrupted );
	}
	// If we haven't already returned based on some condition, continue in
	// this state for the next cycle.
	return( currentState );
}

GraspTrialState GraspTaskManager::UpdateVisualTarget( void ) { 
	// Stay in this state for a fixed time.
	// Nominally, the next step is to tilt the head prior to responding.
	if ( TimerTimeout( presentTargetTimer ) ) return( TiltHead ); 
	return( GraspTaskManager::UpdatePresentTarget() );
	// Check if the hand has been raised, and if so, signal an error.
	if ( raised == HandleHandElevation() ) {
		interruptCondition = RAISED_HAND_VIOLATION;
		return( TrialInterrupted );
	}
}

GraspTrialState GraspTaskManager::UpdateKinestheticTarget( void ) { 
	// Limit the time allowed to achieve the target hand orientation.
	if ( TimerTimeout( alignHandTimer ) ) {
		interruptCondition = ALIGN_HAND_TIMEOUT;
		return( TrialInterrupted );
	}
	// Check if the hand has been raised, and if not, show the prompt.
	if ( lowered == HandleHandElevation() && TimerElapsedTime( alignHandTimer ) > handPromptDelay ) renderer->raiseHandIndicator->Enable();
	else renderer->raiseHandIndicator->Disable();
	// Modulate the color of the hand to guide the subject to a kinesthetic target orientation.
	if ( aligned == HandleHandAlignment( true ) ) return( TiltHead );
	return( GraspTaskManager::UpdatePresentTarget() );
}

void  GraspTaskManager::ExitPresentTarget( void ) {
	// Hide the visible targets.
	renderer->orientationTarget->Disable();
	renderer->positionOnlyTarget->Disable();
	// Hide the visible tools, if any.
	renderer->kkTool->Disable();
	renderer->handRollPrompt->Disable();
	// Hide the wrist zone indication and the raise hand indicator, if they were on.
	renderer->wristZone->Disable();
	renderer->raiseHandIndicator->Disable();
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
	TimerSet( tiltHeadTimer, tiltHeadTimeout ); 
	// Set the desired tilt of the head.
	// Normally this would come from the stimulus sequence.
	SetDesiredHeadRoll( trialParameters[currentTrial].responseHeadTilt, responseHeadTiltTolerance );
	// The hand must be lowered during this phase. Show the position of the hand to remind the subject.
	renderer->kTool->Enable();
}
GraspTrialState GraspTaskManager::UpdateTiltHead( void ) { 
	// Update the visual feedback about the head tilt.
	AlignmentStatus status = HandleHeadAlignment( true );
	// The hand must be lowered before leaving this state. If it is still raised after a short
	// delay, we activate the prompt to remind the subject to lower the arm.
	if ( raised == HandleHandElevation() && TimerElapsedTime( tiltHeadTimer ) > handPromptDelay ) renderer->lowerHandPrompt->Enable();
	else renderer->lowerHandPrompt->Disable();
	// The hand must be lowered before leaving this state.
	// If we are almost to the end of the time allocated to tilt the head,
	// the signal the anomaly and interrupt the trial.
	if ( raised == HandleHandElevation() && TimerRemainingTime( tiltHeadTimer ) < handErrorDelay ) {
		renderer->lowerHandIndicator->Disable();
		interruptCondition = RAISED_HAND_VIOLATION;
		return( TrialInterrupted ); 
	}
	// Stay in this state a fixed time, even if the desired head tilt has been achieved.
	if ( TimerTimeout( tiltHeadTimer ) ) {
		// As long as the head is not judged to be misaligned, go ahead a obtain the response.
		// We say ( status != misaligned ) and not ( status == aligned ) because the head may be heading 
		//  out of the tolerance range, but has not been there long enough to be considered an error.
		if ( status != misaligned ) return( ObtainResponse );
		else {
			interruptCondition = HEAD_TILT_TIMEOUT;
			return( TrialInterrupted ); 
		}
	}
	// Allow an operator to force a move forward. This can be used in a training situation
	//  where the time allowed to tilt the head is very long but we don't want to wait if the subject
	//  succeeds in a short amount of time.
	if ( oculusDisplay->KeyDownEvents['G'] && status == aligned ) return( ObtainResponse ); 
	return( currentState );
}
void  GraspTaskManager::ExitTiltHead( void ) {
	// Remove the prompts about lowering the hand, if any.
	renderer->lowerHandIndicator->Disable();
	renderer->lowerHandPrompt->Disable();
	renderer->kTool->Disable();
}

// ObtainResponse
// The subject reproduces the remembered orientation.
void GraspTaskManager::EnterObtainResponse( void ) {
	// Set a timeout timer to limit the time allowed to respond.
	TimerSet( responseTimer, responseTimeout ); 
	// Show where to aim without showing the target orientation.
	renderer->positionOnlyTarget->Enable();
}
GraspTrialState GraspTaskManager::UpdateObtainResponse( void ) { 
	// Limit the time allowed to give a response
	if ( TimerTimeout( responseTimer ) ) {
		interruptCondition = RESPONSE_TIMEOUT;
		return( TrialInterrupted ); 
	}
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed. Interrupt the trial if not.
	if ( misaligned == HandleHeadAlignment( false ) ) {
		interruptCondition = HEAD_MISALIGNMENT;
		return( TrialInterrupted );
	}
	return( currentState );
}
// Here we provide a common routing to handle updating during a visual 
// response. It does not get called by the base class, but a derived class
// can call it from within its own UpdateOptainResponse() handler.
GraspTrialState GraspTaskManager::UpdateVisualResponse( void ) { 
	// Handle triggering and moving the projectiles.
	if ( Validate() ) return( ProvideFeedback );
	return( GraspTaskManager::UpdateObtainResponse() );
}
// Here we provide a common routing to handle updating during a kinesthetic 
// response. It does not get called by the base class, but a derived class
// can call it from within its own UpdateOptainResponse() handler.
GraspTrialState GraspTaskManager::UpdateKinestheticResponse( void ) { 
	// Check if the hand has been raised, and if not show the prompt.
	ArmStatus arm_state = HandleHandElevation();
	if ( lowered == arm_state && TimerElapsedTime( responseTimer ) > handPromptDelay ) renderer->raiseHandIndicator->Enable();
	else renderer->raiseHandIndicator->Disable();
	// If the hand is raised and the subject presses valide, record the response and move one.
	if ( raised == arm_state && Validate()  ) return( ProvideFeedback );
	return( GraspTaskManager::UpdateObtainResponse() );
}

void  GraspTaskManager::ExitObtainResponse( void ) {
	// Make sure that the head tilt prompt is not still present.
	renderer->headTiltPrompt->Disable();
	// Hide the hand. To make this routine generic, we 
	//  hide all possible representations of the hand;
	renderer->kTool->Disable();
	renderer->vTool->Disable();
	renderer->vkTool->Disable();
	renderer->wristZone->Disable();
	renderer->raiseHandIndicator->Disable();
	renderer->positionOnlyTarget->Disable();
	char tag[32];
	sprintf( tag, "Respns%03d", currentTrial );
	dexServices->SnapPicture( tag );
}

// ProvideFeedback
// The subject sees the result of his or her response.
void GraspTaskManager::EnterProvideFeedback( void ) {
	// Record the response.
	fprintf( response_fp, "%8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
	fOutputDebugString( "Response: %8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
	// Show the target.
	if ( trialParameters[currentTrial].provideFeedback == 1 ) {
		renderer->orientationTarget->Enable();
		renderer->multiProjectile->Enable();
	}
	else {
		renderer->positionOnlyTarget->Enable();
		renderer->monoProjectile->Enable();
	}
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
	renderer->multiProjectile->Disable();
	renderer->monoProjectile->Disable();
}


// TrialCompleted
// Provide an indication that the trial was completed successfully.
// Then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialCompleted( void ) {
	// Put the target head orientation back to upright so that the subject can start moving there already.
	SetDesiredHeadRoll( 0.0, targetHeadTiltTolerance );
	// Show the success indicator.
	renderer->successIndicator->Enable();
	// Show it for a fixed time.
	TimerSet(trialCompletedTimer, indicatorDisplayDuration ); 
}
GraspTrialState GraspTaskManager::UpdateTrialCompleted( void ) { 
	// After timer runs out, move on to the next trial or exit.
	if ( TimerTimeout( trialCompletedTimer ) ) {
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
	SetDesiredHeadRoll( trialParameters[currentTrial].targetHeadTilt, targetHeadTiltTolerance );
}
GraspTrialState GraspTaskManager::UpdateBlockCompleted( void ) { 
	// After timer runs out, move on to the next trial or exit.
	if ( Validate() ||  oculusDisplay->KeyDownEvents['\r'] ) return( ExitStateMachine );
	// Otherwise, continue in this state.
	HandleHeadAlignment( false );
	return( currentState );
}
void  GraspTaskManager::ExitBlockCompleted( void ) {
	renderer->blockCompletedIndicator->Disable();
}

// TrialInterrupted
// Occurs on various error conditons.
// The variable interruptIndicator should be set before entering here to 
//  show to the subect the condition that provoked the error.
// Show a message, then move on to next trial, or exit the sequence.
void GraspTaskManager::EnterTrialInterrupted( void ) {
	fprintf( response_fp, "%f; interrupted\n", 0.0 );
	// Tell the ground what caused the interruption.
	ShowProgress( TrialInterrupted, interruptCondition );
	// Show the cause of the interruption.
	switch ( interruptCondition ) {
	case RAISE_HAND_TIMEOUT: interrupt_indicator = renderer->raiseArmTimeoutIndicator; break;
	case LOWER_HAND_TIMEOUT: interrupt_indicator = renderer->lowerArmTimeoutIndicator; break;
	case RAISED_HAND_VIOLATION: interrupt_indicator = renderer->handShouldNotBeRaisedIndicator; break;
	case HAND_TOO_SOON: interrupt_indicator = renderer->handTooSoonIndicator; break;
	case ALIGN_HAND_TIMEOUT: interrupt_indicator = renderer->handRotateTimeoutIndicator; break;
	case RESPONSE_TIMEOUT: interrupt_indicator = renderer->responseTimeoutIndicator; break;
	case HEAD_ALIGNMENT_TIMEOUT: interrupt_indicator = renderer->headAlignTimeoutIndicator; break;
	case HEAD_TILT_TIMEOUT: interrupt_indicator = renderer->headTiltTimeoutIndicator; break;
	case HEAD_MISALIGNMENT: interrupt_indicator = renderer->headMisalignIndicator; break;
	}
	interrupt_indicator->Enable();
	char tag[32];
	sprintf( tag, "Itpt%1d.%03d", interruptCondition, currentTrial );
	dexServices->SnapPicture( tag );
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
	return( currentState );
}
void  GraspTaskManager::ExitTrialInterrupted( void ) {
	interrupt_indicator->Disable();
}

