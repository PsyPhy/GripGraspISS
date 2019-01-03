/*********************************************************************************/
/*                                                                               */
/*                               MousePoseTrackers.cpp                           */
/*                                                                               */
/*********************************************************************************/

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

#include "OculusInterface.h"
#include "MousePoseTrackers.h"

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

#include <math.h>

using namespace PsyPhy;

MousePoseTracker::MousePoseTracker( OculusMapper *ptr, double gain ) {
	oculusMapper = ptr;
	this->gain = gain;
	TimerStart( timer );
	PoseTracker();
}

// Define a common method to determine the timestamp for all MousePoseTrackers.
double MousePoseTracker::GetTime( void ) { 
	// Time is the elapsed time since this tracker instance was initialized.
	return( TimerElapsedTime( timer ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MouseRollPoseTracker::MouseRollPoseTracker( OculusMapper *mapper, double gain ) {
	oculusMapper = mapper;
	this->gain = gain;
	CopyVector( eulerAngles, zeroVector );
	MousePoseTracker::MousePoseTracker( mapper, gain );
}

bool MouseRollPoseTracker::Update ( void ) {
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

	// Place the position of this simulated tracker.
	CopyVector( pose.pose.position, zeroVector );
	// When using this tracker in simulation, it is useful to have the ability 
	// to raise and lower the hand.
	static double simulated_hand_height;
	if ( oculusMapper->display->Key['W'] ) simulated_hand_height = 0.0;
	if ( oculusMapper->display->Key['X'] ) simulated_hand_height = -250.0;
	pose.pose.position[Y] = simulated_hand_height;

	// Compute the orientation output based on the mouse position and the results
	// of the various key presses handled in the Update() method.
	Quaternion rollQ, pitchQ, yawQ;
	Quaternion intermediateQ;
	// A feature of the Oculus window on the laptop screen is that you can move the mouse infinitely in any direction.
	// When the cursor gets to the edge of the screen, it wraps around to the other side.
	// Here we use this feature to allow the roll angle to move without a limit.
	SetQuaternion( rollQ, ( oculusMapper->display->mouseCumulativeX - oculusMapper->display->mouseCumulativeY ) * gain, kVector );
	// Add in the pitch and yaw components, computed from eulerAngles[] that were modulated with key presses in Update().
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
	SetQuaternion( pose.pose.orientation, roll_angle * gain, kVector );
	pose.time = GetTime();
	pose.visible = true;
	return pose.visible;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OculusRemoteRollPoseTracker::OculusRemoteRollPoseTracker( OculusMapper *mapper, double gain ) {
	oculusMapper = mapper;
	this->gain = gain;
	CopyVector( eulerAngles, zeroVector );
	MousePoseTracker::MousePoseTracker( mapper, gain );
}

bool OculusRemoteRollPoseTracker::Update ( void ) {

	// Read the state of the Oculus remote buttons and use them to drive the roll angle.
	ovrInputState state;
	ovr_GetInputState(	oculusMapper->session,  ovrControllerType_Remote, &state );
	// Use the disk buttons, which are like a mini joystick on a game pad, to move the roll angle.
	// Left and Right move at a moderate rate. LeftDown and RightDown move faster. LeftUp and RightUp move slower.
	// I am trying to get an intuitive way to change the speed. I don't think, however, that this is a
	// good solution. We should look for another.
	if ( state.Buttons & ovrButton_Left && state.Buttons & ovrButton_Down ) eulerAngles[ROLL] -= gain * 32.0;
	else if ( state.Buttons & ovrButton_Left && state.Buttons & ovrButton_Up ) eulerAngles[ROLL] -= gain;
	else if ( state.Buttons & ovrButton_Right && state.Buttons & ovrButton_Up ) eulerAngles[ROLL] += gain;
	else if ( state.Buttons & ovrButton_Right && state.Buttons & ovrButton_Down ) eulerAngles[ROLL] += gain * 32;
	else if ( state.Buttons & ovrButton_Left ) eulerAngles[ROLL] -= gain * 8.0;
	else if ( state.Buttons & ovrButton_Right ) eulerAngles[ROLL] += gain * 8.0;
	// Limit the roll angle of this tracker to a certain range.
	if ( eulerAngles[ROLL] > ToRadians( 90.0 ) ) eulerAngles[ROLL] = ToRadians( 90.0 );
	if ( eulerAngles[ROLL] < ToRadians( -90.0 ) ) eulerAngles[ROLL] = ToRadians( -90.0 );

	// This is a more-or-less hidden feature. You can change pitch and yaw with the arrow keys on the keyboard.
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

bool OculusRemoteRollPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	CopyVector( pose.pose.position, zeroVector );
	Quaternion rollQ, pitchQ, yawQ;
	Quaternion intermediateQ;

	SetQuaternion( rollQ, eulerAngles[ROLL], kVector );
	SetQuaternion( pitchQ, eulerAngles[PITCH], iVector );
	MultiplyQuaternions( intermediateQ, pitchQ, rollQ );
	SetQuaternion( yawQ, eulerAngles[YAW], jVector );
	MultiplyQuaternions( pose.pose.orientation, yawQ, intermediateQ );
	pose.time = GetTime();
	pose.visible = true;
	return pose.visible;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KeyboardPoseTracker::KeyboardPoseTracker ( OculusMapper *mapper, double position_gain, double rotation_gain ) {
	oculusMapper = mapper;
	positionGain = position_gain;
	rotationGain = rotation_gain;
	CopyVector( currentPose.position, zeroVector );
	CopyQuaternion( currentPose.orientation, nullQuaternion );
	MousePoseTracker::MousePoseTracker( mapper, gain );
}

bool KeyboardPoseTracker::Update ( void ) {
	if ( oculusMapper->display->Key[VK_LEFT] ) currentPose.position[X] -= positionGain;
	if ( oculusMapper->display->Key[VK_RIGHT] ) currentPose.position[X] += positionGain;
	if ( oculusMapper->display->Key[VK_UP] ) currentPose.position[Z] -= positionGain;
	if ( oculusMapper->display->Key[VK_DOWN] ) currentPose.position[Z] += positionGain;
	if ( oculusMapper->display->Key['S'] ) currentPose.position[Y] -= positionGain;
	if ( oculusMapper->display->Key['D'] ) currentPose.position[Y] += positionGain;
	return true;
}

bool KeyboardPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) {
	CopyPose( pose.pose, currentPose );
	pose.time = GetTime();
	pose.visible = true;
	return pose.visible;
}
