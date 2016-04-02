/*********************************************************************************/
/*                                                                               */
/*                                  PoseTrackers.h                               */
/*                                                                               */
/*********************************************************************************/

// A 'PoseTracker' is a device that tracks the 6D position and orientation of a object.
// For the moment it is used only for polled real-time tracking.
// They could later be augmented to include continuous recording at a fixed frequency.

#pragma once

#include "../VectorsMixin/VectorsMixin.h"

namespace PsyPhy {

typedef struct {
	PsyPhy::Vector3		position;
	PsyPhy::Quaternion	orientation;
	bool		visible;
	double		time;
} TrackerPose;

class PoseTracker : public PsyPhy::VectorsMixin {

	private:

	protected:

		TrackerPose		nullPose;

	public:

		PoseTracker ();
		~PoseTracker ();

		virtual bool Initialize( void ){ return true; };
		virtual bool  Update( void ){ return true; };
		virtual bool Quit( void ){ return true; };

		virtual void Boresight( TrackerPose *pose );
		virtual bool BoresightCurrent( void );

		virtual bool GetCurrentPosition( PsyPhy::Vector3 position );
		virtual bool GetCurrentOrientation( PsyPhy::Quaternion orientation );
		virtual bool GetCurrentPose( TrackerPose *pose );
		virtual bool GetCurrentPoseIntrinsic( TrackerPose *pose );

		void CopyTrackerPose( TrackerPose *destination, TrackerPose *source ) ;

};

class NullPoseTracker : public PoseTracker {

public:
	bool GetCurrentPoseIntrinsic( TrackerPose *pose );
};

};