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

#include "..\Grasp\Grasp.h"
#include "..\GraspGUI\GraspGUI.h"
#include "..\AlignToRigidBodyGUI\AligntoRigidBodyGUI.h"


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
	codaWindow0 = CreateOpenGLWindowInForm( codaPanel0, hmdWindow0->hRC );
	codaWindow1 = CreateOpenGLWindowInForm( codaPanel1, hmdWindow0->hRC );
	vrWindow = CreateOpenGLWindowInForm( vrPanel, hmdWindow0->hRC );
	subjectWindow = CreateOpenGLWindowInForm( subjectPanel, hmdWindow0->hRC );

	// Create a viewpoint that looks at the origin from the negative Z axis.
	// This is the cannonical viewpoint for an object at zero position and orientation.
	objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
	objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

	// Create viewpoints that looks at the origin from the position of each CODA.
	codaViewpoint0 = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	codaViewpoint0->SetPosition( unitOffset[0] );
	vm.CopyVector( back[Z], unitOffset[0] );
	vm.NormalizeVector( back[Z] );
	vm.ComputeCrossProduct( back[X], vm.jVector, back[Z] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	codaViewpoint0->SetOrientation( back );

	// Remove this_
	codaViewpoint0->SetPosition( 0.0, 200.0, - 2000.0 );
	codaViewpoint0->SetOrientation( 0.0, 0.0, 180.0 );

	codaViewpoint1 = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	codaViewpoint1->SetPosition( unitOffset[1] );
	vm.CopyVector( back[Z], unitOffset[1] );
	vm.NormalizeVector( back[Z] );
	vm.ComputeCrossProduct( back[X], vm.jVector, back[Z] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	codaViewpoint1->SetOrientation( back );

	// Remove this_
	codaViewpoint1->SetPosition( 0.0, 200.0, - 2000.0 );
	codaViewpoint1->SetOrientation( 0.0, 0.0, 180.0 );

	sideViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	sideViewpoint->SetPosition( 2000.0, 200.0, -500.0 );
	sideViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );

	subjectViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	subjectViewpoint->SetPosition( 0.0, 0.0, 0.0 );
	subjectViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create the OpenGLObjects that depict the marker array structure.
	renderer = new Grasp::GraspGLObjects();
	hmdStationary0 = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handStationary0 = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestStationary0 = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );
	hmdStationary1 = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handStationary1 = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestStationary1 = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );
	hmdMobile = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handMobile = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestMobile = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );

	// Now create all the objects for the VR rendering of what the subject sees.
	renderer->CreateVRObjects();
	renderer->PlaceVRObjects();

	// Create trackers that will transform marker positions into poses.
	// This frame is just a place holder.
	MarkerFrame codaFrame;
	hmdTracker = new CodaPoseTracker( &codaFrame );
	hmdTracker->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );
	handTracker = new CodaPoseTracker( &codaFrame );
	handTracker->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );
	chestTracker = new CodaPoseTracker( &codaFrame );
	chestTracker->ReadModelMarkerPositions( "Bdy\\Chest.bdy" );


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

void GraspMMIGraphsForm::RenderSubjectView( OpenGLWindow *window, Viewpoint *viewpoint ) {
	window->Activate();
	window->Clear( 0.10, 0.10, 0.30 );
	viewpoint->Apply( window, CYCLOPS );
	renderer->DrawVR();
	window->Swap();
}
void GraspMMIGraphsForm::RenderVR( unsigned int index ) {

	MarkerFrame unitMarkerFrame[2];	
	MarkerFrame markerFrame;
	TrackerPose pose;

	for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		unitMarkerFrame[0].marker[mrk].visibility = false;
		unitMarkerFrame[1].marker[mrk].visibility = false;
	}

	int currentCodaUnit = graspDataSlice[index].codaUnit;
	CopyMarkerFrame( unitMarkerFrame[currentCodaUnit], graspDataSlice[index].codaFrame );
	int otherIndex;
	int otherCodaUnit;
	for ( otherIndex = index; otherIndex > 0; otherIndex -- ) {
		if ( currentCodaUnit != graspDataSlice[otherIndex].codaUnit ) break;
	}
	if ( otherIndex >= 0 ) 	{
		otherCodaUnit = graspDataSlice[otherIndex].codaUnit;
		CopyMarkerFrame( unitMarkerFrame[otherCodaUnit], graspDataSlice[otherIndex].codaFrame );
	}


	hmdTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	hmdTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		hmdMobile->SetPose( pose.pose );
		hmdMobile->Enable();
	}
	else hmdMobile->Disable();

	handTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	handTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		handMobile->SetPose( pose.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();

	chestTracker->GetCurrentPose( pose );
	chestTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	if ( pose.visible ) {
		chestMobile->SetPose( pose.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();

	hmdStationary0->ShowVisibility( unitMarkerFrame[0], GREEN );
	handStationary0->ShowVisibility( unitMarkerFrame[0], CYAN );
	chestStationary0->ShowVisibility( unitMarkerFrame[0], BLUE );

	RenderWindow( hmdWindow0, objectViewpoint, hmdStationary0 );
	RenderWindow( handWindow0, objectViewpoint, handStationary0 );
	RenderWindow( chestWindow0, objectViewpoint, chestStationary0 );

	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[0], GREEN );
	handMobile->ShowVisibility( unitMarkerFrame[0], CYAN );
	chestMobile->ShowVisibility( unitMarkerFrame[0], BLUE );
	RenderWindow( codaWindow0, codaViewpoint0, mobiles );

	hmdTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	hmdTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		handMobile->SetPose( pose.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();

	handTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	handTracker->GetCurrentPose( pose );
	handMobile->SetPose( pose.pose );

	chestTracker->GetCurrentPose( pose );
	chestTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	if ( pose.visible ) {
		chestMobile->SetPose( pose.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();

	// Show the visibility of each marker superimposed on it's marker 
	// structure, each structure in a separate window for each CODA unit.
	hmdStationary1->ShowVisibility( unitMarkerFrame[1], GREEN );
	handStationary1->ShowVisibility( unitMarkerFrame[1], CYAN );
	chestStationary1->ShowVisibility( unitMarkerFrame[1], BLUE );

	RenderWindow( hmdWindow1, objectViewpoint, hmdStationary1 );
	RenderWindow( handWindow1, objectViewpoint, handStationary1 );
	RenderWindow( chestWindow1, objectViewpoint, chestStationary1 );

	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[1], GREEN );
	handMobile->ShowVisibility( unitMarkerFrame[1], CYAN );
	chestMobile->ShowVisibility( unitMarkerFrame[1], BLUE );
	RenderWindow( codaWindow1, codaViewpoint1, mobiles );

	RenderWindow( subjectWindow, sideViewpoint, mobiles );

	// Enable and disable objects in the VR view:
	unsigned long bit = 0x01;
	renderer->orientationTarget->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->positionOnlyTarget->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->straightAheadTarget->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->response->enabled  = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->projectiles->enabled= ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->multiProjectile->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->monoProjectile->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->glasses->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->headTiltPrompt->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->gazeLaser->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->handLaser->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->successIndicator->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->vTool->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->kTool->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->vkTool->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->kkTool->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->handRollPrompt->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->lowerHandPrompt->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->wristZone->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->room->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->tunnel->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->starrySky->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
	renderer->darkSky->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;

	bit = 0x01;
	renderer->readyToStartIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->blockCompletedIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->lowerHandIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->raiseHandIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->raiseArmTimeoutIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->headMisalignIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->manualRejectIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->invalidateTrialIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->headAlignTimeoutIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->timeoutIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->responseTimeoutIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->handRotateTimeoutIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->handTooSoonIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->handShouldNotBeRaisedIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->straightenHeadIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->vrCompletedIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;
	renderer->demoIndicator->enabled = ( graspDataSlice[index].spinnerBits & bit ) != 0; bit = bit << 1;

	subjectViewpoint->SetOrientation( graspDataSlice[index].headPose.orientation );
	renderer->hand->SetPose( graspDataSlice[index].handPose );
	RenderSubjectView( vrWindow, subjectViewpoint );

}