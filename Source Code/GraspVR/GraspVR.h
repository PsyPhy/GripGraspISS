// GraspVR.h

#pragma once
#include "GraspGLObjects.h"
#include "GraspTrackers.h"

#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

namespace Grasp {
			
	// Possible states of the projectiles.
	typedef enum { cocked, triggered, running, hit, miss } ProjectileState;
	// Possible state when driving toward a desired orientation.
	typedef enum { aligned, misaligned } Alignment;


	class GraspVR : public VectorsMixin
	{


	public:



		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		OculusViewpoint *viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		GraspVR( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs  )  : 
			desiredHeadRoll( 20.0 ), 
			desiredHandRoll( -35.0 ),
			currentProjectileState( cocked )
			{
				hInstance = instance;
				oculusDisplay = display;
				oculusMapper = mapper;
				trackers = trkrs;
			}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );
		void Release( void );

		// Create the necessary VR objects.
		virtual void InitializeVR( HINSTANCE hinst );

		// Initialize the chosen set of tracker.
		virtual void InitializeTrackers( void );
		// Allow each tracker to update during a rendering loop.
		virtual void UpdateTrackers( void );
		// Clean up when finished.
		virtual void ReleaseTrackers( void );

		// A rendering loop that allows one to toggle on and off the various VR objects
		// using key presses, used mostly for debugging.
		void DebugLoop( void );

	// 
	//	State information and routines to handle behaviors from inside a rendering loop.
	//

	private:

		// Prompt the subject to achieve the desired hand orientation.
		double			desiredHandRoll;				// Easiest to specify this in a single Roll angle.
		Quaternion		desiredHandOrientation;			// Converted into a quaternion for convenience.
		Alignment		HandleHandAlignment( void );	// On each iteration of the rendering loop update the feedback.

		double			desiredHeadRoll;
		Quaternion		desiredHeadOrientation;
		Alignment		HandleHeadAlignment( void );

		// Drives movements of the projectiles.

		ProjectileState	currentProjectileState;
		ProjectileState TriggerProjectiles( void );
		ProjectileState HandleProjectiles( void );

	public:

		// Set the parameters for the above behaviors. They are exposed to the public.
		double SetDesiredHandRoll( double roll_angle );
		double SetDesiredHeadRoll( double roll_angle );

	};

}



