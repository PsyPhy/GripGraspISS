/*********************************************************************************/
/*                                                                               */
/*                                  Trackers.h                                   */
/*                                                                               */
/*********************************************************************************/

// A 'Tracker' is a device that tracks in 3D the movements of a set of markers.
// The interface is designed around the CodaRTnet interface.
// The main functions allow to:
//	1) start and stop continuous acquisitions at a fixed frequency
//	2) retrieve the time series of marker positions and their visibility
//	3) query the tracker in real time to retrieve the latest observed marker positions.


#pragma once

#include "../Useful/Useful.h"
#include "../Useful/Timers.h"
#include "../VectorsMixin/VectorsMixin.h"

/********************************************************************************/

// Constants used to allocate buffers. They should be small to save space but
//  large enough to handle most options. Here I have set them for the GRASP project.

// On GRASP with DEX, the max number of markers is 24, but the CODA will acquire 28
//  when in 200Hz mode. So we allocate enough space for all to keep the retrieval 
//  routines happy.
#define MAX_MARKERS				28
// On DEX (GRASP) we have 2 units. In the lab we could have more, but I set it to
//  2 in order to save buffer space.
#define MAX_UNITS				2
// The longest duration of acquisition allowed. (samples/sec * sec/min * min)
#define MAX_MINUTES	10
#define MAX_FRAMES				(200 * 60 * MAX_MINUTES)
#define DEFAULT_SAMPLE_PERIOD	0.005
#define INVISIBLE				-99999.99999

// Structures that hold the collected data.
typedef struct {
	PsyPhy::Vector3	position;
	bool	visibility;
} MarkerState;

typedef struct {
	MarkerState	marker[MAX_MARKERS];
	double		time;
} MarkerFrame;

typedef struct {
	MarkerFrame	combinedFrame;
	MarkerFrame unitFrame[MAX_UNITS];
	bool	hasCombinedFrame;
	int		unitFrames;
} MarkerFrameSet;

class Tracker : public PsyPhy::VectorsMixin {

	private:

	protected:

	public:

		// Number of markers to be acquired.
		int nMarkers;
		int nUnits;
		int nFrames;

		double samplePeriod;

		Tracker() : nUnits( 0 ), nMarkers( MAX_MARKERS ), samplePeriod( DEFAULT_SAMPLE_PERIOD ) {} ;

		virtual void Initialize( void );
		virtual int  Update( void );
		virtual void Quit( void );

		virtual void	StartAcquisition( float max_duration );
		virtual void	StopAcquisition( void );
		virtual bool	GetAcquisitionState( void );
		virtual bool	CheckAcquisitionOverrun( void );
		// Retieve all the marker data 
		virtual int		RetrieveMarkerFrames( MarkerFrame frames[], int max_frames ) = 0;
		virtual int		RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit );

		virtual bool	GetCurrentMarkerFrame( MarkerFrame &frame );
		virtual bool	GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit );
		virtual bool	GetCurrentMarkerFrameIntrinsic( MarkerFrame &frame, int unit );


		virtual double	GetSamplePeriod( void );
		virtual int		GetNumberOfUnits( void );
		virtual void	GetUnitTransform( int unit, PsyPhy::Vector3 &offset, PsyPhy::Matrix3x3 &rotation ) ;

		void	CopyMarkerFrame( MarkerFrame &destination, MarkerFrame &source );
		void	ComputeAverageMarkerFrame( MarkerFrame &frame, MarkerFrame frames[], int n_frames );

};

