#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#include "../VectorsMixin/VectorsMixin.h"
#include "../GraspVR/GraspTrackers.h"

// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

namespace GraspTrackerDaemon {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace PsyPhy;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:

		bool use_coda;
		bool use_legacy;
		bool autohide;

		PsyPhy::Tracker *coda;
		Grasp::GraspDexTrackers *trackers;
		bool recording;
		unsigned int nPoseSamples;

	public: 

	public: 

	public: 
		PsyPhy::VectorsMixin	*vm;

		Form1(void) : use_coda( true ), use_legacy( false ), autohide( false ), recording( false ), nPoseSamples( 0 )
		{
			InitializeComponent();
			if ( 0 == _access_s( "FakeCoda.flg", 0x00 ) ) use_coda = false;
			if ( 0 == _access_s( "LegacyCoda.flg", 0x00 ) ) { use_coda = false; use_legacy = true; }
			vm = new VectorsMixin();
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
		///
		/// Designer generated objects
	private: 
		System::Windows::Forms::Button^  exitButton;
		System::Windows::Forms::GroupBox^  groupBox1;
		System::Windows::Forms::Label^  label2;
		System::Windows::Forms::Label^  label1;
		System::Windows::Forms::TextBox^  visibilityTextBox1;
		System::Windows::Forms::TextBox^  visibilityTextBox0;
		System::Windows::Forms::GroupBox^  groupBox2;
		System::Windows::Forms::TextBox^  timeTextBox0;
		System::Windows::Forms::TextBox^  timeTextBox1;
		System::Windows::Forms::Button^  startButton;
		System::Windows::Forms::Button^  stopButton;
		System::Windows::Forms::Button^  saveButton;
		System::Windows::Forms::SaveFileDialog^  saveFileDialog1;

		System::Windows::Forms::GroupBox^  groupBox3;
		System::Windows::Forms::TextBox^  chestPoseTextBox;
		System::Windows::Forms::TextBox^  handPoseTextBox;
		System::Windows::Forms::TextBox^  hmdPoseTextBox;
		System::Windows::Forms::Label^  label3;
		System::Windows::Forms::Label^  label5;
		System::Windows::Forms::Label^  label4;

	private:
		/// Required designer variable.
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
			this->visibilityTextBox1 = (gcnew System::Windows::Forms::TextBox());
			this->visibilityTextBox0 = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->timeTextBox1 = (gcnew System::Windows::Forms::TextBox());
			this->timeTextBox0 = (gcnew System::Windows::Forms::TextBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->chestPoseTextBox = (gcnew System::Windows::Forms::TextBox());
			this->handPoseTextBox = (gcnew System::Windows::Forms::TextBox());
			this->hmdPoseTextBox = (gcnew System::Windows::Forms::TextBox());
			this->startButton = (gcnew System::Windows::Forms::Button());
			this->stopButton = (gcnew System::Windows::Forms::Button());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// exitButton
			// 
			this->exitButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->exitButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->exitButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->exitButton->Location = System::Drawing::Point(583, 254);
			this->exitButton->Name = L"exitButton";
			this->exitButton->Size = System::Drawing::Size(133, 46);
			this->exitButton->TabIndex = 1;
			this->exitButton->Text = L"Exit";
			this->exitButton->UseVisualStyleBackColor = true;
			this->exitButton->Click += gcnew System::EventHandler(this, &Form1::exitButton_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->visibilityTextBox1);
			this->groupBox1->Controls->Add(this->visibilityTextBox0);
			this->groupBox1->Location = System::Drawing::Point(369, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(354, 94);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Marker Visibility";
			// 
			// visibilityTextBox1
			// 
			this->visibilityTextBox1->Font = (gcnew System::Drawing::Font(L"Consolas", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visibilityTextBox1->Location = System::Drawing::Point(6, 57);
			this->visibilityTextBox1->Name = L"visibilityTextBox1";
			this->visibilityTextBox1->Size = System::Drawing::Size(335, 31);
			this->visibilityTextBox1->TabIndex = 4;
			this->visibilityTextBox1->Text = L" 00000000  00000000  00000000 ";
			// 
			// visibilityTextBox0
			// 
			this->visibilityTextBox0->Font = (gcnew System::Drawing::Font(L"Consolas", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visibilityTextBox0->Location = System::Drawing::Point(6, 20);
			this->visibilityTextBox0->Name = L"visibilityTextBox0";
			this->visibilityTextBox0->Size = System::Drawing::Size(335, 31);
			this->visibilityTextBox0->TabIndex = 3;
			this->visibilityTextBox0->Text = L" 00000000  00000000  00000000 ";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(30, 72);
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
			this->label1->Location = System::Drawing::Point(30, 36);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(144, 20);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Tracker Camera 1";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->timeTextBox1);
			this->groupBox2->Controls->Add(this->timeTextBox0);
			this->groupBox2->Location = System::Drawing::Point(203, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(148, 94);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Sample Time";
			// 
			// timeTextBox1
			// 
			this->timeTextBox1->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->timeTextBox1->Location = System::Drawing::Point(6, 54);
			this->timeTextBox1->Name = L"timeTextBox1";
			this->timeTextBox1->Size = System::Drawing::Size(133, 30);
			this->timeTextBox1->TabIndex = 7;
			this->timeTextBox1->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// timeTextBox0
			// 
			this->timeTextBox0->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->timeTextBox0->Location = System::Drawing::Point(6, 18);
			this->timeTextBox0->Name = L"timeTextBox0";
			this->timeTextBox0->Size = System::Drawing::Size(133, 30);
			this->timeTextBox0->TabIndex = 1;
			this->timeTextBox0->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label5);
			this->groupBox3->Controls->Add(this->label4);
			this->groupBox3->Controls->Add(this->label3);
			this->groupBox3->Controls->Add(this->chestPoseTextBox);
			this->groupBox3->Controls->Add(this->handPoseTextBox);
			this->groupBox3->Controls->Add(this->hmdPoseTextBox);
			this->groupBox3->Location = System::Drawing::Point(16, 119);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(707, 130);
			this->groupBox3->TabIndex = 6;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Pose Trackers";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(14, 99);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(53, 20);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Chest";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(18, 62);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(49, 20);
			this->label4->TabIndex = 9;
			this->label4->Text = L"Hand";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(18, 25);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(49, 20);
			this->label3->TabIndex = 8;
			this->label3->Text = L"HMD";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// chestPoseTextBox
			// 
			this->chestPoseTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chestPoseTextBox->Location = System::Drawing::Point(75, 95);
			this->chestPoseTextBox->Name = L"chestPoseTextBox";
			this->chestPoseTextBox->Size = System::Drawing::Size(625, 29);
			this->chestPoseTextBox->TabIndex = 7;
			this->chestPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// handPoseTextBox
			// 
			this->handPoseTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->handPoseTextBox->Location = System::Drawing::Point(75, 58);
			this->handPoseTextBox->Name = L"handPoseTextBox";
			this->handPoseTextBox->Size = System::Drawing::Size(625, 29);
			this->handPoseTextBox->TabIndex = 6;
			this->handPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// hmdPoseTextBox
			// 
			this->hmdPoseTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->hmdPoseTextBox->Location = System::Drawing::Point(75, 21);
			this->hmdPoseTextBox->Name = L"hmdPoseTextBox";
			this->hmdPoseTextBox->Size = System::Drawing::Size(625, 29);
			this->hmdPoseTextBox->TabIndex = 5;
			this->hmdPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// startButton
			// 
			this->startButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->startButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->startButton->Location = System::Drawing::Point(16, 255);
			this->startButton->Name = L"startButton";
			this->startButton->Size = System::Drawing::Size(133, 46);
			this->startButton->TabIndex = 7;
			this->startButton->Text = L"Record";
			this->startButton->UseVisualStyleBackColor = true;
			this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_Click);
			// 
			// stopButton
			// 
			this->stopButton->Enabled = false;
			this->stopButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->stopButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stopButton->Location = System::Drawing::Point(155, 255);
			this->stopButton->Name = L"stopButton";
			this->stopButton->Size = System::Drawing::Size(133, 46);
			this->stopButton->TabIndex = 8;
			this->stopButton->Text = L"Stop";
			this->stopButton->UseVisualStyleBackColor = true;
			this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
			// 
			// saveButton
			// 
			this->saveButton->Enabled = false;
			this->saveButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->saveButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->saveButton->Location = System::Drawing::Point(294, 255);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(133, 46);
			this->saveButton->TabIndex = 9;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// saveFileDialog1
			// 
			this->saveFileDialog1->DefaultExt = L"mrk";
			this->saveFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveFileDialog1_FileOk);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->exitButton;
			this->ClientSize = System::Drawing::Size(735, 313);
			this->Controls->Add(this->saveButton);
			this->Controls->Add(this->stopButton);
			this->Controls->Add(this->startButton);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->exitButton);
			this->KeyPreview = true;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspTrackerDaemon";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: 
		System::Void exitButton_Click(System::Object^  sender, System::EventArgs^  e) {
			this->Enabled = false;
			Close();
		}

		void ProcessCodaInputs( void );
		void InitializeCoda( void );
		void ReleaseCoda( void );
		void InitializeSocket( void );

		System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

			visibilityTextBox0->Text = " Initializing CODA ... ";
			visibilityTextBox1->Text = "   (Please wait.)";
			Refresh();
			Application::DoEvents();

			InitializeSocket();
			InitializeCoda();

			visibilityTextBox1->Text = "";
			visibilityTextBox0->Text = " Initializing CfODA ... OK.";

			CreateRefreshTimer( 2 );
			StartRefreshTimer();
			if ( autohide ) WindowState = System::Windows::Forms::FormWindowState::Minimized;
			 
		}

		System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			StopRefreshTimer();
			ReleaseCoda();
		}

		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			StopRefreshTimer();
			ProcessCodaInputs();
			StartRefreshTimer();
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


	private: System::Void startButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 startButton->Enabled = false;
				 stopButton->Enabled = true;
				 saveButton->Enabled = false;
				 trackers->codaTracker->StartAcquisition( 1000.0 );
				 nPoseSamples = 0;
				 recording = true;
			 }
	private: System::Void stopButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 startButton->Enabled = true;
				 stopButton->Enabled = false;
				 saveButton->Enabled = true;
				 trackers->codaTracker->StopAcquisition();
				 recording = false;
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 recording = false;
				 saveFileDialog1->ShowDialog();
			 }

	private: System::Void saveFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	};
}

