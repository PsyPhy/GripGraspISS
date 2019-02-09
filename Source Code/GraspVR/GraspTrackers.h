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
#include "../Trackers/CodaRTnetPolledTracker.h"
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
	// of trackers to be used depending on the hardware that is available.

	class GraspTrackers : public VectorsMixin {

	protected:
		static double	mouseGain;
		static double	arrowGain;
		static Pose		chestPoseSim;
		const char		*ini_filename;

	public: 

		static Pose		handPoseV;
		static Pose		handPoseK;

		// Buffers to hold the most recent frame of marker data.
		MarkerFrame markerFrame[MAX_UNITS];

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;
		PoseTracker *rollTracker;

		GraspTrackers(  const char *ini_filename = nullptr ) {
			this->ini_filename = ini_filename;
		}
		// By convention, the constructor code allows one to parse a .ini file.
		// Provide a default callback for parsing an init file.
		// This one does nothing, but derived classes may have their own.
		static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			return 1;
		}
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

	//
	// GraspDexTrackers is any set of trackers that relies on the DEX hardware.
	// It can be used by itself to do marker-only tracking, but it is more
	// likely to be part of a derived class that combines marker data with the 
	// Oculus inertial data.
	//
	class GraspDexTrackers : public GraspTrackers {

	public:

		int nMarkers;
		int nCodaUnits;
		int hmdCodaCascade;
		int handCodaCascade;
		int chestCodaCascade;

		double chestFilterConstant;
		double handFilterConstant;
		double hmdFilterConstant;

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
		bool	cascadeRealignment;

		// Filtered versions of the coda-based trackers.
		PoseTrackerFilter *hmdFilteredTracker;
		PoseTrackerFilter *handFilteredTracker;
		PoseTrackerFilter *chestFilteredTracker;

		void GetMarkerData( void );
		void UpdatePoseTrackers( void );

	public:
		GraspDexTrackers( Tracker *tracker = nullptr, PoseTracker *roll = nullptr, const char *ini_filename = nullptr   ) : 

		  hmdCodaCascade( -1 ),
			  handCodaCascade( -1 ), 
			  chestCodaCascade( -1 ),
			  cascadeRealignment( false ),

			  chestFilterConstant( 10.0 ),
			  handFilterConstant( 2.0 ),
			  hmdFilterConstant( 2.0 ),

			  nMarkers( 24 ),
			  nCodaUnits( 2 )

		  {
			  codaTracker = tracker;
			  rollTracker = roll;
			  this->ini_filename = ini_filename;
			  // Read parameters from an init file, if one has been specified by the caller.
			  if ( ini_filename ) {
				  fOutputDebugString( "GraspDexTrackers: Parsing %s.\n", ini_filename );
				  int error = ini_parse( ini_filename, iniHandler, this );
				  if ( error != 0 ) fAbortMessage( "GraspDexTrackers", "Parsing error %d for file %s.\n", error, ini_filename  );
			  }
		  }
		  ~GraspDexTrackers( void ) {}

		  // Provide the means to read a .ini file to set configuration parameters.
		  // This is defined here as static because its address is sent as a callback to a parsing routine.
		  static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			  GraspDexTrackers *instance = (GraspDexTrackers *) which_instance;
			  if ( !strcmp( name, "hmdCodaCascade" ) && !strcmp( section, "GraspTrackers" ) ) instance->hmdCodaCascade = atoi( value );
			  else if ( !strcmp( name, "handCodaCascade" ) && !strcmp( section, "GraspTrackers" ) ) instance->handCodaCascade = atoi( value );
			  else if ( !strcmp( name, "chestCodaCascade" ) && !strcmp( section, "GraspTrackers" ) ) instance->chestCodaCascade = atoi( value );
			  else if ( !strcmp( name, "cascadeRealignment" ) && !strcmp( section, "GraspTrackers" ) ) instance->cascadeRealignment = ( value[0] == 't' || value[0] == 'T' );
			  else if ( !strcmp( name, "hmdFilterConstant" ) && !strcmp( section, "GraspTrackers" ) ) instance->hmdFilterConstant = atof( value );
			  else if ( !strcmp( name, "handFilterConstant" ) && !strcmp( section, "GraspTrackers" ) ) instance->handFilterConstant = atof( value );
			  else if ( !strcmp( name, "chestFilterConstant" ) && !strcmp( section, "GraspTrackers" ) ) instance->chestFilterConstant = atof( value );
			  else return 1;
			  // If we get to here, there was a match in the list above.
			  // So show what was found when debugging.
			  fOutputDebugString( "--GraspDexTrackers: Hit when parsing .ini file: section = %s name = %s value = %s\n", section, name, value );
			  return 1;
		  }
		  virtual void Initialize( void );
		  virtual void InitializeCodaTrackers( void );
		  virtual unsigned int GetTrackerStatus( void );
		  virtual void Update( void );
		  virtual void Release( void );

		  void WriteAdditionalColumnHeadings( FILE *fp );
		  void WriteAdditionalTrackerData( FILE *fp );


	private:


	};

	//
	// GraspOculusCodaTrackers is nominally the best tracker set for Grasp.
	// It uses a combined Oculus-Coda tracker for the HMD and marker-based Pose
	// trackers for the chest and hand.
	//
	class GraspOculusCodaTrackers : public GraspDexTrackers {

	protected:
		OculusMapper	*oculusMapper;

	public:
		// For the HMD we can combine pose information from both the HMD and a Coda tracker.
		OculusCodaPoseTracker *oculusCodaPoseTracker;
		GraspOculusCodaTrackers( OculusMapper *mapper, CodaRTnetTracker *tracker, const char *ini_filename ) {
			oculusMapper = mapper;
			codaTracker = tracker;
			this->ini_filename = ini_filename;
			// Read parameters from an init file, if one has been specified by the caller.
			if ( ini_filename ) {
				fOutputDebugString( "GraspOculusCodaTrackers: Parsing %s.\n", ini_filename );
				int error = ini_parse( ini_filename, iniHandler, this );
				if ( error != 0 ) fAbortMessage( "GraspDexTrackers", "Parsing error %d for file %s.\n", error, ini_filename  );
			}
		}
		void Initialize( void );
		void WriteAdditionalColumnHeadings( FILE *fp );
		void WriteAdditionalTrackerData( FILE *fp );
	};

	//
	// GraspOculusTrackers provides a solution when the Coda is not available.
	// It uses the standard Oculus tracker with its own constellation system
	//  for drift correction, and uses the Oculus hand controller for the 
	//  chest and the hand. 
	//
	class GraspOculusTrackers : public GraspTrackers {
	protected:
		OculusMapper	*oculusMapper;

	public:
		PoseTracker *chestTrackerRaw;
		GraspOculusTrackers( OculusMapper *mapper, const char *ini_filename ) {
			oculusMapper = mapper;
			this->ini_filename = ini_filename;
			// Read parameters from an init file, if one has been specified by the caller.
			if ( ini_filename ) {
				fOutputDebugString( "GraspOculusTrackers: Parsing %s.\n", ini_filename );
				int error = ini_parse( ini_filename, iniHandler, this );
				if ( error != 0 ) fAbortMessage( "GraspOculusTrackers", "Parsing error %d for file %s.\n", error, ini_filename  );
			}
		}
		void Initialize( void );
	};

	//
	// GraspOculusLiteTrackers provides a solution when the Coda is not available
	// and when we don't have the Oculus touch sensors.
	// It uses the standard Oculus tracker with its own constellation system
	//  for drift correction, and substitutes mouse and keyboard trackers for
	// the chest and hand. 
	//
	class GraspOculusLiteTrackers : public GraspTrackers {
	protected:
		OculusMapper	*oculusMapper;

	public:
		PoseTracker *chestTrackerRaw;
		GraspOculusLiteTrackers( OculusMapper *mapper, const char *ini_filename ) {
			oculusMapper = mapper;
			this->ini_filename = ini_filename;
			// Read parameters from an init file, if one has been specified by the caller.
			if ( ini_filename ) {
				fOutputDebugString( "GraspOculusLiteTrackers: Parsing %s.\n", ini_filename );
				int error = ini_parse( ini_filename, iniHandler, this );
				if ( error != 0 ) fAbortMessage( "GraspOculusLiteTrackers", "Parsing error %d for file %s.\n", error, ini_filename  );
			}
		}
		~GraspOculusLiteTrackers( void ) {}
		void Initialize( void );
	};

	//
	// GraspSimulatedTrackers are intended to provide simulated movements of the 
	// individual trackers without any tracking hardware.
	//
	class GraspSimulatedTrackers : public GraspTrackers {
	protected:
		OpenGLWindow	*window;

	public:
		GraspSimulatedTrackers( OpenGLWindow *window, const char *ini_filename ) {
			this->window = window;
			this->ini_filename = ini_filename;
			if ( ini_filename ) {
				fOutputDebugString( "GraspSimulatedTrackers: Parsing %s.\n", ini_filename );
				int error = ini_parse( ini_filename, iniHandler, this );
				if ( error != 0 ) fAbortMessage( "GraspSimulatedTrackers", "Parsing error %d for file %s.\n", error, ini_filename  );
			}
		}
		~GraspSimulatedTrackers( void ) {}
		void Initialize( void );
	};

};
