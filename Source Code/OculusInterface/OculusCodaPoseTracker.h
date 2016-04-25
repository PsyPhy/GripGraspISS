/*********************************************************************************/
/*                                                                               */
/*                               OculusCodaPoseTrackers.h                            */
/*                                                                               */
/*********************************************************************************/

// A PoseTracker that combines Oculus and Coda.

#pragma once

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"

namespace PsyPhy {

class OculusCodaPoseTracker : public PoseTracker {

	private:

		OculusMapper *oculusMapper;
		CodaPoseTracker	*primaryCoda;
		CodaPoseTracker *secondaryCoda;

		TrackerPose		currentState;
		TrackerPose		predictedState;

	protected:

	public:

		double InertialWeighting;

		OculusCodaPoseTracker( OculusMapper *mapper = nullptr, CodaPoseTracker *coda1 = nullptr, CodaPoseTracker *coda2 = nullptr );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose );

};

};


