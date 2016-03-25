/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"

#include "OpenGLColors.h"
#include "OpenGLUseful.h"
#include "OpenGLObjects.h"
#include "OpenGLViewpoints.h"


/********************************************************************************/

Viewpoint::Viewpoint( double i, double f, double nr, double fr ) {

  ipd = i;
  fov = f;
  nearest = nr;
  farthest = fr;

  OpenGLObject();   // Do what every OpenGLObject does at creation.
}

/*********************************************************************************/

/*
 * Set the position of an object either from a 3D vector or from 3 scalar values.
 */

void Viewpoint::SetPosition( Vector3 p ) {
  CopyVector( position, p );
  gl_position[X] = - position[X];
  gl_position[Y] = - position[Y];
  gl_position[Z] = - position[Z];
}

void Viewpoint::SetPosition( double x, double y, double z ) {
  position[X] = x;
  position[Y] = y;
  position[Z] = z;
  gl_position[X] = - position[X];
  gl_position[Y] = - position[Y];
  gl_position[Z] = - position[Z];
}

/*
 * Offset the object from its control position.
 */

void Viewpoint::SetOffset( Vector3 p ) {
  CopyVector( offset, p );
  gl_offset[X] = - offset[X];
  gl_offset[Y] = - offset[Y];
  gl_offset[Z] = - offset[Z];
}

void Viewpoint::SetOffset( double x, double y, double z ) {
  offset[X] = x;
  offset[Y] = y;
  offset[Z] = z;
  gl_offset[X] = - offset[X];
  gl_offset[Y] = - offset[Y];
  gl_offset[Z] = - offset[Z];
}

/********************************************************************************/

// Viewpoints set the inverse as the rotation matrix.
void Viewpoint::SetOrientation( Matrix3x3 m ) {

	int i, j;

	Matrix3x3 inverse;

	CopyMatrix( orientation, m );
	InvertMatrix( inverse, m );

	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			gl_orientation[ i * 4 + j ] = inverse[i][j];
		}
		gl_orientation[ i * 4 + j ] = 0.0;
	}
	for ( int j = 0; j < 3; j++ ) {
		gl_orientation[ i * 4 + j ] = 0.0;
	}
	gl_orientation[ i * 4 + j ] = 1.0;
}

void Viewpoint::SetOrientation( double angle, Vector3 axis ) {

  Matrix3x3 m;

  SetRotationMatrix( m, ToRadians( angle ), axis );
  SetOrientation( m );

}

void Viewpoint::SetOrientation( double roll, double pitch, double yaw ) {

  Matrix3x3 m;

  roll = ToRadians( roll );
  pitch = ToRadians( pitch );
  yaw = ToRadians( yaw );

  m[X][X] = cos(yaw) * cos(roll);
  m[Y][X] = cos(yaw) * sin(roll);
  m[Z][X] = -sin(yaw);

  m[X][Y] = sin(pitch) * sin(yaw) * cos(roll) - cos(pitch) * sin(roll);
  m[Y][Y] = sin(pitch) * sin(yaw) * sin(roll) + cos(pitch) * cos(roll);
  m[Z][Y] = cos(yaw) * sin(pitch);

  m[X][Z] = cos(pitch) * sin(yaw) * cos(roll) + sin(pitch) * sin(roll);
  m[Y][Z] = cos(pitch) * sin(yaw) * sin(roll) - sin(pitch) * cos(roll);
  m[Z][Z] = cos(yaw) * cos(pitch);

  SetOrientation( m );

}

/********************************************************************************/

// Viewpoints set the inverse as the rotation matrix.
void Viewpoint::SetAttitude( Matrix3x3 m ) {

	int i, j;

	Matrix3x3 inverse;

	CopyMatrix( attitude, m );
	InvertMatrix( inverse, m );

	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			gl_attitude[ i * 4 + j ] = inverse[i][j];
		}
		gl_attitude[ i * 4 + j ] = 0.0;
	}
	for ( int j = 0; j < 3; j++ ) {
		gl_attitude[ i * 4 + j ] = 0.0;
	}
	gl_attitude[ i * 4 + j ] = 1.0;
}

void Viewpoint::SetAttitude( double angle, Vector3 axis ) {

  Matrix3x3 m;

  SetRotationMatrix( m, ToRadians( angle ), axis );
  SetAttitude( m );

}

void Viewpoint::SetAttitude( double roll, double pitch, double yaw ) {

  Matrix3x3 m;

  roll = ToRadians( roll );
  pitch = ToRadians( pitch );
  yaw = ToRadians( yaw );

  m[X][X] = cos(yaw) * cos(roll);
  m[Y][X] = cos(yaw) * sin(roll);
  m[Z][X] = -sin(yaw);

  m[X][Y] = sin(pitch) * sin(yaw) * cos(roll) - cos(pitch) * sin(roll);
  m[Y][Y] = sin(pitch) * sin(yaw) * sin(roll) + cos(pitch) * cos(roll);
  m[Z][Y] = cos(yaw) * sin(pitch);

  m[X][Z] = cos(pitch) * sin(yaw) * cos(roll) + sin(pitch) * sin(roll);
  m[Y][Z] = cos(pitch) * sin(yaw) * sin(roll) - sin(pitch) * cos(roll);
  m[Z][Z] = cos(yaw) * cos(pitch);

  SetAttitude( m );

}

/********************************************************************************/

void Viewpoint::Apply( OpenGLWindow *wnd, Eye eye ) {

  double aspect;


  switch( eye ) {

  case LEFT_EYE:
    glViewport( 0, 0, wnd->width / 2, wnd->height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (double) wnd->width / (double) wnd->height / 2.0;
    gluPerspective( fov, aspect, nearest, farthest );
    glTranslated( - ipd, 0.0, 0.0 );
    break;

  case RIGHT_EYE:
    glViewport(wnd->width / 2, 0, wnd->width / 2, wnd->height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (double) wnd->width / (double) wnd->height / 2.0;
    gluPerspective( fov, aspect, nearest, farthest );
    glTranslated( ipd, 0.0, 0.0 );
    break;

  default:
    glViewport(0, 0, wnd->width, wnd->height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (double) wnd->width / (double) wnd->height;
    gluPerspective( fov, aspect, nearest, farthest );
    break;

  }

  // This additional translation and rotationallows us to change the 
  // control point of the viewpoint, i.e. where it is looking
  // when it's postion is zero.
  glMultMatrixd( gl_attitude );
  glTranslatedv( gl_offset );

  // Set up the local reference frame according to
  // the viewpoint's position and orientation.
  glMultMatrixd( gl_orientation );
  glTranslatedv( gl_position );


}

/********************************************************************************/

OrthoViewpoint::OrthoViewpoint( double min_x, double max_x, 
                                double min_y, double max_y, 
                                double min_depth, double max_depth,
                                bool square ) {

  this->min_x = min_x;
  this->max_x = max_x;
  this->min_y = min_y;
  this->max_y = max_y;
  this->nearest = min_depth;
  this->nearest = max_depth;
  this->square = square;

  OpenGLObject();   // Do what every OpenGLObject does at creation.
}

void OrthoViewpoint::Apply( OpenGLWindow *wnd, Eye eye ) {


  glViewport(0, 0, wnd->width, wnd->height );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( min_x, max_x, min_y, max_y );

  // This additional translation and rotationallows us to change the 
  // control point of the viewpoint, i.e. where it is looking
  // when it's postion is zero.
  glMultMatrixd( gl_attitude );
  glTranslatedv( gl_offset );

  // Set up the local reference frame according to
  // the viewpoint's position and orientation.
  glMultMatrixd( gl_orientation );
  glTranslatedv( gl_position );

}


