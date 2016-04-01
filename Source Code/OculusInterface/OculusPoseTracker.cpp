/*********************************************************************************/
/*                                                                               */
/*                              OculusPoseTracker.cpp                            */
/*                                                                               */
/*********************************************************************************/

// Access the Oculus head tracker in the PsyPhy domain.

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

bool OculusPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose ) { 
	
	ovrPosef ovrHeadPose = oculusMapper->ReadHeadPose();

	pose->position[X] = ovrHeadPose.Position.x;
	pose->position[Y] = ovrHeadPose.Position.y;
	pose->position[Z] = ovrHeadPose.Position.z;
	pose->orientation[X] = ovrHeadPose.Orientation.x;
	pose->orientation[Y] = ovrHeadPose.Orientation.y;
	pose->orientation[Z] = ovrHeadPose.Orientation.z;
	pose->orientation[M] = ovrHeadPose.Orientation.w;
	// We assume that the pose is obtained for this tracker.
	pose->visible = true;

	return true; 

}


