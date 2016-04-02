/*********************************************************************************/
/*                                                                               */
/*                                CodaPoseTracker.cpp                            */
/*                                                                               */
/*********************************************************************************/

#include "stdafx.h"
#include "CodaPoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"

using namespace PsyPhy;

bool CodaPoseTracker::Initialize( void ) { return true; }
bool CodaPoseTracker::Update( void ) { return true; }
bool CodaPoseTracker::Quit( void ) { return true; }

bool CodaPoseTracker::GetCurrentPoseIntrinsic( TrackerPose *pose ) { 

	Vector3		selected_model[MAX_MARKERS];
	Vector3		selected_actual[MAX_MARKERS];

	bool		visible;

	// Select the visible output markers and the corresponding inputs.
	int n_markers = 0;
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		int id = modelMarker[mrk].id;
		if ( frame->marker[id].visibility ) {
			CopyVector( selected_model[n_markers], modelMarker[mrk].position );
			CopyVector( selected_actual[n_markers], frame->marker[id].position );
			n_markers++;
		}
	}

	// ComputeRigidBodyPose() does it all!
	pose->visible = ComputeRigidBodyPose( pose->position, pose->orientation, selected_model, selected_actual, n_markers, nullQuaternion );
	// Convert CODA position in mm to position in meters.
	ScaleVector( pose->position, pose->position, 0.001 );
	pose->time = 0.0;
	return( pose->visible );

}

int CodaPoseTracker::SetModelMarkerPositions( int n_markers, int *marker_list, MarkerFrame *frame ) {
	fAbortMessageOnCondition( ( n_markers < 0 ), "MeasureModelMarkerPositions()", "Invalid number of markers\n  n_markers = %d < 0", n_markers );
	fAbortMessageOnCondition( ( n_markers > MAX_MARKERS ), "MeasureModelMarkerPositions()", "To many markers in marker list\n  n_markers = %d > MAX_MARKERS = %d", n_markers, MAX_MARKERS );
	Vector3 centroid = {0.0, 0.0, 0.0};
	for ( int mrk = 0; mrk < n_markers; mrk++ ) {
		int id = marker_list[mrk];
		fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), "MeasureModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
		fAbortMessageOnCondition( ( !frame->marker[ id ].visibility ), "MeasureModelMarkerPositions()", "Marker %d ID = %d not visible.", mrk, id );
		modelMarker[mrk].id = id;
		CopyVector( modelMarker[mrk].position, frame->marker[id].position );
		AddVectors( centroid, centroid, frame->marker[id].position );
	}
	// Compute the marker positions relative to the centroid.
	ScaleVector( centroid, centroid, 1.0 / (double) n_markers );
	for ( int mrk = 0; mrk < n_markers; mrk++ ) {
		SubtractVectors( modelMarker[mrk].position, modelMarker[mrk].position, centroid );
	}
	nModelMarkers = n_markers;
	return nModelMarkers;
}

void CodaPoseTracker::WriteModelMarkerPositions( FILE *fp ) {
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		fprintf( fp, "%d\t%8.3f\t%8.3f\t%8.3f\n", modelMarker[mrk].id, modelMarker[mrk].position[X], modelMarker[mrk].position[Y], modelMarker[mrk].position[Z] );
	}
	fprintf( fp, "\n;;; Rigid Body Model File\n" );
}

void CodaPoseTracker::ReadModelMarkerPositions( FILE *fp ) {
	int mrk;
	for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		int id, items;
		double x, y, z;
		items = fscanf( fp, "%d %lf\t %lf\t %lf", &id, &x, &y, &z );
		if ( items != 4  ) break;
		fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), "ReadModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
		modelMarker[mrk].id = id;
		modelMarker[mrk].position[X] = x;
		modelMarker[mrk].position[Y] = y;
		modelMarker[mrk].position[Z] = z;
	}
	nModelMarkers = mrk;
}
void CodaPoseTracker::ReadModelMarkerPositions( const char *filename ) {
	FILE *fp = fopen( filename, "r" );
	fAbortMessageOnCondition( ( NULL == fp ), "ReadModelMarkerPositions()", "Error opening %s for read.", filename );
	ReadModelMarkerPositions( fp );
	fclose( fp );
}

