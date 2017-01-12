#pragma once

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../GraspMMIUtilities/GraspMMIUtilities.h"
#include "../GraspGUI/GraspScripts.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Displays.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Views.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Layouts.h"

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
		static double	quaternionRadius = 0.5;	// Range for XYZ components of quaternions. Use 1.0 to see 360° rotations.
		static double	rotationRadius = 90.0;	// Range of rotation amplitudes, in degrees.
		static	int		refreshInterval = 2000;	// How often to update the display, in milliseconds.

		//
		// Objects used to plot to the screen.
		//

		Display		hmdDisplay;
		Display		markerDisplay;
		Display		historyDisplay;
		::Layout	hmdStripChartLayout;
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
	public: 
		String^ scriptDirectory;			// Path to root file of the GUI menu tree.

	public:

		GraspMMIGraphsForm() : 
			TimebaseOffset( -16 ), 
			nDataSlices( 0 ), 
			nHousekeepingSlices( 0 ),
			current_subject( 0 ),
			current_protocol( 0 ),
			current_task( 0 ),
			current_task_leaf( nullptr ),
			previous_task_leaf( nullptr ),
			current_task_start_time( 0.0 ),
			task_tree_current_index( 0 ),
			first_step( 0 )
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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->dataLiveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->lastAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->firstAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->hmdGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->taskGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->taskRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->markerGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->historyTree = (gcnew System::Windows::Forms::TreeView());
			this->visibleHistoryTree = (gcnew System::Windows::Forms::TreeView());
			this->hmdContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->autoscaleHMD = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->hmdContextMenu->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->dataLiveCheckBox);
			this->groupBox1->Controls->Add(this->lastAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->firstAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->spanSelector);
			this->groupBox1->Controls->Add(this->scrollBar);
			this->groupBox1->Location = System::Drawing::Point(13, 13);
			this->groupBox1->Margin = System::Windows::Forms::Padding(4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(4);
			this->groupBox1->Size = System::Drawing::Size(1328, 112);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Time Series";
			// 
			// dataLiveCheckBox
			// 
			this->dataLiveCheckBox->AutoSize = true;
			this->dataLiveCheckBox->Checked = true;
			this->dataLiveCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dataLiveCheckBox->Location = System::Drawing::Point(1257, 0);
			this->dataLiveCheckBox->Name = L"dataLiveCheckBox";
			this->dataLiveCheckBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->dataLiveCheckBox->Size = System::Drawing::Size(56, 22);
			this->dataLiveCheckBox->TabIndex = 9;
			this->dataLiveCheckBox->Text = L"Live";
			this->dataLiveCheckBox->UseVisualStyleBackColor = true;
			this->dataLiveCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::dataLiveCheckBox_CheckedChanged);
			// 
			// lastAbsoluteTimeTextBox
			// 
			this->lastAbsoluteTimeTextBox->Location = System::Drawing::Point(1176, 74);
			this->lastAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->lastAbsoluteTimeTextBox->Name = L"lastAbsoluteTimeTextBox";
			this->lastAbsoluteTimeTextBox->Size = System::Drawing::Size(112, 24);
			this->lastAbsoluteTimeTextBox->TabIndex = 3;
			this->lastAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// firstAbsoluteTimeTextBox
			// 
			this->firstAbsoluteTimeTextBox->Location = System::Drawing::Point(255, 74);
			this->firstAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->firstAbsoluteTimeTextBox->Name = L"firstAbsoluteTimeTextBox";
			this->firstAbsoluteTimeTextBox->Size = System::Drawing::Size(112, 24);
			this->firstAbsoluteTimeTextBox->TabIndex = 2;
			this->firstAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// spanSelector
			// 
			this->spanSelector->LargeChange = 1;
			this->spanSelector->Location = System::Drawing::Point(14, 30);
			this->spanSelector->Margin = System::Windows::Forms::Padding(4);
			this->spanSelector->Maximum = 7;
			this->spanSelector->Name = L"spanSelector";
			this->spanSelector->Size = System::Drawing::Size(199, 53);
			this->spanSelector->TabIndex = 1;
			this->spanSelector->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->spanSelector->Value = 2;
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::spanSelector_ValueChanged);
			// 
			// scrollBar
			// 
			this->scrollBar->LargeChange = 25;
			this->scrollBar->Location = System::Drawing::Point(234, 38);
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(1079, 23);
			this->scrollBar->TabIndex = 0;
			this->scrollBar->ValueChanged += gcnew System::EventHandler(this, &GraspMMIGraphsForm::scrollBar_ValueChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->hmdGraphPanel);
			this->groupBox2->Location = System::Drawing::Point(13, 131);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(1326, 613);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Head";
			// 
			// hmdGraphPanel
			// 
			this->hmdGraphPanel->ContextMenuStrip = this->hmdContextMenu;
			this->hmdGraphPanel->Location = System::Drawing::Point(6, 28);
			this->hmdGraphPanel->Name = L"hmdGraphPanel";
			this->hmdGraphPanel->Size = System::Drawing::Size(1313, 577);
			this->hmdGraphPanel->TabIndex = 0;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->taskGraphPanel);
			this->groupBox3->Location = System::Drawing::Point(16, 856);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(1326, 287);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Task History";
			// 
			// taskGraphPanel
			// 
			this->taskGraphPanel->Location = System::Drawing::Point(6, 31);
			this->taskGraphPanel->Name = L"taskGraphPanel";
			this->taskGraphPanel->Size = System::Drawing::Size(1310, 248);
			this->taskGraphPanel->TabIndex = 1;
			// 
			// taskRightTimeLimit
			// 
			this->taskRightTimeLimit->Location = System::Drawing::Point(1214, 1150);
			this->taskRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskRightTimeLimit->Name = L"taskRightTimeLimit";
			this->taskRightTimeLimit->Size = System::Drawing::Size(112, 24);
			this->taskRightTimeLimit->TabIndex = 8;
			this->taskRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// taskLeftTimeLimit
			// 
			this->taskLeftTimeLimit->Location = System::Drawing::Point(31, 1150);
			this->taskLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskLeftTimeLimit->Name = L"taskLeftTimeLimit";
			this->taskLeftTimeLimit->Size = System::Drawing::Size(112, 24);
			this->taskLeftTimeLimit->TabIndex = 7;
			this->taskLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->markerGraphPanel);
			this->groupBox4->Location = System::Drawing::Point(15, 750);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(1326, 100);
			this->groupBox4->TabIndex = 2;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Marker Visibility";
			// 
			// markerGraphPanel
			// 
			this->markerGraphPanel->Location = System::Drawing::Point(6, 28);
			this->markerGraphPanel->Name = L"markerGraphPanel";
			this->markerGraphPanel->Size = System::Drawing::Size(1311, 64);
			this->markerGraphPanel->TabIndex = 0;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->historyTree);
			this->groupBox5->Controls->Add(this->visibleHistoryTree);
			this->groupBox5->Location = System::Drawing::Point(1348, 13);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(584, 1161);
			this->groupBox5->TabIndex = 9;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Task Execution History";
			// 
			// historyTree
			// 
			this->historyTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyTree->Location = System::Drawing::Point(350, 23);
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
			this->visibleHistoryTree->ContextMenuStrip = this->hmdContextMenu;
			this->visibleHistoryTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->visibleHistoryTree->Location = System::Drawing::Point(6, 37);
			this->visibleHistoryTree->Name = L"visibleHistoryTree";
			treeNode2->ForeColor = System::Drawing::Color::Blue;
			treeNode2->Name = L"Node0";
			treeNode2->Text = L"Node0";
			treeNode3->ForeColor = System::Drawing::SystemColors::WindowText;
			treeNode3->Name = L"Node1";
			treeNode3->Text = L"Node1";
			this->visibleHistoryTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(2) {treeNode2, treeNode3});
			this->visibleHistoryTree->Size = System::Drawing::Size(570, 1118);
			this->visibleHistoryTree->TabIndex = 1;
			// 
			// hmdContextMenu
			// 
			this->hmdContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->autoscaleHMD});
			this->hmdContextMenu->Name = L"contextMenuStrip1";
			this->hmdContextMenu->ShowCheckMargin = true;
			this->hmdContextMenu->ShowImageMargin = false;
			this->hmdContextMenu->Size = System::Drawing::Size(153, 48);
			// 
			// autoscaleHMD
			// 
			this->autoscaleHMD->CheckOnClick = true;
			this->autoscaleHMD->Name = L"autoscaleHMD";
			this->autoscaleHMD->Size = System::Drawing::Size(152, 22);
			this->autoscaleHMD->Text = L"Autoscale";
			// 
			// GraspMMIGraphsForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 18);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1933, 1188);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->taskRightTimeLimit);
			this->Controls->Add(this->taskLeftTimeLimit);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
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
			this->groupBox3->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->hmdContextMenu->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

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
	private: void FillHistoryTree( void );
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
			// If we are live, shift the limits of the plots to reflect the most recent data.
			// Otherwise, keep the window span where it is.
			if ( dataLiveCheckBox->Checked ) MoveToLatest();
			// Replot all of the strip charts.
			RefreshGraphics();
			// Update the history tree.
			FillHistoryTree();
			// Start the timer again to trigger the next cycle after a delay.
			dataLiveCheckBox->Checked = true;
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
			sprintf( label, "%02d:%02d:%02d GMT  ", hour, minute, second );
			return gcnew String( label );
		}

		System::Void GraspMMIGraphsForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			InitializeGraphics();
			ParseSubjectFile( historyTree, scriptDirectory + gcnew String( "Subjects.sbj" ) );
			visibleHistoryTree->Nodes->Clear();
			ReadTelemetryCache( packetCacheFileRoot );
			AdjustScrollSpan();
			dataLiveCheckBox->Checked = true;
			FillHistoryTree();
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
};
}

