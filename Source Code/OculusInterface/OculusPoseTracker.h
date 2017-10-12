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

	protected:

		OculusMapper *oculusMapper;

	protected:

	public:

		OculusPoseTracker( OculusMapper *mapper = nullptr );

		bool Initialize( void );
		bool  Update( void );
		bool Quit( void );

		virtual bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose ){ return( false ); };

};

class OculusHMDPoseTracker : public OculusPoseTracker {
	public:
		OculusHMDPoseTracker( OculusMapper *mapper = nullptr );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );
};
class OculusHandPoseTracker : public OculusPoseTracker {
	public:
		ovrHandType hand;
		OculusHandPoseTracker( ovrHandType hand = ovrHand_Right, OculusMapper *mapper = nullptr );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );
};
class OculusRightHandPoseTracker : public OculusHandPoseTracker {
	public:
		OculusRightHandPoseTracker( OculusMapper *mapper = nullptr );
};
class OculusLeftHandPoseTracker : public OculusHandPoseTracker {
	public:
		OculusLeftHandPoseTracker( OculusMapper *mapper = nullptr );
};

}

