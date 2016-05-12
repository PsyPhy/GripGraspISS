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
const char *GraspGLObjects::wall_texture_bitmap = "Bmp\\lime.bmp";
const char *GraspGLObjects::sky_texture_bitmap= "Bmp\\NightSky.bmp";
			
// Dimensions of the room.
const double GraspGLObjects::room_radius = 2000.0;
const double GraspGLObjects::room_length = 6000.0;
const double GraspGLObjects::wall_thickness = 10.0;
const double GraspGLObjects::reference_bars = 5;
const double GraspGLObjects::reference_bar_radius = 50.0;

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
const double GraspGLObjects::target_ball_spacing = 2000.0 / 7.5;
const int GraspGLObjects::target_balls = 5;
const Vector3 GraspGLObjects::target_location = { 0.0, 0.0, -3500.0 };

const double GraspGLObjects::finger_length = 100.0;

// Set up the lighting and material properties.
void GraspGLObjects::SetLighting( void ) {
	
	GLfloat fintensity = 0.75;
	GLfloat ambient = fintensity;
	GLfloat diffuse = fintensity;
	GLfloat specular = fintensity / 2.0f;

	// Light definition
	GLfloat LightPosition[] = { 0.0, 0.0, 1000.0, 0.0 };
	GLfloat LightAmbient[]  = { ambient, ambient, ambient, 1.0};
	GLfloat LightDiffuse[]  = { diffuse, diffuse, diffuse, 1.0};
	GLfloat LightSpecular[] = { specular, specular, specular, 1.0};

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
void GraspGLObjects::CreateObjects( void ) {

	CreateTextures();
	sky = CreateSky();
	room = CreateRoom();

	target = CreateTarget();
	target->SetPosition( target_location );
	tiltPrompt = CreateTiltPrompt();
	tiltPrompt->SetPosition( prompt_location );

	tool = CreateTool();
	head = CreateHead();
	torso = CreateTorso();
}
void GraspGLObjects::CreateTextures( void ) {
	sky_texture = new Texture( sky_texture_bitmap, 2000, 2000 );
	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 2000, 4000 );
}

Assembly *GraspGLObjects::CreateSky( void ) {
	Assembly *sky = new Assembly();
	Box *box = new Box( room_radius * 2.2, room_radius * 2.2, room_length * 2.2 );
	box->SetTexture( sky_texture );
	sky->AddComponent( box );
	return( sky );
}

Assembly *GraspGLObjects::CreateRoom( void ) {

	Assembly *room = new Assembly();
	room->SetColor( WHITE );

	//Tunnel Plus references
	Cylinder *tunnel = new Cylinder( room_radius, room_radius, 2.0 * room_length );
	tunnel->SetColor( WHITE );
	tunnel->SetTexture( wall_texture );
	tunnel->SetOrientation( 90.0, 0.0, 0.0 );
	room->AddComponent( tunnel );

	for (int i=0; i < reference_bars; i++ ){ 
		Cylinder *referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, 2.0 * room_length );
		referenceBar->SetOffset( room_radius- reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  1.0 - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0 );
		referenceBar->SetTexture( wall_texture );
		room->AddComponent( referenceBar );
		referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, 2.0 * room_length );
		referenceBar->SetOffset( room_radius - reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( - 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  (double) i / reference_bars, (double) i / reference_bars, (double) i / reference_bars, 1.0 );
		referenceBar->SetTexture( wall_texture );
		room->AddComponent( referenceBar );
	}
	return room;
}

Assembly *GraspGLObjects::CreateTarget( void ) {

	Assembly *target = new Assembly();
	for (int trg = - target_balls / 2; trg <= target_balls / 2; trg++ ){
		Sphere *sphere = new Sphere( target_ball_radius );
		sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
		sphere->SetColor(( 75.0 - abs(trg) * 10.0 ) / 255.0, 0.0, 0.0);
		target->AddComponent( sphere );
	}
	return target;

}

Assembly *GraspGLObjects::CreateTool( void ) {

	Assembly *tool = new Assembly();

	// Create a set of 'fingers', each of which is a 'capsule' composed of a tube with rounded caps.

	// There are as many fingers as target balls. This could change.
	static int fingers = target_balls;

	// The fingers have a smaller radius than the target balls. When viewed from
	// the subject's viewpoint they should subtend a similar amount of visual arc.
	static double finger_radius = target_ball_radius / 2.0;

	// For some reason I set the spacing between fingers to be the same as the target ball radius.
	// I don't remember why. This could change.
	static double finger_spacing = target_ball_radius;

	for ( int trg = - target_balls / 2; trg <= fingers / 2; trg++ ){

		// Each finger is a 'capsule' composed of a cylinder that is capped on each end with a sphere.
		Assembly *finger = new Assembly();
		Sphere *sphere = new Sphere( finger_radius );
		sphere->SetPosition( 0.0, 0.0, 0.0 );
		finger->AddComponent( sphere );
		Cylinder *cylinder = new Cylinder( finger_radius, finger_radius, finger_length );
		cylinder->SetPosition( 0.0, 0.0, - finger_length / 2 );
		finger->AddComponent( cylinder );
		sphere = new Sphere( finger_radius );
		sphere->SetPosition( 0.0, 0.0, - finger_length );
		finger->AddComponent( sphere );

		// Create a color that varies as a function of the finger's position and apply it to the entire finger.
		float color[4] = { 0.5f, 0.30f + (float) trg * 0.1f, 0.0f, 1.0f };
		finger->SetColor( color );

		// Space the fingers vertically.
		finger->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		tool->AddComponent( finger );
	}
	return tool;

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

// A set of routines that allows one to set the pose of the various objects and then draw them in 
//  a single operation. This can be convenient, but it goes against the concept of objects having 
//  a current, persistent state that includes its visibility and its pose. In many cases it will make
//  more sense to access the objects directly with obj->SetPosition(), obj->SetOrientation(), obj->Draw(), etc.

// Draw the sky, presumably alwas at the same position.
// For Grasp, it might make more sense if it was attached to the room so that they tilt together.
void GraspGLObjects::DrawSky( void ) {
	sky->Draw();
}

void GraspGLObjects::DrawRoom( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		room->SetPosition( pose->position );
		room->SetOrientation( pose->orientation );
	}
	room->Draw();
}

void GraspGLObjects::DrawTarget( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		target->SetPosition( pose->position );
		target->SetOrientation( pose->orientation );
	}
	target->Draw();
}

void GraspGLObjects::DrawTool( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		tool->SetPosition( pose->position );
		tool->SetOrientation( pose->orientation );
	}
	tool->Draw();
}

void GraspGLObjects::DrawTiltPrompt(  TrackerPose *pose  ) {
	if ( pose != nullptr ) {
		tiltPrompt->SetPosition( pose->position );
		tiltPrompt->SetOrientation( pose->orientation );
	}
	tiltPrompt->Draw();
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

void GraspGLObjects::DrawHead( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		head->SetPosition( pose->position );
		head->SetOrientation( pose->orientation );
	}
	head->Draw();
}

void GraspGLObjects::DrawTorso(  TrackerPose *pose  ) {
	if ( pose != nullptr ) {
		torso->SetPosition( pose->position );
		torso->SetOrientation( pose->orientation );
	}
	torso->Draw();
}

void GraspGLObjects::DrawBody( TrackerPose *pose ) {
	Vector3 torso_position;
	CopyVector( torso_position, pose->position );
	torso_position[Y] -= (head_shape[Y] + torso_shape[Y] / 2.0);
	torso->SetPosition( torso_position );
	torso->SetOrientation( nullQuaternion );
	torso->Draw();
	DrawHead( pose );
}