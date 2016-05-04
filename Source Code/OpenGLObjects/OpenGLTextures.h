/***************************************************************************/

#ifndef OpenGLTexturesH
#define OpenGLTexturesH 

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "../Include/glut.h"

namespace PsyPhy {

class Texture {

private:

//  GLuint	        texture[10];			        // Storage For ten Texture
  int texture_index;
  AUX_RGBImageRec *TextureImage;					// Storage Space For The Image
  AUX_RGBImageRec *LoadBMP( const char *bitmap_filename );
  char bitmap_filename[256];

protected:

public:

  /*
   * A texture can be given a 'physical' size. The drawing routines
   *  will attempt to draw in such a way that one repetition of the
   *  texture image corresponds to a surface area of the specified size.
   * The physical size reverts to zero, in which case the repetition of
   *  the pattern depends on the repetition parameters given when the 
   *  texture is assigned to a given object (See GLObject::SetTexture()).
   */
  double u_length, v_length;  // Length of edge in user units.
  Texture( const char *filename, double x_length = 0.0, double y_length = 0.0 );
  int LoadBitmap( const char *filename );	// Load Bitmaps And Convert To Textures
  void Use();
  void Define();

};

};

#endif
