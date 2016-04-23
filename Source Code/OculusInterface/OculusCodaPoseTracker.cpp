/*********************************************************************************/
/*                                                                               */
/*                              OculusCodaPoseTracker.cpp                            */
/*                                                                               */
/*********************************************************************************/

// A pose tracker that combines the CODA and Oculus instruments.

#include "OculusInterface.h"
#include "OculusCodaPoseTracker.h"
#include "../Useful/Useful.h"

using namespace PsyPhy;

OculusCodaPoseTracker::OculusCodaPoseTracker( OculusMapper *mapper ) {
	oculusMapper = mapper;
	PoseTracker::PoseTracker();
}
bool OculusCodaPoseTracker::Initialize( void ) { 
	
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();

	ovrPosef headPose = sensorState.HeadPose.ThePose;

	currentState.position[X] = headPose.Position.x;
	currentState.position[Y] = headPose.Position.y;
	currentState.position[Z] = headPose.Position.z;
	currentState.orientation[X] = headPose.Orientation.x;
	currentState.orientation[Y] = headPose.Orientation.y;
	currentState.orientation[Z] = headPose.Orientation.z;
	currentState.orientation[M] = headPose.Orientation.w;
	currentState.time = oculusMapper->sensorSampleTime;
	currentState.visible = true;

	return true; 
}
bool OculusCodaPoseTracker::Update( void ) { 
	
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();
	ovrPosef headPose = sensorState.HeadPose.ThePose;
	double time = oculusMapper->sensorSampleTime;
	double delta_time = time - currentState.time;

	Vector3 rate = { sensorState.RawSensorData.Gyro.x, sensorState.RawSensorData.Gyro.y, sensorState.RawSensorData.Gyro.z };
	TrackerPose P;
	Quaternion S;
	GetCurrentPose( &P );
	CopyQuaternion( S, P.orientation );
	//MultiplyQuaternions( S, nullPose.orientation, S );
/*
Quaternion	dQ = {
		  rate[X] * S[Y] + rate[Y] * S[Z] + rate[Z] * S[M],
		- rate[X] * S[X] - rate[Z] * S[Z] + rate[Y] * S[M],
		- rate[Y] * S[X] + rate[Z] * S[Y] - rate[X] * S[M],
		- rate[Z] * S[X] - rate[Y] * S[Y] + rate[X] * S[Z] 
	};
	*/
	Quaternion	dQ = {
		  rate[Z] * S[Y] + rate[Y] * S[Z] + rate[X] * S[M],
		  rate[Z] * S[X] - rate[X] * S[Z] + rate[Y] * S[M],
		- rate[Y] * S[X] + rate[X] * S[Y] + rate[Z] * S[M],
		- rate[X] * S[X] - rate[Y] * S[Y] - rate[Z] * S[Z] 
	};


		/*
				double omega_q[4][4]= {{0,			OVRgyro.x,			OVRgyro.y,			OVRgyro.z },
								   {-OVRgyro.x,	0,					-OVRgyro.z,			OVRgyro.y },
								   {-OVRgyro.y,	OVRgyro.z,			0,					-OVRgyro.x},		
								   {-OVRgyro.z,	-OVRgyro.y,			OVRgyro.x,			0		  }};

	*/
	
	currentState.orientation[X] += (dQ[X] * delta_time );
	currentState.orientation[Y] += (dQ[Y] * delta_time );
	currentState.orientation[Z] += (dQ[Z] * delta_time );
	currentState.orientation[M] += (dQ[M] * delta_time );
	
	NormalizeQuaternion( currentState.orientation );
	currentState.time = time;
	currentState.visible = true;

	return true; 

}
bool OculusCodaPoseTracker::Quit( void ) { return true; }

bool OculusCodaPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose ) { 
	
	CopyVector( pose->position, currentState.position );
	CopyQuaternion( pose->orientation, currentState.orientation );
	pose->time = currentState.time;
	// We assume that the pose is obtained for this tracker.
	pose->visible = true;

	return true; 

}


