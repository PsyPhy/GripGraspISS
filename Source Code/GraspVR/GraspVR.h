// GraspVR.h

#pragma once
#include "GraspGLObjects.h"
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

		static double mouseGain;
		static Pose handPoseVV;
		static double arrowGain;
		static Pose chestPoseSim;

		double desiredHeadRoll;
		Quaternion desiredHeadOrientation;
		double desiredHandRoll;
		Quaternion desiredHandOrientation;

		OculusViewpoint *viewpoint;

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;

		GraspGLObjects	*renderer;

		GraspVR( void )  : desiredHeadRoll(20.0), desiredHandRoll(-35.0) {}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );

		virtual void InitializeVR( HINSTANCE hinst );
		virtual void InitializeTrackers( void ) = 0;
		virtual void UpdateTrackers( void );
		virtual void Release( void );

		ProjectileState HandleProjectiles( void );
		double SetDesiredHeadRoll( double roll_angle );
		Alignment HandleHeadAlignment( void );
		double SetDesiredHandRoll( double roll_angle );
		Alignment HandleHandAlignment( void );

		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );

	};

	class GraspSIM : public GraspVR {
		public:
			virtual void InitializeTrackers( void );
			GraspSIM( void ) {}
			~GraspSIM( void ) {}
	};



	class GraspDEX : public GraspVR {

	protected:

		static const int nMarkers = 24;
		static const int nCodaUnits = 2;


		// A device that records 3D marker positions.
		// Those marker positions will also drive the 6dof pose trackers.
		CodaRTnetTracker codaTracker;

		// Buffers to hold the most recent frame of marker data.
		MarkerFrame markerFrame[MAX_CASCADED_TRACKERS];

		// CodaPoseTrackers compute the pose from a frame of Coda data and a rigid body model.
		// We use one for each Coda unit, but we could use more.
		CodaPoseTracker *hmdCodaPoseTracker[MAX_CASCADED_TRACKERS];
		CodaPoseTracker *handCodaPoseTracker[MAX_CASCADED_TRACKERS];
		CodaPoseTracker *chestCodaPoseTracker[MAX_CASCADED_TRACKERS];

		// CascadePoseTrackers group together multiple PoseTrackers for the same entity.
		// It sequentially tries to get the pose from one after another, stopping as soon as it gets one.
		// We use this mechanism to combine the pose info from each coda unit, because it it better to
		// compute the pose from markers positions measured by the same coda unit. Priority is therefore
		// given to the first coda in the list. Others are used only as needed.
		CascadePoseTracker *hmdCascadeTracker;
		CascadePoseTracker *handCascadeTracker;
		CascadePoseTracker *chestCascadeTracker;

		// For the HMD we can combine pose information from both the HMD and a Coda tracker.
		OculusCodaPoseTracker *oculusCodaPoseTracker;

		virtual void InitializeTrackers( void );
		virtual void UpdateTrackers( void );
		virtual void Release( void );

		GraspDEX( void ) {}
		~GraspDEX( void ) {}
	};

	class GraspVV : public GraspDEX {
		public:
			virtual void InitializeTrackers( void );
			GraspVV( void ) {}
			~GraspVV( void ) {}
	};
	class GraspVK : public GraspDEX {
		public:
			virtual void InitializeTrackers( void );
			GraspVK( void ) {}
			~GraspVK( void ) {}
	};

}

