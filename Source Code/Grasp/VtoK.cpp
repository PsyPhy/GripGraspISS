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


/// VtoK

void VtoK::EnterPresentTarget( void ) {
	// The target is displayed visually.
	renderer->orientationTarget->Enable();
	TimerSet( presentTargetTimer, targetPresentationDuration ); 
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}


void VtoK::EnterObtainResponse( void ) {
	// Show the subject where to point.
	renderer->positionOnlyTarget->Enable();
	// Show the visual representation of the hand.
	renderer->kTool->Enable();
	StartHandAiming();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}
