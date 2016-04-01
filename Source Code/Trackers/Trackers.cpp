/*********************************************************************************/
/*                                                                               */
/*                                    Trackers.cpp                               */
/*                                                                               */
/*********************************************************************************/

// Base class for a 3D/6D Tracker.

#include "stdafx.h"
#include "Trackers.h"

using namespace PsyPhy;

/***************************************************************************/

void Tracker::Initialize( void ) {}
int Tracker::Update( void ) {
	return( 0 );
}
void Tracker::Quit() {}

double Tracker::GetSamplePeriod( void ) {
	return( samplePeriod );
}
int Tracker::GetNumberOfUnits( void ) {
	return( nUnits );
}

void Tracker::StartAcquisition( float max_duration ) {}
void Tracker::StopAcquisition( void ) {}
bool Tracker::GetAcquisitionState( void ) {
	return( false );
}
bool Tracker::CheckAcquisitionOverrun( void ) {
	return( false );
}

bool  Tracker::GetCurrentMarkerFrame( MarkerFrame &frame ) { 
	return( false );
}
bool  Tracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit ) { 
	// If the tracker has no concept of separate units, just get the data from the default unit.
	return( GetCurrentMarkerFrame( frame ) );
}

int  Tracker::RetrieveMarkerFrames( MarkerFrame frames[], int max_frames ) { 
	return( 0 );
}
int  Tracker::RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit ) { 
	// If the tracker has no concept of separate units, just get the data from the default unit.
	return( RetrieveMarkerFrames( frames, max_frames ) );
}


/**************************************************************************************/

void Tracker::CopyMarkerFrame( MarkerFrame &destination, MarkerFrame &source ) {
	int mrk;
	destination.time = source.time;
	for ( mrk = 0; mrk < nMarkers; mrk++ ) {
		destination.marker[mrk].visibility = source.marker[mrk].visibility;
		CopyVector( destination.marker[mrk].position, source.marker[mrk].position );
	}
}

/**************************************************************************************/

// Get the latest frame of marker data from the specified unit.
// Marker positions are expressed in the intrinsic reference frame of the unit.

// Provide a method to retrieve the transformation for a given unit from 
//  the intrinsic to aligned reference frames.
void Tracker::GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) {
	CopyVector( offset, zeroVector );
	CopyMatrix( rotation, identityMatrix );
}

// Now provide the means to retrieve the frames and express the data in the intinsic frame.
// Note that there is not a "combined" version of this because it makes no sense
//  to combine the marker data in the intrinsic reference frames.
bool Tracker::GetCurrentMarkerFrameIntrinsic( MarkerFrame &iframe, int unit ) {

	MarkerFrame	frame;
	int			status;

	Vector3		offset;
	Matrix3x3	rotation;
	Matrix3x3	inverse;
	Vector3		delta;

	// Retrieve the offset and rotation matrix for this unit.
	GetUnitTransform( unit, offset, rotation );

	// Inverse of a rotation matrix is just its transpose.
	TransposeMatrix( inverse, rotation );

	// Get the current frame in aligned coordinates.
	status = GetCurrentMarkerFrameUnit( frame, unit );
	// I'm not sure what could go wrong, but signal if it does.
	if ( !status ) return( false );
	// Compute the position of each maker in intrinsic coordinates.
	for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
		iframe.marker[mrk].visibility = frame.marker[mrk].visibility;
		if ( frame.marker[mrk].visibility ) {
			SubtractVectors( delta, frame.marker[mrk].position, offset );
			MultiplyVector( iframe.marker[mrk].position, delta, inverse );
		}
	}

	return( true );
}
