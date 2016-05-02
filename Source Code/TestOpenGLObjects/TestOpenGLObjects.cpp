// TestOpenGLObjects.cpp : Defines the entry point for the console application.
//

// This is a demonstration program, showing how the OpenGLObjects system works.
// It opens a simple OpenGLWindow and renders a few objects in a visual scence.
#include "stdafx.h"

#include "../Useful/OpenGLUseful.h"
#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

using namespace PsyPhy;

///////////////////////////////////////////////////////////////////////////////////////////////////////

// Initial parameters of the window and the simulated objects.
// Some of these can be changed by commandline arguments (see below).
// They are only used at start up to determine the default values.

// Parameters of the window.
int width = 640;
int height = 480;
bool border = true;
bool stereo = false;
bool fullscreen = false;
double PI=3.14;
// Place the viewer at the origin, looking in the zero orientation.
double initial_viewpoint_position[3] = { 0.0, 0.0, 0.0 };
double initial_viewpoint_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

// In the OpenGL coordinate system, zero orientation means that one is looking
//  down the negative Z axis with positive Y up and positive X to the right. Here we $
//  place the object where we can see it.
double initial_object_position[3] = { 0.0, 0.0, - 1500.0 };
double initial_object_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

// Dimensions of the room.
double room_width = 1000.0;
double room_height = 1000.0;
double room_length = 6000.0;
double wall_thickness = 10.0;

// A texture that is used to decorate the walls of the room.
// This file has to be in the execution directory.
char *wall_texture_bitmap = "lime.bmp";
char *sky_texture_bitmap= "NightSky.bmp";

///////////////////////////////////////////////////////////////////////////////////////////////////////

// These hold the OpenGLObjects that are shared across the local routines.
OpenGLWindow	*window;			// The window on the screen.
Viewpoint		*viewpoint;			// The viewpoint into the virtual world.
Assembly		*room;				// A walled room in the virtual world.
Assembly		*Response;
Assembly		*HandFeedback;
Assembly		*Target;
Assembly		*LaserPointer;

Texture			*wall_texture;		// The texture that is applied to the walls.
Texture			*sky_texture;
Assembly		*object;			// A 3D object that moves within the virtual room.
	
Cylinder *cylinder;


///////////////////////////////////////////////////////////////////////////////////////////////////////

void create_objects( void ) {
	
	// Local objects used to construct the OpenGLObject assemblies.
	Sphere *sphere;
	Slab *slab;
	Disk *disk;

	Cylinder *tunnel;
	Cylinder *reference;
	Cylinder *errortunnel;
	Slab *sky;

	// Create a room to put the object in.

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 2000, 4000 );
	sky_texture = new Texture(sky_texture_bitmap,2000,2000);
	
	room = new Assembly();
	room->SetColor( WHITE );

	//box = new Box( room_height, room_width, room_length );
	//box->SetColor( GRAY );
	//box->SetTexture( wall_texture );
	//room->AddComponent( box );

	//Tunnel Plus references
	tunnel = new Cylinder( room_height, room_width, room_length );
	tunnel->SetColor( WHITE );
	tunnel->SetTexture( wall_texture );
	tunnel->SetOrientation(90,0.0,0.0);
	room->AddComponent( tunnel );

	for (int i=0;i<6; i++){ 
		reference = new Cylinder( 50.0, 50.0, room_length );
		reference->SetPosition(1000.0*sin(i/10.0*2*PI),1000.0*cos(i/10.0*2*PI),0.0);
		reference->SetColor( (1.0-i/5.0), (1.0-i/5.0), (1.0-i/5.0), 255);
		reference->SetTexture( wall_texture );
		room->AddComponent(reference);
		reference = new Cylinder( 50.0, 50.0, room_length );
		reference->SetPosition(1000.0*-sin(i/10.0*2*PI),1000.0*cos(i/10.0*2*PI),0.0);
		reference->SetColor( (1.0-i/5.0), (1.0-i/5.0), (1.0-i/5.0), 255);
		reference->SetTexture( wall_texture );
		room->AddComponent(reference);
	}

	//Error Tunnel
	errortunnel = new Cylinder( room_height, room_width, room_length );
	errortunnel->SetColor( RED );
	errortunnel->SetOrientation(PI/2,0.0,0.0);
	//room->AddComponent( errortunnel );


	// A disk on the front wall, something to look at.
	sky = new Slab( 2000.0, 2000.0, 1.0 );
	sky->SetPosition( 0.0, 0.0, - room_length / 2.0  );
	//disk->SetColor( ORANGE );
	sky->SetTexture(sky_texture);
	room->AddComponent( sky );

	// Response
	Response = new Assembly();
	for (int trg=-2; trg<=2; trg++){
		sphere = new Sphere(50);
		sphere->SetPosition(0.0,0.0+(2000.0)/5*trg,0.0);
		sphere->SetColor(100.0/255.0,(75.0+trg*25.0)/255.0,0);
		Response->AddComponent(sphere);
	}
	Response->SetPosition(0.0, 0.0,-room_length/2+100);

	//hand feedback
	HandFeedback = new Assembly();
	for (int trg=-2; trg<=2; trg++){
		sphere = new Sphere(50);
		sphere->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,0.0);
		sphere->SetColor(100.0/255.0,(75.0+trg*25.0)/255.0,0);
		cylinder = new Cylinder(50,50,200);
		cylinder->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,-200.0/2);
		HandFeedback->AddComponent(sphere);
		HandFeedback->AddComponent(cylinder);
		sphere = new Sphere(50);
		sphere->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,-200.0);
		HandFeedback->AddComponent(sphere);
	}
	HandFeedback->SetPosition(0.0,0.0, -500);

	//LaserPOinter
	LaserPointer = new Assembly();
	disk = new Disk(50);
	disk->SetPosition(0.0, 0.0, (room_length-1000));

	//Target
	Target = new Assembly();
	for (int trg=-3; trg<=3; trg++){
		sphere = new Sphere(100);
		sphere->SetPosition(0.0,0.0+(1000.0-(-1000.0))/7.5*trg,0.0);
		sphere->SetColor((75.0-abs(trg)*10.0)/255.0,0.0,0.0);
		Target->AddComponent(sphere);
	}
	Target->SetPosition(0.0,0.0, -room_length/2+50);


	// The center of the room is at the origin.
	room->SetPosition( 0.0, 0.0, 0.0 );
	room->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a viewpoint into the virtual room.
   /* 
	* Define a viewing projection with:
	*  45° vertical field-of-view - horizontal fov will be determined by window aspect ratio.
	*  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
	*      that I give for the model room here are in mm.
	*  100.0 to 10000.0  depth clipping planes - making this smaller would improve the depth resolution.
	*/
	viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);

	// Initial viewpoint is at the center of the room in the null orientation.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a simple object to look at.
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

	// Initialize the position of the mobile object.
	object->SetPosition( initial_object_position);
	object->SetOrientation( initial_object_orientation );

	// A cylinder 'cause we are testing code to draw cylinders.
	//cylinder = new Cylinder( 50.0, 50.0, 200.0 );
	//cylinder->SetPosition( 0.0, - room_height / 8.0, - room_length / 8.0 );
	//cylinder->SetOrientation( 0.0, 90.0, 0.0 );
	// cylinder->SetColor( BLUE );
	//cylinder->SetTexture( wall_texture, 5.0 );
	//room->AddComponent( cylinder );

}

void render( void ) {

	// Draw the virtual room.
	room->Draw();
	Response->Draw();
	HandFeedback->Draw();
	Target->Draw();
	Sleep( 10 );

	// Draw the complex object at its current position. 
	//object->Draw();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	int frame_counter;

	// Parse the command line arguments.
	int arg;
	for ( arg = 1; arg < argc; arg++ ) {

		// These options determine the size and form of the window.
		if ( !strcmp( argv[arg], "-full" ) ) {
			width = 640;
			height = 480;
			border = false;
			fullscreen = true;
			stereo = false;
		}
		else if ( !strcmp( argv[arg], "-hmd" ) ) {
			width = 1024;
			height = 384;
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
			//      HMDScreen( HMD_STEREO );
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
	}

	// Create and instance of a window object.
	// This does not yet create the actual window on the screen.
	window = new OpenGLWindow();

	// Set border and fullscreen mode depending on arguments above.
	window->Border = border; 
	window->FullScreen = fullscreen;

	// Create sets the new window to be the active window.
	if ( !window->Create( NULL, argv[0], 0, 0, width, height ) ) {
		fMessageBox( MB_OK, "TestOpenGLObjects", "Error creating window." );
		exit( -1 );
	}  
	window->Activate();

	// Initialize the state of the GL graphics engine.
	glUsefulInitializeDefault();

	// Create the room and the moving object.
	create_objects();

	frame_counter = 0;
	for ( double a = 0.0; true; a += 0.05 ) {

		// Oscillate the viewpoint.
		double angle = 45.0 * cos( a );

		object->SetOrientation( angle, object->jVector );
		object->SetPosition( initial_object_position );
		viewpoint->SetOrientation( 0.0, 0.0, 0.1 * angle );
		cylinder->SetOrientation(  0.1 * angle, 45 + 0.1 * angle, 0.0 );
		HandFeedback->SetOrientation(angle,0,0);
		Response->SetOrientation(45.0,0,0);
		window->Clear();

		glUsefulPrepareRendering();

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
	// fMessageBox( MB_OK, "TestOpenGLObjects", "Frame count: %d", frame_counter );
}

