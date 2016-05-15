/*********************************************************************************/
/*                                                                               */
/*                                PoseTrackers.cpp                               */
/*                                                                               */
/*********************************************************************************/

// A 'PoseTracker' is a device that tracks the 6D position and orientation of a object.
// For the moment it is used only for polled real-time tracking.
// They could later be augmented to include continuous recording at a fixed frequency.

#pragma once

#include "stdafx.h"
#include "PoseTrackers.h"
#include "../Useful/Useful.h"

using namespace PsyPhy;

TrackerPose PsyPhy::NullTrackerPose = {{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}, true, 0.0};

PoseTracker::~PoseTracker () {}

// Boresight so that the specified pose in intrinsic coordinates becomes the null pose.
void PoseTracker::BoresightAt( const Pose &pose ) {
	CopyPose( nullPose, pose );
	nullPose.orientation[X] *= -1.0;
	nullPose.orientation[Y] *= -1.0;
	nullPose.orientation[Z] *= -1.0;
	ScaleVector( nullPose.position, nullPose.position, -1.0 );
}

// Boresight so that the current position and orientation is the null position and orientation.
bool PoseTracker::Boresight( void ) {
	TrackerPose tpose;
	bool success;
	if ( success = GetCurrentPoseIntrinsic( tpose ) ) BoresightAt( tpose.pose );
	return success;
}

// Boresight so that the current pose is now the specified pose.
bool PoseTracker::BoresightTo( const Pose &pose ) {
	bool success;
	if ( success = Boresight() ) {
		MultiplyQuaternions( nullPose.orientation, nullPose.orientation, pose.orientation );
		AddVectors( nullPose.position, nullPose.position, pose.position );
	}
	return success;
}

// Remove boresight correction and return to intinsic reference frame.
void PoseTracker::Unboresight( void ) {
	CopyVector( nullPose.position, zeroVector );
	CopyQuaternion( nullPose.orientation, nullQuaternion );
}

bool PoseTracker::GetCurrentPosition( Vector3 position ) {
	TrackerPose pose;
	GetCurrentPose( pose );
	CopyVector( position, pose.pose.position );
	return( pose.visible );
}
bool PoseTracker::GetCurrentOrientation( Quaternion orientation ) {
	TrackerPose tpose;
	GetCurrentPose( tpose );
	CopyVector( orientation, tpose.pose.orientation );
	return( tpose.visible );
}
bool PoseTracker::GetCurrentPose( TrackerPose &pose ) {
	TrackerPose intrinsic_pose;
	GetCurrentPoseIntrinsic( intrinsic_pose );
	MultiplyQuaternions( pose.pose.orientation,  nullPose.orientation, intrinsic_pose.pose.orientation );
	AddVectors( pose.pose.position, intrinsic_pose.pose.position, nullPose.position );
	pose.visible = intrinsic_pose.visible;
	pose.time = intrinsic_pose.time;
	return( pose.visible );
}

void PoseTracker::CopyTrackerPose( TrackerPose &destination, TrackerPose &source ) {
	destination.time = source.time;
	destination.visible = source.visible;
	CopyVector( destination.pose.position, source.pose.position );
	CopyQuaternion( destination.pose.orientation, source.pose.orientation );
};

// If GetCurrentPoseIntrinsic() does not get redefined by an inheriting class, just return a nonvisible pose.
bool PoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	pose.visible = false;
	return( pose.visible );
}

bool NullPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	CopyPose( pose.pose, nullPose );
	pose.visible = true;
	pose.time = 0.0;
	return( pose.visible );
}


