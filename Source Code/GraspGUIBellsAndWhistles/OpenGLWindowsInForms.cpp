#include "stdafx.h"

#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

// Create an OpenGLObjects window that is embedded in a Windows::Form.
// To use, create a panel in the form and then pass the reference to that panel to this routine.
// This is actually a generally useful routine and should probably be included in the 
// OpenGLObjects / OpenGLWindows package, but because it relies on Windows::Forms I have it here.

namespace PsyPhy {

	OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel, HGLRC shared_hRC ) {

	HWND	parent;
	OpenGLWindow *window;
	parent = static_cast<HWND>( panel->Handle.ToPointer() );
	window = new OpenGLWindow();
	window->Border = false;

	if ( !window->Create( parent, "HMD", 0, 0,panel->Width, panel->Height, shared_hRC ) ) {
		fMessageBox( MB_OK, "GraspGUI", "Error creating OpenGLWindow inside Forms Panel." );
		exit( -1 );
	}  
	return( window );
}

OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel ) {
	return( CreateOpenGLWindowInForm( panel, nullptr ) );
}

};

