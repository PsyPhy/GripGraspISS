#pragma once

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"
#include "../GraspVR/GraspGLObjects.h"

#include "..\PsyPhy2dGraphicsLib\Displays.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"
#include "../PsyPhy2dGraphicsLib/OglDisplay.h"


#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"


#include "../GraspMMIUtilities/GraspMMIUtilities.h"
#include "../GraspMMIUtilities/GraspData.h"

#include "../GraspGUI/GraspScripts.h"

#include "../GripGraspVersionControl/GripGraspVersionControl.h"

static ::Timer playbackElapsedTimer;

namespace GraspMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace Grasp;
	// We need InteropServics in order to convert a String to a char *.
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for GraspMMIGraphsForm
	/// </summary>
	public ref class GraspMMIGraphsForm : public System::Windows::Forms::Form
	{

	private: 

		///
		/// Display parameters.
		///

		static double	headPositionRadius = 1000.0; // Range of XYZ values around zero in mm.
		static double	handPositionRadius = 1000.0; // Range of XYZ values around zero in mm.
		static double	chestPositionRadius = 1000.0; // Range of XYZ values around zero in mm.
		static double	quaternionRadius = 0.5;	// Range for XYZ components of quaternions. Use 1.0 to see 360° rotations.
		static double	rotationRadius = 90.0;	// Range of rotation amplitudes, in degrees.
		static	int		refreshInterval = 200;	// How often to update the display, in milliseconds.
		static	int		playbackRefreshInterval = 100;	// How often to update the display, in milliseconds.
		static double	residualPlotMaximum = 10.0;
		static double	coherencePlotMaximum = 200.0;
		static double	coherenceThreshold = 50.0;
		static double	coherenceFilterConstant = 20.0;	// Lowpass filtering of positions for coherence check.

		double taskViewBottom;
		double taskViewTop;

		//
		// Objects used to plot to the screen.
		//

		Display		poseDisplay;
		Display		visibilityDisplay;
		Display		historyDisplay;
		Display		cursorDisplay;
		::Layout	poseStripChartLayout;
		::Layout	markerStripChartLayout;
		::Layout	cursorLayout;
		::Layout	visibilityStripChartLayout;
		::Layout	historyStripChartLayout;

		int	axis_color;

		OpenGLWindow **hmdWindow;
		OpenGLWindow **handWindow;
		OpenGLWindow **chestWindow;
		OpenGLWindow **codaWindow;

		OpenGLWindow *sideWindow;
		OpenGLWindow *forwardWindow;

		Viewpoint **codaViewpoint;
		Viewpoint *objectViewpoint;
		Viewpoint *focusViewpoint;
		Viewpoint *sideViewpoint;
		Viewpoint *forwardViewpoint;

		Viewpoint *vrSubjectViewpoint;
		OpenGLWindow *vrSubjectWindow;
		Viewpoint *vrSideViewpoint;
		OpenGLWindow *vrSideWindow;

		CodaPoseTracker **tracker;
		CodaPoseTracker *hmdTracker;
		CodaPoseTracker *handTracker;
		CodaPoseTracker *chestTracker;

		// A class that provides methods for making a lot of the 
		// OpenGLObjects that we need for displaying the status.
		GraspGLObjects *renderer;
		// A visual representation of each marker structure.
		// These ones are always at the origin in the null orientation.
		MarkerStructureGLObject *hmdStationary;
		MarkerStructureGLObject *handStationary;
		MarkerStructureGLObject *chestStationary;

		// Another set of objects that will move around according to the tracker.
		MarkerStructureGLObject *hmdMobile;
		MarkerStructureGLObject *handMobile;
		MarkerStructureGLObject *chestMobile;

		Assembly *codas;
		Coda	 **coda;

		// Just a way to refer to all the mobile objects together.
		// It makes it easier to draw all of them. 
		Yoke *mobiles;


		//
		// Arrays to hold the data traces.
		//

		GraspRealtimeDataSlice *graspDataSlice;
		unsigned long	nDataSlices;
		GraspHousekeepingSlice *graspHousekeepingSlice;
		unsigned long	nHousekeepingSlices;

		//
		// Maintain the state of the history parser between refreshes
		// so that we don't have to start from scratch each time.
		// This allows the user to collapse some nodes and they will stay collapsed
		// unless a new task comes in.
		int current_subject;
		int current_protocol;
		int current_task;
		int current_step;
		int first_step;
		TreeNode^ current_task_leaf;
		TreeNode^ previous_task_leaf;
		double current_task_start_time;
		unsigned int task_tree_current_index;
		bool leaveDataLive;

		double current_vr_instant;
		double	playbackReferenceTime;
		bool	playbackForward;

		bool	prepped;


	private: void InitializeVR( void );
	private: void MoveToInstant( double instant );
	private: void MoveToSlice( int index );
	private: void PlotCursor( void );
	private: void RenderMissingVR( double instant );
	private: void RenderVR( unsigned int index );
	private: void RenderWindow( OpenGLWindow *window, Viewpoint *viewpoint, OpenGLObject *object );
	private: void RenderWindow( OpenGLWindow *window, OrthoViewpoint *viewpoint, OpenGLObject *object );
	private: void EraseWindow( OpenGLWindow *window );
	private: void RenderSubjectView( OpenGLWindow *window, Viewpoint *viewpoint, bool vr_active );
	private: void RenderSideView( OpenGLWindow *window, Viewpoint *viewpoint, bool vr_active );
	private: void LookAtFrom( Viewpoint *viewpoint, const Vector3 target, Vector3 from );

	private: void ComputeIndividualMarkerVisibility( GraspRealtimeDataSlice *slice, int n_slices );
	private: void ReadTelemetryCache( String^ root );
	private: bool ReadGraspData( String^ root );
	private: void InitializeGraphics( void );
	private: void PlotPoses( double first_instant, double last_instant );
	private: void PlotMarkers( int coda_unit, double first_instant, double last_instant );
	private: void PlotVisibility( double first_instant, double last_instant );
	private: void PlotHistory( double first_instant, double last_instant );

	private: void RefreshGraphics( void );
	private: void KillGraphics( void );
	private: void AdjustScrollSpan( void );
	private: void MoveToLatest( void );

	private: int  ParseLine( char *token[MAX_TOKENS], char *line );
	private: void ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename );
	private: void ParseSessionFile( System::Windows::Forms::TreeNode^  parent, String^ filename );
	private: void ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename );

	private:



	public:

		//
		// Parameters that should be initialized by the caller before running the form.
		//

		int	TimebaseOffset;				// Offset to convert GPS time to UTC.
		String^ packetCacheFileRoot;	// Path to the packet cache files.

	public: 

		String^ scriptDirectory;		// Path to root file of the GUI menu tree.

		GraspMMIGraphsForm() : 
		TimebaseOffset( -17 ), 
			nDataSlices( 0 ), 
			nHousekeepingSlices( 0 ),
			current_subject( 0 ),
			current_protocol( 0 ),
			current_task( 0 ),
			current_task_leaf( nullptr ),
			previous_task_leaf( nullptr ),
			current_task_start_time( 0.0 ),
			task_tree_current_index( 0 ),
			first_step( 0 ),
			taskViewBottom( 0.0 ),
			taskViewTop( 1000.0 ),
			leaveDataLive( true ),
			playbackForward( true ),
			axis_color( GREY4 ),
			prepped( false )

		{
			InitializeComponent();

			// Allocate memory for display traces.
			graspHousekeepingSlice = (GraspHousekeepingSlice *) calloc( MAX_SLICES, sizeof( GraspHousekeepingSlice ) );
			if ( !graspHousekeepingSlice ) fAbortMessage( "GraspMMI", "Error allocating %d frames for Grasp Housekeeping Data", MAX_SLICES );
			graspDataSlice = (GraspRealtimeDataSlice *) calloc( MAX_SLICES, sizeof( GraspRealtimeDataSlice ) );
			if ( !graspDataSlice ) fAbortMessage( "GraspMMI", "Error allocating %d frames for Grasp Realtime Data", MAX_SLICES );

		}

	protected:
		/// Clean up any resources being used.
		~GraspMMIGraphsForm()
		{
			if (components)
			{
				free( graspDataSlice );
				free( graspHousekeepingSlice );
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  plotSelectionComboBox;
	private: System::Windows::Forms::TextBox^  alignmentFrameTextBox;
	private: System::Windows::Forms::OpenFileDialog^  readGraspAlignmentDialog;
	private: System::Windows::Forms::GroupBox^  playbackGroupBox;
	private: System::Windows::Forms::Button^  exitButton;
	private: System::Windows::Forms::Panel^  cursorPanel;
	private: System::Windows::Forms::TextBox^  taskLeftTimeLimit;
	private: System::Windows::Forms::TextBox^  taskRightTimeLimit;
	private: System::Windows::Forms::HScrollBar^  playbackScrollBar;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Button^  toCursorButton;
	private: System::Windows::Forms::Button^  playBackwardButton;
	private: System::Windows::Forms::Button^  stepBackwardButton;
	private: System::Windows::Forms::Button^  stepForwardButton;
	private: System::Windows::Forms::TextBox^  vrFrameTextBox;
	private: System::Windows::Forms::Button^  playForwardButton;
	private: System::Windows::Forms::Button^  stopPlaybackButton;	private: System::Windows::Forms::OpenFileDialog^  readFileDialog;
	private: System::Windows::Forms::Label^  filenameLabel;
	private: System::Windows::Forms::TabControl^  worldTabs;
	private: System::Windows::Forms::TabPage^  trackerTab;
	private: System::Windows::Forms::TabPage^  VRTab;
	private: System::Windows::Forms::Panel^  sidePanel;
	private: System::Windows::Forms::Panel^  vrSubjectPanel;
	private: System::Windows::Forms::Panel^  forwardPanel;
	private: System::Windows::Forms::CheckBox^  fromCodaCheckBox;
	private: System::Windows::Forms::Panel^  vrSidePanel;
	private: System::Windows::Forms::GroupBox^  timeSeriesGroupBox;
	private: System::Windows::Forms::TextBox^  lastAbsoluteTimeTextBox;
	private: System::Windows::Forms::TextBox^  firstAbsoluteTimeTextBox;
	private: System::Windows::Forms::TrackBar^  spanSelector;
	private: System::Windows::Forms::HScrollBar^  scrollBar;
	private: System::Windows::Forms::GroupBox^  poseGraphGroupBox;
	private: System::Windows::Forms::GroupBox^  taskGraphGroupBox;
	private: System::Windows::Forms::Panel^  poseGraphPanel;
	private: System::Windows::Forms::Panel^  taskGraphPanel;
	private: System::Windows::Forms::CheckBox^  dataLiveCheckBox;
	private: System::Windows::Forms::GroupBox^  visibilityGraphGroupBox;
	private: System::Windows::Forms::Panel^  markerGraphPanel;
	private: System::Windows::Forms::ContextMenuStrip^  hmdContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  autoscaleHMD;
	private: System::Windows::Forms::ContextMenuStrip^  taskContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem000;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem100;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemAll;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem200;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem300;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem400;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem500;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem600;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem900;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripTextBox1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripTextBox2;
	private: System::Windows::Forms::ToolStripMenuItem^  clearItemErrorHighlight;
	private: System::Windows::Forms::TextBox^  autoscaleIndicator;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
	private: System::Windows::Forms::ToolStripMenuItem^  clearAllErrorHighlights;
	private: System::Windows::Forms::ToolStripMenuItem^  rebuildTree;
	private: System::Windows::Forms::Label^  Spans;
	private: System::Windows::Forms::PictureBox^  gripPicture;
	private: System::Windows::Forms::GroupBox^  codaGroupBox;
	private: System::Windows::Forms::Panel^  codaPanel1;
	private: System::Windows::Forms::Panel^  codaPanel0;
	private: System::Windows::Forms::GroupBox^  chestGroupBox;
	private: System::Windows::Forms::Panel^  chestPanel1;
	private: System::Windows::Forms::Panel^  chestPanel0;
	private: System::Windows::Forms::GroupBox^  handGroupBox;
	private: System::Windows::Forms::Panel^  handPanel1;
	private: System::Windows::Forms::Panel^  handPanel0;
	private: System::Windows::Forms::GroupBox^  hmdGroupBox;
	private: System::Windows::Forms::Panel^  hmdPanel1;
	private: System::Windows::Forms::Panel^  hmdPanel0;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::CheckBox^  realMarkersCheckBox;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GraspMMIGraphsForm::typeid));
			this->timeSeriesGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->lastAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->filenameLabel = (gcnew System::Windows::Forms::Label());
			this->Spans = (gcnew System::Windows::Forms::Label());
			this->dataLiveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->firstAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->poseGraphGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->plotSelectionComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->poseGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->hmdContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->autoscaleHMD = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->autoscaleIndicator = (gcnew System::Windows::Forms::TextBox());
			this->taskGraphGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->taskGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->taskContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->toolStripMenuItemAll = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripTextBox1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem100 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem200 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem300 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripTextBox2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem400 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem500 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem600 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem000 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem900 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->visibilityGraphGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->markerGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->rebuildTree = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->clearItemErrorHighlight = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearAllErrorHighlights = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gripPicture = (gcnew System::Windows::Forms::PictureBox());
			this->codaGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->fromCodaCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->realMarkersCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->codaPanel1 = (gcnew System::Windows::Forms::Panel());
			this->codaPanel0 = (gcnew System::Windows::Forms::Panel());
			this->chestGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->chestPanel1 = (gcnew System::Windows::Forms::Panel());
			this->chestPanel0 = (gcnew System::Windows::Forms::Panel());
			this->handGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->handPanel1 = (gcnew System::Windows::Forms::Panel());
			this->handPanel0 = (gcnew System::Windows::Forms::Panel());
			this->hmdGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->hmdPanel1 = (gcnew System::Windows::Forms::Panel());
			this->hmdPanel0 = (gcnew System::Windows::Forms::Panel());
			this->worldTabs = (gcnew System::Windows::Forms::TabControl());
			this->trackerTab = (gcnew System::Windows::Forms::TabPage());
			this->forwardPanel = (gcnew System::Windows::Forms::Panel());
			this->sidePanel = (gcnew System::Windows::Forms::Panel());
			this->VRTab = (gcnew System::Windows::Forms::TabPage());
			this->vrSidePanel = (gcnew System::Windows::Forms::Panel());
			this->vrSubjectPanel = (gcnew System::Windows::Forms::Panel());
			this->readFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->readGraspAlignmentDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->playbackGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->toCursorButton = (gcnew System::Windows::Forms::Button());
			this->playBackwardButton = (gcnew System::Windows::Forms::Button());
			this->stepBackwardButton = (gcnew System::Windows::Forms::Button());
			this->stepForwardButton = (gcnew System::Windows::Forms::Button());
			this->vrFrameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->playForwardButton = (gcnew System::Windows::Forms::Button());
			this->stopPlaybackButton = (gcnew System::Windows::Forms::Button());
			this->exitButton = (gcnew System::Windows::Forms::Button());
			this->cursorPanel = (gcnew System::Windows::Forms::Panel());
			this->taskLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->playbackScrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->alignmentFrameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->timeSeriesGroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->poseGraphGroupBox->SuspendLayout();
			this->poseGraphPanel->SuspendLayout();
			this->hmdContextMenu->SuspendLayout();
			this->taskGraphGroupBox->SuspendLayout();
			this->taskContextMenu->SuspendLayout();
			this->visibilityGraphGroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gripPicture))->BeginInit();
			this->codaGroupBox->SuspendLayout();
			this->chestGroupBox->SuspendLayout();
			this->handGroupBox->SuspendLayout();
			this->hmdGroupBox->SuspendLayout();
			this->worldTabs->SuspendLayout();
			this->trackerTab->SuspendLayout();
			this->VRTab->SuspendLayout();
			this->playbackGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// timeSeriesGroupBox
			// 
			this->timeSeriesGroupBox->Controls->Add(this->lastAbsoluteTimeTextBox);
			this->timeSeriesGroupBox->Controls->Add(this->filenameLabel);
			this->timeSeriesGroupBox->Controls->Add(this->Spans);
			this->timeSeriesGroupBox->Controls->Add(this->dataLiveCheckBox);
			this->timeSeriesGroupBox->Controls->Add(this->firstAbsoluteTimeTextBox);
			this->timeSeriesGroupBox->Controls->Add(this->spanSelector);
			this->timeSeriesGroupBox->Controls->Add(this->scrollBar);
			this->timeSeriesGroupBox->Location = System::Drawing::Point(95, 4);
			this->timeSeriesGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->timeSeriesGroupBox->Name = L"timeSeriesGroupBox";
			this->timeSeriesGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->timeSeriesGroupBox->Size = System::Drawing::Size(1011, 91);
			this->timeSeriesGroupBox->TabIndex = 0;
			this->timeSeriesGroupBox->TabStop = false;
			this->timeSeriesGroupBox->Text = L"Time Series";
			// 
			// lastAbsoluteTimeTextBox
			// 
			this->lastAbsoluteTimeTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->lastAbsoluteTimeTextBox->Location = System::Drawing::Point(871, 58);
			this->lastAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->lastAbsoluteTimeTextBox->Name = L"lastAbsoluteTimeTextBox";
			this->lastAbsoluteTimeTextBox->Size = System::Drawing::Size(72, 21);
			this->lastAbsoluteTimeTextBox->TabIndex = 3;
			this->lastAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// filenameLabel
			// 
			this->filenameLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->filenameLabel->Location = System::Drawing::Point(277, 59);
			this->filenameLabel->Name = L"filenameLabel";
			this->filenameLabel->Size = System::Drawing::Size(589, 19);
			this->filenameLabel->TabIndex = 14;
			this->filenameLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// Spans
			// 
			this->Spans->AutoSize = true;
			this->Spans->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Spans->Location = System::Drawing::Point(12, 63);
			this->Spans->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->Spans->Name = L"Spans";
			this->Spans->Size = System::Drawing::Size(176, 13);
			this->Spans->TabIndex = 13;
			this->Spans->Text = L"12h  4h  1h  30m 10m  5m  60s  30s";
			// 
			// dataLiveCheckBox
			// 
			this->dataLiveCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->dataLiveCheckBox->AutoSize = true;
			this->dataLiveCheckBox->Checked = true;
			this->dataLiveCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dataLiveCheckBox->Location = System::Drawing::Point(956, 33);
			this->dataLiveCheckBox->Name = L"dataLiveCheckBox";
			this->dataLiveCheckBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->dataLiveCheckBox->Size = System::Drawing::Size(48, 19);
			this->dataLiveCheckBox->TabIndex = 9;
			this->dataLiveCheckBox->Text = L"Live";
			this->dataLiveCheckBox->UseVisualStyleBackColor = true;
			this->dataLiveCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::dataLiveCheckBox_CheckedChanged);
			// 
			// firstAbsoluteTimeTextBox
			// 
			this->firstAbsoluteTimeTextBox->Location = System::Drawing::Point(198, 58);
			this->firstAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->firstAbsoluteTimeTextBox->Name = L"firstAbsoluteTimeTextBox";
			this->firstAbsoluteTimeTextBox->Size = System::Drawing::Size(72, 21);
			this->firstAbsoluteTimeTextBox->TabIndex = 2;
			this->firstAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// spanSelector
			// 
			this->spanSelector->LargeChange = 1;
			this->spanSelector->Location = System::Drawing::Point(12, 28);
			this->spanSelector->Margin = System::Windows::Forms::Padding(4);
			this->spanSelector->Maximum = 7;
			this->spanSelector->Name = L"spanSelector";
			this->spanSelector->Size = System::Drawing::Size(176, 45);
			this->spanSelector->TabIndex = 1;
			this->spanSelector->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->spanSelector->Value = 6;
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::spanSelector_ValueChanged);
			// 
			// scrollBar
			// 
			this->scrollBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->scrollBar->LargeChange = 25;
			this->scrollBar->Location = System::Drawing::Point(198, 31);
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(745, 23);
			this->scrollBar->TabIndex = 0;
			this->scrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::scrollBar_ValueChanged);
			// 
			// poseGraphGroupBox
			// 
			this->poseGraphGroupBox->Controls->Add(this->plotSelectionComboBox);
			this->poseGraphGroupBox->Controls->Add(this->poseGraphPanel);
			this->poseGraphGroupBox->Location = System::Drawing::Point(7, 96);
			this->poseGraphGroupBox->Name = L"poseGraphGroupBox";
			this->poseGraphGroupBox->Size = System::Drawing::Size(1098, 429);
			this->poseGraphGroupBox->TabIndex = 1;
			this->poseGraphGroupBox->TabStop = false;
			this->poseGraphGroupBox->Text = L"Kinematics";
			// 
			// plotSelectionComboBox
			// 
			this->plotSelectionComboBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->plotSelectionComboBox->FormattingEnabled = true;
			this->plotSelectionComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Poses", L"Markers Coda 1", L"Markers Coda 2", 
				L"Markers Both"});
			this->plotSelectionComboBox->Location = System::Drawing::Point(951, 0);
			this->plotSelectionComboBox->Name = L"plotSelectionComboBox";
			this->plotSelectionComboBox->Size = System::Drawing::Size(138, 20);
			this->plotSelectionComboBox->TabIndex = 1;
			this->plotSelectionComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::plotSelectionComboBox_SelectedIndexChanged);
			// 
			// poseGraphPanel
			// 
			this->poseGraphPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->poseGraphPanel->ContextMenuStrip = this->hmdContextMenu;
			this->poseGraphPanel->Controls->Add(this->autoscaleIndicator);
			this->poseGraphPanel->Location = System::Drawing::Point(6, 23);
			this->poseGraphPanel->Name = L"poseGraphPanel";
			this->poseGraphPanel->Size = System::Drawing::Size(1086, 396);
			this->poseGraphPanel->TabIndex = 0;
			this->poseGraphPanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::poseGraphPanel_SizeChanged);
			// 
			// hmdContextMenu
			// 
			this->hmdContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->autoscaleHMD});
			this->hmdContextMenu->Name = L"contextMenuStrip1";
			this->hmdContextMenu->ShowCheckMargin = true;
			this->hmdContextMenu->ShowImageMargin = false;
			this->hmdContextMenu->Size = System::Drawing::Size(127, 26);
			this->hmdContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::hmdContextMenu_ItemClicked);
			// 
			// autoscaleHMD
			// 
			this->autoscaleHMD->CheckOnClick = true;
			this->autoscaleHMD->Name = L"autoscaleHMD";
			this->autoscaleHMD->Size = System::Drawing::Size(126, 22);
			this->autoscaleHMD->Text = L"Autoscale";
			// 
			// autoscaleIndicator
			// 
			this->autoscaleIndicator->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->autoscaleIndicator->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->autoscaleIndicator->ForeColor = System::Drawing::Color::Maroon;
			this->autoscaleIndicator->Location = System::Drawing::Point(947, 14);
			this->autoscaleIndicator->Name = L"autoscaleIndicator";
			this->autoscaleIndicator->ReadOnly = true;
			this->autoscaleIndicator->Size = System::Drawing::Size(112, 21);
			this->autoscaleIndicator->TabIndex = 1;
			this->autoscaleIndicator->Text = L"Autoscale On";
			this->autoscaleIndicator->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// taskGraphGroupBox
			// 
			this->taskGraphGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->taskGraphGroupBox->Controls->Add(this->taskGraphPanel);
			this->taskGraphGroupBox->Location = System::Drawing::Point(7, 645);
			this->taskGraphGroupBox->Name = L"taskGraphGroupBox";
			this->taskGraphGroupBox->Size = System::Drawing::Size(1098, 110);
			this->taskGraphGroupBox->TabIndex = 2;
			this->taskGraphGroupBox->TabStop = false;
			this->taskGraphGroupBox->Text = L"Task Execution";
			// 
			// taskGraphPanel
			// 
			this->taskGraphPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->taskGraphPanel->ContextMenuStrip = this->taskContextMenu;
			this->taskGraphPanel->Location = System::Drawing::Point(6, 23);
			this->taskGraphPanel->Name = L"taskGraphPanel";
			this->taskGraphPanel->Size = System::Drawing::Size(1084, 80);
			this->taskGraphPanel->TabIndex = 1;
			this->taskGraphPanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::taskGraphPanel_SizeChanged);
			// 
			// taskContextMenu
			// 
			this->taskContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(14) {this->toolStripMenuItemAll, 
				this->toolStripSeparator1, this->toolStripTextBox1, this->toolStripMenuItem100, this->toolStripMenuItem200, this->toolStripMenuItem300, 
				this->toolStripSeparator2, this->toolStripTextBox2, this->toolStripMenuItem400, this->toolStripMenuItem500, this->toolStripMenuItem600, 
				this->toolStripSeparator3, this->toolStripMenuItem000, this->toolStripMenuItem900});
			this->taskContextMenu->Name = L"taskContextMenu";
			this->taskContextMenu->Size = System::Drawing::Size(181, 268);
			this->taskContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::taskContextMenu_ItemClicked);
			// 
			// toolStripMenuItemAll
			// 
			this->toolStripMenuItemAll->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
			this->toolStripMenuItemAll->ImageScaling = System::Windows::Forms::ToolStripItemImageScaling::None;
			this->toolStripMenuItemAll->Name = L"toolStripMenuItemAll";
			this->toolStripMenuItemAll->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItemAll->Tag = L"";
			this->toolStripMenuItemAll->Text = L"Show All";
			this->toolStripMenuItemAll->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(177, 6);
			// 
			// toolStripTextBox1
			// 
			this->toolStripTextBox1->AutoSize = false;
			this->toolStripTextBox1->Enabled = false;
			this->toolStripTextBox1->Name = L"toolStripTextBox1";
			this->toolStripTextBox1->Size = System::Drawing::Size(100, 24);
			this->toolStripTextBox1->Text = L"Seated";
			// 
			// toolStripMenuItem100
			// 
			this->toolStripMenuItem100->Name = L"toolStripMenuItem100";
			this->toolStripMenuItem100->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem100->Tag = L"100";
			this->toolStripMenuItem100->Text = L"100 Visual-Manual";
			// 
			// toolStripMenuItem200
			// 
			this->toolStripMenuItem200->Name = L"toolStripMenuItem200";
			this->toolStripMenuItem200->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem200->Tag = L"200";
			this->toolStripMenuItem200->Text = L"200 Visual-Visual";
			// 
			// toolStripMenuItem300
			// 
			this->toolStripMenuItem300->Name = L"toolStripMenuItem300";
			this->toolStripMenuItem300->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem300->Tag = L"300";
			this->toolStripMenuItem300->Text = L"300 Manual-Manual";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(177, 6);
			// 
			// toolStripTextBox2
			// 
			this->toolStripTextBox2->AutoSize = false;
			this->toolStripTextBox2->Enabled = false;
			this->toolStripTextBox2->Name = L"toolStripTextBox2";
			this->toolStripTextBox2->Size = System::Drawing::Size(100, 24);
			this->toolStripTextBox2->Text = L"Floating";
			// 
			// toolStripMenuItem400
			// 
			this->toolStripMenuItem400->Name = L"toolStripMenuItem400";
			this->toolStripMenuItem400->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem400->Tag = L"400";
			this->toolStripMenuItem400->Text = L"400 Visual-Visual";
			// 
			// toolStripMenuItem500
			// 
			this->toolStripMenuItem500->Name = L"toolStripMenuItem500";
			this->toolStripMenuItem500->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem500->Tag = L"500";
			this->toolStripMenuItem500->Text = L"500 Visual-Manual";
			// 
			// toolStripMenuItem600
			// 
			this->toolStripMenuItem600->Name = L"toolStripMenuItem600";
			this->toolStripMenuItem600->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem600->Tag = L"600";
			this->toolStripMenuItem600->Text = L"600 Manual-Manual";
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(177, 6);
			// 
			// toolStripMenuItem000
			// 
			this->toolStripMenuItem000->Name = L"toolStripMenuItem000";
			this->toolStripMenuItem000->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem000->Tag = L"0";
			this->toolStripMenuItem000->Text = L"000 Setup / Stow";
			// 
			// toolStripMenuItem900
			// 
			this->toolStripMenuItem900->Name = L"toolStripMenuItem900";
			this->toolStripMenuItem900->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem900->Tag = L"900";
			this->toolStripMenuItem900->Text = L"900 Maintenance";
			// 
			// visibilityGraphGroupBox
			// 
			this->visibilityGraphGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->visibilityGraphGroupBox->Controls->Add(this->markerGraphPanel);
			this->visibilityGraphGroupBox->Location = System::Drawing::Point(7, 529);
			this->visibilityGraphGroupBox->Name = L"visibilityGraphGroupBox";
			this->visibilityGraphGroupBox->Size = System::Drawing::Size(1098, 110);
			this->visibilityGraphGroupBox->TabIndex = 2;
			this->visibilityGraphGroupBox->TabStop = false;
			this->visibilityGraphGroupBox->Text = L"Marker Visibility";
			// 
			// markerGraphPanel
			// 
			this->markerGraphPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->markerGraphPanel->Location = System::Drawing::Point(6, 23);
			this->markerGraphPanel->Name = L"markerGraphPanel";
			this->markerGraphPanel->Size = System::Drawing::Size(1084, 79);
			this->markerGraphPanel->TabIndex = 0;
			this->markerGraphPanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::markerGraphPanel_SizeChanged);
			// 
			// rebuildTree
			// 
			this->rebuildTree->Name = L"rebuildTree";
			this->rebuildTree->Size = System::Drawing::Size(32, 19);
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(175, 6);
			// 
			// clearItemErrorHighlight
			// 
			this->clearItemErrorHighlight->Name = L"clearItemErrorHighlight";
			this->clearItemErrorHighlight->Size = System::Drawing::Size(178, 22);
			this->clearItemErrorHighlight->Text = L"Clear This Error Highlight";
			// 
			// clearAllErrorHighlights
			// 
			this->clearAllErrorHighlights->Name = L"clearAllErrorHighlights";
			this->clearAllErrorHighlights->Size = System::Drawing::Size(178, 22);
			this->clearAllErrorHighlights->Text = L"Clear All Error Highlights";
			// 
			// gripPicture
			// 
			this->gripPicture->BackColor = System::Drawing::Color::Transparent;
			this->gripPicture->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"gripPicture.Image")));
			this->gripPicture->Location = System::Drawing::Point(7, 11);
			this->gripPicture->Name = L"gripPicture";
			this->gripPicture->Size = System::Drawing::Size(84, 84);
			this->gripPicture->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->gripPicture->TabIndex = 10;
			this->gripPicture->TabStop = false;
			// 
			// codaGroupBox
			// 
			this->codaGroupBox->Controls->Add(this->fromCodaCheckBox);
			this->codaGroupBox->Controls->Add(this->realMarkersCheckBox);
			this->codaGroupBox->Controls->Add(this->codaPanel1);
			this->codaGroupBox->Controls->Add(this->codaPanel0);
			this->codaGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->codaGroupBox->Location = System::Drawing::Point(1112, 193);
			this->codaGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->codaGroupBox->Name = L"codaGroupBox";
			this->codaGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->codaGroupBox->Size = System::Drawing::Size(418, 164);
			this->codaGroupBox->TabIndex = 21;
			this->codaGroupBox->TabStop = false;
			this->codaGroupBox->Text = L"Coda  View";
			this->codaGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::codaGroupBox_SizeChanged);
			// 
			// fromCodaCheckBox
			// 
			this->fromCodaCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->fromCodaCheckBox->AutoSize = true;
			this->fromCodaCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->fromCodaCheckBox->Location = System::Drawing::Point(325, 1);
			this->fromCodaCheckBox->Name = L"fromCodaCheckBox";
			this->fromCodaCheckBox->Size = System::Drawing::Size(82, 17);
			this->fromCodaCheckBox->TabIndex = 3;
			this->fromCodaCheckBox->Text = L"From CODA";
			this->fromCodaCheckBox->UseVisualStyleBackColor = true;
			this->fromCodaCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::fromCodaCheckBox_CheckedChanged);
			// 
			// realMarkersCheckBox
			// 
			this->realMarkersCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->realMarkersCheckBox->AutoSize = true;
			this->realMarkersCheckBox->Checked = true;
			this->realMarkersCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->realMarkersCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->realMarkersCheckBox->Location = System::Drawing::Point(218, 1);
			this->realMarkersCheckBox->Name = L"realMarkersCheckBox";
			this->realMarkersCheckBox->Size = System::Drawing::Size(89, 17);
			this->realMarkersCheckBox->TabIndex = 4;
			this->realMarkersCheckBox->Text = L"Real Markers";
			this->realMarkersCheckBox->UseVisualStyleBackColor = true;
			this->realMarkersCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::realMarkersCheckBox_CheckedChanged);
			// 
			// codaPanel1
			// 
			this->codaPanel1->Location = System::Drawing::Point(218, 22);
			this->codaPanel1->Margin = System::Windows::Forms::Padding(2);
			this->codaPanel1->Name = L"codaPanel1";
			this->codaPanel1->Size = System::Drawing::Size(192, 132);
			this->codaPanel1->TabIndex = 1;
			// 
			// codaPanel0
			// 
			this->codaPanel0->ForeColor = System::Drawing::SystemColors::ControlText;
			this->codaPanel0->Location = System::Drawing::Point(13, 22);
			this->codaPanel0->Margin = System::Windows::Forms::Padding(2);
			this->codaPanel0->Name = L"codaPanel0";
			this->codaPanel0->Size = System::Drawing::Size(192, 132);
			this->codaPanel0->TabIndex = 0;
			// 
			// chestGroupBox
			// 
			this->chestGroupBox->Controls->Add(this->chestPanel1);
			this->chestGroupBox->Controls->Add(this->chestPanel0);
			this->chestGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chestGroupBox->Location = System::Drawing::Point(1112, 697);
			this->chestGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->chestGroupBox->Name = L"chestGroupBox";
			this->chestGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->chestGroupBox->Size = System::Drawing::Size(418, 164);
			this->chestGroupBox->TabIndex = 18;
			this->chestGroupBox->TabStop = false;
			this->chestGroupBox->Text = L"Chest Marker Visibility";
			this->chestGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::chestGroupBox_SizeChanged);
			// 
			// chestPanel1
			// 
			this->chestPanel1->Location = System::Drawing::Point(218, 22);
			this->chestPanel1->Margin = System::Windows::Forms::Padding(2);
			this->chestPanel1->Name = L"chestPanel1";
			this->chestPanel1->Size = System::Drawing::Size(192, 132);
			this->chestPanel1->TabIndex = 2;
			// 
			// chestPanel0
			// 
			this->chestPanel0->Location = System::Drawing::Point(9, 22);
			this->chestPanel0->Margin = System::Windows::Forms::Padding(2);
			this->chestPanel0->Name = L"chestPanel0";
			this->chestPanel0->Size = System::Drawing::Size(192, 132);
			this->chestPanel0->TabIndex = 1;
			// 
			// handGroupBox
			// 
			this->handGroupBox->Controls->Add(this->handPanel1);
			this->handGroupBox->Controls->Add(this->handPanel0);
			this->handGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->handGroupBox->Location = System::Drawing::Point(1112, 529);
			this->handGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->handGroupBox->Name = L"handGroupBox";
			this->handGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->handGroupBox->Size = System::Drawing::Size(418, 164);
			this->handGroupBox->TabIndex = 17;
			this->handGroupBox->TabStop = false;
			this->handGroupBox->Text = L"Hand Marker Visibility";
			this->handGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::handGroupBox_SizeChanged);
			// 
			// handPanel1
			// 
			this->handPanel1->Location = System::Drawing::Point(218, 22);
			this->handPanel1->Margin = System::Windows::Forms::Padding(2);
			this->handPanel1->Name = L"handPanel1";
			this->handPanel1->Size = System::Drawing::Size(192, 132);
			this->handPanel1->TabIndex = 2;
			// 
			// handPanel0
			// 
			this->handPanel0->Location = System::Drawing::Point(9, 22);
			this->handPanel0->Margin = System::Windows::Forms::Padding(2);
			this->handPanel0->Name = L"handPanel0";
			this->handPanel0->Size = System::Drawing::Size(192, 132);
			this->handPanel0->TabIndex = 1;
			// 
			// hmdGroupBox
			// 
			this->hmdGroupBox->Controls->Add(this->hmdPanel1);
			this->hmdGroupBox->Controls->Add(this->hmdPanel0);
			this->hmdGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->hmdGroupBox->Location = System::Drawing::Point(1111, 361);
			this->hmdGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->hmdGroupBox->Name = L"hmdGroupBox";
			this->hmdGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->hmdGroupBox->Size = System::Drawing::Size(418, 164);
			this->hmdGroupBox->TabIndex = 16;
			this->hmdGroupBox->TabStop = false;
			this->hmdGroupBox->Text = L"HMD Marker Visibility";
			this->hmdGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::hmdGroupBox_SizeChanged);
			// 
			// hmdPanel1
			// 
			this->hmdPanel1->Location = System::Drawing::Point(218, 22);
			this->hmdPanel1->Margin = System::Windows::Forms::Padding(2);
			this->hmdPanel1->Name = L"hmdPanel1";
			this->hmdPanel1->Size = System::Drawing::Size(192, 132);
			this->hmdPanel1->TabIndex = 2;
			// 
			// hmdPanel0
			// 
			this->hmdPanel0->Location = System::Drawing::Point(9, 22);
			this->hmdPanel0->Margin = System::Windows::Forms::Padding(2);
			this->hmdPanel0->Name = L"hmdPanel0";
			this->hmdPanel0->Size = System::Drawing::Size(192, 132);
			this->hmdPanel0->TabIndex = 1;
			// 
			// worldTabs
			// 
			this->worldTabs->Controls->Add(this->trackerTab);
			this->worldTabs->Controls->Add(this->VRTab);
			this->worldTabs->Location = System::Drawing::Point(1112, 4);
			this->worldTabs->Name = L"worldTabs";
			this->worldTabs->SelectedIndex = 0;
			this->worldTabs->Size = System::Drawing::Size(417, 184);
			this->worldTabs->TabIndex = 22;
			this->worldTabs->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::worldTabs_SelectedIndexChanged);
			// 
			// trackerTab
			// 
			this->trackerTab->BackColor = System::Drawing::SystemColors::Control;
			this->trackerTab->Controls->Add(this->forwardPanel);
			this->trackerTab->Controls->Add(this->sidePanel);
			this->trackerTab->Location = System::Drawing::Point(4, 24);
			this->trackerTab->Name = L"trackerTab";
			this->trackerTab->Padding = System::Windows::Forms::Padding(3);
			this->trackerTab->Size = System::Drawing::Size(409, 156);
			this->trackerTab->TabIndex = 0;
			this->trackerTab->Text = L"Tracker";
			this->trackerTab->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::trackerTab_SizeChanged);
			// 
			// forwardPanel
			// 
			this->forwardPanel->Location = System::Drawing::Point(211, 7);
			this->forwardPanel->Margin = System::Windows::Forms::Padding(2);
			this->forwardPanel->Name = L"forwardPanel";
			this->forwardPanel->Size = System::Drawing::Size(191, 132);
			this->forwardPanel->TabIndex = 3;
			// 
			// sidePanel
			// 
			this->sidePanel->Location = System::Drawing::Point(7, 7);
			this->sidePanel->Margin = System::Windows::Forms::Padding(2);
			this->sidePanel->Name = L"sidePanel";
			this->sidePanel->Size = System::Drawing::Size(191, 132);
			this->sidePanel->TabIndex = 2;
			// 
			// VRTab
			// 
			this->VRTab->Controls->Add(this->vrSidePanel);
			this->VRTab->Controls->Add(this->vrSubjectPanel);
			this->VRTab->Location = System::Drawing::Point(4, 22);
			this->VRTab->Name = L"VRTab";
			this->VRTab->Padding = System::Windows::Forms::Padding(3);
			this->VRTab->Size = System::Drawing::Size(409, 190);
			this->VRTab->TabIndex = 1;
			this->VRTab->Text = L"  VR";
			this->VRTab->UseVisualStyleBackColor = true;
			this->VRTab->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::VRTab_SizeChanged);
			// 
			// vrSidePanel
			// 
			this->vrSidePanel->Location = System::Drawing::Point(9, 7);
			this->vrSidePanel->Margin = System::Windows::Forms::Padding(2);
			this->vrSidePanel->Name = L"vrSidePanel";
			this->vrSidePanel->Size = System::Drawing::Size(191, 167);
			this->vrSidePanel->TabIndex = 5;
			// 
			// vrSubjectPanel
			// 
			this->vrSubjectPanel->Location = System::Drawing::Point(213, 7);
			this->vrSubjectPanel->Margin = System::Windows::Forms::Padding(2);
			this->vrSubjectPanel->Name = L"vrSubjectPanel";
			this->vrSubjectPanel->Size = System::Drawing::Size(191, 167);
			this->vrSubjectPanel->TabIndex = 4;
			// 
			// readFileDialog
			// 
			this->readFileDialog->Filter = L"Readable Files|Grasp*.gpk;*.pse|Packet Files|Grasp.gpk|Result Files|*.pse|All Fil" 
				L"es|*.*";
			this->readFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &GraspMMIGraphsForm::readFileDialog_FileOk);
			// 
			// readGraspAlignmentDialog
			// 
			this->readGraspAlignmentDialog->DefaultExt = L"dat";
			this->readGraspAlignmentDialog->Filter = L"Grasp Alignment|*.dat|All Files|*.*";
			this->readGraspAlignmentDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &GraspMMIGraphsForm::readGraspAlignmentDialog_FileOk);
			// 
			// playbackGroupBox
			// 
			this->playbackGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->playbackGroupBox->Controls->Add(this->toCursorButton);
			this->playbackGroupBox->Controls->Add(this->playBackwardButton);
			this->playbackGroupBox->Controls->Add(this->stepBackwardButton);
			this->playbackGroupBox->Controls->Add(this->stepForwardButton);
			this->playbackGroupBox->Controls->Add(this->vrFrameTextBox);
			this->playbackGroupBox->Controls->Add(this->playForwardButton);
			this->playbackGroupBox->Controls->Add(this->stopPlaybackButton);
			this->playbackGroupBox->Controls->Add(this->exitButton);
			this->playbackGroupBox->Controls->Add(this->cursorPanel);
			this->playbackGroupBox->Controls->Add(this->taskLeftTimeLimit);
			this->playbackGroupBox->Controls->Add(this->taskRightTimeLimit);
			this->playbackGroupBox->Controls->Add(this->playbackScrollBar);
			this->playbackGroupBox->Controls->Add(this->panel1);
			this->playbackGroupBox->Location = System::Drawing::Point(8, 761);
			this->playbackGroupBox->Name = L"playbackGroupBox";
			this->playbackGroupBox->Size = System::Drawing::Size(1098, 99);
			this->playbackGroupBox->TabIndex = 3;
			this->playbackGroupBox->TabStop = false;
			this->playbackGroupBox->Text = L"Playback";
			// 
			// toCursorButton
			// 
			this->toCursorButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->toCursorButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->toCursorButton->Location = System::Drawing::Point(347, 69);
			this->toCursorButton->Name = L"toCursorButton";
			this->toCursorButton->Size = System::Drawing::Size(37, 24);
			this->toCursorButton->TabIndex = 49;
			this->toCursorButton->Text = L"><";
			this->toCursorButton->UseVisualStyleBackColor = true;
			this->toCursorButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::toCursorButton_Click);
			// 
			// playBackwardButton
			// 
			this->playBackwardButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->playBackwardButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->playBackwardButton->Location = System::Drawing::Point(422, 69);
			this->playBackwardButton->Name = L"playBackwardButton";
			this->playBackwardButton->Size = System::Drawing::Size(37, 24);
			this->playBackwardButton->TabIndex = 48;
			this->playBackwardButton->Text = L"<";
			this->playBackwardButton->UseVisualStyleBackColor = true;
			this->playBackwardButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::playBackwardButton_Click);
			// 
			// stepBackwardButton
			// 
			this->stepBackwardButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->stepBackwardButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepBackwardButton->Location = System::Drawing::Point(461, 69);
			this->stepBackwardButton->Name = L"stepBackwardButton";
			this->stepBackwardButton->Size = System::Drawing::Size(37, 24);
			this->stepBackwardButton->TabIndex = 47;
			this->stepBackwardButton->Text = L"-";
			this->stepBackwardButton->UseVisualStyleBackColor = true;
			this->stepBackwardButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::stepBackwardButton_Click);
			// 
			// stepForwardButton
			// 
			this->stepForwardButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->stepForwardButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepForwardButton->Location = System::Drawing::Point(587, 69);
			this->stepForwardButton->Name = L"stepForwardButton";
			this->stepForwardButton->Size = System::Drawing::Size(37, 24);
			this->stepForwardButton->TabIndex = 46;
			this->stepForwardButton->Text = L"+";
			this->stepForwardButton->UseVisualStyleBackColor = true;
			this->stepForwardButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::stepForwardButton_Click);
			// 
			// vrFrameTextBox
			// 
			this->vrFrameTextBox->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->vrFrameTextBox->Location = System::Drawing::Point(500, 70);
			this->vrFrameTextBox->Margin = System::Windows::Forms::Padding(4);
			this->vrFrameTextBox->Name = L"vrFrameTextBox";
			this->vrFrameTextBox->Size = System::Drawing::Size(85, 21);
			this->vrFrameTextBox->TabIndex = 45;
			this->vrFrameTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// playForwardButton
			// 
			this->playForwardButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->playForwardButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->playForwardButton->Location = System::Drawing::Point(630, 69);
			this->playForwardButton->Name = L"playForwardButton";
			this->playForwardButton->Size = System::Drawing::Size(37, 24);
			this->playForwardButton->TabIndex = 44;
			this->playForwardButton->Text = L">";
			this->playForwardButton->UseVisualStyleBackColor = true;
			this->playForwardButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::playForwardButton_Click);
			// 
			// stopPlaybackButton
			// 
			this->stopPlaybackButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->stopPlaybackButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stopPlaybackButton->Location = System::Drawing::Point(708, 69);
			this->stopPlaybackButton->Name = L"stopPlaybackButton";
			this->stopPlaybackButton->Size = System::Drawing::Size(37, 24);
			this->stopPlaybackButton->TabIndex = 43;
			this->stopPlaybackButton->Text = L"||";
			this->stopPlaybackButton->UseVisualStyleBackColor = true;
			this->stopPlaybackButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::stopPlaybackButton_Click);
			// 
			// exitButton
			// 
			this->exitButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->exitButton->Location = System::Drawing::Point(882, 68);
			this->exitButton->Name = L"exitButton";
			this->exitButton->Size = System::Drawing::Size(19, 26);
			this->exitButton->TabIndex = 36;
			this->exitButton->Text = L"Exit";
			this->exitButton->UseVisualStyleBackColor = true;
			this->exitButton->Visible = false;
			this->exitButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::button1_Click);
			// 
			// cursorPanel
			// 
			this->cursorPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cursorPanel->ContextMenuStrip = this->taskContextMenu;
			this->cursorPanel->Location = System::Drawing::Point(7, 22);
			this->cursorPanel->Name = L"cursorPanel";
			this->cursorPanel->Size = System::Drawing::Size(1084, 19);
			this->cursorPanel->TabIndex = 35;
			this->cursorPanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::cursorPanel_SizeChanged);
			// 
			// taskLeftTimeLimit
			// 
			this->taskLeftTimeLimit->Location = System::Drawing::Point(6, 71);
			this->taskLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskLeftTimeLimit->Name = L"taskLeftTimeLimit";
			this->taskLeftTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskLeftTimeLimit->TabIndex = 32;
			this->taskLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// taskRightTimeLimit
			// 
			this->taskRightTimeLimit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->taskRightTimeLimit->Location = System::Drawing::Point(1019, 71);
			this->taskRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskRightTimeLimit->Name = L"taskRightTimeLimit";
			this->taskRightTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskRightTimeLimit->TabIndex = 33;
			this->taskRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// playbackScrollBar
			// 
			this->playbackScrollBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->playbackScrollBar->LargeChange = 2;
			this->playbackScrollBar->Location = System::Drawing::Point(8, 44);
			this->playbackScrollBar->Maximum = 1000;
			this->playbackScrollBar->Name = L"playbackScrollBar";
			this->playbackScrollBar->Size = System::Drawing::Size(1083, 23);
			this->playbackScrollBar->TabIndex = 34;
			this->playbackScrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::playbackScrollBar_ValueChanged);
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->ContextMenuStrip = this->taskContextMenu;
			this->panel1->Location = System::Drawing::Point(6, 23);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(0, 0);
			this->panel1->TabIndex = 1;
			// 
			// alignmentFrameTextBox
			// 
			this->alignmentFrameTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->alignmentFrameTextBox->Location = System::Drawing::Point(1328, 3);
			this->alignmentFrameTextBox->Margin = System::Windows::Forms::Padding(4);
			this->alignmentFrameTextBox->Name = L"alignmentFrameTextBox";
			this->alignmentFrameTextBox->Size = System::Drawing::Size(190, 21);
			this->alignmentFrameTextBox->TabIndex = 28;
			this->alignmentFrameTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->alignmentFrameTextBox->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::alignmentFrameTextBox_Click);
			// 
			// GraspMMIGraphsForm
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(1538, 867);
			this->Controls->Add(this->alignmentFrameTextBox);
			this->Controls->Add(this->playbackGroupBox);
			this->Controls->Add(this->worldTabs);
			this->Controls->Add(this->gripPicture);
			this->Controls->Add(this->poseGraphGroupBox);
			this->Controls->Add(this->timeSeriesGroupBox);
			this->Controls->Add(this->codaGroupBox);
			this->Controls->Add(this->taskGraphGroupBox);
			this->Controls->Add(this->visibilityGraphGroupBox);
			this->Controls->Add(this->hmdGroupBox);
			this->Controls->Add(this->chestGroupBox);
			this->Controls->Add(this->handGroupBox);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Location = System::Drawing::Point(-260, -960);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspMMIGraphsForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"GraspMMI Data Plots";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspMMIGraphsForm::GraspMMIGraphsForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspMMIGraphsForm::GraspMMIGraphsForm_Shown);
			this->SizeChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::GraspMMIGraphsForm_SizeChanged);
			this->timeSeriesGroupBox->ResumeLayout(false);
			this->timeSeriesGroupBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->EndInit();
			this->poseGraphGroupBox->ResumeLayout(false);
			this->poseGraphPanel->ResumeLayout(false);
			this->poseGraphPanel->PerformLayout();
			this->hmdContextMenu->ResumeLayout(false);
			this->taskGraphGroupBox->ResumeLayout(false);
			this->taskContextMenu->ResumeLayout(false);
			this->visibilityGraphGroupBox->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gripPicture))->EndInit();
			this->codaGroupBox->ResumeLayout(false);
			this->codaGroupBox->PerformLayout();
			this->chestGroupBox->ResumeLayout(false);
			this->handGroupBox->ResumeLayout(false);
			this->hmdGroupBox->ResumeLayout(false);
			this->worldTabs->ResumeLayout(false);
			this->trackerTab->ResumeLayout(false);
			this->VRTab->ResumeLayout(false);
			this->playbackGroupBox->ResumeLayout(false);
			this->playbackGroupBox->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:

		void ReadRecordedData( void ) {
			nDataSlices = 1;
			nHousekeepingSlices = 1;
			graspDataSlice[nDataSlices].absoluteTime = MISSING_DOUBLE;
			graspHousekeepingSlice[nHousekeepingSlices].absoluteTime = MISSING_DOUBLE;
		}

		// A timer to cue up a single refresh.
		System::Windows::Forms::Timer^ oneShot;
		void CreateOneShotTimer( void ) {
			oneShot = gcnew System::Windows::Forms::Timer;
			oneShot->Interval = 100;
			oneShot->Tick += gcnew EventHandler( this, &GraspMMI::GraspMMIGraphsForm::OnOneShotElapsed );
		}
		void OnOneShotElapsed( System::Object^ source, System::EventArgs ^ e ) {
			RefreshGraphics();
			// MoveToInstant( current_vr_instant );
		}
		void CueRefresh( void ) {
			oneShot->Start();
		}

		// A timer to trigger new polling for packets after a delay.
		System::Windows::Forms::Timer^ timer;
		void CreateRefreshTimer( int interval ) {
			timer = gcnew System::Windows::Forms::Timer;
			timer->Interval = interval;
			timer->Tick += gcnew EventHandler( this, &GraspMMI::GraspMMIGraphsForm::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			timer->Start();
		}
		void StopRefreshTimer( void ) {
			timer->Stop();
		}

		// A timer to update VR during playback.
		System::Windows::Forms::Timer^ playbackTimer;

		void CreatePlaybackTimer( int interval ) {
			playbackTimer = gcnew System::Windows::Forms::Timer;
			playbackTimer->Interval = interval;
			playbackTimer->Tick += gcnew EventHandler( this, &GraspMMI::GraspMMIGraphsForm::OnPlaybackTimerElapsed );
		}
		void StartPlaybackTimer( void ) {
			playbackReferenceTime = current_vr_instant;
			TimerStart( playbackElapsedTimer );
			playbackTimer->Start();
		}
		// This is what we do when the timer goes off.
		void OnPlaybackTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {
			// Stop the timer so that it does not retrigger until we are done refreshing.
			playbackTimer->Stop();
			// fOutputDebugString( "\n" );
			double elapsed = TimerElapsedTime( playbackElapsedTimer );
			// fOutputDebugString( "Playback Timer triggered Elapsed: %f.\n", elapsed  );
			if ( playbackForward ) MoveToInstant( playbackReferenceTime + elapsed );
			else MoveToInstant( playbackReferenceTime - elapsed );
			playbackTimer->Start();
		}

		String^ CreateDurationString( double interval ) {
			char label[256];
			int hours = (int) floor( interval ) / (60 * 60);
			int minutes = ( (int) floor( interval ) % (60 * 60)) / 60;
			int seconds = ( (int) floor( interval ) % 60 );
			sprintf( label, "%02d:%02d:%02d", hours, minutes, seconds );
			return gcnew String( label );
		}
		String^ CreateTimeString( double seconds ) {
			char label[256];
			int day_first = (int) floor(( seconds + TimebaseOffset )) / (24 * 60 * 60);
			int since_midnight = ((int) floor( seconds ) + TimebaseOffset ) % (24 * 60 * 60);
			int hour = since_midnight / (60 * 60);
			int minute = (since_midnight % (60 * 60)) / 60;
			int second = (since_midnight % 60);
			sprintf( label, "%02d:%02d:%02d", hour, minute, second );
			return gcnew String( label );
		}
		String^ CreateTimeStringExtended( double seconds ) {
			char label[256];
			int day_first = (int) floor(( seconds + TimebaseOffset )) / (24 * 60 * 60);
			int since_midnight = ((int) floor( seconds ) + TimebaseOffset ) % (24 * 60 * 60);
			int hour = since_midnight / (60 * 60);
			int minute = (since_midnight % (60 * 60)) / 60;
			int second = (since_midnight % 60);
			int fraction = (int) (( seconds - floor( seconds )) * 1000.0);
			sprintf( label, "%02d:%02d:%02d.%03d", hour, minute, second, fraction );
			return gcnew String( label );
		}

		System::Void GraspMMIGraphsForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			InitializeGraphics();
			InitializeVR();
			dataLiveCheckBox->Checked = true;
			plotSelectionComboBox->SelectedIndex = 0;
			CreateRefreshTimer( refreshInterval );
			CreatePlaybackTimer( playbackRefreshInterval );
			CreateOneShotTimer();
			prepped = true;
			StartRefreshTimer();
		}
		System::Void GraspMMIGraphsForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			playbackTimer->Stop();
			StopRefreshTimer();
			KillGraphics();
		}
		// We have a button that closes the window and thus the application only so that we can set the 
		// CancelButton property to something. Otherwise, when the user presses ESC the program crashes.
		// I'm not exactly why, but this seems to work.
		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			Close();
		}

		// This is what we do when the timer goes off.
		void OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {

			// Stop the timer so that it does not retrigger until we are done refreshing.
			StopRefreshTimer();
			fOutputDebugString( "\n" );
			fOutputDebugString( "Timer triggered.\n" );
			// Erase the filename of any manually loaded file.
			filenameLabel->Text = "";
			// Get the realtime science data packets, if any. 
			ReadTelemetryCache( packetCacheFileRoot );
			// Adjust the scrollbar limits according to the newly loaded data.
			AdjustScrollSpan();
			//// If we are live, shift the limits of the plots to reflect the most recent data.
			//// Otherwise, keep the window span where it is.
			if ( dataLiveCheckBox->Checked ) {
				// MoveToLatest() has the side effect of changing the value of the scrollBar slider
				// which in turn triggers a ValueChanged event on that slider. But when the user 
				// moves the slider, we want to turn of the live mode. There is no way to detect inside
				// spanSelector_ValueChanged() or scrollBar_ValueChanged() whether the change is due to
				// user intervention of a MoveToLatest(). So we set a flag to say when the next
				// ValueChanged events are due to a MoveToLatest.
				leaveDataLive = true;
				MoveToLatest();
				leaveDataLive = false;
				// Start the timer again to trigger the next cycle after a delay.
				StartRefreshTimer();
			}
			RefreshGraphics();

		}
		System::Void spanSelector_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			// Change the span of the data display.
			AdjustScrollSpan();
			// Display with the new span.
			RefreshGraphics();
			PlotCursor();
			// If we are live, set up for the next refresh.
			if ( dataLiveCheckBox->Checked ) StartRefreshTimer();
		}
		System::Void scrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			// Display the data at the new time specified by the scroll bar.
			RefreshGraphics();
			PlotCursor();
			// Since we selected a time, then we are implicitly no longer live.
			if ( !leaveDataLive ) dataLiveCheckBox->Checked = false;
		}

		System::Void dataLiveCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if ( dataLiveCheckBox->Checked ) StartRefreshTimer();
			else StopRefreshTimer();
		}

		System::Void playbackScrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			playbackTimer->Stop();
			if ( nDataSlices > 0 ) MoveToInstant( (double) playbackScrollBar->Value );
			// Since we selected a time, then we are implicitly no longer live.
			if ( !leaveDataLive ) dataLiveCheckBox->Checked = false;
			//leaveDataLive = false;
		}

		System::Void taskContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			if (  e->ClickedItem->Text->Contains( "All") || e->ClickedItem->Text->Contains( "all") ) {
				taskViewBottom = 0;
				taskViewTop = 1000.0;
			}
			else {
				int bottom = Convert::ToInt32( e->ClickedItem->Tag );
				taskViewBottom = bottom;
				taskViewTop = 99.0 + taskViewBottom;
			}
			CueRefresh();
		}

		System::Void hmdContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			CueRefresh();
		}
		System::Void worldTabs_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			alignmentFrameTextBox->Visible = ( worldTabs->SelectedIndex == 0 );
			MoveToInstant( current_vr_instant );
		}			 
		System::Void fromCodaCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			MoveToInstant( current_vr_instant );
		}
		System::Void realMarkersCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			MoveToInstant( current_vr_instant );
		}

		System::Void stepForwardButton_Click(System::Object^  sender, System::EventArgs^  e) {
			unsigned int index;
			dataLiveCheckBox->Checked = false;
			for ( index = nDataSlices - 1; index > 0; index-- ) {
				if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime <= current_vr_instant ) break;
			}
			index++;
			while ( index < nDataSlices ) {
				if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE ) break;
				index++;
			}
			if ( index < nDataSlices ) MoveToSlice( index );
		}

		System::Void stepBackwardButton_Click(System::Object^  sender, System::EventArgs^  e) {

			unsigned int index;
			dataLiveCheckBox->Checked = false;
			for ( index = nDataSlices - 1; index > 0; index-- ) {
				if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < current_vr_instant ) break;
			}
			MoveToSlice( index );
		}

		System::Void playForwardButton_Click(System::Object^  sender, System::EventArgs^  e) {
			dataLiveCheckBox->Checked = false;
			playbackForward = true;
			StartPlaybackTimer();
		}		
		System::Void playBackwardButton_Click(System::Object^  sender, System::EventArgs^  e) {
			dataLiveCheckBox->Checked = false;
			playbackForward = false;
			StartPlaybackTimer();
		}

		System::Void stopPlaybackButton_Click(System::Object^  sender, System::EventArgs^  e) {
			playbackTimer->Stop();
		}
		System::Void toCursorButton_Click(System::Object^  sender, System::EventArgs^  e) {
			dataLiveCheckBox->Checked = false;
			playbackTimer->Stop();
			MoveToInstant( playbackScrollBar->Value );
		}

		// Add an 'About ...' item to the system menu. 
#define SYSMENU_ABOUT_ID		0x01
#define SYSMENU_READ_FILE_ID	0x02

	protected:  virtual void OnHandleCreated( System::EventArgs^ e) override {	

					// Do what one would normally do when the handle is created.
					Form::OnHandleCreated( e );

					// Get a handle to a copy of this form's system (window) menu
					HWND hWnd;
					hWnd = static_cast<HWND>( Handle.ToPointer() );
					HMENU hSysMenu = GetSystemMenu( hWnd, false );
					// Add a separator
					AppendMenu(hSysMenu, MF_SEPARATOR, 0, "" );
					// Add the About menu item
					AppendMenu(hSysMenu, MF_STRING, SYSMENU_READ_FILE_ID, "Read data from file …");
					AppendMenu(hSysMenu, MF_STRING, SYSMENU_ABOUT_ID, "&About …");

				}

	protected:  virtual void WndProc(System::Windows::Forms::Message% m) override {	
					// Test if the About item was selected from the system menu
					if ( m.Msg == WM_SYSCOMMAND ) {
						if ( (int) m.WParam == SYSMENU_ABOUT_ID ) {
							char pwd[MAX_PATH];
							int bytes = GetCurrentDirectory( sizeof( pwd ), pwd );

							// We need InteropServics in order to convert a String to a char *.
							using namespace System::Runtime::InteropServices;

							// Converts the String into a char *.
							// Don't forget to free it when exiting.
							char *cache_root = (char*)(void*)Marshal::StringToHGlobalAnsi( packetCacheFileRoot ).ToPointer();
							char hk_file[MAX_PATHLENGTH];
							char rt_file[MAX_PATHLENGTH];

							// Create the paths to the housekeeping packet file, based on the root and the packet type.
							CreateGripPacketCacheFilename( hk_file, sizeof( hk_file ), GRIP_HK_BULK_PACKET, cache_root );
							CreateGripPacketCacheFilename( rt_file, sizeof( rt_file ), GRIP_RT_SCIENCE_PACKET, cache_root );

							fMessageBox( MB_OK, 
								"GraspMMIDataPlots Version Info", "Source Release:  %s\n         Build Info:  %s\n\nPacket Caches:\n\n  %s\n  %s\n\nExecuting in:\n\n  %s\n", 
								GripGraspSourceRelease, GripGraspBuildInfo, hk_file, rt_file, pwd );
							// Release the memory used to create the ANSI string.
							Marshal::FreeHGlobal( IntPtr( cache_root ) );

							return;
						}
						else if ( (int) m.WParam == SYSMENU_READ_FILE_ID  ) {
							readFileDialog->ShowDialog();
						}
					}
					// Do what one would normally do.
					Form::WndProc( m );
				}

	private: System::Void readFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {

				 // We are loading a previously recorded  file, so stop being live.
				 dataLiveCheckBox->Checked = false;
				 bool is_packet_file = readFileDialog->FileName->EndsWith( ".gpk" );
				 // We actually need to load a pair of files.
				 // We let the user select either in the dialog, or even the "any" file,
				 // but then we strip it down to the root.
				 String^ root;
				 root = readFileDialog->FileName->Replace( ".rt.gpk", "" );
				 root = root->Replace( ".hk.gpk", "" );
				 root = root->Replace( ".any.gpk", "" );
				 root = root->Replace( ".pse", "" );
				 // Show which file we are loading.
				 filenameLabel->Text = root;
				 if ( is_packet_file ) ReadTelemetryCache( root );
				 else ReadGraspData( root );

				 // Set the span to encompass all of the data that was loaded.
				 int i;
				 double span = graspDataSlice[ nDataSlices - 1 ].absoluteTime - graspDataSlice[ 0 ].absoluteTime;
				 for ( i = SPAN_VALUES - 1; i > 0; i-- ) {
					 if ( windowSpanSeconds[i] > span ) break;
				 }
				 spanSelector->Value = i;

				 // Adjust the scrollbar limits according to the newly loaded data.
				 AdjustScrollSpan();

				 // Move to the end of the reccorded data.
				 MoveToLatest();
				 RefreshGraphics();
			 }
	private: System::Void alignmentFrameTextBox_Click(System::Object^  sender, System::EventArgs^  e) {

				 readGraspAlignmentDialog->ShowDialog();

			 }
	private: System::Void readGraspAlignmentDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {

				 // We don't yet have a way of saying at what time the aligment was performed.
				 // So we effectively cancel all alignments that were already loaded in the packet stream,
				 // and we insert the new alignment at the head of the stream.
				 for ( unsigned int i = 0; i < nDataSlices; i++ ) {
					 if ( graspDataSlice[i].clientType == GraspRealtimeDataSlice::ALIGNPOST || graspDataSlice[i].clientType == GraspRealtimeDataSlice::ALIGNPRE ) {
						 graspDataSlice[i].clientType = GraspRealtimeDataSlice::NONE;
					 }
				 }
				 char *path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( readGraspAlignmentDialog->FileName ).ToPointer();
				 LoadGraspCodaAlignment ( &graspDataSlice[0], path );
				 Marshal::FreeHGlobal( IntPtr( path ) );

				 MoveToInstant( current_vr_instant );

			 }

	private: System::Void GraspMMIGraphsForm_SizeChanged(System::Object^  sender, System::EventArgs^  e) {

#undef X
#undef Y
				 int spacer = 6;
				 int tab_extra = 24;
				 int vcr_group_height = playbackGroupBox->Size.Height;

				 int vr_group_height =  (this->ClientSize.Height - tab_extra -  2 * spacer) / 5 - spacer;
				 int vr_group_width = vr_group_height * 4 / 3 * 2;


				 int vr_group_x = this->ClientSize.Width - vr_group_width - spacer;
				 int vr_group_y = worldTabs->Location.Y;


				 worldTabs->Size = System::Drawing::Size( vr_group_width, vr_group_height + tab_extra );
				 worldTabs->Location = System::Drawing::Point(  vr_group_x, vr_group_y );

				 codaGroupBox->Location = System::Drawing::Point(  worldTabs->Location.X, worldTabs->Location.Y + worldTabs->Size.Height + spacer + 1 );
				 codaGroupBox->Size = System::Drawing::Size( vr_group_width, vr_group_height );

				 hmdGroupBox->Location = System::Drawing::Point( worldTabs->Location.X, codaGroupBox->Location.Y + codaGroupBox->Size.Height + spacer + 1 );
				 hmdGroupBox->Size = System::Drawing::Size( vr_group_width, vr_group_height );

				 handGroupBox->Location = System::Drawing::Point( worldTabs->Location.X, hmdGroupBox->Location.Y + hmdGroupBox->Size.Height + spacer + 1 );
				 handGroupBox->Size = System::Drawing::Size( vr_group_width, vr_group_height );

				 chestGroupBox->Location = System::Drawing::Point( worldTabs->Location.X, handGroupBox->Location.Y + handGroupBox->Size.Height + spacer + 1 );
				 chestGroupBox->Size = System::Drawing::Size( vr_group_width, vr_group_height );

				 //vcrGroupBox->Location = System::Drawing::Point( worldTabs->Location.X, chestGroupBox->Location.Y + chestGroupBox->Size.Height + spacer - 2);
				 //vcrGroupBox->Size = System::Drawing::Size( vr_group_width, vcr_group_height );

				 int time_series_width =  this->ClientSize.Width - vr_group_width - 4 * spacer;

				 timeSeriesGroupBox->Size = System::Drawing::Size( time_series_width - gripPicture->Size.Width - spacer, gripPicture->Size.Height );
				 poseGraphGroupBox->Size = System::Drawing::Size( time_series_width, poseGraphGroupBox->Size.Height );
				 visibilityGraphGroupBox->Size = System::Drawing::Size( time_series_width, visibilityGraphGroupBox->Size.Height );
				 taskGraphGroupBox->Size = System::Drawing::Size( time_series_width, taskGraphGroupBox->Size.Height );
				 playbackGroupBox->Size = System::Drawing::Size( time_series_width, playbackGroupBox->Size.Height );

				 poseGraphGroupBox->Size = System::Drawing::Size( 
					 time_series_width, 
					 this->ClientSize.Height - timeSeriesGroupBox->Size.Height 
					 - visibilityGraphGroupBox->Size.Height - taskGraphGroupBox->Size.Height 
					 - playbackGroupBox->Size.Height - 8 * spacer
					 );

				 CueRefresh();
			 }

			 static int spacer = 16;
			 static int font_height = 6;

	private: System::Void chestGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {


				 int width = ( chestGroupBox->ClientSize.Width - 3 * spacer ) / 2;
				 int height = chestGroupBox->ClientSize.Height - font_height - 2 * spacer;

				 chestPanel0->Size = System::Drawing::Size( width, height );
				 chestPanel0->Location = System::Drawing::Point( spacer, font_height + spacer );
				 chestPanel1->Size = System::Drawing::Size( width, height );
				 chestPanel1->Location = System::Drawing::Point(   chestPanel0->Location.X + chestPanel0->Size.Width + spacer, font_height + spacer );
			 }

	private: System::Void handGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {

				 int width = ( handGroupBox->ClientSize.Width - 3 * spacer ) / 2;
				 int height = handGroupBox->ClientSize.Height - font_height - 2 * spacer;

				 handPanel0->Size = System::Drawing::Size( width, height );
				 handPanel0->Location = System::Drawing::Point( spacer, font_height + spacer );
				 handPanel1->Size = System::Drawing::Size( width, height );
				 handPanel1->Location = System::Drawing::Point(   handPanel0->Location.X + handPanel0->Size.Width + spacer, font_height + spacer );
			 }
	private: System::Void hmdGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {

				 int width = ( hmdGroupBox->ClientSize.Width - 3 * spacer ) / 2;
				 int height = hmdGroupBox->ClientSize.Height - font_height - 2 * spacer;

				 hmdPanel0->Size = System::Drawing::Size( width, height );
				 hmdPanel0->Location = System::Drawing::Point( spacer, font_height + spacer );
				 hmdPanel1->Size = System::Drawing::Size( width, height );
				 hmdPanel1->Location = System::Drawing::Point(   hmdPanel0->Location.X + hmdPanel0->Size.Width + spacer, font_height + spacer );
			 }
	private: System::Void codaGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {

				 int width = ( codaGroupBox->ClientSize.Width - 3 * spacer ) / 2;
				 int height = codaGroupBox->ClientSize.Height - font_height - 2 * spacer;

				 codaPanel0->Size = System::Drawing::Size( width, height );
				 codaPanel0->Location = System::Drawing::Point( spacer, font_height + spacer );
				 codaPanel1->Size = System::Drawing::Size( width, height );
				 codaPanel1->Location = System::Drawing::Point(   codaPanel0->Location.X + codaPanel0->Size.Width + spacer, font_height + spacer );
			 }

	private: System::Void trackerTab_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 int width = ( trackerTab->ClientSize.Width - 3 * spacer ) / 2;
				 int height = trackerTab->ClientSize.Height - 2 * spacer;

				 sidePanel->Size = System::Drawing::Size( width, height );
				 sidePanel->Location = System::Drawing::Point( spacer, spacer );
				 forwardPanel->Size = System::Drawing::Size( width, height );
				 forwardPanel->Location = System::Drawing::Point( sidePanel->Location.X + sidePanel->Size.Width + spacer, spacer );
			 }
	private: System::Void VRTab_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 int width = ( VRTab->ClientSize.Width - 3 * spacer ) / 2;
				 int height = VRTab->ClientSize.Height - 2 * spacer;

				 vrSidePanel->Size = System::Drawing::Size( width, height );
				 vrSidePanel->Location = System::Drawing::Point( spacer, spacer );
				 vrSubjectPanel->Size = System::Drawing::Size( width, height );
				 vrSubjectPanel->Location = System::Drawing::Point( vrSidePanel->Location.X + vrSidePanel->Size.Width + spacer, spacer );
			 }

#define X 0
#define Y 1
	private: System::Void poseGraphPanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 OglResize( poseDisplay, poseGraphPanel->ClientSize.Width, poseGraphPanel->ClientSize.Height );
				 LayoutSetDisplayEdgesRelative( poseStripChartLayout, 0.0, 0.0, 1.0, 1.0 );
				 LayoutSetDisplayEdgesRelative( markerStripChartLayout, 0.0, 0.0, 1.0, 1.0 );
			 }
	private: System::Void markerGraphPanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 OglResize( visibilityDisplay, markerGraphPanel->ClientSize.Width, markerGraphPanel->ClientSize.Height );
				 LayoutSetDisplayEdgesRelative( visibilityStripChartLayout, 0.0, 0.0, 1.0, 1.0 );
			 }
	private: System::Void taskGraphPanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 OglResize( historyDisplay, taskGraphPanel->ClientSize.Width, taskGraphPanel->ClientSize.Height );
				 LayoutSetDisplayEdgesRelative( historyStripChartLayout, 0.0, 0.0, 1.0, 1.0 );
			 }
	private: System::Void cursorPanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 OglResize( cursorDisplay, cursorPanel->ClientSize.Width, cursorPanel->ClientSize.Height );
				 LayoutSetDisplayEdgesRelative( cursorLayout, 0.0, 0.0, 1.0, 1.0 );
			 }
	private: System::Void plotSelectionComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 if ( prepped ) CueRefresh();
			 }
};
}

