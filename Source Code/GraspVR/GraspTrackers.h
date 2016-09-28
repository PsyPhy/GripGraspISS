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
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaRTnetNullTracker.h"

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../Trackers/PoseTrackerFilter.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/MousePoseTrackers.h"
#include "../Trackers/OculusRemotePoseTracker.h"

#include "../DexServices/DexServices.h"

#define MAX_ISI 100000

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
		virtual bool GetCurrentHMDPose( TrackerPose &pose ) {
			return( hmdTracker->GetCurrentPose( pose ) );
		}
		virtual bool GetCurrentHandPose( TrackerPose &pose ) {
			return( handTracker->GetCurrentPose( pose ) );
		}
		virtual bool GetCurrentChestPose( TrackerPose &pose ) {
			return( chestTracker->GetCurrentPose( pose ) );
		}
		virtual unsigned int GetTrackerStatus( void ) {
			return( TRACKERSTATUS_UNKNOWN );
		}


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

	public:

		int nMarkers;
		int nCodaUnits;

		// A device that records 3D marker positions.
		// Those marker positions will also drive the 6dof pose trackers.
		CodaRTnetTracker *codaTracker;

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

		// Filtered versions of the coda-based trackers.
		PoseTrackerFilter *hmdFilteredTracker;
		PoseTrackerFilter *handFilteredTracker;
		PoseTrackerFilter *chestFilteredTracker;

		void GetMarkerData( void );
		void UpdatePoseTrackers( void );

	public:
		GraspDexTrackers ( CodaRTnetTracker *tracker = nullptr, PoseTracker *roll = nullptr ) : threadHandle( nullptr ) {
			nMarkers = 24;
			nCodaUnits = 2;
			codaTracker = tracker;
			rollTracker = roll;
		}
		virtual void Initialize( void );
		virtual void InitializeCodaTrackers( void );
		virtual unsigned int GetTrackerStatus( void );
		virtual void Update( void );
		virtual void Release( void );
		virtual void WriteDataFiles( char *filename_root );

		~GraspDexTrackers( void ) {}

	private:

		// Polling the CODA in the rendering loop can cause non-smooth updating.
		// We use a thread to get the CODA pose data in the background.
		// The following provides the means to use a thread.

		bool			requestSharedMemoryParent;
		bool			requestSharedMemoryChild;
		bool			parentHasPriority;

		MarkerFrame		sharedMarkerFrame[MAX_UNITS];
		bool			stopMarkerGrabs;
		HANDLE			threadHandle;
		DWORD			threadID;

	protected:

		// A static function that will be run in a separate thread.
		// It sits in the background and acquires the latest marker frames from CODA.
		static DWORD WINAPI GetCodaMarkerFramesInBackground( LPVOID prm ) {

#ifdef _DEBUG
			// A buffer to hold sample times, used to debug loop times.
			static double	isi[MAX_ISI];
			unsigned long	n_isi;
			n_isi = 0;
#endif
			// The function passed to a new thread takes a single pointer as an argument.
			// The GraspDexTrackers instance that creates the thread passes a pointer to itself 
			//  as the argument and here we cast it back to a GraspDexTrackers pointer.
			GraspDexTrackers *caller = (GraspDexTrackers *)prm;

			// A buffer to hold one frame of marker data.
			MarkerFrame localFrame;

			// A cycle counter and timer just for debugging.
			int count = 0;
			Timer timer;
			TimerStart( timer );

			// Loop until the main thread tells us to stop.
			while ( !caller->stopMarkerGrabs ) {

				for ( int unit = 0; unit < caller->nCodaUnits; unit++ ) {

					// Get the latest marker data into a local frame buffer.
					caller->codaTracker->GetCurrentMarkerFrameUnit( localFrame, unit );
					// Copy it to a shared buffer, being careful not to allow simultaneous access.
					caller->requestSharedMemoryChild = true;
					caller->parentHasPriority = true;
					while ( caller->requestSharedMemoryParent && caller->parentHasPriority ) Sleep( 1 );
					memcpy( &caller->sharedMarkerFrame[unit], &localFrame, sizeof( caller->sharedMarkerFrame[unit] ) );
					caller->requestSharedMemoryChild = false;

				}
				if ( !( count % 1000 ) ) fOutputDebugString( "Background cycle count: %d %.3f\n", count, TimerElapsedTime( timer ) );
				count++;
#ifdef _DEBUG
				isi[n_isi] = TimerElapsedTime( timer );
				if ( n_isi < MAX_ISI ) n_isi++;
#endif
				Sleep( 1 );
			}
			OutputDebugString( "GetCodaMarkerFramesInBackground() thread exiting.\n" );
#ifdef _DEBUG
			fOutputDebugString( "GraspTrackers Updates: %d of %d\n", n_isi, MAX_ISI );
			for ( unsigned long i = 1; i < n_isi; i++ ) {
				double delta;
				if ( ( delta = isi[i] - isi[i-1] ) >= 0.002 ) fOutputDebugString( "%d %.3f\n", i, isi[i] - isi[i-1] );
			}
#endif
			return NULL;
		}

		void GetMarkerFrameFromBackground( int unit, MarkerFrame *destination ) {

			requestSharedMemoryParent = true;
			parentHasPriority = false;
			while ( requestSharedMemoryChild && !parentHasPriority ) Sleep( 1 );
			memcpy( destination, &sharedMarkerFrame[unit], sizeof( *destination ) );
			requestSharedMemoryParent = true;

		}

	};

	class GraspOculusCodaTrackers : public GraspDexTrackers {
	public:
		// For the HMD we can combine pose information from both the HMD and a Coda tracker.
		OculusCodaPoseTracker *oculusCodaPoseTracker;
		GraspOculusCodaTrackers( void ) {}
		GraspOculusCodaTrackers( OculusMapper *mapper, CodaRTnetTracker *tracker ) {
			oculusMapper = mapper;
			codaTracker = tracker;
			GraspOculusCodaTrackers();
		}
		void GraspOculusCodaTrackers::Initialize( void );
	};

};







