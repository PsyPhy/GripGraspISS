//---------------------------------------------------------------------------

#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "../Include/glut.h"

#include "OpenGLColors.h"

#define FONT_DEFAULT "Palatino Linotype" //"Arial Bold"
#define FONT_NCHARS  255
#define FONT_HEIGHT   48
#define FONT_WIDTH    100

namespace PsyPhy {

class OpenGLWindow

{

private:

  HINSTANCE hInstance;	// Holds The Instance Of The Application
  HDC	    hDC;		    // Private GDI Device Context
  HGLRC     hRC;		    // Permanent Rendering Context

  int       lastInput;

  int       pointerLastX;
  int       pointerLastY;

  int       mouseDeltaX;
  int       mouseDeltaY;

protected:
public:

  HWND      hWnd;	    	// Holds Our Window Handle
  HWND      hParent;
  DWORD		dwExStyle;				// Window Extended Style
  DWORD		dwStyle;				  // Window Style

  int width;
  int height;
    
  int  Colordepth;    // bpp
  bool FullScreen;    // fullscreen flag
  bool Border;
  bool DoubleBuffer;  // double buffer flag
  bool Stereo;        // stereo flag
  unsigned long DisplayFrequency;

  void (*keyboard_callback)( int key );
  bool (*event_callback)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  OpenGLWindow( void );
  bool Create( HWND parent, char *title, int x, int y, int w, int h );
  void SetPosition( int x, int y, int w, int h );
  void Destroy( void );
  void Activate( void );
  bool RunOnce( void );
  void Swap( void );
  void Clear( int color = WHITE );
  void Clear( double r, double g, double b );
  void SetKeyboardCallback( void (*callback)( int key ) );
  void SetEventCallback( bool (*callback)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) );

  int GetInput( double timeout = -1.0 );
  int Sleep( double seconds );

  int MouseDeltaX( void );
  int MouseDeltaY( void );

	void OpenGLWindow::printf(GLuint x, GLuint y, double scale, char* format, ...);

  LRESULT WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

};

//---------------------------------------------------------------------------


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
OpenGLWindow *CreateMainGlWindow( int argc, char *argv[] );

};
