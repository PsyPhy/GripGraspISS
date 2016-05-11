/*********************************************************************************/
/*                                                                               */
/*                                CascadePoseTracker.cpp                         */
/*                                                                               */
/*********************************************************************************/

// A pose tracker that combines the CODA and Oculus instruments.

#include "stdafx.h"

#include "CascadePoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"

using namespace PsyPhy;

CascadePoseTracker::CascadePoseTracker( PoseTracker *tracker ) {
	if ( tracker ) AddTracker( tracker );
	PoseTracker::PoseTracker();
}

int CascadePoseTracker::AddTracker( PoseTracker *tracker ) {
	fAbortMessageOnCondition( ( nTrackers < MAX_CASCADED_TRACKERS ), "CascadePoseTracker", "Too many cascaded trackers: %d", nTrackers + 1 );
	this->tracker[nTrackers] = tracker;
	nTrackers++;
	return( nTrackers );
}


bool CascadePoseTracker::Initialize( void ) { 
	PoseTracker::Initialize();
	return true; 
}

bool CascadePoseTracker::Update( void ) { 
	// Here we just update each of the individual trackers.
	// What we should do is perhaps align each tracker to the previous one
	//  at each time step, whenever both members of the pair are available.
	for ( int trk = 0; trk < nTrackers; trk++ ) tracker[trk]->Update();
	return( true );
}

bool CascadePoseTracker::Quit( void ) { 
	return true; 
}

bool CascadePoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose ) { 
	static int cycle_counter = 0;
	TrackerPose component_pose;
	component_pose.visible = false;
	component_pose.time = 0.0;
	for ( int trk = 0; trk < nTrackers; trk++ ) {
		tracker[trk]->GetCurrentPose( &component_pose );
		if ( component_pose.visible ) {
			CopyVector( pose->position, component_pose.position );
			CopyQuaternion( pose->orientation, component_pose.orientation );
			pose->time = component_pose.time;
			pose->visible = component_pose.visible;
			fOutputDebugString( "%8d Component Tracker %d\n", cycle_counter, trk );
			break;
		}
	}
	cycle_counter++;
	return pose->visible; 

}

