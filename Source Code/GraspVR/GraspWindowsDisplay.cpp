// GraspWindowsDisplay.cpp

#include "stdafx.h"
#include "GraspDisplays.h"

using namespace Grasp;
using namespace PsyPhy;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// GraspWindowsDisplay
//

void GraspWindowsDisplay::Initialize( HINSTANCE instance, HWND parent_window ) {
	hInstance = instance;
	window = new OpenGLWindow();
	window->FullScreen = fullscreen;
	window->Create(	parent_window, "GRASP", 0, 0, 1920, 1080 );
}

void GraspWindowsDisplay::Release( void ) {
	window->Destroy();
}

// The OpenGlObjects package needs a viewpoint object that depends on the display.
// So we have GraspDisplay create and apply the appropriate Viewpoint object.
Viewpoint *GraspWindowsDisplay::CreateViewpoint( double ipd, double nearest, double farthest ) {
	Viewpoint *viewpoint = new Viewpoint( ipd, 60.0, nearest, farthest );
	return( viewpoint );
}

void GraspWindowsDisplay::ApplyViewpoint( Viewpoint *viewpoint, Eye eye ) {
	viewpoint->Apply( window, eye );
}

// Prepare to render into the display overall.
// Because we use a single window, we clear the whole thing to black here.
void GraspWindowsDisplay::Prepare( void ) {
	window->Clear( BLACK );
}
// Prepare to render into the display for the specified eye.
// In a simple windows display we don't need to do anything here.
// Prepare() has already cleared the window for both eyes.
void GraspWindowsDisplay::SelectEye( int eye ) {}

// Finish up with the specified eye.
// This is mainly here because Oculus has (had?) a bug that has to be taken care of.
// For a Windows display, it doesn't need to do anything.
void GraspWindowsDisplay::DeselectEye( int eye ) {}

bool GraspWindowsDisplay::HandleMessages( void ) {
	return( window->RunOnce() );
}

void GraspWindowsDisplay::Present( void ) {
	window->Swap();
}


// Wait for a reaction from the user/subject to validate the current condition.
// Typically, this is a press of a key or button, but that depends on the hardware.
bool GraspWindowsDisplay::Validate( void ) {
	return( ButtonDownEvents( MOUSE_LEFT ) );
}

bool GraspWindowsDisplay::KeyDownEvents( int key ) {
	return( window->KeyDownEvents[ key ] > 0 );
}
bool GraspWindowsDisplay::KeyDown( int key ) {
	return( window->Key[ key ] );
}
void GraspWindowsDisplay::ClearKeyDownEvents( void ) {
	window->ClearKeyDownEvents();
}

bool GraspWindowsDisplay::ButtonDownEvents( int button ) {
	return( window->ButtonDownEvents[ button ] > 0 );
}
bool GraspWindowsDisplay::ButtonDown( int button ) {
	return( window->Button[ button ] );
}
void GraspWindowsDisplay::ClearButtonDownEvents( void ) {
	window->ClearButtonDownEvents();
}