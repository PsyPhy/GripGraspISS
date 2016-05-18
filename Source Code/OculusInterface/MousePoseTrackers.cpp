/*********************************************************************************/
/*                                                                               */
/*                               MousePoseTrackers.cpp                           */
/*                                                                               */
/*********************************************************************************/

#include "OculusInterface.h"
#include "MousePoseTrackers.h"

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

using namespace PsyPhy;

MousePoseTracker::MousePoseTracker( OculusMapper *ptr, double gain ) {
	oculusMapper = ptr;
	this->gain = gain;
	PoseTracker();
}
bool MousePoseTracker::Update( void ) { return true; }
bool MousePoseTracker::Quit( void ) { return true; }

// Define a common method to determine the timestamp for all MousePoseTrackers.
double MousePoseTracker::GetTime( void ) { 
	// We shoud set the time of the sample with respect to some clock common to the other trackers.
	// I don't know what that will be, yet, so here I set the time to zero.
	return 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MouseRollPoseTracker::MouseRollPoseTracker( OculusMapper *mapper, double gain ) {
	oculusMapper = mapper;
	this->gain = gain;
	CopyVector( eulerAngles, zeroVector );
	MousePoseTracker::MousePoseTracker( mapper, gain );
}

bool MouseRollPoseTracker::Update ( void ) {
	if ( oculusMapper->display->Key[VK_LEFT] ) eulerAngles[YAW] -= gain;
	if ( oculusMapper->display->Key[VK_RIGHT] ) eulerAngles[YAW] += gain;
	if ( oculusMapper->display->Key[VK_UP] ) eulerAngles[PITCH] -= gain;
	if ( oculusMapper->display->Key[VK_DOWN] ) eulerAngles[PITCH] += gain;
	return true;
}

bool MouseRollPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	CopyVector( pose.pose.position, zeroVector );
	Quaternion rollQ, pitchQ, yawQ;
	Quaternion intermediateQ;

	SetQuaternion( rollQ, ( oculusMapper->display->mouseCumulativeX - oculusMapper->display->mouseCumulativeY ) * gain, kVector );
	SetQuaternion( pitchQ, eulerAngles[PITCH], iVector );
	MultiplyQuaternions( intermediateQ, rollQ, pitchQ );
	SetQuaternion( yawQ, eulerAngles[YAW], jVector );
	MultiplyQuaternions( pose.pose.orientation, yawQ, intermediateQ );
	pose.time = GetTime();
	pose.visible = true;
	return pose.visible;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ArrowsRollPoseTracker::ArrowsRollPoseTracker ( OculusMapper *mapper, double gain ) {
	oculusMapper = mapper;
	this->gain = gain;
	roll_angle = 0.0;
	MousePoseTracker::MousePoseTracker( mapper, gain );
}

bool ArrowsRollPoseTracker::Update ( void ) {
	if ( oculusMapper->display->Key[VK_LEFT] ) roll_angle += gain;
	if ( oculusMapper->display->Key[VK_RIGHT] ) roll_angle -= gain;
	return true;
}

bool ArrowsRollPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	CopyVector( pose.pose.position, zeroVector );
	SetQuaternion( pose.pose.orientation, ( oculusMapper->display->mouseCumulativeX - oculusMapper->display->mouseCumulativeY ) * gain, kVector );
	pose.time = GetTime();
	pose.visible = true;
	return pose.visible;
}