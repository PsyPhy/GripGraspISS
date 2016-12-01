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
#include <Windows.h>

#include "..\Useful\Useful.h"
#include "..\Useful\fOutputDebugString.h"
#include "..\Useful\fMessageBox.h"

#include "Form1.h"
#include "GraspMMIDataPlots.h"

// We make use of a package of plotting routines that I have had around for decades.
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\OglDisplayInterface.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\OglDisplayInterface.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\OglDisplay.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Graphics.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Views.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Layouts.h"


using namespace GraspMMI;

// Local constants defining the ranges of various plots.

double	positionRadius = 500.0;

// Time span in seconds for each position of the span selector.
#define SPAN_VALUES	8
double windowSpanSeconds[SPAN_VALUES] = { 43200.0, 14400.0, 3600.0, 1800.0, 600.0, 300.0, 60.0, 30.0 };

Display hmdDisplay;
Display taskDisplay;

::View	hmdPositionStripchartView;

// Initialize the objects used to plot the data on the screen.
void Form1::InitializeGraphics( void ) {

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

	// HMD Strip Charts
	parent = static_cast<HWND>( hmdGraphPanel->Handle.ToPointer());
   	hmdDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( hmdDisplay, hmdGraphPanel->Size.Width, hmdGraphPanel->Size.Height );
	DisplaySetScreenPosition( hmdDisplay, 0, 0 );
	DisplayInit( hmdDisplay );
	Erase( hmdDisplay );
	/*
	// Create a Layout (an array of Views) that will be used to plot data in stripchart form.
	stripchart_layout = CreateLayout( stripchart_display, STRIPCHARTS, 1 );
	LayoutSetDisplayEdgesRelative( stripchart_layout, 0.0, 0.065, 1.0, 1.0 );
	visibility_view = CreateView( stripchart_display );
	ViewSetDisplayEdgesRelative( visibility_view, 0.005, 0.0, 0.995, 0.06 );

	// Create a specific view for displaying detailed visibility information.
	// It is not part of the stripchart Layout because I want it to be smaller in height
	//  than the rest of the stripcharts.
	detailed_visibility_layout = CreateLayout( stripchart_display, 4, 1 );
	LayoutSetDisplayEdgesRelative( detailed_visibility_layout, 0.0, 0.065, 1.0, 1.0 );
	*/

	// Task Strip Charts
	parent = static_cast<HWND>( taskGraphPanel->Handle.ToPointer());
   	taskDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( taskDisplay, taskGraphPanel->Size.Width, taskGraphPanel->Size.Height );
	DisplaySetScreenPosition( taskDisplay, 0, 0 );
	DisplayInit( taskDisplay );
	Erase( taskDisplay );

	hmdPositionStripchartView = CreateView( hmdDisplay );

}

// The GUI has a scroll bar allowing one to look back at different parts of the data and
// a slider that determines the time span of the data window. Here we determine the time span 
// of the available data, set the limits on the scroll bar accordingly, and show the limits
// in clock format under the scroll bar.
void Form1::AdjustScrollSpan( void ) {

	double min, max;
	unsigned long	i;

	// The time span of data to plot is determined by the slider.
	double span = windowSpanSeconds[spanSelector->Value];

	// Find the time window of the available data packets.
	// The global array RealMarkerTime[] has been filled previously.
	min = 0.0;
	for ( i = 0; i < nDataSlices; i++ ) {
		if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) {
			min = graspDataSlice[i].absoluteTime;
			break;
		}
	}
	max = span;
	for ( i = nDataSlices - 1; i >= 0; i-- ) {
		if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) {
			max = graspDataSlice[i].absoluteTime;
			break;
		}
	}
	// Adjust the behavior of the scroll bar depending on the selected 
	// time span of the data window. A large step moves a full window
	// width, a small step moves 1/10th of the window.
	scrollBar->LargeChange = span;
	scrollBar->SmallChange = span / 10.0;
	// Set the scroll bar limits to match the limits of the available data.
	// Note that you cannot not reach the max value of the scroll bar with the user
	// controls. Best you can do is to get within LargeChange of the maximum. 
	// We extend the range of the scroll bar by that value so that one can
	//  reach the end of the data.
	int top = ceil( max ) + scrollBar->LargeChange;
	int bottom = floor( min );
	if ( bottom >= floor( max ) ) bottom = floor( max );
	scrollBar->Maximum = top;
	scrollBar->Minimum = bottom;

	// Label the two ends of the scroll bar.
	lastAbsoluteTimeTextBox->Text = CreateTimeString( max );
	firstAbsoluteTimeTextBox->Text = CreateTimeString( min );
}

// When the display is 'live' we want to be able to automatically position the scroll bar 
// so as to display the most recent data.
void Form1::MoveToLatest( void ) {
	double latest;
	for ( int i = nDataSlices - 1; i >= 0; i-- ) {
		if ( graspDataSlice[i].absoluteTime != MISSING_DOUBLE ) {
			latest = graspDataSlice[i].absoluteTime;
			break;
		}
	}
	scrollBar->Value = ceil( latest );
}

// Here we do the actual work of plotting the strip charts and phase plots.
// It is assumed that the global data arrays have been filled. The time span
// of the plots is determined by the scroll bar and span slider.
void Form1::RefreshGraphics( void ) {

	unsigned long first_sample;
	unsigned long last_sample;
	unsigned long index;

	fOutputDebugString( "Start RefreshGraphics().\n" );
	DisplayActivate( hmdDisplay );
	Erase( hmdDisplay );

	// Determine the time window, in seconds, based on the scroll bar position and the span slider.
	double last_instant = scrollBar->Value;
	double span = windowSpanSeconds[spanSelector->Value];
	double first_instant = last_instant - span;

	// Convert the earliest and latest instants to clock format and display them.
	hmdLeftTimeLimit->Text = CreateTimeString( first_instant );
	hmdRightTimeLimit->Text = CreateTimeString( last_instant );
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
	fOutputDebugString( "Data: %d to %d Graph: %lf to %lf Indices: %d to %d (%d)\n", scrollBar->Minimum, scrollBar->Maximum, first_instant, last_instant, first_sample, last_sample, (last_sample - first_sample) );

	ViewSetXLimits( hmdPositionStripchartView, first_instant, last_instant );
	ViewSetYLimits( hmdPositionStripchartView, - positionRadius, positionRadius );
	ViewColor( hmdPositionStripchartView, GREY1 );
	// ViewHorizontalLine( hmdPositionStripchartView, 0.0 );

	ViewColor( hmdPositionStripchartView, RED );
	ViewXYPlotDoubles( hmdPositionStripchartView, 
		&graspDataSlice[first_sample].absoluteTime, 
		&graspDataSlice[first_sample].HMD.pose.position[X], 
		first_sample, last_sample, 1,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ) );

	DisplaySwap( hmdDisplay );

	fOutputDebugString( "Finish RefreshGraphics().\n" );

}

// Clean up resources allocated by the Views system.
void Form1::KillGraphics( void ) {
	Close( hmdDisplay );
	Close( taskDisplay );
}
#if 0
///
/// Plotting into Stripcharts
///

// The following routines all share a similar format:
//
//  Parameter 'view' is a pointer to a structure that maps data values to screen coordinates. Basically it references a rectangle on the screen.
//  Parameters 'start_instant' and 'stop_instant' determine the time window in seconds.
//  Parameters 'start_index', 'stop_index' determine the range of entries in the data arrays to be used for the graphs.
//  Parameter 'step' allows for sub-sampling of the data to reduce the number of points that are actually plotted.
//
// All of these routines assume that the data arrays are already filled with valid data.

// The plotting routines use pointers and byte sizes to allow one to plot, for instance, one component from an array 
// of vectors. The sizeof() macro is used to compute the distance in bytes between elements in the array.

// The plotting routines also take a "missing value" flag. Data that is set to this value will not be plotted. This
//  is used to mark breaks in the data stream.

// In the following method names, 'Graph...' refers to a stripchart, while 'Plot...' refers to a phase plot.

void Form1::GraphHeadPosition( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int step ){
			
	double range;

	ViewColor( view, GREY6 );
	ViewBox( view );
	ViewColor( view, BLACK );
	ViewTitle( view, "Manipulandum Position ", INSIDE_RIGHT, INSIDE_TOP, 0.0 );

	// Plot all 3 components of the manipulandum position in the same view;
	// The autoscaling is a bit complicated. I want each trace centered on its own mean
	//  but I want the range of values to be common to all three so that magnitudes of movement
	//  can be compared between X, Y and Z.
	ViewSetXLimits( view, start_instant, stop_instant );
	if ( autoscaleCheckBox->Checked ) {
		// Find the common range.
		range = 0.0;
		for ( int i = X; i <= Z; i++ ) {
			ViewAutoScaleInit( view );
			ViewAutoScaleAvailableDoubles( view, &ManipulandumPosition[0][i], start_frame, stop_frame, sizeof( *ManipulandumPosition ), MISSING_DOUBLE );
			if ( ViewYRange( view ) > range ) range = ViewYRange( view );
		}
	}
	ViewAxes( view );
	for ( int i = X; i <= Z; i++ ) {
		ViewSelectColor( view, i );
		if ( autoscaleCheckBox->Checked ) {
			// Autoscale each component to center each trace on its respective mean.
			ViewAutoScaleInit( view );
			ViewAutoScaleAvailableDoubles( view, &ManipulandumPosition[0][i], start_frame, stop_frame, sizeof( *ManipulandumPosition ), MISSING_DOUBLE );
			// But expand the Y limits so that all 3 components are plotted on a common scale.
			ViewSetYRange( view, range );
		}
		else {
			// Use the fixed limits.
			ViewSetYLimits( view, lowerPositionLimit, upperPositionLimit );
		}
		// Actually plot the data.
		ViewXYPlotAvailableDoubles( view, &RealMarkerTime[0], &ManipulandumPosition[0][i], start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *ManipulandumPosition ), MISSING_DOUBLE );
	}

}

void Form1::GraphHeadPositionComponent( int component, ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int step ){
			
	char *title;

	ViewColor( view, GREY6 );
	ViewBox( view );
	ViewColor( view, BLACK );
	switch ( component ) {
	case X: title = "Manipulandum Position X "; break;
	case Y: title = "Manipulandum Position Y "; break;
	case Z: title = "Manipulandum Position Z "; break;
	default: title = "error";
	}
	ViewTitle( view, title, INSIDE_RIGHT, INSIDE_TOP, 0.0 );
	ViewSetXLimits( view, start_instant, stop_instant );
	if ( autoscaleCheckBox->Checked ) {
		ViewAutoScaleInit( view );
		ViewAutoScaleAvailableDoubles( view, &ManipulandumPosition[0][component], start_frame, stop_frame, sizeof( *ManipulandumPosition ), MISSING_DOUBLE );
	}
	else ViewSetYLimits( view, lowerPositionLimit, upperPositionLimit );
	ViewAxes( view );
	ViewSelectColor( view, component );
	ViewXYPlotAvailableDoubles( view, &RealMarkerTime[0], &ManipulandumPosition[0][component], start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *ManipulandumPosition ), MISSING_DOUBLE );
}


void Form1::GraphHeadVisibility( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int step ) {

	ViewColor( view, GREY6 );
	ViewBox( view );
	ViewColor( view, BLACK );
	ViewTitle( view, "Visibility ", INSIDE_RIGHT, INSIDE_TOP, 0.0 );

	ViewSetXLimits( view, start_instant, stop_instant );
	ViewSetYLimits( view, lowerVisibilityLimit, upperVisibilityLimit );

	ViewColor( view, BLACK );
	ViewScatterPlotAvailableDoubles( view, SYMBOL_FILLED_SQUARE, &RealMarkerTime[0], &PacketReceived[0],  start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *PacketReceived ), MISSING_DOUBLE );
	ViewColor( view, RED );
	ViewScatterPlotAvailableDoubles( view, SYMBOL_FILLED_SQUARE, &RealMarkerTime[0], &ManipulandumVisibility[0],  start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *ManipulandumVisibility ), MISSING_DOUBLE );
	ViewColor( view, GREEN );
	ViewScatterPlotAvailableDoubles( view, SYMBOL_FILLED_SQUARE, &RealMarkerTime[0], &FrameVisibility[0],  start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *FrameVisibility ), MISSING_DOUBLE );
	ViewColor( view, BLUE );
	ViewScatterPlotAvailableDoubles( view, SYMBOL_FILLED_SQUARE, &RealMarkerTime[0], &WristVisibility[0],  start_frame, stop_frame, step, sizeof( *RealMarkerTime ), sizeof( *WristVisibility ), MISSING_DOUBLE );

}
#endif