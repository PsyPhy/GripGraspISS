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

		// This valus determines how fast the inertially-computed pose will be driven
		// toward the absolute pose taken from the CODA. A large number means that drift 
		// will be corrected slowly.
		double InertialWeighting;
		// We define two values for the inertial weighting. One is used up until the 
		// first valid sample is obtained from the absolute tracker. A priori, it is 
		// a small value so that the output pose convergers rapidly from an arbitrary
		// starting state.
		double precaptureInertialWeighting;
		// The second value is presumably much larger. We assume that the inertial model 
		// is working well and that corrections for drift should be applied slowly. This
		// effectively filters the values from the absolute tracker, avoiding jumps in the 
		// output pose when the optical tracker experiences an anomalie.
		double postcaptureInertialWeighting;

		OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda );
		~OculusCodaPoseTracker();

		bool Initialize( void );
		bool  Update( void );
		bool Release( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );

};

};


