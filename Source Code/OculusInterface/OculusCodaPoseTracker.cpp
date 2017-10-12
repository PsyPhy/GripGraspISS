/*********************************************************************************/
/*                                                                               */
/*                              OculusCodaPoseTracker.cpp                        */
/*                                                                               */
/*********************************************************************************/

// A pose tracker that combines the CODA and Oculus instruments.

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

#include "OculusInterface.h"
#include "OculusCodaPoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fOutputDebugString.h"

using namespace PsyPhy;

OculusCodaPoseTracker::OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda ) {

	oculusMapper = mapper;
	absoluteTracker = coda;

	// Decide which data to use from the Oculus as rotation rate.
	useRawSensors = false;

	// This constant deterimines how fast the inertially-computed pose will be driven
	// toward the absolute pose taken from the CODA. A large number means that drift will be corrected slowly.
	InertialWeighting = 10.0;

	// Do what all PoseTrackers need to do.
	PoseTracker::PoseTracker();
}
bool OculusCodaPoseTracker::Initialize( void ) { 
	
	// Only initialize once.
	if ( initialized ) return true;

	// Initialize the connected absolute tracker.
	if ( absoluteTracker ) absoluteTracker->Initialize();

	// Initialize the pose of the tracker.
	// In the final version this should be computed from the CODA contribution.
	// For now, we just start from zero. Even if we leave it this way, it works,
	// because the drift correction will eventually drive the current state to the CODA pose.
	CopyPose( currentState.pose, nullPose );

	// Need to have a reference time to compute the first delta in Update();
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();
	currentState.time = ovr_GetTimeInSeconds();

	// This pose tracker is always visible.
	currentState.visible = true;

	return true; 
}

double OculusCodaPoseTracker::GetRotationRate( Vector3 rate ) {

	if ( useRawSensors ) {

		// Retrieve the sensor data from the Oculus.
		ovrSensorData rawSensorData = oculusMapper->ReadSensorData();

		// Head pose angular velocities, as computed by OVR, are taken as the rotation rates.
		// This may or may not be true, depending on whether OVR uses gravity when computing the
		// angular velocities. In an alternate version we will read the sensors directly.
		rate[X] = rawSensorData.Gyro.x;
		rate[Y] = rawSensorData.Gyro.y; 
		rate[Z] = rawSensorData.Gyro.z;
	
		return( rawSensorData.TimeInSeconds );

	}
	else {

		// Retrieve the sensor data from the Oculus.
		ovrTrackingState sensorState = oculusMapper->ReadTrackingState();

		// Head pose angular velocities, as computed by OVR, are taken as the rotation rates.
		// This may or may not be true, depending on whether OVR uses gravity when computing the
		// angular velocities. In an alternate version we will read the sensors directly.
		rate[X] = sensorState.HeadPose.AngularVelocity.x;
		rate[Y] = sensorState.HeadPose.AngularVelocity.y; 
		rate[Z] = sensorState.HeadPose.AngularVelocity.z;
	
		return( sensorState.HeadPose.TimeInSeconds );
	}
}


bool OculusCodaPoseTracker::Update( void ) { 

	static int cycle_counter = 0;
	
	// Compute how much time since the last inertial update.
	Vector3 rate;
	double time = GetRotationRate( rate );
	double delta_time = time - currentState.time;

	// Compute a finite step rotation from gyro measurements and elapsed time.
	// I use here a full quaternion to compute the finite rotation. There are 
	// other linearization methods that can be used, but I don't see an advantage to using them here.
	//Vector3 rate = { rawSensorData.Gyro.x, rawSensorData.Gyro.y, rawSensorData.Gyro.z };

	Vector3 delta;
	ScaleVector( delta, rate, delta_time / 2.0 );
	Quaternion dQ = { delta[0], delta[1], delta[2], 0.0 };
	dQ[M] = sqrt( 1.0 - delta[X] * delta[X] - delta[Y] * delta[Y]- delta[Z] * delta[Z] );

	// Make one integration step using the computed finite rotation.
	Quaternion newQ;
	MultiplyQuaternions( newQ, currentState.pose.orientation, dQ );
	CopyQuaternion( currentState.pose.orientation, newQ );

	// Read the absolute pose from the CODA tracker (or some other tracker) and take a step towards it.
	// The step is achieved by taking a weighted average between the current inertial state and the CODA state.
	// More weight is given to the inertial state to make it responsive to rapid movement.
	TrackerPose absolutePose;
	if ( absoluteTracker ) {
		absoluteTracker->Update();
		absoluteTracker->GetCurrentPose( absolutePose );
		if ( absolutePose.visible ) {
			// fOutputDebugString( "Cycle %4d: %s  %s  %s\n", cycle_counter, qstr( dQ ), qstr( currentState.pose.orientation ), qstr( absolutePose.pose.orientation ) );
			for ( int i = 0; i < 4; i++ ) currentState.pose.orientation[i] = InertialWeighting * currentState.pose.orientation[i] + absolutePose.pose.orientation[i];
			CopyVector( currentState.pose.position, absolutePose.pose.position );
			//CopyVector( currentState.pose.position, zeroVector );
		}
//		else fOutputDebugString( "Cycle %4d: No absolute update.\n", cycle_counter );
	}
	//else fOutputDebugString( "Cycle %4d: No absolute tracker.\n", cycle_counter );
	cycle_counter++;

	// After the above, we have a weighted *SUM* of inertial and CODA data. This normalization effectively turns the
	// sum into an approximate weighted *AVERAGE*. Note that we are algebraically averaging the quaternion as a vector.
	// This is approximately good for orientations that are close together. Formally, we should use slerp if we want
	// a true weighted average in rotation space with constant rotation velocity, but that is not so important here
	// and the formulas here are much simpler.
	NormalizeQuaternion( currentState.pose.orientation );

	// The time of this pose is the time of the sample of the intertial data.
	currentState.time = time;
	// This tracker is presumed to always be visible.
	currentState.visible = true;

	return true; 

}
bool OculusCodaPoseTracker::Release( void ) { 
	if ( absoluteTracker ) absoluteTracker->Release();
	initialized = false;
	return true; 
}

bool OculusCodaPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ) { 
	CopyTrackerPose( pose, currentState );
	return pose.visible; 
}


