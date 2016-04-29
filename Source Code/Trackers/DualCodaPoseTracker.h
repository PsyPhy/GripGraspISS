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

class DualCodaPoseTracker : public PoseTracker {

	private:

		CodaPoseTracker	*primaryCoda;
		CodaPoseTracker *secondaryCoda;

		FILE *fp;

	protected:

	public:

		TrackerPose currentState;

		DualCodaPoseTracker( CodaPoseTracker *coda1, CodaPoseTracker *coda2 );
		~DualCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose );

};

};


