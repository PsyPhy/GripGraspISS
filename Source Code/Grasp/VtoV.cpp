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

/// VtoV

void VtoV::EnterPresentTarget( void ) {
	// The target is displayed visually.
	renderer->orientationTarget->Enable();
	TimerSet( presentTargetTimer, targetPresentationDuration ); 
	// Do all the default actions as well.
	GraspTaskManager::EnterPresentTarget();
}

void VtoV::EnterObtainResponse( void ) {
	// Set the virtual hand to a random oreintation.
	// Here is a list of possible intial roll angles.
	static double angles[] = { -60.0, -50.0, -40.0, -30.0, -20.0, -10.0, 0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0 };
	// Automatically compute the number of values in the list.
	int nvalues = sizeof( angles ) / sizeof( *angles );
	// Now compute an entry into the list. This could be a random number
	// but even random number generators need a seed. Here I take the 
	// number of miliseconds since the start of the block and wrap it into the 
	// number of entries in the list. That should be fairly random and it avoids
	// having to seed a random number generator.
	int index = ((int) (TimerElapsedTime( blockTimer ) * 1000.0) ) % nvalues; 
	// Compute the pose of the virtual hand, taking into account the 
	// defined position of the virtual hand pose and the randomly generated
	// roll angle.
	Pose pose;
	CopyPose( pose, trackers->handPoseV );
	SetQuaternion( pose.orientation, ToRadians( angles[index] ), kVector );
	trackers->rollTracker->BoresightTo( pose );
	// The above code moves the roll tracker to the random roll orientation.
	// But we are about to enable the virtual tool and it will be rendered
	// once at the old position, because of the order of actions in the event loop.
	// Rather than change the order in the event looop, which is risky, I am going 
	// to hack the orientation of the virtual hand here. Later we can fix this.
	fOutputDebugString( "Initial Angle:  %8.3f -> %d  %.2f \n", TimerElapsedTime( blockTimer ), index, angles[index] );
	renderer->vTool->SetOrientation( pose.orientation );

	// Show the visual representation of the hand that is driven 
	//  by the mouse or buttons.
	renderer->vTool->Enable();
	// Do all the default actions as well.
	GraspTaskManager::EnterObtainResponse();
}
