// GraspVR.h

#pragma once
#include "GraspGLObjects.h"

namespace Grasp {
			
	// Possible states of the projectiles.
	typedef enum { cocked, running, hit, miss } ProjectileState;
	// Possible state when driving toward a desired orientation.
	typedef enum { aligned, misaligned } Alignment;


	class GraspVR : public VectorsMixin
	{

	private:

	public:

		static double mouseGain;
		static Pose handPoseVV;
		static double arrowGain;
		static Pose chestPoseSim;

		double desiredHeadRoll;
		double desiredHandRoll;

		OculusViewpoint *viewpoint;

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;

		GraspGLObjects	*renderer;

		GraspVR( void )  : desiredHeadRoll(20.0), desiredHandRoll(-35.0) {}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );

		virtual void InitializeVR( HINSTANCE hinst );
		virtual void InitializeTrackers( void ) ;
		void UpdateTrackers( void );
	
		ProjectileState HandleProjectiles( void );
		double SetDesiredHeadRoll( double roll_angle );
		Alignment HandleHeadAlignment( void );
		double SetDesiredHandRoll( double roll_angle );
		Alignment HandleHandAlignment( void );

		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );
		void Release( void );

	};
}
