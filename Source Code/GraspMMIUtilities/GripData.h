/*********************************************************************************/
/*                                                                               */
/*                                   GripData.h                                  */
/*                                                                               */
/*********************************************************************************/

// An object to hold the data from the GRASP experiment.

#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "../Useful/Timers.h"
#include "../Useful/Useful.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"


#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"

#define GRIP_MAX_MARKERS 28

namespace Grip {

	using namespace PsyPhy;

	typedef struct {

		int				tick;
		unsigned int	visibility;
		MarkerFrame		markerFrame;


	} GripSample;


	public class GripData : public VectorsMixin {

	public:

		GripSample		sample[MAX_UNITS][MAX_FRAMES];
		unsigned int	nSamples;

		Vector3			visibleMarker[MAX_UNITS][GRIP_MAX_MARKERS][MAX_FRAMES];
		unsigned int	nVisibleSamples[MAX_UNITS][GRIP_MAX_MARKERS];

		int marker_map[GRIP_MAX_MARKERS];
		int visibility_map[GRIP_MAX_MARKERS];


	public:

		GripData ( void ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				for ( int mrk = 0; mrk < GRIP_MAX_MARKERS; mrk++ ) {
					nVisibleSamples[unit][mrk] = 0;
				}
			}
		}


		bool ReadSample( FILE *fid, GripSample *sample ) {

			int result;
			int visible;

			char date[256];
			char time[256];
			int  ms;
			char type[256];
			char packet[256];

			int x, y, z;

			result = fscanf( fid, "%s %s %d %s %s", date, time, &ms, type, packet );
			if ( result < 5 ) return( false );

			result = fscanf( fid, " %d", &sample->tick );
			if ( result < 1 ) return( false );

			for ( int mrk = 0; mrk < GRIP_MAX_MARKERS; mrk++ ) {
				result = fscanf( fid, " %d %d %d", &x, &y, &z );
				if ( result < 3 ) return( false );
				sample->markerFrame.marker[ marker_map[mrk] ].position[X] = (double) x / 10.0;
				sample->markerFrame.marker[ marker_map[mrk] ].position[Y] = (double) y / 10.0;
				sample->markerFrame.marker[ marker_map[mrk] ].position[Z] = (double) z / 10.0;
			}

			result = fscanf( fid, " %d", &sample->visibility );
			if ( result < 1 ) return( false );

			for ( int mrk = 0; mrk < GRIP_MAX_MARKERS; mrk++ ) {
				result = fscanf( fid, " %d", &visible );
				if ( result < 1 ) return( false );
				sample->markerFrame.marker[ visibility_map[mrk] ].visibility = ( visible != 0 );
			}
			return( true );
		}

/* These are not done yet. They are copies from GraspData.
		int ReadTrackerPose( FILE *fid, TrackerPose *pose ) {
			int result;
			bool visible;
			result = fscanf( fid, " %lf;", &pose->time );
			if ( result < 1 ) return( -1 );
			result = fscanf( fid, " %d;", &visible );
			if ( result < 1 ) return( -1 );
			pose->visible = ( visible != 0 );
			result = fscanf( fid, " < %lf %lf %lf>;",
				&pose->pose.position[X], &pose->pose.position[Y], &pose->pose.position[Z] );
			if ( result < 3 ) return( -1 );
			result = fscanf( fid, " {%lfi %lfj %lfk %lf};",
				&pose->pose.orientation[X], &pose->pose.orientation[Y], &pose->pose.orientation[Z], &pose->pose.orientation[M] );
			if ( result < 4 ) return( -1 );
			return( 0 );
		}

		bool ReadRealtimeSample( FILE *fid, GraspSample *sample ) {

			int result;
			result = fscanf( fid, "%d;", &sample->trial );
			if ( result < 1 ) return false;
			result = fscanf( fid, "%lf;", &sample->time );
			if ( result < 1 ) return false;
			result = fscanf( fid, "%x;", &sample->state );
			if ( result < 1 ) return false;

			result = ReadTrackerPose( fid, &sample->hmd );
			if ( result < 0 ) return false;
			result = ReadTrackerPose( fid, &sample->hand );
			if ( result < 0 ) return false;
			result = ReadTrackerPose( fid, &sample->chest );
			if ( result < 0 ) return false;
			result = ReadTrackerPose( fid, &sample->roll );
			if ( result < 0 ) return false;

			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				result = ReadMarkerFrame( fid, &sample->markerFrame[unit] );
				if ( result < 0 ) return false;
			}
			return true;

		}
		*/

		void ExtractVisibleMarkerArrays( void ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					nVisibleSamples[unit][mrk] = 0;
					for ( unsigned int i = 0; i < nSamples; i++ ) {
						if ( sample[unit][i].markerFrame.marker[mrk].visibility ) {
							CopyVector( visibleMarker[unit][mrk][ nVisibleSamples[unit][mrk]++ ],
								sample[unit][i].markerFrame.marker[mrk].position );
						}
					}
					fOutputDebugString( "%d %d %d\n", unit, mrk, nVisibleSamples[unit][mrk] );
				}
			}
		}

		int LoadGripDataUnit( FILE *fid, int unit ) {  

			char date[256];
			char time[256];
			char ms[256];
			char type[256];
			char packet[256];
			char error[256];
			char tick[256];

			char header_item[256];

			int result;
		
			fOutputDebugString( "Start LoadGripData().\n" );

			// We need to parse the header to know which column goes with which marker.
			result = fscanf( fid, "%s %s %s %s %s %s %s", date, time, &ms, type, packet, error, tick );
			if ( result < 6 ) fAbortMessage( "GripData", "Error reading leading headers.\n" );
			for ( int mrk = 0; mrk < GRIP_MAX_MARKERS; mrk++ ){
				for ( int j = 0; j < 3; j++ ) {
					fscanf( fid, " %s", header_item );
					result = sscanf( header_item, "marker_%d_pos", &marker_map[mrk] );
					if ( result < 1 ) fAbortMessage( "GripData", "Error parsing marker header %d.\n", mrk );
				}
				marker_map[mrk]--;
				fOutputDebugString( "Item %s  Marker Column %d -> %d\n", header_item, mrk, marker_map[mrk] );
			}
			fscanf( fid, " %s", header_item );
			fOutputDebugString( "Markers Visbility  Header Item %s\n", header_item );
			for ( int mrk = 0; mrk < GRIP_MAX_MARKERS; mrk++ ){
				fscanf( fid, " %s", header_item );
				result = sscanf( header_item, "marker_%d_visib", &visibility_map[mrk] );
				if ( result < 1 ) fAbortMessage( "GripData", "Error parsing visibility header %d.\n", mrk );
				visibility_map[mrk]--;
				fOutputDebugString( "Item %s  Visibility Column %d -> %d\n", header_item, mrk, visibility_map[mrk] );
			}

			nSamples = 0;
			while ( nSamples < MAX_FRAMES ) {
				bool result = ReadSample( fid, &sample[unit][ nSamples ] );
				if ( !result ) break;
				nSamples++;
			}
			fOutputDebugString( "End LoadGraspData() for CODA unit %d with %d samples.\n", unit, nSamples );
			ExtractVisibleMarkerArrays();
			return ( nSamples );
	
		}




	};

}