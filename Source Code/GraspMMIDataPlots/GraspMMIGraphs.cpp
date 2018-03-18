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


// We make use of a package of plotting routines that I have had around for decades.
#include "..\PsyPhy2dGraphicsLib\OglDisplayInterface.h"
#include "..\PsyPhy2dGraphicsLib\OglDisplay.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"

#include "GraspMMIGraphsForm.h"
#include "GraspMMIDataPlotsStartup.h"

using namespace GraspMMI;

#define POSE_STRIPCHARTS	8
#define MARKER_STRIPCHARTS	MARKER_STRUCTURES
#define HISTORY_STRIPCHARTS	1
#define MAX_PLOT_STEP PACKET_STREAM_BREAK_INSERT_SAMPLES	// Maximum down sampling to display data.
#define MAX_PLOT_SAMPLES (3 * 60 * 20)						// Max samples to plot at one time.

// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

// Time span in seconds for each position of the span selector.
double  windowSpanSeconds[SPAN_VALUES] = { 43200.0, 14400.0, 3600.0, 1800.0, 600.0, 300.0, 60.0, 30.0 };

// Read the cached packets.
void GraspMMIGraphsForm::ReadTelemetryCache( String^ root ) {
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *filename_root = (char*)(void*)Marshal::StringToHGlobalAnsi( root ).ToPointer();
	nDataSlices = GetGraspRT( graspDataSlice, MAX_SLICES, filename_root );
	nHousekeepingSlices = GetHousekeepingTrace( graspHousekeepingSlice, MAX_SLICES, filename_root );
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

	// Marker Visibility Strip Charts
	parent = static_cast<HWND>( markerGraphPanel->Handle.ToPointer());
   	markerDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( markerDisplay, markerGraphPanel->Size.Width, markerGraphPanel->Size.Height );
	DisplaySetScreenPosition( markerDisplay, 0, 0 );
	DisplayInit( markerDisplay );
	DisplayErase( markerDisplay );
	markerStripChartLayout = CreateLayout( markerDisplay, MARKER_STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( markerStripChartLayout, 0.0, 0.0, 1.0, 1.0 );

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

// When the display is 'live' we want to be able to automatically position the scroll bar 
// so as to display the most recent data.
void GraspMMIGraphsForm::MoveToLatest( void ) {
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
	scrollBar->Value = (int) ceil( latest );
}

// Here we do the actual work of plotting the strip charts and phase plots.
// It is assumed that the global data arrays have been filled. The time span
// of the plots is determined by the scroll bar and span slider.
void GraspMMIGraphsForm::RefreshGraphics( void ) {

	static int color_by_object[MARKER_STRUCTURES] = { BLUE, CYAN, GREEN };
	static int axis_color = GREY4;

	static bool __debug__ = false;

	int first_sample;
	int last_sample;
	int index;

	int i, j;

	::View view;

	static int refresh_count = 0; 

	if ( __debug__ ) fOutputDebugString( "Start RefreshGraphics(). %d\n", refresh_count++ );

	// Determine the time window, in seconds, based on the scroll bar position and the span slider.
	double last_instant = scrollBar->Value;
	double span = windowSpanSeconds[spanSelector->Value];
	double first_instant = last_instant - span;

	// Convert the earliest and latest instants to clock format and display them.
	taskLeftTimeLimit->Text = CreateTimeString( first_instant );
	taskRightTimeLimit->Text = CreateTimeString( last_instant );

	// Find the indices into the arrays that correspond to the time window.
	for ( index = nDataSlices - 1; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime <= last_instant ) break;
	}
	last_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < first_instant ) break;
	}
	first_sample = index + 1;

	// Subsample the data if there is a lot to be plotted.
	int step = 1;
	while ( ((last_sample - first_sample) / step) > MAX_PLOT_SAMPLES && step < (MAX_PLOT_STEP - 1) ) step *= 2;

	if ( __debug__ ) fOutputDebugString( "Realtime Data: %d to %d Graph: %lf to %lf Indices: %d to %d (%d)  Step: %d\n", scrollBar->Minimum, scrollBar->Maximum, first_instant, last_instant, first_sample, last_sample, (last_sample - first_sample), step );

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

	// Set the plotting limits.
	ViewSetXLimits( view, first_instant, last_instant );
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( i = first_sample + 1; i < last_sample; i++ ) {
			for ( j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.position[j] > max ) max = graspDataSlice[i].HMD.pose.position[j];
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.position[j] < min ) min = graspDataSlice[i].HMD.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - headPositionRadius, headPositionRadius );

	for ( i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].HMD.pose.position[i], 
			first_sample, last_sample - 1, step,
			sizeof( graspDataSlice[first_sample] ), 
			sizeof( graspDataSlice[first_sample] ),
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
		for ( i = first_sample + 1; i < last_sample; i++ ) {
			for ( j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.orientation[j] > max ) max = graspDataSlice[i].HMD.pose.orientation[j];
				if ( graspDataSlice[i].HMD.visible && graspDataSlice[i].HMD.pose.orientation[j] < min ) min = graspDataSlice[i].HMD.pose.orientation[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - quaternionRadius, quaternionRadius );

	for ( i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].HMD.pose.orientation[i], 
			first_sample, last_sample - 1, step,
			sizeof( graspDataSlice[first_sample] ), 
			sizeof( graspDataSlice[first_sample] ),
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
		for ( i = first_sample + 1; i < last_sample; i++ ) {
			for ( j = 0;  j < 3; j++ ) {
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
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	// Positive Enveloppe Border
	ViewSetYLimits( view, - angle_max, angle_max );
	ViewColor( view, GREY6 );
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	// Roll Angle
	ViewColor( view, MAGENTA );
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRollAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
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
		first_sample, last_sample - 1, step * 10,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, BLACK );
	ViewTitle( view, "HMD Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	//
	// Hand Pose
	//
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[HAND_STRUCTURE]);
	ViewBox( view );

	// Set the plotting limits.
	ViewSetXLimits( view, first_instant, last_instant );
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( i = first_sample + 1; i < last_sample; i++ ) {
			for ( j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].hand.visible && graspDataSlice[i].hand.pose.position[j] > max ) max = graspDataSlice[i].hand.pose.position[j];
				if ( graspDataSlice[i].hand.visible && graspDataSlice[i].hand.pose.position[j] < min ) min = graspDataSlice[i].hand.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - handPositionRadius, handPositionRadius );

	for ( i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].hand.pose.position[i], 
			first_sample, last_sample - 1, step,
			sizeof( graspDataSlice[first_sample] ), 
			sizeof( graspDataSlice[first_sample] ),
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
		first_sample, last_sample - 1, step * 10,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, BLACK );
	ViewTitle( view, "Hand Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );

	//
	// Chest Pose
	//
	row++;
	view = LayoutView( poseStripChartLayout, row, 0 );
	ViewColor( view, color_by_object[CHEST_STRUCTURE]);
	ViewBox( view );

	// Set the plotting limits.
	ViewSetXLimits( view, first_instant, last_instant );
	if ( autoscaleHMD->Checked ) {
		// We can't use the ViewAutoscale... functions because only the visibility flag gets set in the arrays.
		// Position and quaternion values are not defined when visibility is false.
		double min = DBL_MAX;
		double max = - DBL_MAX;
		for ( i = first_sample + 1; i < last_sample; i++ ) {
			for ( j = 0;  j < 3; j++ ) {
				if ( graspDataSlice[i].chest.visible && graspDataSlice[i].chest.pose.position[j] > max ) max = graspDataSlice[i].chest.pose.position[j];
				if ( graspDataSlice[i].chest.visible && graspDataSlice[i].chest.pose.position[j] < min ) min = graspDataSlice[i].chest.pose.position[j];
			}
		}
		if ( min == max ) min = -1.0, max = 1.0;
		ViewSetYLimits( view, min, max );
	}
	else ViewSetYLimits( view, - chestPositionRadius, chestPositionRadius );

	for ( i = 0;  i < 3; i++ ) {
		ViewSelectColor( view, i );
		ViewXYPlotAvailableDoubles( view, 
			&graspDataSlice[0].absoluteTime, 
			&graspDataSlice[0].chest.pose.position[i], 
			first_sample, last_sample - 1, step,
			sizeof( graspDataSlice[first_sample] ), 
			sizeof( graspDataSlice[first_sample] ),
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
		first_sample, last_sample - 1, step * 10,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, color_by_object[CHEST_STRUCTURE]);
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].torsoRollAngle, 
		first_sample, last_sample - 1, step * 10,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);
	ViewColor( view, BLACK );
	ViewTitle( view, "Chest Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	// Zap it to the display.
	DisplaySwap( poseDisplay );


	// Find the indices into the arrays that correspond to the time window.
	for ( index = nHousekeepingSlices - 1; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime <= last_instant ) break;
	}
	last_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime < first_instant ) break;
	}
	first_sample = index + 1;
	if ( __debug__ ) fOutputDebugString( "Housekeeping Data: %d to %d Graph: %lf to %lf Indices: %d to %d (%d)\n", scrollBar->Minimum, scrollBar->Maximum, first_instant, last_instant, first_sample, last_sample, (last_sample - first_sample) );
	// for ( int i = first_sample; i < last_sample; i++ ) fOutputDebugString( "Sample: %d  Time: %f  Protocol: %.0f  Step: %.0f\n", i, graspHousekeepingSlice[i].absoluteTime, graspHousekeepingSlice[i].protocolID, graspHousekeepingSlice[i].taskID );

	// Plot how many markers are visible on each marker structure.
	DisplayActivate( markerDisplay );
	DisplayErase( markerDisplay );
	for ( int object = 0; object < MARKER_STRUCTURES; object++ ) {
		view = LayoutView( markerStripChartLayout, MARKER_STRUCTURES - object - 1, 0 );
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
			first_sample, last_sample - 1, 1,
			sizeof( *graspHousekeepingSlice ), 
			sizeof( *graspHousekeepingSlice ),
			MISSING_DOUBLE );
		ViewColor( view, axis_color );
		ViewTitle( view, StructureLabel[object], INSIDE_LEFT, INSIDE_TOP, 0.0 );

	}
	DisplaySwap( markerDisplay );

	//
	// Plot which protocol and task was executing at each point in time.
	//
	DisplayActivate( historyDisplay );
	DisplayErase( historyDisplay );

	// Shade alternate bands of 100's to make it easier to understand the data plot.
	row = 0;
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
		first_sample, last_sample - 1, 1,
		sizeof( *graspHousekeepingSlice ), 
		sizeof( *graspHousekeepingSlice ),
		MISSING_INT);
	ViewSelectColor( view, 2 );
	ViewScatterPlotAvailableDoubles( view, 
		SYMBOL_FILLED_SQUARE,
		&graspHousekeepingSlice[0].absoluteTime, 
		&graspHousekeepingSlice[0].taskID, 
		first_sample, last_sample - 1, 1,
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
	if ( __debug__ ) fOutputDebugString( "Finish RefreshGraphics().\n" );

}

// Clean up resources allocated by the Views system.
void GraspMMIGraphsForm::KillGraphics( void ) {
	DisplayClose( historyDisplay );
	DisplayClose( markerDisplay );
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




