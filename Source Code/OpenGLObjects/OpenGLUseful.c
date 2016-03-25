/*********************************************************************************/
/*                                                                               */
/*                                OpenGLUseful.c                                 */
/*                                                                               */
/*********************************************************************************/

/* Some useful OpenGL routines. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>

/*********************************************************************************/

void glDefaultLighting( void ) {

  // Light definition
  GLfloat LightPosition[] = { 5000.0, 5000.0, 5000.0, 0.0 };
  GLfloat LightAmbient[]  = { 0.25, 0.25, 0.25, 1.0};
  GLfloat LightDiffuse[]  = { 0.50, 0.50, 0.50, 1.0};
  GLfloat LightSpecular[] = { 0.50, 0.50, 0.50, 1.0};

  glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
  glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

  glEnable(GL_LIGHT0);

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
  glEnable( GL_BLEND );

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glClearColor( 1.0F, 1.0F, 1.0F, 0.0F );



}

void glCodaDefaultLighting( void ) {

  // Light definition
  GLfloat LightPosition[] = { - 5000.0, - 5000.0, 5000.0, 0.0 };
  GLfloat LightAmbient[]  = { 0.25, 0.25, 0.25, 1.0};
  GLfloat LightDiffuse[]  = { 0.50, 0.50, 0.50, 1.0};
  GLfloat LightSpecular[] = { 0.50, 0.50, 0.50, 1.0};

  glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
  glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

  glEnable(GL_LIGHT0);

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
  glEnable( GL_BLEND );

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glClearColor( 1.0F, 1.0F, 1.0F, 0.0F );



}

void glAutoLighting( float intensity ) {

  // Light definition
  GLfloat LightAmbient[]  = { intensity, intensity, intensity, 1.0};
  GLfloat LightDiffuse[]  = { intensity, intensity, intensity, 1.0};
  GLfloat LightSpecular[] = { 0.00, 0.00, 0.00, 1.0};

  glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

  glEnable(GL_LIGHT0);

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
  glEnable( GL_BLEND );

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glClearColor( 1.0F, 1.0F, 1.0F, 0.0F );



}

/*********************************************************************************/

void glDefaultMaterial( void ) {

  // Material definition
  GLfloat MaterialAmbient[]   = { 0.25, 0.25, 0.25, 1.0};
  GLfloat MaterialDiffuse[]   = { 0.50, 0.50, 0.50, 1.0};
  GLfloat MaterialSpecular[]  = { 0.50, 0.50, 0.50, 1.0};
  GLfloat MaterialShininess[] = { 50.0 };

  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient );
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, MaterialSpecular );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, MaterialShininess );

  glEnable( GL_COLOR_MATERIAL );
  glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

}

