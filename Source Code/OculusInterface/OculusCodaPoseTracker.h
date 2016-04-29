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

		OculusMapper *oculusMapper;
		CodaPoseTracker	*primaryCoda;
		CodaPoseTracker *secondaryCoda;

		TrackerPose		currentState;
		TrackerPose		predictedState;

		FILE *fp;

	protected:

	public:

		double InertialWeighting;

		OculusCodaPoseTracker( OculusMapper *mapper, CodaPoseTracker *coda1, CodaPoseTracker *coda2 );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose );

};

};


