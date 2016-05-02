/***************************************************************************/

#ifndef OpenGLObjectsH
#define OpenGLObjectsH 

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <float.h>


#include "../Useful/Useful.h"
#include "../VectorsMixin/VectorsMixin.h"

#include <gl\gl.h>
#include <gl\glu.h>
#include "../Include/glaux.h"
#include "../Useful/OpenGLUseful.h"

#include "OpenGLWindows.h"
#include "OpenGLColors.h"
#include "OpenGLTextures.h"

namespace PsyPhy {

/* 
 * Default values for spheres, cylinders, etc. 
 */
#define DEFAULT_STACKS       32
#define DEFAULT_SLICES       32
#define DEFAULT_FACETS		 60
#define DEFAULT_RADIUS       10.0
#define DEFAULT_HEIGHT       20.0
#define DEFAULT_WIDTH        10.0
#define DEFAULT_THICKNESS     1.0
#define DEFAULT_LENGTH       25.0

/*
 * Maximum number of comonent objects in a given assembly.
 * Assemblies should be implemented as linked lists so as not
 *  to impose a limit, but I'm too lazy.
 */
#define MAX_COMPONENTS  100

/*
 * Maximum number of points in a polygon for structures like an extrusion.
 */
#define MAX_VERTICES 255

/***************************************************************************/

/*
 * The basic class of OpenGL graphics objects.
 * Contains all the common properties, such as the
 * position, orientation, color, texture, etc.
 */

class OpenGLObject : public VectorsMixin {

private:

protected:

public:

  OpenGLObject( void );

  void  PrepDraw();
  virtual void  Draw();
  void  FinishDraw();

  virtual void  SetPosition( const Vector3 p );
  virtual void  SetPosition( double x, double y, double z );
  void  GetPosition( Vector3 p );

  virtual void  SetOrientation( Matrix3x3 m );
  virtual void  SetOrientation( double angle, const Vector3 axis );
  virtual void  SetOrientation( double roll, double pitch, double yaw );
  void  GetOrientation( Matrix3x3 m );

  void  SetAttitude( Matrix3x3 m );
  void  SetAttitude( double angle, const Vector3 axis );
  void  SetAttitude( double roll, double pitch, double yaw );
  void  GetAttitude( Matrix3x3 m );

  virtual void  SetOffset( const Vector3 p );
  virtual void  SetOffset( double x, double y, double z );
  void  GetOffset( Vector3 p );
 
  void SetColor( GLfloat c[4] );
  void SetColor( float r, float g, float b, float a = 1.0 );
  void SetColor( double r, double g, double b, double a = 1.0 ) { 
	  SetColor( ((float) r), ((float) g), ((float) b), ((float) a ));
  }
  void SetColor( int index );

  void  SetTexture( Texture *texture );
  void  SetTexture( Texture *texture, double repeat );
  void  SetTexture( Texture *texture, double repeat_x, double repeat_y );

  void  Enable( void );
  void  Disable( void );
  bool  IsEnabled( void );


  bool    enabled;
  Vector3  position;
  Matrix3x3  orientation;
  double  gl_position[3];
  double  gl_orientation[16];

  Vector3  offset;
  Matrix3x3  attitude;
  double  gl_offset[3];
  double  gl_attitude[16];

  GLfloat color[4];
  Texture *texture;
  GLfloat umag, vmag;

  GLint   list;

};

/***************************************************************************/

/*
 * A collection of OpenGL graphics objects that are drawn together.
 */

class Assembly : public OpenGLObject {

private:

  OpenGLObject  *component[MAX_COMPONENTS];
  int components;

protected:

public:

  Assembly( void );
  ~Assembly( void );
  void  AddComponent( OpenGLObject *c );
  void  RemoveComponent( OpenGLObject *c );
  virtual void Draw();

};

/***************************************************************************/

/*
 * Some basic 3D shapes.
 */

//---------------------------------------------------------------------------

class Box : public OpenGLObject {

private:

protected:

public:

  double width;
  double height;
  double length;

  Box( double width = DEFAULT_WIDTH, double height = DEFAULT_WIDTH, double length = DEFAULT_WIDTH );
  void  Draw();

};

//---------------------------------------------------------------------------

class WindowFrame : public OpenGLObject {

private:

protected:

public:

  double height;
  double width;
  double thickness;

  WindowFrame( double width = DEFAULT_WIDTH, double length = DEFAULT_WIDTH, double thickness = DEFAULT_THICKNESS );
  void  Draw();

};

//---------------------------------------------------------------------------

class Slab : public OpenGLObject {

private:

protected:

public:

  double length;
  double width;
  double thickness;

  Slab( double width = DEFAULT_WIDTH, double length = DEFAULT_WIDTH, double thickness = DEFAULT_THICKNESS );
  void Draw();

};

//---------------------------------------------------------------------------

class Patch : public OpenGLObject {

private:

protected:

public:

  double length;
  double width;

  Patch( double width = DEFAULT_WIDTH, double length = DEFAULT_WIDTH );
  void  Draw();

};

//---------------------------------------------------------------------------

class Bar : public OpenGLObject {

private:

protected:

public:

  double length;
  double width;
  double thickness;
  bool   capped;

  Bar( double length = DEFAULT_LENGTH, double width = DEFAULT_THICKNESS, double height = DEFAULT_THICKNESS, bool cap = false );
  void  Draw();

};

//---------------------------------------------------------------------------

class Extrusion : public OpenGLObject {

private:

protected:

public:

  double length;
  double vertex[MAX_VERTICES][2];
  int    vertices;

  Extrusion( double length, double vertex[][2], int n );
  void  Draw();

};

//---------------------------------------------------------------------------

class Sphere : public OpenGLObject {

private:

protected:

public:

  double radius;
  int    slices;
  int    stacks;
  GLUquadric *quad;

  Sphere( double radius = DEFAULT_RADIUS, int slices = DEFAULT_SLICES, int stacks = DEFAULT_STACKS );
	void SetRadius( double radius );
  void  Draw();

};

//---------------------------------------------------------------------------

class Ellipsoid : public OpenGLObject {

private:

  double x_distortion;
  double y_distortion;
  double z_distortion;
  double max_radius;

protected:

public:

  double x_radius;
  double y_radius;
  double z_radius;

  int    slices;
  int    stacks;
  GLUquadric *quad;

  Ellipsoid( double x_radius, double y_radius, double z_radius,
    int slices = DEFAULT_SLICES, int stacks = DEFAULT_STACKS );

  Ellipsoid( double xy_radius, double z_radius,
    int slices = DEFAULT_SLICES, int stacks = DEFAULT_STACKS );

  void  Draw();

};
//---------------------------------------------------------------------------

class Disk : public OpenGLObject {

private:

protected:

public:

  double inner_radius;
  double outer_radius;
  int    slices;
  int    loops;
  GLUquadric *quad;

  Disk( double outer_radius = DEFAULT_RADIUS, double inner_radius = 0.0, 
    int slices = DEFAULT_SLICES, int loops = DEFAULT_STACKS );

  void  Draw();
  void SetRadius( double outer_radius, double inner_radius = 0.0 );

};

//---------------------------------------------------------------------------

class Cylinder : public OpenGLObject {

private:

protected:

public:

  double top_radius;
  double bottom_radius;
  double height;

  int    facets;

  Cylinder( double top_radius = DEFAULT_RADIUS, double bottom_radius = DEFAULT_RADIUS, 
              double height = DEFAULT_HEIGHT, 
              int facets = DEFAULT_FACETS );

  void  Draw();
  void  SetRadius( double top_radius, double bottom_radius = NaN );
  void  SetHeight( double height );

};

//---------------------------------------------------------------------------

class Frustrum : public OpenGLObject {

private:

  void  draw_polygons( void );
  void  draw_end( void );

protected:

public:

  bool multi_color;

  double near_vertex[3];
  double far_vertex[3];
  
  Frustrum( double near_vertex[3], double far_vertex[3] );

  void  Draw();

};

//---------------------------------------------------------------------------

class Circle : public OpenGLObject {

private:

protected:

public:

  double radius;
  int    slices;
  int    loops;

  Circle( double radius = DEFAULT_RADIUS, 
    int slices = DEFAULT_SLICES, int loops = DEFAULT_STACKS );

  void  Draw();

};

//---------------------------------------------------------------------------

// A rectangle with a circular hole in it
class Hole : public OpenGLObject {

private:

protected:

public:

  double  radius;
  double  width;
  double  height;

  Hole( double radius = DEFAULT_RADIUS, double width = 1.0, double height = 1.0 );
  void SetRadius( double value );

  void  Draw();

};

//---------------------------------------------------------------------------

enum { FLOOR = 0, CEILING, LEFT_WALL, RIGHT_WALL, FRONT_WALL, BACK_WALL };

class Room : public OpenGLObject {

private:

protected:

public:

  double width;
  double height;
  double length;

  Texture wall_texture[6];
  int     wall_color[6];

  void Box( double width = DEFAULT_WIDTH, double height = DEFAULT_WIDTH, double length = DEFAULT_WIDTH );

  void SetColor( int surface, int color );
  void SetColors( int colors[6] );
  void SetTexture( int surface, Texture *texture );
  void SetTextures( int colors[6] );

  void  Draw();

};

};

#endif
