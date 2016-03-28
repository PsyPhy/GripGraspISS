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

#include "OpenGLObjects.h"

#define USE_PARENT_COLOR  -1.0

using namespace PsyPhy;

/***************************************************************************/

// Initialize a gl position to zero.
static void _init_gl_displacement( double *m ) {
  m[0] = m[1] = m[2] = 0.0; 
}

// Initialize a homogeneous coordinate matrix (4x4) to the identity matrix.
 static void _init_gl_rotation( double *m ) {
  m[0] = m[5] = m[10] = m[15] = 1.0; 
  m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0;
}

/***************************************************************************/



OpenGLObject::OpenGLObject( void ) {

	texture = NULL;
	umag = 1.0;
	vmag = 1.0;

	enabled = true;
	list = -1;

	// By default an object has a zero position and orientation.	
	// Here we initialize the object's state and the corresponding GL equivalents.
	CopyVector( position, zeroVector );
	CopyMatrix( orientation, identityMatrix );
	CopyVector( offset, zeroVector );
	CopyMatrix( attitude, identityMatrix );

	_init_gl_rotation( gl_attitude );
	_init_gl_rotation( gl_orientation );
	_init_gl_displacement( gl_position );
	_init_gl_displacement( gl_offset );

	// By default, use the color of the parent or the previously drawn object.
	SetColor( 0.0, 0.0, 0.0, USE_PARENT_COLOR );

}

/*********************************************************************************/
/*

Each object has two positions and two orientations. The top level position and 
orientation determine the control point, the 'offset' and 'attitude' allow you 
to change the position and orientation of the actual drawn object with respect to
the control point. One way to think of it is that offset and attitude allow you 
to set the position and orientation of the rendered object when it is set to the
zero position and orientation.

*/



// Set the position of an object from a 3D vector.
void OpenGLObject::SetPosition( const Vector3 p ) {
  CopyVector( position, p );
  CopyVector( gl_position, position );
}

// Set the position of an object from XYZ scalar values.
void OpenGLObject::SetPosition( double x, double y, double z ) {
  position[X] = x;
  position[Y] = y;
  position[Z] = z;
  CopyVector( gl_position, position );
}

// Retrieve the position of the object into a vector.
void OpenGLObject::GetPosition( Vector3 p ) {
  CopyVector( p, position );
}

// Offset the object from its control position with a vector.
void OpenGLObject::SetOffset( const Vector3 p ) {
  CopyVector( offset, p );
  CopyVector( gl_offset, offset );
}
// Offset the object from its control position with XYZ scalars.
void OpenGLObject::SetOffset( double x, double y, double z ) {
  offset[X] = x;
  offset[Y] = y;
  offset[Z] = z;
  CopyVector( gl_offset, offset );
}
// Retrieve the offset of the object into a vector.
void OpenGLObject::GetOffset( Vector3 p ) {
  CopyVector( p, offset );
}

/*********************************************************************************/

/*
 * Set the orientation of an object either from a 3x3 rotation matrix or
 *  as defined by a 3D axis vector and a scalar angle in degrees.
 */

// Set the orientation of the object.
void OpenGLObject::SetOrientation( Matrix3x3 m ) {
	int i, j;
	CopyMatrix( orientation, m );
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			gl_orientation[ i * 4 + j ] = orientation[i][j];
		}
		gl_orientation[ i * 4 + j ] = 0.0;
	}
	for ( int j = 0; j < 3; j++ ) {
		gl_orientation[ i * 4 + j ] = 0.0;
	}
	gl_orientation[ i * 4 + j ] = 1.0;
}

// Set the orientation of the object.
void OpenGLObject::SetOrientation( double angle, const Vector3 axis ) {

  Matrix3x3 m;

  SetRotationMatrix( m, ToRadians( angle ), axis );
  SetOrientation( m );

}

// Set the orientation of the object.
void OpenGLObject::SetOrientation( double roll, double pitch, double yaw ) {

  double m[3][3];

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

void OpenGLObject::GetOrientation( Matrix3x3 m ) {
  CopyMatrix( m, orientation );
}

/*
 * Set the attitude of an object when it is at it's zero orientation.
 */

void OpenGLObject::SetAttitude( Matrix3x3 m ) {
	int i, j;
  CopyMatrix( attitude, m );
  for ( i = 0; i < 3; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      gl_attitude[ i * 4 + j ] = attitude[i][j];
    }
    gl_attitude[ i * 4 + j ] = 0.0;
  }
  for ( j = 0; j < 3; j++ ) {
    gl_attitude[ i * 4 + j ] = 0.0;
  }
  gl_attitude[ i * 4 + j ] = 1.0;
}

void OpenGLObject::SetAttitude( double angle, const Vector3 axis ) {

  Matrix3x3 m;

  SetRotationMatrix( m, ToRadians( angle ), axis );
  SetAttitude( m );

}

void OpenGLObject::SetAttitude( double roll, double pitch, double yaw ) {

  double m[3][3];

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

void OpenGLObject::GetAttitude( Matrix3x3 m ) {
  CopyMatrix( m, attitude );
}
/***************************************************************************/

void OpenGLObject::Enable( void ) { enabled = true; }
void OpenGLObject::Disable( void ) { enabled = false; }
bool OpenGLObject::IsEnabled( void ) { return( enabled ); }

/***************************************************************************/

/*
 * Set the color of an object either with an rgb vector, 
 *  by 3 rgb values, or by an index into a predefined color
 *  table. Defined values in the table are found in OpenGLColors.h.
 */
void OpenGLObject::SetColor( float c[4] ) {

  color[0] = c[0];
  color[1] = c[1];
  color[2] = c[2];
  color[3] = c[3];

}

void OpenGLObject::SetColor( float r, float g, float b, float a ) {

  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;

}

void OpenGLObject::SetColor( int index ) {
  SetColor( OpenGLColors[ index ] );
}

/***************************************************************************/

void OpenGLObject::SetTexture( Texture *texture ) {
  this->texture = texture;
  umag = vmag = 1.0;
}

void OpenGLObject::SetTexture( Texture *texture, double repeat ) {
  this->texture = texture;
  umag = vmag = repeat;
}

void OpenGLObject::SetTexture( Texture *texture, double repeat_x, double repeat_y ) {
  this->texture = texture;
  umag = repeat_x;
  vmag = repeat_y;
}
/***************************************************************************/

void OpenGLObject::PrepDraw( void ) {

  if ( ! enabled ) return;

  // If the color is defined, set it before drawing.
  // Otherwise continue in the color of the parent.
  if ( color[3] != USE_PARENT_COLOR ) glColor4fv( color );

  // Ditto for the texture.
  if ( texture ) {
    texture->Use();
  }

  // This assumes that we are in Model matrix mode.
  // Maybe we need to select that mode each time?
  // Also, it is essential to call FinishDraw() to
  // correctly pop the matrix stack after drawing.
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  // Set up the local reference frame according to
  // the object's position and orientation and draw
  // all child objects in that frame.
  glTranslatedv( gl_position );
  glMultMatrixd( gl_orientation );

  // This additional translation and rotationallows us to change the 
  // control point of the object, i.e. where is the object
  // when it's postion is zero.
  glTranslatedv( gl_offset );
  glMultMatrixd( gl_attitude );


}

/*
 * By default there is no actual Draw method.
 * This method must be overlayed by the derived classes.
 */

void OpenGLObject::Draw( void ) {

  if ( ! enabled ) return;

  PrepDraw();
  FinishDraw();

}

void OpenGLObject::FinishDraw( void ) {

  if ( ! enabled ) return;

  glPopMatrix();

}

/***************************************************************************/

/*
 * Constructor initializes the position and orientation to zero and 
 *  sets the number of component elements to 0.
 */

Assembly::Assembly( void ) {

  components = 0;   // Start as an empty structure.
  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

Assembly::~Assembly( void ) {

  for ( int i = 0; i < components; i++ ) {
    delete component[i];
  }
}

void Assembly::AddComponent( OpenGLObject *c ) {

  if ( components < MAX_COMPONENTS ) {
    component[ components ] = c;
    components++;
  }

}

void Assembly::RemoveComponent( OpenGLObject *c ) {

  int i;

  for ( i = 0; i < components; i++ ) {
    if ( component[ i ] == c ) break;
  }
  if ( i < components ) {
    /* We should free the object here. */
    for ( i = i + 1; i < components; i++ ) component[i - 1] = component[i];
    components--;
  }

}

/*
 * Draw sets up a local coordinate frame at the position and orientation  
 *  of the entire assembly, then draws the component elements within that frame.
 */

void Assembly::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();
  for ( int i = 0; i < components; i++ ) {
    component[i]->Draw();
    // Restore the color, in case the component changed it.
    // This does not work in the case where the assembly is supposed to use
    //  the parent color. Need to find a way to push the color on the stack
    //  or to query the color and save it to restore it later.
    if ( color[3] != USE_PARENT_COLOR ) glColor4fv( color );

  }
  FinishDraw();

}


/***************************************************************************/

Box::Box( double width, double height, double length ) {

  this->width = width;
  this->height = height;
  this->length = length;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void Box::Draw( void ) {

  double x = width / 2.0;
  double y = height / 2.0;
  double z = length / 2.0;

  if ( ! enabled ) return;
  PrepDraw();

  if ( texture ) {

    GLfloat u, v;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
    glEnable( GL_TEXTURE_2D );
    texture->Use();

    glBegin(GL_QUADS);

      if ( texture->u_length ) u = umag * width / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * height / texture->v_length;
      else v = vmag;

      glNormal3d( 0.0, 0.0, - 1.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, z );
      glTexCoord2f(u, 0.0f); glVertex3d(  x,  - y, z );
      glTexCoord2f(u, v); glVertex3d(   x,   y, z );
      glTexCoord2f(0.0f, v); glVertex3d(  - x, y, z );

      glNormal3d( 0.0, 0.0, 1.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(u, 0.0f); glVertex3d(   - x,  y, - z );
      glTexCoord2f(u, v); glVertex3d(   x,   y, - z );
      glTexCoord2f(0.0f, v); glVertex3d(  x,  - y, - z );

      if ( texture->u_length ) u = umag * length / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * width / texture->v_length;
      else v = vmag;

      glNormal3d( 0.0, - 1.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x,   y, - z );
      glTexCoord2f(u, 0.0f); glVertex3d( - x,   y,   z );
      glTexCoord2f(u, v); glVertex3d(   x,   y,   z );
      glTexCoord2f(0.0, v); glVertex3d(   x,   y, - z );

      glNormal3d( 0.0,  1.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(0.0f, v); glVertex3d(   x, - y, - z );
      glTexCoord2f(u, v); glVertex3d(   x, - y,   z );
      glTexCoord2f(u, 0.0f); glVertex3d( - x, - y,   z );

      if ( texture->u_length ) u = umag * height / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * length / texture->v_length;
      else v = vmag;

      glNormal3d( -1.0, 0.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d(   x, - y, - z );
      glTexCoord2f(u, 0.0f); glVertex3d(   x, y,   - z );
      glTexCoord2f(u, v); glVertex3d(   x,   y,   z );
      glTexCoord2f(0.0f, v); glVertex3d(   x,   - y, z );

      glNormal3d( 1.0, 0.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(0.0f, v); glVertex3d( - x,   - y,  z );
      glTexCoord2f(u, v); glVertex3d( - x,   y,   z );
      glTexCoord2f(u, 0.0f); glVertex3d( - x,  y,    -z );
      glEnd();
      glDisable( GL_TEXTURE_2D );
  }

  else {

	  glBegin(GL_QUADS);

      glNormal3d( 0.0, 0.0, - 1.0 );
      glVertex3d( - x, - y, z );
      glVertex3d( - x,   y, z );
      glVertex3d(   x,   y, z );
      glVertex3d(   x, - y, z );

      glNormal3d( 0.0, 0.0, 1.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x,   y, - z );
      glVertex3d( - x,   y, - z );

      glNormal3d( 0.0, - 1.0, 0.0 );
      glVertex3d( - x,   y, - z );
      glVertex3d( - x,   y,   z );
      glVertex3d(   x,   y,   z );
      glVertex3d(   x,   y, - z );

      glNormal3d( 0.0,  1.0, 0.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x, - y,   z );
      glVertex3d( - x, - y,   z );

      glNormal3d( - 1.0, 0.0, 0.0 );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x, - y,   z );
      glVertex3d(   x,   y,   z );
      glVertex3d(   x,   y, - z );

      glNormal3d( 1.0, 0.0, 0.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d( - x,   y, - z );
      glVertex3d( - x,   y,   z );
      glVertex3d( - x, - y,   z );

    glEnd();
  }

  FinishDraw();
}

/***************************************************************************/

Slab::Slab( double w, double l, double th ) {

  width = w;
  length = l;
  thickness = th;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void Slab::Draw( void ) {

  double x = width / 2.0;
  double y = length / 2.0;
  double z = thickness / 2.0;

  if ( ! enabled ) return;
  PrepDraw();

  if ( texture ) {
    
    GLfloat u, v;

    glEnable( GL_TEXTURE_2D );
    texture->Use();

    glBegin(GL_QUADS);

      if ( texture->u_length ) u = umag * width / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * length / texture->v_length;
      else v = vmag;

      glNormal3d( 0.0, 0.0, - 1.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, z );
      glTexCoord2f(0.0f, v); glVertex3d( - x,   y, z );
      glTexCoord2f(   u, v); glVertex3d(   x,   y, z );
      glTexCoord2f(   u, 0.0); glVertex3d(   x, - y, z );

      glNormal3d( 0.0, 0.0, 1.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(   u, 0.0f); glVertex3d(   x, - y, - z );
      glTexCoord2f(   u,    v); glVertex3d(   x,   y, - z );
      glTexCoord2f(0.0f,    v); glVertex3d( - x,   y, - z );

      if ( texture->u_length ) u = umag * length / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * thickness / texture->v_length;
      else v = vmag;

      glNormal3d( 0.0, - 1.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x,   y, - z );
      glTexCoord2f(0.0f,    v); glVertex3d( - x,   y,   z );
      glTexCoord2f(   u,    v); glVertex3d(   x,   y,   z );
      glTexCoord2f(   u, 0.0f); glVertex3d(   x,   y, - z );

      glNormal3d( 0.0,  1.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(   u, 0.0f); glVertex3d(   x, - y, - z );
      glTexCoord2f(   u,    v); glVertex3d(   x, - y,   z );
      glTexCoord2f(0.0f,    v); glVertex3d( - x, - y,   z );

      if ( texture->u_length ) u = umag * width / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * thickness / texture->v_length;
      else v = vmag;

      glNormal3d( - 1.0, 0.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d(   x, - y, - z );
      glTexCoord2f(0.0f, v); glVertex3d(   x, - y,   z );
      glTexCoord2f(u, v); glVertex3d(   x,   y,   z );
      glTexCoord2f(u, 0.0f); glVertex3d(   x,   y, - z );

      glNormal3d( 1.0, 0.0, 0.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, - z );
      glTexCoord2f(0.0f, v); glVertex3d( - x,   y, - z );
      glTexCoord2f(u, v); glVertex3d( - x,   y,   z );
      glTexCoord2f(u, 0.0f); glVertex3d( - x, - y,   z );

      glEnd();
      glDisable( GL_TEXTURE_2D );

  }

  else {

	  glBegin(GL_QUADS);

      glNormal3d( 0.0, 0.0, - 1.0 );
      glVertex3d( - x, - y, z );
      glVertex3d( - x,   y, z );
      glVertex3d(   x,   y, z );
      glVertex3d(   x, - y, z );

      glNormal3d( 0.0, 0.0, 1.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x,   y, - z );
      glVertex3d( - x,   y, - z );

      glNormal3d( 0.0, - 1.0, 0.0 );
      glVertex3d( - x,   y, - z );
      glVertex3d( - x,   y,   z );
      glVertex3d(   x,   y,   z );
      glVertex3d(   x,   y, - z );

      glNormal3d( 0.0,  1.0, 0.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x, - y,   z );
      glVertex3d( - x, - y,   z );

      glNormal3d( - 1.0, 0.0, 0.0 );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x, - y,   z );
      glVertex3d(   x,   y,   z );
      glVertex3d(   x,   y, - z );

      glNormal3d( 1.0, 0.0, 0.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d( - x,   y, - z );
      glVertex3d( - x,   y,   z );
      glVertex3d( - x, - y,   z );

    glEnd();
  }

  FinishDraw();
}

/***************************************************************************/

Patch::Patch( double w, double l ) {

  width = w;
  length = l;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Patch::Draw( void ) {

  double x = width / 2.0;
  double y = length / 2.0;
  double z = 0.0;

  if ( ! enabled ) return;
  PrepDraw();

  if ( texture ) {
    
    GLfloat u, v;

    glEnable( GL_TEXTURE_2D );
    texture->Use();

    glBegin(GL_QUADS);

      if ( texture->u_length ) u = umag * width / texture->u_length;
      else u = umag;
      if ( texture->v_length ) v = vmag * length / texture->v_length;
      else v = vmag;

      glNormal3d( 0.0, 0.0, - 1.0 );
      glTexCoord2f(0.0f, 0.0f); glVertex3d( - x, - y, z );
      glTexCoord2f(0.0f, v); glVertex3d( - x,   y, z );
      glTexCoord2f(   u, v); glVertex3d(   x,   y, z );
      glTexCoord2f(   u, 0.0); glVertex3d(   x, - y, z );

    glEnd();
    glDisable( GL_TEXTURE_2D );
  }

  else {

	  glBegin(GL_QUADS);

      glNormal3d( 0.0, 0.0, - 1.0 );
      glVertex3d( - x, - y, z );
      glVertex3d( - x,   y, z );
      glVertex3d(   x,   y, z );
      glVertex3d(   x, - y, z );

    glEnd();
  }

  FinishDraw();
}
/***************************************************************************/


Bar::Bar( double l, double w, double th, bool cap ) {

  width = w;
  thickness = th;
  length = l;
  capped = cap;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void Bar::Draw( void ) {

  double x = width / 2.0;
  double y = thickness / 2.0;
  double z = length / 2.0;

  if ( ! enabled ) return;
  PrepDraw();

  // Consider the position to be the end of the bar, not the middle.
  glTranslated( 0.0, 0.0, z );

  glBegin( GL_POLYGON );

    glNormal3d( 0.0, 0.0, - 1.0 );
    glVertex3d( - x, y, - z );
    glVertex3d( - x, y,   z );
    glVertex3d(   x, y,   z );
    glVertex3d(   x, y, - z );

  glEnd();

  glBegin( GL_POLYGON );

    glNormal3d( 0.0, 0.0, 1.0 );
    glVertex3d( - x, - y, - z );
    glVertex3d(   x, - y, - z );
    glVertex3d(   x, - y,   z );
    glVertex3d( - x, - y,   z );

  glEnd();

  if ( capped ) {

    glBegin( GL_POLYGON );

      glNormal3d( 0.0, - 1.0, 0.0 );
      glVertex3d( - x, - y, z );
      glVertex3d( - x,   y, z );
      glVertex3d(   x,   y, z );
      glVertex3d(   x, - y, z );

    glEnd();

    glBegin( GL_POLYGON );

      glNormal3d( 0.0,  1.0, 0.0 );
      glVertex3d( - x, - y, - z );
      glVertex3d(   x, - y, - z );
      glVertex3d(   x,   y, - z );
      glVertex3d( - x,   y, - z );

    glEnd();

  }


  glBegin( GL_POLYGON );

    glNormal3d( - 1.0, 0.0, 0.0 );
    glVertex3d(   x, - y, - z );
    glVertex3d(   x,   y, - z );
    glVertex3d(   x,   y,   z );
    glVertex3d(   x, - y,   z );

  glEnd();

  glBegin( GL_POLYGON );

    glNormal3d( 1.0, 0.0, 0.0 );
    glVertex3d( - x, - y, - z );
    glVertex3d( - x, - y,   z );
    glVertex3d( - x,   y,   z );
    glVertex3d( - x,   y, - z );

  glEnd();

  FinishDraw();
}

/********************************************************************************/

Extrusion::Extrusion( double l, double vert[][2], int n ) {

	vertices = n;
	length = l;
	for ( int i = 0; i < n; i++ ){
		vertex[i][0] = vert[i][0];
		vertex[i][1] = vert[i][1];
	} 

	OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void Extrusion::Draw( void ) {

	double l2 = length / 2.0;
	int i, j;
	Vector3 delta, normal;

	if ( ! enabled ) return;
	PrepDraw();

	/* Draw the ends first. */
	glBegin( GL_POLYGON );
	glNormal3d( 0.0, 0.0, - 1.0 );
	for ( i = 0; i < vertices; i++ ) glVertex3d( vertex[i][X], vertex[i][Y], l2 );
	glEnd();
	glBegin( GL_POLYGON );
	glNormal3d( 0.0, 0.0,   1.0 );
	for ( i = vertices - 1; i >= 0; i-- ) glVertex3d( vertex[i][X], vertex[i][Y], - l2 );
	glEnd();

	/* Now draw the facets. */
	for ( i = 0; i < vertices; i++ ) {
		j = ( i + 1 ) % vertices;
		// subtract_vectors( vertex[i], vertex[j], delta );
		delta[0] = vertex[i][0] - vertex[j][0];
		delta[1] = vertex[i][1] - vertex[j][1];
		delta[2] = 0.0;
		ComputeCrossProduct( normal, delta, kVector );
		NormalizeVector( normal );
		glBegin( GL_POLYGON );
		glNormal3dv( normal );
		glVertex3d( vertex[i][X], vertex[i][Y], - l2 );
		glVertex3d( vertex[i][X], vertex[i][Y], l2 );
		glVertex3d( vertex[j][X], vertex[j][Y], l2 );
		glVertex3d( vertex[j][X], vertex[j][Y], - l2 );
		glEnd();
	}
	FinishDraw();
}


/********************************************************************************/

Sphere::Sphere( double r, int sl, int st ) {

  quad = gluNewQuadric();

  radius = r;
  slices = sl;
  stacks = st;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Sphere::SetRadius( double value ) {
	radius = value;
}

void Sphere::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();
  gluSphere( quad, radius, slices, stacks );
  FinishDraw();

}

/********************************************************************************/

Ellipsoid::Ellipsoid( double rx, double ry, double rz, int sl, int st ) {

  quad = gluNewQuadric();

  x_radius = rx;
  y_radius = ry;
  z_radius = rz;

  max_radius = x_radius > y_radius ? x_radius : y_radius;
  max_radius = max_radius > z_radius ? max_radius : z_radius;

  x_distortion = x_radius / max_radius;
  y_distortion = y_radius / max_radius;
  z_distortion = z_radius / max_radius;

  slices = sl;
  stacks = st;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

Ellipsoid::Ellipsoid( double rxy, double rz, int sl, int st ) {

 Ellipsoid( rxy, rxy, rz, sl, st );

}

void Ellipsoid::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();
  glScaled( x_distortion, y_distortion, z_distortion );
  gluSphere( quad, max_radius, slices, stacks );
  FinishDraw();

}
/********************************************************************************/

Disk::Disk( double outer, double inner, int sl, int lps ) {

  quad = gluNewQuadric();

  outer_radius = outer;
  inner_radius = inner;
  slices = sl;
  loops = lps;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Disk::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();
  gluDisk( quad, inner_radius, outer_radius, slices, loops );
  FinishDraw();

}

void Disk::SetRadius( double outer, double inner ) {

  inner_radius = inner;
  outer_radius = outer;

}

/********************************************************************************/

Cylinder::Cylinder( double top_r, double bottom_r, double hght, int slcs, int stks ) {

  quad = gluNewQuadric();

  top_radius = top_r;
  bottom_radius = bottom_r;
  slices = slcs;
  stacks = stks;
  height = hght;


  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Cylinder::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();
  gluCylinder( quad, top_radius, bottom_radius, height, slices, stacks );
  FinishDraw();

}

void Cylinder::SetRadius( double top_radius, double bottom_radius ) {

  if ( !_finite( bottom_radius ) ) bottom_radius = top_radius;
  this->top_radius = top_radius;
  this->bottom_radius = bottom_radius;

}

void Cylinder::SetHeight( double height ) {
	this->height = height;
}

/***************************************************************************/

Frustrum::Frustrum( double near_v[3], double far_v[3] ) {


  CopyVector( near_vertex, near_v  );
  CopyVector( far_vertex, far_v );

  multi_color = true;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Frustrum::draw_polygons( void ) {

  if ( multi_color ) glColor4fv( OpenGLColors[ Translucid( GRAY ) ] );

  glBegin( GL_POLYGON );

    glVertex3d(   near_vertex[X],   near_vertex[Y], near_vertex[Z] );
    glVertex3d(   near_vertex[X], - near_vertex[Y], near_vertex[Z] );
    glVertex3d( - near_vertex[X], - near_vertex[Y], near_vertex[Z] );
    glVertex3d( - near_vertex[X],   near_vertex[Y], near_vertex[Z] );

  glEnd();

  if ( multi_color ) glColor4fv( OpenGLColors[ Translucid( MAGENTA ) ] );

  glBegin( GL_POLYGON );

    glVertex3d(   - near_vertex[X],   near_vertex[Y], near_vertex[Z] );
    glVertex3d(   - far_vertex[X],    far_vertex[Y],  far_vertex[Z] );
    glVertex3d(   - far_vertex[X],  - far_vertex[Y],  far_vertex[Z] );
    glVertex3d(   - near_vertex[X], - near_vertex[Y], near_vertex[Z] );

  glEnd();

  if ( multi_color ) glColor4fv( OpenGLColors[ Translucid( CYAN ) ] );

  glBegin( GL_POLYGON );

    glVertex3d(   near_vertex[X],   near_vertex[Y], near_vertex[Z] );
    glVertex3d(   far_vertex[X],    far_vertex[Y],  far_vertex[Z] );
    glVertex3d( - far_vertex[X],    far_vertex[Y],  far_vertex[Z] );
    glVertex3d( - near_vertex[X],   near_vertex[Y], near_vertex[Z] );

  glEnd();

  if ( multi_color ) glColor4fv( OpenGLColors[ Translucid( BLUE ) ] );

  glBegin( GL_POLYGON );

    glVertex3d(   near_vertex[X], - near_vertex[Y], near_vertex[Z] );
    glVertex3d( - near_vertex[X], - near_vertex[Y], near_vertex[Z] );
    glVertex3d( - far_vertex[X],  - far_vertex[Y],  far_vertex[Z] );
    glVertex3d(   far_vertex[X],  - far_vertex[Y],  far_vertex[Z] );

  glEnd();

  if ( multi_color ) glColor4fv( OpenGLColors[ Translucid( ORANGE ) ] );

  glBegin( GL_POLYGON );

    glVertex3d(   near_vertex[X],   near_vertex[Y], near_vertex[Z] );
    glVertex3d(   far_vertex[X],    far_vertex[Y],  far_vertex[Z] );
    glVertex3d(   far_vertex[X],  - far_vertex[Y],  far_vertex[Z] );
    glVertex3d(   near_vertex[X], - near_vertex[Y], near_vertex[Z] );

  glEnd();

}

void Frustrum::draw_end( void ) {

  glBegin( GL_POLYGON );

    glVertex3d(   far_vertex[X],   far_vertex[Y], far_vertex[Z] );
    glVertex3d( - far_vertex[X],   far_vertex[Y], far_vertex[Z] );
    glVertex3d( - far_vertex[X], - far_vertex[Y], far_vertex[Z] );
    glVertex3d(   far_vertex[X], - far_vertex[Y], far_vertex[Z] );

  glEnd();

}

void Frustrum::Draw( void ) {

  if ( ! enabled ) return;
  PrepDraw();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable( GL_LIGHTING );
  draw_polygons();

  glDisable( GL_LIGHTING );

  glColor4fv( OpenGLColors[ RED ] );
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
  draw_end();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable( GL_LIGHTING );

  FinishDraw();
}

/********************************************************************************/

Circle::Circle( double r, int sl, int lps ) {

  radius = r;
  slices = sl;
  loops = lps;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Circle::Draw( void ) {

  double     rx, ry, angle, angle_step = Pi / 10.0;
  if ( ! enabled ) return;
  PrepDraw();
  
  glBegin( GL_POLYGON );
  for ( angle = 0.0; angle < 2.0 * Pi; angle += angle_step ) {
    rx = radius * cos( angle );
    ry = radius * sin( angle );
    glVertex2f( rx, ry );
  }  
  glEnd();
  FinishDraw();

}

/********************************************************************************/

Hole::Hole( double radius, double width, double height ) {

  this->radius = radius;
  this->width = width;
  this->height = height;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

void Hole::SetRadius( double value ) {
  radius = value;
}

void Hole::Draw( void ) {

  double     rx, ry, angle, angle_step = Pi / 128.0;
  if ( ! enabled ) return;
  PrepDraw();
  
  glBegin( GL_POLYGON );
  glVertex2f( width, height );
  glVertex2f( width, 0.0 );
  for ( angle = - angle_step; angle <= 0.5 * Pi + angle_step; angle += angle_step ) {
    rx = radius * cos( angle );
    ry = radius * sin( angle );
    glVertex2f( rx, ry );
  }  
  glVertex2f( 0.0, height );
  glVertex2f( width, height );
  glEnd();

  glBegin( GL_POLYGON );
  glVertex2f( - width, height );
  glVertex2f( 0.0, height );
  for ( angle = 0.5 * Pi - angle_step; angle <= Pi + angle_step; angle += angle_step ) {
    rx = radius * cos( angle );
    ry = radius * sin( angle );
    glVertex2f( rx, ry );
  }  
  glVertex2f( - width, 0.0 );
  glVertex2f( - width, height );
  glEnd();

  glBegin( GL_POLYGON );
  glVertex2f( - width, - height );
  glVertex2f( - width, 0.0 );
  for ( angle = Pi - angle_step; angle <= 1.5 * Pi + angle_step; angle += angle_step ) {
    rx = radius * cos( angle );
    ry = radius * sin( angle );
    glVertex2f( rx, ry );
  }  
  glVertex2f( 0.0, - height );
  glVertex2f( - width, - height );
  glEnd();

  glBegin( GL_POLYGON );
  glVertex2f( width, - height );
  glVertex2f( 0.0, - height );
  for ( angle = 1.5 * Pi - angle_step; angle <= 2.0 * Pi + angle_step; angle += angle_step ) {
    rx = radius * cos( angle );
    ry = radius * sin( angle );
    glVertex2f( rx, ry );
  }  
  glVertex2f( width, 0.0 );
  glVertex2f( width, - height );
  glEnd();
  FinishDraw();

}

/***************************************************************************/

/* A rectangle with a hole in it. */
WindowFrame::WindowFrame( double width, double height, double thickness ) {

  this->width = width;
  this->height = height;
  this->thickness = thickness;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void WindowFrame::Draw( void ) {

  double x = width / 2.0;
  double y = height / 2.0;
  double z = 0.0;

  if ( ! enabled ) return;
  PrepDraw();

	glBegin(GL_QUADS);

    glNormal3d( 0.0, 0.0, - 1.0 );
    glVertex3d( - x - thickness, - y - thickness, z );
    glVertex3d( - x - thickness,   y + thickness, z );
    glVertex3d( - x,   y, z );
    glVertex3d( - x, - y, z );

    glVertex3d(   x, - y, z );
    glVertex3d(   x,   y, z );
    glVertex3d(   x + thickness,   y + thickness, z );
    glVertex3d(   x + thickness, - y - thickness, z );

    glVertex3d( - x - thickness, - y - thickness, z );
    glVertex3d( - x, - y, z );
    glVertex3d(   x, - y, z );
    glVertex3d(   x + thickness, - y - thickness, z );

    glVertex3d( - x, y, z );
    glVertex3d( - x - thickness, y + thickness, z );
    glVertex3d(   x + thickness, y + thickness, z );
    glVertex3d(   x, y, z );


  glEnd();

  FinishDraw();
}


