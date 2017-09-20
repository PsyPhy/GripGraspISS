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

		OculusMapper	*oculusMapper;
		PoseTracker		*absoluteTracker;

		TrackerPose		currentState;
		TrackerPose		predictedState;

	protected:

	public:

		double InertialWeighting;

		ovrTrackingState sensorState;
		ovrSensorData rawSensorData;

		OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Release( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );

};

};


