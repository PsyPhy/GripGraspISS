///
/// Module:	GraspTrackers
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		30 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#pragma once

// There is an incompatibilty between the M macro defined in Useful.h that is
// used to access the real part of a Quaternion and the Oculus library.
// So the following header file must come before Useful.h. This also 
// implies that this include file must come before Useful.h in any file that 
// uses them both.
#include "../OculusInterface/OculusInterface.h"

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaRTnetTracker.h"

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../Trackers/PoseTrackerFilter.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/MousePoseTrackers.h"

namespace Grasp {

	using namespace PsyPhy;

	class GraspTrackers : public VectorsMixin {

	protected:
		static double	mouseGain;
		static Pose		handPoseV;
		static Pose		handPoseK;
		static double	arrowGain;
		static Pose		chestPoseSim;
		OculusMapper	*oculusMapper;

	public: 

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;
		PoseTracker *rollTracker;

		GraspTrackers() {}
		virtual void Initialize( void ) = 0;
		virtual void Update( void );
		virtual void Release( void );
		virtual void WriteDataFiles( char *filename_root ){};

	protected:
		~GraspTrackers() {}

	};

	class GraspSimTrackers : public GraspTrackers {
	public:

		// We will need a mouse tracker of some kind to do the V-V protocol.
		MouseRollPoseTracker *mouseRollTracker;

		PoseTracker *chestTrackerRaw;

		virtual void Initialize( void );
		GraspSimTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
		}
		~GraspSimTrackers( void ) {}
	};


	class GraspDexTrackers : public GraspTrackers {

	protected:

		int nMarkers;
		int nCodaUnits;

		// A device that records 3D marker positions.
		// Those marker positions will also drive the 6dof pose trackers.
		CodaRTnetTracker codaTracker;

		// Buffers to hold the most recent frame of marker data.
		MarkerFrame markerFrame[MAX_UNITS];

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

		// For the other two coda-only tracker we probably need to filter.
		PoseTrackerFilter *handFilteredTracker;
		PoseTrackerFilter *chestFilteredTracker;

		// We will need a mouse tracker of some kind to do the V-V protocol.
		MouseRollPoseTracker *mouseRollTracker;
		// Or we can use the Oculus remote.
		OculusRemoteRollPoseTracker *oculusRollTracker;

	public:

		void Initialize( void );
		void Update( void );
		void Release( void );
		void WriteDataFiles( char *filename_root );

		GraspDexTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
			nMarkers = 24;
			nCodaUnits = 2;
		}
		~GraspDexTrackers( void ) {}

	private:

		// Polling the CODA in the rendering loop can cause non-smooth updating.
		// We use a thread to get the CODA pose data in the background.
		// The following provides the means to use a thread.

		bool			lockSharedMarkerFrame[MAX_UNITS];
		MarkerFrame		sharedMarkerFrame[MAX_UNITS];
		bool			stopMarkerGrabs;
		HANDLE			threadHandle;
		DWORD			threadID;

		// A static function that will be run in a separate thread.
		// It sits in the background and acquires the latest marker frames from CODA.
		static DWORD WINAPI GetCodaMarkerFramesInBackground( LPVOID prm ) {

			// The function passed to a new thread takes a single pointer as an argument.
			// The GraspDexTrackers instance that creates the thread passes a pointer to itself 
			//  as the argument and here we cast it back to a GraspDexTrackers pointer.
			GraspDexTrackers *caller = (GraspDexTrackers *)prm;

			// A buffer to hold one frame of marker data.
			MarkerFrame localFrame;

			// Loop until the main thread tells us to stop.
			while ( !caller->stopMarkerGrabs ) {
				for ( int unit = 0; unit < caller->nCodaUnits; unit++ ) {
					// Get the latest marker data into a local frame buffer.
					caller->codaTracker.GetCurrentMarkerFrameUnit( localFrame, unit );
					// Copy it to a shared buffer, being careful not to allow simultaneous access.
					while ( caller->lockSharedMarkerFrame[unit] );
					caller->lockSharedMarkerFrame[unit] = true;
					memcpy( &caller->sharedMarkerFrame[unit], &localFrame, sizeof( caller->sharedMarkerFrame[unit] ) );
					caller->lockSharedMarkerFrame[unit] = false;
				}
			}
			OutputDebugString( "GetCodaMarkerFramesInBackground() thread exiting.\n" );
			return NULL;
		}
		
		void GetMarkerFrameFromBackground( int unit, MarkerFrame *destination ) {
			while ( lockSharedMarkerFrame[unit] );
			lockSharedMarkerFrame[unit] = true;
			memcpy( destination, &sharedMarkerFrame[unit], sizeof( *destination ) );
			lockSharedMarkerFrame[unit] = false;
		}

	};

}


