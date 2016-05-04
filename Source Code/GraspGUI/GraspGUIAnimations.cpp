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
#include "GraspGLObjects.h"
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
OpenGLWindow *GraspDesktop::CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel, HGLRC shared_hRC ) {

	HWND	parent;
	OpenGLWindow *window;
	parent = static_cast<HWND>( panel->Handle.ToPointer() );
	window = new OpenGLWindow();
	window->Border = false;

	if ( !window->Create( parent, "HMD", 0, 0,panel->Width, panel->Height, shared_hRC ) ) {
		fMessageBox( MB_OK, "GraspGUI", "Error creating OpenGLWindow inside Forms Panel." );
		exit( -1 );
	}  
	return( window );
}

// Initialize the objects used to show the status on the screen.
void GraspDesktop::InitializeAnimations( void ) {

	// Dimensions of the room.
	double room_width = 1800.0;
	double room_height = 2000.0;
	double room_length = 8000.0;
	double wall_thickness = 10.0;

	coda_distance = 2500;
		
	// Create a window and viewpoint to show what the subject is seeing.
	hmdWindow = CreateOpenGLWindowInForm( hmdPanel);
	hmdWindow->Activate();
	hmdWindow->Clear( 0.50, 0.50, 0.60 );

 	hmdViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	hmdViewpoint->SetPosition( 0.0, 0.0, 0.0 );
	hmdViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a window and viewpoint to observe what is going on in the workspace.
	// This window shares the rendering context (hRC) from hmdWindow so that 
	// textures are defined in both.
	workspaceWindow = CreateOpenGLWindowInForm( columbusPanel, hmdWindow->hRC );
	workspaceWindow->Activate();
	workspaceWindow->Clear( 0.30, 0.50, 0.60 );
	
 	workspaceViewpoint = new Viewpoint( 6.0, 50.0, 10.0, 10000.0);
	workspaceViewpoint->SetPosition( 1950.0, 0.0, -1800.0 );
	workspaceViewpoint->SetOrientation( 0.0, 0.0, -90.0 );

	// Create windows to show the status of each of the tracked objects.
	hmdDynamicWindow = CreateOpenGLWindowInForm( hmdDynamicPanel, workspaceWindow->hRC );
	hmdStaticWindow = CreateOpenGLWindowInForm( hmdStaticPanel, workspaceWindow->hRC );
	toolDynamicWindow = CreateOpenGLWindowInForm( toolDynamicPanel, workspaceWindow->hRC );
	toolStaticWindow = CreateOpenGLWindowInForm( toolStaticPanel, workspaceWindow->hRC );
	torsoDynamicWindow = CreateOpenGLWindowInForm( torsoDynamicPanel, workspaceWindow->hRC );
	torsoStaticWindow = CreateOpenGLWindowInForm( torsoStaticPanel, workspaceWindow->hRC );
 	codaViewpoint = new Viewpoint( 6.0, 5.0, 10.0, 10000.0);
	codaViewpoint->SetPosition( 0.0, 0.0, - coda_distance );
	codaViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

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

	side_room = new Assembly();
	side_room->SetColor( WHITE );

	box = new Box( room_height, room_width, room_length );
	box->SetColor( GRAY );
	box->SetTexture( wall_texture );
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

	Cylinder *cylinder = new Cylinder( 10.0, 30.0, 160.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetOrientation( 0.0, 90.0, 0.0 );
	cylinder->SetColor( YELLOW );
	object->AddComponent( cylinder );

	cylinder = new Cylinder( 10.0, 10.0, 80.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetOrientation( 0.0, - 90.0, 0.0 );
	cylinder->SetColor( GREEN );
	object->AddComponent( cylinder );

	// Initialize the position of the mobile object.
	object->SetAttitude( 0.0, 90.0, 0.0 );
	object->SetPosition(  0.0, 0.0, 650.0 );
	object->SetOrientation( 0.0, 0.0, 0.0 );

	head = new Assembly();
	sphere = new Sphere( 100.0 );
	sphere->SetColor( .4f, 0.0f, .4f );
	head->AddComponent( sphere );
	sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( -50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( 50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	cylinder = new Cylinder( 20.0, 5.0, 30.0 );	
	cylinder->SetPosition( 0.0, -20.0, -100.0 );
	cylinder->SetOrientation( 0.0, 90.0, 0.0 );
	cylinder->SetColor( YELLOW );
	head->AddComponent( cylinder );
	head->SetPosition(  0.0, 0.0, 0.0 );
	head->SetOrientation( 0.0, 0.0, 0.0 );

	torso = new Assembly();
	slab = new Slab( 160.0, 200.0, 40.0 );
	slab->SetColor( 0.1f, 0.4f, 0.0f );
	torso->AddComponent( slab );
	disk = new Disk( 50.0 );
	disk->SetPosition( 0.0, 0.0, -40.0 );
	disk->SetColor( 1.0f, 0.7f, 0.0f );
	torso->AddComponent( disk );

}

// Draw the objects used to show the status on the screen.
void GraspDesktop::RefreshAnimations( void ) {
		
	// Create an oscillating angle.
	static double pseudo_time = 0.0;
	double angle = 45.0 * cos( pseudo_time );
	pseudo_time += 0.05;

	// Make the object move as if the hand is moving it.
	object->SetOrientation( angle, object->kVector );
	object->SetPosition( 0.0, 0.0, -750.0 );
	head->SetOrientation( angle, object->kVector );
	torso->SetPosition( 0.0, -220.0, 0.0 );

	// Simulate movements of the head and show what the subject is seeing.
	hmdViewpoint->SetOrientation( 0.0, 0.0, 0.1 * angle );
	head->SetOrientation( 0.0, 0.0, 0.1 * angle );
	hmdWindow->Activate();
	hmdWindow->Clear();
	glUsefulPrepareRendering();
	hmdViewpoint->Apply( hmdWindow, CYCLOPS );
	objectRenderer->DrawSky();
	objectRenderer->DrawRoom();
	objectRenderer->DrawTarget();
	objectRenderer->DrawTool();
	hmdWindow->Swap();

	// Show what is going on from a fixed viewpoint into the 3D workspace.
	workspaceWindow->Activate();
	workspaceWindow->Clear( 0.0, 0.0, 0.0 );
	glUsefulPrepareRendering();
	workspaceViewpoint->Apply( workspaceWindow, CYCLOPS );
	// side_room->Draw();
	objectRenderer->DrawSky();
	objectRenderer->DrawRoom();
	objectRenderer->DrawTool();
	objectRenderer->DrawTarget();
	head->Draw();
	torso->Draw();
	workspaceWindow->Swap();

	hmdDynamicWindow->Activate();
	hmdDynamicWindow->Clear( 0.8, 1.0, 1.0 );
	codaViewpoint->Apply( hmdDynamicWindow, CYCLOPS );
	head->Draw();
	hmdDynamicWindow->Swap();

	toolDynamicWindow->Activate();
	toolDynamicWindow->Clear( 0.8, 1.0, 1.0 );
	codaViewpoint->Apply( toolDynamicWindow, CYCLOPS );
	object->Draw();
	toolDynamicWindow->Swap();

	torsoDynamicWindow->Activate();
	torsoDynamicWindow->Clear( 0.8, 1.0, 1.0 );
	codaViewpoint->Apply( torsoDynamicWindow, CYCLOPS );
	torso->SetPosition( 0.0, 0.0, 0.0 );
	torso->Draw();
	torsoDynamicWindow->Swap();

	hmdStaticWindow->Activate();
	hmdStaticWindow->Clear(1.0, 0.8, 1.0 );
	codaViewpoint->Apply( hmdStaticWindow, CYCLOPS );
	head->SetOrientation( 0.0, 0.0, 0.0 );
	head->SetPosition( 0.0, 0.0, 0.0 );
	head->Draw();
	hmdStaticWindow->Swap();

	toolStaticWindow->Activate();
	toolStaticWindow->Clear(1.0, 0.8, 1.0 );
	codaViewpoint->Apply( toolStaticWindow, CYCLOPS );
	object->SetPosition( 0.0, 0.0, 0.0 );
	object->SetOrientation( 0.0, 0.0, 0.0 );
	object->Draw();
	toolStaticWindow->Swap();

	torsoStaticWindow->Activate();
	torsoStaticWindow->Clear(1.0, 0.8, 1.0 );
	codaViewpoint->Apply( torsoStaticWindow, CYCLOPS );
	torso->SetOrientation( 0.0, 0.0, 0.0 );
	torso->Draw();
	torsoStaticWindow->Swap();


}

void GraspDesktop::KillAnimations( void ) {
	hmdWindow->Destroy();
}
