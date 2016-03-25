// TestOpenGL.cpp : Defines the entry point for the console application.
// This program appears to test how to open an OpenGL window and draw into it.

#include "stdafx.h"
#include <gl/gl.h>

static const char *s_class_name  = "TestOpenGL";
static const char *s_window_name = "TestOpenGL";

static BOOL    s_rgba        = TRUE;
static BOOL    s_lighting    = FALSE;
static BOOL    s_benchmark   = FALSE;
static BOOL    s_remote      = FALSE;
static BOOL    s_lock_arrays = TRUE;
static BOOL    s_vcull       = TRUE;

static HPALETTE s_hPalette = NULL;
static HWND     s_hWnd  = NULL;
static HGLRC    s_hglrc = NULL;
static HDC      s_hDC   = NULL;
static int      s_bpp   = 8;

static int		s_winwidth = 1024;
static int		s_winheight = 768;
static int		s_winx = 10;
static int		s_winy = 0;

static GLint s_lit_tex_indexes[3];
/* struct used to manage color ramps */
struct colorIndexState {
    GLfloat amb[3];		/* ambient color / bottom of ramp */
    GLfloat diff[3];	/* diffuse color / middle of ramp */
    GLfloat spec[3];	/* specular color / top of ramp */
    GLfloat ratio;		/* ratio of diffuse to specular in ramp */
    GLint indexes[3];	/* where ramp was placed in palette */
};

#define NUM_COLORS (sizeof(s_colors) / sizeof(s_colors[0]))
struct colorIndexState s_colors[] = {
    {
        { 0.0F, 0.0F, 0.0F },
        { 1.0F, 0.0F, 0.0F },
        { 1.0F, 1.0F, 1.0F },
        0.75F, { 0, 0, 0 },
    },
    {
        { 0.0F, 0.05F, 0.05F },
        { 0.9F, 0.0F, 1.0F },
        { 1.0F, 1.0F, 1.0F },
        1.0F, { 0, 0, 0 },
    },
    {
        { 0.0F, 0.0F, 0.0F },
        { 1.0F, 0.9F, 0.1F },
        { 1.0F, 1.0F, 1.0F },
        0.75F, { 0, 0, 0 },
    },
    {
        { 0.0F, 0.0F, 0.0F },
        { 0.1F, 1.0F, 0.9F },
        { 1.0F, 1.0F, 1.0F },
        0.75F, { 0, 0, 0 },
    },
};


/*********************************************************************************/

static BOOL SetupPixelFormat( HDC hDC )
{
   unsigned char pixelType = s_rgba ? PFD_TYPE_RGBA : PFD_TYPE_COLORINDEX;
   PIXELFORMATDESCRIPTOR pfd = 
   {
      sizeof( PIXELFORMATDESCRIPTOR ),
      1,
      PFD_DRAW_TO_WINDOW |	PFD_SUPPORT_OPENGL |	PFD_DOUBLEBUFFER,
      pixelType,
      s_bpp,
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      16,				// 16-bit depth buffer
      0,				// no stencil buffer
      0,				// no aux buffers
      PFD_MAIN_PLANE,	// main layer */
      0,	
      0, 0, 0
   };
   int  selected_pf;
   
   if ( ( selected_pf = ChoosePixelFormat( hDC, &pfd ) ) == 0 ) {
      MessageBox( 0, "Failed to find acceptable pixel format", "TestOpenGL Error", MB_ICONERROR | MB_OK );
      return FALSE;
   }
   if ( !SetPixelFormat( hDC, selected_pf, &pfd) ) {
      MessageBox( 0, "Failed to SetPixelFormat", "TestOpenGL Error", MB_ICONERROR | MB_OK );
      return FALSE;
   }
   return TRUE;
}


/*********************************************************************************/

static void SetupPalette( HDC hDC )
{
    PIXELFORMATDESCRIPTOR  pfd;
    LOGPALETTE            *pPal;
    int                    paletteSize;
    int                    pixelFormat = GetPixelFormat(hDC);
    
    DescribePixelFormat( hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (!(pfd.dwFlags & PFD_NEED_PALETTE || pfd.iPixelType == PFD_TYPE_COLORINDEX))
        return;
    
    paletteSize = 1 << pfd.cColorBits;

    pPal                = (LOGPALETTE * ) malloc( sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY) );
    pPal->palVersion    = 0x300;
    pPal->palNumEntries = paletteSize;
    
    /* start with a copy of the current system palette */
    GetSystemPaletteEntries( hDC, 0, paletteSize, &pPal->palPalEntry[0] );
    
    if ( pfd.iPixelType == PFD_TYPE_RGBA )
    {
        /* fill in an RGBA color palette */
        int redMask = (1 << pfd.cRedBits) - 1;
        int greenMask = (1 << pfd.cGreenBits) - 1;
        int blueMask = (1 << pfd.cBlueBits) - 1;
        int i;
        
        for (i=0; i<paletteSize; ++i) 
        {
            pPal->palPalEntry[i].peRed =
                (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
            pPal->palPalEntry[i].peGreen =
                (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
            pPal->palPalEntry[i].peBlue =
                (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
            pPal->palPalEntry[i].peFlags = 0;
        }
    } 
    else 
    {
        /* fill in a Color Index ramp color palette */
        int numRamps = NUM_COLORS;
        int rampSize = (paletteSize - 20) / numRamps;
        int extra = (paletteSize - 20) - (numRamps * rampSize);
        int i, r;
        
        for (r=0; r<numRamps; ++r) 
        {
            int rampBase = r * rampSize + 10;
            PALETTEENTRY *pe = &pPal->palPalEntry[rampBase];
            int diffSize = (int) (rampSize * s_colors[r].ratio);
            int specSize = rampSize - diffSize;
            
            for (i=0; i<rampSize; ++i) 
            {
                GLfloat *c0, *c1;
                GLint a;
                
                if (i < diffSize) 
                {
                    c0 = s_colors[r].amb;
                    c1 = s_colors[r].diff;
                    a = (i * 255) / (diffSize - 1);
                } 
                else 
                {
                    c0 = s_colors[r].diff;
                    c1 = s_colors[r].spec;
                    a = ((i - diffSize) * 255) / (specSize - 1);
                }
                
                pe[i].peRed = (BYTE) (a * (c1[0] - c0[0]) + 255 * c0[0]);
                pe[i].peGreen = (BYTE) (a * (c1[1] - c0[1]) + 255 * c0[1]);
                pe[i].peBlue = (BYTE) (a * (c1[2] - c0[2]) + 255 * c0[2]);
                pe[i].peFlags = PC_NOCOLLAPSE;
            }
            
            s_colors[r].indexes[0] = rampBase;
            s_colors[r].indexes[1] = rampBase + (diffSize-1);
            s_colors[r].indexes[2] = rampBase + (rampSize-1);
        }
        s_lit_tex_indexes[0] = 0;
        s_lit_tex_indexes[1] = (GLint)(rampSize*s_colors[0].ratio)-1;
        s_lit_tex_indexes[2] = rampSize-1;
        
        for (i=0; i<extra; ++i) 
        {
            int index = numRamps*rampSize+10+i;
            PALETTEENTRY *pe = &pPal->palPalEntry[index];
            
            pe->peRed = (BYTE) 0;
            pe->peGreen = (BYTE) 0;
            pe->peBlue = (BYTE) 0;
            pe->peFlags = PC_NOCOLLAPSE;
        }
    }
    
    s_hPalette = CreatePalette( pPal );
    free( pPal );
    
    if ( s_hPalette )
    {
        SelectPalette( hDC, s_hPalette, FALSE );
        RealizePalette( hDC );
    }
}

/*********************************************************************************/

static void SetDefaults( void )
{
   // setup a default color
   glColor3f( 1.0F, 1.0F, 1.0F );
   // Define the background color
   glClearColor( 0.0F, 0.0F, 0.5F, 1.0F );

#if 0
   glClearDepth( 1.0F );
   glDepthFunc( GL_LEQUAL );
   glEnable( GL_DEPTH_TEST );
   glDepthMask( GL_TRUE );
#endif

   // setup perspective
   glMatrixMode( GL_PROJECTION );
   glFrustum( -0.5, 0.5, -0.5, 0.5, 1.0, 1000.0 );

   // setup viewer
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0F, 0.0F, -3.0F );

   // setup Z-buffering
   // setup shade model
   glShadeModel( GL_SMOOTH );

   // setup lighting
   if ( !s_lighting ) {
      glDisable( GL_LIGHTING );
      glDisable( GL_LIGHT0 );
   }
   else
   {
      GLfloat light0Pos[4] = { 0.70F, 0.70F, 1.25F, 0.00F };
      GLfloat matAmb[4] = { 0.01F, 0.01F, 0.01F, 1.00F };
      GLfloat matDiff[4] = { 0.65F, 0.05F, 0.20F, 0.60F };
      GLfloat matSpec[4] = { 0.50F, 0.50F, 0.50F, 1.00F };
      GLfloat matShine = 20.00F;

      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
      glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
      glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
      glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
      glMaterialf(GL_FRONT, GL_SHININESS, matShine);
   }

   // setup cull mode
   glEnable( GL_CULL_FACE );

#ifdef GL_SGI_cull_vertex
   if ( s_vcull ) {
       GLfloat eyeDir[] = { 0.0F, 0.0F, 1.0F, 0.0F };
       glEnable(GL_CULL_VERTEX_SGI);
       if ( CullParameterfvSGI )
           CullParameterfvSGI( GL_CULL_VERTEX_EYE_POSITION_SGI, eyeDir );
   }
   else {
       glDisable(GL_CULL_VERTEX_SGI);
   }
#endif

   // setup dither
   glEnable( GL_DITHER );

}

/*********************************************************************************/

static LRESULT APIENTRY WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
   switch ( msg )
   {
   case WM_CREATE:

		s_hDC = GetDC( hWnd );

		if ( !SetupPixelFormat( s_hDC ) ) exit( 1 );
		SetupPalette( s_hDC );
		s_hglrc = wglCreateContext( s_hDC );
		wglMakeCurrent( s_hDC, s_hglrc );

		SetDefaults();

		return 0;

   case WM_DESTROY:
      return 0;
   case WM_CHAR:	
      break;
   case WM_SIZE:
       if ( s_hglrc )
       {
           int winWidth  = ( int ) LOWORD( lParam );
           int winHeight = ( int ) HIWORD( lParam );
           glViewport( 0, 0, winWidth, winHeight );
       }
       return 0;
   case WM_PALETTECHANGED:
       if (s_hPalette != NULL && (HWND) wParam != hWnd) 
       {
           UnrealizeObject( s_hPalette );
           SelectPalette( s_hDC, s_hPalette, FALSE );
           RealizePalette( s_hDC );
           return 0;
       }
       break;
   case WM_QUERYNEWPALETTE:
       if ( s_hPalette != NULL ) 
       {
           UnrealizeObject( s_hPalette );
           SelectPalette( s_hDC, s_hPalette, FALSE );
           RealizePalette( s_hDC );
           return TRUE;
       }
       break;
   case WM_COMMAND:
      break;
   }

   return DefWindowProc( hWnd, msg, wParam, lParam);
}

/*********************************************************************************/

// BOOL InitWindow( HINSTANCE hInstance, LPSTR lpszCmdLine ) {
BOOL InitWindow( void ) {

   WNDCLASS wc;
   
   wc.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = WndProc;
   wc.cbClsExtra  = 0;
   wc.cbWndExtra  = 0;
   wc.hInstance   = NULL;
   wc.hIcon       = LoadIcon(NULL, IDI_APPLICATION);
   wc.hCursor     = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName = NULL;
   wc.lpszClassName = s_class_name;
   
   RegisterClass( &wc );
   
   {
      HDC hdc = GetDC( 0 );
      s_bpp = GetDeviceCaps( hdc, BITSPIXEL );
      s_rgba = 1;
      ReleaseDC( 0, hdc );
   }


   /*
   ** create a window of the previously defined class
   */
   s_hWnd = CreateWindow( s_class_name,
                          s_window_name, 
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                          s_winx, s_winy, 
                          s_winwidth, s_winheight,
                          NULL,
                          NULL,
 // hInstance,
                          NULL,
                          NULL );

   if ( s_hWnd != NULL ) return TRUE;
   return FALSE;

#if 0
   glClearDepth( 1.0F );
   glDepthFunc( GL_LEQUAL );
   glEnable( GL_DEPTH_TEST );
   glDepthMask( GL_TRUE );
#endif

   // setup perspective
   glMatrixMode( GL_PROJECTION );
   glFrustum( -0.5, 0.5, -0.5, 0.5, 1.0, 1000.0 );

   // setup viewer
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0F, 0.0F, -3.0F );

   // setup Z-buffering
   // setup shade model
   glShadeModel( GL_SMOOTH );

   // setup lighting
   if ( !s_lighting )
   {
      glDisable( GL_LIGHTING );
      glDisable( GL_LIGHT0 );
   }
   else
   {
      GLfloat light0Pos[4] = { 0.70F, 0.70F, 1.25F, 0.00F };
      GLfloat matAmb[4] = { 0.01F, 0.01F, 0.01F, 1.00F };
      GLfloat matDiff[4] = { 0.65F, 0.05F, 0.20F, 0.60F };
      GLfloat matSpec[4] = { 0.50F, 0.50F, 0.50F, 1.00F };
      GLfloat matShine = 20.00F;

      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
      glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
      glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
      glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
      glMaterialf(GL_FRONT, GL_SHININESS, matShine);
   }

}

/*********************************************************************************/

bool StartWindow( void ) {

	if ( InitWindow() ) {
		ShowWindow( s_hWnd, 1 );
		return( true );
	}
	else return( false );

}

/*********************************************************************************/

void ShutdownWindow( void )
{

   if ( s_hglrc )
   {
      wglMakeCurrent( NULL, NULL );
      wglDeleteContext( s_hglrc );
   }
   ReleaseDC( s_hWnd, s_hDC );

}

/*********************************************************************************/

void Swap ( void ) {
   SwapBuffers( s_hDC );
}


/*********************************************************************************/

int _tmain(int argc, _TCHAR* argv[])
{
	if ( StartWindow() ) {

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glColor3f( 1.0, 0.0, 0.0 );
		glBegin( GL_TRIANGLE_STRIP );
			glNormal3f( 0, 0, 1 );
			glVertex3f( 0, 0, 0 );
			glVertex3f( 0, 0.5, 0);
			glVertex3f( 0.5, 0.5, 0 );
//			glVertex3f( 0.5, 0, 0 );
			glVertex2f( 0, 0 );
		glEnd();
		Swap();

	}

	Sleep(5000);
	ShutdownWindow();

  return( 0 );}

