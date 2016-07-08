// OculusMouse.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include "../Useful/fMessageBox.h"

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include <assert.h>

INPUT MouseMoveInput;
INPUT MouseLeftDownInput;
INPUT MouseLeftUpInput;
int n_inputs = 1;

int main(int argc, char *argv[])
{
	fprintf( stderr, "\n\n%s\nEmulates a mouse using the Oculus remote.\n\n", "OculusMouse.exe" );

	int slow = 10;
	int fast = slow * 2;

	// Initialize the Input structures.
	MouseMoveInput.mi.dx = 0;
	MouseMoveInput.mi.dy = 0;
	MouseMoveInput.mi.mouseData = 0;
	MouseMoveInput.mi.dwFlags =  MOUSEEVENTF_MOVE;
	MouseMoveInput.type = INPUT_MOUSE;

	MouseLeftDownInput.mi.dx = 0;
	MouseLeftDownInput.mi.dy = 0;
	MouseLeftDownInput.mi.mouseData = 0;
	MouseLeftDownInput.mi.dwFlags =   MOUSEEVENTF_LEFTDOWN;
	MouseLeftDownInput.type = INPUT_MOUSE;

	MouseLeftUpInput.mi.dx = 0;
	MouseLeftUpInput.mi.dy = 0;
	MouseLeftUpInput.mi.mouseData = 0;
	MouseLeftUpInput.mi.dwFlags =   MOUSEEVENTF_LEFTUP;
	MouseLeftUpInput.type = INPUT_MOUSE;

	ovrResult result;
	result = ovr_Initialize( nullptr );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "OculusMouse", "Failed to initialize libOVR." );
	ovrSession			session;
	ovrGraphicsLuid		luid;
	result = ovr_Create( &session, &luid );
	fAbortMessageOnCondition( !OVR_SUCCESS( result), "OculusMouse", "Failed to create OVR session." );
	fprintf( stderr, "Oculus OVR intialized.\n" );

	bool previouslyDown = false;

	while ( 1 ) {

		// Read the state of the Oculus remote buttons and use them to drive the roll angle.
		ovrInputState state;
		ovr_GetInputState(	session,  ovrControllerType_Remote, &state );

		MouseMoveInput.mi.dx = MouseMoveInput.mi.dy = 0;
		if ( state.Buttons & ovrButton_Right ) MouseMoveInput.mi.dx = slow;
		if ( state.Buttons & ovrButton_Left ) MouseMoveInput.mi.dx = - slow;
		if ( state.Buttons & ovrButton_Up ) MouseMoveInput.mi.dy = - slow;
		if ( state.Buttons & ovrButton_Down ) MouseMoveInput.mi.dy = slow;
		if ( MouseMoveInput.mi.dx != 0 || MouseMoveInput.mi.dy != 0 ) SendInput( n_inputs, &MouseMoveInput, sizeof( MouseMoveInput ) );

		if ( state.Buttons & ovrButton_Enter && !previouslyDown )  SendInput( n_inputs, &MouseLeftDownInput, sizeof( MouseLeftDownInput ) );
		if ( !(state.Buttons & ovrButton_Enter) && previouslyDown )  SendInput( n_inputs, &MouseLeftUpInput, sizeof( MouseLeftUpInput ) );
		previouslyDown = state.Buttons & ovrButton_Enter;
		Sleep( 50 );
	}

	return 0;
}

