// GraspVR.h

#pragma once

#include "../OculusInterface/OculusInterface.h"

#include "GraspGLObjects.h"
#include "GraspTrackers.h"

#include "../DexServices/DexServices.h"

#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"

#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/OculusViewpoint.h"

// Exit codes.
// These get passed back to the calling shell program, which then puts up 
//  a message page describing the error and possible actions to take.
// It it good if they can be unique.
#define NORMAL_EXIT		0
// Lots of routines and programs use -1 as a generic exit code, so I reseve it here just in case.
#define GENERIC_ERROR	-1
// The CODA tracker can exit directly with exit codes defined in that package.
// I should update the tracker package so that it returns an error code rather than exiting directly.
// Until then, the RTNET return codes should be set to the same value as TRACKER_ERROR in source code.
#define TRACKER_ERROR	-2
#define OCULUS_ERROR	-3
namespace Grasp {

	class GraspDisplay
	{

	public:

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		GraspDisplay( void )  : hInstance( nullptr ), oculusDisplay( nullptr ), oculusMapper( nullptr ) {}
		void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, HWND parentWindow );
		void Release( void );
		~GraspDisplay( void ) {}

		OculusViewpoint *GraspDisplay::CreateViewpoint( double ipd, double nearest, double farthest );
		void ApplyViewpoint( Viewpoint *viewpoint, Eye eye );
		void SelectEye( int eye );
		void DeselectEye( int eye );
		void Present( void );

		// Detect action of the subject to record a response.
		bool waitForUp;
		bool Validate( void );
		bool KeyDownEvents( int key );
		void ClearKeyDownEvents( void );

	};

}



