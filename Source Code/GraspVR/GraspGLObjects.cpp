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
//const double GraspGLObjects::wall_thickness = 10.0;

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

	glEnable( GL_LIGHT0 );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	glEnable( GL_BLEND );

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
}


	laserPointer = CreateLaserPointer();
	disk_target = CreateDiskTarget();
	glasses = CreateGlasses();
void GraspGLObjects::CreateTextures( void ) {
	sky_texture = new Texture( sky_texture_bitmap, 2000, 2000 );
	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 1000, 2000 );
	references_texture = new Texture( references_texture_bitmap, 500, 500 );
}

Assembly *GraspGLObjects::CreateSky( void ) {
	Assembly *sky = new Assembly();
	Patch *patch = new Patch( room_radius * 2.2, room_radius * 2.2 );
	patch->SetTexture( sky_texture );
	sky->AddComponent( patch );
	sky->SetColor(WHITE);
	return( sky );
}

Assembly *GraspGLObjects::CreateDarkSky( void ) {
	Assembly *sky = new Assembly();
	Patch *patch = new Patch( room_radius * 2.2, room_radius * 2.2 );
	patch->SetTexture( sky_texture );
	sky->AddComponent( patch );
	sky->SetColor( BLACK );
	sky->enabled = false;
	return( sky );
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
		referenceBar->SetTexture( references_texture );
		room->AddComponent( referenceBar );
		referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius,  room_length );
		referenceBar->SetOffset( room_radius - reference_bar_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( - 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  (double) i / reference_bars, (double) i / reference_bars, (double) i / reference_bars, 1.0 );
		referenceBar->SetTexture( references_texture );
		room->AddComponent( referenceBar );
	}
	return room;
}

Assembly *GraspGLObjects::CreateTarget( void ) {

	Assembly *target = new Assembly();
	for (int trg = - target_balls ; trg <= target_balls ; trg++ ){
		Sphere *sphere = new Sphere( target_ball_radius );
		sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
		sphere->SetColor(( 255.0 - abs(trg) * 50.0 ) / 255.0, 0.0, 0.0);
		target->AddComponent( sphere );
	}
	return target;

}

Assembly *GraspGLObjects::CreateDiskTarget( void ) {

	Assembly *disk_target = new Assembly();
	
		Sphere *sphere = new Sphere( target_ball_radius );
		sphere->SetPosition( 0.0, 0.0, -room_length/2.0+10 );
		sphere->SetColor(1.0, 165.0/255.0, 0.0);
		disk_target->AddComponent( sphere );
	return disk_target;

}

Assembly *GraspGLObjects::CreateGlasses(void){
	double glass_width=100;
	double glass_height=60.0;
	double glass_thickness=5.0;

	Assembly *glasses = new Assembly();
	glasses->SetColor(WHITE);

	Slab *slab_u = new Slab(glass_width, glass_thickness, 0.1);
	slab_u->SetPosition(0.0, glass_height/2.0, -100.0);
	glasses->AddComponent(slab_u);

	Slab *slab_d = new Slab(glass_width, glass_thickness, 0.1);
	slab_d->SetPosition(0.0, -glass_height/2.0, -100.0);
	glasses->AddComponent(slab_d);

	Slab *slab_l = new Slab(glass_thickness, glass_height-(glass_thickness), 0.1);
	slab_l->SetPosition(-(glass_width/2.0-glass_thickness/2.0), 0.0, -100.0);
	glasses->AddComponent(slab_l);

	Slab *slab_r = new Slab(glass_thickness, glass_height-(glass_thickness), 0.1);
	slab_r->SetPosition((glass_width/2.0-glass_thickness/2.0), 0.0, -100.0);
	glasses->AddComponent(slab_r);

	return glasses;
}

void GraspGLObjects::ColorGlasses( double error ) {
	double epsilon=0.1/3.14*180; // it does not seem to work properly but I have no way to testing in an effective way
	if (error<epsilon*0.2){//GREEN
		glasses->SetColor(100.0/255.0, 255.0/255.0,  100.0/255.0, 0.5);
	}else{
		if (error>(2*epsilon)){//RED
			glasses->SetColor(200.0/255.0, 100.0/255.0,  100.0/255.0, 0.5);
		}else{
			if (error>epsilon){//Yellow->red
				glasses->SetColor(200.0/255.0, (100.0*(1-error-epsilon)/(2*epsilon-epsilon)+100.0)/255.0,  100.0/255.0, 0.5);
			}else{ //green->yellow
				glasses->SetColor((55.0*(error-epsilon*0.20)/(epsilon-epsilon*0.2)+145.0)/255.0, 200.0/255.0,  100.0/255.0, 0.5);
			}
		}
	}

}


Assembly *GraspGLObjects::CreateTool( void ) {

	Assembly *tool = new Assembly();

	// Create a set of 'fingers', each of which is a 'capsule' composed of a tube with rounded caps.

	// There are as many fingers as target balls. This could change.
	static int fingers = target_balls-1;

	// For some reason I set the spacing between fingers to be the same as the target ball radius.
	// I don't remember why. This could change.
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
		float color[4] = { 200.0f/255.0f, (75.0f + float(trg) * 75.0f/2.0)/255.0f , 0.0f, 1.0f };//(75.0f + (float) trg * 25.0f)/255.0f
		finger->SetColor( color );

		// Space the fingers vertically.
		finger->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		tool->AddComponent( finger );
	}
	return tool;

}

Assembly * GraspGLObjects::CreateLaserPointer( void ) {
	
	Assembly *laserPointer = new Assembly();

	Sphere *sphere = new Sphere( finger_ball_radius*2.0 );
	//sphere->SetColor( RED );
	sphere->SetPosition( 0.0, 0.0, -(room_length/2.0-1000.0) );
	laserPointer->AddComponent( sphere );

	return laserPointer;
}

void GraspGLObjects::ColorLaserPointer( double error ) {
	double epsilon=0.1/3.14*180; // it does not seem to work properly but I have no way to testing in an effective way
	if (error<epsilon*0.2){//GREEN
		laserPointer->SetColor(100.0/255.0, 255.0/255.0,  100.0/255.0, 1.0);
	}else{
		if (error>(2*epsilon)){//RED
			laserPointer->SetColor(200.0/255.0, 100.0/255.0,  100.0/255.0, 1.0);
		}else{
			if (error>epsilon){//Yellow->red
				laserPointer->SetColor(200.0/255.0, (100.0*(1-error-epsilon)/(2*epsilon-epsilon)+100.0)/255.0,  100.0/255.0, 1.0);
			}else{ //green->yellow
				laserPointer->SetColor((55.0*(error-epsilon*0.20)/(epsilon-epsilon*0.2)+145.0)/255.0, 200.0/255.0,  100.0/255.0, 1.0);
			}
		}
	}

}

Assembly *GraspGLObjects::CreateProjectiles( void ) {

	Assembly *projectiles = new Assembly();

	// Create a set of 'fingers', each of which is a 'capsule' composed of a tube with rounded caps.
	static int fingers = target_balls - 1;

	// For some reason I set the spacing between fingers to be the same as the target ball radius.
	// I don't remember why. This could change.
	static double finger_spacing = finger_ball_radius * 2;

	for ( int trg = - fingers; trg <= fingers ; trg++ ){

		Sphere *sphere = new Sphere( finger_ball_radius );
		// Create a color that varies as a function of the ball's position.
		float color[4] = { 200.0f/255.0f, (75.0f + float(trg) * 75.0f/2.0)/255.0f , 0.0f, 1.0f };//(75.0f + (float) trg * 25.0f)/255.0f
		sphere->SetColor( color );
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

	SetLighting();
	CreateTextures();

	room = CreateRoom();
	lightSky = CreateSky();
	darkSky = CreateDarkSky();
	// Only one sky should be enabled at a time, so we disable darkSky at this point.
	darkSky->Disable();
	// The skies get attached to the room so that they move (tilt) with it.
	// This means that you don't actually have to draw the skies explicitly.
	room->AddComponent( lightSky );
	room->AddComponent( darkSky );

	target = CreateTarget();
	tiltPrompt = CreateTiltPrompt();

	tool = CreateTool();
	projectiles = CreateProjectiles();
}

// Place objects at default locations.
void GraspGLObjects::PlaceVRObjects( void ) {
	lightSky->SetPosition( sky_location );
	darkSky->SetPosition( sky_location );
	target->SetPosition( target_location );
	tiltPrompt->SetPosition( prompt_location );
}


// Draw the objects that are used during VR rendering.
// Note that only those objects that are currently active are actually drawn.
void GraspGLObjects::DrawVR( void ) {

	glUsefulDefaultSpecularLighting( 0.7 );

	// Draw the room with non-shiny material.
	glUsefulMatteMaterial();

	// Because the skies are attached to the room, one need not draw them explicitly.
	// I leave these lines here in comments, though, to remind us that they will be drawn.
	// If we ever decide not to attach them to the room, these lines should be uncommented.
	// DrawLightSky();
	// DrawDarkSky();
	DrawRoom();

	// Draw the other objects with the hopes of seeing specular reflections. 
	// I am still trying to get specular reflections to work.
	// Someday, the material should be made part of the object.
	glUsefulShinyMaterial();

	DrawTarget();
	DrawTiltPrompt();
	DrawTool();
	DrawProjectiles();

}

// A set of routines that allows one to set the pose of the various objects and then draw them in 
//  a single operation. This can be convenient, but it goes against the concept of objects having 
//  a current, persistent state that includes its visibility and its pose. In many cases it will make
//  more sense to access the objects directly with obj->SetPosition(), obj->SetOrientation(), obj->Draw(), etc.


// Draw the sky, presumably always at the same position.
// For Grasp, it might make more sense if it is attached to the room so that they tilt together.
// If so, these routines need not be called explicitly. See comments above.
void GraspGLObjects::DrawLightSky( void ) {
	lightSky->Draw();
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

void GraspGLObjects::DrawTarget( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		target->SetPosition( pose->pose.position );
		target->SetOrientation( pose->pose.orientation );
	}
	target->Draw();
}

void GraspGLObjects::DrawTool( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		tool->SetPosition( pose->pose.position );
		tool->SetOrientation( pose->pose.orientation );
	}
	tool->Draw();
}

void GraspGLObjects::DrawLaserPointer( TrackerPose *pose ) {
	// If the caller has specified a pose, move to that pose first.
	// Otherwise, just draw it at it's current pose.
	if ( pose != nullptr ) {
		laserPointer->SetPosition( pose->position );
		laserPointer->SetOrientation( pose->orientation );
	}
	laserPointer->Draw();
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
		glasses->SetPosition( pose->position );
		glasses->SetOrientation( pose->orientation );
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
