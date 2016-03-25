
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

LONG WINAPI
FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 

  static PAINTSTRUCT ps;
  
  switch( uMsg ) {
    
  case WM_SYSCOMMAND:					// Intercept System Commands
    
    switch ( wParam )	{				  // Check System Calls
      
    case SC_SCREENSAVE:			// Screensaver Trying To Start?
    case SC_MONITORPOWER:		// Monitor Trying To Enter Powersave?
      return 0;				        // Prevent From Happening
      break;
      
    }
    break;						        // Exit
    
  case WM_PAINT:
    
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    return 0;
    break;
    
  case WM_SIZE:
    
   PostMessage(hWnd, WM_PAINT, 0, 0);
    return 0;
    break;
    
  case WM_CHAR:
    
    switch (wParam) {
      
    case 27:			/* ESC key */
      PostQuitMessage( 0 );
      break;
      
    case ' ':
      PostMessage(hWnd, WM_PAINT, 0, 0);
      break;
          
    case 'P':
      PostMessage(hWnd, WM_PRINT, 0, 0);
      break;

     }
    return 0;
 	break;
   
  case WM_LBUTTONDOWN:
    PostMessage(hWnd, WM_PAINT, 0, 0);
    return 0;
 	  break;
   
  case WM_RBUTTONDOWN:
    return 0;
 	  break;
   
  case WM_MOUSEMOVE:
    return 0;
 	  break;
   
  case WM_DESTROY:
  case WM_QUIT:
    return 0;
	break;
     
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
} 


HWND CreateFrame( HWND parent ) {

  HINSTANCE hInstance;
  WNDCLASS	wc;						    // Windows Class Structure
  HWND      hWnd;
  DWORD		  dwExStyle;				// Window Extended Style
  DWORD		  dwStyle;				  // Window Style
  static int already_registered = FALSE;


  hInstance			  = GetModuleHandle(NULL);				      // Grab An Instance For Our Window
  wc.style			  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc	= FrameWindowProc;   		  // WndProc Handles Messages
  wc.cbClsExtra		= 0;									                // No Extra Window Data
  wc.cbWndExtra		= 0;									                // No Extra Window Data
  wc.hInstance		= hInstance;							            // Set The Instance
  wc.hIcon			  = LoadIcon(NULL, IDI_WINLOGO);			  // Load The Default Icon
  wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			  // Load The Arrow Pointer
  wc.hbrBackground	= NULL;									            // No Background Required For GL
  wc.lpszMenuName		= NULL;									            // We Don't Want A Menu
  wc.lpszClassName	= "JoeFrame";								          // Set The Class Name
  
  if ( !already_registered ) {
    if ( !RegisterClass(&wc) )									          // Attempt To Register The Window Class
    {
      MessageBox(NULL, "Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;											// Return false
    }
    already_registered = TRUE;
  }

  dwExStyle = WS_EX_OVERLAPPEDWINDOW;			// Window Extended Style
	dwStyle   = WS_BORDER |
    WS_CHILD;							// Windows Style

  if ( !( hWnd = CreateWindow(
    "JoeFrame",					  // Class Name
    "Frame",						// Window Title
    dwStyle,
    0, 0,	
    200, 200,
    parent,								// Parent Window
    NULL,								  // No Menu
    hInstance,					  // Instance
    NULL )))							// Dont Pass Anything To WM_CREATE
  {
    MessageBox( NULL, "Window Creation Error.","ERROR", MB_OK | MB_ICONEXCLAMATION );
    return FALSE;								// Return false
  }

  ShowWindow(hWnd,SW_SHOW);						// Show The Window
  SetForegroundWindow(hWnd);					// Slightly Higher Priority
  SetFocus(hWnd);									    // Sets Keyboard Focus To The Window

  return( hWnd );

  }
