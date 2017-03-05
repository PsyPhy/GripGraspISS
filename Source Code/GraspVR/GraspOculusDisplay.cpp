
#include "stdafx.h"
#include "GraspDisplays.h"

using namespace Grasp;
using namespace PsyPhy;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// GraspOculusDisplay
//

void GraspOculusDisplay::Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, HWND parent_window ) {

	ovrResult result;

	hInstance = instance;
	oculusDisplay = display;
	oculusMapper = mapper;

	// Decide if we are in full screen mode or not.
	// To avoid losing focus by clicking outside the desktop window it is best to be in fullscreen mode.
	static const bool fullscreen = true;

	// Mirroring to the console is determined by the presence or absence of a cookie file.
	if ( FileExists( "Mirroring.flg" ) ) mirror = true;

	// Initializes LibOVR, and the Rift
	result = ovr_Initialize( nullptr );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspDisplay", "Error initializing VR Headset.\n - Failed to initialize libOVR (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

	// Initialize the Oculus-enabled Windows platform.
	result = oculusDisplay->InitWindow( instance, L"GraspDisplay", ( parent_window ? false : fullscreen ), parent_window );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspDisplay", "Error initializing VR Headset.\n - Failed to initialize oculusDisplay (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper->Initialize( oculusDisplay, ( parent_window ? true : mirror ) );
	if ( OVR_FAILURE( result ) ) {
		fMessageBox( MB_OK | MB_ICONERROR, "GraspDisplay", "Error initializing VR Headset.\n - Failed to initialize oculusMapper (ovrError %d).", result );
		exit( OCULUS_ERROR );
	}

	// Initialize state for button pushes.
	ovrInputState state;
	ovr_GetInputState(	oculusMapper->session,  ovrControllerType_Remote, &state );
	waitForUp = ( oculusDisplay->Button[MOUSE_LEFT] || oculusDisplay->Button[MOUSE_MIDDLE] || oculusDisplay->Button[MOUSE_RIGHT] || (state.Buttons & ovrButton_Enter));

}

void GraspOculusDisplay::Release( void ) {

	// Shutdown the Rift.
	ovr_Shutdown();
	oculusDisplay->CloseWindow();
	oculusDisplay->ReleaseDevice();

}

// The OpenGlObjects package needs a viewpoint object that depends on the display.
// So we have GraspDisplay create and apply the appropriate Viewpoint object.
OculusViewpoint *GraspOculusDisplay::CreateViewpoint( double ipd, double nearest, double farthest ) {
	OculusViewpoint *viewpoint = new OculusViewpoint( oculusMapper, ipd, nearest, farthest );
	return( viewpoint );
}

void GraspOculusDisplay::ApplyViewpoint( Viewpoint *viewpoint, Eye eye ) {
	viewpoint->Apply( nullptr, eye );
}

// Prepare to render into the display for the specified eye.
void GraspOculusDisplay::SelectEye( int eye ) {
	oculusMapper->SelectEye( eye );
}

// Finish up with the specified eye.
// This is mainly here because Oculus has (had?) a bug that has to be taken care of.
void GraspOculusDisplay::DeselectEye( int eye ) {
	// Take care of an Oculus bug.
	oculusMapper->DeselectEye( eye );
}

bool GraspOculusDisplay::HandleMessages( void ) {
	return( oculusDisplay->HandleMessages() );
}

void GraspOculusDisplay::Present( void ) {
	// Do distortion rendering, Present and flush/sync
	ovrResult result = oculusMapper->BlastIt();
	// fOutputDebugString( "BlastIt() returns: %d\n", result );
}

// Wait for a reaction from the user/subject to validate the current condition.
// Typically, this is a press of a key or button, but that depends on the hardware.
bool GraspOculusDisplay::Validate( void ) {
	ovrInputState state;
	ovr_GetInputState(	oculusMapper->session,  ovrControllerType_Remote, &state );
	bool current_state = 
		( oculusDisplay->Button[MOUSE_LEFT] || oculusDisplay->Button[MOUSE_MIDDLE] || oculusDisplay->Button[MOUSE_RIGHT] || (state.Buttons & ovrButton_Enter));
	if ( waitForUp && current_state ) return( false );
	if ( !current_state ) {
		waitForUp = false;
		return( false );
	}
	waitForUp = true;
	return( true );
}

bool GraspOculusDisplay::KeyDownEvents( int key ) {
	return( oculusDisplay->KeyDownEvents[ key ] > 0 );
}
void GraspOculusDisplay::ClearKeyDownEvents( void ) {
	oculusDisplay->ClearKeyDownEvents();
}

