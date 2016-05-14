// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

#include "../OpenGLObjects/OpenGLObjects.h"
#include "PsyPhyRendering.h"

extern OculusMapper oculusMapper;

using namespace PsyPhy;

// A texture that is used to decorate the walls of the room.
// This file has to be in the execution directory.
char *wall_texture_bitmap_filename = "Bmp\\Rockwall.bmp";

OculusViewpoint *viewpoint;
Assembly	*tool;
Assembly	*room;

void CreatePsyPhyObjects ( void ) {

	// Some local objects used to construct the Assemblies.
	Sphere	*sphere;
	Box		*box;
	Disk	*disk;
	Cylinder *cylinder;

	// Set up a default GL rendering context.
	glUsefulInitializeDefault();

	// Tone down the intensity a little bit from the default lighting.
	// Auto lighting is used to avoid giving a reference from directional lighting.
	// I don't fully understand the lighting thing, because when I set the intensity to 0
	//  one can still see the objects. But nevertheless this works to reduce the intensity somewhat.
	glUsefulAutoLighting( 0.0 );

	// Create a viewpoint into the scene, using default IPD, FOV and near/far culling.
	viewpoint = new OculusViewpoint( &oculusMapper );
		
	/// Create a room to put the object in.

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 5 meters wide by 10 meters high in the virtual scene.
	static Texture *wall_texture = new Texture( wall_texture_bitmap_filename, 5, 10 );
	room = new Assembly();

	// The room is just a box.
	box = new Box( 10.0, 10.0, 20.0 );
	box->SetColor( GRAY );
	box->SetTexture( wall_texture );
	room->AddComponent( box );

	// A disk on the front wall, something to look at.
	disk = new Disk( 4.0 );
	disk->SetPosition( 0.0, 0.0, 9.0 );
	disk->SetColor( ORANGE );
	room->AddComponent( disk );

	// The center of the room is at the origin.
	room->SetPosition( 0.0, 0.0, 0.0 );
	room->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a tool.

	tool = new Assembly();
	sphere = new Sphere( 0.5 );
	sphere->SetPosition( 0.0, 0.0, 0.0 );
	sphere->SetColor( RED );
	tool->AddComponent( sphere );
	cylinder = new Cylinder( 0.25, 0.25, 2.0 );
	cylinder->SetColor( BLUE );
	cylinder->SetOrientation( 90.0, cylinder->iVector );
	tool->AddComponent( cylinder );
	cylinder = new Cylinder( 0.25, 0.25, 2.0 );
	cylinder->SetColor( BLUE );
	cylinder->SetOrientation( -90.0, cylinder->jVector );
	tool->AddComponent( cylinder );

	tool->SetPosition( 0.0, 0.0, 5.0 );

}

void DrawPsyPhyObjects ( void ) {

	// Draw the room and the tool.
	glUsefulDefaultSpecularLighting( 0.7 );
	glUsefulMatteMaterial();
	room->Draw();
	glUsefulShinyMaterial();
	tool->Draw();

}
