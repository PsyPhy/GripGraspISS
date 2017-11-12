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

// We make use of a package of plotting routines that I have had around for decades.
#include "..\PsyPhy2dGraphicsLib\OglDisplayInterface.h"
#include "..\PsyPhy2dGraphicsLib\OglDisplay.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"

#include "GraspMMIGraphsForm.h"
#include "GraspMMIDataPlotsStartup.h"

using namespace GraspMMI;

#define HMD_STRIPCHARTS	4
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

	// HMD Strip Charts
	parent = static_cast<HWND>( hmdGraphPanel->Handle.ToPointer());
   	hmdDisplay = CreateOglDisplay();
	SetOglWindowParent( parent );
	DisplaySetSizePixels( hmdDisplay, hmdGraphPanel->Size.Width, hmdGraphPanel->Size.Height );
	DisplaySetScreenPosition( hmdDisplay, 0, 0 );
	DisplayInit( hmdDisplay );
	DisplayErase( hmdDisplay );
	hmdStripChartLayout = CreateLayout( hmdDisplay, HMD_STRIPCHARTS - 1, 1 );
	LayoutSetDisplayEdgesRelative( hmdStripChartLayout, 0.0, 0.1, 1.0, 1.0 );
	tiltStripChartLayout = CreateLayout( hmdDisplay, 1, 1 );
	LayoutSetDisplayEdgesRelative( tiltStripChartLayout, 0.0, 0.0, 1.0, 0.1 );

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

	int first_sample;
	int last_sample;
	int index;

	int i, j;

	::View view;

	static int refresh_count = 0; 

	fOutputDebugString( "Start RefreshGraphics(). %d\n", refresh_count++ );

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

	fOutputDebugString( "Realtime Data: %d to %d Graph: %lf to %lf Indices: %d to %d (%d)  Step: %d\n", scrollBar->Minimum, scrollBar->Maximum, first_instant, last_instant, first_sample, last_sample, (last_sample - first_sample), step );

	// Plot the continous data about the head position and orientation.
	DisplayActivate( hmdDisplay );
	DisplayErase( hmdDisplay );

	int row = 0;
	int axis_color = GREY4;
	view = LayoutView( hmdStripChartLayout, row, 0 );
	ViewColor( view, axis_color );
	ViewBox( view );
	ViewTitle( view, "HMD Position", INSIDE_LEFT, INSIDE_TOP, 0.0 );

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
	else ViewSetYLimits( view, - positionRadius, positionRadius );

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

	row++;
	view = LayoutView( hmdStripChartLayout, row, 0 );
	ViewColor( view, axis_color );
	ViewBox( view );
	ViewTitle( view, "HMD Orientation", INSIDE_LEFT, INSIDE_TOP, 0.0 );
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
	view = LayoutView( hmdStripChartLayout, row, 0 );
	ViewColor( view, axis_color );
	ViewBox( view );
	ViewTitle( view, "HMD Rotation", INSIDE_LEFT, INSIDE_TOP, 0.0 );
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

	ViewSetYLimits( view, angle_max, - angle_max );

	ViewColor( view, GREY6 );
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);

	ViewSetYLimits( view, - angle_max, angle_max );
	ViewColor( view, GREY6 );
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRotationAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);

	ViewColor( view, MAGENTA );
	ViewXYPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRollAngle, 
		first_sample, last_sample - 1, step,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);

	view = LayoutView( tiltStripChartLayout, 0, 0 );
	ViewSetXLimits( view, first_instant, last_instant );
	ViewColor( view, axis_color );
	ViewBox( view );
	ViewTitle( view, "HMD Roll", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	ViewColor( view, MAGENTA );
	ViewTiltPlotAvailableDoubles( view, 
		&graspDataSlice[0].absoluteTime, 
		&graspDataSlice[0].hmdRollAngle, 
		first_sample, last_sample - 1, step * 10,
		sizeof( graspDataSlice[first_sample] ), 
		sizeof( graspDataSlice[first_sample] ),
		MISSING_DOUBLE);

	DisplaySwap( hmdDisplay );


	// Find the indices into the arrays that correspond to the time window.
	for ( index = nHousekeepingSlices - 1; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime <= last_instant ) break;
	}
	last_sample = index;
	for ( index = index; index > 0; index -- ) {
		if ( graspHousekeepingSlice[index].absoluteTime != MISSING_DOUBLE && graspHousekeepingSlice[index].absoluteTime < first_instant ) break;
	}
	first_sample = index + 1;
	fOutputDebugString( "Housekeeping Data: %d to %d Graph: %lf to %lf Indices: %d to %d (%d)\n", scrollBar->Minimum, scrollBar->Maximum, first_instant, last_instant, first_sample, last_sample, (last_sample - first_sample) );
	// for ( int i = first_sample; i < last_sample; i++ ) fOutputDebugString( "Sample: %d  Time: %f  Protocol: %.0f  Step: %.0f\n", i, graspHousekeepingSlice[i].absoluteTime, graspHousekeepingSlice[i].protocolID, graspHousekeepingSlice[i].taskID );

	// Plot how many markers are visible on each marker structure.
	DisplayActivate( markerDisplay );
	DisplayErase( markerDisplay );
	for ( row = 0; row < MARKER_STRUCTURES; row++ ) {
		view = LayoutView( markerStripChartLayout, row, 0 );
		ViewSetXLimits( view, first_instant, last_instant );
		ViewSetYLimits( view, 0, 8.0 );
		ViewColor( view, axis_color );
		ViewBox( view );
		ViewSelectColor( view, row + 2 );
		ViewFillPlotAvailableDoubles( view,
			&graspHousekeepingSlice[0].absoluteTime, 
			&graspHousekeepingSlice[0].visibleMarkers[row], 
			first_sample, last_sample - 1, 1,
			sizeof( *graspHousekeepingSlice ), 
			sizeof( *graspHousekeepingSlice ),
			MISSING_DOUBLE );

	}
	view = LayoutView( markerStripChartLayout, HMD_STRUCTURE, 0 );
	ViewColor( view, axis_color );
	ViewTitle( view, "HMD", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	view = LayoutView( markerStripChartLayout, HAND_STRUCTURE, 0 );
	ViewColor( view, axis_color );
	ViewTitle( view, "Hand", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	view = LayoutView( markerStripChartLayout, CHEST_STRUCTURE, 0 );
	ViewColor( view, axis_color );
	ViewTitle( view, "Chest", INSIDE_LEFT, INSIDE_TOP, 0.0 );
	DisplaySwap( markerDisplay );

	// Plot which protocol and task was executing at each point in time.
	DisplayActivate( historyDisplay );
	fOutputDebugString( "DisplayErase( historyDisplay ) started.\n" );
	DisplayErase( historyDisplay );
	fOutputDebugString( "DisplayErase( historyDisplay ) finished.\n" );

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

	ViewColor( view, axis_color );
	ViewBox( view );
	ViewTitle( view, "Protocol", INSIDE_LEFT, INSIDE_TOP, 0.0 );

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

	ViewColor( view, axis_color );
	for ( double y = bottom; y < top; y += 200.0 ) {
		char label[16];
		sprintf( label, "%03.0f's", y );
		ViewTitle( view, label, INSIDE_LEFT, UserToDisplayY( view, y + 50.0 ), 0.0 );
	}
	for ( double y = bottom + 100.0; y < top; y += 200.0 ) {
		char label[16];
		sprintf( label, "%03.0f's", y );
		ViewTitle( view, label, INSIDE_RIGHT, UserToDisplayY( view, y + 50.0 ), 0.0 );
	}

	DisplaySwap( historyDisplay );
	fOutputDebugString( "Finish RefreshGraphics().\n" );

}

// Clean up resources allocated by the Views system.
void GraspMMIGraphsForm::KillGraphics( void ) {
	DisplayClose( hmdDisplay );
	DisplayClose( historyDisplay );
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

void GraspMMIGraphsForm::RebuildHistoryTree( void ) {
	visibleHistoryTree->Nodes->Clear();
	task_tree_current_index = 0;
	BuildHistoryTree();
}

void GraspMMIGraphsForm::BuildHistoryTree( void ) {

	// Starting from where we left off, look for the beginning and end of each task.
	for ( unsigned int index = task_tree_current_index; index < nHousekeepingSlices; index++ ) {

		// Get the pertinent data from the housekeeping packet.
		int subject = (int) graspHousekeepingSlice[index].userID;
		int protocol = (int) graspHousekeepingSlice[index].protocolID;
		int task = (int) graspHousekeepingSlice[index].taskID;
		int step = (int) graspHousekeepingSlice[index].stepID;

		// Ignore 0000 combinations and whenever there is missing data.
		if ( subject != 0 && protocol != 0 && task != 0 && subject != MISSING_INT && protocol != MISSING_INT && task != MISSING_INT ) {

			// Look for transitions in the subject, protocol and task IDs.
			if ( task != current_task || protocol != current_protocol || subject != current_subject ) {

				// Tasks that are older than the task just completed are maintained in normal color text.
				if ( previous_task_leaf && previous_task_leaf->ForeColor !=  System::Drawing::Color::Red ) previous_task_leaf->ForeColor = System::Drawing::SystemColors::WindowText;

				// If the task has changed from what was the running task, then we have a new task.
				// Mark when the previous task ended. NB: This may not be correct if we had an LOS.
				if ( current_task_leaf ) {
					if ( current_task_leaf->Text->StartsWith( "Initiated:" ) ) {
						current_task_leaf->Text += 
							"Finished: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) +
							" Duration: " + CreateDurationString( graspHousekeepingSlice[index].absoluteTime - current_task_start_time ) +
							current_task_leaf->Tag;
					}
					// Set color of the task that just completed according to completion code.
					if ( current_task_leaf->Text->EndsWith( "!"  ) ) current_task_leaf->ForeColor = System::Drawing::Color::Red;
					else if ( current_task_leaf->Text->EndsWith( "OK"  ) ) current_task_leaf->ForeColor = System::Drawing::Color::Green;
					// No completion codes were noted, so mark it as done, but neurtrally as to good or bad.
					else current_task_leaf->ForeColor = System::Drawing::Color::Blue;
					// Keep track of this node so that we can change its color back to normal.
					previous_task_leaf = current_task_leaf;
				}

				// Find the node corresponding to the new task.
				// First see if it is present in the tree that is visible to the user.
				TreeNode^ subject_node = nullptr;
				int j;
				for ( j = 0; j < visibleHistoryTree->Nodes->Count; j++ ) {
					if ( (int) visibleHistoryTree->Nodes[j]->Tag == subject ) {
						subject_node = visibleHistoryTree->Nodes[j];
						break;
					}
				}
				// If we did not find it in the tree, get it from the full tree.
				if ( !subject_node ) {
					for ( int i = 0; i < historyTree->Nodes->Count; i++ ) {
						if (  (int) historyTree->Nodes[i]->Tag == subject ) {
							subject_node = ( TreeNode^ ) historyTree->Nodes[i]->Clone();
							break;
						}
					}
					fAbortMessageOnCondition( !subject_node, "GraspMMI", "Could not find subject ID %d in history tree.", subject );
					// Insert it into the visible tree according to the subject ID number.
					for ( j = 0; j < visibleHistoryTree->Nodes->Count; j++ ) {
						if ( (int) visibleHistoryTree->Nodes[j]->Tag > subject ) break;
					}
					visibleHistoryTree->Nodes->Insert( j, subject_node );
				}
				TreeNode^ protocol_node = nullptr;
				for ( int i = 0; i < subject_node->Nodes->Count; i++ ) {
					if (  (int) subject_node->Nodes[i]->Tag == protocol ) protocol_node = subject_node->Nodes[i];
				}
				fAbortMessageOnCondition( !protocol_node, "GraspMMI", "Could not find protocol ID %d for subject ID %d in history tree.", protocol, subject );
				TreeNode^ task_node = nullptr;
				for ( int i = 0; i < protocol_node->Nodes->Count; i++ ) {
					if (  (int) protocol_node->Nodes[i]->Tag == task ) task_node = protocol_node->Nodes[i];
				}
				fAbortMessageOnCondition( !task_node, "GraspMMI", "Could not find task ID %d for protocol ID %d and subject ID %d in history tree.", task, protocol, subject );

				// Show that a new instance of the task has been selected by creating a new leaf.
				task_node->Nodes->Add( "Selected: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) );
				// Keep track of the leaf so that we can annotate it when the task finishes.
				current_task_leaf = task_node->Nodes[task_node->Nodes->Count - 1];
				// Highlight the active task.
				current_task_leaf->ForeColor = System::Drawing::Color::Purple;
				current_task_leaf->Tag = gcnew String( "" );
				current_task_start_time = graspHousekeepingSlice[index].absoluteTime;
				// When a new instance of a task starts, we want the user to notice. So we expand the node corresponding to that task.
				// But we do not expand the full tree. This allows the user to collapes a protocol (session) that is no longer active
				// and it will stay collapsed.
				subject_node->Expand();
				protocol_node->Expand();
				task_node->Expand();
				// Note the characteristics of the new instance so that we can detect changes.
				current_subject = subject;
				current_protocol = protocol;
				current_task = task;
				current_step = -1;
				first_step = step;
			}
			// Because tasks get selected automatically when the previous task is finished, changes of the task ID 
			// do not necessarily mean that the subject has started the task. We look for some activity to indicate
			// that the task has actually started and we update the indicator and start time accordingly.
			else if ( current_task_leaf->Text->StartsWith( "Selected:" ) && ( step != first_step || graspHousekeepingSlice[index].scriptEngine >= STEP_EXECUTING )) {
				// Activity has been detected, so change from "Selected" to "Initiated" and update the time.
				current_task_leaf->Text = "Initiated: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) + " GMT ";
				current_task_start_time = graspHousekeepingSlice[index].absoluteTime;
				// Change the color to attract the attention of the user.
				current_task_leaf->ForeColor = System::Drawing::Color::Magenta;
			}
			// If a task has been initiated, look for indications that it completed successfully or not.
			else if ( current_task_leaf->Text->StartsWith( "Initiated:" ) && ( graspHousekeepingSlice[index].scriptEngine >= STEP_FINISHED_ABNORMAL )) {
				// A task has exited with an error code.
				// Prepare the tag to be added to the text of the leaf to show that there was a problem during the execution of this task.
				current_task_leaf->Tag = gcnew String( " !!!" );
				// Add a leaf to the task node that provides information about the completion code.
				if ( step != current_step ) {
					int error_code = - ( ( (int) graspHousekeepingSlice[index].scriptEngine ) - STEP_FINISHED_ABNORMAL );
					TreeNode^ node = gcnew TreeNode( "Step " + step +  " exited with error code " + error_code );
					node->ForeColor = System::Drawing::Color::Red;
					current_task_leaf->Nodes->Add( node );
					// Keep track of the step number so that we don't repeat for the same step.
					current_step = step;
				}
			}
			else if ( current_task_leaf->Text->StartsWith( "Initiated:" ) && ( graspHousekeepingSlice[index].scriptEngine >= STEP_FINISHED_NORMAL )) {
				// An normal exit has been detected.
				// Add a leaf to the task node that provides information about the completion code.
				if ( step != current_step ) {
					int error_code = ( ( (int) graspHousekeepingSlice[index].scriptEngine ) - STEP_FINISHED_NORMAL );
					TreeNode^ node = gcnew TreeNode( "Step " + step +  " exited normally with code " + error_code );
					node->ForeColor = System::Drawing::Color::Green;
					current_task_leaf->Nodes->Add( node );
					// Keep track of the step number so that we don't repeat for the same step.
					current_step = step;
				}
			}
		}
	}
	// Keep track of where we left off so that we don't re-traverse the entire tree again on the next refresh cycle.
	task_tree_current_index = nHousekeepingSlices;
}


