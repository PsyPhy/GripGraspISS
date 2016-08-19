#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "GraspTrackerDaemon.h"

// A device that records 3D marker positions.
PsyPhy::CodaRTnetContinuousTracker codaTracker;
// A structured data buffer with fields for all of the tracker data.
static GraspTrackerRecord record;
// Read the echo of the packet into this buffer.
static GraspTrackerRecord echo;
static int record_length = sizeof( record );
SOCKET sock;
struct sockaddr_in Sender_addr;

namespace GraspTrackerDaemon {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace PsyPhy;

	// The data output by GraspTrackerDaemon.
	typedef struct {
		unsigned long count;
		PsyPhy::TrackerPose	hmd;
		PsyPhy::TrackerPose hand;
		PsyPhy::TrackerPose chest;
		int	nUnits;
		MarkerFrame frame[MAX_UNITS];
		PsyPhy::Vector3 alignmentOffset[MAX_UNITS];
		PsyPhy::Matrix3x3 alignmentRotation[MAX_UNITS];
	} GraspTrackerRecord;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:

		bool use_coda;

		Form1(void) : use_coda( true )
		{
			InitializeComponent();
			if ( 0 == _access_s( "NoCoda.flg", 0x00 ) ) use_coda = false;

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 
	private: System::Windows::Forms::Button^  exitButton;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  visibilityTextBox1;
	private: System::Windows::Forms::TextBox^  visibilityTextBox0;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  timeTextBox;

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
			this->exitButton = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->visibilityTextBox1 = (gcnew System::Windows::Forms::TextBox());
			this->visibilityTextBox0 = (gcnew System::Windows::Forms::TextBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->timeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// exitButton
			// 
			this->exitButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->exitButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->exitButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->exitButton->Location = System::Drawing::Point(21, 70);
			this->exitButton->Name = L"exitButton";
			this->exitButton->Size = System::Drawing::Size(133, 34);
			this->exitButton->TabIndex = 1;
			this->exitButton->Text = L"Exit";
			this->exitButton->UseVisualStyleBackColor = true;
			this->exitButton->Click += gcnew System::EventHandler(this, &Form1::exitButton_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->visibilityTextBox1);
			this->groupBox1->Controls->Add(this->visibilityTextBox0);
			this->groupBox1->Location = System::Drawing::Point(169, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(526, 94);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Marker Visibility";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(18, 65);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(144, 20);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Tracker Camera 2";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(18, 29);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(144, 20);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Tracker Camera 1";
			// 
			// visibilityTextBox1
			// 
			this->visibilityTextBox1->Font = (gcnew System::Drawing::Font(L"Consolas", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visibilityTextBox1->Location = System::Drawing::Point(181, 58);
			this->visibilityTextBox1->Name = L"visibilityTextBox1";
			this->visibilityTextBox1->Size = System::Drawing::Size(335, 31);
			this->visibilityTextBox1->TabIndex = 4;
			this->visibilityTextBox1->Text = L" 00000000  00000000  00000000 ";
			// 
			// visibilityTextBox0
			// 
			this->visibilityTextBox0->Font = (gcnew System::Drawing::Font(L"Consolas", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visibilityTextBox0->Location = System::Drawing::Point(181, 22);
			this->visibilityTextBox0->Name = L"visibilityTextBox0";
			this->visibilityTextBox0->Size = System::Drawing::Size(335, 31);
			this->visibilityTextBox0->TabIndex = 3;
			this->visibilityTextBox0->Text = L" 00000000  00000000  00000000 ";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->timeTextBox);
			this->groupBox2->Location = System::Drawing::Point(15, 15);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(148, 49);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Sample Time";
			// 
			// timeTextBox
			// 
			this->timeTextBox->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->timeTextBox->Location = System::Drawing::Point(6, 19);
			this->timeTextBox->Name = L"timeTextBox";
			this->timeTextBox->Size = System::Drawing::Size(133, 30);
			this->timeTextBox->TabIndex = 1;
			this->timeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->exitButton;
			this->ClientSize = System::Drawing::Size(695, 114);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->exitButton);
			this->KeyPreview = true;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspTrackerDaemon";
			this->WindowState = System::Windows::Forms::FormWindowState::Minimized;
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void exitButton_Click(System::Object^  sender, System::EventArgs^  e) {
			Close();
		}

		void ProcessCodaInputs( void ) {
			if ( use_coda ) {
				record.nUnits = codaTracker.nUnits;
				for ( int unit = 0; unit < record.nUnits; unit++ ) {
					codaTracker.GetCurrentMarkerFrameUnit( record.frame[unit], unit );
				}
			}
			else {
				record.count++;
				record.nUnits = codaTracker.nUnits;
				for ( int unit = 0; unit < record.nUnits; unit++ ) {
					record.frame[unit].time = (double) record.count / 200.0;
					for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
						if ( ( (mrk + record.count / 10 ) / 8 ) % 4 == unit ) record.frame[unit].marker[mrk].visibility = true;
						else record.frame[unit].marker[mrk].visibility = false;
					}
				}
			}

			String ^line;
			int unit = 0;
			double frame_time = record.frame[unit].time;
			timeTextBox->Text = frame_time.ToString("F3");
			line = " ";
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( record.frame[unit].marker[mrk].visibility ) line += "O";
				else line += ".";
				if ( ((mrk+1) % 8) == 0 ) line += "  ";
			}
			visibilityTextBox0->Text = line;
			unit = 1;
			line = " ";
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( record.frame[unit].marker[mrk].visibility ) line += "O";
				else line += ".";
				if ( ((mrk+1) % 8) == 0 ) line += "  ";
			}
			visibilityTextBox1->Text = line;
			if ( sendto( sock, (char *) &record, sizeof( record ), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
			{
				int error_value = WSAGetLastError();
				closesocket( sock );
				fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );		
			}
			fprintf( stderr, "message sent successfully\n" );
		}

		System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

			if ( use_coda ) {
				visibilityTextBox0->Text = " Initializing CODA ... ";
				visibilityTextBox1->Text = "   (Please wait.)";
				Refresh();
				Application::DoEvents();

				codaTracker.Initialize();
				visibilityTextBox1->Text = "";
				visibilityTextBox0->Text = " Initializing CODA ... OK.";
				for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {
					codaTracker.GetAlignment( record.alignmentOffset, record.alignmentRotation );
				}
			}
			else {
				visibilityTextBox0->Text = "Simulating startup ... ";
				visibilityTextBox1->Text = "   (Please wait.)";
				Refresh();
				Application::DoEvents();
				codaTracker.nUnits = MAX_UNITS;
				codaTracker.nMarkers = MAX_MARKERS;
				for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {
					codaTracker.CopyVector( record.alignmentOffset[unit], codaTracker.zeroVector );
					codaTracker.CopyMatrix( record.alignmentRotation[unit], codaTracker.identityMatrix );
				}
				Sleep( 5000 );
				visibilityTextBox1->Text = "";
				visibilityTextBox0->Text = " Simulating startup ... OK.";
			}

			// Initialize a socket to which we will broadcast the data.
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);

			sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

			BOOL enabled = TRUE;
			if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0 ) 
			{
				closesocket(sock);
				fAbortMessage( "GraspTrackerDaemon", "Error setting broadcast options." );		
			}

			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons( TRACKER_DAEMON_PORT );
			Sender_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );

			CreateRefreshTimer( 100 );
			StartRefreshTimer();
		}

		System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			StopRefreshTimer();
			if ( use_coda ) {
				codaTracker.AbortAcquisition();
				codaTracker.Quit();
			}
		}

		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			ProcessCodaInputs();
		}
		void CreateRefreshTimer( int interval ) {
			refreshTimer = gcnew( System::Windows::Forms::Timer );
			refreshTimer->Interval = interval;
			refreshTimer->Tick += gcnew EventHandler( this, &Form1::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			refreshTimer->Start();
		}
		void StopRefreshTimer( void ) {
			refreshTimer->Stop();
		}		

	};
}

