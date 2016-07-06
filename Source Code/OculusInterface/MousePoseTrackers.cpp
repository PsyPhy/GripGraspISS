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

	// Read the state of the Oculus remote buttons and use them to drive the roll angle as well.
	ovrInputState state;
	ovr_GetInputState(	oculusMapper->session,  ovrControllerType_Remote, &state );
	if ( state.Buttons & ovrButton_Left ) eulerAngles[ROLL] -= gain * 8.0;
	if ( state.Buttons & ovrButton_Down ) eulerAngles[ROLL] -= gain;
	if ( state.Buttons & ovrButton_Right ) eulerAngles[ROLL] += gain * 8.0;
	if ( state.Buttons & ovrButton_Up ) eulerAngles[ROLL] += gain;

	// Here we check the state of the key and move one gain step per cycle.
	// If framerate is constant, velocity should be constant.
	// An alternative approach would be to move according to the number of key down
	// events since the last cycle. That would allow for finer control and benefit from the
	// accelerating nature of events when you hold down a key.
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

	SetQuaternion( rollQ, ( oculusMapper->display->mouseCumulativeX - oculusMapper->display->mouseCumulativeY ) * gain + eulerAngles[ROLL], kVector );
	SetQuaternion( pitchQ, eulerAngles[PITCH], iVector );
	MultiplyQuaternions( intermediateQ, pitchQ, rollQ );
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