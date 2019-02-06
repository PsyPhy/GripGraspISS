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

/// VtoVK

void VtoVK::EnterPresentTarget( void ) {
	// The target is displayed visually.
	renderer->orientationTarget->Enable();
	TimerSet( presentTargetTimer, targetPresentationDuration ); 
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}

void VtoVK::EnterObtainResponse( void ) {
	// Show the visual representation of the hand that is driven 
	//  by the mouse or buttons.
	renderer->vkTool->Enable();
	if ( ! noLasers ) renderer->handLaser->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}
