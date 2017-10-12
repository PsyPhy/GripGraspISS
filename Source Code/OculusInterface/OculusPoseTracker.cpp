/*********************************************************************************/
/*                                                                               */
/*                              OculusPoseTracker.cpp                            */
/*                                                                               */
/*********************************************************************************/

// Access the Oculus head tracker in the PsyPhy domain. We simply take whatever
//  pose the Oculus has computed using whatever means it usually uses. It will 
//  presumably use the compass and gravity to cancel drift in the inertial 
//  orientation measurements. If the Oculus tracking camera is not connected, then
//  the position component of the pose stays at 0.

#include "OculusInterface.h"
#include "OculusPoseTracker.h"

#include "../Useful//Useful.h"

using namespace PsyPhy;

OculusPoseTracker::OculusPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	PoseTracker::PoseTracker();
}
bool OculusPoseTracker::Initialize( void ) { return true; }
bool OculusPoseTracker::Update( void ) { return true; }
bool OculusPoseTracker::Quit( void ) { return true; }

OculusHMDPoseTracker::OculusHMDPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	OculusPoseTracker();
}

bool OculusHMDPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	
	ovrPoseStatef ovrHeadPose = oculusMapper->ReadHeadPose();

	pose.pose.position[X] = ovrHeadPose.ThePose.Position.x * 1000.0;
	pose.pose.position[Y] = ovrHeadPose.ThePose.Position.y * 1000.0;
	pose.pose.position[Z] = ovrHeadPose.ThePose.Position.z * 1000.0;
	pose.pose.orientation[X] = ovrHeadPose.ThePose.Orientation.x;
	pose.pose.orientation[Y] = ovrHeadPose.ThePose.Orientation.y;
	pose.pose.orientation[Z] = ovrHeadPose.ThePose.Orientation.z;
	pose.pose.orientation[M] = ovrHeadPose.ThePose.Orientation.w;
	pose.time = ovrHeadPose.TimeInSeconds;

	// Oculus seems to track the center of the head, while we have been tracking
	// the centroid of the marker array. So I shift the position forward.
	Vector3 shift;
	RotateVector( shift, pose.pose.orientation, kVector );
	ScaleVector( shift, shift, -100.0 );
	AddVectors( pose.pose.position, pose.pose.position, shift );

	// We assume that the pose is always obtained for this tracker.
	pose.visible = true;

	return pose.visible; 

}


OculusHandPoseTracker::OculusHandPoseTracker( ovrHandType hand, OculusMapper *mapper ) {
	this->hand = hand;
	oculusMapper = mapper;
}

bool OculusHandPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	
	unsigned int flags;
	ovrPoseStatef handPose = oculusMapper->ReadHandPose( hand, &flags );

	pose.pose.position[X] = handPose.ThePose.Position.x * 1000.0;
	pose.pose.position[Y] = handPose.ThePose.Position.y * 1000.0;
	pose.pose.position[Z] = handPose.ThePose.Position.z * 1000.0;
	pose.pose.orientation[X] = handPose.ThePose.Orientation.x;
	pose.pose.orientation[Y] = handPose.ThePose.Orientation.y;
	pose.pose.orientation[Z] = handPose.ThePose.Orientation.z;
	pose.pose.orientation[M] = handPose.ThePose.Orientation.w;
	pose.time = handPose.TimeInSeconds;

	pose.visible = ( flags & ovrStatus_OrientationTracked ) && ( flags & ovrStatus_PositionTracked );
	return( pose.visible );
}

OculusLeftHandPoseTracker::OculusLeftHandPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	hand = ovrHand_Left;
}

OculusRightHandPoseTracker::OculusRightHandPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	hand = ovrHand_Right;
}
