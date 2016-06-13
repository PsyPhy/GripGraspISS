///
/// Module:	GraspGLObjects
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		03 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#include "stdafx.h"
#include "GraspGLObjects.h"

using namespace PsyPhy;
using namespace Grasp;

// Textures that are used to decorate certain objects.
// These files have to be in the execution directory.
const char *GraspGLObjects::wall_texture_bitmap = "Bmp\\Rockwall.bmp";
const char *GraspGLObjects::references_texture_bitmap = "Bmp\\Metal.bmp";
const char *GraspGLObjects::sky_texture_bitmap= "Bmp\\NightSky.bmp";
const char *GraspGLObjects::timeout_texture_bitmap = "Bmp\\Timeout.bmp";
const char *GraspGLObjects::head_misalign_texture_bitmap = "Bmp\\Headtilted.bmp";
			
// Dimensions of the room.
const double GraspGLObjects::room_radius = 1000.0;
const double GraspGLObjects::room_length = 6000.0;
const int GraspGLObjects::room_facets = 32;
const double GraspGLObjects::reference_bars = 5;
const double GraspGLObjects::reference_bar_radius = 50.0;
const double GraspGLObjects::reference_bar_facets = 8;

// Dimensions of objects in the room;

// Head shape is specified as the axis radii of an ellipsoid.
const Vector3 GraspGLObjects::head_shape = { 100.0, 150.0, 125.0 };
// Torso shape is a slab with width, height and thickness.
const Vector3 GraspGLObjects::torso_shape = { 200.0, 300.0, 125.0 };

// We may want to use a circular arrow to indicate to the subject which
//  way to tilt the head, in addition to color cues.
const double GraspGLObjects::prompt_radius = 160.0;
const Vector3 GraspGLObjects::prompt_location = { 0.0, 0.0, -500.0 };

// The target is a line of spheres.
const double GraspGLObjects::target_ball_radius = 100.0;
const double GraspGLObjects::target_ball_spacing = 2.0 * room_radius / 7.5;
const int GraspGLObjects::target_balls = 3;

const Vector3 GraspGLObjects::target_location = { 0.0, 0.0, -room_length / 2.0 };
const Vector3 GraspGLObjects::sky_location = { 0.0, 0.0, - room_length / 2.0 };

const double GraspGLObjects::finger_ball_radius = 10.0;
const double GraspGLObjects::finger_length = 100.0;

const double GraspGLObjects::headRollTolerance = 5.0;		// Tolerance around desired head tilt, in degrees.
const double GraspGLObjects::kkTolerance = 5.0;				// Tolerance for hand orientation in K-K.
const double GraspGLObjects::errorColorMapEpsilon = 10.0;	// Deterimines how quickly an object changes color according to orientation error.
const double GraspGLObjects::errorColorMapTransparency = 0.5;

// Set up the lighting and material properties.
void GraspGLObjects::SetLighting( void ) {
	
	GLfloat fintensity = 0.75;
	GLfloat ambient = fintensity;
	GLfloat diffuse = fintensity;
	GLfloat specular = fintensity;

	// Light definition
	GLfloat LightPosition[] = { 0.0, 0.0, 1000.0, 0.0 };
	GLfloat LightAmbient[]  = { ambient, ambient, ambient, 1.0};
	GLfloat LightDiffuse[]  = { diffuse, diffuse, diffuse, 1.0};
	GLfloat LightSpecular[] = { specular, specular, specular, 1.0};

	glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
	glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

	glEnable( GL_LIGHT0 );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	glEnable( GL_BLEND );

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
}

void GraspGLObjects::CreateTextures( void ) {
	sky_texture = new Texture( sky_texture_bitmap, 2000, 2000 );
	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 1000, 2000 );
	references_texture = new Texture( references_texture_bitmap, 500, 500 );
	timeout_texture = new Texture( timeout_texture_bitmap );
	head_misalign_texture = new Texture( head_misalign_texture_bitmap );

}

Assembly *GraspGLObjects::CreateStarrySky( void ) {
	Assembly *sky = new Assembly();
	Patch *patch = new Patch( room_radius * 2.2, room_radius * 2.2 );
	patch->SetTexture( sky_texture );
	sky->AddComponent( patch );
	sky->SetColor(WHITE);
	return( sky );
}

Assembly *GraspGLObjects::CreateDarkSky( void ) {
	Assembly *darkSky = new Assembly();
	Patch *patch = new Patch( room_radius * 2.2, room_radius * 2.2 );
	patch->SetTexture( sky_texture );// there is something strange if I dont put the texture the ligthing of the whole tunnel changes
	darkSky->AddComponent( patch );
	darkSky->SetColor( BLACK );
	darkSky->enabled = false;
	return( darkSky );
}

Assembly *GraspGLObjects::CreateRoom( void ) {

	Assembly *room = new Assembly();
	room->SetColor( BLACK );

	//Tunnel Plus references
	tunnel = new Cylinder( room_radius, room_radius, room_length, room_facets );
	tunnel->SetColor( WHITE );
	tunnel->SetTexture( wall_texture );
	tunnel->SetOrientation( 90.0, 0.0, 0.0 );
	room->AddComponent( tunnel );

	for (int i=0; i < reference_bars; i++ ){ 
		Cylinder *referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, room_length, reference_bar_facets );
		referenceBar->SetOffset( room_radius- reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  1.0 - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0 );
		// The texturing on the bars may be commented out for the moment because it lengthens the rendering time too much.
		referenceBar->SetTexture( references_texture );
		room->AddComponent( referenceBar );
		referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius,  room_length );
		referenceBar->SetOffset( room_radius - reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( - 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  (double) i / reference_bars, (double) i / reference_bars, (double) i / reference_bars, 1.0 );
		// See above.
		referenceBar->SetTexture( references_texture );
		room->AddComponent( referenceBar );
	}

	Sphere *sphere = new Sphere( target_ball_radius );
	sphere->SetPosition( 0.0, 0.0, room_length / 2.0 );
	sphere->SetColor( RED );
	room->AddComponent( sphere );

	return room;
}

Assembly *GraspGLObjects::CreateOrientationTarget( void ) {

	Assembly *target = new Assembly();
	for (int trg = - target_balls ; trg <= target_balls ; trg++ ){
		Sphere *sphere = new Sphere( target_ball_radius );
		sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
		sphere->SetColor(( 255.0 - abs(trg) * 50.0 ) / 255.0, 0.0, 0.0);
		target->AddComponent( sphere );
	}
	return target;
}

Assembly *GraspGLObjects::CreateResponse( void ) {
	static int fingers = target_balls - 1;
	static double finger_spacing = target_ball_radius;
	Assembly *projectiles = new Assembly();
	for ( int trg = - fingers; trg <= fingers ; trg++ ){
		Sphere *sphere = new Sphere( target_ball_radius * 0.75 );
		sphere->SetColor( 200.0f/255.0f, (75.0f + float(trg) * 75.0f/2.0f)/255.0f , 0.0f, 1.0f  );
		// Space the balls vertically.
		sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
		projectiles->AddComponent( sphere );
	}
	return projectiles;
}

Assembly *GraspGLObjects::CreatePositionOnlyTarget( void ) {

	Assembly *target = new Assembly();
	Sphere *sphere = new Sphere( target_ball_radius );
	sphere->SetColor(1.0, 165.0/255.0, 0.0);
	target->AddComponent( sphere );
	return target;

}

Glasses *GraspGLObjects::CreateGlasses( void ) {

	static double visor_radius = 350.0;
	// Create a circular portal to look through to avoid visual cues about the egocentric reference frame.
	Glasses *glasses = new Glasses( visor_radius - 70.0, visor_radius, room_radius, room_radius, 16 );
	return glasses;
}

bool GraspGLObjects::SetColorByRollError( OpenGLObject *object, double desired_roll_angle, double tolerance, double epsilon ) {

#if 0
	static double transparency = 0.75;
	Vector3 rotated;
	// Rotate the unit vector in the X direction by the orientation of the object.
	// Then compute the orientation of that vector in the frontal (XY) plane.
	// This calculation becomes ill conditioned as the yaw angle approaches 90�,
	// but for the moment we assume that pitch and yaw will be small.
	MultiplyVector( rotated, iVector, object->orientation );
	double error = fabs( roll_angle - ToDegrees( atan2( rotated[Y], rotated[X] ) ));
	// fOutputDebugString( "%s\n", vstr( rotated ) );
	if ( rotated[X] < 0.0 ) {
		// Error is more than 90�. Clamp to red and return false to say out of tolerance zone.
		object->SetColor( 1.0, 0.0, 0.0, transparency );
		return( false );
	}

	// @Michele
	// This simple formula goes nicely from gree to yellow to red as the angle increases.
	// But please replace it with the calculation of a set of color values based on the angular error computed above.
	double hypotenuse = sqrt( rotated[X] * rotated[X] + rotated[Y] * rotated[Y] );
	double green = abs( rotated[X] ) / hypotenuse;
	double red = abs( rotated[Y] ) / hypotenuse;
	// fOutputDebugString( "%s  r: %.3lf g: %.3lf %f\n", vstr( rotated ), red, green, error );
	object->SetColor( red, green, 0.0, 0.75 );
#endif

	// Compute the roll angle of the object that is being followed.
	// I got this off of a web site. It seems to work for gaze close to straight ahead,
	// but I'm not sure that it will work well for pitch or yaw far from zero.
	// In fact, I am pretty sure it is the same thing as what I did above by rotating the iVector.
	// We need a reliable method for computing the roll angle independent from pitch and yaw.
	double object_roll_angle = ToDegrees( atan2( object->orientation[0][1], object->orientation[0][0] ) );
	// Compute the error with respect to the desired roll angle.
	double error = fabs( desired_roll_angle - object_roll_angle );

#if 0
	// fOutputDebugString( "Desired: %6.2f  Measured: %6.2f  Error: %6.2f\n", desired_roll_angle, object_roll_angle, error );


	// Michele's formula for the color transitions.
	//
	// There are a number of things that I do not know how to explain:
	//  Why epsilon * 0.2 and not simply epsilon to be in the good zone?
	//  Why (2*epsilon - epsilon) and not just epsilon?
	//  Why + 0.0?
	//  Why all the divides by 255? 
	if ( error < epsilon * 0.2 ){//GREEN
		object->SetColor( 0.0/255.0, 255.0/255.0,  200.0/255.0, 0.5 );
		return(true);
	} else {
		if ( error > ( 2 * epsilon ) ){//RED
			object->SetColor(255.0/255.0, 0.0/255.0,  0.0/255.0, 0.5);
		} else {
			if ( error > epsilon ){//Yellow->red
				object->SetColor(200.0/255.0, (200.0*(1-(error-epsilon)/(2*epsilon-epsilon))+0.0)/255.0,  0.0/255.0, 0.5);
			} 
			else { //green->yellow
				object->SetColor((200.0*(error-epsilon*0.20)/(epsilon-epsilon*0.2)+0.0)/255.0, 200.0/255.0,  0.0/255.0, 0.5);
			}
		}
		return(false);
	}
#endif

	if ( error < tolerance ){//GREEN
		// If we are within the defined tolerance the color is simply green.
		// In a future version, perhaps it should turn towards yellow when approaching the edges.
		double fade = pow( error / tolerance / 2.0, 2 );;
		object->SetColor( fade, 1.0,  0.0, errorColorMapTransparency );
		return(true);
	} else {
		// Colors will change as one moves farther from the edge of the good zone.
		double from_edge = error - tolerance;
		if ( from_edge < epsilon ) {
			// Go from cyan right at the edge to blue.
			double fade = from_edge / epsilon;
			object->SetColor( 0.0, 1.0 - fade, 1.0, errorColorMapTransparency );
		}
		else if ( from_edge < 2.0 * epsilon ) {
			// Go from blue to magenta.
			double fade = ( from_edge - epsilon ) / epsilon;
			object->SetColor( fade, 0.0, 1.0, errorColorMapTransparency );
		}
		// If more than 2 epsilons, the color is magenta (because ESA does not like red).
		else object->SetColor( 1.0, 0.0, 1.0, errorColorMapTransparency );

		return(false);
	}

}

bool GraspGLObjects::ColorGlasses( double roll_angle ) {
	// Color will be set according to the difference between the desired and measured head angles.
	bool is_aligned =  SetColorByRollError( glasses, roll_angle, headRollTolerance, errorColorMapEpsilon );
	return ( is_aligned );
}

Assembly *GraspGLObjects::CreateVisualTool( void ) {

	Assembly *tool = new Assembly();

	// Create a set of 'fingers', each of which is a 'capsule' composed of a tube with rounded caps.
	// There are as many fingers as target balls. This could change.
	static int fingers = target_balls-1;
	static double finger_spacing = finger_ball_radius*2;

	for ( int trg = - fingers; trg <= fingers ; trg++ ){

		// Each finger is a 'capsule' composed of a cylinder that is capped on each end with a sphere.
		Assembly *finger = new Assembly();
		Sphere *sphere = new Sphere( finger_ball_radius );
		sphere->SetPosition( 0.0, 0.0, 0.0 );
		finger->AddComponent( sphere );
		Cylinder *cylinder = new Cylinder( finger_ball_radius, finger_ball_radius, finger_length );
		cylinder->SetPosition( 0.0, 0.0, - finger_length / 2 );
		finger->AddComponent( cylinder );
		sphere = new Sphere( finger_ball_radius );
		sphere->SetPosition( 0.0, 0.0, - finger_length );
		finger->AddComponent( sphere );

		// Create a color that varies as a function of the finger's position and apply it to the entire finger.
		finger->SetColor( 200.0f/255.0f, (75.0f + float(trg) * 75.0f/2.0f)/255.0f , 0.0f, 1.0f  );

		// Space the fingers vertically.
		finger->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		tool->AddComponent( finger );
	}
	// Add a laser pointer to the end.
	Assembly *laser = CreateLaserPointer();
	// This laser is always on and always the same color.
	laser->SetColor( RED );
	tool->AddComponent( laser );
	return tool;
}

Assembly *GraspGLObjects::CreateKinestheticTool( void ) {

	Assembly *finger = new Assembly();
	Sphere *sphere = new Sphere( finger_ball_radius );
	sphere->SetPosition( 0.0, 0.0, 0.0 );
	finger->AddComponent( sphere );
	Cylinder *cylinder = new Cylinder( finger_ball_radius, finger_ball_radius, finger_length );
	cylinder->SetPosition( 0.0, 0.0, - finger_length / 2 );
	finger->AddComponent( cylinder );
	sphere = new Sphere( finger_ball_radius );
	sphere->SetPosition( 0.0, 0.0, - finger_length );
	finger->AddComponent( sphere );
	// Add a laser pointer to the end.
	Assembly *laser = CreateLaserPointer();
	finger->AddComponent( laser );
	// Set a default color. I like blue.
	// In K-K color will be varied according to the orientation error.
	finger->SetColor( 0.0, 0.0, 1.0, 1.0 );
	return finger;

}

Assembly * GraspGLObjects::CreateLaserPointer( void ) {
	Assembly *laserPointer = new Assembly();
	Sphere *sphere = new Sphere( finger_ball_radius*2.0 );
	sphere->SetPosition( 0.0, 0.0, -(room_length/2.0-1000.0) );
	laserPointer->AddComponent( sphere );
	return laserPointer;
}

bool GraspGLObjects::ColorKK( double error ) {
	return( SetColorByRollError( kkTool, error, kkTolerance, errorColorMapEpsilon ) );
}

// Group all the elements that move with the hand into a single entity.
// Each component will be activated or deactivated separately, but their pose will be set
// jointly by accessing the hand object.
Yoke *GraspGLObjects::CreateHand( void ) {
	Yoke *hand = new Yoke();
	hand->AddComponent( kTool );
	hand->AddComponent( vkTool );
	hand->AddComponent( kkTool );
	return( hand );
}

Yoke *GraspGLObjects::CreateHUD( void ) {
	Yoke *yoke = new Yoke();
		
	// The glasses will be positioned at the same place as the head based on the tracker.
	// By setting the offset in depth, the glasses will be positioned a bit in front of the subject.
	glasses->SetOffset( 0.0, 0.0, -400.0 );
	yoke->AddComponent( glasses );
	timeoutIndicator->SetOffset( prompt_location );
	yoke->AddComponent( timeoutIndicator );
	headMisalignIndicator->SetOffset( prompt_location );
	yoke->AddComponent( headMisalignIndicator );
	vTool->SetOffset( 0.0, 0.0, -200.0 );
	yoke->AddComponent( vTool );
	return( yoke );

}

Assembly *GraspGLObjects::CreateProjectiles( void ) {

	Assembly *projectiles = new Assembly();
	static int fingers = target_balls - 1;
	static double finger_spacing = finger_ball_radius * 2;

	for ( int trg = - fingers; trg <= fingers ; trg++ ){

		Sphere *sphere = new Sphere( finger_ball_radius*1.0 );
		// Create a color that varies as a function of the ball's position.
		sphere->SetColor( 200.0f/255.0f, (75.0f + float(trg) * 75.0f / 2.0f ) / 255.0f , 0.0f, 1.0f );
		// Space the balls vertically.
		sphere->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		projectiles->AddComponent( sphere );
		
	}

	return projectiles;

}
Assembly *GraspGLObjects::CreateTiltPrompt( void ) {

	Assembly *prompt = new Assembly();
	
	// Size of the circular arrow, where 1.0 = 360�.
	static double size = 0.85;

	Annulus *donut = new Annulus( prompt_radius, prompt_radius / 6.0, size, 20, 20 );
	donut->SetAttitude( 0.0, 90.0, 0.0 );
	prompt->AddComponent( donut );

	TaperedAnnulus *tip = new TaperedAnnulus( prompt_radius, prompt_radius / 3.0, 1.0, 0.05, 20 );
	tip->SetAttitude( 0.0, 90.0, 0.0 );
	tip->SetOrientation( - size * 360.0, 0.0, 0.0 );
	prompt->AddComponent( tip );

	Ellipsoid *base = new Ellipsoid ( prompt_radius / 6.0, prompt_radius / 12.0, prompt_radius / 6.0 );
	base->SetPosition( prompt_radius, 0.0, 0.0 );
	prompt->AddComponent( base );
	prompt->SetAttitude( - 15.0, 0.0, 0.0 );
	prompt->SetColor( 0.5, 0.0, 0.4 );

	return prompt;

}

Assembly *GraspGLObjects::CreateSuccessIndicator( void ) {
	Assembly	*assembly = new Assembly();
	Sphere		*sphere = new Sphere( prompt_radius );
	sphere->SetColor( GREEN );
	assembly->AddComponent( sphere );
	return assembly;
}

Assembly *GraspGLObjects::CreateTimeoutIndicator( void ) {
	// For the moment the indicator for a timeout is just a magenta 
	// sphere. I would like to create something that is more intuitive.
	Assembly	*assembly = new Assembly();
	Disk		*surface;
	
	surface = new Disk( 120.0, 0.0, 128 );
	surface->SetColor( 0.9, 0.9, 1.0 );
	surface->SetTexture( timeout_texture );
	assembly->AddComponent( surface );

	return assembly;
}

Assembly *GraspGLObjects::CreateHeadMisalignIndicator( void ) {
	// For the moment the indicator for a timeout is just a magenta 
	// sphere. I would like to create something that is more intuitive.
	Assembly	*assembly = new Assembly();
	Disk		*surface;
	
	surface = new Disk( 120.0, 0.0, 128 );
	surface->SetColor( 0.9, 0.9, 1.0 );
	surface->SetTexture( head_misalign_texture );
	assembly->AddComponent( surface );

	return assembly;
}

void GraspGLObjects::CreateVRObjects( void ) {

	glUsefulInitializeDefault();
	SetLighting();
	CreateTextures();

	room = CreateRoom();
	glasses = CreateGlasses();
	ColorGlasses( 0.0 );
	starrySky = CreateStarrySky();
	darkSky = CreateDarkSky();
	// Only one sky should be enabled at a time, so we disable darkSky at this point.
	//darkSky->Disable();
	// The skies get attached to the room so that they move (tilt) with it.
	// This means that you don't actually have to draw the skies explicitly.
	room->AddComponent( starrySky );
	room->AddComponent( darkSky );

	orientationTarget = CreateOrientationTarget();
	positionOnlyTarget = CreatePositionOnlyTarget();
	response = CreateResponse();

	tiltPrompt = CreateTiltPrompt();
	successIndicator = CreateSuccessIndicator();
	timeoutIndicator = CreateTimeoutIndicator();
	headMisalignIndicator = CreateHeadMisalignIndicator();
	projectiles = CreateProjectiles();

	// Orientated tool used when responding with only visual feedback (e.g. V-V).
	vTool = CreateVisualTool();
	// Orientated tool that allows visual feedback of the hand's orientation.
	vkTool = CreateVisualTool();
	// A tool that allows one to point, but does not show the roll orientation.
	kTool = CreateKinestheticTool();
	// Same as the above, but this one we use when presenting a kinesthetic target in K-K.
	kkTool = CreateKinestheticTool();
	// Collect all the things that may be attached to the hand.
	hand = CreateHand();
	// And to the gaze.
	hud = CreateHUD();
}

// Place objects at default locations.
void GraspGLObjects::PlaceVRObjects( void ) {
	starrySky->SetPosition( sky_location );
	darkSky->SetPosition( sky_location );
	orientationTarget->SetPosition( target_location );
	positionOnlyTarget->SetPosition( target_location );
	response->SetPosition( target_location[X], target_location[Y], target_location[Z] + target_ball_radius * 2.0 );
}


// Draw the objects that are used during VR rendering.
// Note that only those objects that are currently active are actually drawn.
void GraspGLObjects::DrawVR( void ) {

	SetLighting();

	// Draw the room with non-shiny material.
	glUsefulMatteMaterial();

	// Because the skies are attached to the room, one need not draw them explicitly.
	// I leave these lines here in comments, though, to remind us that they will be drawn.
	// If we ever decide not to attach them to the room, these lines should be uncommented.
	// DrawLightSky();
	// DrawDarkSky();
	room->Draw();
	glasses->Draw();

	// Draw the other objects with the hopes of seeing specular reflections. 
	// I am still trying to get specular reflections to work.
	// Someday, the material should be made part of the object.
	glUsefulShinyMaterial();

	orientationTarget->Draw();
	positionOnlyTarget->Draw();
	response->Draw();
	tiltPrompt->Draw();
	successIndicator->Draw();
	timeoutIndicator->Draw();
	headMisalignIndicator->Draw();
	vTool->Draw();
	kTool->Draw();
	kkTool->Draw();
	projectiles->Draw();

}


// The following objects are not used during the Grasp protocol and are not seen by the subject.
// Rather, these objects are used in the GraspGUI and elswhere to visualize the subject's pose.
// Perhaps they would be better placed in another class so that they are not included when not needed.
Assembly *GraspGLObjects::CreateHead( void ) {

	Assembly *head = new Assembly();
	// Skull
	Ellipsoid *skull = new Ellipsoid( head_shape );
	skull->SetColor( .4f, 0.0f, .4f );
	// Eyes
	head->AddComponent( skull );
	Sphere *sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( -50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( 50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	// Nose
	Cylinder *cylinder = new Cylinder( 20.0, 5.0, 30.0 );	
	cylinder->SetPosition( 0.0, -20.0, -100.0 );
	cylinder->SetOrientation( 0.0, 90.0, 0.0 );
	cylinder->SetColor( YELLOW );
	head->AddComponent( cylinder );
	return head;
}

Assembly *GraspGLObjects::CreateTorso( void ) {

	Assembly *torso = new Assembly();
	Slab *slab = new Slab( torso_shape[X], torso_shape[Y], torso_shape[Z] );
	slab->SetColor( 0.1f, 0.4f, 0.0f );
	torso->AddComponent( slab );
	Disk *disk = new Disk( 50.0 );
	disk->SetPosition( 0.0, 0.0, -40.0 );
	disk->SetColor( 1.0f, 0.7f, 0.0f );
	torso->AddComponent( disk );
	return torso;

}


void GraspGLObjects::CreateAuxiliaryObjects( void ) {
	head = CreateHead();
	torso = CreateTorso();

	hmdStructure = CreateHmdMarkerStructure( "Bdy\\CADHMD.bdy" );
	hmdStructure->SetPosition( 0.0, 0.0, -500.0 );

	handStructure = CreateHandMarkerStructure( "Bdy\\CADHand.bdy" );
	handStructure->SetPosition( 0.0, 0.0, -500.0 );

	chestStructure = CreateChestMarkerStructure( "Bdy\\CADChest.bdy" );
	chestStructure->SetPosition( 0.0, 0.0, -500.0 );

}

void GraspGLObjects::DrawHead( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		head->SetPosition( pose->pose.position );
		head->SetOrientation( pose->pose.orientation );
	}
	head->Draw();
}

void GraspGLObjects::DrawTorso(  TrackerPose *pose  ) {
	if ( pose != nullptr ) {
		torso->SetPosition( pose->pose.position );
		torso->SetOrientation( pose->pose.orientation );
	}
	torso->Draw();
}

void GraspGLObjects::DrawBody( TrackerPose *pose ) {
	Vector3 torso_position;
	CopyVector( torso_position, pose->pose.position );
	torso_position[Y] -= (head_shape[Y] + torso_shape[Y] / 2.0);
	torso->SetPosition( torso_position );
	torso->SetOrientation( nullQuaternion );
	torso->Draw();
	DrawHead( pose );
}

#define STRUCTURE_BALL_RADIUS 7.0
#define STRUCTURE_BAR_RADIUS 10.0

void MarkerStructureGLObject::ShowVisibility( MarkerFrame &marker_frame ) {
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		if ( marker_frame.marker[ modelMarker[mrk].id ].visibility ) component[mrk]->SetColor( GREEN );
		else component[mrk]->SetColor( 0.1, 0.0, 0.05, 1.0 );
	}
}

void MarkerStructureGLObject::AddBar( int marker1, int marker2 ) {

	Slab *slab;
	Vector3 center, delta;

	AddVectors( center, modelMarker[marker1].position, modelMarker[marker2].position );
	ScaleVector( center, center, 0.5 );
	SubtractVectors( delta, modelMarker[marker1].position, modelMarker[marker2].position );
	slab = new Slab( STRUCTURE_BAR_RADIUS, VectorNorm( delta ) * 1.1, STRUCTURE_BAR_RADIUS );
	slab->SetPosition( center[X], center[Y], center[Z] + STRUCTURE_BAR_RADIUS );
	slab->SetOrientation( ToDegrees( atan2( delta[X], delta[Y] ) ), 0.0, 0.0 );
	slab->SetColor( 1.0, 1.0, 0.0, 0.35 );
	AddComponent( slab );
}

MarkerStructureGLObject::MarkerStructureGLObject( char *model_file ) {

	if ( model_file ) {
		FILE *fp = fopen( model_file, "r" );
		fAbortMessageOnCondition( ( NULL == fp ), "ReadModelMarkerPositions()", "Error opening %s for read.", model_file );
		int mrk;
		for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
			int id, items;
			double x, y, z;
			items = fscanf( fp, "%d %lf\t %lf\t %lf", &id, &x, &y, &z );
			if ( items != 4  ) break;
			fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), "ReadModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
			modelMarker[mrk].id = id;
			modelMarker[mrk].position[X] = x;
			modelMarker[mrk].position[Y] = y;
			modelMarker[mrk].position[Z] = z;
		}
		nModelMarkers = mrk;	
		fclose( fp );
	}
	else nModelMarkers = 0;

	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		Sphere *sphere = new Sphere( STRUCTURE_BALL_RADIUS );
		sphere->SetPosition( modelMarker[mrk].position );
		sphere->SetColor( 0.2, 0.0, 0.2, 1.0 );
		AddComponent( sphere );
	}

}

MarkerStructureGLObject *GraspGLObjects::CreateHmdMarkerStructure ( char *model_file ) {
	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );
	structure->AddBar( 1, 2 );
	structure->AddBar( 5, 6 );
	structure->AddBar( 0, 3 );
	structure->AddBar( 4, 7 );

	// The real structure has right-angle elbows but I am using a slanted piece to connect the bars.

	Slab *slab;
	Vector3 center, delta, corner;

	AddVectors( center, structure->modelMarker[0].position, structure->modelMarker[3].position );
	ScaleVector( center, center, 0.5 );
	SubtractVectors( delta, center, structure->modelMarker[1].position );
	AddVectors( corner, center, structure->modelMarker[1].position );
	ScaleVector( corner, corner, 0.5 );
	slab = new Slab( VectorNorm( delta ) * 1.1, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	slab->SetPosition( corner );
	slab->SetOrientation(  0.0, 0.0, ToDegrees( atan2( delta[Z], delta[X] ) ) );
	slab->SetColor( 1.0, 1.0, 0.0, 0.15 );
	structure->AddComponent( slab );

	AddVectors( center, structure->modelMarker[4].position, structure->modelMarker[7].position );
	ScaleVector( center, center, 0.5 );
	SubtractVectors( delta, center, structure->modelMarker[2].position );
	AddVectors( corner, center, structure->modelMarker[2].position );
	ScaleVector( corner, corner, 0.5 );
	slab = new Slab( VectorNorm( delta ) * 1.1, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	slab->SetPosition( corner );
	slab->SetOrientation(  0.0, 0.0, ToDegrees( atan2( delta[Z], delta[X] ) ) );
	slab->SetColor( 1.0, 1.0, 0.0, 0.15 );
	structure->AddComponent( slab );

	return( structure );
}

MarkerStructureGLObject *GraspGLObjects::CreateHandMarkerStructure ( char *model_file ) {
	static double vertices[][2] = {{-100, -40}, {-100, 40}, {0, 70}, {100, 50}, {100, -50}, {0, -70}, {-100, -40}};
	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );
	structure->AddBar( 0, 1 );
	structure->AddBar( 1, 2 );
	structure->AddBar( 2, 3 );
	structure->AddBar( 3, 0 );
	structure->AddBar( 4, 5 );
	structure->AddBar( 5, 6 );
	structure->AddBar( 6, 7 );
	structure->AddBar( 7, 4 );	

	Extrusion *plate = new Extrusion( 10.0, vertices, 7 );
	plate->SetOrientation( 0.0, 0.0, 90.0 );
	plate->SetPosition( -10.0, 0.0, 0.0 );
	
	structure->AddComponent( plate );
	return( structure );
}

MarkerStructureGLObject *GraspGLObjects::CreateChestMarkerStructure ( char *model_file ) {
	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );
	double vertices[8][2];

	vertices[0][X] = structure->modelMarker[1].position[X];
	vertices[0][Y] = structure->modelMarker[1].position[Y];
	vertices[1][X] = structure->modelMarker[5].position[X];
	vertices[1][Y] = structure->modelMarker[5].position[Y];
	vertices[2][X] = structure->modelMarker[2].position[X];
	vertices[2][Y] = structure->modelMarker[2].position[Y];
	vertices[3][X] = structure->modelMarker[6].position[X];
	vertices[3][Y] = structure->modelMarker[6].position[Y];
	Extrusion *plate = new Extrusion( STRUCTURE_BAR_RADIUS, vertices, 4 );
	plate->SetColor( 1.0, 1.0, 0.0, 0.35 );
	plate->SetPosition( 0.0, 0.0, ( structure->modelMarker[1].position[Z] + structure->modelMarker[5].position[Z] + structure->modelMarker[2].position[Z] + structure->modelMarker[6].position[Z] )/ 4.0 + STRUCTURE_BAR_RADIUS );
	structure->AddComponent( plate );

	vertices[0][X] = structure->modelMarker[0].position[X];
	vertices[0][Y] = structure->modelMarker[0].position[Y];
	vertices[1][X] = structure->modelMarker[3].position[X];
	vertices[1][Y] = structure->modelMarker[3].position[Y];
	vertices[2][X] = structure->modelMarker[4].position[X];
	vertices[2][Y] = structure->modelMarker[4].position[Y];
	vertices[3][X] = structure->modelMarker[7].position[X];
	vertices[3][Y] = structure->modelMarker[7].position[Y];
	plate = new Extrusion( STRUCTURE_BAR_RADIUS, vertices, 4 );
	structure->AddComponent( plate );
	plate->SetPosition( 0.0, 0.0, ( structure->modelMarker[7].position[Z] + structure->modelMarker[4].position[Z] + structure->modelMarker[3].position[Z] + structure->modelMarker[0].position[Z] )/ 4.0 + STRUCTURE_BAR_RADIUS );
	plate->SetColor( 1.0, 0., 0.0, 0.35 );

	return( structure );
}

Glasses::Glasses( double inner_radius, double outer_radius, double width, double height, int facets ) {

	// Create a black mask with a circular hole to look through.
	// The idea is that it should be big enough to mask out to the edges of the visual scene.
	blinders = new Hole( outer_radius, width, height, facets );
	blinders->SetColor( BLACK );
	AddComponent( blinders );
	// Now create a frame around the portal. In the Grasp experiment, this frame can change colors
	//  in order to reflect the orientation of the head. I also think that having concentric circular
	//  borders, rather than a single black edge, further reduces the saliencey of the non-circular HMD viewing frustrum.
	frame = new Hole( inner_radius, width, height, facets );
	frame->SetOffset( 0.0, 0.0, - 20.0 );
	AddComponent( frame );
}

// Override SetColor so that the blinders are always black.
void Glasses::SetColor( float r, float g, float b, float a = 1.0 ) {
	Assembly::SetColor( r, g, b, a );
	blinders->SetColor( BLACK );
}

