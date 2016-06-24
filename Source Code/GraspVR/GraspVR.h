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

	class GraspVR : public VectorsMixin
	{

	private:

		// Count down of how many cycles that the orientation has been good.
		static const int cyclesToBeGood; // Number of cycles that the head alignment has to be within tolerance to be considered good.
		int	headGoodCycles;
		int handGoodCycles;

	public:

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		OculusViewpoint *viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		GraspVR( void )  : 
			currentProjectileState( cocked ),
			hInstance( nullptr ),
			oculusDisplay( nullptr ),
			oculusMapper( nullptr ),
			desiredHeadRoll( 20.0 ), 
			desiredHeadRollSweetZone( 2.0 ),
			desiredHandRoll( -35.0 ),
			desiredHandRollSweetZone( 2.0 ),
			desiredHandRollTolerance( 5.0 ),
			handGoodCycles( cyclesToBeGood ),
			headGoodCycles( cyclesToBeGood ),
			trackers( nullptr ) {}

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
		bool SetColorByRollError( OpenGLObject *object, double desired_angle, double sweet_zone, double tolerance );

		// Prompt the subject to achieve the desired hand orientation.
		double			desiredHandRoll;				// Easiest to specify this in a single Roll angle.
		double			desiredHandRollSweetZone;
		double			desiredHandRollTolerance;		
		bool			HandleHandAlignment( void );	// On each iteration of the rendering loop update the feedback.

		// Prompt the subject to achieve the desired head orientation.
		double			desiredHeadRoll;
		double			desiredHeadRollSweetZone;
		double			desiredHeadRollTolerance;		
		bool			HandleHeadAlignment( void );

		// We want prompts to spin to avoid providing an implicit reference frame by text prompts.
		void			HandleSpinningPrompts( void );

		// Drives movements of the projectiles.
		ProjectileState	currentProjectileState;
		ProjectileState TriggerProjectiles( void );
		ProjectileState HandleProjectiles( void );

	public:

		// Set the parameters for the above behaviors. They are exposed to the public.
		double SetDesiredHandRoll( double roll_angle );
		double SetDesiredHeadRoll( double roll_angle, double tolerance );
		double SetTargetOrientation( double roll_angle );

	};

}



