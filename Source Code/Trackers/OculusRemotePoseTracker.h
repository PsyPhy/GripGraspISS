/*********************************************************************************/
/*                                                                               */
/*                            OculusRemotePoseTracker.h                         */
/*                                                                               */
/*********************************************************************************/

#pragma once

#include "../Trackers/PoseTrackers.h"

namespace PsyPhy {

class OculusRemotePoseTracker : public PoseTracker {

	private:

		TrackerPose current_pose;
		Vector3		eulerAngles;

		ovrSession			session;
		ovrGraphicsLuid		luid;

		Timer timer;
		double gain;

	public:

		OculusRemotePoseTracker( double gain = 1.0 );
		~OculusRemotePoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Release( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );

};

};


