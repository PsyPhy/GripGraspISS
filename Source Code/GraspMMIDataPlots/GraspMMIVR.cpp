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

	// Forms does not allow mixed managed and unmanaged arrays.
	// So to have arrays of the following, they have to be 
	// allocated at runtime. 
	hmdWindow = CreateOpenGLWindowArray( CODA_UNITS );
	handWindow = CreateOpenGLWindowArray( CODA_UNITS );
	chestWindow = CreateOpenGLWindowArray( CODA_UNITS );
	codaWindow = CreateOpenGLWindowArray( CODA_UNITS );
	codaViewpoint = CreateViewpointArray( CODA_UNITS );

	// Create the required OpenGLWindows each linked to a pane in the Form.
	hmdWindow[0] = CreateOpenGLWindowInForm( hmdPanel0 );
	hmdWindow[1] = CreateOpenGLWindowInForm( hmdPanel1, hmdWindow[0]->hRC );
	handWindow[0] = CreateOpenGLWindowInForm( handPanel0,hmdWindow[0]->hRC );
	handWindow[1] = CreateOpenGLWindowInForm( handPanel1, hmdWindow[0]->hRC );
	chestWindow[0] = CreateOpenGLWindowInForm( chestPanel0, hmdWindow[0]->hRC );
	chestWindow[1] = CreateOpenGLWindowInForm( chestPanel1, hmdWindow[0]->hRC );
	codaWindow[0] = CreateOpenGLWindowInForm( codaPanel0, hmdWindow[0]->hRC );
	codaWindow[1] = CreateOpenGLWindowInForm( codaPanel1, hmdWindow[0]->hRC );
	sideWindow = CreateOpenGLWindowInForm( sidePanel, hmdWindow[0]->hRC );
	forwardWindow = CreateOpenGLWindowInForm( forwardPanel, hmdWindow[0]->hRC );

	vrSubjectWindow = CreateOpenGLWindowInForm( vrSubjectPanel );
	vrSideWindow = CreateOpenGLWindowInForm( vrSidePanel, vrSubjectWindow->hRC );

	// Create a viewpoint that looks at the origin from the negative Z axis.
	// This is the cannonical viewpoint for an object at zero position and orientation.
	objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
	objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );
	// The following viewpoint is used to zoom in on each object when it 
	//  it at its position and orientation.
	focusViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	// Create viewpoints for each coda.
	// Default view is from straight in front, but these will be 
	// set to look at the origin from each coda when alignment
	// information is available.
	for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
		codaViewpoint[unit] = new Viewpoint( 6.0, 35.0, 10.0, 10000.0);
		codaViewpoint[unit]->SetPosition( 0.0, 200.0, - 2000.0 );
		codaViewpoint[unit]->SetOrientation( 0.0, 0.0, 180.0 );
	}
	// Look at the subject from the side.
	sideViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	sideViewpoint->SetPosition( 2000.0, 200.0, -500.0 );
	sideViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );
	// Look forward toward the codas. 
	// We shift it forward and up so that the codas fall in a reasonable field of view.
	forwardViewpoint = new Viewpoint( 6.0, 25.0, 10.0, 8000.0);
	forwardViewpoint->SetPosition( 0.0, 500.0, 3000.0 );
	forwardViewpoint->SetOrientation( 0.0, 0.0, 0.0 );
	// This is the viewpoint of the subject in the virtual world.
	// Default is looking straight ahead.
	vrSubjectViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);
	vrSubjectViewpoint->SetPosition( 0.0, 0.0, 0.0 );
	vrSubjectViewpoint->SetOrientation( 0.0, 0.0, 0.0 );
	// Look at the subject from the side.
	vrSideViewpoint = new Viewpoint( 6.0, 85.0, 10.0, 10000.0);
	vrSideViewpoint->SetPosition( 900.0, 0.0, 200.0 );
	vrSideViewpoint->SetOrientation( 0.0, 0.0, -45.0 );

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

	codas = new Assembly();
	coda = (Coda **) calloc( CODA_UNITS, sizeof( *coda ) );
	for (int unit = 0; unit < CODA_UNITS; unit++ ) {
		coda[unit] = new Coda();
		codas->AddComponent( coda[unit] );
	}
	coda[0]->SetColor( .5, .1, .1 );
	coda[1]->SetColor( .1, .1, .6 );
	coda[0]->proximity->SetColor( 1.0, 0.5, 0.5, 0.2 );
	coda[1]->proximity->SetColor( 0.5, 0.5, 1.0, 0.2 );
	coda[0]->proximity->Disable();
	coda[1]->proximity->Disable();
	coda[0]->rays->Disable();
	coda[1]->rays->Disable();
	coda[0]->fov->SetColor( 1.0, 0.0, 0.0, 0.5 );
	coda[1]->fov->SetColor( 0.0, 0.0, 1.0, 0.5 );

	// Create an anchor that we can rotate and displace to take into account
	// where the origin is with respect to the ISS module for a given hardware configuration.
	ensemble = new Assembly();
	ensemble->AddComponent( codas );
	static Vector3 chairPosition = { -250.0, -200.0, 1900 };
	ensemble->SetPosition( chairPosition );
	ensemble->SetOrientation( 0.0, 0.0, 0.0 );



	// Create trackers that will transform marker positions into poses.
	// This frame is just a place holder.
	MarkerFrame codaFrame;
	tracker = (CodaPoseTracker **) calloc( MARKER_STRUCTURES, sizeof( *tracker ) );
	tracker[HMD_STRUCTURE] = hmdTracker = new CodaPoseTracker( &codaFrame );
	hmdTracker->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );
	tracker[HAND_STRUCTURE] = handTracker = new CodaPoseTracker( &codaFrame );
	handTracker->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );
	tracker[CHEST_STRUCTURE] = chestTracker = new CodaPoseTracker( &codaFrame );
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

void GraspMMIGraphsForm::EraseWindow( OpenGLWindow *window ) {
	window->Activate();
	window->Clear( 0.10, 0.10, 0.30, 1.0 );
	window->Swap();
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

void GraspMMIGraphsForm::RenderSideView( OpenGLWindow *window, Viewpoint *viewpoint, bool vr_active ) {
	window->Activate();
	if ( vr_active ) {
		window->Clear( BLACK );
		viewpoint->Apply( window, CYCLOPS );
		renderer->DrawVR();
		renderer->DrawBody();
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
	vm.NormalizeVector( back[X] );
	vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
	vm.NormalizeVector( back[Y] );
	viewpoint->SetPosition( from );
	viewpoint->SetOrientation( back );

}

void GraspMMIGraphsForm::MoveToInstant( double instant ) {
	unsigned int hk, rt;
	for ( hk = nHousekeepingSlices - 1; hk > 0; hk-- ) {
		if ( graspHousekeepingSlice[hk].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[hk].absoluteTime <= instant ) break;
	}
	for ( rt = nDataSlices - 1; rt > 0; rt-- ) {
		if ( graspDataSlice[rt].absoluteTime != MISSING_DOUBLE && graspDataSlice[rt].absoluteTime <= instant ) break;
	}
	// fOutputDebugString( "Instant: %.3f  HK: %u  (%.3f) %u  (%.3f)  RT: %u (%.3f)\n", instant, hk, graspHousekeepingSlice[hk].absoluteTime, hk+1, graspHousekeepingSlice[hk+1].absoluteTime, rt, graspDataSlice[rt].absoluteTime );
	if ( fabs( graspDataSlice[rt].absoluteTime - instant ) < PACKET_STREAM_BREAK_THRESHOLD ) {
			RenderVR( rt );
	}
	else if ( fabs( graspHousekeepingSlice[hk].absoluteTime - instant ) < PACKET_STREAM_BREAK_THRESHOLD ) {
		RenderMissingVR( graspHousekeepingSlice[hk].absoluteTime );
	}
	else RenderMissingVR( instant );

}
void GraspMMIGraphsForm::MoveToSlice( int index ) {
	fOutputDebugString( "MoveToSlice() Index: %u  Instant: %.3f\n", index, graspDataSlice[index].absoluteTime );
	RenderVR( index );
}

void GraspMMIGraphsForm::PlotCursor( void ) {

	DisplayActivate( cursorDisplay );
	DisplayErase( cursorDisplay );
	::View view = LayoutView( cursorLayout, 0, 0 );
	ViewColor( view, axis_color );
	ViewBox( view );
	double cursor_instant = current_vr_instant;
	if ( cursor_instant <  playbackScrollBar->Minimum )  cursor_instant =  playbackScrollBar->Minimum;
	if ( cursor_instant > playbackScrollBar->Maximum  )  cursor_instant = playbackScrollBar->Maximum;
	ViewSetXLimits( view, playbackScrollBar->Minimum, playbackScrollBar->Maximum );
	ViewSetYLimits( view, 0, 1 );
	ViewColor( view, MAGENTA );
	ViewVerticalLine( view, cursor_instant );
	DisplaySwap( cursorDisplay );

}


void GraspMMIGraphsForm::RenderMissingVR( double instant ) {

	EraseWindow( vrSubjectWindow );
	EraseWindow( vrSideWindow );
	EraseWindow( sideWindow );
	EraseWindow( forwardWindow );
	for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
		EraseWindow( hmdWindow[unit] );
		EraseWindow( handWindow[unit] );
		EraseWindow( chestWindow[unit] );
		EraseWindow( codaWindow[unit] );
	}
	// Show the time instant corresponding to this slice.
	vrFrameTextBox->Text = CreateTimeStringExtended( instant );
	// Keep track of it to be able to plot the cursor.
	current_vr_instant = instant;
	PlotCursor();

}

void GraspMMIGraphsForm::RenderVR( unsigned int index ) {

	MarkerFrame unitMarkerFrame[2];	
	MarkerFrame markerFrame;
	TrackerPose hmdPose, handPose, chestPose;

	int alignment_index;
	static VectorsMixin vm;
	bool show_real_markers = realMarkersCheckBox->Checked;

	// Show the time instant corresponding to this slice.
	double frame_time = graspDataSlice[index].absoluteTime;
	vrFrameTextBox->Text = CreateTimeStringExtended( frame_time );
	// Keep track of it to be able to plot the cursor.
	current_vr_instant = frame_time;

	// Search for a slice with alignment information that tells us where the codas are.
	for ( alignment_index = index; alignment_index >= 0; alignment_index -- ) {

		if ( graspDataSlice[alignment_index].clientType == GraspRealtimeDataSlice::ALIGNPRE ||
			 graspDataSlice[alignment_index].clientType == GraspRealtimeDataSlice::ALIGNPOST ) {

			break;
		}
	}
	// If we found the alignment information, define viewpoints that look from each coda to the origin.
	// If not, use a canonical view from straight in front of the chair.
	if ( alignment_index >= 0 ) {

		Matrix3x3 inverse, transpose;
		for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
			coda[unit]->SetPosition( graspDataSlice[alignment_index].alignmentOffset[unit] );
			vm.TransposeMatrix( transpose, graspDataSlice[alignment_index].alignmentRotation[unit] );
			vm.InvertMatrix( inverse, graspDataSlice[alignment_index].alignmentRotation[unit] );
			coda[unit]->SetOrientation( inverse );
			coda[unit]->Enable();
			LookAtFrom( codaViewpoint[unit], vm.zeroVector, graspDataSlice[alignment_index].alignmentOffset[unit] );
		}

		if ( graspDataSlice[alignment_index].clientType == GraspRealtimeDataSlice::ALIGNPRE ) {
			alignmentFrameTextBox->Text = CreateTimeString( graspDataSlice[alignment_index].absoluteTime ) + " PRE";
		}
		if ( graspDataSlice[alignment_index].clientType == GraspRealtimeDataSlice::ALIGNPOST ) {
			alignmentFrameTextBox->Text = CreateTimeString( graspDataSlice[alignment_index].absoluteTime ) + " POST";
		}

		FILE *fp = fopen( "tempxform.dat", "w" );
		fprintf( fp, "header\n" );
		for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
			fprintf( fp, "sn\n" );
			fprintf( fp, "Offset%d=%f,%f,%f\n", unit,
				graspDataSlice[alignment_index].alignmentOffset[unit][X],
				graspDataSlice[alignment_index].alignmentOffset[unit][Y],
				graspDataSlice[alignment_index].alignmentOffset[unit][Z] );
			fprintf( fp, "TransformX%d=%f,%f,%f\n", unit,
				graspDataSlice[alignment_index].alignmentRotation[unit][X][X],
				graspDataSlice[alignment_index].alignmentRotation[unit][X][Y],
				graspDataSlice[alignment_index].alignmentRotation[unit][X][Z] );
			fprintf( fp, "TransformY%d=%f,%f,%f\n", unit,
				graspDataSlice[alignment_index].alignmentRotation[unit][Y][X],
				graspDataSlice[alignment_index].alignmentRotation[unit][Y][Y],
				graspDataSlice[alignment_index].alignmentRotation[unit][Y][Z] );
			fprintf( fp, "TransformZ%d=%f,%f,%f\n", unit,
				graspDataSlice[alignment_index].alignmentRotation[unit][Z][X],
				graspDataSlice[alignment_index].alignmentRotation[unit][Z][Y],
				graspDataSlice[alignment_index].alignmentRotation[unit][Z][Z] );
		}
		fclose( fp );

	}

	else {

		for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
			coda[unit]->Disable();
			codaViewpoint[unit]->SetPosition( 0.0, 0.0, -3000.0 );
			codaViewpoint[unit]->SetOrientation( 0.0, 0.0, 180.0 );
		}
		alignmentFrameTextBox->Text = "not available";

	}
	
	// We want to show which markers were visible, but we don't know which coda was actually
	// used to compute the pose in the cascade tracker. So we 
	// use the average of the data from both codas to compute the 
	// visibility so that the markers are shown as on if visible in either one.
	for ( int unit = 0; unit < CODA_UNITS; unit++ ) CopyMarkerFrame( unitMarkerFrame[unit], graspDataSlice[index].codaFrame[unit] );
	ComputeAverageMarkerFrame( markerFrame, unitMarkerFrame, 2 );
	
	// Use the pose that was computed by the GraspDaemonTracker
	// and sent in the telemetry packet.
	if ( graspDataSlice[index].HMD.visible ) {
		hmdMobile->ShowVisibility( markerFrame, GREEN );
		hmdMobile->SetPose( graspDataSlice[index].HMD.pose );
		hmdMobile->Enable();
	}
	else hmdMobile->Disable();

	if ( graspDataSlice[index].hand.visible ) {
		handMobile->ShowVisibility( markerFrame, CYAN );
		handMobile->SetPose( graspDataSlice[index].hand.pose );
		handMobile->Enable();
	}
	else handMobile->Disable();

	if ( graspDataSlice[index].chest.visible ) {
		chestMobile->ShowVisibility( markerFrame, BLUE );
		chestMobile->SetPose( graspDataSlice[index].chest.pose );
		chestMobile->Enable();
	}
	else chestMobile->Disable();

	// We don't know which CODA was used to compute the pose of each object,
	// so we don't try to show the real marker positions in this view.
	hmdMobile->HideRealMarkers();
	handMobile->HideRealMarkers();
	chestMobile->HideRealMarkers();

	// Show the objects from the side.
	RenderWindow( sideWindow, sideViewpoint, mobiles );
	
	// Show where the CODAs are, looking from the origin.
	RenderWindow( forwardWindow, forwardViewpoint, ensemble );



	for ( int unit = 0; unit < CODA_UNITS; unit++ ) {

		// Show which markers are on or off for each object.
		// The marker structure itself is color coded to match the stripcharts.
		hmdStationary->SetColor( color_by_unit[unit] );
		hmdStationary->ShowVisibility( unitMarkerFrame[unit], GREEN );
		handStationary->SetColor( color_by_unit[unit] );
		handStationary->ShowVisibility( unitMarkerFrame[unit], CYAN );
		chestStationary->SetColor( color_by_unit[unit] );
		chestStationary->ShowVisibility( unitMarkerFrame[unit], BLUE );

		hmdMobile->ShowVisibility( unitMarkerFrame[unit], GREEN );
		handMobile->ShowVisibility( unitMarkerFrame[unit], CYAN );
		chestMobile->ShowVisibility( unitMarkerFrame[unit], BLUE );

		if ( show_real_markers ) {
			hmdMobile->ShowRealMarkers( unitMarkerFrame[unit] );
			handMobile->ShowRealMarkers( unitMarkerFrame[unit] );
			chestMobile->ShowRealMarkers( unitMarkerFrame[unit] );
		}

		// Show the objects as seen from each CODA unit.
		RenderWindow( codaWindow[unit], codaViewpoint[unit], mobiles );

		static Vector3 front_viewpoint = { 0.0, 0.0, -2500.0 };

		hmdTracker->ComputePose( hmdPose, &unitMarkerFrame[unit] );
		if ( hmdPose.visible ) {
			hmdMobile->SetPose( hmdPose.pose );
			hmdMobile->Enable();
		}
		else hmdMobile->Disable();
		if (  graspDataSlice[index].interUnitCoherence[HMD_STRUCTURE] != MISSING_DOUBLE && graspDataSlice[index].interUnitCoherence[HMD_STRUCTURE] > coherenceThreshold ) {
			hmdWarningPanel->BackColor = System::Drawing::Color::IndianRed;
		}
		else hmdWarningPanel->BackColor = normalBackgroundColor;
		hmdWarningPanel->Update();
		if ( hmdPose.visible && visibilityViewpointComboBox->SelectedIndex != VISIBILITY_ONLY ) {
			if ( visibilityViewpointComboBox->SelectedIndex == FROM_CODA && alignment_index >= 0 ) LookAtFrom( focusViewpoint, hmdPose.pose.position, graspDataSlice[alignment_index].alignmentOffset[unit] );
			else LookAtFrom( focusViewpoint, hmdPose.pose.position, front_viewpoint );
			RenderWindow( hmdWindow[unit], focusViewpoint, hmdMobile );
		}
		else RenderWindow( hmdWindow[unit], objectViewpoint, hmdStationary );

		handTracker->ComputePose( handPose, &unitMarkerFrame[unit] );
		if ( handPose.visible ) {
			handMobile->SetPose( handPose.pose );
			handMobile->Enable();
		}
		else handMobile->Disable();
		if (  graspDataSlice[index].interUnitCoherence[HAND_STRUCTURE] != MISSING_DOUBLE && graspDataSlice[index].interUnitCoherence[HAND_STRUCTURE] > coherenceThreshold ) {
			handWarningPanel->BackColor = System::Drawing::Color::IndianRed;
		}
		else handWarningPanel->BackColor = normalBackgroundColor;
		handWarningPanel->Update();
		if ( handPose.visible && visibilityViewpointComboBox->SelectedIndex != VISIBILITY_ONLY ) {
			if ( visibilityViewpointComboBox->SelectedIndex == FROM_CODA && alignment_index >= 0 ) LookAtFrom( focusViewpoint, handPose.pose.position, graspDataSlice[alignment_index].alignmentOffset[unit] );
			else LookAtFrom( focusViewpoint, handPose.pose.position, front_viewpoint );
			RenderWindow( handWindow[unit], focusViewpoint, handMobile );
		}
		else RenderWindow( handWindow[unit], objectViewpoint, handStationary );

		chestTracker->ComputePose( chestPose, &unitMarkerFrame[unit] );
		if ( chestPose.visible ) {
			chestMobile->SetPose( chestPose.pose );
			chestMobile->Enable();
		}
		else chestMobile->Disable();
		if (  graspDataSlice[index].interUnitCoherence[CHEST_STRUCTURE] != MISSING_DOUBLE && graspDataSlice[index].interUnitCoherence[CHEST_STRUCTURE] > coherenceThreshold ) {
			chestWarningPanel->BackColor = System::Drawing::Color::IndianRed;
		}
		else chestWarningPanel->BackColor = normalBackgroundColor;
		chestWarningPanel->Update();
		if ( chestPose.visible &&  visibilityViewpointComboBox->SelectedIndex != VISIBILITY_ONLY ) {
			if ( visibilityViewpointComboBox->SelectedIndex == FROM_CODA && alignment_index >= 0 ) LookAtFrom( focusViewpoint, chestPose.pose.position, graspDataSlice[alignment_index].alignmentOffset[unit] );
			else LookAtFrom( focusViewpoint, chestPose.pose.position, front_viewpoint );
			RenderWindow( chestWindow[unit], focusViewpoint, chestMobile );
		}
		else RenderWindow( chestWindow[unit], objectViewpoint, chestStationary );


	}

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
		renderer->projectiles->enabled = ( graspDataSlice[index].enableBits & bit ) != 0; bit = bit << 1;
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

		static	Vector3 projectile_direction;
		static	Pose	projectile_start_pose;
		static double	launch_time = 0.0;
		static double	projectile_speed = 200.0;  
		static bool		first = true;
		if ( !renderer->projectiles->enabled || first ) {
			// Position the projectiles to wherever the tool is now, in preparation for the next launch.
			if ( renderer->vTool->enabled ) {
				vm.CopyPose( projectile_start_pose, graspDataSlice[index].headPose.pose );
				vm.CopyVector( projectile_direction, vm.kVector );
				renderer->projectiles->SetPose( graspDataSlice[index].headPose.pose );
				renderer->projectiles->SetOrientation( graspDataSlice[index].rollQuaternion );

			}
			else {
				vm.CopyPose( projectile_start_pose, graspDataSlice[index].handPose.pose );
				vm.RotateVector( projectile_direction, graspDataSlice[index].handPose.pose.orientation, vm.kVector );
				vm.NormalizeVector( projectile_direction );
				renderer->projectiles->SetPose( graspDataSlice[index].handPose.pose );
			}
			launch_time = graspDataSlice[index].absoluteTime;
			first = false;
		}
		else {
			first = first;
		}

		Vector3 new_position, delta;
		double  duration;
		duration = graspDataSlice[index].absoluteTime - launch_time;
		vm.ScaleVector( delta, projectile_direction, - duration * projectile_speed );
		vm.AddVectors( new_position, projectile_start_pose.position, delta );
		renderer->projectiles->SetPosition( new_position );

		// fOutputDebugString( "Duration: %f  Delta: %s\n", duration, vm.vstr( delta ) );

		vrSubjectViewpoint->SetPose( graspDataSlice[index].headPose.pose );
		renderer->hmd->SetPose( graspDataSlice[index].headPose.pose );
		renderer->hand->SetPose( graspDataSlice[index].handPose.pose );
		renderer->orientationTarget->SetOrientation( graspDataSlice[index].targetOrientation, 0.0, 0.0 );
		RenderSubjectView( vrSubjectWindow, vrSubjectViewpoint, true );
		renderer->tunnel->enabled = true;
		renderer->glasses->enabled = false;
		renderer->head->SetPose( graspDataSlice[index].headPose.pose );

		RenderSideView( vrSideWindow, vrSideViewpoint, true );

	}	
	else {
		RenderSubjectView( vrSubjectWindow, vrSubjectViewpoint, false );
		RenderSubjectView( vrSideWindow, vrSideViewpoint, false );
	}

	PlotCursor();
}


