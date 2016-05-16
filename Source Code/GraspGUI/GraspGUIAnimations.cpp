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

#include <Windows.h>
#include <mmsystem.h>

#include "../GraspVR/GraspGLObjects.h"
#include "../Trackers/PoseTrackers.h"
#include "OpenGLWindowsInForms.h"
#include "GraspDesktopForm.h"

using namespace GraspGUI;


// Initialize the objects used to show the status on the screen.
void GraspDesktop::InitializeAnimations( void ) {

	static double coda_distance = 2500.0;
		
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
 	codaViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
	codaViewpoint->SetPosition( 0.0, 0.0, - coda_distance );
	codaViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

	// Initialize the state of the GL graphics engine.
	// Some of this will be overridden by the object renderer.
	glUsefulInitializeDefault();
	glUsefulDefaultSpecularLighting( 0.75 );

	// Create the OpenGLOjbects that are going to be displayed.
	// If they use textures, those textures need to be preloaded into the OpenGL rendering context.
	// We therefore activate the HMD window. Since the other windows share the same rendering 
	//  context, this is sufficient for the RC state to be initialized for all windows.
	hmdWindow->Activate();
	objectRenderer = new GraspGLObjects();
	objectRenderer->SetLighting();
	objectRenderer->CreateVRObjects();

	CreateRefreshTimer( 20 );

}


// Draw the objects used to show the status on the screen.
void GraspDesktop::RefreshAnimations( void ) {

	static float dynamic_object_background[4] = { 0.65f, 0.85f, 0.65f, 1.0f };
	static float static_object_background[4] = { 0.65f, 0.65f, 0.85f, 1.0f };

	TrackerPose head_pose;
	TrackerPose hand_pose;
	// TrackerPose torso_pose;

	// Need to handle vectors but we cannot mix in the VectorsMixin class
	// because of incompatibility, I think, with CLR.
	// So I create an object here to handle it. 
	static VectorsMixin vp;

	// Simulate movements of the head and hand.
	// This will later be handled by the tracker.
	// Create an oscillating angle.
	static double pseudo_time = 0.0;
	double angle = cos( pseudo_time );
	pseudo_time += 0.05;

	vp.SetQuaternion( head_pose.pose.orientation, 0.1 * angle, vp.jVector );
	vp.SetQuaternion( hand_pose.pose.orientation, 0.2 * angle, vp.kVector );

	// Head is at the origin.
	vp.CopyVector( head_pose.pose.position, vp.zeroVector );

	// Place the hand at shoulder level.
	static double arms_length = 700.0;
	static double shoulder_drop = 300.0;
	hand_pose.pose.position[X] = 0.0;
	hand_pose.pose.position[Y] = - shoulder_drop;
	hand_pose.pose.position[Z] = - arms_length;

	// Show what the subject is seeing.
	hmdWindow->Activate();
	hmdWindow->Clear();
	glUsefulPrepareRendering();
	hmdViewpoint->SetOrientation( head_pose.pose.orientation );
	hmdViewpoint->SetPosition( head_pose.pose.position );
	hmdViewpoint->Apply( hmdWindow, CYCLOPS );
	objectRenderer->DrawStarrySky();
	objectRenderer->DrawRoom();
	objectRenderer->DrawOrientationTarget();
	objectRenderer->DrawPositionOnlyTarget();
	objectRenderer->DrawTiltPrompt();
	objectRenderer->DrawTool( &hand_pose );
	hmdWindow->Swap();

	// Show what is going on from a fixed viewpoint into the 3D workspace.
	workspaceWindow->Activate();
	workspaceWindow->Clear( 0.0, 0.0, 0.0 );
	glUsefulPrepareRendering();
	workspaceViewpoint->Apply( workspaceWindow, CYCLOPS );
	objectRenderer->DrawStarrySky();
	objectRenderer->DrawRoom();
	objectRenderer->DrawTool( &hand_pose );
	objectRenderer->DrawOrientationTarget();
	objectRenderer->DrawPositionOnlyTarget();
	objectRenderer->DrawTiltPrompt();
	objectRenderer->DrawBody( &head_pose );
	workspaceWindow->Swap();

	hmdDynamicWindow->Activate();
	hmdDynamicWindow->Clear( dynamic_object_background );
	codaViewpoint->Apply( hmdDynamicWindow, CYCLOPS );
	vp.CopyVector( head_pose.pose.position, vp.zeroVector );
	objectRenderer->DrawHead(  &head_pose );
	hmdDynamicWindow->Swap();

	toolDynamicWindow->Activate();
	toolDynamicWindow->Clear( dynamic_object_background );
	codaViewpoint->Apply( toolDynamicWindow, CYCLOPS );
	vp.CopyVector( hand_pose.pose.position, vp.zeroVector );
	objectRenderer->DrawTool( &hand_pose );
	toolDynamicWindow->Swap();

	torsoDynamicWindow->Activate();
	torsoDynamicWindow->Clear( dynamic_object_background );
	codaViewpoint->Apply( torsoDynamicWindow, CYCLOPS );
	objectRenderer->DrawTorso();
	torsoDynamicWindow->Swap();

	hmdStaticWindow->Activate();
	hmdStaticWindow->Clear( static_object_background );
	codaViewpoint->Apply( hmdStaticWindow, CYCLOPS );
	objectRenderer->DrawHead(  &NullTrackerPose );
	hmdStaticWindow->Swap();

	toolStaticWindow->Activate();
	toolStaticWindow->Clear( static_object_background );
	codaViewpoint->Apply( toolStaticWindow, CYCLOPS );
	objectRenderer->DrawTool( &NullTrackerPose );
	toolStaticWindow->Swap();

	torsoStaticWindow->Activate();
	torsoStaticWindow->Clear( static_object_background );
	codaViewpoint->Apply( torsoStaticWindow, CYCLOPS );
	objectRenderer->DrawTorso( &NullTrackerPose );
	torsoStaticWindow->Swap();

}

void GraspDesktop::KillAnimations( void ) {
	hmdWindow->Destroy();
}
