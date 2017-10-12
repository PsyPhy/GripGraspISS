/*********************************************************************************/
/*                                                                               */
/*                             OculusCodaPoseTrackers.h                          */
/*                                                                               */
/*********************************************************************************/

// A PoseTracker that combines Oculus and Coda.

#pragma once

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"

namespace PsyPhy {

class OculusCodaPoseTracker : public PoseTracker {

	private:

		PoseTracker		*absoluteTracker;

		TrackerPose		currentState;
		TrackerPose		predictedState;

	protected:

		double GetRotationRate( Vector3 rate );

	public:

		OculusMapper	*oculusMapper;
		bool useRawSensors;

		double InertialWeighting;

		OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Release( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );

};

};


