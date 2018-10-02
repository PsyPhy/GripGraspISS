/***************************************************************************/

#ifndef OpenGLViewpointsH
#define OpenGLViewpointsH 

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#include <gl\gl.h>
#include <gl\glu.h>

#include "../Useful/fMessageBox.h"
#include "OpenGLObjects.h"

namespace PsyPhy {

/* 
 * Default values for a Viewpoint object. 
 */
#define DEFAULT_IPD       6.0
#define DEFAULT_FOV       30.0
#define DEFAULT_NEAREST   1.0
#define DEFAULT_FARTHEST  1000.0


//---------------------------------------------------------------------------

/*
 * Viewpoints are built on top of OpenGLObject to take advantage of the
 *  position and orientation properties and methods.
 * It also means that they can have a color and texture, but these are
 *  ignored.
 */

typedef enum { LEFT_EYE = 0, RIGHT_EYE = 1, CYCLOPS = -1 } Eye;


class Viewpoint : public OpenGLObject {

private:

protected:

public:

  double ipd;       // inter-pupillary distance
  double fov;       // vertical field-of-view in degrees
  double nearest;   // near clipping plane
  double farthest;  // far clipping plane

  Viewpoint( double ipd = DEFAULT_IPD, double fov = DEFAULT_FOV, 
    double nearest = DEFAULT_NEAREST, double farthest = DEFAULT_FARTHEST );

  virtual void Apply( OpenGLWindow *wnd, Eye eye = CYCLOPS );

  void SetPosition( double p[3] );
  void SetPosition( double x, double y, double z );
  void SetOrientation( double m[3][3] );
  void SetOrientation( Quaternion q );
  void SetOrientation( double angle, const double axis[3] );
  void SetOrientation( double roll, double pitch, double yaw );
  void SetPose( Pose &pose );

  void SetOffset( double p[3] );
  void SetOffset( double x, double y, double z );
  void SetAttitude( double m[3][3] );
  void SetAttitude( Quaternion q );
  void SetAttitude( double angle, const double axis[3] );
  void SetAttitude( double roll, double pitch, double yaw );

};


class OrthoViewpoint : public OpenGLObject {

private:

protected:

public:

  double min_x;    // Specify the limits in user coordinates,
  double max_x;
  double min_y;
  double max_y;

  double nearest;   // near clipping plane
  double farthest;  // far clipping plane

  bool square;

  OrthoViewpoint( double min_x = -1.0, double max_x = 1.0,
                  double min_y = -1.0, double max_y = 1.0,
                  double min_depth = DEFAULT_NEAREST, 
                  double max_depth = DEFAULT_FARTHEST,
                  bool square = false );
  void Apply( OpenGLWindow *wnd, Eye eye = CYCLOPS );

};



};

extern PsyPhy::Viewpoint **CreateViewpointArray( int n );

#endif
