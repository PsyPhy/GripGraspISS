/*********************************************************************************/
/*                                                                               */
/*                                   CodaSimTracker.h                            */
/*                                                                               */
/*********************************************************************************/

// Simulate a CodaRTnetTracker.

#pragma once

// Disable warnings about "unsafe" functions.
#define _CRT_SECURE_NO_WARNINGS

#include "..\Trackers\Trackers.h"
#include "..\Useful\Timers.h"

namespace PsyPhy {

#define MARKER_ARRAYS	3

class CodaSimTracker : public Tracker {

private:

	// Define groups of
	int nMarkerArrays;
	struct {
		struct {
			int	id;
			Vector3	position;
		} vertex[MAX_MARKERS];
		int nVertices;
	} structure[MARKER_ARRAYS];

public:
	// Buffers to hold the simulated data retrieved from the CODA units.
	MarkerFrame		recordedMarkerFrames[MAX_UNITS][MAX_FRAMES];

protected:

	int codaUnits;
	int nFrames;
	int nMarkers;

	::Timer acquisitionTimer;
	bool acquiring;

public:

	CodaSimTracker( void ) : codaUnits(2), nFrames(0), nMarkers(28), acquiring( false ) {
		for ( int i = 0; i < MARKER_ARRAYS; i++ ) structure[i].nVertices = 0;
	}

	void  Initialize( const char *ini_filename = "CodaRTnet.ini" );
	int  Update( void );
	void Quit( void );

	void StartAcquisition( float max_duration );
	void StopAcquisition( void );
	void AbortAcquisition( void );
	bool CheckAcquisitionOverrun( void );

	bool GetAcquisitionState( void );
	int  GetNumberOfCodas( void );

	int		RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit );
	int		RetrieveMarkerFrames( MarkerFrame frames[], int max_frames );
	bool	GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit );

	int		PerformAlignment( int origin, int x_negative, int x_positive, int xy_negative, int xy_positive, bool force_show = true );
	void	AnnulAlignment( const char *filename = nullptr );
	void	GetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS] );
	void	SetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS], const char *filename = nullptr );
	// This is a little different from the above. If we have the pose of an object in the intrinsic frame,
	// the transformation has to be inverted before sending it to the CODA system.
	void	SetAlignmentFromPoses( Pose pose[MAX_UNITS], const char *filename );

	void	GetUnitPlacement( int unit, Vector3 &pos, Quaternion &ori ) ;
	void	GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) ;

	void	WriteMarkerFile( char *filename );
	void	Shutdown( void );

	void	ReadModelMarkerPositions( FILE *fp, int structure_index );
	void	ReadModelMarkerPositions( const char *filename, int structure_index );

};

};
