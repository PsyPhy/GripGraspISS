#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#include "../VectorsMixin/VectorsMixin.h"
#include "../GraspVR/GraspTrackers.h"

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
		PsyPhy::CodaRTnetTracker *coda;
		Grasp::GraspDexTrackers *trackers;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TextBox^  chestPoseTextBox;


	private: System::Windows::Forms::TextBox^  handPoseTextBox;

	private: System::Windows::Forms::TextBox^  hmdPoseTextBox;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;

	public: 
		PsyPhy::VectorsMixin	*vm;

		Form1(void) : use_coda( true )
		{
			InitializeComponent();
			if ( 0 == _access_s( "NoCoda.flg", 0x00 ) ) use_coda = false;
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
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->chestPoseTextBox = (gcnew System::Windows::Forms::TextBox());
			this->handPoseTextBox = (gcnew System::Windows::Forms::TextBox());
			this->hmdPoseTextBox = (gcnew System::Windows::Forms::TextBox());
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
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label5);
			this->groupBox3->Controls->Add(this->label4);
			this->groupBox3->Controls->Add(this->label3);
			this->groupBox3->Controls->Add(this->chestPoseTextBox);
			this->groupBox3->Controls->Add(this->handPoseTextBox);
			this->groupBox3->Controls->Add(this->hmdPoseTextBox);
			this->groupBox3->Location = System::Drawing::Point(21, 119);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(674, 130);
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
			this->chestPoseTextBox->Size = System::Drawing::Size(589, 29);
			this->chestPoseTextBox->TabIndex = 7;
			this->chestPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// handPoseTextBox
			// 
			this->handPoseTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->handPoseTextBox->Location = System::Drawing::Point(75, 58);
			this->handPoseTextBox->Name = L"handPoseTextBox";
			this->handPoseTextBox->Size = System::Drawing::Size(589, 29);
			this->handPoseTextBox->TabIndex = 6;
			this->handPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// hmdPoseTextBox
			// 
			this->hmdPoseTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->hmdPoseTextBox->Location = System::Drawing::Point(75, 21);
			this->hmdPoseTextBox->Name = L"hmdPoseTextBox";
			this->hmdPoseTextBox->Size = System::Drawing::Size(589, 29);
			this->hmdPoseTextBox->TabIndex = 5;
			this->hmdPoseTextBox->Text = L" 00000000  00000000  00000000 ";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->exitButton;
			this->ClientSize = System::Drawing::Size(701, 262);
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

		}
#pragma endregion
	private: 
		System::Void exitButton_Click(System::Object^  sender, System::EventArgs^  e) {
			Close();
		}

		void ProcessCodaInputs( void );
		void InitializeCoda( void );
		void ReleaseCoda( void );
		void InitializeSocket( void );
		void FakeTheCodaData( void );

		System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

			visibilityTextBox0->Text = " Initializing CODA ... ";
			visibilityTextBox1->Text = "   (Please wait.)";
			Refresh();
			Application::DoEvents();

			InitializeCoda();
			InitializeSocket();

			visibilityTextBox1->Text = "";
			visibilityTextBox0->Text = " Initializing CODA ... OK.";

			CreateRefreshTimer( 1 );
			StartRefreshTimer();
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


};
}

