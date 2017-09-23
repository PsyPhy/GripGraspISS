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
	
	ovrPosef ovrHeadPose = oculusMapper->ReadHeadPose();

	pose.pose.position[X] = ovrHeadPose.Position.x * 1000.0;
	pose.pose.position[Y] = ovrHeadPose.Position.y * 1000.0;
	pose.pose.position[Z] = ovrHeadPose.Position.z * 1000.0;
	pose.pose.orientation[X] = ovrHeadPose.Orientation.x;
	pose.pose.orientation[Y] = ovrHeadPose.Orientation.y;
	pose.pose.orientation[Z] = ovrHeadPose.Orientation.z;
	pose.pose.orientation[M] = ovrHeadPose.Orientation.w;
	// Oculus seems to track the center of the head, while we have been tracking
	// the centroid of the marker array. So I shift the position forward.
	Vector3 shift;
	RotateVector( shift, pose.pose.orientation, kVector );
	ScaleVector( shift, shift, -100.0 );
	AddVectors( pose.pose.position, pose.pose.position, shift );
	// Timestamp the sample.
	pose.time = oculusMapper->sensorSampleTime;

	// We assume that the pose is always obtained for this tracker.
	pose.visible = true;

	return pose.visible; 

}


OculusRightHandPoseTracker::OculusRightHandPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	OculusPoseTracker();
}

bool OculusRightHandPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	
	ovrPosef ovrHandPose = oculusMapper->ReadHandPose( ovrHand_Right );

	pose.pose.position[X] = ovrHandPose.Position.x * 1000.0;
	pose.pose.position[Y] = ovrHandPose.Position.y * 1000.0;
	pose.pose.position[Z] = ovrHandPose.Position.z * 1000.0;
	pose.pose.orientation[X] = ovrHandPose.Orientation.x;
	pose.pose.orientation[Y] = ovrHandPose.Orientation.y;
	pose.pose.orientation[Z] = ovrHandPose.Orientation.z;
	pose.pose.orientation[M] = ovrHandPose.Orientation.w;

	// Timestamp the sample.
	pose.time = oculusMapper->sensorSampleTime;
	pose.visible =  (oculusMapper->handStatusFlags[ ovrHand_Right ] & ovrStatus_OrientationTracked ) && (oculusMapper->handStatusFlags[ ovrHand_Right ] & ovrStatus_PositionTracked );
	return( pose.visible );
}

OculusLeftHandPoseTracker::OculusLeftHandPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	OculusPoseTracker();
}

bool OculusLeftHandPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	
	ovrPosef ovrHandPose = oculusMapper->ReadHandPose( ovrHand_Left );

	pose.pose.position[X] = ovrHandPose.Position.x * 1000.0;
	pose.pose.position[Y] = ovrHandPose.Position.y * 1000.0;
	pose.pose.position[Z] = ovrHandPose.Position.z * 1000.0;
	pose.pose.orientation[X] = ovrHandPose.Orientation.x;
	pose.pose.orientation[Y] = ovrHandPose.Orientation.y;
	pose.pose.orientation[Z] = ovrHandPose.Orientation.z;
	pose.pose.orientation[M] = ovrHandPose.Orientation.w;
	// Timestamp the sample.
	pose.time = oculusMapper->sensorSampleTime;
	pose.visible =  (oculusMapper->handStatusFlags[ ovrHand_Left ] & ovrStatus_OrientationTracked ) && (oculusMapper->handStatusFlags[ ovrHand_Left ] & ovrStatus_PositionTracked );
	return pose.visible; 

}