// GraspScreenshots.cpp 
//

// A program to generate still images of the GRASP VR environment and write them to a file.
// This is used primarily to generate documentation (i.e. a document for screen review).

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

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
	bool confirm = false;
	char *filename = "GraspScreenshot.bmp";
	int  size = 1024;

	// Parse command line for arguments affecting the window.
	for ( int arg = 1; arg < argc; arg++ ) {
		if ( 1 == sscanf( argv[arg], "--size=%d", &size ) ) fOutputDebugString( "Window size: %d x %d\n", size, size );
	}
	
	// Create and instance of a window object.
	// This does not yet create the actual window on the screen.
	window = new OpenGLWindow();

	// Create sets the new window to be the active window.
	if ( !window->Create( NULL, argv[0], 860, 0, size, size ) ) {
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

	for ( int arg = 1; arg < argc; arg++ ) {

		double angle;

		if ( 1 == sscanf( argv[arg], "--head=%lf", &angle ) ) {
			viewpoint->SetOrientation( angle, 0.0, 0.0 );
		}
		else if ( 1 == sscanf( argv[arg], "--headError=%lf", &angle ) ) {
			objects->SetColorByRollError( objects->glasses, angle, 2.0 );
		}

		else if ( 1 == sscanf( argv[arg], "--target=%lf", &angle ) ) {
			objects->orientationTarget->SetOrientation( angle, 0.0, 0.0 );
			objects->orientationTarget->Enable();
		}
		else if ( 1 == sscanf( argv[arg], "--handError=%lf", &angle ) ) {
			objects->SetColorByRollError( objects->kkTool, angle, 2.0 );
		}


		else if ( 1 == sscanf( argv[arg], "--hand=%lf", &angle ) ) {
			objects->hand->SetOrientation( angle, 0.0, 0.0 );
		}
		else if ( !strcmp( argv[arg], "--vkTool" )) {
			objects->vkTool->Enable();
		}
		else if ( !strcmp( argv[arg], "--kTool" )) {
			objects->kTool->Enable();
		}
		else if ( !strcmp( argv[arg], "--vTool" )) {
			objects->vTool->Enable();
		}
		else if ( !strcmp( argv[arg], "--kkTool" )) {
			objects->kkTool->Enable();
		}
		else if ( !strcmp( argv[arg], "--handArrow" )) {
			objects->tiltPrompt->SetPosition( objects->hand->position );
			objects->tiltPrompt->SetOrientation( objects->hand->orientation );
			objects->tiltPrompt->Enable();
		}
		else if ( !strcmp( argv[arg], "--headArrowLeft" )) {
			objects->tiltPrompt->SetPosition( objects->hud->position );
			objects->tiltPrompt->SetOffset( 0.0, 0.0, -250.0 );
			objects->tiltPrompt->SetOrientation( objects->hand->orientation );
			objects->tiltPrompt->Enable();
		}
		else if ( !strcmp( argv[arg], "--headArrowRight" )) {
			objects->tiltPrompt->SetPosition( objects->hud->position );
			objects->tiltPrompt->SetOffset( 0.0, 0.0, -250.0 );
			objects->tiltPrompt->SetOrientation( objects->hand->orientation );
			objects->tiltPrompt->SetAttitude( 0.0, 0.0, 180.0 );
			objects->tiltPrompt->Enable();
		}

		else if ( !strcmp( argv[arg], "--ReadyToStart" )) {
			objects->readyToStartIndicator->Enable();
		}

		else if ( !strcmp( argv[arg], "--BlockCompleted" )) {
			objects->blockCompletedIndicator->Enable();
		}

		else if ( !strcmp( argv[arg], "--TrialCompleted" )) {
			objects->successIndicator->Enable();
		}

		else if ( !strcmp( argv[arg], "--Misalign" )) {
			objects->headMisalignIndicator->Enable();
		}

		else if ( !strcmp( argv[arg], "--Timeout" )) {
			objects->timeoutIndicator->Enable();
		}

		else if ( !strcmp( argv[arg], "--confirm" )) {
			confirm = true;
		}
		else if ( 1 == sscanf( argv[arg], "--size=%d", &size ) ); // Ignore this parameter. It was handled earlier.
		else filename = argv[arg];

	}
	DrawToBMP( filename );

	if ( confirm ) fMessageBox( MB_OK, "GraspScreenshots", "Screenshot saved to %s.", filename );

	return 0;
}

