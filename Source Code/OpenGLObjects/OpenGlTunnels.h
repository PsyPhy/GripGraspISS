/***************************************************************************/

#ifndef OpenGLTunnelsH
#define OpenGLTunnelsH 

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "../Include/glaux.h"

#include "OpenGLWindows.h"
#include "OpenGLColors.h"
#include "OpenGLUseful.h"
#include "OpenGLTextures.h"


/* 
 * Default values for spheres, cylinders, etc. 
 */
#define DEFAULT_STACKS       32
#define DEFAULT_SLICES       32
#define DEFAULT_RADIUS       10.0
#define DEFAULT_HEIGHT       20.0
#define DEFAULT_WIDTH        10.0
#define DEFAULT_THICKNESS     1.0
#define DEFAULT_LENGTH       25.0

class TunnelSegment : public OpenGLObject {

private:
  
protected:

public:

  float length;
  float radius;

  void Draw();
  TunnelSegment( float length = 100.0, float radius = 25.0 );

};

class TunnelJunction : public OpenGLObject {

private:

  float T;
  
protected:

public:

  void Draw();

};


class Tunnel : public Assembly {

private:
  
protected:

public:
  
  Tunnel( void );

};



#endif
