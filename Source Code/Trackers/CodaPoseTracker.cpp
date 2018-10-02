/*********************************************************************************/
/*                                                                               */
/*                                CodaPoseTracker.cpp                            */
/*                                                                               */
/*********************************************************************************/

#include "stdafx.h"
#include "CodaPoseTracker.h"
#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

using namespace PsyPhy;

// This may seem strange, but a CodaPoseTracker does not need to do anything to
//  initialize, update or release. The work is done by the CodaTracker that 
//  is presumed to fill in the marker buffers each time.
bool CodaPoseTracker::Initialize( void ) { return true; }
bool CodaPoseTracker::Update( void ) { return true; }
bool CodaPoseTracker::Release( void ) { return true; }

bool CodaPoseTracker::GetCurrentPoseIntrinsic( TrackerPose &pose ) { 
	return( ComputePose( pose, this->frame ) );
}

int  CodaPoseTracker::VisibleMarkers( MarkerFrame *frame ) {
	int visible_markers = 0;
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		int id = modelMarker[mrk].id;
		if ( frame->marker[id].visibility ) visible_markers++;
	}
	return( visible_markers );
}

int  CodaPoseTracker::VisibleMarkers( void ) {
	return( VisibleMarkers( this->frame ) );
}

double CodaPoseTracker::Residual( MarkerFrame *frame ) {
	double	sum = 0.0;
	double	count = 0.0;
	Vector3	rotated;
	Vector3 transformed;
	Vector3	difference;
	TrackerPose	pose;
	ComputePose( pose, frame );

	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		int id = modelMarker[mrk].id;
		if ( frame->marker[id].visibility ) {
			RotateVector( rotated, pose.pose.orientation, modelMarker[mrk].position );
			AddVectors( transformed, rotated, pose.pose.position );
			SubtractVectors( difference, frame->marker[id].position, transformed );
			sum += difference[X] * difference[X] + difference[Y] * difference[Y] + difference[Z] * difference[Z];
			count++;
		}
	}
	if ( sum > 0 ) return( sqrt( sum / count ) );
	else return( MISSING_DOUBLE );
}


bool CodaPoseTracker::ComputePose( TrackerPose &pose, MarkerFrame *frame, bool compute_indicators ) { 

	Vector3		selected_model[MAX_MARKERS];
	Vector3		selected_actual[MAX_MARKERS];
	Vector3		validated_model[MAX_MARKERS];
	Vector3		validated_actual[MAX_MARKERS];

	// Select the visible output markers and the corresponding inputs.
	int selected_markers = 0;
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		int id = modelMarker[mrk].id;
		if ( frame->marker[id].visibility ) {
			CopyVector( selected_model[selected_markers], modelMarker[mrk].position );
			CopyVector( selected_actual[selected_markers], frame->marker[id].position );
			selected_markers++;
		}
	}

	// Try to discard markers with reflections by comparing inter-marker distances.
	// We step through each of the visible markers, compute the distance between it and each of
	// the other markers and then compare the distance with the distance from the model. If
	// at least one distance is within tolerance, then the first marker is included in the 
	// validated list. 
	int validated_markers = 0;
	for ( int mrk1 = 0; mrk1 < selected_markers; mrk1++ ) {
		for ( int mrk2 = 0; mrk2 < selected_markers; mrk2++ ) {
			if ( mrk1 != mrk2 ) {
				Vector3 delta_model, delta_actual;
				SubtractVectors( delta_model, selected_model[mrk1], selected_model[mrk2] );
				SubtractVectors( delta_actual, selected_actual[mrk1], selected_actual[mrk2] );
				if ( fabs( VectorNorm( delta_model ) - VectorNorm( delta_actual ) ) < intermarkerDistanceTolerance ) {
					CopyVector( validated_model[validated_markers], selected_model[mrk1] );
					CopyVector( validated_actual[validated_markers], selected_actual[mrk1] );
					validated_markers++;
					break;
				}
			}
		}
	}
	//if ( validated_markers < visible_markers ) {
	//	fOutputDebugString( "%08x Visible Markers: %d Rejected %d markers.\n", (unsigned int) this, visible_markers, visible_markers - validated_markers );
	//}

	// A measure of the quality of the sample is that of how many markers
	// are rejected based on the intermarker distance criterion.
	pose.quality = selected_markers - validated_markers;

	if ( validated_markers < 4 ) {
		pose.visible = false;
		pose.fidelity = MISSING_DOUBLE;
	}
	else {
		
		// ComputeRigidBodyPose() does it all!
		pose.visible = ComputeRigidBodyPose( pose.pose.position, pose.pose.orientation, validated_model, validated_actual, validated_markers, nullptr );

		// Compute the residual of the selected markers as a measure of fidelity.
		// But only do it if we are not in fast mode.
		if ( pose.visible && compute_indicators ) {

			double	sum = 0.0;
			double	count = 0.0;
			Vector3	rotated;
			Vector3 transformed;
			Vector3	difference[MAX_MARKERS];

			for ( int mrk = 0; mrk < validated_markers; mrk++ ) {
				RotateVector( rotated, pose.pose.orientation, validated_model[mrk] );
				AddVectors( transformed, rotated, pose.pose.position );
				SubtractVectors( difference[mrk], validated_actual[mrk], transformed );
				sum += difference[mrk][X] * difference[mrk][X] + difference[mrk][Y] * difference[mrk][Y] + difference[mrk][Z] * difference[mrk][Z];
				count++;
			}
			pose.fidelity = sqrt( sum / count );

		}
		else pose.fidelity = MISSING_DOUBLE;

		// Scale the position. This can be used to convert the intrinsic mm to meters.
		ScaleVector( pose.pose.position, pose.pose.position, positionScaleFactor );
	}

	// Here we should set the time of the sample with respect to some clock common to the other tracker.
	// For the moment I use the time of the marker frame.
	pose.time = frame->time;

	//	fOutputDebugString( "CodaPoseTracker Intrinsic %s\n", ( pose.visible ? "Visible" : "Occluded" ));
	// returns whether or not the rigid body is visible.
	return( pose.visible );

}

int CodaPoseTracker::SetModelMarkerPositions( int n_markers, int *marker_list, MarkerFrame *frame ) {
	fAbortMessageOnCondition( ( n_markers < 0 ), "MeasureModelMarkerPositions()", "Invalid number of markers\n  n_markers = %d < 0", n_markers );
	fAbortMessageOnCondition( ( n_markers > MAX_MARKERS ), "MeasureModelMarkerPositions()", "To many markers in marker list\n  n_markers = %d > MAX_MARKERS = %d", n_markers, MAX_MARKERS );
	Vector3 centroid = {0.0, 0.0, 0.0};

	int visible_markers = 0;
	char invisible[256] = "";
	for ( int mrk = 0; mrk < n_markers; mrk++ ) {
		int id = marker_list[mrk];
		fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), "MeasureModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
		if ( !frame->marker[ id ].visibility ) {
			char tochar[256];
			sprintf( tochar, " %02d", id );
			strcat( invisible, tochar );
		}
		else {
			modelMarker[visible_markers].id = id;
			CopyVector( modelMarker[visible_markers].position, frame->marker[id].position );
			AddVectors( centroid, centroid, frame->marker[id].position );
			visible_markers++;
		}
	}

	if ( visible_markers < n_markers ) {
		int response;
		response = fMessageBox( MB_OKCANCEL | MB_ICONEXCLAMATION, "CodaPoseTracker::SetModelMarkerPositions", "Markers not visible: %s\nContinue anyway?", invisible );
		if ( response == IDCANCEL ) return( 0 );
	}

	if ( visible_markers < 4 ) {
		int response;
		response = fMessageBox( MB_OKCANCEL | MB_ICONQUESTION, "CodaPoseTracker::SetModelMarkerPositions", "Not enough markers to define rigid body pose.\nNumber of visibles markers: %d\n\nContinue anyway?", visible_markers );
		if ( response == IDCANCEL ) return( 0 );
	}

	// Compute the marker positions relative to the centroid.
	if ( visible_markers > 0 ) ScaleVector( centroid, centroid, 1.0 / (double) visible_markers );
	for ( int mrk = 0; mrk < visible_markers; mrk++ ) {
		SubtractVectors( modelMarker[mrk].position, modelMarker[mrk].position, centroid );
	}
	nModelMarkers = visible_markers;
	return nModelMarkers;
}

void CodaPoseTracker::WriteModelMarkerPositions( FILE *fp ) {

	SYSTEMTIME  st;
	GetSystemTime( &st );

	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		fprintf( fp, "%d\t%8.3f\t%8.3f\t%8.3f\n", modelMarker[mrk].id, modelMarker[mrk].position[X], modelMarker[mrk].position[Y], modelMarker[mrk].position[Z] );
	}
	if ( nModelMarkers < 1 ) fprintf( fp, "\n;;; Empty Rigid Body Model File (N markers < 1 )\n" );
	else fprintf( fp, "\n;;; Rigid Body Model File\n" );
	fprintf( fp, ";;; Created %d/%02d/%02d %02dh%02dm%02ds GMT\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
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

void CodaPoseTracker::WriteModelMarkerPositions( const char *filename ) {
	FILE *fp = fopen( filename, "w" );
	fAbortMessageOnCondition( ( NULL == fp ), "WriteModelMarkerPositions()", "Error opening %s for write.", filename );
	fclose( fp );
}
