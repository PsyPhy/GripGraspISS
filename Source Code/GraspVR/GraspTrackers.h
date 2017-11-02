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
#include "../Useful/ini.h"

#include "../OpenGLObjects/OpenGLWindows.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaRTnetNullTracker.h"
#include "../Trackers/CodaLegacyPolledTracker.h"
#include "../Trackers/CodaLegacyContinuousTracker.h"

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/NullPoseTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../Trackers/PoseTrackerFilter.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/MousePoseTrackers.h"
#include "../Trackers/OculusRemotePoseTracker.h"
#include "../Trackers/WindowsMousePoseTrackers.h"

#include "../DexServices/DexServices.h"

#define MAX_ISI 100000

namespace Grasp {

	using namespace PsyPhy;

	// Grasp requires a set of trackers to follow the hmd, hand and chest, plus
	// a roll-only tracker used to orient the virtual hand in a visual response mode.
	// GraspTrackers defines the set of trackers and the functionality that is required
	// from those trackers. Dervied classes will then define the actual combinations 
	// of trackers to be used depending on the hardware.

	class GraspTrackers : public VectorsMixin {

	protected:
		static double	mouseGain;
		static double	arrowGain;
		static Pose		chestPoseSim;

	public: 

		static Pose		handPoseV;
		static Pose		handPoseK;

		// Buffers to hold the most recent frame of marker data.
		MarkerFrame markerFrame[MAX_UNITS];

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

		// The base class GraspTrackers takes care of writing out the pose data from each cycle.
		// Derived classes are given the chance to add additional columns to the data file
		// by overlaying the following two routines. By default, they do nothing.
		virtual void WriteAdditionalColumnHeadings( FILE *fp ) {}
		virtual void WriteAdditionalTrackerData( FILE *fp ){}

	protected:
		~GraspTrackers() {}

	};

	// GraspSimulatedTrackers are intended to provide simulated movements of the 
	// individual trackers without any tracking hardware.
	class GraspSimulatedTrackers : public GraspTrackers {
	protected:
		OpenGLWindow	*window;

	public:
		virtual void Initialize( void );
		GraspSimulatedTrackers( void ) {}
		GraspSimulatedTrackers( OpenGLWindow *window ) {
			this->window = window;
		}
		~GraspSimulatedTrackers( void ) {}
	};

	// GraspDexTrackers is any set of trackers that relies on the DEX hardware.
	// It can be used by itself to do marker-only tracking, but it is more
	// likely to be part of a derived class that combines marker data with the 
	// Oculus inertial data.
	class GraspDexTrackers : public GraspTrackers {

	public:

		int nMarkers;
		int nCodaUnits;
		int headCodaCascade;
		// A device that records 3D marker positions.
		// Those marker positions will also drive the 6dof pose trackers.
		Tracker *codaTracker;

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
		GraspDexTrackers( Tracker *tracker = nullptr, PoseTracker *roll = nullptr ) : headCodaCascade( -1 ) {

#ifdef BACKGROUND_GET_DATA
			threadHandle = nullptr;
#endif

				nMarkers = 24;
				nCodaUnits = 2;
				codaTracker = tracker;
				rollTracker = roll;
				int error = ini_parse( "Grasp.ini", iniHandler, this );
				if ( error != 0 ) fOutputDebugString( "GraspTrackers: Parsing error (%d).\n", error );
			}
		// Provide the means to read a .ini file to set configuration parameters.
		// This is defined here as static because its address is sent as a callback to a parsing routine.
		static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			GraspDexTrackers *instance = (GraspDexTrackers *) which_instance;
			if ( !strcmp( name, "headCodaCascade" ) && !strcmp( section, "GraspTrackers" ) ) instance->headCodaCascade = atof( value );
			return 1;
		}
		virtual void Initialize( void );
		virtual void InitializeCodaTrackers( void );
		virtual unsigned int GetTrackerStatus( void );
		virtual void Update( void );
		virtual void Release( void );

		void WriteAdditionalColumnHeadings( FILE *fp );
		void WriteAdditionalTrackerData( FILE *fp );

		~GraspDexTrackers( void ) {}

	private:
#ifdef BACKGROUND_GET_DATA
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
#endif

	};

	// GraspOculusCodaTrackers is nominally the best tracker set for Grasp.
	// It uses a combined Oculus-Coda tracker for the HMD and marker-based Pose
	// trackers for the chest and hand.
	class GraspOculusCodaTrackers : public GraspDexTrackers {

	protected:
		OculusMapper	*oculusMapper;

	public:
		// For the HMD we can combine pose information from both the HMD and a Coda tracker.
		OculusCodaPoseTracker *oculusCodaPoseTracker;
		GraspOculusCodaTrackers( void ) {}
		GraspOculusCodaTrackers( OculusMapper *mapper, CodaRTnetTracker *tracker ) {
			oculusMapper = mapper;
			codaTracker = tracker;
			GraspOculusCodaTrackers();
		}
		void Initialize( void );
	};

	// We use this one when we want to pretend that we have CODAs.
	class GraspOculusTrackers : public GraspTrackers {
	protected:
		OculusMapper	*oculusMapper;

	public:
		PoseTracker *chestTrackerRaw;
		GraspOculusTrackers( void ) {}
		GraspOculusTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
			GraspOculusTrackers();
		}
		void Initialize( void );
	};


	// GraspOculusLiteTrackers provides a solution when the Coda is not available
	// and when we don't have the Oculus touch sensors.
	// It uses the standard Oculus tracker with its own constellation system
	//  for drift correction, and substitutes mouse and keyboard trackers for
	// the chest and hand. Perhaps a future version could use the Oculus hand controllers.
	class GraspOculusLiteTrackers : public GraspTrackers {
	protected:
		OculusMapper	*oculusMapper;

	public:
		PoseTracker *chestTrackerRaw;
		void Initialize( void );
		GraspOculusLiteTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
		}
		~GraspOculusLiteTrackers( void ) {}
	};


};



