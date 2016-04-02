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

PoseTracker::PoseTracker() {
	CopyQuaternion( nullPose.orientation, nullQuaternion );
	CopyVector( nullPose.position, zeroVector );
}
PoseTracker::~PoseTracker () {}

void PoseTracker::Boresight( TrackerPose *pose ) {
	CopyTrackerPose( &nullPose, pose );
	nullPose.orientation[X] *= -1.0;
	nullPose.orientation[Y] *= -1.0;
	nullPose.orientation[Z] *= -1.0;
	ScaleVector( nullPose.position, nullPose.position, -1.0 );
}
bool PoseTracker::BoresightCurrent( void ) {
	TrackerPose pose;
	bool success;
	if ( success = GetCurrentPoseIntrinsic( &pose ) ) Boresight( &pose );
	return success;
}

bool PoseTracker::GetCurrentPosition( Vector3 position ) {
	TrackerPose pose;
	GetCurrentPose( &pose );
	CopyVector( position, pose.position );
	return( pose.visible );
}
bool PoseTracker::GetCurrentOrientation( Quaternion orientation ) {
	TrackerPose pose;
	GetCurrentPose( &pose );
	CopyVector( orientation, pose.orientation );
	return( pose.visible );
}
bool PoseTracker::GetCurrentPose( TrackerPose *pose ) {
	TrackerPose intrinsic_pose;
	GetCurrentPoseIntrinsic( &intrinsic_pose );
	MultiplyQuaternions( pose->orientation,  nullPose.orientation, intrinsic_pose.orientation );
	AddVectors( pose->position, intrinsic_pose.position, nullPose.position );
	pose->visible = intrinsic_pose.visible;
	pose->time = intrinsic_pose.time;
	return( pose->visible );
}

void PoseTracker::CopyTrackerPose( TrackerPose *destination, TrackerPose *source ) {
	destination->time = source->time;
	destination->visible = source->visible;
	CopyVector( destination->position, source->position );
	CopyQuaternion( destination->orientation, source->orientation );
};
bool PoseTracker::GetCurrentPoseIntrinsic( TrackerPose *pose ) {
	return( false );
}

bool NullPoseTracker::GetCurrentPoseIntrinsic( TrackerPose *pose ) {
	CopyVector( pose->position, zeroVector );
	CopyQuaternion( pose->orientation, nullQuaternion );
	pose->visible = true;
	pose->time = 0.0;
	return( pose->visible );
}



