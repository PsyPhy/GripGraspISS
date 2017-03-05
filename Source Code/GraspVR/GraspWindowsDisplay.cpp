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
	window->FullScreen = true;
	window->Create(	parent_window, "GRASP", 0, 0, 1920, 1080 );

	// Decide if we are in full screen mode or not.
	// To avoid losing focus by clicking outside the desktop window it is best to be in fullscreen mode.
	static const bool fullscreen = true;


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

// Prepare to render into the display for the specified eye.
// In a simple windows display we don't need to do anything here.
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
	return( false );
}

bool GraspWindowsDisplay::KeyDownEvents( int key ) {
	return( false );
}
void GraspWindowsDisplay::ClearKeyDownEvents( void ) {}

