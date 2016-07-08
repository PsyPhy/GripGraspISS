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
INPUT MouseRightDownInput;
INPUT MouseRightUpInput;
INPUT KeyReturnDownInput;
INPUT KeyReturnUpInput;

int n_inputs = 1;

int main(int argc, char *argv[])
{
	fprintf( stderr, "\n\n%s\nEmulates a mouse using the Oculus remote.\n\n", "OculusMouse.exe" );

	int slow = 10;

	// Initialize the various Input structures.
	MouseMoveInput.mi.dx = 0;
	MouseMoveInput.mi.dy = 0;
	MouseMoveInput.mi.mouseData = 0;
	MouseMoveInput.mi.dwFlags =  MOUSEEVENTF_MOVE;
	MouseMoveInput.type = INPUT_MOUSE;

	MouseLeftDownInput.mi.time = 0;
	MouseLeftDownInput.mi.dx = 0;
	MouseLeftDownInput.mi.dy = 0;
	MouseLeftDownInput.mi.mouseData = 0;
	MouseLeftDownInput.mi.dwFlags =   MOUSEEVENTF_LEFTDOWN;
	MouseLeftDownInput.type = INPUT_MOUSE;

	MouseLeftUpInput.mi.time = 0;
	MouseLeftUpInput.mi.dx = 0;
	MouseLeftUpInput.mi.dy = 0;
	MouseLeftUpInput.mi.mouseData = 0;
	MouseLeftUpInput.mi.dwFlags =   MOUSEEVENTF_LEFTUP;
	MouseLeftUpInput.type = INPUT_MOUSE;

	MouseRightDownInput.mi.time = 0;
	MouseRightDownInput.mi.dx = 0;
	MouseRightDownInput.mi.dy = 0;
	MouseRightDownInput.mi.mouseData = 0;
	MouseRightDownInput.mi.dwFlags =   MOUSEEVENTF_RIGHTDOWN;
	MouseRightDownInput.type = INPUT_MOUSE;

	MouseRightUpInput.mi.time = 0;
	MouseRightUpInput.mi.dx = 0;
	MouseRightUpInput.mi.dy = 0;
	MouseRightUpInput.mi.mouseData = 0;
	MouseRightUpInput.mi.dwFlags =   MOUSEEVENTF_RIGHTUP;
	MouseRightUpInput.type = INPUT_MOUSE;

	KeyReturnDownInput.ki.time = 0;
	KeyReturnDownInput.ki.dwExtraInfo = 0;
	KeyReturnDownInput.ki.dwFlags = 0;
	KeyReturnDownInput.ki.wScan = 0;
	KeyReturnDownInput.ki.wVk = VK_RETURN;

	KeyReturnUpInput.ki.time = 0;
	KeyReturnUpInput.ki.dwExtraInfo = 0;
	KeyReturnUpInput.ki.dwFlags = KEYEVENTF_KEYUP;
	KeyReturnUpInput.ki.wScan = 0;
	KeyReturnUpInput.ki.wVk = VK_RETURN;

	ovrResult result;
	result = ovr_Initialize( nullptr );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "OculusMouse", "Failed to initialize libOVR." );
	ovrSession			session;
	ovrGraphicsLuid		luid;
	result = ovr_Create( &session, &luid );
	fAbortMessageOnCondition( !OVR_SUCCESS( result), "OculusMouse", "Failed to create OVR session." );
	fprintf( stderr, "Oculus OVR intialized.\n" );

	// Read the state of the Oculus remote buttons to initialize the up/down states.
	ovrInputState state;
	ovr_GetInputState(	session,  ovrControllerType_Remote, &state );

	bool enterPreviouslyDown = state.Buttons & ovrButton_Enter;
	bool backPreviouslyDown = state.Buttons & ovrButton_Back;

	while ( 1 ) {

		// Get the current state of the Oculus Remote buttons.
		ovr_GetInputState(	session,  ovrControllerType_Remote, &state );

		// Move the mouse pointer based on mini-joystick (ring) presses.
		MouseMoveInput.mi.dx = MouseMoveInput.mi.dy = 0;
		if ( state.Buttons & ovrButton_Right ) MouseMoveInput.mi.dx = slow;
		if ( state.Buttons & ovrButton_Left ) MouseMoveInput.mi.dx = - slow;
		if ( state.Buttons & ovrButton_Up ) MouseMoveInput.mi.dy = - slow;
		if ( state.Buttons & ovrButton_Down ) MouseMoveInput.mi.dy = slow;
		if ( MouseMoveInput.mi.dx != 0 || MouseMoveInput.mi.dy != 0 ) SendInput( n_inputs, &MouseMoveInput, sizeof( MouseMoveInput ) );

		// Center button is mapped to left mouse click.
		if ( state.Buttons & ovrButton_Enter && !enterPreviouslyDown )  SendInput( n_inputs, &MouseLeftDownInput, sizeof( MouseLeftDownInput ) );
		if ( !(state.Buttons & ovrButton_Enter) && enterPreviouslyDown )  SendInput( n_inputs, &MouseLeftUpInput, sizeof( MouseLeftUpInput ) );
		enterPreviouslyDown = state.Buttons & ovrButton_Enter;

		// Back button is mapped to right mouse click.
		if ( state.Buttons & ovrButton_Back && !backPreviouslyDown )  SendInput( n_inputs, &MouseRightDownInput, sizeof( MouseRightDownInput ) );
		if ( !(state.Buttons & ovrButton_Back ) && backPreviouslyDown )  SendInput( n_inputs, &MouseRightUpInput, sizeof( MouseRightUpInput ) );
		backPreviouslyDown = state.Buttons & ovrButton_Back;


		Sleep( 50 );
	}

	return 0;
}

