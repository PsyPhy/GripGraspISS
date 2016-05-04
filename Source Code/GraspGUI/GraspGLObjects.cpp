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

void GraspGLObjects::CreateObjects( void ) {
	CreateTextures();
	sky = CreateSky();
	room = CreateRoom();
	target = CreateTarget();
	tool = CreateTool();
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
	for (int trg = -3; trg <= 3; trg++ ){
		Sphere *sphere = new Sphere(100);
		sphere->SetPosition( 0.0, 0.0 + (1000.0 - (-1000.0)) / 7.5 * trg, 0.0 );
		sphere->SetColor(( 75.0 - abs(trg) * 10.0 ) / 255.0, 0.0, 0.0);
		target->AddComponent( sphere );
	}
	target->SetPosition( 0.0, 0.0, -room_length / 2 + 50 );
	return target;

}

Assembly *GraspGLObjects::CreateTool( void ) {

	Assembly *tool = new Assembly();
	for (int trg=-2; trg<=2; trg++){
		Sphere *sphere = new Sphere(50);
		sphere->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,0.0);
		sphere->SetColor(100.0/255.0,(75.0+trg*25.0)/255.0,0);
		tool->AddComponent(sphere);
		Cylinder *cylinder = new Cylinder(50,50,200);
		cylinder->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,-200.0/2);
		tool->AddComponent(cylinder);
		sphere = new Sphere(50);
		sphere->SetPosition(0.0,0.0+(1000.0-(-1000.0))/20*trg,-200.0);
		tool->AddComponent(sphere);
	}
	tool->SetPosition(0.0,0.0, -500);
	return tool;

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

void GraspGLObjects::DrawTool( void ) {
	tool->Draw();
}
