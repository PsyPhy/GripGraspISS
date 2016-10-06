// StartOculus.cpp 
// A stupid little hack to get the Oculus interface to load.

#include <Windows.h>
#include <stdio.h>
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include <assert.h>

int main(int argc, char* argv[])
{

	// The following commands force Oculus to start up, if it is not already running.
	ovrResult			result;
	ovrSession			session;
	ovrGraphicsLuid		luid;
	ovrHmdDesc			hmdDesc;
	result = ovr_Initialize( nullptr );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "OculusMouse", "Failed to initialize libOVR.\nIs the Virtual Reality Helmet plugged in?" );
	result = ovr_Create( &session, &luid );
	fAbortMessageOnCondition( !OVR_SUCCESS( result), "OculusMouse", "Failed to create OVR session.\nIs the Virtual Reality Helmet plugged in?" );
	hmdDesc = ovr_GetHmdDesc( session );
	fprintf( stderr, "Oculus OVR intialized.\n" );
	fprintf( stderr, "Waiting for Oculus window to appear ..." );

	// Now we wait for the Oculus store window to appear so that we can then minimize it.
	HWND hwnd;
	while ( !(hwnd = FindWindow( NULL, "Oculus" ))) Sleep( 200 );
	while ( !IsWindowVisible( hwnd ) ) Sleep( 200 );
	ShowWindow( hwnd, SW_FORCEMINIMIZE );

	//// Have the subject press the Select button on the Oculus remote to get rid of the 
	////  Health and Safety warning.
	//MessageBox( NULL, 
	//	"Press Select button (center button)\n on VR Remote and hold for 2 seconds.\n\nThen press 'OK' below.", 
	//	"Startup Oculus", MB_OK | MB_ICONASTERISK );
		
	return 0;
}

