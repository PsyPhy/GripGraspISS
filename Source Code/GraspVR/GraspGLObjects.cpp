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
const Vector3 GraspGLObjects::prompt_location = { 0.0, 0.0, -1500.0 };

// The target is a line of spheres.
const double GraspGLObjects::target_ball_radius = 100.0;
const double GraspGLObjects::target_ball_spacing = 2.0 * room_radius / 7.5;
const int GraspGLObjects::target_balls = 3;

const Vector3 GraspGLObjects::target_location = { 0.0, 0.0, -room_length / 2.0 };
const Vector3 GraspGLObjects::sky_location = { 0.0, 0.0, - room_length / 2.0 };

const double GraspGLObjects::finger_ball_radius = 10.0;
const double GraspGLObjects::finger_length = 100.0;

const double GraspGLObjects::headRollTolerance = 10.0;	// Tolerance around desired head tilt, in degrees.
const double GraspGLObjects::kkTolerance = 5.0;			// Tolerance for hand orientation in K-K.

// Set up the lighting and material properties.
void GraspGLObjects::SetLighting( void ) {
	
	GLfloat fintensity = 0.75;
	GLfloat ambient = fintensity;
	GLfloat diffuse = fintensity;
	GLfloat specular = fintensity;

	// Light definition
	GLfloat LightPosition[] = { 0.0, 0.0, -1000.0, 0.0 };
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
//	tunnel->SetTexture( wall_texture );
	tunnel->SetOrientation( 90.0, 0.0, 0.0 );
	room->AddComponent( tunnel );

	for (int i=0; i < reference_bars; i++ ){ 
		Cylinder *referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, room_length, reference_bar_facets );
		referenceBar->SetOffset( room_radius- reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  1.0 - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0 );
//		referenceBar->SetTexture( references_texture );
		room->AddComponent( referenceBar );
		referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius,  room_length );
		referenceBar->SetOffset( room_radius - reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( - 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  (double) i / reference_bars, (double) i / reference_bars, (double) i / reference_bars, 1.0 );
//		referenceBar->SetTexture( references_texture );
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

Assembly *GraspGLObjects::CreateGlasses( void ) {

	static double visor_radius = 150.0;

	Assembly *glasses = new Assembly();
	glasses->SetColor(WHITE);
	Hole *hole = new Hole( visor_radius, room_radius, room_radius, 8 );
	glasses->AddComponent( hole );
	glasses->SetOffset( 0.0, 0.0, -230.0 );

	return glasses;
}

bool GraspGLObjects::SetColorByRollError( OpenGLObject *object, double roll_angle, double epsilon ) {

	static double transparency = 0.75;
	Vector3 rotated;
	// Rotate the unit vector in the X direction by the orientation of the object.
	// Then compute the orientation of that vector in the frontal (XY) plane.
	// This calculation becomes ill conditioned as the yaw angle approaches 90°,
	// but for the moment we assume that pitch and yaw will be small.
	MultiplyVector( rotated, iVector, object->orientation );
	double error = ToDegrees( atan2( rotated[Y], rotated[X] ) );
	// fOutputDebugString( "%s\n", vstr( rotated ) );
	if ( rotated[X] < 0.0 ) {
		// Error is more than 90°. Clamp to red and return false to say out of tolerance zone.
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
	// Indicate if we are in the tolerance zone or not.
	return( abs( error ) < epsilon );
}

bool GraspGLObjects::ColorGlasses( double roll_angle ) {
	return ( SetColorByRollError( glasses, roll_angle, headRollTolerance ) );
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
	return( SetColorByRollError( kkTool, error, kkTolerance ) );
}

// Group all the elements that move with the hand into a single entity.
// Each component will be activated or deactivated separately, but their pose will be set
// jointly by accessing the hand object.
Yoke *GraspGLObjects::CreateHand( void ) {
	hand = new Yoke();
	hand->AddComponent( vTool );
	hand->AddComponent( kTool );
	hand->AddComponent( kkTool );
	return( hand );
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
	
	// Size of the circular arrow, where 1.0 = 360°.
	static double size = 0.85;

	Annulus *donut = new Annulus( prompt_radius, prompt_radius / 6.0, size, 60, 20 );
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
	projectiles = CreateProjectiles();

	// Orientated tool used when responding with full visual feedback.
	vTool = CreateVisualTool();
	// A tool that allows one to point, but does not show the roll orientation.
	kTool = CreateKinestheticTool();
	// Same as the above, but this one we use when presenting a kinesthetic target in K-K.
	kkTool = CreateKinestheticTool();
	// Collect all the things that may be attached to the hand.
	hand = CreateHand();
}

// Place objects at default locations.
void GraspGLObjects::PlaceVRObjects( void ) {
	starrySky->SetPosition( sky_location );
	darkSky->SetPosition( sky_location );
	orientationTarget->SetPosition( target_location );
	positionOnlyTarget->SetPosition( target_location );
	response->SetPosition( target_location[X], target_location[Y], target_location[Z] + target_ball_radius * 2.0 );
	tiltPrompt->SetPosition( prompt_location );
	glasses->SetPosition( 0.0, 0.0, 0.0 );
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
	DrawRoom();
	DrawGlasses();

	// Draw the other objects with the hopes of seeing specular reflections. 
	// I am still trying to get specular reflections to work.
	// Someday, the material should be made part of the object.
	glUsefulShinyMaterial();

	DrawOrientationTarget();
	DrawPositionOnlyTarget();
	DrawResponse();
	DrawTiltPrompt();
	DrawHand();
	DrawProjectiles();

}

// A set of routines that allows one to set the pose of the various objects and then draw them in 
//  a single operation. This can be convenient, but it goes against the concept of objects having 
//  a current, persistent state that includes its visibility and its pose. In many cases it will make
//  more sense to access the objects directly with obj->SetPosition(), obj->SetOrientation(), obj->Draw(), etc.


// Draw the sky, presumably always at the same position.
// For Grasp, it might make more sense if it is attached to the room so that they tilt together.
// If so, these routines need not be called explicitly. See comments above.
void GraspGLObjects::DrawStarrySky( void ) {
	starrySky->Draw();
}

void GraspGLObjects::DrawDarkSky( void ) {
	darkSky->Draw();
}

void GraspGLObjects::DrawRoom( TrackerPose *pose ) {
	// If the caller has specified a pose, move the room to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != static_cast<TrackerPose *>( nullptr ) ) {
		room->SetPosition( pose->pose.position );
		room->SetOrientation( pose->pose.orientation );
	}
	room->Draw();
}

void GraspGLObjects::DrawOrientationTarget( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		orientationTarget->SetPosition( pose->pose.position );
		orientationTarget->SetOrientation( pose->pose.orientation );
	}
	orientationTarget->Draw();
}

void GraspGLObjects::DrawResponse( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		response->SetPosition( pose->pose.position );
		response->SetOrientation( pose->pose.orientation );
	}
	response->Draw();
}
void GraspGLObjects::DrawPositionOnlyTarget( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		positionOnlyTarget->SetPosition( pose->pose.position );
		positionOnlyTarget->SetOrientation( pose->pose.orientation );
	}
	positionOnlyTarget->Draw();
}
void GraspGLObjects::DrawHand( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		hand->SetPosition( pose->pose.position );
		hand->SetOrientation( pose->pose.orientation );
	}
	hand->Draw();
}

void GraspGLObjects::DrawProjectiles( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		projectiles->SetPosition( pose->pose.position );
		projectiles->SetOrientation( pose->pose.orientation );
	}
	projectiles->Draw();
}

void GraspGLObjects::DrawTiltPrompt(  TrackerPose *pose  ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		tiltPrompt->SetPosition( pose->pose.position );
		tiltPrompt->SetOrientation( pose->pose.orientation );
	}
	tiltPrompt->Draw();
}

void GraspGLObjects::DrawGlasses(  TrackerPose *pose  ) {
	if ( pose != nullptr ) {
		glasses->SetPosition( pose->pose.position );
		glasses->SetOrientation( pose->pose.orientation );
	}
	glasses->Draw();
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
	double vertices[4][2];

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

