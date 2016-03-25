/*********************************************************************************/

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

#include <useful.h>
#include <screen.h>
#include <3dMatrix.h>
#include <Timers.h>

#include <Trackers.h>
#include <TrackerUDPServices.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "OpenGLUseful.h"
#include "OpenGLColors.h"
#include "OpenGLWindows.h"
#include "OpenGLObjects.h"
#include "OpenGLViewpoints.h"
#include "OpenGLTextures.h"

double frequency = 100.0;

int width = 640;
int height = 480;
bool border = true;
bool stereo = false;
bool fullscreen = false;

double room_width = 1800.0;
double room_height = 2000.0;
double room_length = 8000.0;
double wall_thickness = 10.0;

OpenGLWindow  *window;
Assembly      *object;
Assembly      *room;

Texture       *wall_texture;
Texture       *ceiling_texture;
Texture       *floor_texture;

Sphere *sphere;
Slab *slab;
Box *box;
Cylinder *cylinder;
Disk *disk;

Viewpoint *viewpoint;
int viewpoint_sensor = 0;
int object_sensor = 1;

/*
 * Place the viewer at the origin, looking in the zero orientation.
 */
double viewpoint_position[3] = { 0.0, 0.0, 0.0 };
double viewpoint_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

/*
 * In the OpenGL coordinate system, zero orientation means that one is looking
 *  down the negative Z axis with positive Y up and positive X to the right.
 * Here we place the object where we can see it.
 */
double object_position[3] = { 0.0, 0.0, - 1500.0 };
double object_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };


// These parameters are for setting up the simulated tracker.
// When using a real tracker, they should have no effect.
int nominal_head_height = 300.0;
int nominal_head_tilt = -10.0;
// 800 mm translation and 5° rotation.
double sim_translation = 800.0;
double sim_rotation = 30.0;

Timer frame_timer;
int   frame_counter;

char *closure_record = 0;

UDPTracker trkr;

/*********************************************************************************/

void render( void ) {
  
    
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* Draw the complex objects at its current position. */
  object->Draw();
  room->Draw();
  
}

/*********************************************************************************/

void keyboard_callback ( int key ) {

  FILE *fp;
  if ( key == '\r' ) {
    fp = fopen( closure_record, "a+" );
    fprintf( fp, "\ndone\n" );
    fclose( fp );
  }

}

/*********************************************************************************/

void create_objects( void ) {

 

 /*
  * Create a room to put the object in.
  */
  /*
   * The wall texture is 256 pixels wide by 512 high.
   * We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
   */
  wall_texture = new Texture( "lime.bmp", 2000, 4000 );
  ceiling_texture = new Texture( "girder.bmp", 500, 500 );
  floor_texture = new Texture( "lwood3.bmp", 1000, 1000 );
  room = new Assembly();

  room->SetColor( WHITE );
  

#if 0

  // Back wall - Nominal zero is along negative Z, so Z position is positive to be behind.
  slab = new Slab( room_width, room_height, wall_thickness );
  slab->SetPosition( 0.0, 0.0, room_length / 2.0 );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  
  
  // Front wall - Z is negative for reasons above.
  slab = new Slab( room_width, room_height, wall_thickness );
  slab->SetPosition( 0.0, 0.0, - room_length / 2.0 );
  slab->texture = wall_texture;
  room->AddComponent( slab );
  

  // Ceiling
  slab = new Slab( room_width, room_length, wall_thickness );
  slab->SetPosition( 0.0, room_height / 2.0, 0.0 );
  slab->SetOrientation( -90.0, i_vector );
  slab->texture = ceiling_texture;
  room->AddComponent( slab );
  
  // Floor
  slab = new Slab( room_width, room_length, wall_thickness );
  slab->SetPosition( 0.0, - room_height / 2.0, 0.0 );
  slab->SetOrientation( -90.0, i_vector );
  slab->texture = floor_texture;
  room->AddComponent( slab );

  slab = new Slab( room_length, room_height, wall_thickness );
  slab->SetPosition( room_width / 2.0, 0.0, 0.0 );
  slab->SetOrientation( -90.0, j_vector );
  slab->texture = wall_texture;
  room->AddComponent( slab );

  slab = new Slab( room_length, room_height, wall_thickness );
  slab->SetPosition( - room_width / 2.0, 0.0, 0.0 );
  slab->SetOrientation( 90.0, j_vector );
  slab->texture = wall_texture;
  room->AddComponent( slab );

#else
  box = new Box( room_height, room_width, room_length );
  box->SetColor( GRAY );
  box->SetTexture( wall_texture );
  room->AddComponent( box );
#endif
  // A disk on the front wall, something to look at.
  disk = new Disk( 750.0 );
  disk->SetPosition( 0.0, 0.0, - room_length / 2.0 + wall_thickness );
  disk->SetColor( ORANGE );
  room->AddComponent( disk );

  room->SetPosition( 0.0, 0.0, 0.0 );
/*
  * Create a simple object to look at.
  */
  object = new Assembly();
  
  slab = new Slab( 150.0, 100.0, 10.0 );
  slab->SetPosition( 0.0, 0.0, 10.0 );
  slab->SetColor( BLUE );
  object->AddComponent( slab );
  
  slab = new Slab( 150.0, 100.0, 10.0 );
  slab->SetPosition( 0.0, 0.0, - 10.0 );
  slab->SetColor( GREEN );
  object->AddComponent( slab );
  
  sphere = new Sphere( 40.0 );
  sphere->SetColor( RED );
  object->AddComponent( sphere );
  
  cylinder = new Cylinder( 10.0, 30.0, 60.0 );
  cylinder->SetPosition( 0.0, 0.0, 0.0 );
  cylinder->SetColor( YELLOW );
  object->AddComponent( cylinder );
  
  cylinder = new Cylinder( 10.0, 10.0, 80.0 );
  cylinder->SetPosition( 0.0, 0.0, 0.0 );
  cylinder->SetOrientation( 0.0, - 90.0, 0.0 );
  cylinder->SetColor( GREEN );
  object->AddComponent( cylinder );

  object->SetPosition( object_position);
  object->SetOrientation( object_orientation );

}

/*********************************************************************************/

int main( int argc, char *argv[] ) {
  
  int arg;
  
  int use_tracker = FALSE;
  int use_isense = FALSE;
  int use_udp = FALSE;
  int orientation_only = FALSE;

  IsenseDataPacket iSenseData;
  UDPDataPacket udpData;
  int data_available = NO;
  
  for ( arg = 1; arg < argc; arg++ ) {
    
    if ( !strcmp( argv[arg], "-full" ) ) {
      width = 640;
      height = 480;
      border = false;
      fullscreen = true;
      stereo = false;
    }
    else if ( !strcmp( argv[arg], "-hmd" ) ) {
      width = 1280;
      height = 480;
      border = false;
      fullscreen = true;
      stereo = true;
//       HMDScreen( HMD_STEREO );
    }
    else if ( !strcmp( argv[arg], "-svga" ) ) {
      width = 2048;
      height = 768;
      border = false;
      fullscreen = true;
      stereo = true;
      HMDScreen( HMD_STEREO );
    }
    else if ( !strcmp( argv[arg], "-nVisL" ) ) {
      fprintf( stderr, "LowRes nVis\n" );
      width = 2048;
      height = 768;
      border = false;
      fullscreen = true;
      stereo = true;
//      HMDScreen( HMD_STEREO );
    }
    else if ( !strcmp( argv[arg], "-nVis" ) ) {
      width = 2560;
      height = 1024;
      border = false;
      fullscreen = true;
      stereo = true;
//      HMDScreen( HMD_STEREO );
    }
    else if ( !strcmp( argv[arg], "-noborder" ) ) border = FALSE;
    else if ( !strcmp( argv[arg], "-tracker" ) ) use_tracker = TRUE;
    else if ( !strcmp( argv[arg], "-isense" ) ) use_isense = TRUE;
    else if ( !strcmp( argv[arg], "-udp" ) ) use_udp = TRUE;
    else if ( !strcmp( argv[arg], "-ori" ) ) orientation_only = TRUE;
    else if ( !strcmp( argv[arg], "-sensor" ) ) {
      arg++;
      if ( arg < argc ) sscanf( argv[arg], "%d", &viewpoint_sensor );
      fprintf( stderr, "Using sensor %d.\n", viewpoint_sensor );
    }
    else closure_record = argv[arg];

  }
  
  fprintf( stderr, "Closure record: %s\n", closure_record );
  
    
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
    SimulateSetMovement( viewpoint_sensor, sim_translation, sim_rotation ); 
    
    /* Shift the nominal viewpoint up, then tilt the view back down to look at the target. */
    viewpoint_position[Y] = nominal_head_height;
    SimulateSetLocation( viewpoint_sensor, viewpoint_position );
    viewpoint_orientation[Y][Y] = viewpoint_orientation[Z][Z] = cos( radians( nominal_head_tilt ) );
    viewpoint_orientation[Y][Z] = sin( radians( nominal_head_tilt ) ) ;
    viewpoint_orientation[Z][Y] = - viewpoint_orientation[Y][Z];
    SimulateSetOrientation( viewpoint_sensor, viewpoint_orientation );

    SimulateSetLocation( object_sensor, object_position );
    SimulateSetMovement( object_sensor, sim_translation, sim_rotation ); 
  }
  
  if ( use_isense || use_udp ) {
    if ( UDPTrackerInitClient( &trkr, NULL, DEFAULT_PORT ) ) {
      MessageBox( NULL, "Error opening socket.", "Isense UDP", MB_OK );
      use_isense = NO;
    }
  }
    
 /* 
  * Define a viewing projection with:
  *  45° vertical field-of-view - horizontal fov will be determined by window aspect ratio.
  *  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
  *      that I give for the model room here are in mm.
  *  100.0 to 10000.0  depth clipping planes - making this smaller would improve the depth resolution.
  */
  viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
  
   
  int x = 100, y = 100;
  
  /*
  * Create window. 
  */
  window = new OpenGLWindow();
  window->Border = border; // Remove borders for an HMD display.
  window->FullScreen = fullscreen;

  if ( window->Create( NULL, argv[0], 0, 0, width, height ) ) {
		/*
    * Create sets the new window to be the active window.
    * Setup the lights and materials for that window.
    */
    glDefaultLighting();
    glDefaultMaterial();
//    wall_texture->Define();
  }  
  window->Activate();
  window->SetKeyboardCallback( keyboard_callback );

  // Shade model
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

  create_objects();

  TimerSet( &frame_timer, 10.0 );
  frame_counter = 0;
  for ( double angle = 0.0; true; angle += 5.0 ) {

    if ( TimerTimeout( &frame_timer )) {
      fprintf( stderr, "Frame rate: %f\n", (double) frame_counter / TimerElapsedTime( &frame_timer ) );

      if ( use_isense ) {
        UDPTrackerGetIsenseData( &trkr, &iSenseData );
        printf("Isense Quarternion %7.3f %7.3f %7.3f %7.3f   %.3f\n",
        iSenseData.Station[0].orientation[0],
        iSenseData.Station[0].orientation[1],
        iSenseData.Station[0].orientation[2],
        iSenseData.Station[0].orientation[3],
        
        iSenseData.Station[0].timestamp );
      }

      TimerStart( &frame_timer );
      frame_counter = 0;
    }
  
    if ( use_tracker ) {
      data_available = !GetSensorPositionOrientation( viewpoint_sensor, YES, viewpoint_position, viewpoint_orientation );
      if ( data_available ) {
        if ( !orientation_only ) viewpoint->SetPosition( viewpoint_position );
        viewpoint->SetOrientation( viewpoint_orientation );
      }
    }
    else if ( use_isense ) {
      data_available = !( UDPTrackerGetIsenseData( &trkr, &iSenseData ));
      if ( data_available ) {
        isense_to_matrix( iSenseData.Station[0].orientation, viewpoint_orientation );
        isense_to_vector( iSenseData.Station[0].position, viewpoint_position );
        scale_vector( 10.0, viewpoint_position, viewpoint_position );
        if ( !orientation_only ) viewpoint->SetPosition( viewpoint_position );
        viewpoint->SetOrientation( viewpoint_orientation );
      }
    }
    else if ( use_udp ) {
      data_available = !( UDPTrackerGetData( &trkr, &udpData ));
      if ( data_available ) {
        quaternion_to_matrix( udpData.Station[0].orientation, viewpoint_orientation );
        copy_vector( udpData.Station[0].position, viewpoint_position );
        scale_vector( 10.0, viewpoint_position, viewpoint_position );
        if ( !orientation_only ) viewpoint->SetPosition( viewpoint_position );
        viewpoint->SetOrientation( viewpoint_orientation );
      }
    }
    else {
      data_available = data_available;
    }

    if ( use_tracker ) {
      data_available = !GetSensorPositionOrientation( object_sensor, YES, object_position, object_orientation );
        object->SetPosition( object_position );
        object->SetOrientation( object_orientation );
    }
    else if ( use_isense && data_available ) {
      isense_to_matrix( iSenseData.Station[1].orientation, object_orientation );
      isense_to_vector( iSenseData.Station[1].position, object_position );
      scale_vector( 10.0, object_position, object_position );
      object->SetOrientation( object_orientation );
      if ( !orientation_only ) object->SetPosition( object_position );
    }
    else {
      object->SetOrientation( angle, j_vector );
      object->SetPosition( object_position );
    }

    
    window->Clear();

    if ( stereo ) {
      viewpoint->Apply( window, LEFT_EYE );
      render();

      viewpoint->Apply( window, RIGHT_EYE );
      render();
    }
    else {
      viewpoint->Apply( window, CYCLOPS );
      render();
    }
    window->Swap();
    if ( ! window->RunOnce() ) break;
 

    frame_counter++;
    
  }
  
  window->Destroy();
  
  if ( use_tracker ) KillTracker();
  
  RevertScreen();
  
  return( 0 );
  
}

