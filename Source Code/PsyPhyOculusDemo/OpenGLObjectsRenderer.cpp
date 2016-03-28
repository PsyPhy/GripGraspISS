// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

#include "../OpenGLObjects/OpenGLObjects.h"
#include "OpenGLObjectsRenderer.h"

using namespace PsyPhy;

// A texture that is used to decorate the walls of the room.
// This file has to be in the execution directory.
char *wall_texture_bitmap_filename = "lime.bmp";

OculusViewpoint *viewpoint;
Assembly	*tool;
Assembly	*room;

void CreateObjects ( void ) {

	// Some local objects used to construct the Assemblies.
	Sphere	*sphere;
	Box		*box;
	Disk	*disk;
	Cylinder *cylinder;

	// Set up a default GL rendering context.
	glUsefulInitializeDefault();

	// Create a viewpoint into the scene, using default IPD, FOV and near/far culling.
	viewpoint = new OculusViewpoint();
		
	/// Create a room to put the object in.

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 5 meters wide by 10 meters high in the virtual scene.
	static Texture *wall_texture = new Texture( wall_texture_bitmap_filename, 5, 10 );
	room = new Assembly();
	room->SetColor( WHITE );

	// The room is just a box.
	box = new Box( 10.0, 10.0, 20.0 );
	box->SetColor( GRAY );
	box->SetTexture( wall_texture );
	room->AddComponent( box );

	// A disk on the front wall, something to look at.
	disk = new Disk( 1 );
	disk->SetPosition( 0.0, 0.0, 9.0 );
	disk->SetColor( ORANGE );
	room->AddComponent( disk );

	// The center of the room is at the origin.
	room->SetPosition( 0.0, 0.0, 0.0 );
	room->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a tool.

	tool = new Assembly();
	sphere = new Sphere( 0.05 );
	sphere->SetPosition( 0.0, 0.0, 0.0 );
	sphere->SetColor( RED );
	tool->AddComponent( sphere );
	cylinder = new Cylinder( 0.025, 0.025, 0.2 );
	cylinder->SetColor( BLUE );
	cylinder->SetOrientation( 90.0, cylinder->iVector );
	tool->AddComponent( cylinder );
	cylinder = new Cylinder( 0.025, 0.025, 0.2 );
	cylinder->SetColor( BLUE );
	cylinder->SetOrientation( -90.0, cylinder->jVector );
	tool->AddComponent( cylinder );

	tool->SetPosition( 0.0, 0.0, 1.0 );

}

void DrawObjects ( int eye ) {

	// Prepare the GL graphics state for drawing in a way that is compatible 
	//  with OpenGLObjects. I am doing this each time we call DrawObjects in 
	//  case other GL stuff is going on elsewhere. Otherwise, we could probably
	//  do this just once at the beginning, e.g. in CreateObjects.
	glUsefulPrepareRendering();

	// Tone down the intensity a little bit from the default lighting.
	// Auto lighting is used to avoid giving a reference from directional lighting.
	// I don't fully understand the lighting thing, because when I set the intensity to 0
	//  one can still see the objects. But nevertheless this works to reduce the intensity somewhat.
	glUsefulAutoLighting( 0.0 );

	// Set up the viewing transformations.
	viewpoint->Apply( eye );

	// Draw the room and the tool.
	room->Draw();
	tool->Draw();

}
