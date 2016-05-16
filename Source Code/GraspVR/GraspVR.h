// GraspVR.h

#pragma once
#include "GraspGLObjects.h"

namespace Grasp {
			
	// Possible states of the projectiles.
	typedef enum { cocked, running, hit, miss } ProjectileState;

	class GraspVR : public VectorsMixin
	{

	private:

	public:

		static const double mouseGain;
		static Pose trackerPoseVV;

		OculusViewpoint *viewpoint;

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;

		GraspGLObjects	*renderer;

		GraspVR( void ) {}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );

		virtual void InitializeVR( HINSTANCE hinst );
		virtual void InitializeTrackers( void ) ;
		void UpdateTrackers( void );
	
		ProjectileState HandleProjectiles( void );
		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );
		void Release( void );

	};
}
