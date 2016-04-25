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
	// toward the absolute pose taken from the CODA. A large number means that drift will be corrected slowly.
	InertialWeighting = 10.0;
}
bool OculusCodaPoseTracker::Initialize( void ) { 
	
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();

	// Initialize the pose of the tracker.
	// In the final version this should be computed from the CODA contribution.
	// For now, we just start from zero. Even if we leave it this way, it works,
	// because the drift correction will eventually drive the current state to the CODA pose.
	CopyVector( currentState.position, zeroVector );
	CopyQuaternion( currentState.orientation, nullQuaternion );

	// Need to have a reference time to compute the first delta in Update();
	currentState.time = oculusMapper->sensorSampleTime;

	// This pose tracker is always visible.
	currentState.visible = true;

	// Open a file to output some debugging info.
	char *filename = "OculusCodaPoseTracker.pse";
	fp = fopen( filename, "w" );
	if ( !fp ) {
		fOutputDebugString( "Error opening %s for writing.\n", filename );
		exit( -1 );
	}

	return true; 
}
bool OculusCodaPoseTracker::Update( void ) { 

	static int cycle_counter = 0;
	
	// Retrieve the sensor data from the Oculus.
	ovrTrackingState sensorState = oculusMapper->ReadTrackingState();
	ovrPosef headPose = sensorState.HeadPose.ThePose;

	// Compute how much time since the last inertial update.
	double time = oculusMapper->sensorSampleTime;
	double delta_time = time - currentState.time;

	// Compute a finite step rotation from gyro measurements and elapsed time.
	// I use here a full quaternion to compute the finite rotation. There are 
	// other linearization methods that can be used, but I don't see an advantage to using them here.
	Vector3 rate = { sensorState.RawSensorData.Gyro.x, sensorState.RawSensorData.Gyro.y, sensorState.RawSensorData.Gyro.z };
	Vector3 delta;
	ScaleVector( delta, rate, delta_time / 2.0 );
	Quaternion dQ = { delta[0], delta[1], delta[2], 0.0 };
	dQ[M] = sqrt( 1.0 - delta[X] * delta[X] - delta[Y] * delta[Y]- delta[Z] * delta[Z] );

	// Make one integration step using the computed finite rotation.
	Quaternion newQ;
	MultiplyQuaternions( newQ, currentState.orientation, dQ );
	CopyQuaternion( currentState.orientation, newQ );

	// Read the absolute pose from the CODA tracker and take a step towards it.
	// The step is achieved by taking a weighted average between the current inertial state and the CODA state.
	// More weight is given to the inertial state to make it responsive to rapid movement.
	// The preference is to compute the orientation with data from a single coda. We call one of them the primary
	// unit and use it if available. If not, we try the second unit.
	TrackerPose codaPose1, codaPose2;
	if ( primaryCoda ) primaryCoda->GetCurrentPose( &codaPose1 );
	if ( secondaryCoda ) secondaryCoda->GetCurrentPose( &codaPose2 );
	if ( codaPose1.visible ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = InertialWeighting * currentState.orientation[i] + codaPose1.orientation[i];
		// fOutputDebugString( "Cycle %4d: Update from Primary.\n", cycle_counter );
	}
	else if ( codaPose2.visible ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = InertialWeighting * currentState.orientation[i] + codaPose2.orientation[i];
		fOutputDebugString( "Cycle %4d: Update from Secondary.\n", cycle_counter );
	}
	else fOutputDebugString( "Cycle %4d: No CODA update.\n", cycle_counter );
	cycle_counter++;

	// After the above, we have a weighted *SUM* of inertial and CODA data. This normalization effectively turns the
	// sum into an approximate weighted *AVERAGE*. Note that we are algebraically averaging the quaternion as a vector.
	// This is approximately good for orientations that are close together. Formally, we should use slerp if we want
	// a true weighted average in rotation space with constant rotation velocity, but that is not so important here
	// and the formulas here are much simpler.
	NormalizeQuaternion( currentState.orientation );
	currentState.time = time;
	currentState.visible = true;

	// Output to a file for debugging.
	fprintf( fp, "%s %s | %3d %7.3lf %s | %3d %7.3lf %s | %7.3lf %s\n", 
		qstr( dQ ), qstr( newQ ), 
		(int) codaPose1.visible, codaPose1.time, qstr( codaPose1.orientation ), 
		(int) codaPose2.visible, codaPose2.time, qstr( codaPose2.orientation ), 
		currentState.time, qstr( currentState.orientation ) 
		);
	return true; 

}
bool OculusCodaPoseTracker::Quit( void ) { 
	fclose( fp );
	return true; 
}

bool OculusCodaPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose ) { 
	
	CopyVector( pose->position, currentState.position );
	CopyQuaternion( pose->orientation, currentState.orientation );
	pose->time = currentState.time;
	// We assume that the pose is always available for this tracker.
	pose->visible = true;

	return true; 

}


