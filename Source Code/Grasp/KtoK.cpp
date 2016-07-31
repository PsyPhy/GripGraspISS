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

/// KtoK

void KtoK::EnterPresentTarget( void ) {

	// Show the subject where to point.
	renderer->positionOnlyTarget->Enable();
	// Visualize the hand, but not its actual roll orientation, by using the kkTool.
	// But the orientation will be reflected by the color.
	renderer->kkTool->Enable();
	// The target orientation for the hand.
	SetDesiredHandRoll( trialParameters[currentTrial].targetOrientation, trialParameters[currentTrial].hapticTargetOrientationTolerance );
	// Set a timeout to achieve the target orientation of the hand.
	TimerSet( alignHandTimer, alignHandTimeout );
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();

}

GraspTrialState KtoK::UpdatePresentTarget( void ) { 

	// Limit the time allowed to achieve the target hand orientation.
	if ( TimerTimeout( alignHandTimer ) ) {
		interruptCondition = ALIGN_HAND_TIMEOUT;
		return( TrialInterrupted );
	}
	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed.
	if ( misaligned == HandleHeadAlignment( false ) ) {
		interruptCondition = HEAD_MISALIGNMENT;
		return( TrialInterrupted );
	}
	// Check if the hand has been raised, and if not show the prompt.
	if ( lowered == HandleHandElevation() && TimerElapsedTime( alignHandTimer ) > handPromptDelay ) renderer->wristZone->Enable();
	else renderer->wristZone->Disable();
	// Modulate the color of the hand to guide the subject to a kinesthetic target orientation.
	if ( aligned == HandleHandAlignment( true ) ) return( TiltHead );
	// If we haven't returned based on some condition above, continue in
	// this state for the next cycle.
	return( currentState );

}


void  KtoK::ExitPresentTarget( void ) {

	// Hide target position.
	renderer->positionOnlyTarget->Disable();
	// Hide the tool showing the orientation of the hand via color.
	renderer->kkTool->Disable();
	renderer->handRollPrompt->Disable();
	// Hide the wrist zone indication.
	renderer->wristZone->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitPresentTarget();

}

void KtoK::EnterObtainResponse( void ) {

	// Show the visual representation of the hand.
	renderer->kTool->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();

}

GraspTrialState KtoK::UpdateObtainResponse( void ) { 

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
	// Check if the hand has been raised, and if not show the prompt.
	if ( lowered == HandleHandElevation() && TimerElapsedTime( responseTimer ) > handPromptDelay ) renderer->wristZone->Enable();
	else renderer->wristZone->Disable();
	// If the hand is raised and the subject presses valide, record the response and move one.
	if ( raised == HandleHandElevation() && Validate()  ) {
		// Record the response.
		fprintf( response_fp, "%8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		fOutputDebugString( "Response: %8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		return( ProvideFeedback );
	}
	return( currentState );

}

void KtoK::ExitObtainResponse( void ) {

	// Hide the hand.
	renderer->kTool->Disable();
	renderer->wristZone->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitObtainResponse();

}
