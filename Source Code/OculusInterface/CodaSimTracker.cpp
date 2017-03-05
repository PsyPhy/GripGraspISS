/***************************************************************************/
/*                                                                         */
/*                                CodaSimTracker                           */
/*                                                                         */
/***************************************************************************/

// Simulate a CodaRTnetTracker() using the Oculus and other input devices.
// This version depends on the Oculus capabilities, so it sits in the 
// OculusInterface package to avoid circular dependencies.

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/Timers.h"
#include "../Useful/ini.h"

#include "CodaSimTracker.h"

using namespace PsyPhy;

/***************************************************************************/

void CodaSimTracker::ReadModelMarkerPositions( FILE *fp, int strct ) {
	int mrk;
	for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		int id, items;
		double x, y, z;
		items = fscanf( fp, "%d %lf\t %lf\t %lf", &id, &x, &y, &z );
		if ( items != 4  ) break;
		fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), 
			"ReadModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
		structure[strct].vertex[mrk].id = id;
		structure[strct].vertex[mrk].position[X] = x;
		structure[strct].vertex[mrk].position[Y] = y;
		structure[strct].vertex[mrk].position[Z] = z;
	}
	structure[strct].nVertices = mrk;
}

void CodaSimTracker::ReadModelMarkerPositions( const char *filename, int strct ) {
	FILE *fp = fopen( filename, "r" );
	fAbortMessageOnCondition( ( NULL == fp ), "ReadModelMarkerPositions()", "Error opening %s for read.", filename );
	ReadModelMarkerPositions( fp, strct );
	fclose( fp );
}

/***************************************************************************/

void CodaSimTracker::Initialize( const char *ini_filename ) {
	ReadModelMarkerPositions( "Bdy\\Chest.bdy", 2 );
	ReadModelMarkerPositions( "Bdy\\Hand.bdy", 1 );
	ReadModelMarkerPositions( "Bdy\\HMD.bdy", 0 );
}

/***************************************************************************/

int CodaSimTracker::Update( void ) {
	if ( TimerTimeout( acquisitionTimer ) ) acquiring = false;
	return( 0 );
}

void CodaSimTracker::Shutdown( void ) {}

void CodaSimTracker::Quit( void ) {}

/*********************************************************************************/

void CodaSimTracker::StartAcquisition( float max_duration ) {
	acquiring = true;
	TimerSet( acquisitionTimer, max_duration );
}
void CodaSimTracker::AbortAcquisition( void ) {
	acquiring = false;
}
void CodaSimTracker::StopAcquisition( void ) {
	acquiring = false;
}

bool CodaSimTracker::CheckAcquisitionOverrun( void ) { 
	return( (bool) TimerTimeout( acquisitionTimer ) );
}

// Retrieve the stored CX1 data from a specified unit.
int CodaSimTracker::RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit ) {
	if ( unit >= nUnits ) {
		fMessageBox( MB_OK, "CodaRTnetTracker", "Specified unit out of range. \n   Specified: %d Units: %d", unit, nUnits );
		exit( -2 );
	}
	//* Loop through and get them all.
	for ( int frm = 0; frm < nFrames; frm++ ) {}
	return( nFrames );
}

// Retrieve the combined marker data.
int CodaSimTracker::RetrieveMarkerFrames( MarkerFrame frames[], int max_frames ) {
	//* Loop through and get them all.
	for ( int frm = 0; frm < nFrames; frm++ ) {
		frames[frm].time += recordedMarkerFrames[0][frm].time;
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {}
	}
	return( nFrames );
}

bool CodaSimTracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit ) {
	for ( int mrk = 0; mrk < nFrames; mrk++ ) {
		CopyVector( frame.marker[mrk].position, zeroVector );
		frame.marker[mrk].visibility = true;
	}
	if ( GetAcquisitionState() ) frame.time = TimerElapsedTime( acquisitionTimer );
	else frame.time = 0.0;

	return( true );
}

/****************************************************************************************/

bool CodaSimTracker::GetAcquisitionState( void ) {
	return( acquiring );
}

int CodaSimTracker::GetNumberOfCodas( void ) {
	return( codaUnits );
}

void CodaSimTracker::GetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS] ) {
	for ( int unit = 0; unit < codaUnits; unit++ ) {
			CopyVector( offset[unit], zeroVector );
			CopyMatrix( rotation[unit], identityMatrix );
	}
}

int  CodaSimTracker::PerformAlignment( int origin, int x_negative, int x_positive, int xy_negative, int xy_positive, bool force_show ) {
	int response = fMessageBox( MB_OK, "CodaSimTracker", "Simulating CODA alignment.\n  Units: %d", codaUnits ); 
	return( response );
}

void  CodaSimTracker::AnnulAlignment( const char *filename ) {
	SetAlignment( nullptr, nullptr, filename );
}

// Given the pose of a reference object computed in the intrinsic reference frame of each CODA unit,
//  compute and set the alignment file that the CODA uses to align the units and send it to the server.
void  CodaSimTracker::SetAlignmentFromPoses( Pose pose[MAX_UNITS], const char *filename ) {

	Vector3 offset[MAX_UNITS];
	Matrix3x3 rotation[MAX_UNITS], transpose_matrix;

	for ( int unit = 0; unit < nUnits; unit++ ) {
		// The pose gives us the orientation as a quaternion. 
		// CODA wants it as a rotation matrix.
		QuaternionToMatrix( rotation[unit], pose[unit].orientation );
		// The offset that the CODA appears to use is the computed from the measured offset
		// in intrinsic coordinates transformed by the inverse rotation (transpose).
		// NB The transpose may come from the fact that I use row vectors.
		// In any case, I came up with this by trial and error.
		TransposeMatrix( transpose_matrix, rotation[unit] );
		MultiplyVector( offset[unit], pose[unit].position, transpose_matrix );
	}
	SetAlignment( offset, rotation, filename );

}

// Send the alignment transformations specified as offset and rotation matrix to the CODA.
void  CodaSimTracker::SetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS], const char *filename ) {}	

/*********************************************************************************/

void CodaSimTracker::GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) {
	CopyVector( offset, zeroVector );
	CopyMatrix( rotation, identityMatrix );
}

void CodaSimTracker::WriteMarkerFile( char *filename ) {
	FILE *fp = fopen( filename, "w" );
	fAbortMessageOnCondition( !fp, "Error opening %s for writing.", filename );
	fprintf( fp, "%s\n", filename );
	fprintf( fp, "Tracker Units: %d\n", nUnits );
	fprintf( fp, "Frame\tTime" );
	for ( int mrk = 0; mrk <nMarkers; mrk++ ) {
		for ( int unit = 0; unit < nUnits; unit++ ) {
			fprintf( fp, "\tM%02d.%1d.V\tM%02d.%1d.X\tM%02d.%1d.Y\tM%02d.%1d.Z", mrk, unit, mrk, unit, mrk, unit, mrk, unit  );
		}
	}
	fprintf( fp, "\n" );

	for ( int frm = 0; frm < nFrames; frm++ ) {
		fprintf( fp, "%05d %9.3f", frm, recordedMarkerFrames[0][frm].time );
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			for ( int unit = 0; unit < nUnits; unit++ ) {
				fprintf( fp, " %1d",  recordedMarkerFrames[unit][frm].marker[mrk].visibility );
				for ( int i = 0; i < 3; i++ ) fprintf( fp, " %9.3f",  recordedMarkerFrames[unit][frm].marker[mrk].position[i] );
			}
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
}


