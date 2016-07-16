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
	// Show where to put the wrist.
	//renderer->wristZone->SetColor( 0.0, 1.0, 0.0, 0.05 );
	//renderer->wristZone->Enable();
	// Show the subject where to point.
	renderer->positionOnlyTarget->Enable();
	// Visualize the hand, but not its actual roll orientation, by using the kkTool.
	// But the orientation will be reflected by the color.
	renderer->kkTool->Enable();
	// The desired orientation of the head to the specified head orientation.
	SetDesiredHandRoll( trialParameters[currentTrial].targetOrientation, trialParameters[currentTrial].hapticTargetOrientationTolerance );
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}

GraspTrialState KtoK::UpdatePresentTarget( void ) { 

	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed.
	if ( misaligned == HandleHeadAlignment( false ) ) return( TrialInterrupted );

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
	// Hide the wrist zone indication.
	// renderer->wristZone->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitPresentTarget();
}

void KtoK::EnterObtainResponse( void ) {
	// Show where to put the wrist.
	// renderer->wristZone->SetColor( 0.0, 1.0, 0.0, 0.05 );
	// renderer->wristZone->Enable();
	// Show the visual representation of the hand that is driven 
	//  by the mouse or buttons.
	renderer->kTool->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}

GraspTrialState KtoK::UpdateObtainResponse( void ) { 

	//Pose up = {{50.0, -100.0, -500.0}, {0.0, 0.0, 0.0, 1.0}};
	//Pose down = {{50.0, -250.0, -500.0}, {0.0, 0.0, 0.0, 1.0}};

	//if (  oculusDisplay->Key['W'] ) trackers->handTracker->OffsetTo( up );
	//else trackers->handTracker->OffsetTo( down );

	// Update the visual feedback about the head tilt and see if 
	// the head is still aligned as needed. Interrupt the trial if not.
	if ( misaligned == HandleHeadAlignment( false ) ) return( TrialInterrupted );

	if ( raised == HandleHandElevation() && Validate()  ) {
		// Record the response.
		fprintf( response_fp, "%8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		fOutputDebugString( "Response: %8.3f; %s\n", TimerElapsedTime( blockTimer ), renderer->selectedTool->mstr( renderer->selectedTool->orientation ) );
		return( ProvideFeedback );
	}
	if ( TimerTimeout( stateTimer ) ) return( Timeout ); 
	return( currentState );

}

void KtoK::ExitObtainResponse( void ) {
	// Hide the hand.
	renderer->kTool->Disable();
	// renderer->wristZone->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitObtainResponse();
}
