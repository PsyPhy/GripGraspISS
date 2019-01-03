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

CascadePoseTracker::CascadePoseTracker( PoseTracker *tracker ) : nTrackers(0), performLocalRealignment(false) {
	if ( tracker ) AddTracker( tracker );
	PoseTracker::PoseTracker();
}

int CascadePoseTracker::AddTracker( PoseTracker *tracker ) {
	fAbortMessageOnCondition( ( nTrackers >= MAX_CASCADED_TRACKERS ), "CascadePoseTracker", "Too many cascaded trackers: %d", nTrackers + 1 );
	this->tracker[nTrackers] = tracker;
	nTrackers++;
	return( nTrackers );
}


bool CascadePoseTracker::Initialize( void ) { 
	// Only initialize once.
	if ( initialized ) return true;
	// Initialize the components one-by-one. 
	// Overall success requires that all be successful.
	initialized = true;
	for ( int trk = 0; trk < nTrackers; trk++ ) initialized &= tracker[trk]->Initialize();
	return( initialized );
}

bool CascadePoseTracker::Update( void ) { 
	// Here we just update each of the individual trackers.
	// What we should do is perhaps align each tracker to the previous one
	//  at each time step, whenever both members of the pair are available.
	bool success = true;
	for ( int trk = 0; trk < nTrackers; trk++ ) success &= tracker[trk]->Update();
	return( success );
}

bool CascadePoseTracker::Release( void ) { 
	for ( int trk = 0; trk < nTrackers; trk++ ) tracker[trk]->Release();
	return true; 
}

bool CascadePoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	static int cycle_counter = 0;
	TrackerPose component_pose[MAX_CASCADED_TRACKERS];
	pose.visible = false;
	pose.time = 0.0;
	for ( int trk = nTrackers - 1; trk >= 0; trk++ ) {
		// Compute the pose from each component tracker.
		tracker[trk]->GetCurrentPose( component_pose[trk] );
		// The highest one in the list that is visible will
		// determine the pose of the cascade.
		if ( component_pose[trk].visible ) {
			CopyTrackerPose( pose, component_pose[trk] );
			pose.time = component_pose[trk].time;
			pose.visible = component_pose[trk].visible;
		}
	}
	// Realign each component tracker locally to the immediate previous one, if they 
	// are both visible. This should reduce jumps in the data when there is a transition
	// from one tracker to the next.
	for ( int trk = 1; trk < nTrackers; trk++ ) {
		if ( component_pose[trk-1].visible ) tracker[trk]->BoresightTo( component_pose[trk-1].pose );
	}
	cycle_counter++;
	return pose.visible; 
}


