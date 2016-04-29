/*********************************************************************************/
/*                                                                               */
/*                                DualCodaPoseTracker.cpp                        */
/*                                                                               */
/*********************************************************************************/

// A pose tracker that combines the CODA and Oculus instruments.

#include "stdafx.h"

#include "DualCodaPoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fOutputDebugString.h"

using namespace PsyPhy;

DualCodaPoseTracker::DualCodaPoseTracker( CodaPoseTracker *coda1, CodaPoseTracker *coda2 ) {
	primaryCoda = coda1;
	secondaryCoda = coda2;
	PoseTracker::PoseTracker();
}

bool DualCodaPoseTracker::Initialize( void ) { 
	
	// Open a file to output some debugging info.
	char *filename = "DualCodaPoseTracker.pse";
	fp = fopen( filename, "w" );
	if ( !fp ) {
		fOutputDebugString( "Error opening %s for writing.\n", filename );
		exit( -1 );
	}

	return true; 
}
bool DualCodaPoseTracker::Update( void ) { 

	static int cycle_counter = 0;
	
	// Read the absolute pose from the CODA tracker and take a step towards it.
	// The step is achieved by taking a weighted average between the current inertial state and the CODA state.
	// More weight is given to the inertial state to make it responsive to rapid movement.
	// The preference is to compute the orientation with data from a single coda. We call one of them the primary
	// unit and use it if available. If not, we try the second unit.
	TrackerPose codaPose1, codaPose2;
	if ( primaryCoda ) primaryCoda->GetCurrentPose( &codaPose1 );
	if ( secondaryCoda ) secondaryCoda->GetCurrentPose( &codaPose2 );
	if ( codaPose1.visible ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = codaPose1.orientation[i];
		currentState.visible = true;
		// fOutputDebugString( "Cycle %4d: Update from Primary.\n", cycle_counter );
	}
	else if ( codaPose2.visible ) {
		for ( int i = 0; i < 4; i++ ) currentState.orientation[i] = codaPose2.orientation[i];
		currentState.visible = true;
		fOutputDebugString( "Cycle %4d: Update from Secondary.\n", cycle_counter );
	}
	else {
		currentState.visible = false;
		fOutputDebugString( "Cycle %4d: No CODA update.\n", cycle_counter );
	}
	cycle_counter++;

	NormalizeQuaternion( currentState.orientation );
	currentState.time = 0.0
		;
	
	// Output to a file for debugging.
	fprintf( fp, "%s %s | %3d %7.3lf %s | %3d %7.3lf %s | %7.3lf %s\n", 
		(int) codaPose1.visible, qstr( codaPose1.orientation ), 
		(int) codaPose2.visible, qstr( codaPose2.orientation ), 
		currentState.time, qstr( currentState.orientation ) );
	return currentState.visible; 

}
bool DualCodaPoseTracker::Quit( void ) { 
	fclose( fp );
	return true; 
}

bool DualCodaPoseTracker::GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose ) { 
	
	CopyVector( pose->position, currentState.position );
	CopyQuaternion( pose->orientation, currentState.orientation );
	pose->time = currentState.time;
	pose->visible = currentState.visible;

	return currentState.visible; 

}


