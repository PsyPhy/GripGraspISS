// GraspVR.h

#pragma once

#include "../OculusInterface/OculusInterface.h"

#include "../Useful/ini.h"

#include "GraspGLObjects.h"
#include "GraspTrackers.h"
#include "GraspDisplays.h"

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
		static int secondsToBeGood;		// Number of cycles that the head alignment has to be within tolerance to be considered good.
		static int secondsToBeBad;		// Number of cycles that the head alignment has to be within tolerance to be considered good.
		static int handSecondsToBeGood; // Number of cycles that the hand alignment has to be within tolerance to be considered good.
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

		Transform	localAlignment; 

		static double near_clipping;
		static double far_clipping;

		static double projectile_speed;
		static double prompt_spin_speed;

	public:

		// Flags that determine different modes.
		bool		showGoodAiming;
		bool		stopCheating;
		bool		noLasers;
		bool		snuffLaser;
		bool		balloon;
		double		balloonInflationRate;

	public:

		GraspDisplay	*display;
		Viewpoint		*viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		double conflictGain;
		static double chestOffset;
		static double viewpointOffset;
		static double interpupillary_distance;


		GraspVR( char *ini_filename )  : 

			display( nullptr ),
			trackers( nullptr ),

			desiredHeadRoll( 20.0 ), 
			desiredHandRoll( -35.0 ),
			currentProjectileState( cocked ),

			laserTargetingActive( false ),
			laserTargetingAcquired( false ),
			showGoodAiming( false ),
			stopCheating( true ),
			snuffLaser( false ),
			noLasers( false ),
			balloon( false ),
			balloonInflationRate( 20.0 ),

			conflictGain( 1.0 )

			{
				if ( ini_filename ) {
					fOutputDebugString( "GraspVR (%p): Parsing %s.\n", this, ini_filename );
					int error = ini_parse( ini_filename, iniHandler, this );
					if ( error != 0 ) fOutputDebugString( "GraspVR: Parsing error (%d).\n", error );
				}
				renderer = new GraspGLObjects( ini_filename );
			}

		bool	laserTargetingActive;
		bool	laserTargetingAcquired;

		void StartHandAiming ( void ) {
			if ( !noLasers ) renderer->handLaser->Enable();
			renderer->aimingErrorSphere->Disable();
			renderer->positionOnlyTarget->Enable();
			if ( balloon ) {
				laserTargetingActive = true;
				laserTargetingAcquired = false;
			}
			else laserTargetingActive = false;
		}
		void StopHandAiming( void ) {
			laserTargetingActive = false;
			renderer->aimingErrorSphere->Disable();
		}

		void Initialize( GraspDisplay *dsply, GraspTrackers *trkrs ) {
				display = dsply;
				trackers = trkrs;
				InitializeVR();
				InitializeTrackers();
		}
		void Release( void );
		~GraspVR( void ) {}

		// Create the necessary VR objects.
		virtual void InitializeVR( void );

		// Initialize the chosen set of tracker.
		virtual void InitializeTrackers( void );
		// Allow each tracker to update during a rendering loop.
		virtual void UpdateTrackers( void );
		// Clean up when finished.
		virtual void ReleaseTrackers( void );
		// Set the local transform to center the world at the current HMD 
		//  position and orientation.
		void AlignToHMD( void );
		// Undo the above transformation to align with the CODA reference frame.
		void AlignToCODA( void );

		// Draw everything and output to the display.
		void GraspVR::Render( void );

		// A rendering loop that allows one to toggle on and off the various VR objects
		// using key presses, used mostly for debugging.
		void DebugLoop( void );

	// 
	//	State information and routines to handle behaviors from inside a rendering loop.
	//

	public:

		// A common routine to compute the roll angle of an object.
		double ObjectRollAngle( OpenGLObject * object );

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
		static double	straightAheadThreshold;
		static double	pointingThreshold;

		AlignmentStatus	HandleHeadAlignment( bool use_arrow );
		AlignmentStatus HandleHeadOnShoulders( bool use_arrow );
		AlignmentStatus HandleGazeDirection( void );

		// We want prompts to spin to avoid providing an implicit reference frame by text prompts.
		void			HandleSpinningPrompts( void );

		// Hand the visibility and diffusion of the laser pointers.
		void			HandleLasers( void );

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

		// Provide the means to read a .ini file to set configuration parameters.
		// This is defined here as static because its address is sent as a callback to a parsing routine.
		static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			GraspVR *instance = (GraspVR *) which_instance;
			if ( !strcmp( name, "IPD" ) && !strcmp( section, "GraspVR" ) ) instance->interpupillary_distance = atof( value );
			else if ( !strcmp( name, "chestOffset" ) && !strcmp( section, "GraspVR" ) ) instance->chestOffset = atof( value );
			else if ( !strcmp( name, "viewpointOffset" ) && !strcmp( section, "GraspVR" ) ) instance->viewpointOffset = atof( value );
			else if ( !strcmp( name, "armRaisedThreshold" ) && !strcmp( section, "GraspVR" ) ) instance->armRaisedThreshold = atof( value );
			else if ( !strcmp( name, "straightAheadThreshold" ) && !strcmp( section, "GraspVR" ) ) instance->straightAheadThreshold = atof( value );
			else if ( !strcmp( name, "pointingThreshold" ) && !strcmp( section, "GraspVR" ) ) instance->pointingThreshold = atof( value );
			else if ( !strcmp( name, "useLaserEndpoint" ) && !strcmp( section, "GraspVR" ) ) instance->stopCheating = ( *value == 'T' || *value == 't' );
			else if ( !strcmp( name, "showGoodAiming" ) && !strcmp( section, "GraspVR" ) ) instance->showGoodAiming = ( *value == 'T' || *value == 't' );
			else if ( !strcmp( name, "extinguishLaser" ) && !strcmp( section, "GraspVR" ) ) instance->snuffLaser = ( *value == 'T' || *value == 't' );
			else if ( !strcmp( name, "balloonMask" ) && !strcmp( section, "GraspVR" ) ) instance->balloon = ( *value == 'T' || *value == 't' );
			else if ( !strcmp( name, "noLasers" ) && !strcmp( section, "GraspVR" ) ) instance->noLasers = ( *value == 'T' || *value == 't' );
			else return 1;
			fOutputDebugString( "GraspVR: Hit when parsing .ini file: section = %s name = %s value = %s\n", section, name, value );
			return 1;
		}
	};

}



