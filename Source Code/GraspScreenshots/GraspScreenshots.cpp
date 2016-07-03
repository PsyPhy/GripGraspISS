// GraspScreenshots.cpp 
//

// A program to generate still images of the GRASP VR environment and write them to a file.
// This is used primarily to generate documentation (i.e. a document for screen review).

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <math.h>
#include <time.h> 

#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "../Useful/OpenGLUseful.h"
#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

#include "../GraspVR/GraspGLObjects.h"

using namespace PsyPhy;
using namespace Grasp;

// These hold the OpenGLObjects that are shared across the local routines.
OpenGLWindow	*window;			// The window on the screen.
Viewpoint		*viewpoint;			// The viewpoint into the virtual world.
GraspGLObjects	*objects;

void DrawToBMP( const char *filename ) {

	// Prepare for drawing.
	window->Activate();
	window->Clear();

	glUsefulPrepareRendering();
	viewpoint->Apply( window, CYCLOPS );

	// Draw everything that is enabled.
	objects->DrawVR();
	objects->hmdStructure->Draw();
	objects->handStructure->Draw();
	objects->chestStructure->Draw();

	window->Swap();
	window->SaveAsBMP( filename );
	
}

int _tmain(int argc, char *argv[])
{

	// Should have at least one command line argument.
	if ( argc < 2 ) {
		fOutputDebugString( "GraspScreenshots: At least one command line argument is required.\n" );
		fprintf( stderr, "GraspScreenshots: At least one command line argument is required.\n" );
		exit( -1 );
	}
	// Create and instance of a window object.
	// This does not yet create the actual window on the screen.
	window = new OpenGLWindow();

	// Create sets the new window to be the active window.
	if ( !window->Create( NULL, argv[0], 860, 0, 1024, 1024 ) ) {
		fMessageBox( MB_OK, "TestOpenGLObjects", "Error creating window." );
		exit( -1 );
	}  
	window->Activate();

   /* 
	* Define a viewing projection with:
	*  45° vertical field-of-view - horizontal fov will be determined by window aspect ratio.
	*  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
	*      that I give for the model room here are in mm.
	*  100.0 to 10000.0  depth clipping planes - making this smaller would improve the depth resolution.
	*/
	viewpoint = new Viewpoint( 6.0, 75.0, 10.0, 10000.0);

	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation(0.0, 0.0, 0.0 );

	objects = new GraspGLObjects();
	objects->CreateVRObjects();
	objects->PlaceVRObjects();
	objects->CreateAuxiliaryObjects();

	objects->hand->SetPosition( 25.0, -25.0, -200.0 );
	objects->hand->SetOrientation( 0.0, -1.0, -1.0 );
		
	objects->tiltPrompt->SetOrientation(110.0, 0.0, 0.0);
	objects->response->SetOrientation( 33.0, 0.0, 0.0 );
	objects->orientationTarget->SetOrientation( 30.0, 0.0, 0.0 );

	objects->tunnel->SetColor( GRAY );
	objects->glasses->SetColor( 0.0f, 0.85f, 0.85f, 0.5f );

	objects->chestStructure->SetPosition( 0.0, -200.0, -500.0 );
	objects->handStructure->SetPosition( 0.0, 0.0, -500.0 );
	objects->hmdStructure->SetPosition( 0.0, 200.0, -500.0 );

	// Initialize state of the objects.
	objects->starrySky->Enable();
	objects->darkSky->Disable();
	objects->room->Enable();

	objects->orientationTarget->Disable();
	objects->positionOnlyTarget->Disable();
	objects->response->Disable();
	objects->tiltPrompt->Disable();
	objects->vTool->Disable();
	objects->vkTool->Disable();
	objects->kTool->Disable();
	objects->kkTool->Disable();
	objects->projectiles->Disable();
	objects->glasses->Enable();
	objects->successIndicator->Disable();
	objects->timeoutIndicator->Disable();
	objects->headMisalignIndicator->Disable();
	objects->readyToStartIndicator->Disable();
	objects->blockCompletedIndicator->Disable();
	objects->wristZone->Disable();
		
	objects->hmdStructure->Disable();
	objects->handStructure->Disable();
	objects->chestStructure->Disable();

	for ( int arg = 2; arg < argc; arg++ ) {

		if ( !strcmp( argv[1], "ReadyToStart" )) {
			objects->vTool->Disable();
			objects->readyToStartIndicator->Enable();
		}
		else if ( !strcmp( argv[1], "BlockCompleted" )) {
			objects->timeoutIndicator->Disable();
			objects->blockCompletedIndicator->Enable();
			DrawToBMP( "BlockCompletedScreenShot.bmp" );
		}
		else if ( !strcmp( argv[1], "HeadMisalign" )) {
			objects->readyToStartIndicator->Disable();
			objects->headMisalignIndicator->Enable();
			DrawToBMP( "HeadMisalignScreenShot.bmp" );
		}
		else if ( !strcmp( argv[1], "Timeout" )) {
			objects->headMisalignIndicator->Disable();
			objects->timeoutIndicator->Enable();
			DrawToBMP( "TimeoutScreenShot.bmp" );
		}
		else {
			fOutputDebugString( "GraspScreenshots: Unrecognized argument (%s).\n", argv[1] );
			fprintf( stderr, "GraspScreenshots: Unrecognized argument (%s).\n", argv[1] );
			exit( -2 );
		}
	return 0;
}

