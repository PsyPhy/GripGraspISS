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

// Get the most recent frame of marker data.
// By default, simply take the data from unit 0.
bool  Tracker::GetCurrentMarkerFrame( MarkerFrame &frame ) { 
	return( GetCurrentMarkerFrameUnit( frame, 0 ) );
}
// Should return the most recent frame of data. 
// If your tracker does not provide this functionality, then simply
//  return false to signal that no frame is available.
bool  Tracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit ) { 
	return( false );
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

void	Tracker::ComputeAverageMarkerFrame( MarkerFrame &frame, MarkerFrame frames[], int n_frames ) {
	int mrk, frm;
	for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		CopyVector( frame.marker[mrk].position, zeroVector );
		frame.marker[mrk].visibility = false;
	}
	frame.time = 0.0;

	for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		int count = 0;
		for ( frm = 0; frm < n_frames; frm++ ) {
			if ( frames[frm].marker[mrk].visibility ) {
				AddVectors( frame.marker[mrk].position, frame.marker[mrk].position, frames[frm].marker[mrk].position );
				count++;
			}
		}
		if ( count > 0 ) {
			frame.marker[mrk].visibility = true;
			ScaleVector( frame.marker[mrk].position, frame.marker[mrk].position, 1.0 / (double) count );
		}
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
