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
#include "../Useful/fMessageBox.h"
#include "../Useful/ini.h"

using namespace PsyPhy;

OculusCodaPoseTracker::OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda, const char *ini_filename ) {

	oculusMapper = mapper;
	absoluteTracker = coda;

	// Decide which data to use from the Oculus as rotation rate.
	useRawSensors = true;

	// A small value to react quickly to the first CODA pose.
	precaptureInertialWeighting = 10.0;
	// The value used when data from the CODA are readily available.
	// The value is large to filter artifacts from the computed CODA pose.
	postcaptureInertialWeighting = 100.0;
	// Time constant to determine how to adapt the weight.
	occlusionWeightingTimeConstant = 0.1;

	// Do what all PoseTrackers need to do.
	PoseTracker::PoseTracker();

	if ( ini_filename ) {
		fOutputDebugString( "Parsing %s for OculusCodaPoseTracker.\n", ini_filename );
		int error = ini_parse( ini_filename, iniHandler, this );
		if ( error != 0 ) fAbortMessage( "OculusCodaPoseTracker","Parsing error %d for file %s.\n", error, ini_filename );
	}

}
bool OculusCodaPoseTracker::Initialize( void ) { 
	
	// Regardless of whether the the tracker has already been intilized,
	// reset to the initial weighting of the inertial inputs. This allows
	// one to anticipate large discrepancies between the CODA and inertial 
	// poses and to react quickly to a new CODA pose.
	InertialWeighting = precaptureInertialWeighting;

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
	// In general, more weight is given to the inertial state to make it responsive to rapid movement and
	// to filter artifacts in the CODA pose. But initially the CODA pose receives high weight to quickly initialize
	// the output pose to be aligned with the CODA pose.
	TrackerPose absolutePose;
	if ( absoluteTracker ) {
		absoluteTracker->Update();
		absoluteTracker->GetCurrentPose( absolutePose );
		if ( absolutePose.visible ) {
			// fOutputDebugString( "Cycle %4d: %s  %s  %s\n", cycle_counter, qstr( dQ ), qstr( currentState.pose.orientation ), qstr( absolutePose.pose.orientation ) );
			for ( int i = 0; i < 4; i++ ) currentState.pose.orientation[i] = InertialWeighting * currentState.pose.orientation[i] + absolutePose.pose.orientation[i];
			CopyVector( currentState.pose.position, absolutePose.pose.position );
			// Once we have updated once toward a pose from the absolute tracker, subsequent steps
			// will use a larger and larger inertial weigthing factor to slow down the action of
			// optical data.
			InertialWeighting += ( postcaptureInertialWeighting - InertialWeighting ) * occlusionWeightingTimeConstant;
			fOutputDebugString( "Cycle %4d: %f Step toward Absolute.\n", cycle_counter, InertialWeighting );
		}
		else {
			// The longer the absolute tracker data has been missing, the more rapidly we want to
			// move to the absolute position when it comes back, to avoid a slow convergence to the
			// absolute data that is noticeable in the VR.
			InertialWeighting += ( precaptureInertialWeighting - InertialWeighting ) * occlusionWeightingTimeConstant;
			fOutputDebugString( "Cycle %4d: %f No absolute update.\n", cycle_counter, InertialWeighting );
		}
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


