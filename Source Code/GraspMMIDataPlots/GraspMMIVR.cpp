///
/// Module:	GraspMMI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GripMMI
///
/// Copyright (c) 2014, 2015 PsyPhy Consulting
///

/// Methods for drawing the various graphs on the screen.

#include "stdafx.h"

#include "..\Useful\Useful.h"
#include "..\Useful\fOutputDebugString.h"
#include "..\Useful\fMessageBox.h"

#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"
#include "../GraspVR/GraspGLObjects.h"

#include "GraspMMIGraphsForm.h"

using namespace GraspMMI;


void GraspMMIGraphsForm::InitializeVR( void ) {

	static VectorsMixin vm;

	Vector3	unitOffset[MAX_UNITS];
	Matrix3x3 unitRotation[MAX_UNITS];
	Matrix3x3 back;

	//	coda->GetAlignmentTransforms( unitOffset, unitRotation );

	// Create the required OpenGLWindows each linked to a pane in the Form.
	hmdWindow0 = CreateOpenGLWindowInForm( hmdPanel0 );
	hmdWindow1 = CreateOpenGLWindowInForm( hmdPanel1, hmdWindow0->hRC );
	handWindow0 = CreateOpenGLWindowInForm( handPanel0,hmdWindow0->hRC );
	handWindow1 = CreateOpenGLWindowInForm( handPanel1, hmdWindow0->hRC );
	chestWindow0 = CreateOpenGLWindowInForm( chestPanel0, hmdWindow0->hRC );
	chestWindow1 = CreateOpenGLWindowInForm( chestPanel1, hmdWindow0->hRC );
	vrWindow0 = CreateOpenGLWindowInForm( vrPanel0, hmdWindow0->hRC );
	vrWindow1 = CreateOpenGLWindowInForm( vrPanel1, hmdWindow0->hRC );

	// Create a viewpoint that looks at the origin from the negative Z axis.
	// This is the cannonical viewpoint for an object at zero position and orientation.
	objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
	objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

	// Create viewpoints that looks at the origin from the position of each CODA.
	codaViewpoint0 = new Viewpoint( 6.0, 25.0, 10.0, 10000.0);
	codaViewpoint0->SetPosition( unitOffset[0] );
	vm.CopyVector( back[Z], unitOffset[0] );
	vm.NormalizeVector( back[Z] );
	vm.ComputeCrossProduct( back[X], vm.jVector, back[Z] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	codaViewpoint0->SetOrientation( back );

	codaViewpoint1 = new Viewpoint( 6.0, 25.0, 10.0, 10000.0);
	codaViewpoint1->SetPosition( unitOffset[1] );
	vm.CopyVector( back[Z], unitOffset[1] );
	vm.NormalizeVector( back[Z] );
	vm.ComputeCrossProduct( back[X], vm.jVector, back[Z] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	codaViewpoint1->SetOrientation( back );

	// Create the OpenGLObjects that depict the marker array structure.
	objects = new Grasp::GraspGLObjects();
	hmdStationary0 = objects->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handStationary0 = objects->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestStationary0 = objects->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );
	hmdStationary1 = objects->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handStationary1 = objects->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestStationary1 = objects->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );
	hmdMobile = objects->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handMobile = objects->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestMobile = objects->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );

	// Initially set the hand and chest away from the center.
	// This will be overidden later when the trackers are active, 
	// but it is convenient to do this here for testing when the trackers is not used.
	handMobile->SetPosition( 400.0, -100.0, -400.0 );
	chestMobile->SetPosition( 0.0, -500.0, 0.0 );
	mobiles = new Yoke();
	mobiles->AddComponent( hmdMobile );
	mobiles->AddComponent( handMobile );
	mobiles->AddComponent( chestMobile );


}


// Draw the 3D graphics.
void GraspMMIGraphsForm::RenderWindow( OpenGLWindow *window, Viewpoint *viewpoint, OpenGLObject *object ) {
	window->Activate();
	window->Clear( 0.10, 0.10, 0.30 );
	viewpoint->Apply( window, CYCLOPS );
	object->Draw();
	window->Swap();
}

void GraspMMIGraphsForm::Render( void ) {

	MarkerFrame unitMarkerFrame[2];	
	MarkerFrame markerFrame;
	TrackerPose pose;

	hmdMobile->SetPose( pose.pose );
	handMobile->SetPose( pose.pose );
	chestMobile->SetPose( pose.pose );

	hmdStationary0->ShowVisibility( unitMarkerFrame[0], CYAN );
	handStationary0->ShowVisibility( unitMarkerFrame[0], MAGENTA );
	chestStationary0->ShowVisibility( unitMarkerFrame[0], YELLOW );

	RenderWindow( hmdWindow0, objectViewpoint, hmdStationary0 );
	RenderWindow( handWindow0, objectViewpoint, handStationary0 );
	RenderWindow( chestWindow0, objectViewpoint, chestStationary0 );

	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[0], CYAN );
	handMobile->ShowVisibility( unitMarkerFrame[0], MAGENTA );
	chestMobile->ShowVisibility( unitMarkerFrame[0], YELLOW );
	RenderWindow( vrWindow0, codaViewpoint0, mobiles );

	// Show the visibility of each marker superimposed on it's marker 
	// structure, each structure in a separate window for each CODA unit.
	hmdStationary1->ShowVisibility( unitMarkerFrame[1], CYAN );
	handStationary1->ShowVisibility( unitMarkerFrame[1], MAGENTA );
	chestStationary1->ShowVisibility( unitMarkerFrame[1], YELLOW );

	RenderWindow( hmdWindow1, objectViewpoint, hmdStationary1 );
	RenderWindow( handWindow1, objectViewpoint, handStationary1 );
	RenderWindow( chestWindow1, objectViewpoint, chestStationary1 );

	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[1], CYAN );
	handMobile->ShowVisibility( unitMarkerFrame[1], MAGENTA );
	chestMobile->ShowVisibility( unitMarkerFrame[1], YELLOW );
	RenderWindow( vrWindow1, codaViewpoint1, mobiles );

}