/*********************************************************************************/
/*                                                                               */
/*                               OculusPoseTrackers.h                            */
/*                                                                               */
/*********************************************************************************/

// Access the Oculus head tracker in the PsyPhy domain.

#pragma once

#include "../Trackers/PoseTrackers.h"

namespace PsyPhy {

class OculusPoseTracker : public PoseTracker {

	private:

		OculusMapper *oculusMapper;

	protected:

	public:

		OculusPoseTracker( OculusMapper *mapper = nullptr );
		~OculusPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose *pose );

};

};


