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

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"


#include "../GraspMMIUtilities/GraspMMIUtilities.h"
#include "../GraspGUI/GraspScripts.h"

#include "../GripGraspVersionControl/GripGraspVersionControl.h"

#define MAX_SLICES	(8*60*60*10)
// Time span in seconds for each position of the span selector.
#define SPAN_VALUES	8
extern double windowSpanSeconds[SPAN_VALUES];

namespace GraspMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace Grasp;

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

		double taskViewBottom;
		double taskViewTop;

		//
		// Objects used to plot to the screen.
		//

		Display		poseDisplay;
		Display		markerDisplay;
		Display		historyDisplay;
		Display		cursorDisplay;
		::Layout	poseStripChartLayout;
		::Layout	cursorLayout;
		::Layout	markerStripChartLayout;
		::Layout	historyStripChartLayout;

		int	axis_color;

		OpenGLWindow *hmdWindow0;
		OpenGLWindow *hmdWindow1;
		OpenGLWindow *handWindow0;
		OpenGLWindow *handWindow1;
		OpenGLWindow *chestWindow0;
		OpenGLWindow *chestWindow1;

		OpenGLWindow *codaWindow0;
		OpenGLWindow *codaWindow1;
		OpenGLWindow *sideWindow;
		OpenGLWindow *forwardWindow;

		Viewpoint *codaViewpoint0;
		Viewpoint *codaViewpoint1;
		Viewpoint *objectViewpoint;
		Viewpoint *focusViewpoint;
		Viewpoint *sideViewpoint;
		Viewpoint *forwardViewpoint;

		Viewpoint *vrSubjectViewpoint;
		OpenGLWindow *vrSubjectWindow;
		Viewpoint *vrSideViewpoint;
		OpenGLWindow *vrSideWindow;

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
private: System::Windows::Forms::Button^  exitButton;


		double current_vr_instant;


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

	private: void ReadTelemetryCache( String^ root );
	private: void InitializeGraphics( void );
	private: void RefreshGraphics( void );
	private: void KillGraphics( void );
	private: void AdjustScrollSpan( void );
	private: void MoveToLatest( void );

	private: int  ParseLine( char *token[MAX_TOKENS], char *line );
	private: void ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename );
	private: void ParseSessionFile( System::Windows::Forms::TreeNode^  parent, String^ filename );
	private: void ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename );
	
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
			  axis_color( GREY4 )

		  {
			  InitializeComponent();

			  // Allocate memory for display traces.
			  graspHousekeepingSlice = (GraspHousekeepingSlice *) malloc( MAX_SLICES * sizeof( GraspHousekeepingSlice ) );
			  if ( !graspHousekeepingSlice ) fAbortMessage( "GraspMMI", "Error allocating %d frames for Grasp Housekeeping Data", MAX_SLICES );
			  graspDataSlice = (GraspRealtimeDataSlice *) malloc( MAX_SLICES * sizeof( GraspRealtimeDataSlice ) );
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




	private: System::Windows::Forms::HScrollBar^  playbackScrollBar;
	private: System::Windows::Forms::TextBox^  taskRightTimeLimit;
	private: System::Windows::Forms::TextBox^  taskLeftTimeLimit;
	private: System::Windows::Forms::Panel^  cursorPanel;
	private: System::Windows::Forms::TabControl^  worldTabs;
	private: System::Windows::Forms::TabPage^  trackerTab;
	private: System::Windows::Forms::TabPage^  VRtab;
	private: System::Windows::Forms::Panel^  sidePanel;
	private: System::Windows::Forms::Panel^  vrSubjectPanel;
	private: System::Windows::Forms::Panel^  forwardPanel;
	private: System::Windows::Forms::CheckBox^  fromCodaCheckBox;
	private: System::Windows::Forms::Panel^  vrSidePanel;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  lastAbsoluteTimeTextBox;
	private: System::Windows::Forms::TextBox^  firstAbsoluteTimeTextBox;
	private: System::Windows::Forms::TrackBar^  spanSelector;
	private: System::Windows::Forms::HScrollBar^  scrollBar;
	private: System::Windows::Forms::GroupBox^  poseGraphGroupBox;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Panel^  poseGraphPanel;
	private: System::Windows::Forms::Panel^  taskGraphPanel;
	private: System::Windows::Forms::TextBox^  alignmentFrameTextBox;
	private: System::Windows::Forms::TextBox^  vrFrameTextBox;
	private: System::Windows::Forms::CheckBox^  dataLiveCheckBox;
	private: System::Windows::Forms::GroupBox^  groupBox4;
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
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
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
	private: System::Windows::Forms::Button^  stepForwardButton;
	private: System::Windows::Forms::Button^  stepBackwardButton;

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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->Spans = (gcnew System::Windows::Forms::Label());
			this->dataLiveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->lastAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->firstAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->poseGraphGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->poseGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->hmdContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->autoscaleHMD = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->autoscaleIndicator = (gcnew System::Windows::Forms::TextBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
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
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->markerGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->rebuildTree = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->clearItemErrorHighlight = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearAllErrorHighlights = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->codaPanel1 = (gcnew System::Windows::Forms::Panel());
			this->codaPanel0 = (gcnew System::Windows::Forms::Panel());
			this->chestGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->chestPanel1 = (gcnew System::Windows::Forms::Panel());
			this->chestPanel0 = (gcnew System::Windows::Forms::Panel());
			this->handGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->handPanel1 = (gcnew System::Windows::Forms::Panel());
			this->handPanel0 = (gcnew System::Windows::Forms::Panel());
			this->hmdGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->fromCodaCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->hmdPanel1 = (gcnew System::Windows::Forms::Panel());
			this->hmdPanel0 = (gcnew System::Windows::Forms::Panel());
			this->worldTabs = (gcnew System::Windows::Forms::TabControl());
			this->trackerTab = (gcnew System::Windows::Forms::TabPage());
			this->forwardPanel = (gcnew System::Windows::Forms::Panel());
			this->alignmentFrameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->sidePanel = (gcnew System::Windows::Forms::Panel());
			this->VRtab = (gcnew System::Windows::Forms::TabPage());
			this->vrSidePanel = (gcnew System::Windows::Forms::Panel());
			this->vrSubjectPanel = (gcnew System::Windows::Forms::Panel());
			this->vrFrameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->stepForwardButton = (gcnew System::Windows::Forms::Button());
			this->stepBackwardButton = (gcnew System::Windows::Forms::Button());
			this->playbackScrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->taskRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->cursorPanel = (gcnew System::Windows::Forms::Panel());
			this->exitButton = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->poseGraphGroupBox->SuspendLayout();
			this->poseGraphPanel->SuspendLayout();
			this->hmdContextMenu->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->taskContextMenu->SuspendLayout();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->chestGroupBox->SuspendLayout();
			this->handGroupBox->SuspendLayout();
			this->hmdGroupBox->SuspendLayout();
			this->worldTabs->SuspendLayout();
			this->trackerTab->SuspendLayout();
			this->forwardPanel->SuspendLayout();
			this->VRtab->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->Spans);
			this->groupBox1->Controls->Add(this->dataLiveCheckBox);
			this->groupBox1->Controls->Add(this->lastAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->firstAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->spanSelector);
			this->groupBox1->Controls->Add(this->scrollBar);
			this->groupBox1->Location = System::Drawing::Point(95, 4);
			this->groupBox1->Margin = System::Windows::Forms::Padding(4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(4);
			this->groupBox1->Size = System::Drawing::Size(1011, 91);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Time Series";
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
			this->dataLiveCheckBox->AutoSize = true;
			this->dataLiveCheckBox->Checked = true;
			this->dataLiveCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dataLiveCheckBox->Location = System::Drawing::Point(958, 33);
			this->dataLiveCheckBox->Name = L"dataLiveCheckBox";
			this->dataLiveCheckBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->dataLiveCheckBox->Size = System::Drawing::Size(46, 17);
			this->dataLiveCheckBox->TabIndex = 9;
			this->dataLiveCheckBox->Text = L"Live";
			this->dataLiveCheckBox->UseVisualStyleBackColor = true;
			this->dataLiveCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::dataLiveCheckBox_CheckedChanged);
			// 
			// lastAbsoluteTimeTextBox
			// 
			this->lastAbsoluteTimeTextBox->Location = System::Drawing::Point(871, 58);
			this->lastAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->lastAbsoluteTimeTextBox->Name = L"lastAbsoluteTimeTextBox";
			this->lastAbsoluteTimeTextBox->Size = System::Drawing::Size(72, 21);
			this->lastAbsoluteTimeTextBox->TabIndex = 3;
			this->lastAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
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
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::spanSelector_ValueChanged);
			// 
			// scrollBar
			// 
			this->scrollBar->LargeChange = 25;
			this->scrollBar->Location = System::Drawing::Point(198, 31);
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(745, 23);
			this->scrollBar->TabIndex = 0;
			this->scrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::scrollBar_ValueChanged);
			// 
			// poseGraphGroupBox
			// 
			this->poseGraphGroupBox->Controls->Add(this->poseGraphPanel);
			this->poseGraphGroupBox->Location = System::Drawing::Point(8, 96);
			this->poseGraphGroupBox->Name = L"poseGraphGroupBox";
			this->poseGraphGroupBox->Size = System::Drawing::Size(1098, 644);
			this->poseGraphGroupBox->TabIndex = 1;
			this->poseGraphGroupBox->TabStop = false;
			this->poseGraphGroupBox->Text = L"Kinematics";
			// 
			// poseGraphPanel
			// 
			this->poseGraphPanel->ContextMenuStrip = this->hmdContextMenu;
			this->poseGraphPanel->Controls->Add(this->autoscaleIndicator);
			this->poseGraphPanel->Location = System::Drawing::Point(6, 23);
			this->poseGraphPanel->Name = L"poseGraphPanel";
			this->poseGraphPanel->Size = System::Drawing::Size(1082, 615);
			this->poseGraphPanel->TabIndex = 0;
			// 
			// hmdContextMenu
			// 
			this->hmdContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->autoscaleHMD});
			this->hmdContextMenu->Name = L"contextMenuStrip1";
			this->hmdContextMenu->ShowCheckMargin = true;
			this->hmdContextMenu->ShowImageMargin = false;
			this->hmdContextMenu->Size = System::Drawing::Size(133, 26);
			this->hmdContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::hmdContextMenu_ItemClicked);
			// 
			// autoscaleHMD
			// 
			this->autoscaleHMD->CheckOnClick = true;
			this->autoscaleHMD->Name = L"autoscaleHMD";
			this->autoscaleHMD->Size = System::Drawing::Size(132, 22);
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
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->taskGraphPanel);
			this->groupBox3->Location = System::Drawing::Point(9, 856);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(1098, 110);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Task Execution";
			// 
			// taskGraphPanel
			// 
			this->taskGraphPanel->ContextMenuStrip = this->taskContextMenu;
			this->taskGraphPanel->Location = System::Drawing::Point(6, 23);
			this->taskGraphPanel->Name = L"taskGraphPanel";
			this->taskGraphPanel->Size = System::Drawing::Size(1082, 80);
			this->taskGraphPanel->TabIndex = 1;
			// 
			// taskContextMenu
			// 
			this->taskContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(14) {this->toolStripMenuItemAll, 
				this->toolStripSeparator1, this->toolStripTextBox1, this->toolStripMenuItem100, this->toolStripMenuItem200, this->toolStripMenuItem300, 
				this->toolStripSeparator2, this->toolStripTextBox2, this->toolStripMenuItem400, this->toolStripMenuItem500, this->toolStripMenuItem600, 
				this->toolStripSeparator3, this->toolStripMenuItem000, this->toolStripMenuItem900});
			this->taskContextMenu->Name = L"taskContextMenu";
			this->taskContextMenu->Size = System::Drawing::Size(179, 268);
			this->taskContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::taskContextMenu_ItemClicked);
			// 
			// toolStripMenuItemAll
			// 
			this->toolStripMenuItemAll->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
			this->toolStripMenuItemAll->ImageScaling = System::Windows::Forms::ToolStripItemImageScaling::None;
			this->toolStripMenuItemAll->Name = L"toolStripMenuItemAll";
			this->toolStripMenuItemAll->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItemAll->Tag = L"";
			this->toolStripMenuItemAll->Text = L"Show All";
			this->toolStripMenuItemAll->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(175, 6);
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
			this->toolStripMenuItem100->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem100->Tag = L"100";
			this->toolStripMenuItem100->Text = L"100 Visual-Manual";
			// 
			// toolStripMenuItem200
			// 
			this->toolStripMenuItem200->Name = L"toolStripMenuItem200";
			this->toolStripMenuItem200->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem200->Tag = L"200";
			this->toolStripMenuItem200->Text = L"200 Visual-Visual";
			// 
			// toolStripMenuItem300
			// 
			this->toolStripMenuItem300->Name = L"toolStripMenuItem300";
			this->toolStripMenuItem300->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem300->Tag = L"300";
			this->toolStripMenuItem300->Text = L"300 Manual-Manual";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(175, 6);
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
			this->toolStripMenuItem400->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem400->Tag = L"400";
			this->toolStripMenuItem400->Text = L"400 Visual-Visual";
			// 
			// toolStripMenuItem500
			// 
			this->toolStripMenuItem500->Name = L"toolStripMenuItem500";
			this->toolStripMenuItem500->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem500->Tag = L"500";
			this->toolStripMenuItem500->Text = L"500 Visual-Manual";
			// 
			// toolStripMenuItem600
			// 
			this->toolStripMenuItem600->Name = L"toolStripMenuItem600";
			this->toolStripMenuItem600->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem600->Tag = L"600";
			this->toolStripMenuItem600->Text = L"600 Manual-Manual";
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(175, 6);
			// 
			// toolStripMenuItem000
			// 
			this->toolStripMenuItem000->Name = L"toolStripMenuItem000";
			this->toolStripMenuItem000->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem000->Tag = L"0";
			this->toolStripMenuItem000->Text = L"000 Setup / Stow";
			// 
			// toolStripMenuItem900
			// 
			this->toolStripMenuItem900->Name = L"toolStripMenuItem900";
			this->toolStripMenuItem900->Size = System::Drawing::Size(178, 22);
			this->toolStripMenuItem900->Tag = L"900";
			this->toolStripMenuItem900->Text = L"900 Maintenance";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->markerGraphPanel);
			this->groupBox4->Location = System::Drawing::Point(9, 743);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(1098, 111);
			this->groupBox4->TabIndex = 2;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Marker Visibility";
			// 
			// markerGraphPanel
			// 
			this->markerGraphPanel->Location = System::Drawing::Point(6, 23);
			this->markerGraphPanel->Name = L"markerGraphPanel";
			this->markerGraphPanel->Size = System::Drawing::Size(1082, 80);
			this->markerGraphPanel->TabIndex = 0;
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
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Transparent;
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(7, 11);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(81, 84);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 10;
			this->pictureBox1->TabStop = false;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->codaPanel1);
			this->groupBox2->Controls->Add(this->codaPanel0);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(1111, 214);
			this->groupBox2->Margin = System::Windows::Forms::Padding(2);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(2);
			this->groupBox2->Size = System::Drawing::Size(418, 196);
			this->groupBox2->TabIndex = 21;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Coda  View";
			// 
			// codaPanel1
			// 
			this->codaPanel1->Location = System::Drawing::Point(218, 22);
			this->codaPanel1->Margin = System::Windows::Forms::Padding(2);
			this->codaPanel1->Name = L"codaPanel1";
			this->codaPanel1->Size = System::Drawing::Size(192, 167);
			this->codaPanel1->TabIndex = 1;
			// 
			// codaPanel0
			// 
			this->codaPanel0->Location = System::Drawing::Point(13, 22);
			this->codaPanel0->Margin = System::Windows::Forms::Padding(2);
			this->codaPanel0->Name = L"codaPanel0";
			this->codaPanel0->Size = System::Drawing::Size(192, 167);
			this->codaPanel0->TabIndex = 0;
			// 
			// chestGroupBox
			// 
			this->chestGroupBox->Controls->Add(this->chestPanel1);
			this->chestGroupBox->Controls->Add(this->chestPanel0);
			this->chestGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chestGroupBox->Location = System::Drawing::Point(1111, 802);
			this->chestGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->chestGroupBox->Name = L"chestGroupBox";
			this->chestGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->chestGroupBox->Size = System::Drawing::Size(418, 196);
			this->chestGroupBox->TabIndex = 18;
			this->chestGroupBox->TabStop = false;
			this->chestGroupBox->Text = L"Chest Marker Visibility";
			// 
			// chestPanel1
			// 
			this->chestPanel1->Location = System::Drawing::Point(218, 22);
			this->chestPanel1->Margin = System::Windows::Forms::Padding(2);
			this->chestPanel1->Name = L"chestPanel1";
			this->chestPanel1->Size = System::Drawing::Size(192, 167);
			this->chestPanel1->TabIndex = 2;
			// 
			// chestPanel0
			// 
			this->chestPanel0->Location = System::Drawing::Point(9, 22);
			this->chestPanel0->Margin = System::Windows::Forms::Padding(2);
			this->chestPanel0->Name = L"chestPanel0";
			this->chestPanel0->Size = System::Drawing::Size(192, 167);
			this->chestPanel0->TabIndex = 1;
			// 
			// handGroupBox
			// 
			this->handGroupBox->Controls->Add(this->handPanel1);
			this->handGroupBox->Controls->Add(this->handPanel0);
			this->handGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->handGroupBox->Location = System::Drawing::Point(1111, 606);
			this->handGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->handGroupBox->Name = L"handGroupBox";
			this->handGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->handGroupBox->Size = System::Drawing::Size(418, 196);
			this->handGroupBox->TabIndex = 17;
			this->handGroupBox->TabStop = false;
			this->handGroupBox->Text = L"Hand Marker Visibility";
			// 
			// handPanel1
			// 
			this->handPanel1->Location = System::Drawing::Point(218, 22);
			this->handPanel1->Margin = System::Windows::Forms::Padding(2);
			this->handPanel1->Name = L"handPanel1";
			this->handPanel1->Size = System::Drawing::Size(192, 167);
			this->handPanel1->TabIndex = 2;
			// 
			// handPanel0
			// 
			this->handPanel0->Location = System::Drawing::Point(9, 22);
			this->handPanel0->Margin = System::Windows::Forms::Padding(2);
			this->handPanel0->Name = L"handPanel0";
			this->handPanel0->Size = System::Drawing::Size(192, 167);
			this->handPanel0->TabIndex = 1;
			// 
			// hmdGroupBox
			// 
			this->hmdGroupBox->Controls->Add(this->fromCodaCheckBox);
			this->hmdGroupBox->Controls->Add(this->hmdPanel1);
			this->hmdGroupBox->Controls->Add(this->hmdPanel0);
			this->hmdGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->hmdGroupBox->Location = System::Drawing::Point(1111, 410);
			this->hmdGroupBox->Margin = System::Windows::Forms::Padding(2);
			this->hmdGroupBox->Name = L"hmdGroupBox";
			this->hmdGroupBox->Padding = System::Windows::Forms::Padding(2);
			this->hmdGroupBox->Size = System::Drawing::Size(418, 196);
			this->hmdGroupBox->TabIndex = 16;
			this->hmdGroupBox->TabStop = false;
			this->hmdGroupBox->Text = L"HMD Marker Visibility";
			// 
			// fromCodaCheckBox
			// 
			this->fromCodaCheckBox->AutoSize = true;
			this->fromCodaCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->fromCodaCheckBox->Location = System::Drawing::Point(305, 2);
			this->fromCodaCheckBox->Name = L"fromCodaCheckBox";
			this->fromCodaCheckBox->Size = System::Drawing::Size(105, 17);
			this->fromCodaCheckBox->TabIndex = 3;
			this->fromCodaCheckBox->Text = L"View from CODA";
			this->fromCodaCheckBox->UseVisualStyleBackColor = true;
			// 
			// hmdPanel1
			// 
			this->hmdPanel1->Location = System::Drawing::Point(218, 22);
			this->hmdPanel1->Margin = System::Windows::Forms::Padding(2);
			this->hmdPanel1->Name = L"hmdPanel1";
			this->hmdPanel1->Size = System::Drawing::Size(192, 167);
			this->hmdPanel1->TabIndex = 2;
			// 
			// hmdPanel0
			// 
			this->hmdPanel0->Location = System::Drawing::Point(9, 22);
			this->hmdPanel0->Margin = System::Windows::Forms::Padding(2);
			this->hmdPanel0->Name = L"hmdPanel0";
			this->hmdPanel0->Size = System::Drawing::Size(192, 167);
			this->hmdPanel0->TabIndex = 1;
			// 
			// worldTabs
			// 
			this->worldTabs->Controls->Add(this->trackerTab);
			this->worldTabs->Controls->Add(this->VRtab);
			this->worldTabs->Location = System::Drawing::Point(1112, 4);
			this->worldTabs->Name = L"worldTabs";
			this->worldTabs->SelectedIndex = 0;
			this->worldTabs->Size = System::Drawing::Size(417, 209);
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
			this->trackerTab->Size = System::Drawing::Size(409, 181);
			this->trackerTab->TabIndex = 0;
			this->trackerTab->Text = L"Tracker";
			// 
			// forwardPanel
			// 
			this->forwardPanel->Controls->Add(this->alignmentFrameTextBox);
			this->forwardPanel->Location = System::Drawing::Point(211, 7);
			this->forwardPanel->Margin = System::Windows::Forms::Padding(2);
			this->forwardPanel->Name = L"forwardPanel";
			this->forwardPanel->Size = System::Drawing::Size(191, 167);
			this->forwardPanel->TabIndex = 3;
			// 
			// alignmentFrameTextBox
			// 
			this->alignmentFrameTextBox->Location = System::Drawing::Point(89, 4);
			this->alignmentFrameTextBox->Margin = System::Windows::Forms::Padding(4);
			this->alignmentFrameTextBox->Name = L"alignmentFrameTextBox";
			this->alignmentFrameTextBox->Size = System::Drawing::Size(98, 21);
			this->alignmentFrameTextBox->TabIndex = 27;
			this->alignmentFrameTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// sidePanel
			// 
			this->sidePanel->Location = System::Drawing::Point(7, 7);
			this->sidePanel->Margin = System::Windows::Forms::Padding(2);
			this->sidePanel->Name = L"sidePanel";
			this->sidePanel->Size = System::Drawing::Size(191, 167);
			this->sidePanel->TabIndex = 2;
			// 
			// VRtab
			// 
			this->VRtab->Controls->Add(this->vrSidePanel);
			this->VRtab->Controls->Add(this->vrSubjectPanel);
			this->VRtab->Location = System::Drawing::Point(4, 22);
			this->VRtab->Name = L"VRtab";
			this->VRtab->Padding = System::Windows::Forms::Padding(3);
			this->VRtab->Size = System::Drawing::Size(409, 183);
			this->VRtab->TabIndex = 1;
			this->VRtab->Text = L"  VR";
			this->VRtab->UseVisualStyleBackColor = true;
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
			// vrFrameTextBox
			// 
			this->vrFrameTextBox->Location = System::Drawing::Point(1280, 1010);
			this->vrFrameTextBox->Margin = System::Windows::Forms::Padding(4);
			this->vrFrameTextBox->Name = L"vrFrameTextBox";
			this->vrFrameTextBox->Size = System::Drawing::Size(85, 21);
			this->vrFrameTextBox->TabIndex = 27;
			this->vrFrameTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// stepForwardButton
			// 
			this->stepForwardButton->Location = System::Drawing::Point(1377, 1009);
			this->stepForwardButton->Name = L"stepForwardButton";
			this->stepForwardButton->Size = System::Drawing::Size(37, 24);
			this->stepForwardButton->TabIndex = 28;
			this->stepForwardButton->Text = L">";
			this->stepForwardButton->UseVisualStyleBackColor = true;
			this->stepForwardButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::stepForwardButton_Click);
			// 
			// stepBackwardButton
			// 
			this->stepBackwardButton->Location = System::Drawing::Point(1231, 1009);
			this->stepBackwardButton->Name = L"stepBackwardButton";
			this->stepBackwardButton->Size = System::Drawing::Size(37, 24);
			this->stepBackwardButton->TabIndex = 29;
			this->stepBackwardButton->Text = L"<";
			this->stepBackwardButton->UseVisualStyleBackColor = true;
			this->stepBackwardButton->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &GraspMMIGraphsForm::stepBackwardButton_MouseClick);
			// 
			// playbackScrollBar
			// 
			this->playbackScrollBar->LargeChange = 2;
			this->playbackScrollBar->Location = System::Drawing::Point(9, 989);
			this->playbackScrollBar->Maximum = 1000;
			this->playbackScrollBar->Name = L"playbackScrollBar";
			this->playbackScrollBar->Size = System::Drawing::Size(1097, 23);
			this->playbackScrollBar->TabIndex = 25;
			this->playbackScrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::playbackScrollBar_ValueChanged);
			// 
			// taskRightTimeLimit
			// 
			this->taskRightTimeLimit->Location = System::Drawing::Point(1020, 1016);
			this->taskRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskRightTimeLimit->Name = L"taskRightTimeLimit";
			this->taskRightTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskRightTimeLimit->TabIndex = 24;
			this->taskRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// taskLeftTimeLimit
			// 
			this->taskLeftTimeLimit->Location = System::Drawing::Point(16, 1016);
			this->taskLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskLeftTimeLimit->Name = L"taskLeftTimeLimit";
			this->taskLeftTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskLeftTimeLimit->TabIndex = 23;
			this->taskLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// cursorPanel
			// 
			this->cursorPanel->ContextMenuStrip = this->taskContextMenu;
			this->cursorPanel->Location = System::Drawing::Point(18, 972);
			this->cursorPanel->Name = L"cursorPanel";
			this->cursorPanel->Size = System::Drawing::Size(1078, 12);
			this->cursorPanel->TabIndex = 30;
			// 
			// exitButton
			// 
			this->exitButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->exitButton->Location = System::Drawing::Point(1457, 1005);
			this->exitButton->Name = L"exitButton";
			this->exitButton->Size = System::Drawing::Size(69, 26);
			this->exitButton->TabIndex = 31;
			this->exitButton->Text = L"Exit";
			this->exitButton->UseVisualStyleBackColor = true;
			this->exitButton->Visible = false;
			this->exitButton->Click += gcnew System::EventHandler(this, &GraspMMIGraphsForm::button1_Click);
			// 
			// GraspMMIGraphsForm
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->CancelButton = this->exitButton;
			this->ClientSize = System::Drawing::Size(1538, 1042);
			this->Controls->Add(this->exitButton);
			this->Controls->Add(this->cursorPanel);
			this->Controls->Add(this->stepBackwardButton);
			this->Controls->Add(this->stepForwardButton);
			this->Controls->Add(this->vrFrameTextBox);
			this->Controls->Add(this->taskLeftTimeLimit);
			this->Controls->Add(this->taskRightTimeLimit);
			this->Controls->Add(this->worldTabs);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->playbackScrollBar);
			this->Controls->Add(this->chestGroupBox);
			this->Controls->Add(this->handGroupBox);
			this->Controls->Add(this->hmdGroupBox);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->poseGraphGroupBox);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox4);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Location = System::Drawing::Point(10, 20);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspMMIGraphsForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspMMI Data Plots";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspMMIGraphsForm::GraspMMIGraphsForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspMMIGraphsForm::GraspMMIGraphsForm_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->EndInit();
			this->poseGraphGroupBox->ResumeLayout(false);
			this->poseGraphPanel->ResumeLayout(false);
			this->poseGraphPanel->PerformLayout();
			this->hmdContextMenu->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->taskContextMenu->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->chestGroupBox->ResumeLayout(false);
			this->handGroupBox->ResumeLayout(false);
			this->hmdGroupBox->ResumeLayout(false);
			this->hmdGroupBox->PerformLayout();
			this->worldTabs->ResumeLayout(false);
			this->trackerTab->ResumeLayout(false);
			this->forwardPanel->ResumeLayout(false);
			this->forwardPanel->PerformLayout();
			this->VRtab->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private:

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

		// This is what we do when the timer goes off.
		void OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {

			// Stop the timer so that it does not retrigger until we are done refreshing.
			StopRefreshTimer();
			fOutputDebugString( "\n" );
			fOutputDebugString( "Timer triggered.\n" );
			// Get the realtime science data packets, if any. 
			ReadTelemetryCache( packetCacheFileRoot );
			// Adjust the scrollbar limits according to the newly loaded data.
			AdjustScrollSpan();
			//// If we are live, shift the limits of the plots to reflect the most recent data.
			//// Otherwise, keep the window span where it is.
			if ( dataLiveCheckBox->Checked ) {
				MoveToLatest();
				// MoveToLatest() has the side effect of unchecking the live check box, 
				// becaue it causes a value change in the slider which is presumed to be by user action.
				// So here we have to reset the check.
				dataLiveCheckBox->Checked = true;
			}
			RefreshGraphics();
			// Start the timer again to trigger the next cycle after a delay.
			StartRefreshTimer();

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
			CreateRefreshTimer( refreshInterval );
			StartRefreshTimer();
		}
		System::Void spanSelector_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			// Change the span of the data display.
			AdjustScrollSpan();
			// Display with the new span.
			RefreshGraphics();
			// If we are live, set up for the next refresh.
			if ( dataLiveCheckBox->Checked ) StartRefreshTimer();
			else StopRefreshTimer();
		}
		System::Void scrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			// Display the data at the new time specified by the scroll bar.
			RefreshGraphics();
			// Since we selected a time, then we are implicitly no longer live.
			dataLiveCheckBox->Checked = false;
		}
		System::Void GraspMMIGraphsForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			StopRefreshTimer();
			KillGraphics();
		}

		System::Void dataLiveCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if ( dataLiveCheckBox->Checked ) StartRefreshTimer();
			else StopRefreshTimer();
		}

		System::Void playbackScrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// Stop the timer so that we don't get overlapping refresh requests.
			StopRefreshTimer();
			if ( nDataSlices > 0 ) MoveToInstant( (double) playbackScrollBar->Value );
			// Since we selected a time, then we are implicitly no longer live.
			dataLiveCheckBox->Checked = false;
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
			StartRefreshTimer();
		}

		System::Void hmdContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			StartRefreshTimer();
		}
		System::Void worldTabs_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
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

		System::Void stepBackwardButton_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			unsigned int index;
			dataLiveCheckBox->Checked = false;
			for ( index = nDataSlices - 1; index > 0; index-- ) {
				if ( graspDataSlice[index].absoluteTime != MISSING_DOUBLE && graspDataSlice[index].absoluteTime < current_vr_instant ) break;
			}
			MoveToSlice( index );
		 }
		
		
		// Add an 'About ...' item to the system menu. 
#define SYSMENU_ABOUT_ID 0x01

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
					AppendMenu(hSysMenu, MF_STRING, SYSMENU_ABOUT_ID, "&About …");

				}

	protected:  virtual void WndProc(System::Windows::Forms::Message% m) override {	
					// Test if the About item was selected from the system menu
					if ((m.Msg == WM_SYSCOMMAND) && ((int)m.WParam == SYSMENU_ABOUT_ID))
					{
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
					// Do what one would normally do.
					Form::WndProc( m );
				}



private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 Close();
		 }
};
}

