#pragma once

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "..\PsyPhy2dGraphicsLib\Displays.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"

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

	/// <summary>
	/// Summary for GraspMMIGraphsForm
	/// </summary>
	public ref class GraspMMIGraphsForm : public System::Windows::Forms::Form
	{

	private: 

		///
		/// Display parameters.
		///

		static double	positionRadius = 500.0; // Range of XYZ values around zero in mm.
		static double	quaternionRadius = 0.5;	// Range for XYZ components of quaternions. Use 1.0 to see 360� rotations.
		static double	rotationRadius = 90.0;	// Range of rotation amplitudes, in degrees.
		static	int		refreshInterval = 500;	// How often to update the display, in milliseconds.

		double taskViewBottom;
		double taskViewTop;

		//
		// Objects used to plot to the screen.
		//

		Display		hmdDisplay;
		Display		markerDisplay;
		Display		historyDisplay;
		::Layout	hmdStripChartLayout;
		::Layout	tiltStripChartLayout;
		::Layout	markerStripChartLayout;
		::Layout	historyStripChartLayout;

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

	public:

		//
		// Parameters that should be initialized by the caller before running the form.
		//

		int	TimebaseOffset;				// Offset to convert GPS time to UTC.

	public: 
		String^ packetCacheFileRoot;	// Path to the packet cache files.
	private: System::Windows::Forms::Label^  Spans;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	public: 

	public: 

	public: 

	public: 
		String^ scriptDirectory;			// Path to root file of the GUI menu tree.

	public:

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
			taskViewTop( 1000.0 )
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
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  lastAbsoluteTimeTextBox;
	private: System::Windows::Forms::TextBox^  firstAbsoluteTimeTextBox;
	private: System::Windows::Forms::TrackBar^  spanSelector;
	private: System::Windows::Forms::HScrollBar^  scrollBar;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Panel^  hmdGraphPanel;
	private: System::Windows::Forms::Panel^  taskGraphPanel;
	private: System::Windows::Forms::TextBox^  taskRightTimeLimit;
	private: System::Windows::Forms::TextBox^  taskLeftTimeLimit;
	private: System::Windows::Forms::CheckBox^  dataLiveCheckBox;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Panel^  markerGraphPanel;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::TreeView^  historyTree;
	private: System::Windows::Forms::TreeView^  visibleHistoryTree;
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
	private: System::Windows::Forms::ContextMenuStrip^  taskTreeContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  clearItemErrorHighlight;
	private: System::Windows::Forms::TextBox^  autoscaleIndicator;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
	private: System::Windows::Forms::ToolStripMenuItem^  clearAllErrorHighlights;
	private: System::Windows::Forms::ToolStripMenuItem^  rebuildTree;


private: System::ComponentModel::IContainer^  components;

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
			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"This tree will be hidden."));
			System::Windows::Forms::TreeNode^  treeNode2 = (gcnew System::Windows::Forms::TreeNode(L"Node0"));
			System::Windows::Forms::TreeNode^  treeNode3 = (gcnew System::Windows::Forms::TreeNode(L"Node1"));
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GraspMMIGraphsForm::typeid));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->Spans = (gcnew System::Windows::Forms::Label());
			this->dataLiveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->lastAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->firstAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->hmdGraphPanel = (gcnew System::Windows::Forms::Panel());
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
			this->taskLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->markerGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->historyTree = (gcnew System::Windows::Forms::TreeView());
			this->visibleHistoryTree = (gcnew System::Windows::Forms::TreeView());
			this->taskTreeContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->rebuildTree = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->clearItemErrorHighlight = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearAllErrorHighlights = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->hmdGraphPanel->SuspendLayout();
			this->hmdContextMenu->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->taskContextMenu->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->taskTreeContextMenu->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
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
			this->Spans->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Spans->Location = System::Drawing::Point(10, 58);
			this->Spans->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->Spans->Name = L"Spans";
			this->Spans->Size = System::Drawing::Size(164, 13);
			this->Spans->TabIndex = 13;
			this->Spans->Text = L"12h  4h  1h 30m 10m 5m 60s 30s";
			// 
			// dataLiveCheckBox
			// 
			this->dataLiveCheckBox->AutoSize = true;
			this->dataLiveCheckBox->Checked = true;
			this->dataLiveCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dataLiveCheckBox->Location = System::Drawing::Point(958, 33);
			this->dataLiveCheckBox->Name = L"dataLiveCheckBox";
			this->dataLiveCheckBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->dataLiveCheckBox->Size = System::Drawing::Size(48, 19);
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
			this->firstAbsoluteTimeTextBox->Location = System::Drawing::Point(182, 58);
			this->firstAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->firstAbsoluteTimeTextBox->Name = L"firstAbsoluteTimeTextBox";
			this->firstAbsoluteTimeTextBox->Size = System::Drawing::Size(72, 21);
			this->firstAbsoluteTimeTextBox->TabIndex = 2;
			this->firstAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// spanSelector
			// 
			this->spanSelector->LargeChange = 1;
			this->spanSelector->Location = System::Drawing::Point(12, 23);
			this->spanSelector->Margin = System::Windows::Forms::Padding(4);
			this->spanSelector->Maximum = 7;
			this->spanSelector->Name = L"spanSelector";
			this->spanSelector->Size = System::Drawing::Size(162, 42);
			this->spanSelector->TabIndex = 1;
			this->spanSelector->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->spanSelector->Value = 2;
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::spanSelector_ValueChanged);
			// 
			// scrollBar
			// 
			this->scrollBar->LargeChange = 25;
			this->scrollBar->Location = System::Drawing::Point(182, 31);
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(761, 23);
			this->scrollBar->TabIndex = 0;
			this->scrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::scrollBar_ValueChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->hmdGraphPanel);
			this->groupBox2->Location = System::Drawing::Point(8, 96);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(1098, 532);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Head";
			// 
			// hmdGraphPanel
			// 
			this->hmdGraphPanel->ContextMenuStrip = this->hmdContextMenu;
			this->hmdGraphPanel->Controls->Add(this->autoscaleIndicator);
			this->hmdGraphPanel->Location = System::Drawing::Point(6, 23);
			this->hmdGraphPanel->Name = L"hmdGraphPanel";
			this->hmdGraphPanel->Size = System::Drawing::Size(1082, 500);
			this->hmdGraphPanel->TabIndex = 0;
			// 
			// hmdContextMenu
			// 
			this->hmdContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->autoscaleHMD});
			this->hmdContextMenu->Name = L"contextMenuStrip1";
			this->hmdContextMenu->ShowCheckMargin = true;
			this->hmdContextMenu->ShowImageMargin = false;
			this->hmdContextMenu->Size = System::Drawing::Size(122, 26);
			this->hmdContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::hmdContextMenu_ItemClicked);
			// 
			// autoscaleHMD
			// 
			this->autoscaleHMD->CheckOnClick = true;
			this->autoscaleHMD->Name = L"autoscaleHMD";
			this->autoscaleHMD->Size = System::Drawing::Size(121, 22);
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
			this->groupBox3->Controls->Add(this->taskLeftTimeLimit);
			this->groupBox3->Controls->Add(this->taskRightTimeLimit);
			this->groupBox3->Location = System::Drawing::Point(8, 750);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(1098, 252);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Task Execution";
			// 
			// taskGraphPanel
			// 
			this->taskGraphPanel->ContextMenuStrip = this->taskContextMenu;
			this->taskGraphPanel->Location = System::Drawing::Point(6, 23);
			this->taskGraphPanel->Name = L"taskGraphPanel";
			this->taskGraphPanel->Size = System::Drawing::Size(1082, 190);
			this->taskGraphPanel->TabIndex = 1;
			// 
			// taskContextMenu
			// 
			this->taskContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(14) {this->toolStripMenuItemAll, 
				this->toolStripSeparator1, this->toolStripTextBox1, this->toolStripMenuItem100, this->toolStripMenuItem200, this->toolStripMenuItem300, 
				this->toolStripSeparator2, this->toolStripTextBox2, this->toolStripMenuItem400, this->toolStripMenuItem500, this->toolStripMenuItem600, 
				this->toolStripSeparator3, this->toolStripMenuItem000, this->toolStripMenuItem900});
			this->taskContextMenu->Name = L"taskContextMenu";
			this->taskContextMenu->Size = System::Drawing::Size(168, 268);
			this->taskContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::taskContextMenu_ItemClicked);
			// 
			// toolStripMenuItemAll
			// 
			this->toolStripMenuItemAll->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
			this->toolStripMenuItemAll->ImageScaling = System::Windows::Forms::ToolStripItemImageScaling::None;
			this->toolStripMenuItemAll->Name = L"toolStripMenuItemAll";
			this->toolStripMenuItemAll->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItemAll->Tag = L"";
			this->toolStripMenuItemAll->Text = L"Show All";
			this->toolStripMenuItemAll->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(164, 6);
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
			this->toolStripMenuItem100->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem100->Tag = L"100";
			this->toolStripMenuItem100->Text = L"100 Visual-Visual";
			// 
			// toolStripMenuItem200
			// 
			this->toolStripMenuItem200->Name = L"toolStripMenuItem200";
			this->toolStripMenuItem200->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem200->Tag = L"200";
			this->toolStripMenuItem200->Text = L"200 Visual-Manual";
			// 
			// toolStripMenuItem300
			// 
			this->toolStripMenuItem300->Name = L"toolStripMenuItem300";
			this->toolStripMenuItem300->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem300->Tag = L"300";
			this->toolStripMenuItem300->Text = L"300 Manual-Manual";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(164, 6);
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
			this->toolStripMenuItem400->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem400->Tag = L"400";
			this->toolStripMenuItem400->Text = L"400 Visual-Visual";
			// 
			// toolStripMenuItem500
			// 
			this->toolStripMenuItem500->Name = L"toolStripMenuItem500";
			this->toolStripMenuItem500->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem500->Tag = L"500";
			this->toolStripMenuItem500->Text = L"500 Visual-Manual";
			// 
			// toolStripMenuItem600
			// 
			this->toolStripMenuItem600->Name = L"toolStripMenuItem600";
			this->toolStripMenuItem600->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem600->Tag = L"600";
			this->toolStripMenuItem600->Text = L"600 Manual-Manual";
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(164, 6);
			// 
			// toolStripMenuItem000
			// 
			this->toolStripMenuItem000->Name = L"toolStripMenuItem000";
			this->toolStripMenuItem000->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem000->Tag = L"0";
			this->toolStripMenuItem000->Text = L"000 Setup / Stow";
			// 
			// toolStripMenuItem900
			// 
			this->toolStripMenuItem900->Name = L"toolStripMenuItem900";
			this->toolStripMenuItem900->Size = System::Drawing::Size(167, 22);
			this->toolStripMenuItem900->Tag = L"900";
			this->toolStripMenuItem900->Text = L"900 Maintenance";
			// 
			// taskLeftTimeLimit
			// 
			this->taskLeftTimeLimit->Location = System::Drawing::Point(12, 220);
			this->taskLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskLeftTimeLimit->Name = L"taskLeftTimeLimit";
			this->taskLeftTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskLeftTimeLimit->TabIndex = 7;
			this->taskLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// taskRightTimeLimit
			// 
			this->taskRightTimeLimit->Location = System::Drawing::Point(1016, 220);
			this->taskRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskRightTimeLimit->Name = L"taskRightTimeLimit";
			this->taskRightTimeLimit->Size = System::Drawing::Size(72, 21);
			this->taskRightTimeLimit->TabIndex = 8;
			this->taskRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->markerGraphPanel);
			this->groupBox4->Location = System::Drawing::Point(8, 629);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(1098, 120);
			this->groupBox4->TabIndex = 2;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Marker Visibility";
			// 
			// markerGraphPanel
			// 
			this->markerGraphPanel->Location = System::Drawing::Point(6, 23);
			this->markerGraphPanel->Name = L"markerGraphPanel";
			this->markerGraphPanel->Size = System::Drawing::Size(1082, 91);
			this->markerGraphPanel->TabIndex = 0;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->historyTree);
			this->groupBox5->Controls->Add(this->visibleHistoryTree);
			this->groupBox5->Location = System::Drawing::Point(1113, 4);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(437, 998);
			this->groupBox5->TabIndex = 9;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Task Execution History";
			// 
			// historyTree
			// 
			this->historyTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyTree->Location = System::Drawing::Point(199, 8);
			this->historyTree->Name = L"historyTree";
			treeNode1->ForeColor = System::Drawing::Color::Blue;
			treeNode1->Name = L"Node0";
			treeNode1->Text = L"This tree will be hidden.";
			this->historyTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode1});
			this->historyTree->Size = System::Drawing::Size(207, 23);
			this->historyTree->TabIndex = 0;
			this->historyTree->Visible = false;
			// 
			// visibleHistoryTree
			// 
			this->visibleHistoryTree->ContextMenuStrip = this->taskTreeContextMenu;
			this->visibleHistoryTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->visibleHistoryTree->Location = System::Drawing::Point(6, 23);
			this->visibleHistoryTree->Name = L"visibleHistoryTree";
			treeNode2->ForeColor = System::Drawing::Color::Blue;
			treeNode2->Name = L"Node0";
			treeNode2->Text = L"Node0";
			treeNode3->ForeColor = System::Drawing::SystemColors::WindowText;
			treeNode3->Name = L"Node1";
			treeNode3->Text = L"Node1";
			this->visibleHistoryTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(2) {treeNode2, treeNode3});
			this->visibleHistoryTree->Size = System::Drawing::Size(425, 964);
			this->visibleHistoryTree->TabIndex = 1;
			this->visibleHistoryTree->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &GraspMMIGraphsForm::visibleHistoryTree_NodeMouseClick);
			// 
			// taskTreeContextMenu
			// 
			this->taskTreeContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->rebuildTree, 
				this->toolStripSeparator4, this->clearItemErrorHighlight, this->clearAllErrorHighlights});
			this->taskTreeContextMenu->Name = L"taskTreeContextMenu";
			this->taskTreeContextMenu->ShowImageMargin = false;
			this->taskTreeContextMenu->Size = System::Drawing::Size(168, 76);
			this->taskTreeContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIGraphsForm::taskTreeContextMenu_ItemClicked);
			// 
			// rebuildTree
			// 
			this->rebuildTree->Name = L"rebuildTree";
			this->rebuildTree->Size = System::Drawing::Size(167, 22);
			this->rebuildTree->Text = L"Rebuild Tree";
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(164, 6);
			// 
			// clearItemErrorHighlight
			// 
			this->clearItemErrorHighlight->Name = L"clearItemErrorHighlight";
			this->clearItemErrorHighlight->Size = System::Drawing::Size(167, 22);
			this->clearItemErrorHighlight->Text = L"Clear This Error Highlight";
			// 
			// clearAllErrorHighlights
			// 
			this->clearAllErrorHighlights->Name = L"clearAllErrorHighlights";
			this->clearAllErrorHighlights->Size = System::Drawing::Size(167, 22);
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
			// GraspMMIGraphsForm
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(1552, 1008);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
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
			this->groupBox2->ResumeLayout(false);
			this->hmdGraphPanel->ResumeLayout(false);
			this->hmdGraphPanel->PerformLayout();
			this->hmdContextMenu->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->taskContextMenu->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->taskTreeContextMenu->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

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
	private: void BuildHistoryTree( void );
	private: void RebuildHistoryTree( void );
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
			// Update the history tree.
			BuildHistoryTree();
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

		System::Void GraspMMIGraphsForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			InitializeGraphics();
			ParseSubjectFile( historyTree, scriptDirectory + gcnew String( "Subjects.sbj" ) );
			visibleHistoryTree->Nodes->Clear();
			ReadTelemetryCache( packetCacheFileRoot );
			AdjustScrollSpan();
			dataLiveCheckBox->Checked = true;
			BuildHistoryTree();
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
			KillGraphics();
		}

		System::Void dataLiveCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if ( dataLiveCheckBox->Checked ) StartRefreshTimer();
			else StopRefreshTimer();
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


		System::Void taskTreeContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			TreeNode^ node = visibleHistoryTree->SelectedNode;
			StopRefreshTimer();
			if ( e->ClickedItem == clearItemErrorHighlight ) {
				// Completed nodes that have errors persist as red in the tree.
				// Here we give the user the possibility to clear the red color as a way of acknowledging the error.
				// This makes newly occuring errors stand out more.
				if ( node->Text->EndsWith( "!" ) && node->ForeColor == System::Drawing::Color::Red ) {
					node->ForeColor = System::Drawing::SystemColors::WindowText;
				}
			}
			else if (  e->ClickedItem == clearAllErrorHighlights ) {
				// They can all be cleared as once.
				ClearAllErrorHighlights();
			}
			else if (  e->ClickedItem == rebuildTree ) {
				// Rebuilding the tree restores all of the error hightlights.
				RebuildHistoryTree();
			}
			StartRefreshTimer();
		}		 

		System::Void ClearAllErrorHighlights( void ) {
			StopRefreshTimer();
			for ( int i = 0; i < visibleHistoryTree->Nodes->Count; i++ ) {
				TreeNode^ subject_node = visibleHistoryTree->Nodes[i];
				for ( int i = 0; i < subject_node->Nodes->Count; i++ ) {
					TreeNode^ protocol_node = subject_node->Nodes[i];
					for ( int i = 0; i < protocol_node->Nodes->Count; i++ ) {
						TreeNode^ task_node = protocol_node->Nodes[i];
						for ( int i = 0; i < task_node->Nodes->Count; i++ ) {
							TreeNode^ task_leaf = task_node->Nodes[i];
							if ( task_leaf->Text->EndsWith( "!" ) && task_leaf->ForeColor == System::Drawing::Color::Red ) {
								task_leaf->ForeColor = System::Drawing::SystemColors::WindowText;
							}
						}
					}
				}
			}
			StartRefreshTimer();
		}		 

		System::Void visibleHistoryTree_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
			// Normal behavior for a left click is to select the node, but right click leaves the selection unchanged.
			// Here we force any click to select the node, which is convenient for setting the target of the context menu.
			visibleHistoryTree->SelectedNode = e->Node;
			if ( e->Node->Text->EndsWith( "!" ) && e->Node->ForeColor == System::Drawing::Color::Red ) clearItemErrorHighlight->Enabled = true;
			else clearItemErrorHighlight->Enabled = false;

		 }
	private: System::Void hmdContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			StartRefreshTimer();
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
					AppendMenu(hSysMenu, MF_STRING, SYSMENU_ABOUT_ID, "&About �");

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
							"GraspMMIMirror Version Info", "Source Release:  %s\n         Build Info:  %s\n\nPacket Caches:\n\n  %s\n  %s\n\nExecuting in:\n\n  %s\n", 
							GripGraspSourceRelease, GripGraspBuildInfo, hk_file, rt_file, pwd );
						// Release the memory used to create the ANSI string.
						Marshal::FreeHGlobal( IntPtr( cache_root ) );

						return;
					}
					// Do what one would normally do.
					Form::WndProc( m );
				}


};
}

