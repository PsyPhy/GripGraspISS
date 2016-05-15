/*********************************************************************************/
/*                                                                               */
/*                               OculusPoseTrackers.h                            */
/*                                                                               */
/*********************************************************************************/

// Access the Oculus head tracker in the PsyPhy domain.

#pragma once

#include "Trackers.h"
#include "PoseTrackers.h"

namespace PsyPhy {

class CodaPoseTracker : public PoseTracker {

	private:

		// This will hold a pointer to one frame of CODA data.
		// In this flavor of a CodaPoseTracker it is assumed that some outside agent 
		//  will fill the buffer before each call to GetCurrentPoseIntrinsic();
		// In this way multiple CodaPoseTrackers can share the same buffer of marker data.
		MarkerFrame	*frame;

		// A buffer that holds the model for the rigid body.
		// Each element is the number and the 3D position of a marker with respect
		//  to the control point of the rigid body when in the null pose.
		struct {
			int		id;
			Vector3	position;
		} modelMarker[MAX_MARKERS];
		int nModelMarkers;

	protected:

	public:

		double intermarkerDistanceTolerance;

		CodaPoseTracker( MarkerFrame *frame = nullptr ) : nModelMarkers(0), intermarkerDistanceTolerance(2.0) {
			// Store the pointer to the marker frame.
			this->frame = frame;
			// Do whatever the default PoseTracker would do.
			PoseTracker::PoseTracker();
		}
		~CodaPoseTracker();

		void SetMarkerFrameBuffer( MarkerFrame *frame ) {
			this->frame = frame;
		}

		bool Initialize( void );
		bool Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( TrackerPose &pose );
		int  SetModelMarkerPositions( int n_markers, int *marker_list, MarkerFrame *frame = nullptr );
		void	ReadModelMarkerPositions( FILE *fp );
		void	WriteModelMarkerPositions( FILE *fp );
		void	ReadModelMarkerPositions( const char *filename );

};

};


