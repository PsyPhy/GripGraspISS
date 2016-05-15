// GraspVR.h

#pragma once
#include "GraspGLObjects.h"

namespace Grasp {

	class GraspVR : public GraspGLObjects
	{

	private:

	public:

		static const double mouseGain;
		static Pose trackerPoseVV;

		OculusViewpoint *viewpoint;

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;

		GraspVR( void ) {}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );

		void InitializeVR( HINSTANCE hinst );
		void InitializeTrackers( void ) ;
		

		// Draw the objects that are used during VR.
		void Draw( void );

		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );
		void Release( void );

	};
}
