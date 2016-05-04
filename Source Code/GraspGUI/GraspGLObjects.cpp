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
const char *GraspGLObjects::wall_texture_bitmap = "lime.bmp";
const char *GraspGLObjects::sky_texture_bitmap= "NightSky.bmp";
			
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

const Vector3 GraspGLObjects::target_location = { 0.0, 2500.0, 0.0 };
const double GraspGLObjects::target_ball_radius = 100.0;
const int GraspGLObjects::target_balls = 5;

const double GraspGLObjects::finger_length = 100.0;

void GraspGLObjects::CreateObjects( void ) {
	CreateTextures();
	sky = CreateSky();
	room = CreateRoom();
	target = CreateTarget();
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
		sphere->SetPosition( 0.0, 0.0 + (1000.0 - (-1000.0)) / 7.5 * trg, 0.0 );
		sphere->SetColor(( 75.0 - abs(trg) * 10.0 ) / 255.0, 0.0, 0.0);
		target->AddComponent( sphere );
	}
	target->SetPosition( 0.0, 0.0, -room_length / 2 + 50 );
	return target;

}

Assembly *GraspGLObjects::CreateTool( void ) {

	static double finger_spacing = target_ball_radius;
	Assembly *tool = new Assembly();
	// Create a set of 'fingers'. There are as many fingers as target balls.
	// The fingers have a smaller radius than the target balls. When viewed from
	// the subject's viewpoint they subtend a similar amount of visual arc.
	for ( int trg = - target_balls / 2; trg <= target_balls / 2; trg++ ){
		// Each finger is a 'capsule' composed of a cylinder that is capped on each end with a sphere.
		Assembly *finger = new Assembly();
		Sphere *sphere = new Sphere( target_ball_radius / 2.0 );
		sphere->SetPosition( 0.0, 0.0, 0.0 );
		finger->AddComponent( sphere );
		Cylinder *cylinder = new Cylinder( target_ball_radius / 2.0, target_ball_radius / 2.0, finger_length );
		cylinder->SetPosition( 0.0, 0.0, - finger_length / 2 );
		finger->AddComponent( cylinder );
		sphere = new Sphere( target_ball_radius / 2.0 );
		sphere->SetPosition( 0.0, 0.0, - finger_length );
		finger->AddComponent( sphere );
		// Create a color that varies as a function of the finger's position.
		float color[4] = { 0.5f, 0.30f + (float) trg * 0.1f, 0.0f, 1.0f };
		finger->SetColor( color );
		// Space the fingers vertically.
		finger->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		tool->AddComponent( finger );
	}
	return tool;

}

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
	head->SetPosition(  0.0, 90.0, 0.0 );
	head->SetOrientation( 0.0, 0.0, 0.0 );
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

void GraspGLObjects::DrawSky( void ) {
	sky->Draw();
}

void GraspGLObjects::DrawRoom( void ) {
	room->Draw();
}

void GraspGLObjects::DrawTarget( void ) {
	target->Draw();
}

void GraspGLObjects::DrawTool( TrackerPose *pose ) {
	tool->SetPosition( pose->position );
	tool->SetOrientation( pose->orientation );
	tool->Draw();
}

void GraspGLObjects::DrawHead( TrackerPose *pose ) {
	head->SetPosition( pose->position );
	head->SetOrientation( pose->orientation );
	head->Draw();
}

void GraspGLObjects::DrawTorso(  TrackerPose *pose  ) {
	torso->SetPosition( pose->position );
	torso->SetOrientation( pose->orientation );
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