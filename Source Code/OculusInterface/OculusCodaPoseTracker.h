/*********************************************************************************/
/*                                                                               */
/*                               OculusCodaPoseTrackers.h                            */
/*                                                                               */
/*********************************************************************************/

// A PoseTracker that combines Oculus and Coda.

#pragma once

#include "../Trackers/PoseTrackers.h"

namespace PsyPhy {

class OculusCodaPoseTracker : public PoseTracker {

	private:

		OculusMapper *oculusMapper;

		TrackerPose		currentState;
		TrackerPose		predictedState;

	protected:

	public:

		OculusCodaPoseTracker( OculusMapper *mapper = nullptr );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose );

};

};


