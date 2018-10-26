
#include "../OpenGLObjects/OpenGLObjects.h"

// Create an OpenGLObjects window that is embedded in a Windows::Form.
// To use, create a panel in the form and then pass the reference to that panel to this routine.
// You also need to explicitly include this file in you Windows Forms project, as the routine
// is not included in  unmanaged OpenGLObjects.lib. 

namespace PsyPhy {

#ifdef _MANAGED
	System::Void OpenGLWindowPanelSizeChanged(System::Object^  sender, System::EventArgs^  e) {

		System::Windows::Forms::Panel^  panel = (System::Windows::Forms::Panel^) sender;
		HWND panel_window = static_cast<HWND>( panel->Handle.ToPointer() );
		OpenGLWindow *window = (OpenGLWindow *) GetWindowLongPtr( panel_window, GWLP_USERDATA );
		SetWindowPos( window->hWnd, HWND_TOP, 0, 0, panel->ClientSize.Width, panel->ClientSize.Height, SWP_NOMOVE );

		fOutputDebugString( "sender (panel) %p   Window: %08lx\n", (LONG_PTR) &sender, (unsigned long) window );  

	}

	OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel, HGLRC shared_hRC ) {

		HWND			parent = static_cast<HWND>( panel->Handle.ToPointer() );
		OpenGLWindow	*window = new OpenGLWindow();
		window->Border = false;

		fOutputDebugString( "sender (panel) %p   Window: %08lx\n", (LONG_PTR) &panel, (unsigned long) window );  

		if ( !window->Create( parent, "HMD", 0, 0,panel->Width, panel->Height, shared_hRC ) ) {
			fMessageBox( MB_OK, "GraspGUI", "Error creating OpenGLWindow inside Forms Panel." );
			exit( -1 );
		}  
		// Store a link to this window in the parent window so that 
		// the parent can access this one.
		SetWindowLongPtr( parent, GWLP_USERDATA, (LONG_PTR) window );
		panel->SizeChanged += gcnew System::EventHandler( &PsyPhy::OpenGLWindowPanelSizeChanged );
		return( window );
	}

	OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel ) {
		return( CreateOpenGLWindowInForm( panel, nullptr ) );
	}


#endif

};

