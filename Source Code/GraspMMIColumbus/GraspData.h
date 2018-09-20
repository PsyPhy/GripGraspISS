/*********************************************************************************/
/*                                                                               */
/*                                  GraspData.h                                  */
/*                                                                               */
/*********************************************************************************/

// An object to hold the data from the GRASP experiment.

#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "../Useful/Timers.h"
#include "../Useful/Useful.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"


namespace Grasp {

	using namespace PsyPhy;

	typedef struct {

		int				trial;
		double			time;
		unsigned int	state;

		TrackerPose		hmd;
		TrackerPose		hand;
		TrackerPose		chest;
		TrackerPose		roll;

		MarkerFrame		markerFrame[MAX_UNITS];



	} GraspSample;


	public class GraspData : public VectorsMixin {

	public:

		GraspSample		sample[MAX_FRAMES];
		unsigned int	nSamples;

		Vector3			visibleMarker[MAX_UNITS][MAX_MARKERS][MAX_FRAMES];
		unsigned int	nVisibleSamples[MAX_UNITS][MAX_MARKERS];

	public:

		GraspData ( void ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					nVisibleSamples[unit][mrk] = 0;
				}
			}
		}


		int ReadMarkerFrame( FILE *fid, MarkerFrame *frame ) {
			int result;
			int visible;
			result = fscanf( fid, "%lf;", &frame->time );
			if ( result < 1 ) return( -1 );
			for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
				result = fscanf( fid, " %d; %lf; %lf; %lf;",
					&visible,
					&frame->marker[mrk].position[X],
					&frame->marker[mrk].position[Y],
					&frame->marker[mrk].position[Z] );
				if ( result < 4 ) return( -1 );
				frame->marker[mrk].visibility = ( visible != 0 );
			}
			return( 0 );
		}

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

		bool ReadGraspSample( FILE *fid, GraspSample *sample ) {

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

		void ExtractVisibleMarkerArrays( void ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					nVisibleSamples[unit][mrk] = 0;
					for ( unsigned int i = 0; i < nSamples; i += 100 ) {
						if ( sample[i].markerFrame[unit].marker[mrk].visibility ) {
							CopyVector( visibleMarker[unit][mrk][ nVisibleSamples[unit][mrk]++ ],
								sample[i].markerFrame[unit].marker[mrk].position );
						}
					}
					fOutputDebugString( "%d %d %d\n", unit, mrk, nVisibleSamples[unit][mrk] );
				}
			}
		}

		int LoadGraspData( FILE *fid ) {  
		
			char header[2048];
			fOutputDebugString( "Start LoadGraspData().\n" );
			nSamples = 0;
			fgets( header, sizeof( header ), fid );
			while ( nSamples < MAX_FRAMES ) {
				bool result = ReadGraspSample( fid, &sample[ nSamples ] );
				if ( !result ) break;
				nSamples++;
			}
			fOutputDebugString( "End LoadGraspData() with %d samples.\n", nSamples );
			ExtractVisibleMarkerArrays();
			return ( nSamples );
	
		}




	};

}