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

	// Possible state of head or hand alignment with reference.
	typedef enum { aligned, misaligned, transitioningToGood, transitioningToBad } AlignmentStatus;
			
	// Possible states of the projectiles.
	typedef enum { cocked, triggered, running, hit, miss } ProjectileState;

	class GraspVR : public VectorsMixin
	{

	private:

		// Count down of how many cycles that the orientation has been good.
		static const int secondsToBeGood; // Number of cycles that the head alignment has to be within tolerance to be considered good.
		static const int secondsToBeBad; // Number of cycles that the head alignment has to be within tolerance to be considered good.
		Timer	headGoodTimer;
		Timer	headBadTimer;
		Timer	handGoodTimer;
		Timer	handBadTimer;

	public:

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		OculusViewpoint *viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		GraspVR( void )  : 

			hInstance( nullptr ),
			oculusDisplay( nullptr ),
			oculusMapper( nullptr ),
			trackers( nullptr ),

			desiredHeadRoll( 20.0 ), 
			desiredHeadRollSweetZone( 2.0 ),
			desiredHeadRollTolerance( 5.0 ),

			desiredHandRoll( -35.0 ),
			desiredHandRollSweetZone( 5.0 ),
			desiredHandRollTolerance( 5.0 ),

			currentProjectileState( cocked )

			{}

		void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs ) {
				hInstance = instance;
				oculusDisplay = display;
				oculusMapper = mapper;
				trackers = trkrs;
				InitializeVR( hInstance );
				InitializeTrackers();
		}
		void Release( void );
		~GraspVR( void ) {}

		// Create the necessary VR objects.
		virtual void InitializeVR( HINSTANCE hinst );

		// Initialize the chosen set of tracker.
		virtual void InitializeTrackers( void );
		// Allow each tracker to update during a rendering loop.
		virtual void UpdateTrackers( void );
		// Clean up when finished.
		virtual void ReleaseTrackers( void );

		// Draw everything and output to the display.
		void GraspVR::Render( void );

		// A rendering loop that allows one to toggle on and off the various VR objects
		// using key presses, used mostly for debugging.
		void DebugLoop( void );

	// 
	//	State information and routines to handle behaviors from inside a rendering loop.
	//

	protected:

		// Use color to guide the subject to a specified roll angle.
		static const double errorColorMapTransparency;
		static const double fadeDistance;
		void SetColorByRollError( OpenGLObject *object, double roll_error, double sweet_zone );

		// Prompt the subject to achieve the desired hand orientation.
		double			desiredHandRoll;				// Easiest to specify this in a single Roll angle.
		double			desiredHandRollSweetZone;
		double			desiredHandRollTolerance;		
		AlignmentStatus	HandleHandAlignment( bool use_arrow );	// On each iteration of the rendering loop update the feedback.

		// Prompt the subject to achieve the desired head orientation.
		double			desiredHeadRoll;
		double			desiredHeadRollSweetZone;
		double			desiredHeadRollTolerance;		
		AlignmentStatus	HandleHeadAlignment( bool use_arrow );

		// We want prompts to spin to avoid providing an implicit reference frame by text prompts.
		void			HandleSpinningPrompts( void );

		// Drives movements of the projectiles.
		Vector3			projectileDirection;
		ProjectileState	currentProjectileState;
		ProjectileState TriggerProjectiles( void );
		ProjectileState HandleProjectiles( void );

	public:

		// Set the parameters for the above behaviors. They are exposed to the public.
		double SetDesiredHandRoll( double desired_roll_angle, double tolerance );
		double SetDesiredHeadRoll( double desired_roll_angle, double tolerance );
		double SetTargetOrientation( double roll_angle );

	};

}



