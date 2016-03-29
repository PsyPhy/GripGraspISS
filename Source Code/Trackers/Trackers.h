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

#define MAX_MARKERS	24
#define MAX_UNITS	2
#define MAX_FRAMES	20000
#define DEFAULT_SAMPLE_PERIOD	0.005

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

typedef struct {
	PsyPhy::Vector3		position;
	PsyPhy::Quaternion	orientation;
	bool		visibility;
	double		time;
} TrackerPose;

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
		virtual int		RetrieveMarkerFrames( MarkerFrame frames[], int max_frames );
		virtual int		RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit );

		virtual bool	GetCurrentMarkerFrame( MarkerFrame &frame );
		virtual bool	GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit );
		virtual bool	GetCurrentMarkerFrameIntrinsic( MarkerFrame &frame, int unit );

		virtual double	GetSamplePeriod( void );
		virtual int		GetNumberOfUnits( void );
		virtual void	GetUnitTransform( int unit, PsyPhy::Vector3 &offset, PsyPhy::Matrix3x3 &rotation ) ;

		void			CopyMarkerFrame( MarkerFrame &destination, MarkerFrame &source );

};

