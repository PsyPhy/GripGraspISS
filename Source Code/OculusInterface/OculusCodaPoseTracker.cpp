/*********************************************************************************/
/*                                                                               */
/*                              OculusCodaPoseTracker.cpp                        */
/*                                                                               */
/*********************************************************************************/

// A pose tracker that combines the CODA and Oculus instruments.

#include "OculusInterface.h"
#include "OculusCodaPoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fOutputDebugString.h"

using namespace PsyPhy;

OculusCodaPoseTracker::OculusCodaPoseTracker( OculusMapper *mapper, CodaPoseTracker *coda1, CodaPoseTracker *coda2 ) {
	oculusMapper = mapper;
	primaryCoda = coda1;
	secondaryCoda = coda2;
	PoseTracker::PoseTracker();
	// This constant deterimines how fast the inertially-computed pose will be driven
	// toward the absolute pose taken from the CODA.
	InertialWeighting = 10.0;
}
bool OculusCodaPoseTracker::Initialize( void ) { 
	
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();

	// Initialize the pose of the tracker.
	// In the final version this should be computed from the CODA contribution.
	// For now, we just start from zero.
	CopyVector( currentState.position, zeroVector );
	CopyQuaternion( currentState.orientation, nullQuaternion );

	// Need to have a reference time to compute the first delta in Update();
	currentState.time = oculusMapper->sensorSampleTime;

	// This pose tracker is always visible.
	currentState.visible = true;

	return true; 
}
bool OculusCodaPoseTracker::Update( void ) { 
	
	static int cycle_counter = 0;
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();
	ovrPosef headPose = sensorState.HeadPose.ThePose;
	double time = oculusMapper->sensorSampleTime;
	double delta_time = time - currentState.time;

	Vector3 rate = { sensorState.RawSensorData.Gyro.x, sensorState.RawSensorData.Gyro.y, sensorState.RawSensorData.Gyro.z };
	Vector3 delta;
	ScaleVector( delta, rate, delta_time / 2.0 );
	Quaternion dQ = { delta[0], delta[1], delta[2], 0.0 };
	dQ[M] = sqrt( 1.0 - delta[X] * delta[X] - delta[Y] * delta[Y]- delta[Z] * delta[Z] );
	Quaternion newQ;
	MultiplyQuaternions( newQ, currentState.orientation, dQ );
	CopyQuaternion( currentState.orientation, newQ );

	// Read the absolute pose from the CODA tracker and take a step towards it.
	// The step is achieved by taking a weighted average between the current inertial state and the CODA state.
	// More weight is given to the inertial state to make it responsive to rapid movement.
	TrackerPose codaPose;
	if ( primaryCoda && primaryCoda->GetCurrentPose( &codaPose ) ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = InertialWeighting * currentState.orientation[i] + codaPose.orientation[i];
		// fOutputDebugString( "Update from Primary.\n" );
	}
	else if ( secondaryCoda && secondaryCoda->GetCurrentPose( &codaPose ) ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = InertialWeighting * currentState.orientation[i] + codaPose.orientation[i];
		fOutputDebugString( "Update from Secondary.\n" );
	}
	else fOutputDebugString( "No CODA update.\n" );

	NormalizeQuaternion( currentState.orientation );
	currentState.time = time;
	currentState.visible = true;

	if ( !(cycle_counter++ % 120) ) fOutputDebugString( "%s %s\n", qstr( currentState.orientation ), vstr( currentState.position ) );

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


