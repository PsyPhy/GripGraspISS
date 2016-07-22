// GraspVR.h

#pragma once
#include "GraspGLObjects.h"
#include "GraspTrackers.h"

#include "../DexServices/DexServices.h"

#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"

#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

namespace Grasp {

	// Possible state of head or hand alignment with reference.
	typedef enum { aligned, misaligned, transitioningToGood, transitioningToBad } AlignmentStatus;
	typedef enum { raised, lowered } ArmStatus;
			
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

	protected:

		// Hold the poses.
		TrackerPose headPose;
		TrackerPose chestPose;
		TrackerPose handPose;
		TrackerPose rollPose;

		static double handFilterConstant;
		static double interpupillary_distance;
		static double near_clipping;
		static double far_clipping;

		static double projectile_speed;
		static double prompt_spin_speed;


	public:

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		OculusViewpoint *viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		Transform	localAlignment; 

		GraspVR( void )  : 

			hInstance( nullptr ),
			oculusDisplay( nullptr ),
			oculusMapper( nullptr ),
			trackers( nullptr ),

			desiredHeadRoll( 20.0 ), 
			desiredHandRoll( -35.0 ),
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
		virtual void InitializeTrackers( const char *filename_root = nullptr );
		// Allow each tracker to update during a rendering loop.
		virtual void UpdateTrackers( void );
		// Clean up when finished.
		virtual void ReleaseTrackers( void );
		// Set the local transform to center the world at the current HMD 
		//  position and orientation.
		void AlignToHMD( void );

		// Draw everything and output to the display.
		void GraspVR::Render( void );

		// A rendering loop that allows one to toggle on and off the various VR objects
		// using key presses, used mostly for debugging.
		void DebugLoop( void );

	// 
	//	State information and routines to handle behaviors from inside a rendering loop.
	//

	protected:

		// Prompt the subject to raise or lower the arm.
		static double	armRaisedThreshold;					// Essentially the cosine of the angle of the cone of acceptance for raised.
		ArmStatus		HandleHandElevation( void );		// On each iteration of the rendering loop update the feedback.

		// Prompt the subject to achieve the desired hand orientation.
		double			desiredHandRoll;				// Easiest to specify this in a single Roll angle.
		static double	desiredHandRollSweetZone;
		static double	desiredHandRollTolerance;		
		AlignmentStatus	HandleHandAlignment( bool use_arrow );	// On each iteration of the rendering loop update the feedback.

		// Prompt the subject to achieve the desired head orientation.
		double			desiredHeadRoll;
		static double	desiredHeadRollSweetZone;
		static double	desiredHeadRollTolerance;		
		AlignmentStatus	HandleHeadAlignment( bool use_arrow );
		static double	straightAheadThreshold;
		AlignmentStatus HandleHeadOnShoulders( bool use_arrow );

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



