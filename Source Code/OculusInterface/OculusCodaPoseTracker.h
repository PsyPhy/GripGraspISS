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
		// output pose when the optical tracker experiences an anomaly.
		double postcaptureInertialWeighting;
		// We adaptively adjust the weighting between the extremes. If the absolute data
		// has been missing for a while, adjust the InertialWeighting toward the precapture
		// value and vice-versa. The following constant determines how fast to adapt the
		// weight. 
		double	occlusionWeightingTimeConstant;

		OculusCodaPoseTracker( OculusMapper *mapper, PoseTracker *coda, const char *ini_filename = nullptr );
		~OculusCodaPoseTracker();

		// Provide the means to read a .ini file to set configuration parameters.
		// This is defined here as static because its address is sent as a callback to a parsing routine.
		static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			OculusCodaPoseTracker *instance = (OculusCodaPoseTracker *) which_instance;
			if ( !strcmp( name, "precaptureInertialWeighting" ) && !strcmp( section, "GraspTrackers" ) ) instance->precaptureInertialWeighting = atof( value );
			if ( !strcmp( name, "postcaptureInertialWeighting" ) && !strcmp( section, "GraspTrackers" ) ) instance->postcaptureInertialWeighting = atof( value );
			if ( !strcmp( name, "occlusionWeightingTimeConstant" ) && !strcmp( section, "GraspTrackers" ) ) instance->occlusionWeightingTimeConstant = atof( value );
			return 1;
		}


		bool Initialize( void );
		bool  Update( void );
		bool Release( void );
		bool GetCurrentPoseIntrinsic( PsyPhy::TrackerPose &pose );

};

};


