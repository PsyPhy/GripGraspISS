#pragma once

///
/// Module:	GripMMI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GripMMI
///
/// Copyright (c) 2014, 2015 PsyPhy Consulting
///

// The main form for the GripMMI graphical interface.
// Refer to https://github.com/PsyPhy/GripMMI/wiki for an overview of the functionality
// of the GripMMI graphical user interface.

// Most of this file is generated automatically by the Visual C++ Forms graphical designer.
// This includes instance variables and event handlers as part of the GripMMIDesktop class,
// which is the class that generates the main window of the GripMMI GUI.
// Because the structure of these instance variables and methods will be obvious to someone
// familiar with the Windows Forms constucts, each invidual member is not documented with 
// hand generated comments in this file. The actions taken within customized event handlers
// are, however, commented to indicate how the GUI uses the rest of the GripMMI code.

#include "..\Useful\fOutputDebugString.h"
#include "..\VectorsMixin\VectorsMixin.h"
#include "..\PsyPhy2dGraphicsLib\Displays.h"
#include "..\PsyPhy2dGraphicsLib\Views.h"
#include "..\PsyPhy2dGraphicsLib\Layouts.h"
#include "..\Grip\DexAnalogMixin.h"
#include "..\Grip\GripPackets.h"

#include "GripMMIAbout.h"
#include "GripMMIStartup.h"
#include "GripMMIFullStep.h"
#include "..\GripMMIVersionControl\GripMMIVersionControl.h"


#include "GripMMIGlobals.h"

// Time in milliseconds between screen refreshes.
#define REFRESH_TIMEOUT	500

namespace GripMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// The single window that represents the GripMMI workspace.
	/// </summary>
	public ref class GripMMIDesktop : public System::Windows::Forms::Form
	{
	public:
		GripMMIDesktop( void )
		{

			// Now initialize the actual GripMMI Desktop.
			InitializeComponent();

			// Show the version number in the window title.
			this->Text = gcnew String( GripMMIVersion );

			// Create other dialog windows.
			fullStepForm = gcnew GripMMIFullStep();
			aboutForm = gcnew GripMMIAbout( GripMMIVersion, GripMMIBuildInfo );

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GripMMIDesktop()
		{
			if (components)
			{
				delete components;
			}
		}

	// Forms used to create and show popup windows.
	private: GripMMIFullStep^	fullStepForm;
	private: GripMMIAbout^		aboutForm;
	private: GripMMIStartup^	startupForm;

	// Automatically created components of the main Form.
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  groupBox5;

	private: System::Windows::Forms::PictureBox^  LogoPictureBox;
	private: System::Windows::Forms::PictureBox^  XYPlot;
	private: System::Windows::Forms::PictureBox^  CoPPlot;
	private: System::Windows::Forms::PictureBox^  ZYPlot;
	private: System::Windows::Forms::PictureBox^  StripCharts;
	private: System::Windows::Forms::CheckBox^  filterCheckbox;
	private: System::Windows::Forms::CheckBox^  dataLiveCheckbox;

	private: System::Windows::Forms::HScrollBar^  scrollBar;
	private: System::Windows::Forms::TrackBar^  spanSelector;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::TextBox^  dexText;
	private: System::Windows::Forms::PictureBox^  dexPicture;

	private: System::Windows::Forms::Button^  fakeIgnore;
	private: System::Windows::Forms::Button^  fakeOK;
	private: System::Windows::Forms::Button^  fakeInterrupt;
	private: System::Windows::Forms::Button^  fakeStatus;
	private: System::Windows::Forms::Button^  fakeCancel;
	private: System::Windows::Forms::Button^  fakeRetry;

	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::TextBox^  messageTypeBox;
	private: System::Windows::Forms::CheckBox^  scriptLiveCheckbox;
	private: System::Windows::Forms::RadioButton^  scriptErrorCheckbox;

	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::Button^  gotoButton;
	private: System::Windows::Forms::Button^  nextButton;
	private: System::Windows::Forms::TextBox^  stepIDBox;
	private: System::Windows::Forms::TextBox^  taskIDBox;
	private: System::Windows::Forms::TextBox^  protocolIDBox;
	private: System::Windows::Forms::TextBox^  subjectIDBox;

	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::ListBox^  subjectList;
	private: System::Windows::Forms::GroupBox^  groupBox10;
	private: System::Windows::Forms::ListBox^  protocolList;
	private: System::Windows::Forms::GroupBox^  groupBox11;
	private: System::Windows::Forms::ListBox^  taskList;
	private: System::Windows::Forms::GroupBox^  groupBox12;
	private: System::Windows::Forms::ListBox^  stepList;

	private: System::Windows::Forms::GroupBox^  groupBox13;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  markersTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox14;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  targetsTextBox;
	private: System::Windows::Forms::TextBox^  tonesTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox15;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::GroupBox^  groupBox16;
	private: System::Windows::Forms::TextBox^  cradlesTextBox;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::GroupBox^  groupBox17;
	private: System::Windows::Forms::TextBox^  acquisitionTextBox;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::CheckBox^  autoscaleCheckBox;
	private: System::Windows::Forms::ComboBox^  graphCollectionComboBox;
	private: System::Windows::Forms::Label^  Spans;
	private: System::Windows::Forms::TextBox^  earliestTextBox;
	private: System::Windows::Forms::TextBox^  latestTextBox;
	private: System::Windows::Forms::TextBox^  rightLimitTextBox;
private: System::Windows::Forms::TextBox^  filterConstantTextBox;
	private: System::Windows::Forms::TextBox^  leftLimitTextBox;

	private: 

	///
	/// Initialize the display when it first gets shown.
	///
		System::Void GripMMIDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {

			// Set up graphs.
			InitializeGraphics();
			AdjustScrollSpan();

			// Construct the path to the root script and intialize the crawler menus.
			char filename[MAX_PATHLENGTH];
			strcpy( filename, scriptDirectory );
			strcat( filename, "users.dex" );
			if ( ERROR_EXIT == ParseSubjectFile( filename ) ) {
			// If we are unable to parse the subject file, just shut down.
				Close();
			}
			else {
				// Subject file was parsed, so move ahead.
				// Create a timer to periodically check for data and refresh.
				CreateRefreshTimer( REFRESH_TIMEOUT );
				// Set the filter constant according to the initial state of the filter checkbox.
				if ( filterCheckbox->Checked ) dex.SetFilterConstant( System::Convert::ToDouble( filterConstantTextBox->Text ) );
				else dex.SetFilterConstant( 0.0 );
				// Select the summary graph collection by default.
				// This does a ForceUdate as a side effect, which requires that the refresh timer
				//  be created already. That is why CreateRefreshTimer() is performed first in this else clause.
				graphCollectionComboBox->SelectedIndex = 0;
				// The next time that the refresh timer goes off, plot the data regardless of 
				//  whether there is new data or not. This draws the data plots, even if empty.
				// ForceUpdate() is a side effect of changing the graph collection,
				//  so it is commented out here. But if the previous line is removed, 
				//  the ForceUpdate() call should be made here.
				// ForceUpdate();
			}	
		}

	private: 

		/// 
		/// GripMMI periodically checks for new data packets and refresh the display if required.
		/// The following routines implement that functionality.
		/// 

		// A timer to trigger new polling for packets after a delay.
		static Timer^ timer;
		void CreateRefreshTimer( int interval ) {
			timer = gcnew Timer;
			timer->Interval = interval;
			timer->Tick += gcnew EventHandler( this, &GripMMI::GripMMIDesktop::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			timer->Start();
		}
		void StopRefreshTimer( void ) {
			timer->Stop();
		}
		// The display will normally be updated only when there are new packets.
		// This flag allows one to force the update, for instance when the filtering is turned off or on.
		bool forceUpdate;

		// This is what we do when the timer goes off.
		void OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {
			int new_data;
			// Stop the timer so that it does not retrigger until we are done refreshing.
			StopRefreshTimer();
			fOutputDebugString( "\n" );
			fOutputDebugString( "Timer triggered.\n" );
			// Get the realtime science data packets. Return value says if new packets
			// have arrived since the previous call.
			new_data = GetGripRT();
			// Adjust the scrollbar limits according to the newly loaded data.
			AdjustScrollSpan();
			// If we are live, shift the limits of the plots to reflect the most recent data.
			// Otherwise, keep the window span where it is.
			if ( dataLiveCheckbox->Checked ) MoveToLatest();
			// If we have received new data, or if another function has requested a forced update,
			//  replot all of the strip charts and scatter plots.
			if ( new_data || forceUpdate ) RefreshGraphics();
			// Handle HK packets and the script crawler display.
			if ( scriptLiveCheckbox->Checked ) {
				fOutputDebugString( "UpdateStatus.\n" );
				UpdateStatus( forceUpdate );
			}
			// If we forced an update, reset it to false so that we do it only once.
			forceUpdate = false;
			// Start the timer again to trigger the next cycle after a delay.
			StartRefreshTimer();
		}
		// Some plot parameters can be changed by the GUI interface.
		// When that happens, ForceUpdate() is called so that the data will be replotted
		// on the next refresh cycle even if there is no new data.
		void ForceUpdate( void ) {
			StartRefreshTimer();
			forceUpdate = true;
		}
		// During some operations a new refresh should not be performed. This routine
		// will cancel a pending refresh timer. To restart, use ForceUpdate() or StartRefreshTimer().
		void ImpedeUpdate( void ) {
			StopRefreshTimer();
		}

	private: 

		// Objects used by the legacy code for data plots.
		// These items will be initialized by InitializeGraphics().
		::Display	xy_display;
		::Display	zy_display;
		::Display	cop_display;
		::Display	stripchart_display;
		::Layout	stripchart_layout;
		::View		visibility_view;
		::Layout	detailed_visibility_layout;
		::View		xy_view;
		::View		zy_view;
		::View		cop_view;

		// GripMMIGraphics.cpp

		void InitializeGraphics( void );
		void RefreshGraphics( void );
		void KillGraphics( void );
		void AdjustScrollSpan( void );
		void MoveToLatest( void );

		void GraphManipulandumPosition( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );
		void GraphManipulandumRotations( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );
		void PlotManipulandumPosition( double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );
		void GraphLoadForce( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip ) ;
		void GraphAcceleration( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip ) ;
		void GraphGripForce( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip ) ;
		void GraphVisibility( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip ) ;
		void GraphVisibilityDetails( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip ) ;
		void GraphCoP( ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );
		void PlotCoP( double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );

		void GraphManipulandumPositionComponent( int component, ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );
		void GraphAccelerationComponent( int component, ::View view, double start_instant, double stop_instant, int start_frame, int stop_frame, int skip );

		// GripMMIData.cpp

		void ResetBuffers( void );
		int  GetGripRT( void );
		void SimulateGripRT ( void ); // For testing only.
		int	 GetLatestGripHK( GripHealthAndStatusInfo *hk );
		void UpdateStatus( bool force );

		// GripMMIScripts.cpp

		void ParseTaskFile ( const char *task_file );
		void ParseProtocolFile ( const char *protocol_file );
		void ParseSessionFile ( const char *session_file );
		int  ParseSubjectFile ( const char *subject_file );

		void GoToSpecifiedSubject ( int subject );
		void GoToSpecifiedProtocol ( int protocol );
		void GoToSpecifiedTask ( int task );
		void GoToSpecifiedStep ( int step );
		void GoToSpecifiedIDs( int subject_id, int protocol_id, int task_id, int step_id );

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GripMMIDesktop::typeid));
			this->LogoPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->XYPlot = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->ZYPlot = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->CoPPlot = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->filterConstantTextBox = (gcnew System::Windows::Forms::TextBox());
			this->rightLimitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->leftLimitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->graphCollectionComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->autoscaleCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StripCharts = (gcnew System::Windows::Forms::PictureBox());
			this->filterCheckbox = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->latestTextBox = (gcnew System::Windows::Forms::TextBox());
			this->earliestTextBox = (gcnew System::Windows::Forms::TextBox());
			this->Spans = (gcnew System::Windows::Forms::Label());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->dataLiveCheckbox = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->fakeOK = (gcnew System::Windows::Forms::Button());
			this->fakeInterrupt = (gcnew System::Windows::Forms::Button());
			this->fakeStatus = (gcnew System::Windows::Forms::Button());
			this->fakeCancel = (gcnew System::Windows::Forms::Button());
			this->fakeRetry = (gcnew System::Windows::Forms::Button());
			this->fakeIgnore = (gcnew System::Windows::Forms::Button());
			this->dexPicture = (gcnew System::Windows::Forms::PictureBox());
			this->dexText = (gcnew System::Windows::Forms::TextBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->messageTypeBox = (gcnew System::Windows::Forms::TextBox());
			this->scriptLiveCheckbox = (gcnew System::Windows::Forms::CheckBox());
			this->scriptErrorCheckbox = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->gotoButton = (gcnew System::Windows::Forms::Button());
			this->nextButton = (gcnew System::Windows::Forms::Button());
			this->stepIDBox = (gcnew System::Windows::Forms::TextBox());
			this->taskIDBox = (gcnew System::Windows::Forms::TextBox());
			this->protocolIDBox = (gcnew System::Windows::Forms::TextBox());
			this->subjectIDBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->subjectList = (gcnew System::Windows::Forms::ListBox());
			this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
			this->protocolList = (gcnew System::Windows::Forms::ListBox());
			this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
			this->taskList = (gcnew System::Windows::Forms::ListBox());
			this->groupBox12 = (gcnew System::Windows::Forms::GroupBox());
			this->stepList = (gcnew System::Windows::Forms::ListBox());
			this->groupBox13 = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->markersTextBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox14 = (gcnew System::Windows::Forms::GroupBox());
			this->targetsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tonesTextBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox15 = (gcnew System::Windows::Forms::GroupBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox16 = (gcnew System::Windows::Forms::GroupBox());
			this->cradlesTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBox17 = (gcnew System::Windows::Forms::GroupBox());
			this->acquisitionTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LogoPictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XYPlot))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZYPlot))->BeginInit();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CoPPlot))->BeginInit();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->StripCharts))->BeginInit();
			this->groupBox5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->groupBox6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dexPicture))->BeginInit();
			this->groupBox7->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->groupBox10->SuspendLayout();
			this->groupBox11->SuspendLayout();
			this->groupBox12->SuspendLayout();
			this->groupBox13->SuspendLayout();
			this->groupBox14->SuspendLayout();
			this->groupBox15->SuspendLayout();
			this->groupBox16->SuspendLayout();
			this->groupBox17->SuspendLayout();
			this->SuspendLayout();
			// 
			// LogoPictureBox
			// 
			this->LogoPictureBox->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LogoPictureBox.Image")));
			this->LogoPictureBox->Location = System::Drawing::Point(20, 39);
			this->LogoPictureBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->LogoPictureBox->Name = L"LogoPictureBox";
			this->LogoPictureBox->Size = System::Drawing::Size(267, 246);
			this->LogoPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->LogoPictureBox->TabIndex = 0;
			this->LogoPictureBox->TabStop = false;
			// 
			// XYPlot
			// 
			this->XYPlot->BackColor = System::Drawing::Color::Maroon;
			this->XYPlot->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->XYPlot->Location = System::Drawing::Point(24, 31);
			this->XYPlot->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->XYPlot->Name = L"XYPlot";
			this->XYPlot->Size = System::Drawing::Size(327, 241);
			this->XYPlot->TabIndex = 2;
			this->XYPlot->TabStop = false;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->XYPlot);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(320, 11);
			this->groupBox1->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox1->Size = System::Drawing::Size(372, 279);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Frontal (XY)";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->ZYPlot);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(700, 11);
			this->groupBox2->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox2->Size = System::Drawing::Size(372, 279);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Sagittal (ZY)";
			// 
			// ZYPlot
			// 
			this->ZYPlot->BackColor = System::Drawing::Color::Maroon;
			this->ZYPlot->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ZYPlot->Location = System::Drawing::Point(23, 31);
			this->ZYPlot->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->ZYPlot->Name = L"ZYPlot";
			this->ZYPlot->Size = System::Drawing::Size(327, 241);
			this->ZYPlot->TabIndex = 2;
			this->ZYPlot->TabStop = false;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->CoPPlot);
			this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox3->Location = System::Drawing::Point(1080, 11);
			this->groupBox3->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox3->Size = System::Drawing::Size(372, 279);
			this->groupBox3->TabIndex = 6;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"CoP";
			// 
			// CoPPlot
			// 
			this->CoPPlot->BackColor = System::Drawing::Color::Maroon;
			this->CoPPlot->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->CoPPlot->Location = System::Drawing::Point(24, 31);
			this->CoPPlot->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->CoPPlot->Name = L"CoPPlot";
			this->CoPPlot->Size = System::Drawing::Size(327, 241);
			this->CoPPlot->TabIndex = 2;
			this->CoPPlot->TabStop = false;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->filterConstantTextBox);
			this->groupBox4->Controls->Add(this->rightLimitTextBox);
			this->groupBox4->Controls->Add(this->leftLimitTextBox);
			this->groupBox4->Controls->Add(this->graphCollectionComboBox);
			this->groupBox4->Controls->Add(this->autoscaleCheckBox);
			this->groupBox4->Controls->Add(this->StripCharts);
			this->groupBox4->Controls->Add(this->filterCheckbox);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(5, 396);
			this->groupBox4->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox4->Size = System::Drawing::Size(1449, 798);
			this->groupBox4->TabIndex = 7;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Time Series";
			// 
			// filterConstantTextBox
			// 
			this->filterConstantTextBox->BackColor = System::Drawing::SystemColors::Window;
			this->filterConstantTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->filterConstantTextBox->ForeColor = System::Drawing::SystemColors::WindowText;
			this->filterConstantTextBox->Location = System::Drawing::Point(1237, 0);
			this->filterConstantTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->filterConstantTextBox->Name = L"filterConstantTextBox";
			this->filterConstantTextBox->Size = System::Drawing::Size(61, 26);
			this->filterConstantTextBox->TabIndex = 26;
			this->filterConstantTextBox->Text = L"2.0";
			this->filterConstantTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->filterConstantTextBox->Enter += gcnew System::EventHandler(this, &GripMMIDesktop::filterConstantTextBox_Enter);
			this->filterConstantTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::filterConstantTextBox_KeyPress);
			this->filterConstantTextBox->Leave += gcnew System::EventHandler(this, &GripMMIDesktop::filterConstantTextBox_Leave);
			// 
			// rightLimitTextBox
			// 
			this->rightLimitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->rightLimitTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->rightLimitTextBox->Location = System::Drawing::Point(1359, 775);
			this->rightLimitTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->rightLimitTextBox->Name = L"rightLimitTextBox";
			this->rightLimitTextBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->rightLimitTextBox->Size = System::Drawing::Size(81, 19);
			this->rightLimitTextBox->TabIndex = 15;
			this->rightLimitTextBox->Text = L"00:00:00";
			this->rightLimitTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// leftLimitTextBox
			// 
			this->leftLimitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->leftLimitTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->leftLimitTextBox->Location = System::Drawing::Point(15, 775);
			this->leftLimitTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->leftLimitTextBox->Name = L"leftLimitTextBox";
			this->leftLimitTextBox->Size = System::Drawing::Size(116, 19);
			this->leftLimitTextBox->TabIndex = 15;
			this->leftLimitTextBox->Text = L"00:00:00";
			// 
			// graphCollectionComboBox
			// 
			this->graphCollectionComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->graphCollectionComboBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->graphCollectionComboBox->FormattingEnabled = true;
			this->graphCollectionComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Summary", L"Kinematics", L"Visibility"});
			this->graphCollectionComboBox->Location = System::Drawing::Point(939, 0);
			this->graphCollectionComboBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->graphCollectionComboBox->Name = L"graphCollectionComboBox";
			this->graphCollectionComboBox->Size = System::Drawing::Size(188, 26);
			this->graphCollectionComboBox->TabIndex = 25;
			this->graphCollectionComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GripMMIDesktop::graphCollectionComboBox_SelectedIndexChanged);
			this->graphCollectionComboBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::graphCollectionComboBox_KeyPress);
			this->graphCollectionComboBox->MouseCaptureChanged += gcnew System::EventHandler(this, &GripMMIDesktop::graphCollectionComboBox_MouseCaptureChanged);
			this->graphCollectionComboBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::graphCollectionComboBox_MouseDown);
			// 
			// autoscaleCheckBox
			// 
			this->autoscaleCheckBox->AutoSize = true;
			this->autoscaleCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->autoscaleCheckBox->Location = System::Drawing::Point(1319, 1);
			this->autoscaleCheckBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->autoscaleCheckBox->Name = L"autoscaleCheckBox";
			this->autoscaleCheckBox->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->autoscaleCheckBox->Size = System::Drawing::Size(105, 24);
			this->autoscaleCheckBox->TabIndex = 12;
			this->autoscaleCheckBox->Text = L"Autoscale";
			this->autoscaleCheckBox->UseVisualStyleBackColor = true;
			this->autoscaleCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GripMMIDesktop::autoscaleCheckBox_CheckedChanged);
			// 
			// StripCharts
			// 
			this->StripCharts->Location = System::Drawing::Point(8, 26);
			this->StripCharts->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->StripCharts->Name = L"StripCharts";
			this->StripCharts->Size = System::Drawing::Size(1433, 750);
			this->StripCharts->TabIndex = 0;
			this->StripCharts->TabStop = false;
			// 
			// filterCheckbox
			// 
			this->filterCheckbox->AutoSize = true;
			this->filterCheckbox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->filterCheckbox->Location = System::Drawing::Point(1145, 1);
			this->filterCheckbox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->filterCheckbox->Name = L"filterCheckbox";
			this->filterCheckbox->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->filterCheckbox->Size = System::Drawing::Size(69, 24);
			this->filterCheckbox->TabIndex = 9;
			this->filterCheckbox->Text = L"Filter";
			this->filterCheckbox->UseVisualStyleBackColor = true;
			this->filterCheckbox->CheckedChanged += gcnew System::EventHandler(this, &GripMMIDesktop::filterCheckbox_CheckedChanged);
			// 
			// groupBox5
			// 
			this->groupBox5->BackColor = System::Drawing::Color::Transparent;
			this->groupBox5->Controls->Add(this->latestTextBox);
			this->groupBox5->Controls->Add(this->earliestTextBox);
			this->groupBox5->Controls->Add(this->Spans);
			this->groupBox5->Controls->Add(this->scrollBar);
			this->groupBox5->Controls->Add(this->spanSelector);
			this->groupBox5->Controls->Add(this->dataLiveCheckbox);
			this->groupBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox5->Location = System::Drawing::Point(5, 298);
			this->groupBox5->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox5->Size = System::Drawing::Size(1449, 91);
			this->groupBox5->TabIndex = 8;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Data Display";
			// 
			// latestTextBox
			// 
			this->latestTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->latestTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->latestTextBox->Location = System::Drawing::Point(1264, 64);
			this->latestTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->latestTextBox->Name = L"latestTextBox";
			this->latestTextBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->latestTextBox->Size = System::Drawing::Size(81, 19);
			this->latestTextBox->TabIndex = 14;
			this->latestTextBox->Text = L"00:00:00";
			this->latestTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// earliestTextBox
			// 
			this->earliestTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->earliestTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->earliestTextBox->Location = System::Drawing::Point(323, 64);
			this->earliestTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->earliestTextBox->Name = L"earliestTextBox";
			this->earliestTextBox->Size = System::Drawing::Size(136, 19);
			this->earliestTextBox->TabIndex = 13;
			this->earliestTextBox->Text = L"00:00:00";
			// 
			// Spans
			// 
			this->Spans->AutoSize = true;
			this->Spans->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Spans->Location = System::Drawing::Point(16, 63);
			this->Spans->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->Spans->Name = L"Spans";
			this->Spans->Size = System::Drawing::Size(293, 20);
			this->Spans->TabIndex = 12;
			this->Spans->Text = L"12h  4h   1h    30m 10m  5m  60s  30s";
			// 
			// scrollBar
			// 
			this->scrollBar->LargeChange = 100000;
			this->scrollBar->Location = System::Drawing::Point(323, 27);
			this->scrollBar->Maximum = 864000;
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(1023, 25);
			this->scrollBar->SmallChange = 10000;
			this->scrollBar->TabIndex = 10;
			this->scrollBar->Value = 1000;
			this->scrollBar->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &GripMMIDesktop::scrollBar_Scroll);
			// 
			// spanSelector
			// 
			this->spanSelector->AutoSize = false;
			this->spanSelector->BackColor = System::Drawing::Color::White;
			this->spanSelector->LargeChange = 1;
			this->spanSelector->Location = System::Drawing::Point(17, 22);
			this->spanSelector->Margin = System::Windows::Forms::Padding(1);
			this->spanSelector->Maximum = 7;
			this->spanSelector->Name = L"spanSelector";
			this->spanSelector->Size = System::Drawing::Size(292, 39);
			this->spanSelector->TabIndex = 11;
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &GripMMIDesktop::spanSelector_ValueChanged);
			// 
			// dataLiveCheckbox
			// 
			this->dataLiveCheckbox->AutoSize = true;
			this->dataLiveCheckbox->Checked = true;
			this->dataLiveCheckbox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dataLiveCheckbox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->dataLiveCheckbox->Location = System::Drawing::Point(1365, 38);
			this->dataLiveCheckbox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->dataLiveCheckbox->Name = L"dataLiveCheckbox";
			this->dataLiveCheckbox->Size = System::Drawing::Size(70, 29);
			this->dataLiveCheckbox->TabIndex = 0;
			this->dataLiveCheckbox->Text = L"Live";
			this->dataLiveCheckbox->UseVisualStyleBackColor = true;
			this->dataLiveCheckbox->CheckedChanged += gcnew System::EventHandler(this, &GripMMIDesktop::dataLiveCheckbox_CheckedChanged);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->fakeOK);
			this->groupBox6->Controls->Add(this->fakeInterrupt);
			this->groupBox6->Controls->Add(this->fakeStatus);
			this->groupBox6->Controls->Add(this->fakeCancel);
			this->groupBox6->Controls->Add(this->fakeRetry);
			this->groupBox6->Controls->Add(this->fakeIgnore);
			this->groupBox6->Controls->Add(this->dexPicture);
			this->groupBox6->Controls->Add(this->dexText);
			this->groupBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox6->Location = System::Drawing::Point(1464, 11);
			this->groupBox6->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox6->Size = System::Drawing::Size(564, 562);
			this->groupBox6->TabIndex = 9;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Workspace Tablet Display";
			// 
			// fakeOK
			// 
			this->fakeOK->Location = System::Drawing::Point(152, 516);
			this->fakeOK->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeOK->Name = L"fakeOK";
			this->fakeOK->Size = System::Drawing::Size(139, 39);
			this->fakeOK->TabIndex = 15;
			this->fakeOK->Text = L"OK";
			this->fakeOK->UseVisualStyleBackColor = true;
			// 
			// fakeInterrupt
			// 
			this->fakeInterrupt->Location = System::Drawing::Point(13, 516);
			this->fakeInterrupt->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeInterrupt->Name = L"fakeInterrupt";
			this->fakeInterrupt->Size = System::Drawing::Size(139, 39);
			this->fakeInterrupt->TabIndex = 14;
			this->fakeInterrupt->Text = L"Interrupt";
			this->fakeInterrupt->UseVisualStyleBackColor = true;
			// 
			// fakeStatus
			// 
			this->fakeStatus->Location = System::Drawing::Point(429, 516);
			this->fakeStatus->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeStatus->Name = L"fakeStatus";
			this->fakeStatus->Size = System::Drawing::Size(128, 39);
			this->fakeStatus->TabIndex = 13;
			this->fakeStatus->Text = L"Status";
			this->fakeStatus->UseVisualStyleBackColor = true;
			// 
			// fakeCancel
			// 
			this->fakeCancel->Location = System::Drawing::Point(291, 516);
			this->fakeCancel->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeCancel->Name = L"fakeCancel";
			this->fakeCancel->Size = System::Drawing::Size(139, 39);
			this->fakeCancel->TabIndex = 12;
			this->fakeCancel->Text = L"Cancel";
			this->fakeCancel->UseVisualStyleBackColor = true;
			// 
			// fakeRetry
			// 
			this->fakeRetry->Location = System::Drawing::Point(152, 517);
			this->fakeRetry->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeRetry->Name = L"fakeRetry";
			this->fakeRetry->Size = System::Drawing::Size(139, 39);
			this->fakeRetry->TabIndex = 11;
			this->fakeRetry->Text = L"Retry";
			this->fakeRetry->UseVisualStyleBackColor = true;
			// 
			// fakeIgnore
			// 
			this->fakeIgnore->Location = System::Drawing::Point(13, 517);
			this->fakeIgnore->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->fakeIgnore->Name = L"fakeIgnore";
			this->fakeIgnore->Size = System::Drawing::Size(139, 39);
			this->fakeIgnore->TabIndex = 10;
			this->fakeIgnore->Text = L"Ignore";
			this->fakeIgnore->UseVisualStyleBackColor = true;
			// 
			// dexPicture
			// 
			this->dexPicture->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->dexPicture->Location = System::Drawing::Point(15, 137);
			this->dexPicture->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->dexPicture->Name = L"dexPicture";
			this->dexPicture->Size = System::Drawing::Size(535, 371);
			this->dexPicture->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->dexPicture->TabIndex = 1;
			this->dexPicture->TabStop = false;
			// 
			// dexText
			// 
			this->dexText->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->dexText->Location = System::Drawing::Point(13, 31);
			this->dexText->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->dexText->Multiline = true;
			this->dexText->Name = L"dexText";
			this->dexText->Size = System::Drawing::Size(537, 98);
			this->dexText->TabIndex = 0;
			this->dexText->Text = L"ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABC" 
				L"DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->messageTypeBox);
			this->groupBox7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox7->Location = System::Drawing::Point(1467, 576);
			this->groupBox7->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox7->Size = System::Drawing::Size(456, 62);
			this->groupBox7->TabIndex = 10;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Message Type";
			// 
			// messageTypeBox
			// 
			this->messageTypeBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->messageTypeBox->Location = System::Drawing::Point(12, 27);
			this->messageTypeBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->messageTypeBox->Multiline = true;
			this->messageTypeBox->Name = L"messageTypeBox";
			this->messageTypeBox->Size = System::Drawing::Size(431, 26);
			this->messageTypeBox->TabIndex = 0;
			// 
			// scriptLiveCheckbox
			// 
			this->scriptLiveCheckbox->AutoSize = true;
			this->scriptLiveCheckbox->Checked = true;
			this->scriptLiveCheckbox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->scriptLiveCheckbox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->scriptLiveCheckbox->Location = System::Drawing::Point(1933, 583);
			this->scriptLiveCheckbox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->scriptLiveCheckbox->Name = L"scriptLiveCheckbox";
			this->scriptLiveCheckbox->Size = System::Drawing::Size(70, 29);
			this->scriptLiveCheckbox->TabIndex = 11;
			this->scriptLiveCheckbox->Text = L"Live";
			this->scriptLiveCheckbox->UseVisualStyleBackColor = true;
			this->scriptLiveCheckbox->CheckedChanged += gcnew System::EventHandler(this, &GripMMIDesktop::scriptLiveCheckbox_CheckedChanged);
			// 
			// scriptErrorCheckbox
			// 
			this->scriptErrorCheckbox->AutoSize = true;
			this->scriptErrorCheckbox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->scriptErrorCheckbox->Location = System::Drawing::Point(1933, 613);
			this->scriptErrorCheckbox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->scriptErrorCheckbox->Name = L"scriptErrorCheckbox";
			this->scriptErrorCheckbox->Size = System::Drawing::Size(75, 29);
			this->scriptErrorCheckbox->TabIndex = 12;
			this->scriptErrorCheckbox->TabStop = true;
			this->scriptErrorCheckbox->Text = L"Error";
			this->scriptErrorCheckbox->UseVisualStyleBackColor = true;
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->gotoButton);
			this->groupBox8->Controls->Add(this->nextButton);
			this->groupBox8->Controls->Add(this->stepIDBox);
			this->groupBox8->Controls->Add(this->taskIDBox);
			this->groupBox8->Controls->Add(this->protocolIDBox);
			this->groupBox8->Controls->Add(this->subjectIDBox);
			this->groupBox8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox8->Location = System::Drawing::Point(1467, 641);
			this->groupBox8->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox8->Size = System::Drawing::Size(561, 62);
			this->groupBox8->TabIndex = 11;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"Script Navigator";
			// 
			// gotoButton
			// 
			this->gotoButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->gotoButton->Location = System::Drawing::Point(452, 26);
			this->gotoButton->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->gotoButton->Name = L"gotoButton";
			this->gotoButton->Size = System::Drawing::Size(96, 32);
			this->gotoButton->TabIndex = 5;
			this->gotoButton->Text = L"GoTo";
			this->gotoButton->UseVisualStyleBackColor = true;
			this->gotoButton->Click += gcnew System::EventHandler(this, &GripMMIDesktop::gotoButton_Click);
			// 
			// nextButton
			// 
			this->nextButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->nextButton->Location = System::Drawing::Point(12, 26);
			this->nextButton->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->nextButton->Name = L"nextButton";
			this->nextButton->Size = System::Drawing::Size(96, 32);
			this->nextButton->TabIndex = 4;
			this->nextButton->Text = L"Next";
			this->nextButton->UseVisualStyleBackColor = true;
			this->nextButton->Click += gcnew System::EventHandler(this, &GripMMIDesktop::nextButton_Click);
			// 
			// stepIDBox
			// 
			this->stepIDBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepIDBox->Location = System::Drawing::Point(371, 26);
			this->stepIDBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->stepIDBox->Name = L"stepIDBox";
			this->stepIDBox->Size = System::Drawing::Size(61, 28);
			this->stepIDBox->TabIndex = 3;
			this->stepIDBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->stepIDBox->Enter += gcnew System::EventHandler(this, &GripMMIDesktop::stepIDBox_Enter);
			this->stepIDBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::stepIDBox_KeyPress);
			this->stepIDBox->Leave += gcnew System::EventHandler(this, &GripMMIDesktop::stepIDBox_Leave);
			// 
			// taskIDBox
			// 
			this->taskIDBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskIDBox->Location = System::Drawing::Point(289, 26);
			this->taskIDBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->taskIDBox->Name = L"taskIDBox";
			this->taskIDBox->Size = System::Drawing::Size(61, 28);
			this->taskIDBox->TabIndex = 2;
			this->taskIDBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->taskIDBox->Enter += gcnew System::EventHandler(this, &GripMMIDesktop::taskIDBox_Enter);
			this->taskIDBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::taskIDBox_KeyPress);
			this->taskIDBox->Leave += gcnew System::EventHandler(this, &GripMMIDesktop::taskIDBox_Leave);
			// 
			// protocolIDBox
			// 
			this->protocolIDBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolIDBox->Location = System::Drawing::Point(208, 26);
			this->protocolIDBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->protocolIDBox->Name = L"protocolIDBox";
			this->protocolIDBox->Size = System::Drawing::Size(61, 28);
			this->protocolIDBox->TabIndex = 1;
			this->protocolIDBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->protocolIDBox->Enter += gcnew System::EventHandler(this, &GripMMIDesktop::protocolIDBox_Enter);
			this->protocolIDBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::protocolIDBox_KeyPress);
			this->protocolIDBox->Leave += gcnew System::EventHandler(this, &GripMMIDesktop::protocolIDBox_Leave);
			// 
			// subjectIDBox
			// 
			this->subjectIDBox->BackColor = System::Drawing::SystemColors::Window;
			this->subjectIDBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectIDBox->ForeColor = System::Drawing::SystemColors::WindowText;
			this->subjectIDBox->Location = System::Drawing::Point(127, 26);
			this->subjectIDBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->subjectIDBox->Name = L"subjectIDBox";
			this->subjectIDBox->Size = System::Drawing::Size(61, 28);
			this->subjectIDBox->TabIndex = 0;
			this->subjectIDBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->subjectIDBox->Enter += gcnew System::EventHandler(this, &GripMMIDesktop::subjectIDBox_Enter);
			this->subjectIDBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GripMMIDesktop::subjectIDBox_KeyPress);
			this->subjectIDBox->Leave += gcnew System::EventHandler(this, &GripMMIDesktop::subjectIDBox_Leave);
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->subjectList);
			this->groupBox9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox9->Location = System::Drawing::Point(1468, 706);
			this->groupBox9->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox9->Size = System::Drawing::Size(181, 226);
			this->groupBox9->TabIndex = 13;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"Subject";
			// 
			// subjectList
			// 
			this->subjectList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectList->FormattingEnabled = true;
			this->subjectList->ItemHeight = 18;
			this->subjectList->Location = System::Drawing::Point(7, 31);
			this->subjectList->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->subjectList->Name = L"subjectList";
			this->subjectList->Size = System::Drawing::Size(165, 184);
			this->subjectList->TabIndex = 0;
			this->subjectList->SelectedIndexChanged += gcnew System::EventHandler(this, &GripMMIDesktop::subjectList_SelectedIndexChanged);
			this->subjectList->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::subjectList_MouseDown);
			// 
			// groupBox10
			// 
			this->groupBox10->Controls->Add(this->protocolList);
			this->groupBox10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox10->Location = System::Drawing::Point(1468, 944);
			this->groupBox10->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox10->Name = L"groupBox10";
			this->groupBox10->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox10->Size = System::Drawing::Size(181, 300);
			this->groupBox10->TabIndex = 14;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = L"Protocol";
			// 
			// protocolList
			// 
			this->protocolList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolList->FormattingEnabled = true;
			this->protocolList->ItemHeight = 18;
			this->protocolList->Location = System::Drawing::Point(7, 31);
			this->protocolList->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->protocolList->Name = L"protocolList";
			this->protocolList->Size = System::Drawing::Size(165, 256);
			this->protocolList->TabIndex = 1;
			this->protocolList->SelectedIndexChanged += gcnew System::EventHandler(this, &GripMMIDesktop::protocolList_SelectedIndexChanged);
			this->protocolList->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::protocolList_MouseDown);
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->taskList);
			this->groupBox11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox11->Location = System::Drawing::Point(1657, 705);
			this->groupBox11->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox11->Name = L"groupBox11";
			this->groupBox11->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox11->Size = System::Drawing::Size(181, 539);
			this->groupBox11->TabIndex = 15;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = L"Task";
			// 
			// taskList
			// 
			this->taskList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskList->FormattingEnabled = true;
			this->taskList->ItemHeight = 18;
			this->taskList->Location = System::Drawing::Point(5, 31);
			this->taskList->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->taskList->Name = L"taskList";
			this->taskList->Size = System::Drawing::Size(169, 490);
			this->taskList->TabIndex = 2;
			this->taskList->SelectedIndexChanged += gcnew System::EventHandler(this, &GripMMIDesktop::taskList_SelectedIndexChanged);
			this->taskList->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::taskList_MouseDown);
			// 
			// groupBox12
			// 
			this->groupBox12->Controls->Add(this->stepList);
			this->groupBox12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox12->Location = System::Drawing::Point(1847, 705);
			this->groupBox12->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox12->Name = L"groupBox12";
			this->groupBox12->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox12->Size = System::Drawing::Size(181, 539);
			this->groupBox12->TabIndex = 16;
			this->groupBox12->TabStop = false;
			this->groupBox12->Text = L"Step";
			// 
			// stepList
			// 
			this->stepList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepList->FormattingEnabled = true;
			this->stepList->ItemHeight = 18;
			this->stepList->Location = System::Drawing::Point(5, 31);
			this->stepList->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->stepList->Name = L"stepList";
			this->stepList->Size = System::Drawing::Size(169, 490);
			this->stepList->TabIndex = 3;
			this->stepList->SelectedIndexChanged += gcnew System::EventHandler(this, &GripMMIDesktop::stepList_SelectedIndexChanged);
			this->stepList->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::stepList_MouseDown);
			// 
			// groupBox13
			// 
			this->groupBox13->Controls->Add(this->label1);
			this->groupBox13->Controls->Add(this->markersTextBox);
			this->groupBox13->Location = System::Drawing::Point(7, 1195);
			this->groupBox13->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox13->Name = L"groupBox13";
			this->groupBox13->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox13->Size = System::Drawing::Size(403, 49);
			this->groupBox13->TabIndex = 18;
			this->groupBox13->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->label1->Location = System::Drawing::Point(11, 16);
			this->label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(83, 25);
			this->label1->TabIndex = 20;
			this->label1->Text = L"Markers";
			// 
			// markersTextBox
			// 
			this->markersTextBox->Font = (gcnew System::Drawing::Font(L"Wingdings", 6, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(2)));
			this->markersTextBox->Location = System::Drawing::Point(107, 14);
			this->markersTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->markersTextBox->Multiline = true;
			this->markersTextBox->Name = L"markersTextBox";
			this->markersTextBox->Size = System::Drawing::Size(281, 29);
			this->markersTextBox->TabIndex = 19;
			this->markersTextBox->Text = L" mmmmmmmm  mmmm  mmmmmmmu\r\n uuuuuuuu  uuuu  uuuuuuuu";
			this->markersTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->markersTextBox->WordWrap = false;
			// 
			// groupBox14
			// 
			this->groupBox14->Controls->Add(this->targetsTextBox);
			this->groupBox14->Controls->Add(this->label2);
			this->groupBox14->Location = System::Drawing::Point(435, 1195);
			this->groupBox14->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox14->Name = L"groupBox14";
			this->groupBox14->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox14->Size = System::Drawing::Size(285, 49);
			this->groupBox14->TabIndex = 21;
			this->groupBox14->TabStop = false;
			// 
			// targetsTextBox
			// 
			this->targetsTextBox->Font = (gcnew System::Drawing::Font(L"Wingdings", 6, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(2)));
			this->targetsTextBox->Location = System::Drawing::Point(103, 14);
			this->targetsTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->targetsTextBox->Multiline = true;
			this->targetsTextBox->Name = L"targetsTextBox";
			this->targetsTextBox->Size = System::Drawing::Size(165, 29);
			this->targetsTextBox->TabIndex = 21;
			this->targetsTextBox->Text = L" mmmmummmmm\r\n mmmmmmmmmummm";
			this->targetsTextBox->WordWrap = false;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->label2->Location = System::Drawing::Point(11, 16);
			this->label2->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(79, 25);
			this->label2->TabIndex = 20;
			this->label2->Text = L"Targets";
			// 
			// tonesTextBox
			// 
			this->tonesTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tonesTextBox->Location = System::Drawing::Point(101, 12);
			this->tonesTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->tonesTextBox->Name = L"tonesTextBox";
			this->tonesTextBox->Size = System::Drawing::Size(135, 30);
			this->tonesTextBox->TabIndex = 21;
			this->tonesTextBox->Text = L"...|...";
			this->tonesTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tonesTextBox->WordWrap = false;
			// 
			// groupBox15
			// 
			this->groupBox15->Controls->Add(this->tonesTextBox);
			this->groupBox15->Controls->Add(this->label3);
			this->groupBox15->Location = System::Drawing::Point(744, 1195);
			this->groupBox15->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox15->Name = L"groupBox15";
			this->groupBox15->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox15->Size = System::Drawing::Size(249, 49);
			this->groupBox15->TabIndex = 22;
			this->groupBox15->TabStop = false;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->label3->Location = System::Drawing::Point(11, 16);
			this->label3->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(70, 25);
			this->label3->TabIndex = 20;
			this->label3->Text = L"Sound";
			// 
			// groupBox16
			// 
			this->groupBox16->Controls->Add(this->cradlesTextBox);
			this->groupBox16->Controls->Add(this->label4);
			this->groupBox16->Location = System::Drawing::Point(1016, 1195);
			this->groupBox16->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox16->Name = L"groupBox16";
			this->groupBox16->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox16->Size = System::Drawing::Size(208, 49);
			this->groupBox16->TabIndex = 23;
			this->groupBox16->TabStop = false;
			// 
			// cradlesTextBox
			// 
			this->cradlesTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cradlesTextBox->Location = System::Drawing::Point(101, 12);
			this->cradlesTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->cradlesTextBox->Name = L"cradlesTextBox";
			this->cradlesTextBox->Size = System::Drawing::Size(92, 30);
			this->cradlesTextBox->TabIndex = 21;
			this->cradlesTextBox->Text = L"S M L";
			this->cradlesTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->cradlesTextBox->WordWrap = false;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->label4->Location = System::Drawing::Point(11, 16);
			this->label4->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(80, 25);
			this->label4->TabIndex = 20;
			this->label4->Text = L"Cradles";
			// 
			// groupBox17
			// 
			this->groupBox17->Controls->Add(this->acquisitionTextBox);
			this->groupBox17->Controls->Add(this->label5);
			this->groupBox17->Location = System::Drawing::Point(1245, 1195);
			this->groupBox17->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox17->Name = L"groupBox17";
			this->groupBox17->Padding = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->groupBox17->Size = System::Drawing::Size(208, 49);
			this->groupBox17->TabIndex = 24;
			this->groupBox17->TabStop = false;
			// 
			// acquisitionTextBox
			// 
			this->acquisitionTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->acquisitionTextBox->Location = System::Drawing::Point(101, 12);
			this->acquisitionTextBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->acquisitionTextBox->Name = L"acquisitionTextBox";
			this->acquisitionTextBox->Size = System::Drawing::Size(92, 30);
			this->acquisitionTextBox->TabIndex = 21;
			this->acquisitionTextBox->Text = L"A M";
			this->acquisitionTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->acquisitionTextBox->WordWrap = false;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->label5->Location = System::Drawing::Point(11, 16);
			this->label5->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(79, 25);
			this->label5->TabIndex = 20;
			this->label5->Text = L"Acquire";
			// 
			// GripMMIDesktop
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->ClientSize = System::Drawing::Size(1924, 1060);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox12);
			this->Controls->Add(this->groupBox11);
			this->Controls->Add(this->groupBox10);
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->groupBox8);
			this->Controls->Add(this->scriptErrorCheckbox);
			this->Controls->Add(this->groupBox7);
			this->Controls->Add(this->scriptLiveCheckbox);
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->LogoPictureBox);
			this->Controls->Add(this->groupBox13);
			this->Controls->Add(this->groupBox14);
			this->Controls->Add(this->groupBox15);
			this->Controls->Add(this->groupBox16);
			this->Controls->Add(this->groupBox17);
			this->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->MaximumSize = System::Drawing::Size(2131, 1470);
			this->Name = L"GripMMIDesktop";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"GripMMI";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GripMMIDesktop::GripMMIDesktop_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GripMMIDesktop::GripMMIDesktop_Shown);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GripMMIDesktop::GripMMIDesktop_Paint);
			this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &GripMMIDesktop::GripMMIDesktop_MouseClick);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LogoPictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XYPlot))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZYPlot))->EndInit();
			this->groupBox3->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CoPPlot))->EndInit();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->StripCharts))->EndInit();
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->EndInit();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dexPicture))->EndInit();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->groupBox9->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->groupBox12->ResumeLayout(false);
			this->groupBox13->ResumeLayout(false);
			this->groupBox13->PerformLayout();
			this->groupBox14->ResumeLayout(false);
			this->groupBox14->PerformLayout();
			this->groupBox15->ResumeLayout(false);
			this->groupBox15->PerformLayout();
			this->groupBox16->ResumeLayout(false);
			this->groupBox16->PerformLayout();
			this->groupBox17->ResumeLayout(false);
			this->groupBox17->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void GripMMIDesktop_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 // The graphs may need to be refreshed.
				 // RefreshGraphics();
			 }
	private: System::Void GripMMIDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 // Free resources allocated by the PsyPhy graphics routines.
				 KillGraphics();
			 }
	private: System::Void spanSelector_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 // When the user selects a different span with the slider, update the parameters
				 // of the scroll bar and refresh the data display accordingly.
				 AdjustScrollSpan();
				 RefreshGraphics();
			 }
	private: System::Void scrollBar_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) {
				 // When the user moves the scroll bar, we are implicitly no longer 'live'.
				 dataLiveCheckbox->Checked = false;
				 // Redraw the graphs based on the new position of the scroll bar.
				 RefreshGraphics();
			 }
	private: System::Void subjectList_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 // When the user manually selects a subject, the script crawler is no longer live.
				 scriptLiveCheckbox->Checked = false;
			 }
	private: System::Void subjectList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Update whenever a different subject is selected.
				 // This can happen by manually selecting a subject in the list box, or if 
				 // we are live and the HK packet specifies a new subject.
				 int subject = subjectList->SelectedIndex;
				 GoToSpecifiedSubject( subject );
			 }
	private: System::Void protocolList_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 // When the user manually selects a protocol, the script crawler is no longer live.
				 scriptLiveCheckbox->Checked = false;
			 }
	private: System::Void protocolList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Update when a different protocol is selected.
				 // This can happen by manually selecting a protocol in the list box, or if 
				 // we are live and the HK packet specifies a new subject.
				 int protocol = protocolList->SelectedIndex;
				 GoToSpecifiedProtocol( protocol );
			 }
	private: System::Void taskList_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 // When the user manually selects a task, the script crawler is no longer live.
				 scriptLiveCheckbox->Checked = false;
			 }
	private: System::Void taskList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Update when a different task is selected.
				 // This can happen by manually selecting a task in the list box, or if 
				 // we are live and the HK packet specifies a new subject.
				 int task = taskList->SelectedIndex;
				 // Keep the selected task near the top of the box
				 //  so that we can see what is coming next.
				 int top_index = task - 10;
				 if ( top_index < 0 ) top_index = 0;
				 taskList->TopIndex = top_index;
				 // Now update to show the selected task.
				 GoToSpecifiedTask( task );
			 }
	private: System::Void stepList_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 if ( e->Button == System::Windows::Forms::MouseButtons::Right ) {	
					 // A right button click shows the full text of the task step
					 // in a popup window.
					 fullStepForm->Show();
				 }
				 else {
				 // When the user manually selects a step, the script crawler is no longer live.
					 scriptLiveCheckbox->Checked = false;
				 }
			 }
	private: System::Void stepList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 // A new step has been selected.
				 // Keep the selected step near the top of the box
				 //  so that we can see what is coming next.
				 int step = stepList->SelectedIndex;
				 int top_index = step - 10;
				 if ( top_index < 0 ) top_index = 0;
				 stepList->TopIndex = top_index;
				 // Now update the picture and text.
				 GoToSpecifiedStep( step );
			 }
	private: System::Void nextButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 // Script crawler is no longer live.
				 scriptLiveCheckbox->Checked = false;
				 // Find the next non-comment step.
				 // int selected_line = SendDlgItemMessage( IDC_STEPS, LB_GETCURSEL, 0, 0 );
				 int selected_line = stepList->SelectedIndex;
				 selected_line++;
				 while ( comment[selected_line] ) selected_line++;
				 stepList->SelectedIndex = selected_line;
				 GoToSpecifiedStep( selected_line );
			 }
	private: System::Void gotoButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 // The user has entered subject, protocol, task and step IDs in the 
				 // text boxes and pressed 'GoTo'. We try to go to the specified step, if it exists.
				 // Script crawler is no longer live.
				 scriptLiveCheckbox->Checked = false;

				 // If the button has been highlighted, turn it back to normal.
				 gotoButton->BackColor = SystemColors::Window;

				 int subject_id;
				 int protocol_id;
				 int task_id;
				 int step_id;
				 // Read the ID numbers from the text boxes.
				 // Handle the cases where the content of the text box is not a number.
				 try
				 {
					 subject_id = Convert::ToInt32( subjectIDBox->Text );
				 }
				 catch (System::FormatException^ e)
				 {
					 subject_id = 0;
					 e;
				 }
				 try
				 {
					 protocol_id = Convert::ToInt32( protocolIDBox->Text );
				 }
				 catch (System::FormatException^ e)
				 {
					 protocol_id = 0;
					 e;
				 }
				 try
				 {
					 task_id = Convert::ToInt32( taskIDBox->Text );
				 }
				 catch (System::FormatException^ e)
				 {
					 task_id = 0;
					 e;
				 }
				 try
				 {
					 step_id = Convert::ToInt32( stepIDBox->Text );
				 }
				 catch (System::FormatException^ e)
				 {
					 step_id = 0;
					 e;
				 }

				 GoToSpecifiedIDs( subject_id, protocol_id, task_id, step_id );
				 ForceUpdate();

			 }

	// The user can manually enter subject, protocol, task and step IDs in the text boxes
	// and then press GoTo to position the list boxes accordingly. This creates some issues
	// of when to update the display while the user is typing into the text boxes.
	// The following routines are intended to deal with this situation.
	private: System::Void subjectIDBox_Enter(System::Object^  sender, System::EventArgs^  e) {
				 // Inhibit script crawler update if we start to manually enter an ID.
				 scriptLiveCheckbox->Checked = false;
				 ImpedeUpdate();
				 // Highlight the box that is awaiting input, in case we clicked by mistake.
				 subjectIDBox->BackColor = Color::Lime;
				 // Highlight the GoTo box to hint at what to do next as well.
				 gotoButton->BackColor = Color::Lime;
			 }
	private: System::Void subjectIDBox_Leave(System::Object^  sender, System::EventArgs^  e) {
				 // Remove the highlighting.
				 subjectIDBox->BackColor = SystemColors::Window;
				 // Restart updating after manually specifying a value.
				 ForceUpdate();
			 }
	private: System::Void subjectIDBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 // If user presses <enter>, treat it like pressing GoTo.
				 if ( e->KeyChar == 0x0d ){
					 gotoButton->Focus();
					 InvokeOnClick(gotoButton, System::EventArgs::Empty);	
				 }
			 }
	private: System::Void protocolIDBox_Enter(System::Object^  sender, System::EventArgs^  e) {
				// Inhibit script crawler update if we start to manually enter an ID.
				scriptLiveCheckbox->Checked = false;
				ImpedeUpdate();
				// Highlight the box that is awaiting input, in case we clicked by mistake.
				protocolIDBox->BackColor = Color::Lime;
				 // Highlight the GoTo box to hint at what to do next as well.
				 gotoButton->BackColor = Color::Lime;
			}
	private: System::Void protocolIDBox_Leave(System::Object^  sender, System::EventArgs^  e) {
				 // Remove the highlighting.
				 protocolIDBox->BackColor = SystemColors::Window;
				 // Restart updating after manually specifying a value.
				 ForceUpdate();
			 }
	private: System::Void protocolIDBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 // If user presses <enter>, treat it like pressing GoTo.
				 if ( e->KeyChar == 0x0d ){
					 gotoButton->Focus();
					 InvokeOnClick(gotoButton, System::EventArgs::Empty);
				 }
			 }
	private: System::Void taskIDBox_Enter(System::Object^  sender, System::EventArgs^  e) {
				// Inhibit script crawler update if we start to manually enter an ID.
				scriptLiveCheckbox->Checked = false;
				ImpedeUpdate();
				// Highlight the box that is awaiting input, in case we clicked by mistake.
				taskIDBox->BackColor = Color::Lime;
				 // Highlight the GoTo box to hint at what to do next as well.
				 gotoButton->BackColor = Color::Lime;
			 }
	private: System::Void taskIDBox_Leave(System::Object^  sender, System::EventArgs^  e) {
				 // Remove the highlighting.
				 taskIDBox->BackColor = SystemColors::Window;
				 // Restart updating after manually specifying a value.
				 ForceUpdate();
			 }
	private: System::Void taskIDBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 // If user presses <enter>, treat it like pressing GoTo.
				 if ( e->KeyChar == 0x0d ){
					 gotoButton->Focus();
					 InvokeOnClick(gotoButton, System::EventArgs::Empty);
				 }
			 }
	private: System::Void stepIDBox_Enter(System::Object^  sender, System::EventArgs^  e) {
				// Inhibit script crawler update if we start to manually enter an ID.
				scriptLiveCheckbox->Checked = false;
				ImpedeUpdate();
				// Highlight the box that is awaiting input, in case we clicked by mistake.
				stepIDBox->BackColor = Color::Lime;
				 // Highlight the GoTo box to hint at what to do next as well.
				 gotoButton->BackColor = Color::Lime;
			}
	private: System::Void stepIDBox_Leave(System::Object^  sender, System::EventArgs^  e) {
				 // Remove the highlighting.
				 stepIDBox->BackColor = SystemColors::Window;
				 // Restart updating after manually specifying a value.
				 ForceUpdate();
			 }
	private: System::Void stepIDBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 // If user presses <enter>, treat it like pressing GoTo.
				 if ( e->KeyChar == 0x0d ){
					 gotoButton->Focus();
					 InvokeOnClick(gotoButton, System::EventArgs::Empty);
				 }
			 }
	private: System::Void filterConstantTextBox_Enter(System::Object^  sender, System::EventArgs^  e) {
				// Inhibit data update if we start to manually enter a filter constant.
				ImpedeUpdate();
				// Highlight to show that the box is awaiting input.
				filterConstantTextBox->BackColor = Color::Lime;
			 }
	private: System::Void filterConstantTextBox_Leave(System::Object^  sender, System::EventArgs^  e) {
				filterConstantTextBox->BackColor = SystemColors::Window;
				filterCheckbox_CheckedChanged( sender, e);
			 }
	private: System::Void filterConstantTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				if ( e->KeyChar == 0x0d ){
					// If the user has changed the filter constant, they probably want filtering to be on.
					filterCheckbox->Checked = true;
					// If user hits return, change the focus to the filter button mainly just to leave focus.
					filterCheckbox->Focus();
				}
			 }

	// The GUI includes a number of checkboxes and pulldown lists to set the configuration.
	// The Forms objects take care of changing the state according to the action and 
	// the state of these objects will be read by the various graphing routines. But
	// we need to force an update when the state of these indicators changes.
	private: System::Void filterCheckbox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				// Read the filter constant value to be used from the text box on the screen,
				// taking care that it is a valid number.
				double filter_constant;
				try {
					filter_constant = System::Convert::ToDouble( filterConstantTextBox->Text );
				}
				catch (System::FormatException^ e) {
					// If there is a conversion error signal that the text box value is
					// not valid and use 0.0 by default.
					filterConstantTextBox->BackColor = Color::Pink;
					filter_constant = 0.0;
					e; // Ingore e. This is here to avoid warning about unused.
				}
				 if ( filterCheckbox->Checked ) dex.SetFilterConstant( filter_constant );
				 else dex.SetFilterConstant( 0.0 );
				 ForceUpdate();
			 }
	private: System::Void scriptLiveCheckbox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Check the new state of the checkbox after the state change.
				 if ( scriptLiveCheckbox->Checked ) {
					 // Clear the text boxes until the next HK packet comes in.
					 GoToSpecifiedIDs( UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED );
					 // Force an update of the screen.
					 ForceUpdate();
				 }
			 }
	private: System::Void dataLiveCheckbox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if ( dataLiveCheckbox->Checked ) ForceUpdate();
			 }
	private: System::Void autoscaleCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 ForceUpdate();
			 }
	private: System::Void graphCollectionComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 ForceUpdate();
			 }

	// There is a dropdown menu to select which collections of data to plot in the strip charts.
	// It can take a little time to make the selection. If the refresh timer goes off while the user
	// is still trying to select a collection, things get messed up. So the following event handlers
	// inhibit update while the selection is in progress, then restarts.
	private: System::Void graphCollectionComboBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 // Don't do an update of the data screens while the user
				 //  is making a selection.
				 ImpedeUpdate();
			 }
	private: System::Void graphCollectionComboBox_MouseCaptureChanged(System::Object^  sender, System::EventArgs^  e) {
				 // If the combo box loses focus, restart periodic updates.
				 if ( !(this->graphCollectionComboBox->Focused) ) ForceUpdate();
			 }
	private: System::Void graphCollectionComboBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 // If the user leaves the combo box by pressing ESC or RETURN
				 //  we need to restart the periodic refresh.
				 if ( e->KeyChar == 0x1b || e->KeyChar == 0x0d ) ForceUpdate();
			 }
	private: System::Void GripMMIDesktop_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 // Show an about box on right click in the main window.
				 if ( e->Button == System::Windows::Forms::MouseButtons::Right ) {
					 GripMMIAbout^ about = gcnew GripMMIAbout( GripMMIVersion, GripMMIBuildInfo );
					 about->ShowDialog();
				 }
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
						aboutForm->ShowDialog();
						return;
					}
					// Do what one would normally do.
					Form::WndProc( m );
				}

};

}

