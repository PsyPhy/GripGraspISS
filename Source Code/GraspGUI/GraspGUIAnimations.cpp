///
/// Module:	GraspGUI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#include "stdafx.h"

#include "GraspDesktopForm.h"

using namespace GraspGUI;
using namespace PsyPhy;

void GraspDesktop::OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
	RefreshAnimations();
}
void GraspDesktop::CreateRefreshTimer( int interval ) {
	refreshTimer = gcnew( System::Windows::Forms::Timer );
	refreshTimer->Interval = interval;
	refreshTimer->Tick += gcnew EventHandler( this, &GraspGUI::GraspDesktop::OnTimerElapsed );
}
void GraspDesktop::StartRefreshTimer( void ) {
	refreshTimer->Start();
}
void GraspDesktop::StopRefreshTimer( void ) {
	refreshTimer->Stop();
}

// Create an OpenGLObjects window that is embedded in a Windows::Form.
// To use, create a panel in the form and then pass the reference to that panel to this routine.
// This is actually a generally useful routine and should probably be included in the 
// OpenGLObjects / OpenGLWindows package, rather than here.
OpenGLWindow *GraspDesktop::CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel ) {

	HWND	parent;
	OpenGLWindow *window;
	parent = static_cast<HWND>( panel->Handle.ToPointer() );
	window = new OpenGLWindow();
	window->Border = false;

	if ( !window->Create( parent, "HMD", 0, 0,panel->Width, panel->Height ) ) {
		fMessageBox( MB_OK, "GraspGUI", "Error creating OpenGLWindow inside Forms Panel." );
		exit( -1 );
	}  
	return( window );
}

// Initialize the objects used to show the status on the screen.
void GraspDesktop::InitializeAnimations( void ) {

	hmdWindow = CreateOpenGLWindowInForm( hmdPanel);
	hmdWindow->Activate();
	hmdWindow->Clear( 0.50, 0.50, 0.60 );
		
	// Create a viewpoint in the center of the virtual room.
 	hmdViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	hmdViewpoint->SetPosition( 0.0, 0.0, 0.0 );
	hmdViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	workspaceWindow = CreateOpenGLWindowInForm( columbusPanel);
	workspaceWindow->Activate();
	workspaceWindow->Clear( 0.30, 0.50, 0.60 );
	
	// Create a fixed viewpoint into the 3D workspace.
 	workspaceViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	workspaceViewpoint->SetPosition( 1000.0, 0.0, -500.0 );
	workspaceViewpoint->SetOrientation( 0.0, 0.0, -90.0 );

	// Initialize the state of the GL graphics engine.
	glUsefulInitializeDefault();

	// Local objects used to construct the OpenGLObject assemblies.
	Sphere *sphere;
	Slab *slab;
	Box *box;
	Disk *disk;
	
	// A texture that is used to decorate the walls of the room.
	// This file has to be in the execution directory.
	char *wall_texture_bitmap = "lime.bmp";

	// Dimensions of the room.
	double room_width = 1800.0;
	double room_height = 2000.0;
	double room_length = 8000.0;
	double wall_thickness = 10.0;

	// Create a room to put the object in.

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	hmdWindow->Activate();
	wall_texture = new Texture( wall_texture_bitmap, 2000, 4000 );

	room = new Assembly();
	room->SetColor( WHITE );

	box = new Box( room_height, room_width, room_length );
	box->SetColor( GRAY );
	box->SetTexture( wall_texture );
	room->AddComponent( box );

	// A disk on the front wall, something to look at.
	disk = new Disk( 750.0 );
	disk->SetPosition( 0.0, 0.0, - room_length / 2.0 + wall_thickness );
	disk->SetColor( ORANGE );
	room->AddComponent( disk );

	// The center of the room is at the origin.
	room->SetPosition( 0.0, 0.0, 0.0 );
	room->SetOrientation( 0.0, 0.0, 0.0 );

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	char *side_texture_bitmap = "kLime.bmp";
	workspaceWindow->Activate();
	side_texture = new Texture( side_texture_bitmap, 2000, 4000 );

	side_room = new Assembly();
	side_room->SetColor( WHITE );

	box = new Box( room_height, room_width, room_length );
	box->SetColor( GRAY );
	box->SetTexture( side_texture );
	side_room->AddComponent( box );

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

	Cylinder *cylinder = new Cylinder( 10.0, 30.0, 60.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetColor( YELLOW );
	object->AddComponent( cylinder );

	cylinder = new Cylinder( 10.0, 10.0, 80.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetOrientation( 0.0, - 90.0, 0.0 );
	cylinder->SetColor( GREEN );
	object->AddComponent( cylinder );

	// Initialize the position of the mobile object.
	object->SetPosition(  0.0, 0.0, 750.0 );
	object->SetOrientation( 0.0, 0.0, 0.0 );

	CreateRefreshTimer( 20 );
	StartRefreshTimer();
	RefreshAnimations();
}

// Draw the objects used to show the status on the screen.
void GraspDesktop::RefreshAnimations( void ) {
		
	// Create an oscillating angle.
	static double pseudo_time = 0.0;
	double angle = 45.0 * cos( pseudo_time );
	pseudo_time += 0.05;

	// Make the object move as if the hand is moving it.
	object->SetOrientation( angle, object->jVector );
	object->SetPosition( 0.0, 0.0, -750.0 );

	// Simulate movements of the head and show what the subject is seeing.
	hmdViewpoint->SetOrientation( 0.0, 0.0, 0.1 * angle );
	hmdWindow->Activate();
	hmdWindow->Clear();
	glUsefulPrepareRendering();
	hmdViewpoint->Apply( hmdWindow, CYCLOPS );
	room->Draw();
	object->Draw();
	hmdWindow->Swap();

	// Show what is going on from a fixed viewpoint into the 3D workspace.
	workspaceWindow->Activate();
	workspaceWindow->Clear();
	glUsefulPrepareRendering();
	workspaceViewpoint->Apply( workspaceWindow, CYCLOPS );
	side_room->Draw();
	object->Draw();
	workspaceWindow->Swap();


}

void GraspDesktop::KillAnimations( void ) {
	hmdWindow->Destroy();
}
