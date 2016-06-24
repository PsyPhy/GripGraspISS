/*********************************************************************************/
/*                                                                               */
/*                               MousePoseTrackers.h                             */
/*                                                                               */
/*********************************************************************************/

// Use the mouse to drive a pose in various ways.

#pragma once

#include "../Trackers/PoseTrackers.h"

namespace PsyPhy {

// A base class on which to build all sorts of mouse-based trackers.
class MousePoseTracker : public PoseTracker {

	private:

	protected:
		OculusMapper *oculusMapper;
		double gain;

	public:

		MousePoseTracker( OculusMapper *ptr = nullptr, double gain = 1.0 );
		~MousePoseTracker() {}

		virtual bool GetCurrentPoseIntrinsic( TrackerPose *pose ) { return true; }
		virtual double GetTime( void );

};

// A mouse tracker that controls roll movements only.
// Moving the mouse toward the right or upwards will cause a counter-clockwise rotation around k.
// Moving along the diagonal will change the orientation most rapidly.
// You can change the gain and change the direction by setting the gain to be negative.
class MouseRollPoseTracker : public MousePoseTracker {
public:
		Vector3 eulerAngles;
		MouseRollPoseTracker( OculusMapper *ptr = nullptr, double gain = 1.0 );
		~MouseRollPoseTracker();
		bool Update( void );
		bool GetCurrentPoseIntrinsic( TrackerPose &pose );
};

// A  tracker that controls roll movements using the arrow keys on the keyboard.
// It is not really a mouse tracker, but this is a quick way to implement it.
class ArrowsRollPoseTracker : public MousePoseTracker {

private:
		double roll_angle;
public:
		ArrowsRollPoseTracker ( OculusMapper *ptr = nullptr, double gain = 1.0 );
		~ArrowsRollPoseTracker();
		bool Update( void );
		bool GetCurrentPoseIntrinsic( TrackerPose &pose );
};

};


