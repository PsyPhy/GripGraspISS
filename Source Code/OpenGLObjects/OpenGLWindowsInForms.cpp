
#include "../OpenGLObjects/OpenGLObjects.h"

// Create an OpenGLObjects window that is embedded in a Windows::Form.
// To use, create a panel in the form and then pass the reference to that panel to this routine.
// You also need to explicitly include this file in you Windows Forms project, as the routine
// is not included in  unmanaged OpenGLObjects.lib. 

namespace PsyPhy {

#ifdef _MANAGED
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
#endif

};

