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


		bool ReadGraspSample( FILE *fid, GraspSample *sample ) {

			int result;
			result = fscanf( fid, "%d;", &sample->trial );
			if ( result < 1 ) return false;
			result = fscanf( fid, "%lf;", &sample->time );
			if ( result < 1 ) return false;
			result = fscanf( fid, "%x;", &sample->state );
			if ( result < 1 ) return false;

			result = ReadTrackerPose( sample->hmd, fid );
			if ( result < 0 ) return false;
			fscanf( fid, ";" );
			result = ReadTrackerPose( sample->hand, fid );
			if ( result < 0 ) return false;
			fscanf( fid, ";" );
			result = ReadTrackerPose( sample->chest, fid );
			if ( result < 0 ) return false;
			fscanf( fid, ";" );
			result = ReadTrackerPose( sample->roll, fid );
			if ( result < 0 ) return false;
			fscanf( fid, ";" );

			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				result = ReadMarkerFrame( sample->markerFrame[unit], fid );
				if ( result < 0 ) return false;
			}
			return true;

		}

		void ExtractVisibleMarkerArrays( void ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					nVisibleSamples[unit][mrk] = 0;
					for ( unsigned int i = 0; i < nSamples; i++ ) {
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
			fgets( header, sizeof( header ), fid );
			nSamples = 0;
			while ( nSamples < MAX_FRAMES ) {
				bool result = ReadGraspSample( fid, &sample[ nSamples ] );
				if ( !result ) break;
				nSamples++;
			}
			fOutputDebugString( "End LoadGraspData() with %d samples.\n", nSamples );
			ExtractVisibleMarkerArrays();
			return ( nSamples );
	
		}

		int LoadGraspData( char *filename ) {  
			FILE *fp = fopen( filename, "r" );
			if ( !fp ) {
				fMessageBox( MB_OK, "GraspData", "Error opening %s for read.", filename );
				return( 0 );
			}
			int samples = LoadGraspData( fp );
			fclose( fp );
		}

	};

}