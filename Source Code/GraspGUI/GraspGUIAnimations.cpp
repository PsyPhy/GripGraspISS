///
/// Module:	GraspGUI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#include "stdafx.h"

#include "GraspDesktopForm.h"

using namespace GraspGUI;
//using namespace PsyPhy;

PsyPhy::OpenGLWindow	*window;			// The window on the screen.
PsyPhy::Viewpoint		*viewpoint;			// The viewpoint into the virtual world.

// Initialize the objects used to show the status on the screen.
void GraspDesktop::InitializeAnimations( void ) {

	HWND	parent;
	parent = static_cast<HWND>(columbusPanel->Handle.ToPointer());
	window = new PsyPhy::OpenGLWindow();
	window->Border = false;

	if ( !window->Create( parent, "Columbus", 0, 0, columbusPanel->Width, columbusPanel->Height ) ) {
		fMessageBox( MB_OK, "GraspGUI", "Error creating window." );
		exit( -1 );
	}  
	window->Activate();
	window->Clear( 1.0, 0.0, 0.0 );
	window->Swap();

}
void GraspDesktop::KillAnimations( void ) {
	window->Destroy();
}
