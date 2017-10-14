// GraspDisplays.h

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
// It is good if they can be unique.
// These should probably be defined in a different place, because they are
//  not specific to GraspDisplays, but I don't know where yet.

// Normal exit typically returns a 0.
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

		GraspDisplay( void ) {}
		~GraspDisplay( void ) {}

		virtual void Initialize( void ){};
		virtual void Release( void ){};
		virtual Viewpoint *CreateViewpoint( double ipd, double nearest, double farthest ){ 
			return( new PsyPhy::Viewpoint() ); 
		}
		virtual void ApplyViewpoint( Viewpoint *viewpoint, Eye eye ){};
		virtual void SelectEye( int eye ){};
		virtual void DeselectEye( int eye ){};
		virtual bool HandleMessages( void ){ return false; }
		virtual void Present( void ){};

		// Detect action of the subject to record a response.
		virtual bool Validate( void ) { return false; }
		virtual bool KeyDown( int key ){ return false; }
		virtual bool KeyDownEvents( int key ){ return false; }
		virtual void ClearKeyDownEvents( void ){};

	};

	//
	// GraspOculusDisplay
	// A display that can drive the Oculus Rift.
	//
	class GraspOculusDisplay : public GraspDisplay
	{

	private:

		// Detect action of the subject to record a response.
		bool waitForUp;

	public:

		HINSTANCE			hInstance;
		OculusDisplayOGL	*oculusDisplay;
		OculusMapper		*oculusMapper;

		// Usually we mirror the Oculus display on the computer screen. But you may want to hide
		// what is going on in the HMD. To do so, set the following to false;
		bool mirror;


		GraspOculusDisplay( void )  : hInstance( nullptr ), oculusDisplay( nullptr ), oculusMapper( nullptr ), mirror( false ) {}
		~GraspOculusDisplay( void ) {}

		virtual void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, HWND parentWindow );
		virtual void Release( void );
		virtual OculusViewpoint *CreateViewpoint( double ipd, double nearest, double farthest );
		virtual void ApplyViewpoint( Viewpoint *viewpoint, Eye eye );
		virtual void SelectEye( int eye );
		virtual void DeselectEye( int eye );
		virtual bool HandleMessages( void );
		virtual void Present( void );

		virtual bool Validate( void );
		virtual bool KeyDownEvents( int key );
		virtual bool KeyDown( int key );
		virtual void ClearKeyDownEvents( void );

	};



	// 
	// GraspWindowsDisplay
	// VR is displayed only on the Windows computer.
	//
	class GraspWindowsDisplay : public GraspDisplay
	{

	private:

		// Detect action of the subject to record a response.
		bool waitForUp;

	public:

		bool			fullscreen;

		HINSTANCE		hInstance;
		OpenGLWindow	*window;

		GraspWindowsDisplay( void ) : fullscreen( true ) {}
		~GraspWindowsDisplay( void ) {}

		virtual void Initialize( HINSTANCE instance, HWND parentWindow );
		virtual void Release( void );
		virtual Viewpoint *CreateViewpoint( double ipd, double nearest, double farthest );
		virtual void ApplyViewpoint( Viewpoint *viewpoint, Eye eye );
		virtual void SelectEye( int eye );
		virtual void DeselectEye( int eye );
		virtual bool HandleMessages( void );
		virtual void Present( void );

		virtual bool Validate( void );
		virtual bool KeyDownEvents( int key );
		virtual void ClearKeyDownEvents( void );

	};
}

