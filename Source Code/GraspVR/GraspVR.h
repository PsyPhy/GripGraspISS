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
	typedef enum { cocked, running, hit, miss } ProjectileState;
	// Possible state when driving toward a desired orientation.
	typedef enum { aligned, misaligned } Alignment;


	class GraspVR : public VectorsMixin
	{

	private:

	public:

		double desiredHeadRoll;
		Quaternion desiredHeadOrientation;
		double desiredHandRoll;
		Quaternion desiredHandOrientation;

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		OculusViewpoint *viewpoint;
		GraspGLObjects	*renderer;
		GraspTrackers	*trackers;

		GraspVR( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs  )  : 
			desiredHeadRoll(20.0), 
			desiredHandRoll(-35.0) {
				hInstance = instance;
				oculusDisplay = display;
				oculusMapper = mapper;
				trackers = trkrs;
			}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );
		void Release( void );

		virtual void InitializeVR( HINSTANCE hinst );
		virtual void InitializeTrackers( void );
		virtual void UpdateTrackers( void );
		virtual void ReleaseTrackers( void );

		ProjectileState HandleProjectiles( void );
		double SetDesiredHeadRoll( double roll_angle );
		Alignment HandleHeadAlignment( void );
		double SetDesiredHandRoll( double roll_angle );
		Alignment HandleHandAlignment( void );

		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );

	};

}



