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

	// Create the required OpenGLWindows each linked to a pane in the Form.
	hmdWindow0 = CreateOpenGLWindowInForm( hmdPanel0 );
	hmdWindow1 = CreateOpenGLWindowInForm( hmdPanel1, hmdWindow0->hRC );
	handWindow0 = CreateOpenGLWindowInForm( handPanel0,hmdWindow0->hRC );
	handWindow1 = CreateOpenGLWindowInForm( handPanel1, hmdWindow0->hRC );
	chestWindow0 = CreateOpenGLWindowInForm( chestPanel0, hmdWindow0->hRC );
	chestWindow1 = CreateOpenGLWindowInForm( chestPanel1, hmdWindow0->hRC );
	codaWindow0 = CreateOpenGLWindowInForm( codaPanel0, hmdWindow0->hRC );
	codaWindow1 = CreateOpenGLWindowInForm( codaPanel1, hmdWindow0->hRC );
	sideWindow = CreateOpenGLWindowInForm( sidePanel, hmdWindow0->hRC );
	forwardWindow = CreateOpenGLWindowInForm( forwardPanel, hmdWindow0->hRC );

	vrWindow = CreateOpenGLWindowInForm( vrPanel );

	// Create a viewpoint that looks at the origin from the negative Z axis.
	// This is the cannonical viewpoint for an object at zero position and orientation.
	objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
	objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );
	objectViewpoint0 = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	objectViewpoint1 = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);

	// Create viewpoints for each coda.
	codaViewpoint0 = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	codaViewpoint1 = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);

	// Default view is from straight in front.
	codaViewpoint0->SetPosition( 0.0, 200.0, - 2000.0 );
	codaViewpoint0->SetOrientation( 0.0, 0.0, 180.0 );
	codaViewpoint1->SetPosition( 0.0, 200.0, - 2000.0 );
	codaViewpoint1->SetOrientation( 0.0, 0.0, 180.0 );

	sideViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	sideViewpoint->SetPosition( 2000.0, 200.0, -500.0 );
	sideViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );

	// Look forward toward the codas. 
	forwardViewpoint = new Viewpoint( 6.0, 65.0, 10.0, 10000.0);
	// Shift forward and up so that the codas fall in a reasonable field of view.
	forwardViewpoint->SetPosition( 0.0, 500.0, -1000.0 );

	subjectViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	subjectViewpoint->SetPosition( 0.0, 0.0, 0.0 );
	subjectViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create the OpenGLObjects that depict the marker array structure.
	renderer = new Grasp::GraspGLObjects();
	hmdStationary = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handStationary = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestStationary = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );
	hmdMobile = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
	handMobile = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
	chestMobile = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );

	// Now create all the objects for the VR rendering of what the subject sees.
	renderer->CreateVRObjects();
	renderer->PlaceVRObjects();
	renderer->CreateAuxiliaryObjects();

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
	window->Clear( 0.10, 0.10, 0.30, 1.0 );
	viewpoint->Apply( window, CYCLOPS );
	renderer->SetLighting();
	object->Draw();
	window->Swap();
}

void GraspMMIGraphsForm::RenderWindow( OpenGLWindow *window, OrthoViewpoint *viewpoint, OpenGLObject *object ) {
	window->Activate();
	window->Clear( 0.10, 0.10, 0.30, 1.0 );
	viewpoint->Apply( window );
	renderer->SetLighting();
	object->Draw();
	window->Swap();
}

void GraspMMIGraphsForm::RenderSubjectView( OpenGLWindow *window, Viewpoint *viewpoint, bool vr_active ) {
	window->Activate();
	if ( vr_active ) {
		window->Clear( BLACK );
		viewpoint->Apply( window, CYCLOPS );
		renderer->DrawVR();
	}
	else window->Clear( 0.0, 0.3, 0.3, 1.0 );
	window->Swap();
}

void GraspMMIGraphsForm::LookAtFrom( Viewpoint *viewpoint, const Vector3 target, Vector3 from ) {

	static VectorsMixin vm;
	Matrix3x3 back;

	// Viewpoints look at the origin from the position of each CODA.
	vm.SubtractVectors( back[Z], from, target );
	vm.NormalizeVector( back[Z] );
	vm.ComputeCrossProduct( back[X], vm.jVector, back[Z] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	viewpoint->SetPosition( from );
	viewpoint->SetOrientation( back );

}

void GraspMMIGraphsForm::RenderVR( unsigned int index ) {

	MarkerFrame unitMarkerFrame[2];	
	MarkerFrame markerFrame;
	TrackerPose pose;

	bool fromCoda = fromCodaCheckBox->Checked;

	int alignmentIndex;
	static VectorsMixin vm;

	// Search for a slice with alignment information that tells us where the codas are.
	for ( alignmentIndex = index; alignmentIndex > 0; alignmentIndex -- ) {

		if ( graspDataSlice[alignmentIndex].clientType == GraspRealtimeDataSlice::ALIGNPRE ||
			 graspDataSlice[alignmentIndex].clientType == GraspRealtimeDataSlice::ALIGNPOST ) {

			break;
		}
	}
	// If we found the alignment information, define viewpoints that look from each coda to the origin.
	// If not, use a canonical view from straight in front of the chair.
	if ( alignmentIndex > 0 ) {

		LookAtFrom( codaViewpoint0, vm.zeroVector, graspDataSlice[alignmentIndex].alignmentOffset[0] );
		LookAtFrom( codaViewpoint1, vm.zeroVector, graspDataSlice[alignmentIndex].alignmentOffset[1] );

		renderer->coda[0]->SetPosition( graspDataSlice[alignmentIndex].alignmentOffset[0] );
		renderer->coda[0]->SetOrientation( graspDataSlice[alignmentIndex].alignmentRotation[0] );
		renderer->coda[1]->SetPosition( graspDataSlice[alignmentIndex].alignmentOffset[1] );
		renderer->coda[1]->SetOrientation( graspDataSlice[alignmentIndex].alignmentRotation[1] );

	}

	else {

		// View from straight in front.
		codaViewpoint0->SetPosition( 0.0, 200.0, - 2000.0 );
		codaViewpoint0->SetOrientation( 0.0, 0.0, 180.0 );
		codaViewpoint1->SetPosition( 0.0, 200.0, - 2000.0 );
		codaViewpoint1->SetOrientation( 0.0, 0.0, 180.0 );

	}

	// Each realtime slice has only one marker frame, corresponding to one or the other coda.
	// Here we need to find the most recent slice that contains the marker information from
	// the 'other' coda.  First, we set the visibility of all markers in a local instance of
	// marker frames so that if there is no previous slice that contains the 'other' coda's frame
	// we will show here that the markers are not visible.
	for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		unitMarkerFrame[0].marker[mrk].visibility = false;
		unitMarkerFrame[1].marker[mrk].visibility = false;
	}
	// Copy the frame from the current slice into the appropriate local copy.
	int currentCodaUnit = graspDataSlice[index].codaUnit;
	CopyMarkerFrame( unitMarkerFrame[currentCodaUnit], graspDataSlice[index].codaFrame );
	// Now look backward for a slice that contains the frame from the other coda.
	// If we find it, copy that frame into the local frame for the other coda.
	int otherIndex;
	int otherCodaUnit;
	for ( otherIndex = index; otherIndex > 0; otherIndex -- ) {
		if ( currentCodaUnit != graspDataSlice[otherIndex].codaUnit ) break;
	}
	if ( otherIndex >= 0 ) 	{
		otherCodaUnit = graspDataSlice[otherIndex].codaUnit;
		CopyMarkerFrame( unitMarkerFrame[otherCodaUnit], graspDataSlice[otherIndex].codaFrame );
	}

	// Show the objects from the side.
	// Use the pose that was computed by the GraspDaemonTracker.
	// We want to show which markers were visible, but we don't know which coda was actually
	// used to compute the pose in the cascade tracker. So we 
	// use the average of the data from both codas to compute the 
	// visibility so that the markers are shown as on if visible in either one.
	ComputeAverageMarkerFrame( markerFrame, unitMarkerFrame, 2 );
	hmdTracker->SetMarkerFrameBuffer( &markerFrame );
	hmdMobile->ShowVisibility( markerFrame, GREEN );
	// Use the pose that was sent in the telemetry packet.
	if ( graspDataSlice[index].HMD.visible ) {
		hmdMobile->SetPose( graspDataSlice[index].HMD.pose );
		hmdMobile->Enable();
	}
	else hmdMobile->Disable();

	handTracker->SetMarkerFrameBuffer( &markerFrame );
	handMobile->ShowVisibility( markerFrame, CYAN );
	if ( graspDataSlice[index].hand.visible ) {
		handMobile->SetPose( graspDataSlice[index].hand.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();

	chestTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	chestMobile->ShowVisibility( markerFrame, BLUE );
	if ( graspDataSlice[index].chest.visible ) {
		chestMobile->SetPose( graspDataSlice[index].chest.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();
	RenderWindow( sideWindow, sideViewpoint, mobiles );
	
	// Show where the CODAs are, looking from the origin.
	RenderWindow( forwardWindow, forwardViewpoint, renderer->codas );

	// Now look at where each object is measured to be by each coda unit.
	// Tell each tracker to use the marker frame corresponding to coda 0 to compute the pose
	// and then set the pose of the corresponding object to that pose.
	// Show which markers are on or off for each object.
	hmdStationary->ShowVisibility( unitMarkerFrame[0], GREEN );
	handStationary->ShowVisibility( unitMarkerFrame[0], CYAN );
	chestStationary->ShowVisibility( unitMarkerFrame[0], BLUE );
	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[0], GREEN );
	handMobile->ShowVisibility( unitMarkerFrame[0], CYAN );
	chestMobile->ShowVisibility( unitMarkerFrame[0], BLUE );

	hmdTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	hmdTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		hmdMobile->SetPose( pose.pose );
		hmdMobile->Enable();
	}
	else hmdMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint0, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[0] );
		RenderWindow( hmdWindow0, objectViewpoint0, hmdMobile );
	}
	else RenderWindow( hmdWindow0, objectViewpoint, hmdStationary );
	handTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	handTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		handMobile->SetPose( pose.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint0, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[0] );
		RenderWindow( handWindow0, objectViewpoint0, handMobile );
	}
	else RenderWindow( handWindow0, objectViewpoint, handStationary );
	chestTracker->GetCurrentPose( pose );
	chestTracker->SetMarkerFrameBuffer( &unitMarkerFrame[0] );
	if ( pose.visible ) {
		chestMobile->SetPose( pose.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint0, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[0] );
		RenderWindow( chestWindow0, objectViewpoint0, chestMobile );
	}
	else RenderWindow( chestWindow0, objectViewpoint, chestStationary );

	RenderWindow( codaWindow0, codaViewpoint0, mobiles );

	///
	/// Now repeat the above process for coda 1.
	///
	hmdStationary->ShowVisibility( unitMarkerFrame[1], GREEN );
	handStationary->ShowVisibility( unitMarkerFrame[1], CYAN );
	chestStationary->ShowVisibility( unitMarkerFrame[1], BLUE );
	// Show the position and orientation of each marker structure
	// from the perspective of each CODA unit.
	hmdMobile->ShowVisibility( unitMarkerFrame[1], GREEN );
	handMobile->ShowVisibility( unitMarkerFrame[1], CYAN );
	chestMobile->ShowVisibility( unitMarkerFrame[1], BLUE );

	hmdTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	hmdTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		hmdMobile->SetPose( pose.pose );
		hmdMobile->Enable();
	}
	else hmdMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint1, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[1] );
		RenderWindow( hmdWindow1, objectViewpoint1, hmdMobile );
	}
	else RenderWindow( hmdWindow1, objectViewpoint, hmdStationary );
	handTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	handTracker->GetCurrentPose( pose );
	if ( pose.visible ) {
		handMobile->SetPose( pose.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint1, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[1] );
		RenderWindow( handWindow1, objectViewpoint1, handMobile );
	}
	else RenderWindow( handWindow1, objectViewpoint, handStationary );
	chestTracker->GetCurrentPose( pose );
	chestTracker->SetMarkerFrameBuffer( &unitMarkerFrame[1] );
	if ( pose.visible ) {
		chestMobile->SetPose( pose.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();
	if ( alignmentIndex > 0 && pose.visible && fromCoda ) {
		LookAtFrom( objectViewpoint1, pose.pose.position, graspDataSlice[alignmentIndex].alignmentOffset[1] );
		RenderWindow( chestWindow1, objectViewpoint1, chestMobile );
	}
	else RenderWindow( chestWindow1, objectViewpoint, chestStationary );

	RenderWindow( codaWindow1, codaViewpoint1, mobiles );



	/// 
	/// Show what the subject should be seeing in the VR display.
	///

	if ( graspDataSlice[index].clientType == GraspRealtimeDataSlice::GRASP ) {

		// Enable and disable objects in the VR view.
		// This sequence has to correspond to what happens in GraspTaskManager::FillClientData().
		// I could easily have made an error here.
		unsigned long bit;
	
		bit = 0x01;
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

		// subjectViewpoint->SetOrientation( graspDataSlice[index].headPose.orientation );
		subjectViewpoint->SetPose( graspDataSlice[index].headPose );
		renderer->hand->SetPose( graspDataSlice[index].handPose );
		RenderSubjectView( vrWindow, subjectViewpoint, true );



	}	
	else {
		RenderSubjectView( vrWindow, subjectViewpoint, false );
	}


}