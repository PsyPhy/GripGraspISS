// TestGraspGLObjects.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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

#include <../GraspVR/GraspGLObjects.h>

using namespace PsyPhy;
using namespace Grasp;

// These hold the OpenGLObjects that are shared across the local routines.
OpenGLWindow	*window;			// The window on the screen.
Viewpoint		*viewpoint;			// The viewpoint into the virtual world.
GraspGLObjects	*objects;


int _tmain(int argc, char *argv[])
{

	// Create and instance of a window object.
	// This does not yet create the actual window on the screen.
	window = new OpenGLWindow();

	// Create sets the new window to be the active window.
	if ( !window->Create( NULL, argv[0], 0, 0, 768, 512 ) ) {
		fMessageBox( MB_OK, "TestOpenGLObjects", "Error creating window." );
		exit( -1 );
	}  
	window->Activate();

	// Initialize the state of the GL graphics engine.
	glUsefulInitializeDefault();
	glUsefulDefaultSpecularLighting( 0.7 );

  /* 
	* Define a viewing projection with:
	*  45° vertical field-of-view - horizontal fov will be determined by window aspect ratio.
	*  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
	*      that I give for the model room here are in mm.
	*  100.0 to 10000.0  depth clipping planes - making this smaller would improve the depth resolution.
	*/
	viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);

	// Initial viewpoint is at the center of the room in the null orientation.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	objects = new GraspGLObjects();
	objects->CreateObjects();

	objects->tool->SetPosition( 0.0, -100.0, -1000.0 );
	objects->tool->SetOrientation( 40.0, 0.0, 0.0 );

	while ( true ) {

		// Draw everything.
		window->Clear( 0.0, 0.0, 0.0 );
		glUsefulPrepareRendering();

		viewpoint->Apply( window, CYCLOPS );
		objects->DrawSky();
		objects->DrawRoom();
		objects->DrawTarget();
		objects->DrawTiltPrompt();
		objects->DrawTool();

		window->Swap();

		if ( ! window->RunOnce() ) break;

	}

	return 0;
}

