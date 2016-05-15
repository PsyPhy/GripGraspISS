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

bool OculusPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	
	ovrPosef ovrHeadPose = oculusMapper->ReadHeadPose();

	pose.pose.position[X] = ovrHeadPose.Position.x;
	pose.pose.position[Y] = ovrHeadPose.Position.y;
	pose.pose.position[Z] = ovrHeadPose.Position.z;
	pose.pose.orientation[X] = ovrHeadPose.Orientation.x;
	pose.pose.orientation[Y] = ovrHeadPose.Orientation.y;
	pose.pose.orientation[Z] = ovrHeadPose.Orientation.z;
	pose.pose.orientation[M] = ovrHeadPose.Orientation.w;
	// Timestamp the sample.

	// We assume that the pose is always obtained for this tracker.
	pose.visible = true;

	return pose.visible; 

}


