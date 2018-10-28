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
#include "../Grasp/Grasp.h"

#include "../GraspMMIUtilities/GraspData.h"
#include "../GraspMMIUtilities/GraspMMIUtilities.h"

// We make use of a package of plotting routines that I have had around for decades.
#include "..\PsyPhy2dGraphicsLib\OglDisplayInterface.h"
#include "..\PsyPhy2dGraphicsLib\OglDisplay.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"

#include "GraspMMIGraphsForm.h"
#include "GraspMMIDataPlotsStartup.h"

using namespace GraspMMI;

#define POSE_STRIPCHARTS	8
#define MARKER_STRIPCHARTS	MARKER_STRUCTURES * 3			// Separate XYZ for each marker structure
#define VISIBILITY_STRIPCHARTS	MARKER_STRUCTURES
#define HISTORY_STRIPCHARTS	1
#define MAX_PLOT_STEP PACKET_STREAM_BREAK_INSERT_SAMPLES	// Maximum down sampling to display data.
#define MAX_PLOT_SAMPLES (3 * 60 * 20)						// Max samples to plot at one time.
#define MAX_TILT_SAMPLES 200

// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

// Trial parameters.
// I have to put them here as statics because a managed class
// cannot have mixed types.
struct {
	int		trial;
	char	paradigm[16];
	double	target_head_tilt;
	double	target_head_tilt_tolerance;
	double	target_head_tilt_duration;
	double	target_orientation;
	double	target_orientation_tolerance;
	double	target_duration;
	double	response_head_tilt;
	double	response_head_tilt_tolerance;
	double	response_head_tilt_duration;
	double	response_timeout;
	double	conflict_gain;
	int		feedback;
	double	time;
	char	*response;
} trial_parameters[MAX_GRASP_TRIALS];
int n_trials;

void GraspMMIGraphsForm::ComputeIndividualMarkerVisibility( GraspRealtimeDataSlice *slice, int n_slices ) {

	static VectorsMixin vm;

	for ( int i = 0; i < n_slices; i++ ) {

		// Compute the pose, the fidelity and the marker visibility for each unit.
		for ( int unit = 0; unit < MAX_UNITS; unit++  ) {
			hmdTracker->ComputePose( slice[i].unitHMD[unit], &slice[i].codaFrame[unit], true );
			slice[i].visibleMarkers[unit][HMD_STRUCTURE] = 
				hmdTracker->VisibleMarkers( &slice[i].codaFrame[unit] );
			handTracker->ComputePose( slice[i].unitHand[unit], &slice[i].codaFrame[unit], true );
			slice[i].visibleMarkers[unit][HAND_STRUCTURE] = 
				handTracker->VisibleMarkers( &slice[i].codaFrame[unit] );
			chestTracker->ComputePose( slice[i].unitChest[unit], &slice[i].codaFrame[unit], true );
			slice[i].visibleMarkers[unit][CHEST_STRUCTURE] = 
				chestTracker->VisibleMarkers( &slice[i].codaFrame[unit] );

			for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
				if ( ! slice[i].codaFrame[unit].marker[mrk].visibility ) {
					slice[i].codaFrame[unit].marker[mrk].position[X] =
						slice[i].codaFrame[unit].marker[mrk].position[Y] =
						slice[i].codaFrame[unit].marker[mrk].position[Z] = MISSING_DOUBLE;
				}
			}
		}
	}
	
	// Compute a check on the coherence of the position data between the coda units.
	// This can be used to detect if the data from the codas is out of sync.
	// We use the position of the objects, rather than the position of individual markers,
	// because the computation of the object pose rejects reflections and other anomalies
	// that can affect a single marker.
	// The positions are filtered because otherwise skew in time creates incoherence
	// between the unit-specific poses in the real-time data because each telemetry record
	// contains a marker frame from only one unit.
	Vector3 filtered_hmd[MAX_UNITS];
	Vector3 filtered_hand[MAX_UNITS];
	Vector3 filtered_chest[MAX_UNITS];
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
		vm.CopyVector( filtered_hmd[unit], vm.zeroVector );
		vm.CopyVector( filtered_hand[unit], vm.zeroVector );
		vm.CopyVector( filtered_chest[unit], vm.zeroVector );
	}

	for ( int i = 0; i < n_slices; i++ ) {

		if (   slice[i].absoluteTime != MISSING_DOUBLE && slice[i].unitHMD[0].visible && slice[i-2].unitHMD[0].visible && slice[i].unitHMD[1].visible ) {
		for ( int unit = 0; unit < 2; unit++ ) {
				vm.ScaleVector( filtered_hmd[unit], filtered_hmd[unit], coherenceFilterConstant );
				vm.AddVectors( filtered_hmd[unit], filtered_hmd[unit], slice[i].unitHMD[unit].pose.position );
				vm.ScaleVector( filtered_hmd[unit], filtered_hmd[unit], 1.0 / (coherenceFilterConstant + 1.0) );
			}
			Vector3 delta;
			vm.SubtractVectors( delta, filtered_hmd[0], filtered_hmd[1] );
			slice[i].interUnitCoherence[HMD_STRUCTURE] = vm.VectorNorm( delta );
		}
		else slice[i].interUnitCoherence[HMD_STRUCTURE] = MISSING_DOUBLE;
		if (  slice[i].absoluteTime != MISSING_DOUBLE && slice[i].unitHand[0].visible && slice[i].unitHand[1].visible ) {
			for ( int unit = 0; unit < 2; unit++ ) {
				vm.ScaleVector( filtered_hand[unit], filtered_hand[unit], coherenceFilterConstant );
				vm.AddVectors( filtered_hand[unit], filtered_hand[unit], slice[i].unitHand[unit].pose.position );
				vm.ScaleVector( filtered_hand[unit], filtered_hand[unit], 1.0 / (coherenceFilterConstant + 1.0) );
			}
			Vector3 delta;
			vm.SubtractVectors( delta, filtered_hand[0], filtered_hand[1] );
			slice[i].interUnitCoherence[HAND_STRUCTURE] = vm.VectorNorm( delta );
		}
		else slice[i].interUnitCoherence[HAND_STRUCTURE] = MISSING_DOUBLE;
		if (  slice[i].absoluteTime != MISSING_DOUBLE && slice[i].unitChest[0].visible && slice[i].unitChest[1].visible ) {
			for ( int unit = 0; unit < 2; unit++ ) {
				vm.ScaleVector( filtered_chest[unit], filtered_chest[unit], coherenceFilterConstant );
				vm.AddVectors( filtered_chest[unit], filtered_chest[unit], slice[i].unitChest[unit].pose.position );
				vm.ScaleVector( filtered_chest[unit], filtered_chest[unit], 1.0 / (coherenceFilterConstant + 1.0) );
			}
			Vector3 delta;
			vm.SubtractVectors( delta, filtered_chest[0], filtered_chest[1] );
			slice[i].interUnitCoherence[CHEST_STRUCTURE] =vm.VectorNorm( delta );
		}
		else slice[i].interUnitCoherence[CHEST_STRUCTURE] = MISSING_DOUBLE;
	}

}


// Read data stored by the GRASP application.
bool GraspMMIGraphsForm::ReadGraspData( String^ root ) {

	static VectorsMixin vm;

	char line[2048];
	char *path;
	FILE *fid;

	// Parse the filename for the subject, subsession and session IDs.
	// Note that we don't care too much. So if the filename is not standard
	// it should be OK. 
	double	subject = 0;
	double	protocol = 0;
	double	task = 0;

	String^ name_only = root->Substring( root->LastIndexOf( "\\" ) + 1 );
	char *ptr = (char*)(void*)Marshal::StringToHGlobalAnsi( name_only ).ToPointer();
	sscanf( ptr, "S%lf_%lf_%lf", &subject, &protocol, &task );
	Marshal::FreeHGlobal( IntPtr( ptr ) );

	String^ response_filename = root + ".rsp";

	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	path = (char*)(void*)Marshal::StringToHGlobalAnsi( response_filename ).ToPointer();


	fid = fopen( path, "r" );
	// Read and ignore header.
	fgets( line, sizeof( line ), fid );
	for ( n_trials = 0; n_trials < MAX_GRASP_TRIALS; n_trials++ ) {
		if ( ! fgets( line, sizeof( line ), fid ) ) break;
		sscanf( line, "%d; %s %lf; %lf; %lf; %lf; %lf; %lf; %lf; %lf; %lf; %lf; %lf; %d; %lf;",
			&trial_parameters[ n_trials ].trial,
			trial_parameters[ n_trials ].paradigm,
			&trial_parameters[ n_trials ].target_head_tilt,
			&trial_parameters[ n_trials ].target_head_tilt_tolerance,
			&trial_parameters[ n_trials ].target_head_tilt_duration,
			&trial_parameters[ n_trials ].target_orientation,
			&trial_parameters[ n_trials ].target_orientation_tolerance,
			&trial_parameters[ n_trials ].target_duration,
			&trial_parameters[ n_trials ].response_head_tilt,
			&trial_parameters[ n_trials ].response_head_tilt_tolerance,
			&trial_parameters[ n_trials ].response_head_tilt_duration,
			&trial_parameters[ n_trials ].response_timeout,
			&trial_parameters[ n_trials ].conflict_gain,
			&trial_parameters[ n_trials ].feedback,
			&trial_parameters[ n_trials ].time );
			trial_parameters[ n_trials ].response = "IGNORE";

	}
	fclose( fid );
	Marshal::FreeHGlobal( IntPtr( path ) );

	/// Read the poses and marker data.
	String^ pose_filename = root + ".pse";

	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	path = (char*)(void*)Marshal::StringToHGlobalAnsi( pose_filename ).ToPointer();
	fid = fopen( path, "r" );
	// Read and ignore header.
	fgets( line, sizeof( line ), fid );

	double start_time = 36.0 * 60 * 60;
	for ( nDataSlices = 0, nHousekeepingSlices = 0; nDataSlices < MAX_SLICES; nDataSlices++, nHousekeepingSlices++ ) {

		int trial;
		int state;
		double time;
		TrackerPose pose;

		int result;
		bool success;

		int preceeding_state = 0;

		result = fscanf( fid, "%d;", &trial );
		if ( result < 1 ) break;
		result = fscanf( fid, "%lf;", &time );
		if ( result < 1 ) break;
		result = fscanf( fid, "%d;", &state );
		if ( result < 1 ) break;

		success = ReadTrackerPose( graspDataSlice[ nDataSlices ].headPose, fid );
		if ( !success ) break;
		fscanf( fid, " ; " );
		success = ReadTrackerPose( graspDataSlice[ nDataSlices ].handPose, fid );
		if ( !success ) break;
		fscanf( fid, " ; " );
		success = ReadTrackerPose( graspDataSlice[ nDataSlices ].chestPose, fid );
		if ( !success ) break;
		fscanf( fid, " ; " );
		success = ReadTrackerPose( pose, fid );
		if ( !success ) break;
		fscanf( fid, " ; " );
		vm.CopyQuaternion( graspDataSlice[ nDataSlices ].rollQuaternion, pose.pose.orientation );

		for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
			if ( ! ReadMarkerFrame( graspDataSlice[ nDataSlices ].codaFrame[unit], fid ) ) break;
		}

		graspDataSlice[ nDataSlices ].absoluteTime = time + start_time;

		// For each marker structure, compute the pose using one CODA, then if it is not visible, try the other.
		// This is what the cascade trackers are doing on board. But it's not clear which comes first onboard.
		hmdTracker->ComputePose( graspDataSlice[ nDataSlices ].HMD, &graspDataSlice[ nDataSlices ].codaFrame[0] );
		if ( !graspDataSlice[ nDataSlices ].HMD.visible ) hmdTracker->ComputePose( graspDataSlice[ nDataSlices ].HMD, &graspDataSlice[ nDataSlices ].codaFrame[1] );
		if ( !graspDataSlice[ nDataSlices ].HMD.visible ) vm.CopyPose( graspDataSlice[ nDataSlices ].HMD.pose, vm.missingPose );
		handTracker->ComputePose( graspDataSlice[ nDataSlices ].hand, &graspDataSlice[ nDataSlices ].codaFrame[0] );
		if ( !graspDataSlice[ nDataSlices ].hand.visible ) handTracker->ComputePose( graspDataSlice[ nDataSlices ].hand, &graspDataSlice[ nDataSlices ].codaFrame[1] );
		if ( !graspDataSlice[ nDataSlices ].hand.visible ) vm.CopyPose( graspDataSlice[ nDataSlices ].hand.pose, vm.missingPose );
		chestTracker->ComputePose( graspDataSlice[ nDataSlices ].chest, &graspDataSlice[ nDataSlices ].codaFrame[0] );
		if ( !graspDataSlice[ nDataSlices ].chest.visible ) chestTracker->ComputePose( graspDataSlice[ nDataSlices ].chest, &graspDataSlice[ nDataSlices ].codaFrame[1] );
		if ( !graspDataSlice[ nDataSlices ].chest.visible ) vm.CopyPose( graspDataSlice[ nDataSlices ].chest.pose, vm.missingPose );

		// It is convenient to compute some derived values here.
		ComputeGraspRTDerivedValues( &graspDataSlice[ nDataSlices ] );

		// The file was written by Grasp.exe, so all records are of that type.
		graspDataSlice[ nDataSlices ].clientType = GraspRealtimeDataSlice::GRASP;

		// The pse file unfortunately does not include the enable bits of the VR objects.
		// We can try to deduce them from the state variable.

		switch ( state ) {

		case StartBlock:
			graspDataSlice[ nDataSlices ].enableBits = VK_TOOL;
			graspDataSlice[ nDataSlices ].spinnerBits = READY_TO_START_INDICATOR;
			break;

		case StartTrial:
			graspDataSlice[ nDataSlices ].enableBits = 0;
			graspDataSlice[ nDataSlices ].spinnerBits = READY_TO_START_INDICATOR;
			break;

		case StraightenHead:
			graspDataSlice[ nDataSlices ].enableBits = 
				STARRY_SKY | STRAIGHT_AHEAD_TARGET | K_TOOL;
			graspDataSlice[ nDataSlices ].spinnerBits = STRAIGHTEN_HEAD_INDICATOR;
			break;

		case AlignHead:
			graspDataSlice[ nDataSlices ].enableBits = GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case PresentTarget:
			// Always show the visual target and the VK tool, so we can tell what is happening.
			graspDataSlice[ nDataSlices ].enableBits = VK_TOOL | GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY | ORIENTATION_TARGET;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case TiltHead:
			graspDataSlice[ nDataSlices ].enableBits = GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case ObtainResponse:
			// Always show the visual target and the VK tool, so we can tell what is happening.
			graspDataSlice[ nDataSlices ].enableBits = 
				GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY | POSITION_ONLY_TARGET | VK_TOOL | HAND_LASER;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case ProvideFeedback:
			graspDataSlice[ nDataSlices ].enableBits = 
				GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY | PROJECTILES | MULTI_PROJECTILE | ORIENTATION_TARGET;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case TrialCompleted:
			graspDataSlice[ nDataSlices ].enableBits = 
				GRASP_ROOM | GRASP_TUNNEL | STARRY_SKY | SUCCESS_INDICATOR;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
			break;

		case BlockCompleted:
			graspDataSlice[ nDataSlices ].enableBits = VK_TOOL;
			graspDataSlice[ nDataSlices ].spinnerBits = BLOCK_COMPLETED_INDICATOR;
			break;

		case VRCompleted:
			graspDataSlice[ nDataSlices ].enableBits = DARK_SKY | VK_TOOL;
			graspDataSlice[ nDataSlices ].spinnerBits = BLOCK_COMPLETED_INDICATOR;
			break;

		case TrialInterrupted:
			graspDataSlice[ nDataSlices ].enableBits = GRASP_ROOM | DARK_SKY | VK_TOOL;
			switch ( preceeding_state ) {
			case StraightenHead:
				graspDataSlice[ nDataSlices ].spinnerBits = HEAD_ALIGN_TIMEOUT_INDICATOR;
				break;

			case AlignHead:
				graspDataSlice[ nDataSlices ].spinnerBits = HEAD_ALIGN_TIMEOUT_INDICATOR;
				break;

			case PresentTarget:
				graspDataSlice[ nDataSlices ].spinnerBits = HEAD_MISALIGNED_INDICATOR;
				break;

			case TiltHead:
				graspDataSlice[ nDataSlices ].spinnerBits = HEAD_ALIGN_TIMEOUT_INDICATOR;
				break;

			case ObtainResponse:
				graspDataSlice[ nDataSlices ].spinnerBits = RESPONSE_TIMEOUT_INDICATOR;
				break;

			case ProvideFeedback:
				graspDataSlice[ nDataSlices ].spinnerBits = MANUAL_REJECT_INDICATOR;
				break;


			default:
				graspDataSlice[ nDataSlices ].spinnerBits = INTERRUPT_INDICATOR;
				break;
			}
			break;

		case Demo:
			graspDataSlice[ nDataSlices ].enableBits = GRASP_ROOM | STARRY_SKY | VK_TOOL;
			graspDataSlice[ nDataSlices ].spinnerBits = DEMO_STATE_INDICATOR;
			break;

		default:
			graspDataSlice[ nDataSlices ].enableBits = 
				ORIENTATION_TARGET | VK_TOOL | GRASP_TUNNEL | GRASP_ROOM | DARK_SKY;
			graspDataSlice[ nDataSlices ].spinnerBits = 0;
		}
		graspDataSlice[ nDataSlices ].targetOrientation = trial_parameters[trial].target_orientation;

		graspHousekeepingSlice[nHousekeepingSlices].absoluteTime = time + start_time;
		graspHousekeepingSlice[nHousekeepingSlices].userID = subject;
		graspHousekeepingSlice[nHousekeepingSlices].protocolID = protocol;
		graspHousekeepingSlice[nHousekeepingSlices].taskID = task;
		graspHousekeepingSlice[nHousekeepingSlices].stepID = -1;
		graspHousekeepingSlice[nHousekeepingSlices].scriptEngine = -1;

		//Do what GraspDexTrackers::GetTrackerStatus() does to compute the HK visibility.
		unsigned int status = 0;
		int group, id, mrk, unit;
		for ( group = 0, id = 0; group < 3 && id < MAX_MARKERS; group ++ ) {
			int group_count = 0;
			for ( mrk = 0; mrk < 8 && id < MAX_MARKERS; mrk++ , id++ ) {
				bool visibility = false;
				for ( unit = 0; unit < MAX_UNITS; unit++ ) {
					if ( graspDataSlice[ nDataSlices ].codaFrame[unit].marker[id].visibility ) visibility = true;
				}
				if ( visibility ) group_count++;
			}
			graspHousekeepingSlice[nHousekeepingSlices].visibleMarkers[group] = group_count;
		}

		preceeding_state = state;

	}
	ComputeIndividualMarkerVisibility( graspDataSlice, nDataSlices );
	fOutputDebugString( "graspDataSlices %d of %d\n", nDataSlices, MAX_SLICES );
	Marshal::FreeHGlobal( IntPtr( path ) );

	return true;

}

// Read the cached packets.
void GraspMMIGraphsForm::ReadTelemetryCache( String^ root ) {
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *filename_root = (char*)(void*)Marshal::StringToHGlobalAnsi( root ).ToPointer();
	nDataSlices = GetGraspRT( graspDataSlice, MAX_SLICES, filename_root );
	nHousekeepingSlices = GetHousekeepingTrace( graspHousekeepingSlice, MAX_SLICES, filename_root );
	ComputeIndividualMarkerVisibility( graspDataSlice, nDataSlices );
	Marshal::FreeHGlobal( IntPtr(filename_root) );
}

// Initialize the objects used to plot the data on the screen.
void GraspMMIGraphsForm::InitializeGraphics( void ) {

	HWND	parent;

	// Each Display will draw into a defined subwindow. 
	// The View code defines 'Displays' as regions of a window and
	// 'Views' as sub-regions with define limits in user coordinates.

	// All the code below creates the link between the Display object
	//  and the corresponding Windows pane, setting the size of the 
	//  Display object to that of the pane.
	// More of this should be done inside the routine that creates
	//  the Display, but for historical purposes it is done here.

	// Then the Views are defined with respect to each Display and the 
	//  limits in user coordinates are initialized. 

	parent = static_cast<HWND>( cursorPanel->Handle.ToPointer());
	cursorDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( cursorDisplay, cursorPanel->Size.Width, cursorPanel->Size.Height );
	DisplaySetScreenPosition( cursorDisplay, 0, 0 );
	DisplayInit( cursorDisplay );
	DisplayErase( cursorDisplay );
	cursorLayout = CreateLayout( cursorDisplay, 1, 1 );
	LayoutSetDisplayEdgesRelative( cursorLayout, 0.0, 0.0, 1.0, 1.0 );

	// Pose Strip Charts
	parent = static_cast<HWND>( poseGraphPanel->Handle.ToPointer());
	poseDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( poseDisplay, poseGraphPanel->Size.Width, poseGraphPanel->Size.Height );
	DisplaySetScreenPosition( poseDisplay, 0, 0 );
	DisplayInit( poseDisplay );
	DisplayErase( poseDisplay );
	poseStripChartLayout = CreateLayout( poseDisplay, POSE_STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( poseStripChartLayout, 0.0, 0.0, 1.0, 1.0 );
	markerStripChartLayout = CreateLayout( poseDisplay, MARKER_STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( markerStripChartLayout, 0.0, 0.0, 1.0, 1.0 );

	// Marker Visibility Strip Charts
	parent = static_cast<HWND>( markerGraphPanel->Handle.ToPointer());
	visibilityDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( visibilityDisplay, markerGraphPanel->Size.Width, markerGraphPanel->Size.Height );
	DisplaySetScreenPosition( visibilityDisplay, 0, 0 );
	DisplayInit( visibilityDisplay );
	DisplayErase( visibilityDisplay );
	visibilityStripChartLayout = CreateLayout( visibilityDisplay, VISIBILITY_STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( visibilityStripChartLayout, 0.0, 0.0, 1.0, 1.0 );

	// Task History Strip Chart
	parent = static_cast<HWND>( taskGraphPanel->Handle.ToPointer());
	historyDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( historyDisplay, taskGraphPanel->Size.Width, taskGraphPanel->Size.Height );
	DisplaySetScreenPosition( historyDisplay, 0, 0 );
	DisplayInit( historyDisplay );
	DisplayErase( historyDisplay );
	historyStripChartLayout = CreateLayout( historyDisplay, HISTORY_STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( historyStripChartLayout, 0.0, 0.0, 1.0, 1.0 );

}

// The GUI has a scroll bar allowing one to look back at different parts of the data and
// a slider that determines the time span of the data window. Here we determine the time span 
// of the available data, set the limits on the scroll bar accordingly, and show the limits
// in clock format under the scroll bar.
void GraspMMIGraphsForm::AdjustScrollSpan( void ) {

	double min, max;
	unsigned long	i;

	// The time span of data to plot is determined by the slider.
	double span = windowSpanSeconds[spanSelector->Value];

	if ( nDataSlices < 2 && nHousekeepingSlices < 2 ) {
		min = 0.0;
		max = 1.0;
	}
	else {
		// Find the time window of the available data packets.
		// The global array RealMarkerTime[] has been filled previously.
		min = DBL_MAX;
		for ( i = 0; i < nDataSlices; i++ ) {
			if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) {
				min = graspDataSlice[i].absoluteTime;
				break;
			}
		}
		for ( i = 0; i < nHousekeepingSlices; i++ ) {
			if ( graspHousekeepingSlice[i].absoluteTime != MISSING_DOUBLE ) {
				if ( graspHousekeepingSlice[i].absoluteTime < min ) min = graspHousekeepingSlice[i].absoluteTime;
				break;
			}
		}
		max = span;
		for ( i = nDataSlices - 1; i > 0 && nDataSlices > 0; i-- ) {
			if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) break;
		}
		if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE && graspDataSlice[i].absoluteTime > max ) max = graspDataSlice[i].absoluteTime;
		for ( i = nHousekeepingSlices - 1; i > 0 && nHousekeepingSlices > 0; i-- ) {
			if ( graspHousekeepingSlice[i].absoluteTime != MISSING_DOUBLE ) break;
		}
		if ( graspHousekeepingSlice[i].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[i].absoluteTime > max ) max = graspHousekeepingSlice[i].absoluteTime;	
	}

	// Adjust the behavior of the scroll bar depending on the selected 
	// time span of the data window. A large step moves a full window
	// width, a small step moves 1/10th of the window.
	scrollBar->LargeChange = (int) span;
	scrollBar->SmallChange = (int) (span / 10.0);
	// Set the scroll bar limits to match the limits of the available data.
	// Note that you cannot not reach the max value of the scroll bar with the user
	// controls. Best you can do is to get within LargeChange of the maximum. 
	// We extend the range of the scroll bar by that value so that one can
	//  reach the end of the data.
	int top = (int) ceil( max ) + scrollBar->LargeChange;
	int bottom = (int) floor( min );
	if ( bottom >= floor( max ) ) bottom = (int) floor( max );
	scrollBar->Maximum = top;
	scrollBar->Minimum = bottom;

	// Label the two ends of the scroll bar.
	lastAbsoluteTimeTextBox->Text = CreateTimeString( max );
	firstAbsoluteTimeTextBox->Text = CreateTimeString( min );
}

// When the display is 'live' we want to be able to automatically position the scroll bars 
// so as to display the most recent data.
void GraspMMIGraphsForm::MoveToLatest( void ) {
	// Find the timestamp of the latest sample in the series.
	// Need to check both RT science and housekeeping traces.
	double latest;
	for ( int i = nDataSlices - 1; i >= 0; i-- ) {
		if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) {
			latest = graspDataSlice[i].absoluteTime;
			break;
		}
	}
	for ( int i = nHousekeepingSlices - 1; i >= 0; i-- ) {
		if ( graspHousekeepingSlice[i].absoluteTime != MISSING_DOUBLE ) {
			if ( graspHousekeepingSlice[i].absoluteTime > latest ) latest = graspHousekeepingSlice[i].absoluteTime;
			break;
		}
	}
	// Adjust the slider to match the latest epoch. 
	scrollBar->Value = (int) ceil( latest );
	// Point the VR display to the latest sample.
	playbackScrollBar->Value = playbackScrollBar->Maximum;
	MoveToInstant( playbackScrollBar->Value );
}

/// 
/// Ploting of various time series.
///

// Here we do the actual work of plotting the strip charts and phase plots.
// It is assumed that the global data arrays have been filled. The time span
// of the plots is determined by the scroll bar and span slider.

void GraspMMIGraphsForm::PlotPoses( double first_instant, double last_instant ) {

	::View view;

	// Find the indices into the arrays that correspond to the time window.
	int first_rt_sample;
	int last_rt_sample;
	int index;
	for ( index = nDataSlices - 1; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime <= last_instant ) break;
	}
	last_rt_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < first_instant ) break;
	}
	first_rt_sample = index + 1;

	// Subsample the data if there is a lot to be plotted.
	int step = 1;
	while ( ((last_rt_sample - first_rt_sample) / step) > MAX_PLOT_SAMPLES && step < (MAX_PLOT_STEP - 1) ) step *= 2;

	int tilt_step;
	tilt_step = ( last_rt_sample - first_rt_sample ) / MAX_TILT_SAMPLES;
	if ( tilt_step < 1 ) tilt_step = 1;

	// Plot the continous data about the head position and orientation.
	DisplayActivate( poseDisplay );
	DisplayErase( poseDisplay );
	int row = 0;

	//
	// Head Pose
	//
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[HMD_STRUCTURE]);
	ViewBox( view );

	ViewSetXLimits( view, first_instant, last_instant );

	// Plot coherency between coda1-computed pose and coda2-computed pose.
	ViewSetColorRGB( view, 1.0, 0.5, 0.5 );
	ViewSetYLimits( view, coherencePlotMinimum, coherencePlotMaximum );
	ViewFillPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].interUnitCoherence[HMD_STRUCTURE], 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);


	ViewSetYLimits( view, - headPositionRadius, headPositionRadius );
	// Set the plotting limits for the position data.
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( int i = first_rt_sample + 1; i < last_rt_sample; i++ ) {
			for ( int j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.position[j] > max ) max = graspDataSlice[i].HMD.pose.position[j];
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.position[j] < min ) min = graspDataSlice[i].HMD.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - headPositionRadius, headPositionRadius );

	for ( int i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotClippedDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].HMD.pose.position[i], 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, BLACK );
	ViewTitle( view, "HMD Position", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[HMD_STRUCTURE]);
	ViewBox( view );
	ViewColor( view, GREY7 );
	ViewHorizontalLine( view, 0.0 );

	ViewSetXLimits( view, first_instant, last_instant );
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( int i = first_rt_sample + 1; i < last_rt_sample; i++ ) {
			for ( int j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.orientation[j] > max ) max = graspDataSlice[i].HMD.pose.orientation[j];
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.orientation[j] < min ) min = graspDataSlice[i].HMD.pose.orientation[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - quaternionRadius, quaternionRadius );

	for ( int i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotClippedDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].HMD.pose.orientation[i], 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, BLACK );
	ViewTitle( view, "HMD Orientation", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	//
	// Plot the amplitude of the head rotation away from straight ahead.
	// Ideally, this should only be around the roll axis. But that
	// might not be the case. So we compute the total angle and plot it
	// as an enveloppe (+ and -) in grey. Then we plot the computed roll angle
	// (which is a signed value) on top of this in red. If the roation of the 
	// HMD is indeed primarily around the roll axis, then the red line
	// will fall on top of either the positive or negative grey line. If 
	// there is a lot of pitch or yaw, there will be a discrepancy between
	// the red and grey lines.
	//

	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[HMD_STRUCTURE]);
	ViewBox( view );
	double angle_max = - DBL_MAX;
	ViewSetXLimits( view, first_instant, last_instant );
	if ( autoscaleHMD->Checked ) {
		autoscaleIndicator->Visible = true;
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		for ( int i = first_rt_sample + 1; i < last_rt_sample; i++ ) {
			for ( int j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].HMD.visible && fabs( graspDataSlice[i].hmdRotationAngle ) > angle_max ) angle_max = fabs( graspDataSlice[i].hmdRotationAngle );
			}
		}
	}
	else {
		angle_max = rotationRadius;
		autoscaleIndicator->Visible = false;
	}

	// Negative Enveloppe Border
	ViewSetYLimits( view, angle_max, - angle_max );
	ViewColor( view, GREY6 );
	ViewXYPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	// Positive Enveloppe Border
	ViewSetYLimits( view, - angle_max, angle_max );
	ViewColor( view, GREY6 );
	ViewXYPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	// Roll Angle
	ViewColor( view, MAGENTA );
	ViewXYPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRollAngle, 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, BLACK );
	ViewTitle( view, "HMD Rotation from Zero", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	// Plot the roll angle of the head as a stick figure.
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewSetXLimits( view, first_instant, last_instant );
	ViewColor( view, color_by_object[HMD_STRUCTURE]);
	ViewBox( view );
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRollAngle, 
		first_rt_sample, last_rt_sample - 1, tilt_step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);

	// Overlay the residuals.
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
		// The following is a trick to invert one of the two traces.
		ViewSetYLimits( view, - pow( -1.0, unit ) * residualPlotMaximum, pow( -1.0, unit ) * residualPlotMaximum );
		ViewSetColorRGB( view, color_by_unit[unit][0], color_by_unit[unit][1], color_by_unit[unit][2] );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].unitHMD[unit].fidelity, 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, GREY4 );
	ViewHorizontalLine( view, 0.0 );
	ViewColor( view, BLACK );
	ViewTitle( view, "HMD Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );


	//
	// Hand Pose
	//
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[HAND_STRUCTURE]);
	ViewBox( view );
	ViewSetXLimits( view, first_instant, last_instant );

	// Plot coherency between coda1-computed pose and coda2-computed pose.
	ViewSetColorRGB( view, 1.0, 0.5, 0.5 );
	ViewSetYLimits( view, coherencePlotMinimum, coherencePlotMaximum );
	ViewFillPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].interUnitCoherence[HAND_STRUCTURE], 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);

	// Set the plotting limits.
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( int i = first_rt_sample + 1; i < last_rt_sample; i++ ) {
			for ( int j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].hand.visible && graspDataSlice[i].hand.pose.position[j] > max ) max = graspDataSlice[i].hand.pose.position[j];
				if ( graspDataSlice[i].hand.visible && graspDataSlice[i].hand.pose.position[j] < min ) min = graspDataSlice[i].hand.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - handPositionRadius, handPositionRadius );

	for ( int i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotClippedDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].hand.pose.position[i], 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, BLACK );
	ViewTitle( view, "Hand Position", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	// Plot the roll angle of the hand as a stick figure.
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewSetXLimits( view, first_instant, last_instant );
	ViewColor( view, color_by_object[HAND_STRUCTURE]);
	ViewBox( view );
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].handRollAngle, 
		first_rt_sample, last_rt_sample - 1, tilt_step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	// Overlay the residuals.
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
		// The following is a trick to invert one of the two traces.
		ViewSetYLimits( view, - pow( -1.0, unit ) * residualPlotMaximum, pow( -1.0, unit ) * residualPlotMaximum );
		ViewSetColorRGB( view, color_by_unit[unit][0], color_by_unit[unit][1], color_by_unit[unit][2] );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].unitHand[unit].fidelity, 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, GREY4 );
	ViewHorizontalLine( view, 0.0 );
	ViewColor( view, BLACK );
	ViewTitle( view, "Hand Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	//
	// Chest Pose
	//
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[CHEST_STRUCTURE]);
	ViewBox( view );
	ViewSetXLimits( view, first_instant, last_instant );

	// Plot coherency between coda1-computed pose and coda2-computed pose.
	ViewSetColorRGB( view, 1.0, 0.5, 0.5 );
	ViewSetYLimits( view, coherencePlotMinimum, coherencePlotMaximum );
	ViewFillPlotClippedDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].interUnitCoherence[CHEST_STRUCTURE], 
		first_rt_sample, last_rt_sample - 1, step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);

	// Set the plotting limits.
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( int i = first_rt_sample + 1; i < last_rt_sample; i++ ) {
			for ( int j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].chest.visible && graspDataSlice[i].chest.pose.position[j] > max ) max = graspDataSlice[i].chest.pose.position[j];
				if ( graspDataSlice[i].chest.visible && graspDataSlice[i].chest.pose.position[j] < min ) min = graspDataSlice[i].chest.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - chestPositionRadius, chestPositionRadius );

	for ( int i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotClippedDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].chest.pose.position[i], 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, BLACK );
	ViewTitle( view, "Chest Position", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	// Plot the roll angle of the hand as a stick figure.
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewSetXLimits( view, first_instant, last_instant );
	ViewColor( view, color_by_object[CHEST_STRUCTURE]);
	ViewBox( view );
	ViewColor( view, GREY6 );
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].chestRollAngle, 
		first_rt_sample, last_rt_sample - 1, tilt_step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, color_by_object[CHEST_STRUCTURE]);
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].torsoRollAngle, 
		first_rt_sample, last_rt_sample - 1, tilt_step,
		sizeof( graspDataSlice[first_rt_sample] ), 
		sizeof( graspDataSlice[first_rt_sample] ),
		MISSING_DOUBLE);
	// Overlay the residuals.
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
		// The following is a trick to invert one of the two traces.
		ViewSetYLimits( view, - pow( -1.0, unit ) * residualPlotMaximum, pow( -1.0, unit ) * residualPlotMaximum );
		ViewSetColorRGB( view, color_by_unit[unit][0], color_by_unit[unit][1], color_by_unit[unit][2] );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].unitChest[unit].fidelity, 
			first_rt_sample, last_rt_sample - 1, step,
			sizeof( graspDataSlice[first_rt_sample] ), 
			sizeof( graspDataSlice[first_rt_sample] ),
			MISSING_DOUBLE);
	}
	ViewColor( view, GREY4 );
	ViewHorizontalLine( view, 0.0 );
	ViewColor( view, BLACK );
	ViewTitle( view, "Chest Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	// Zap it to the display.
	DisplaySwap( poseDisplay );
}

void GraspMMIGraphsForm::PlotMarkers( int unit, double first_instant, double last_instant ) {

	::View view;

	int first_unit, last_unit;
	if ( unit == -1 ) {
		first_unit = 0;
		last_unit = MAX_UNITS - 1;
	}
	else first_unit = last_unit = unit;

	// Find the indices into the arrays that correspond to the time window.
	int first_rt_sample;
	int last_rt_sample;
	int index;
	for ( index = nDataSlices - 1; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime <= last_instant ) break;
	}
	last_rt_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < first_instant ) break;
	}
	first_rt_sample = index + 1;

	// Subsample the data if there is a lot to be plotted.
	int step = 1;
	while ( ((last_rt_sample - first_rt_sample) / step) > MAX_PLOT_SAMPLES && step < (MAX_PLOT_STEP - 1) ) step *= 2;

	DisplayActivate( poseDisplay );
	DisplayErase( poseDisplay );

	for ( int object = 0; object < MARKER_STRUCTURES; object++ ) {
		for ( int j = 0; j < 3; j++ ) {
			view = LayoutView( markerStripChartLayout, (MARKER_STRUCTURES - object - 1) * 3 + j, 0 );
			ViewColor( view, color_by_object[object]);
			ViewBox( view );
			ViewSetXLimits( view, first_instant, last_instant );
			ViewSetYLimits( view, -1000.0, 1000.0 );
			for ( int mrk = 0; mrk < tracker[object]->nModelMarkers; mrk++ ) {
				ViewSelectColor( view, mrk );
				int id = tracker[object]->modelMarker[mrk].id;
				for ( int unit = first_unit; unit <= last_unit; unit++ ) {
					ViewXYPlotAvailableDoubles( view, 
						&graspDataSlice[0].absoluteTime, 
						&graspDataSlice[0].codaFrame[unit].marker[id].position[j], 
						first_rt_sample, last_rt_sample - 1, step,
						sizeof( graspDataSlice[first_rt_sample] ), 
						sizeof( graspDataSlice[first_rt_sample] ),
						MISSING_DOUBLE);
				}
			}
		}
	}
	// Zap it to the display.
	DisplaySwap( poseDisplay );

}

void GraspMMIGraphsForm::PlotVisibility( double first_instant, double last_instant ) {

	::View view;

	// Find the indices into the arrays that correspond to the time window.
	int first_rt_sample;
	int last_rt_sample;
	int index;
	for ( index = nDataSlices - 1; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime <= last_instant ) break;
	}
	last_rt_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < first_instant ) break;
	}
	first_rt_sample = index + 1;

	int first_hk_sample;
	int last_hk_sample;
	for ( index = nHousekeepingSlices - 1; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime <= last_instant ) break;
	}
	last_hk_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime < first_instant ) break;
	}
	first_hk_sample = index + 1;

	// Plot how many markers are visible on each marker structure.
	DisplayActivate( visibilityDisplay );
	DisplayErase( visibilityDisplay );
	for ( int object = 0; object < MARKER_STRUCTURES; object++ ) {
		view = LayoutView( visibilityStripChartLayout, MARKER_STRUCTURES - object - 1, 0 );
		ViewSetXLimits( view, first_instant, last_instant );
		ViewSetYLimits( view, 0, 8.0 );
		ViewColor( view, axis_color );
		ViewBox( view );
		ViewSetColorRGB( view, 1.0f, 0.85f, 0.85f );
		ViewHorizontalBand( view, 0.0, 4.0 );

		ViewColor( view, color_by_object[object]);
		ViewFillPlotAvailableDoubles( view,
			&graspHousekeepingSlice[0].absoluteTime, 
			&graspHousekeepingSlice[0].visibleMarkers[object], 
			first_hk_sample, last_hk_sample - 1, 1,
			sizeof( *graspHousekeepingSlice ), 
			sizeof( *graspHousekeepingSlice ),
			MISSING_DOUBLE );

		for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
			ViewSetColorRGB( view, color_by_unit[unit][0], color_by_unit[unit][1], color_by_unit[unit][2] );
			ViewXYPlotAvailableDoubles( view,
				&graspDataSlice[0].absoluteTime, 
				&graspDataSlice[0].visibleMarkers[unit][object], 
				first_rt_sample, last_rt_sample - 1, 1,
				sizeof( *graspDataSlice ), 
				sizeof( *graspDataSlice ),
				MISSING_DOUBLE );
		}

		ViewColor( view, axis_color );
		ViewTitle( view, StructureLabel[object], INSIDE_LEFT, INSIDE_TOP, 0.0 );

	}
	DisplaySwap( visibilityDisplay );
}

void GraspMMIGraphsForm::PlotHistory( double first_instant, double last_instant ) {

	::View view;

	// Find the indices into the arrays that correspond to the time window.
	int index;
	int first_hk_sample;
	int last_hk_sample;
	for ( index = nHousekeepingSlices - 1; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime <= last_instant ) break;
	}
	last_hk_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime < first_instant ) break;
	}
	first_hk_sample = index + 1;
	//
	// Plot which protocol and task was executing at each point in time.
	//
	DisplayActivate( historyDisplay );
	DisplayErase( historyDisplay );

	// Shade alternate bands of 100's to make it easier to understand the data plot.
	int row = 0;
	double bottom = taskViewBottom;
	double top = taskViewTop;
	view = LayoutView( historyStripChartLayout, row++, 0 );
	ViewSetXLimits( view, first_instant, last_instant );
	ViewSetYLimits( view, bottom, top );
	if ( ( top - bottom ) > 100.0 ) {
		ViewColor( view, GREY7 );
		for ( double y = bottom; y < top; y += 200.0 ) ViewHorizontalBand( view, y, y + 100.0 );
	}

	// Plot the protocol data.
	ViewSelectColor( view, 1 );
	ViewScatterPlotAvailableDoubles( view, 
		SYMBOL_FILLED_SQUARE,
		&graspHousekeepingSlice[0].absoluteTime, 
		&graspHousekeepingSlice[0].protocolID, 
		first_hk_sample, last_hk_sample - 1, 1,
		sizeof( *graspHousekeepingSlice ), 
		sizeof( *graspHousekeepingSlice ),
		MISSING_INT);
	ViewSelectColor( view, 2 );
	ViewScatterPlotAvailableDoubles( view, 
		SYMBOL_FILLED_SQUARE,
		&graspHousekeepingSlice[0].absoluteTime, 
		&graspHousekeepingSlice[0].taskID, 
		first_hk_sample, last_hk_sample - 1, 1,
		sizeof( *graspHousekeepingSlice ), 
		sizeof( *graspHousekeepingSlice ),
		MISSING_INT);

	// Label the  bands.
	ViewColor( view, axis_color );
	ViewBox( view );

	double shift_up = 10.0;	// A fudge factor to position the band labels correctly.
	ViewColor( view, axis_color );
	for ( double y = bottom; y < top; y += 200.0 ) {
		char label[16];
		sprintf( label, "%03.0f's", y );
		ViewTitle( view, label, INSIDE_LEFT, UserToDisplayY( view, y + shift_up ), 0.0 );
	}
	for ( double y = bottom + 100.0; y < top; y += 200.0 ) {
		char label[16];
		sprintf( label, "%03.0f's", y );
		ViewTitle( view, label, INSIDE_RIGHT, UserToDisplayY( view, y + shift_up ), 0.0 );
	}

	DisplaySwap( historyDisplay );
	}
void GraspMMIGraphsForm::RefreshGraphics( void ) {


	static bool __debug__ = false;

	static int refresh_count = 0; 

	if ( __debug__ ) fOutputDebugString( "Start RefreshGraphics(). %d\n", refresh_count++ );

	// Determine the time window, in seconds, based on the scroll bar position and the span slider.
	double last_instant = scrollBar->Value;
	double span = windowSpanSeconds[spanSelector->Value];
	double first_instant = last_instant - span;

	// Convert the earliest and latest instants to clock format and display them.
	taskLeftTimeLimit->Text = CreateTimeString( first_instant );
	taskRightTimeLimit->Text = CreateTimeString( last_instant );

	playbackScrollBar->Maximum = ceil( last_instant );
	playbackScrollBar->Minimum = floor( first_instant );
	if ( playbackScrollBar->Value < playbackScrollBar->Minimum ) playbackScrollBar->Value = playbackScrollBar->Minimum;
	if ( playbackScrollBar->Value > playbackScrollBar->Maximum ) playbackScrollBar->Value = playbackScrollBar->Maximum;

	if ( plotSelectionComboBox->SelectedIndex == 0 ) PlotPoses( first_instant, last_instant );
	else if ( plotSelectionComboBox->SelectedIndex == 1 ) PlotMarkers( 0, first_instant, last_instant );
	else if ( plotSelectionComboBox->SelectedIndex == 2 ) PlotMarkers( 1, first_instant, last_instant );
	else if ( plotSelectionComboBox->SelectedIndex == 3 ) PlotMarkers( -1, first_instant, last_instant );
	else {
		DisplayActivate( poseDisplay );
		DisplayErase( poseDisplay );
	}

	PlotVisibility( first_instant, last_instant );
	PlotHistory( first_instant, last_instant );

	if ( __debug__ ) fOutputDebugString( "Finish RefreshGraphics().\n" );

}

// Clean up resources allocated by the Views system.
void GraspMMIGraphsForm::KillGraphics( void ) {
	DisplayClose( cursorDisplay );
	DisplayClose( historyDisplay );
	DisplayClose( visibilityDisplay );
	DisplayClose( poseDisplay );
}


// Parse a line from the script files.
#define PARSER_BUFFERS	16
#define iswhite( c ) ( c <= 0x20 || c >= 0x7f )
int GraspMMIGraphsForm::ParseLine( char *token[MAX_TOKENS], char *line ) {

	static char return_tokens[PARSER_BUFFERS][1024];
	static int circular = 0;

	char *tkn, *chr;
	unsigned int	 n = 0;

	if ( strlen( line ) > sizeof( return_tokens[circular] ) ) {
		fprintf( stderr, "Line too long.\n%s\n", line );
		exit( -1 );
	}
	strcpy( return_tokens[circular], line );
	tkn = return_tokens[circular];
	while ( *tkn && n < MAX_TOKENS - 1 ) {
		/* Skip to first non-white character. */
		while ( iswhite( *tkn ) && *tkn ) tkn++;
		if ( *tkn == 0 ) break;
		/* Find the next delimiter */
		for ( chr = tkn; *chr; chr++ ) {
			if ( *chr == ';' || *chr == '\n' || *chr == '\r' ) break;
		}
		*chr = 0;
		/* Record this as a valid token. */
		token[n++] = tkn;
		if ( chr >= return_tokens[circular] + strlen( line ) ) {
			token[n] = NULL;
			break;
		}
		tkn = chr + 1;
	}

	/* Last token shall be a null pointer by definition. */
	token[n] = NULL;

	/* Next time around use a different buffer for the strings. */
	circular = ( circular + 1 ) % PARSER_BUFFERS;

	return( n );

}
void GraspMMIGraphsForm::ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];

	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	tree->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 4 ) {
			int number;
			sscanf( token[0], "%d", &number );
			// subjectList[nSubjects] = gcnew Subject( number, token[1], token[2], token[3] );
			System::Windows::Forms::TreeNode^  node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[1] ) + " " + gcnew String( token[2] ) ));
			node->Tag = number;
			ParseSessionFile( node, scriptDirectory + (gcnew String( token[3] )));
			tree->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in subject file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}

void GraspMMIGraphsForm::ParseSessionFile( System::Windows::Forms::TreeNode^  subject, String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );

	subject->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 3 ) {
			int number;
			sscanf( token[0], "%d", &number );
			// protocolList[nProtocols] = gcnew Protocol( number, token[1], token[2] );
			System::Windows::Forms::TreeNode^ node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[1] ) ));
			subject->Nodes->Add( node );
			node->Tag = number;
			ParseProtocolFile( node, scriptDirectory + (gcnew String( token[2] )));
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in session file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );

}

void GraspMMIGraphsForm::ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	protocol->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens > 2 || tokens < MAX_TOKENS ) {
			int number;
			sscanf( token[0], "%d", &number );
			//taskList[nTasks] = gcnew Task( number, token[1], token[2] );
			System::Windows::Forms::TreeNode^ node = gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[2] ));
			node->Tag = number;
			protocol->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in protocol file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}




