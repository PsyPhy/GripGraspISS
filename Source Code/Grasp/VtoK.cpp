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
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}
void  VtoK::ExitPresentTarget( void ) {
	// Hide the visible target.
	renderer->orientationTarget->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitPresentTarget();
}

void VtoK::EnterObtainResponse( void ) {
	// Show the visual representation of the hand that is driven 
	//  by the mouse or buttons.
	renderer->kTool->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}

void VtoK::ExitObtainResponse( void ) {
	// Hide the hand.
	renderer->kTool->Disable();
	// Do all the default actions as well.
	GraspTaskManager::ExitObtainResponse();
}
