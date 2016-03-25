/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>


#include <gl/gl.h>
#include <gl/glu.h>

#include <useful.h>
#include <screen.h>
#include <3dMatrix.h>
#include <Timers.h>

#include <Trackers.h>
#include <TrackerUDPServices.h>

extern "C" {
  int _kbhit( void );
}

#include "OpenGLUseful.h"
#include "OpenGLColors.h"
#include "OpenGLWindows.h"
#include "OpenGLObjects.h"
#include "OpenGLViewpoints.h"
#include "OpenGLTextures.h"
#include "VoilaObjects.h"

double frequency = 100.0;

int width = 320;
int height = 240;
int left_x = 0, left_y = 0;
int right_x = 320, right_y = 0;
bool border = true;

double room_width = 2000.0;
double room_height = 2000.0;
double room_length = 8000.0;
double wall_thickness = 10.0;

OpenGLWindow  *left_window;
//OpenGLWindow *right_window;
Assembly      *object;
Assembly      *room;
Wand          *wand;

Texture       *wall_texture, *ceiling_texture;

Sphere *sphere;
Slab *slab;
Cylinder *cylinder;
Disk *disk;

Viewpoint *viewpoint;
int viewpoint_sensor = 0;
int wand_sensor = 1;

int nominal_head_height = 800.0;
int nominal_head_tilt = 20.0;

Timer ball_timer;

UDPTracker trkr;
IsenseDataPacket iSenseData;


/*********************************************************************************/

void render( void ) {
  
  
  // Shade model
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

	glLoadIdentity();									// Reset The View
  
  /* Draw the complex objects at its current position. */
  object->Draw();
  wand->Draw();
  room->Draw();
  
}


/*********************************************************************************/

int main( int argc, char *argv[] ) {
  
  int arg;
  
  int use_tracker = FALSE;
  int use_isense = FALSE;
  
  for ( arg = 1; arg < argc; arg++ ) {
    
    if ( !strcmp( argv[arg], "-hmd" ) ) {
      width = 640;
      height = 480;
      left_x = 0;
      left_y = 0;
      right_x = 640;
      right_y = 0;
      border = false;
      HMDScreen( HMD_STEREO );
    }
    else if ( !strcmp( argv[arg], "-full" ) ) {
      width = 1024;
      height = 768;
      left_x = 0;
      left_y = 0;
      right_x = 640;
      right_y = 0;
      border = false;
//      HMDScreen( HMD_MONO );
    }
    else if ( !strcmp( argv[arg], "-double" ) ) {
      width = 640;
      height = 480;
      left_x = 0;
      left_y = 0;
      right_x = 640;
      right_y = 0;
    }
    else if ( !strcmp( argv[arg], "-triple" ) ) {
      width = 640;
      height = 480;
      left_x = 1024;
      left_y = 0;
      right_x = 1664;
      right_y = 0;
    }
    else if ( !strcmp( argv[arg], "-nVis" ) ) {
      width = 1280;
      height = 1024;
      left_x = 0;
      left_y = 0;
      right_x = 1280;
      right_y = 0;
      border = false;
    }
 
    else if ( !strcmp( argv[arg], "-tracker" ) ) use_tracker = TRUE;
    else if ( !strcmp( argv[arg], "-isense" )) use_isense = TRUE;
    else if ( !strcmp( argv[arg], "-sensor" ) ) {
      arg++;
      if ( arg < argc ) sscanf( argv[arg], "%d", &viewpoint_sensor );
      fprintf( stderr, "Using sensor %d.\n", viewpoint_sensor );
    }
  }
  
  
  /*
  * Place the viewer at the origin, looking in the zero orientation.
  */
  double viewpoint_position[3] = { 0.0, 1000.0, 0.0 };
  double viewpoint_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  
  /*
  * In the OpenGL coordinate system, zero orientation means that one is looking
  *  down the negative Z axis with positive Y up and positive X to the right.
  * Here we place the object where we can see it.
  */
  double object_position[3] = { 0.0, 1000.0, - 2500.0 };
  double object_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    
  double wand_position[3] = { 0.0, 0.0, - 2500.0 };
  double wand_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

  
  /* Start up optical tracker. */
  if ( use_tracker ) {
    SetupTracker();
    SensorSetHysteresis( 1, 2.0, 0.5 ); // mm and degrees
                                        /*
                                        * The simulated tracker goes through 7 phases.
                                        * The first is stationary.
                                        * The next three are translation movements along X,Y and Z, respectively.
                                        * The last three are rotations around Z, Y and X, respectively.
                                        * The following sets the amplitude of each phase.
                                        * This call will have no effect on real tracker movements.
    */
    SimulateSetMovement( viewpoint_sensor, 800.0, 5.0 ); // 800 mm translation and 5° rotation.

#if 0
    /* Shift the nominal viewpoint up, then tilt the view back down to look at the target. */
    viewpoint_position[Y] = nominal_head_height;
    SimulateSetLocation( viewpoint_sensor, viewpoint_position );
    viewpoint_orientation[Y][Y] = viewpoint_orientation[Z][Z] = cos( radians( nominal_head_tilt ) );
    viewpoint_orientation[Y][Z] = sin( radians( nominal_head_tilt ) ) ;
    viewpoint_orientation[Z][Y] = - viewpoint_orientation[Y][Z];
    SimulateSetOrientation( viewpoint_sensor, viewpoint_orientation );
#endif

  }
  if ( use_isense ) {
    if ( UDPTrackerInitClient( &trkr, NULL, SIMULATOR_PORT ) ) {
      MessageBox( NULL, "Error opening socket.", "Isense UDP", MB_OK );
      use_isense = NO;
    }
  }
  
  
  left_window = new OpenGLWindow();
//  right_window = new OpenGLWindow();
  
  /* 
  * Define a viewing projection with:
  *  45° vertical field-of-view - horizontal fov will be determined by window aspect ratio.
  *  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
  *      that I give for the model room here are in mm.
  *  100.0 to 1000.0  depth clipping planes - making this smaller would improve the depth resolution.
  */
  viewpoint = new Viewpoint(45.0, 60.0, 100.0, 10000.0);

  wand = new Wand();

  /*
  * Create a simple object to look at.
  */
  object = new Assembly();
  
  slab = new Slab( 750.0, 500.0, 10.0 );
  slab->SetPosition( 0.0, 0.0, 100.0 );
  slab->SetColor( GREEN );
//  object->AddComponent( slab );
  
  slab = new Slab( 750.0, 500.0, 10.0 );
  slab->SetPosition( 0.0, 0.0, - 100.0 );
  slab->SetColor( BLUE );
//  object->AddComponent( slab );
  
  sphere = new Sphere( 250.0 );
  sphere->SetColor( RED );
  object->AddComponent( sphere );
  
  cylinder = new Cylinder( 10.0, 300.0, 500.0 );
  cylinder->SetPosition( 0.0, 0.0, 0.0 );
  cylinder->SetColor( YELLOW );
//  object->AddComponent( cylinder );
  
  /*
   * Create a room to put the object in.
   */
  wall_texture = new Texture( "Lime.bmp" );
  ceiling_texture = new Texture( "light.bmp" );
  room = new Assembly();

  slab = new Slab( room_width, room_height, wall_thickness );
  slab->SetPosition( 0.0, 0.0, room_length / 2.0 );
  slab->SetColor( GRAY );
//  slab->texture = ceiling_texture;
  room->AddComponent( slab );
  
  disk = new Disk( 300.0 );
  disk->SetPosition( 0.0, 0.0, - ( room_length / 2.0 - 200.0 ) );
  disk->SetColor( BLACK );
  room->AddComponent( disk );
  cylinder = new Cylinder( 300.0, 300.0, 500.0 );
  cylinder->SetPosition( 0.0, 0.0, - ( room_length / 2.0 ) );
  cylinder->SetColor( YELLOW );
  room->AddComponent( cylinder );
  
  
  slab = new Slab( room_width, room_height, wall_thickness );
  slab->SetPosition( 0.0, 0.0, - room_length / 2.0 );
  slab->SetColor( GRAY );
//  slab->texture = ceiling_texture;
  room->AddComponent( slab );
  
  slab = new Slab( room_width, room_length, wall_thickness );
  slab->SetPosition( 0.0, room_width / 2.0, 0.0 );
  slab->SetOrientation( -90.0, i_vector );
  slab->SetColor( GREEN );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  
  slab = new Slab( room_width, room_length, wall_thickness );
  slab->SetPosition( 0.0, - room_width / 2.0, 0.0 );
  slab->SetOrientation( 90.0, i_vector );
  slab->SetColor( GREEN );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  
  slab = new Slab( room_length, room_height, wall_thickness );
  slab->SetPosition( room_height / 2.0, 0.0, 0.0 );
  slab->SetOrientation( -90.0, j_vector );
  slab->SetColor( GREEN );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  
  slab = new Slab( room_length, room_height, wall_thickness );
  slab->SetPosition( - room_height / 2.0, 0.0, 0.0 );
  slab->SetOrientation( 90.0, j_vector );
  slab->SetColor( GREEN );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  room->SetPosition( 0.0, 1000.0, 0.0 );
  
  int x = 100, y = 100;
  
  /*
  * Create two windows for a stereo display. 
  */
  left_window->Border = border; // Remove borders for an HMD display.
  if ( left_window->Create( NULL, "Left", left_x, left_y, width, height ) ) {
		/*
    * Create sets the new window to be the active window.
    * Setup the lights and materials for that window.
    */
    glDefaultLighting();
    glDefaultMaterial();
//    wall_texture->Define();
//    ceiling_texture->Define();
  }
#if 0
  /*
   * Do the same for the other eye.
   */
  right_window->Border = border;
  if ( right_window->Create( NULL, "Right", right_x, right_y, width, height ) ) {
    glDefaultLighting();
    glDefaultMaterial();
    wall_texture->Define();
  }
#endif
  
  TimerStart( &ball_timer );

  for ( double angle = 0.0; 1; angle += 5.0 ) {

    double time, distance;
    
    if ( _kbhit() ) TimerStart( &ball_timer );
    if ( distance < - 10000000.0 ) TimerStart( &ball_timer );


    if ( use_tracker ) {
      GetSensorPositionOrientation( viewpoint_sensor, YES, viewpoint_position, viewpoint_orientation );
      viewpoint->SetPosition( viewpoint_position );
      viewpoint->SetOrientation( viewpoint_orientation );
      GetSensorPositionOrientation( wand_sensor, YES, wand_position, wand_orientation );
      wand->SetPosition( wand_position );
      wand->SetOrientation( wand_orientation );
    }
    else if ( use_isense ) {
      UDPTrackerGetIsenseData( &trkr, &iSenseData );
      isense_to_matrix( iSenseData.Station[0].orientation, viewpoint_orientation );
      isense_to_vector( iSenseData.Station[0].position, viewpoint_position );
      scale_vector( 10.0, viewpoint_position, viewpoint_position );
      viewpoint_position[Y] += nominal_head_height;

      viewpoint->SetPosition( viewpoint_position );
      viewpoint->SetOrientation( viewpoint_orientation );
    }
    else {
      viewpoint->SetPosition( viewpoint_position );
      viewpoint->SetOrientation( viewpoint_orientation );
    }

    
    time = TimerElapsedTime( &ball_timer );
    distance = room_length / 2.0 - 450 * time * time;
    object_position[Z] = - distance;

    object->SetPosition( object_position );
    object->SetOrientation( angle, j_vector );
    
    left_window->Activate();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    viewpoint->Apply( left_window, CYCLOPS );
    render();
//    viewpoint->Apply( left_window, RIGHT_EYE );
//    render();
    left_window->Swap();
    
    if ( ! left_window->RunOnce() ) break;
#if 0   
    right_window->Activate();
    viewpoint->Apply( right_window, RIGHT_EYE );
    render();
    right_window->Swap();
    
    if ( ! right_window->RunOnce() ) break;
#endif

    
  }
  
//  right_window->Destroy();
  left_window->Destroy();
  
  if ( use_tracker ) KillTracker();
  
  RevertScreen();
  
  return( 0 );
  
}

