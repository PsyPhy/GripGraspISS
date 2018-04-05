/********************************************************************************/

#include "OpenGLObjects.h"
#include "OpenGLWindows.h"

using namespace PsyPhy;

static bool already_registered = false;

/********************************************************************************/

// An event handler that recognizes OpenGLWindows.
LONG WINAPI OpenGLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 

	OpenGLWindow *oglw;
	static PAINTSTRUCT ps;

	oglw = (OpenGLWindow *) GetWindowLong( hWnd, GWL_USERDATA );
	// If the window is an OpenGLWindow, use its WindowProc().
	// The method used here is not very robust. It assumes that any window
	//  with GWL_USERDATA filled in is an OpenGLWindow instance. 
	// We should probably do some other sort of runtime type checking.
	// But since this event handler is probably only used by windows created by
	//  OpenGLWindow::Create(), there is little danger of making a mistake.
	// Indeed, we could probably assume that the window passed here *is*  an
	//  OpenGLWindow.
	if ( oglw ) {
		return( oglw->WindowProc( hWnd, uMsg, wParam, lParam ) );
	}
	// Otherwise, do what one would normally do with the events.
	else {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


} 

/********************************************************************************/

OpenGLWindow::OpenGLWindow( void ) {

	// Init vars
	hWnd = NULL;
	hRC = NULL;
	hDC = NULL;

	Colordepth = 32;
	Stereo = FullScreen = false;
	Border = true;
	DoubleBuffer = true;
	DisplayFrequency = 60;

	keyboard_callback = NULL;
	event_callback = NULL;

	width = 0;
	height = 0;
	
	// Clear input
	for (int i = 0; i < sizeof(Key) / sizeof(Key[0]); ++i) Key[i] = false;
	for (int i = 0; i < sizeof(Button) / sizeof(Button[0]); ++i) Button[i] = false;
	ClearKeyDownEvents();
	ClearButtonDownEvents();
	pointerLastX = UNDEFINED;
	pointerLastY = UNDEFINED;


}

/********************************************************************************/

bool OpenGLWindow::Create( HWND parent, char *title, int x, int y, int w, int h, HGLRC shared_hRC )
{

	int		PixelFormat;	// Holds The Results After Searching For A Match
	WNDCLASS	wc;				// Windows Class Structure
	RECT		WindowRect;		// Grabs Rectangle Upper Left / Lower Right Values

	hParent = parent;

	if ( FullScreen ) x = y = 0;

	WindowRect.left = (long) x;       // Set Left Value To x
	WindowRect.right = (long) x + w;  // Set Right Value To Achieve Requested Width
	WindowRect.top = (long) y;        // Set Top Value To y
	WindowRect.bottom = (long) y + h; // Set Bottom Value To Acheive Requested Height

	hInstance			= GetModuleHandle(NULL);					// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) OpenGLWindowProc;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;										// No Extra Window Data
	wc.cbWndExtra		= 0;										// No Extra Window Data
	wc.hInstance		= hInstance;								// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);				// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);				// Load The Arrow Pointer
	wc.hbrBackground	= NULL;										// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";									// Set The Class Name

	if ( !already_registered ) {
		if ( !RegisterClass(&wc) )									// Attempt To Register The Window Class
		{
			//      MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			//      return false;											// Return false
		}
		//    already_registered = true;
	}

	if ( FullScreen )												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;									// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = w;							// Selected Screen Width
		dmScreenSettings.dmPelsHeight = h;	    			        // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = Colordepth; 			    // Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = DisplayFrequency;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL) {
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				FullScreen = false;		// Windowed Mode Selected.  Fullscreen = false
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox( NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP );
				return false;									
			}
		}
	}

	if ( FullScreen )					// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;		// Window Extended Style
		dwStyle=WS_POPUP;				// Windows Style
		ShowCursor(false);				// Hide Mouse Pointer
	}
	else if ( !Border ) {
		dwExStyle = WS_EX_APPWINDOW;					// Window Extended Style
		dwStyle = ( parent ? WS_CHILD : WS_POPUP );		// Windows Style
		//ShowCursor(false);								// Hide Mouse Pointer
	}

	else 
	{
		dwExStyle = WS_EX_APPWINDOW;									// Window Extended Style
		dwStyle   = WS_OVERLAPPEDWINDOW | ( parent ? WS_CHILD : 0 );	// Windows Style
	}

	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );		// Adjust Window To True Requested Size
	width = WindowRect.right-WindowRect.left;							// Calculate Window Width
	height = WindowRect.bottom-WindowRect.top;						// Calculate Window Height  

	// Create The Window
	if (!(hWnd=CreateWindowEx( 	dwExStyle,	// Extended Style For The Window
		"OpenGL",								// Class Name
		title,									// Window Title
		dwStyle |								// Defined Window Style
		WS_CLIPSIBLINGS |						// Required Window Style
		WS_CLIPCHILDREN,						// Required Window Style
		x, y,									// Window Position
		width,	
		height,
		parent,									// Parent Window
		NULL,									// No Menu
		hInstance,								// Instance
		NULL )))								// Dont Pass Anything To WM_CREATE
	{
		Destroy();								// If CreateWindowEx() failed, reset the display.
		MessageBox( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;							// Abort.
	}

	// Store a pointer to the OpenGLWindow instance for use by the callback routines.
	SetWindowLong( hWnd, GWL_USERDATA, (unsigned long) this );

	DWORD flag = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	if ( DoubleBuffer ) flag |= PFD_DOUBLEBUFFER;
	if ( Stereo ) flag |= PFD_STEREO;

	static	PIXELFORMATDESCRIPTOR pfd =		// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size Of This Pixel Format Descriptor
		1,									// Version Number
		flag,								// Must Support Double Buffering
		PFD_TYPE_RGBA,						// Request An RGBA Format
		Colordepth,							// Select Our Color Depth
		0, 0, 0, 0, 0, 0,					// Color Bits Ignored
		1,									// YES Alpha Buffer
		0,									// Shift Bit Ignored
		0,									// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		16,									// 16Bit Z-Buffer (Depth Buffer)
		0,									// No Stencil Buffer
		0,									// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	hDC = GetDC( hWnd );

	// Did We Get A Device Context?
	if ( !hDC )				
	{
		// If not, reset the display.
		Destroy();								
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;							
	}

	// Did Windows Find A Matching Pixel Format?
	if ( !(PixelFormat = ChoosePixelFormat( hDC,&pfd )) )	
	{
		// If not, reset the display.
		Destroy();											
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;										
	}

	// Are We Able To Set The Pixel Format?
	if( !SetPixelFormat( hDC, PixelFormat, &pfd) )		
	{
		// Reset The DisplayDestroy();											
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;										
	}

	if ( shared_hRC != nullptr ) {
		hRC = shared_hRC;
		hRCisShared = true;
	}
	else {
		hRC = wglCreateContext( hDC );
		hRCisShared = false;
	}

	// Are We Able To Get A Rendering Context?
	if ( !( hRC ) )		
	{
		Destroy();								
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								 
	}

	// Try To Activate The Rendering Context
	if( !wglMakeCurrent( hDC, hRC ) )					
	{
		Destroy();								            
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								 
	}

	ShowWindow(hWnd,SW_SHOW);					// Show The Window
	SetForegroundWindow(hWnd);					// Slightly Higher Priority
	SetFocus(hWnd);								// Sets Keyboard Focus To The Window

	Clear( BLACK );

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	// Builds font list
	// BuildGLFont(FONT_DEFAULT, 1);

	return true; // Success

}

/********************************************************************************/

void OpenGLWindow::SetPosition( int x, int y, int w, int h )  {
	SetWindowPos( hWnd, NULL, x, y, w, h, SWP_NOZORDER );
}

/********************************************************************************/

void OpenGLWindow::Activate( void )
{
	// Try To Activate The Rendering Context
	if( !wglMakeCurrent(hDC,hRC) )					
	{
		Destroy();								   
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

}

/********************************************************************************/

void OpenGLWindow::Destroy( void )
{
	// Free memory
	// KillGLFont();

	// Are We In Fullscreen Mode?
	if (FullScreen)	{									
	
		// If So Switch Back To The Desktop
		ChangeDisplaySettings(NULL,0);	
		ShowCursor(true);
	}

	// Do We Have A Rendering Context?
	if (hRC && ! hRCisShared )											    
	{
		if ( !wglMakeCurrent(NULL,NULL) )	// Are We Able To Release The DC And RC Contexts?
		{
			//      MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if ( !wglDeleteContext(hRC) )			// Are We Able To Delete The RC?
		{
			//      MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;	
	}

	if (hDC && ! hRCisShared && !ReleaseDC( hWnd, hDC ))	// Are We Able To Release The DC
	{
		//    MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;	
	}

	if (hWnd && !DestroyWindow( hWnd ))	// Are We Able To Destroy The Window?
	{
		//    MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;
	}

	if ( !UnregisterClass( "OpenGL", hInstance ) )			// Are We Able To Unregister Class
	{
		//    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		//    hInstance=NULL;	
	}
}

/********************************************************************************/

void OpenGLWindow::Clear( int c )
{
	glClearColor( OpenGLColors[c][0], OpenGLColors[c][1], OpenGLColors[c][2], 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void OpenGLWindow::Clear( double r, double g, double b, double alpha )
{
	glClearColor( (float) r, (float) g, (float) b, (float) alpha );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void OpenGLWindow::Clear( float rgba[] )
{
	glClearColor( rgba[0], rgba[1], rgba[2], rgba[3] );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
}

/********************************************************************************/

void OpenGLWindow::Swap( void )
{
	SwapBuffers( hDC );
}

/********************************************************************************/

void	SaveBitmap( const char *szFilename, HDC hdc, HBITMAP hBitmap );

void OpenGLWindow::SaveAsBMP( const char *filename )
{
	// Presumably we just did a swap. 
	// Sleep a little to be sure that the bitmap has been updated.
	Sleep( 100 );

	HDC		hBmpFileDC = CreateCompatibleDC( hDC );
	HBITMAP	hBmpFileBitmap = CreateCompatibleBitmap( hDC, width, height );
	HBITMAP hOldBitmap = (HBITMAP) SelectObject( hBmpFileDC, hBmpFileBitmap );
	int result = BitBlt( hBmpFileDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY );
	HBITMAP hJoe = (HBITMAP) SelectObject( hBmpFileDC, hOldBitmap );
	// fOutputDebugString( "SaveAsBMP %x %x %x %x %d\n", hDC, hOldBitmap, hBmpFileBitmap, hJoe, result );

	SaveBitmap( filename, hDC, hBmpFileBitmap );

	DeleteDC( hBmpFileDC );
	DeleteObject( hBmpFileBitmap );

}

/********************************************************************************/

#define MARGIN 50

LRESULT OpenGLWindow::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	static PAINTSTRUCT ps;

	union {
		unsigned long   param;
		unsigned short  word[2];
	} unpack;

	int mouse_x, mouse_y;


	// Allow one to substitute a function for the event handler.
	// A better way would be to to create a new class of OpenGLWindow with its specific
	// event handling procedure.
	if ( event_callback ) {
		return( (*event_callback)( hWnd, uMsg, wParam, lParam ) );
	}
	// This is what the OpenGLWindow will normally do.
	else {

		switch( uMsg ) {

		case WM_SYSCOMMAND:					// Intercept System Commands

			switch ( wParam )	{			  // Check System Calls

			case SC_SCREENSAVE:			  // Screensaver Trying To Start?
			case SC_MONITORPOWER:	  	// Monitor Trying To Enter Powersave?
				return 0;				        // Prevent From Happening
				break;

			}
			break;						        // Exit

		case WM_CLOSE:
			PostMessage( hWnd, WM_QUIT, 0, 0 );
			return 0;
			break;

		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
			break;

		case WM_SIZE:
			// Update the size in the OpenGLWindow structure as well.
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			PostMessage(hWnd, WM_PAINT, 0, 0);
			return 0;
			break;

		case WM_CHAR:

			switch (wParam) {

			case 27:			/* ESC key */
				PostMessage( hWnd, WM_QUIT, 0, 0 );
				break;

			default:
				if ( keyboard_callback ) (*keyboard_callback)( wParam );
				else lastInput = wParam;
			}
			return 0;
			break;

#ifdef OLD
		case WM_KEYDOWN:
			if ( wParam >= VK_SPACE && wParam <= VK_HELP ) {
				lastInput = wParam;
			}
			return 0;
			break;

		case WM_LBUTTONDOWN:
			lastInput = WM_LBUTTONDOWN;
			return 0;
			break;

		case WM_RBUTTONDOWN:
			lastInput = WM_RBUTTONDOWN;
			return 0;
			break;

		case WM_MBUTTONDOWN:
			lastInput = WM_MBUTTONDOWN;
			return 0;
			break;

#else
		case WM_LBUTTONDOWN:
			lastInput = WM_LBUTTONDOWN;
			Button[MOUSE_LEFT] = true;
			ButtonDownEvents[MOUSE_LEFT]++;
			break;
		case WM_LBUTTONUP:
			Button[MOUSE_LEFT] = false;
			break;
		case WM_MBUTTONDOWN:
			lastInput = WM_MBUTTONDOWN;
			Button[MOUSE_MIDDLE] = true;
			ButtonDownEvents[MOUSE_MIDDLE]++;
			break;
		case WM_MBUTTONUP:
			Button[MOUSE_MIDDLE] = false;
			break;
		case WM_RBUTTONDOWN:
			lastInput = WM_RBUTTONDOWN;
			Button[MOUSE_RIGHT] = true;
			ButtonDownEvents[MOUSE_RIGHT]++;
			break;
		case WM_RBUTTONUP:
			Button[MOUSE_RIGHT] = false;
			break;

		case WM_KEYDOWN:
			Key[wParam] = true;
			KeyDownEvents[wParam]++;
			break;

		case WM_KEYUP:
			if ( wParam >= VK_SPACE && wParam <= VK_HELP ) {
				lastInput = wParam;
			}
			Key[wParam] = false;
			break;
#endif

		case WM_MOUSEMOVE:

			unpack.param = lParam;
			mouse_x = unpack.word[0];
			mouse_y = unpack.word[1];

			if ( wParam == MK_LBUTTON || FullScreen ) {

				// If this is the first event we need to establish the reference point for mouseDeltaX and mouseDeltaY.
				if ( pointerLastX == UNDEFINED ) {
					pointerLastX = mouse_x;
					pointerLastY = mouse_y;
				}

				mouseDeltaX = mouse_x - pointerLastX;
				mouseDeltaY = mouse_y - pointerLastY;
				mouseCumulativeX += mouseDeltaX;
				mouseCumulativeY += mouseDeltaY;

				if ( mouse_x < MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.right - 2 * MARGIN - point.x;
					pointerLastX += delta;
					SetCursorPos( rect.right - 2 * MARGIN, point.y );
					return 0;
				}
				if ( mouse_x > width -  MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.left + 2 * MARGIN - point.x;
					pointerLastX += delta;
					SetCursorPos( rect.left + 2 * MARGIN, point.y );
					return 0;
				}

				if ( mouse_y < MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.bottom - 2 * MARGIN - point.y;
					pointerLastY += delta;
					SetCursorPos( point.x, rect.bottom - 2 * MARGIN );
					return 0;
				}
				if ( mouse_y > height -  MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.top + 2 * MARGIN - point.y;
					pointerLastY += delta;
					SetCursorPos( point.x, rect.top + 2 * MARGIN );
					return 0;
				}

				pointerLastX = mouse_x;
				pointerLastY = mouse_y;
				lastInput = WM_MOUSEMOVE;
			}
			else {
				pointerLastX = mouse_x;
				pointerLastY = mouse_y;
			}
			return 0;
			break;          
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	}
}


void OpenGLWindow::SetKeyboardCallback( void (*callback)( int key ) )
{
	keyboard_callback = callback;
}

void OpenGLWindow::SetEventCallback( bool (*callback)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) )
{
	event_callback = callback;
}

void OpenGLWindow::ClearKeyDownEvents( int key ) {
	if ( key < 0 ) 	for (int i = 0; i < sizeof(KeyDownEvents) / sizeof(KeyDownEvents[0]); ++i) KeyDownEvents[i] = 0;
	else KeyDownEvents[key] = 0;
}
void OpenGLWindow::ClearButtonDownEvents( int button ) {
	if ( button < 0 ) 	for (int i = 0; i < sizeof(ButtonDownEvents) / sizeof(ButtonDownEvents[0]); ++i) ButtonDownEvents[i] = 0;
	else ButtonDownEvents[button] = 0;
}
/*********************************************************************************/

bool OpenGLWindow::RunOnce( void ) {

	MSG msg;				/* message */

	while ( PeekMessage( &msg, hWnd, 0, 0, 0 ) ) {
		if ( !GetMessage( &msg, hWnd, 0, 0 ) ) return( false );
		else {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	return( true );

}


#if 0

/*********************************************************************************/

int OpenGLWindow::GetInput( double timeout ) {

	int hold;
	Timer timer;

	// Clear any previous input
	lastInput = 0;
	mouseDeltaX = 0;
	mouseDeltaY = 0;

	TimerSet( &timer, timeout );
	while ( lastInput == 0 ) {
		if ( timeout > 0.0 && TimerTimeout( &timer ) ) {
			return( TIMEOUT );
		}
		if ( ! RunOnce() ) return( WM_QUIT );
	}
	hold = lastInput;
	lastInput = 0;
	return( hold );

}

int OpenGLWindow::MouseDeltaX( void ) {
	return( mouseDeltaX );
}
int OpenGLWindow::MouseDeltaY( void ) {
	return( mouseDeltaY );
}


int OpenGLWindow::Sleep( double seconds ) {

	Timer timer;
	TimerSet( &timer, seconds );
	while ( ! TimerTimeout( &timer ) ) {
		if ( ! RunOnce() ) return( WM_QUIT );
	}
	return( 0 );
}
#endif

/*************************************************************************************/

/* 

general purpose text routine.  draws a string according to the
format in a stroke font at x, y after scaling it by the scale
specified.  x, y and scale are all in window-space [i.e., pixels]
with origin at the lower-left.

*/

void OpenGLWindow::printf(GLuint x, GLuint y, double scale, char* format, ...) {

	va_list args;
	char buffer[1024], *p;
	GLfloat font_scale = 119.05F + 33.33F;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	glViewport( 0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//    glPolygonMode(GL_FRONT, GL_LINE);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_DEPTH_TEST);

	glTranslatef((GLfloat) x, (GLfloat) y, 0.0F);
	glScalef(scale/font_scale, scale/font_scale, scale/font_scale);

	for(p = buffer; *p; p++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
		//      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *p );
		//        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, *p );
	}

	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}

/*************************************************************************************/

OpenGLWindow *CreateMainGlWindow( int argc, char *argv[] ) {

	int width = 1024;
	int height = 768;
	bool border = true;
	bool fullscreen = false;
	bool stereo = false;

	char * title;

	if ( argc > 0 ) title = argv[0];
	else title = "OpenGL Window";

	OpenGLWindow *window;

	for ( int arg = 1; arg < argc; arg++ ) {

		if ( !strcmp( argv[arg], "-full" ) ) {
			width = 1024;
			height = 768;
			border = false;
			fullscreen = true;
			stereo = false;
		}
		else if ( !strcmp( argv[arg], "-hmd" ) ) {
			width = 1280;
			height = 480;
			border = false;
			fullscreen = true;
			stereo = true;
			//       HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-svga" ) ) {
			width = 2048;
			height = 768;
			border = false;
			fullscreen = true;
			stereo = true;
			//			HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-nVisL" ) ) {
			fprintf( stderr, "LowRes nVis\n" );
			width = 2048;
			height = 768;
			border = false;
			fullscreen = true;
			stereo = true;
			//      HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-nVis" ) ) {
			width = 2560;
			height = 1024;
			border = false;
			fullscreen = true;
			stereo = true;
			//      HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-noborder" ) ) border = FALSE;

	}

	/*
	* Create a window for an OpenGL application. 
	* Parameters are set by command line arguments.
	*/
	window = new OpenGLWindow();
	if ( !window ) return NULL;

	window->Border = border; 
	window->FullScreen = fullscreen;

	if ( window->Create( NULL, title, 0, 0, width, height ) ) {
		/*
		* Create sets the new window to be the active window.
		* Setup the lights and materials for that window.
		*/
		glUsefulInitializeDefault();
		glUsefulAutoLighting( 1.0 );
		glUsefulDefaultMaterial();
		window->Activate();

	}  

	// Shade model
	glDisable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									  // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	return window;

}


