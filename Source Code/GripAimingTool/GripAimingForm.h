#pragma once

#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"
#include "../GraspVR/GraspGLObjects.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"

#include "../GraspMMIUtilities/GripGLObjects.h"

#define DEX_CODA_UNITS 2
#define DEX_MARKERS 20

#define WRIST_MARKER_COLOR { 0.75, 0.5, 0.5, 0.5 }
#define MANIPULANDUM_MARKER_COLOR { 0.3f, 0.0f, 0.7f, 0.5f }
static GLfloat  markerColor[][4] = {
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	MANIPULANDUM_MARKER_COLOR,
	{ 0.0f, 0.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.8f, 1.0f },
	{ !.0f, 0.8f, 0.0f, 1.0f },
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR,
	WRIST_MARKER_COLOR };

	namespace GripAimingTool {

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
		public ref class GripAimingForm : public System::Windows::Forms::Form
		{
		public:

			Tracker *tracker;

			GripAimingForm ( Tracker *tracker ) : unit( 0 )
			{
				InitializeComponent();
				stabilityMeter = gcnew array<System::Windows::Forms::ProgressBar^>(DEX_MARKERS);
				this->tracker = tracker;
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~GripAimingForm()
			{
				if (components)
				{
					delete components;
				}
			}

#pragma region Windows Form Designer Object

			// An array of progress bars to simplify the coding.
		private: array<System::Windows::Forms::ProgressBar^>^ stabilityMeter;
		private: System::Windows::Forms::GroupBox^  boresightGroup0;
		private: System::Windows::Forms::Panel^  boresightPanel0;
		private: System::Windows::Forms::GroupBox^  groupBox4;
		private: System::Windows::Forms::GroupBox^  groupBox6;
		private: System::Windows::Forms::ProgressBar^  leftStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  rightStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  lowerStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  upperStabilityMeter0;
		private: System::Windows::Forms::Panel^  tabletPanel0;
		private: System::Windows::Forms::GroupBox^  groupBox3;
		private: System::Windows::Forms::Panel^  distancePanel0;
		private: System::Windows::Forms::GroupBox^  groupBox1;
		private: System::Windows::Forms::Button^  cameraButton1;
		private: System::Windows::Forms::Button^  cameraButton0;
		private: System::Windows::Forms::Button^  exitButton;
		private: System::Windows::Forms::TextBox^  intializing;
		private: System::Windows::Forms::TextBox^  instructions;
		private: System::Windows::Forms::Label^  label4;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::GroupBox^  groupBox2;
		private: System::Windows::Forms::Button^  wideButton;
		private: System::Windows::Forms::Button^  narrowButton;
		private: System::Windows::Forms::RadioButton^  supineButton;
		private: System::Windows::Forms::RadioButton^  seatedButton;
		private: System::Windows::Forms::Label^  label6;
		private: System::Windows::Forms::Label^  label5;




#pragma endregion

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
				System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GripAimingForm::typeid));
				this->instructions = (gcnew System::Windows::Forms::TextBox());
				this->intializing = (gcnew System::Windows::Forms::TextBox());
				this->boresightGroup0 = (gcnew System::Windows::Forms::GroupBox());
				this->boresightPanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
				this->supineButton = (gcnew System::Windows::Forms::RadioButton());
				this->seatedButton = (gcnew System::Windows::Forms::RadioButton());
				this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
				this->label4 = (gcnew System::Windows::Forms::Label());
				this->label3 = (gcnew System::Windows::Forms::Label());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->leftStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->rightStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->lowerStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->upperStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->tabletPanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
				this->distancePanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->cameraButton1 = (gcnew System::Windows::Forms::Button());
				this->cameraButton0 = (gcnew System::Windows::Forms::Button());
				this->exitButton = (gcnew System::Windows::Forms::Button());
				this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
				this->wideButton = (gcnew System::Windows::Forms::Button());
				this->narrowButton = (gcnew System::Windows::Forms::Button());
				this->label5 = (gcnew System::Windows::Forms::Label());
				this->label6 = (gcnew System::Windows::Forms::Label());
				this->boresightGroup0->SuspendLayout();
				this->groupBox4->SuspendLayout();
				this->groupBox6->SuspendLayout();
				this->groupBox3->SuspendLayout();
				this->groupBox1->SuspendLayout();
				this->groupBox2->SuspendLayout();
				this->SuspendLayout();
				// 
				// instructions
				// 
				this->instructions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->instructions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->instructions->Location = System::Drawing::Point(12, 12);
				this->instructions->Multiline = true;
				this->instructions->Name = L"instructions";
				this->instructions->ReadOnly = true;
				this->instructions->Size = System::Drawing::Size(1508, 163);
				this->instructions->TabIndex = 1;
				this->instructions->Text = resources->GetString(L"instructions.Text");
				// 
				// intializing
				// 
				this->intializing->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 72, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->intializing->ForeColor = System::Drawing::Color::DarkRed;
				this->intializing->Location = System::Drawing::Point(265, 421);
				this->intializing->Name = L"intializing";
				this->intializing->Size = System::Drawing::Size(921, 116);
				this->intializing->TabIndex = 5;
				this->intializing->Text = L"Initializing Tracker";
				this->intializing->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
				this->intializing->Visible = false;
				// 
				// boresightGroup0
				// 
				this->boresightGroup0->Controls->Add(this->boresightPanel0);
				this->boresightGroup0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, 
					System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				this->boresightGroup0->Location = System::Drawing::Point(119, 181);
				this->boresightGroup0->Name = L"boresightGroup0";
				this->boresightGroup0->Size = System::Drawing::Size(1141, 913);
				this->boresightGroup0->TabIndex = 6;
				this->boresightGroup0->TabStop = false;
				this->boresightGroup0->Text = L"Boresight";
				// 
				// boresightPanel0
				// 
				this->boresightPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->boresightPanel0->Location = System::Drawing::Point(10, 26);
				this->boresightPanel0->Name = L"boresightPanel0";
				this->boresightPanel0->Size = System::Drawing::Size(1121, 875);
				this->boresightPanel0->TabIndex = 0;
				// 
				// groupBox4
				// 
				this->groupBox4->Controls->Add(this->supineButton);
				this->groupBox4->Controls->Add(this->seatedButton);
				this->groupBox4->Controls->Add(this->groupBox6);
				this->groupBox4->Controls->Add(this->tabletPanel0);
				this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox4->Location = System::Drawing::Point(1266, 181);
				this->groupBox4->Name = L"groupBox4";
				this->groupBox4->Size = System::Drawing::Size(254, 378);
				this->groupBox4->TabIndex = 8;
				this->groupBox4->TabStop = false;
				this->groupBox4->Text = L"Reference Markers";
				// 
				// supineButton
				// 
				this->supineButton->AutoSize = true;
				this->supineButton->Location = System::Drawing::Point(141, 224);
				this->supineButton->Name = L"supineButton";
				this->supineButton->Size = System::Drawing::Size(88, 28);
				this->supineButton->TabIndex = 3;
				this->supineButton->Text = L"Supine";
				this->supineButton->UseVisualStyleBackColor = true;
				// 
				// seatedButton
				// 
				this->seatedButton->AutoSize = true;
				this->seatedButton->Checked = true;
				this->seatedButton->Location = System::Drawing::Point(26, 224);
				this->seatedButton->Name = L"seatedButton";
				this->seatedButton->Size = System::Drawing::Size(87, 28);
				this->seatedButton->TabIndex = 2;
				this->seatedButton->TabStop = true;
				this->seatedButton->Text = L"Seated";
				this->seatedButton->UseVisualStyleBackColor = true;
				// 
				// groupBox6
				// 
				this->groupBox6->Controls->Add(this->label4);
				this->groupBox6->Controls->Add(this->label3);
				this->groupBox6->Controls->Add(this->label2);
				this->groupBox6->Controls->Add(this->label1);
				this->groupBox6->Controls->Add(this->leftStabilityMeter0);
				this->groupBox6->Controls->Add(this->rightStabilityMeter0);
				this->groupBox6->Controls->Add(this->lowerStabilityMeter0);
				this->groupBox6->Controls->Add(this->upperStabilityMeter0);
				this->groupBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox6->Location = System::Drawing::Point(12, 271);
				this->groupBox6->Name = L"groupBox6";
				this->groupBox6->Size = System::Drawing::Size(236, 101);
				this->groupBox6->TabIndex = 1;
				this->groupBox6->TabStop = false;
				this->groupBox6->Text = L"Visibility Stability";
				// 
				// label4
				// 
				this->label4->AutoSize = true;
				this->label4->Location = System::Drawing::Point(124, 28);
				this->label4->Name = L"label4";
				this->label4->Size = System::Drawing::Size(20, 24);
				this->label4->TabIndex = 12;
				this->label4->Text = L"4";
				// 
				// label3
				// 
				this->label3->AutoSize = true;
				this->label3->Location = System::Drawing::Point(7, 28);
				this->label3->Name = L"label3";
				this->label3->Size = System::Drawing::Size(20, 24);
				this->label3->TabIndex = 11;
				this->label3->Text = L"3";
				// 
				// label2
				// 
				this->label2->AutoSize = true;
				this->label2->Location = System::Drawing::Point(8, 66);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(20, 24);
				this->label2->TabIndex = 10;
				this->label2->Text = L"2";
				// 
				// label1
				// 
				this->label1->AutoSize = true;
				this->label1->Location = System::Drawing::Point(124, 66);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(20, 24);
				this->label1->TabIndex = 9;
				this->label1->Text = L"1";
				// 
				// leftStabilityMeter0
				// 
				this->leftStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->leftStabilityMeter0->ForeColor = System::Drawing::Color::Blue;
				this->leftStabilityMeter0->Location = System::Drawing::Point(147, 66);
				this->leftStabilityMeter0->Maximum = 10;
				this->leftStabilityMeter0->Name = L"leftStabilityMeter0";
				this->leftStabilityMeter0->RightToLeftLayout = true;
				this->leftStabilityMeter0->Size = System::Drawing::Size(84, 24);
				this->leftStabilityMeter0->Step = 1;
				this->leftStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->leftStabilityMeter0->TabIndex = 8;
				this->leftStabilityMeter0->Value = 5;
				// 
				// rightStabilityMeter0
				// 
				this->rightStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->rightStabilityMeter0->ForeColor = System::Drawing::Color::Yellow;
				this->rightStabilityMeter0->Location = System::Drawing::Point(147, 28);
				this->rightStabilityMeter0->Maximum = 10;
				this->rightStabilityMeter0->Name = L"rightStabilityMeter0";
				this->rightStabilityMeter0->RightToLeftLayout = true;
				this->rightStabilityMeter0->Size = System::Drawing::Size(84, 24);
				this->rightStabilityMeter0->Step = 1;
				this->rightStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->rightStabilityMeter0->TabIndex = 7;
				this->rightStabilityMeter0->Value = 5;
				// 
				// lowerStabilityMeter0
				// 
				this->lowerStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->lowerStabilityMeter0->ForeColor = System::Drawing::Color::Cyan;
				this->lowerStabilityMeter0->Location = System::Drawing::Point(30, 66);
				this->lowerStabilityMeter0->Maximum = 10;
				this->lowerStabilityMeter0->Name = L"lowerStabilityMeter0";
				this->lowerStabilityMeter0->RightToLeftLayout = true;
				this->lowerStabilityMeter0->Size = System::Drawing::Size(84, 24);
				this->lowerStabilityMeter0->Step = 1;
				this->lowerStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->lowerStabilityMeter0->TabIndex = 6;
				this->lowerStabilityMeter0->Value = 5;
				// 
				// upperStabilityMeter0
				// 
				this->upperStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->upperStabilityMeter0->ForeColor = System::Drawing::Color::DeepPink;
				this->upperStabilityMeter0->Location = System::Drawing::Point(30, 28);
				this->upperStabilityMeter0->Maximum = 10;
				this->upperStabilityMeter0->Name = L"upperStabilityMeter0";
				this->upperStabilityMeter0->RightToLeftLayout = true;
				this->upperStabilityMeter0->Size = System::Drawing::Size(84, 24);
				this->upperStabilityMeter0->Step = 1;
				this->upperStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->upperStabilityMeter0->TabIndex = 5;
				this->upperStabilityMeter0->Value = 5;
				// 
				// tabletPanel0
				// 
				this->tabletPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->tabletPanel0->Location = System::Drawing::Point(12, 25);
				this->tabletPanel0->Name = L"tabletPanel0";
				this->tabletPanel0->Size = System::Drawing::Size(230, 186);
				this->tabletPanel0->TabIndex = 0;
				// 
				// groupBox3
				// 
				this->groupBox3->Controls->Add(this->label6);
				this->groupBox3->Controls->Add(this->label5);
				this->groupBox3->Controls->Add(this->distancePanel0);
				this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox3->ForeColor = System::Drawing::SystemColors::ActiveCaption;
				this->groupBox3->Location = System::Drawing::Point(12, 181);
				this->groupBox3->Name = L"groupBox3";
				this->groupBox3->Size = System::Drawing::Size(101, 913);
				this->groupBox3->TabIndex = 7;
				this->groupBox3->TabStop = false;
				this->groupBox3->Text = L"Distance";
				// 
				// distancePanel0
				// 
				this->distancePanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->distancePanel0->Location = System::Drawing::Point(10, 48);
				this->distancePanel0->Name = L"distancePanel0";
				this->distancePanel0->Size = System::Drawing::Size(81, 834);
				this->distancePanel0->TabIndex = 0;
				// 
				// groupBox1
				// 
				this->groupBox1->Controls->Add(this->cameraButton1);
				this->groupBox1->Controls->Add(this->cameraButton0);
				this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox1->Location = System::Drawing::Point(1266, 565);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Size = System::Drawing::Size(254, 280);
				this->groupBox1->TabIndex = 9;
				this->groupBox1->TabStop = false;
				this->groupBox1->Text = L"Tracking Camera";
				// 
				// cameraButton1
				// 
				this->cameraButton1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->cameraButton1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 72, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->cameraButton1->ForeColor = System::Drawing::SystemColors::ControlText;
				this->cameraButton1->Location = System::Drawing::Point(29, 154);
				this->cameraButton1->Name = L"cameraButton1";
				this->cameraButton1->Size = System::Drawing::Size(196, 120);
				this->cameraButton1->TabIndex = 12;
				this->cameraButton1->Text = L"2";
				this->cameraButton1->UseVisualStyleBackColor = true;
				this->cameraButton1->Click += gcnew System::EventHandler(this, &GripAimingForm::cameraButton1_Click);
				// 
				// cameraButton0
				// 
				this->cameraButton0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->cameraButton0->BackColor = System::Drawing::SystemColors::Highlight;
				this->cameraButton0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 72, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->cameraButton0->ForeColor = System::Drawing::SystemColors::ControlText;
				this->cameraButton0->Location = System::Drawing::Point(29, 28);
				this->cameraButton0->Name = L"cameraButton0";
				this->cameraButton0->Size = System::Drawing::Size(196, 120);
				this->cameraButton0->TabIndex = 11;
				this->cameraButton0->Text = L"1";
				this->cameraButton0->UseVisualStyleBackColor = false;
				this->cameraButton0->Click += gcnew System::EventHandler(this, &GripAimingForm::cameraButton0_Click);
				// 
				// exitButton
				// 
				this->exitButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->exitButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->exitButton->ForeColor = System::Drawing::SystemColors::ControlText;
				this->exitButton->Location = System::Drawing::Point(1326, 1046);
				this->exitButton->Name = L"exitButton";
				this->exitButton->Size = System::Drawing::Size(146, 48);
				this->exitButton->TabIndex = 10;
				this->exitButton->Text = L"Exit";
				this->exitButton->UseVisualStyleBackColor = true;
				this->exitButton->Click += gcnew System::EventHandler(this, &GripAimingForm::exitButton_Click);
				// 
				// groupBox2
				// 
				this->groupBox2->Controls->Add(this->wideButton);
				this->groupBox2->Controls->Add(this->narrowButton);
				this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox2->Location = System::Drawing::Point(1266, 851);
				this->groupBox2->Name = L"groupBox2";
				this->groupBox2->Size = System::Drawing::Size(254, 180);
				this->groupBox2->TabIndex = 11;
				this->groupBox2->TabStop = false;
				this->groupBox2->Text = L"Boresight Zoom";
				// 
				// wideButton
				// 
				this->wideButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->wideButton->BackColor = System::Drawing::SystemColors::Highlight;
				this->wideButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->wideButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->wideButton->ForeColor = System::Drawing::SystemColors::ControlText;
				this->wideButton->Location = System::Drawing::Point(29, 28);
				this->wideButton->Name = L"wideButton";
				this->wideButton->Size = System::Drawing::Size(196, 70);
				this->wideButton->TabIndex = 16;
				this->wideButton->Text = L"Wide";
				this->wideButton->UseVisualStyleBackColor = false;
				this->wideButton->Click += gcnew System::EventHandler(this, &GripAimingForm::wideButton_Click);
				// 
				// narrowButton
				// 
				this->narrowButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->narrowButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->narrowButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->narrowButton->ForeColor = System::Drawing::SystemColors::ControlText;
				this->narrowButton->Location = System::Drawing::Point(29, 104);
				this->narrowButton->Name = L"narrowButton";
				this->narrowButton->Size = System::Drawing::Size(196, 70);
				this->narrowButton->TabIndex = 15;
				this->narrowButton->Text = L"Narrow";
				this->narrowButton->UseVisualStyleBackColor = true;
				this->narrowButton->Click += gcnew System::EventHandler(this, &GripAimingForm::narrowButton_Click);
				// 
				// label5
				// 
				this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label5->Location = System::Drawing::Point(10, 26);
				this->label5->Name = L"label5";
				this->label5->Size = System::Drawing::Size(80, 22);
				this->label5->TabIndex = 1;
				this->label5->Text = L"far";
				this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				// 
				// label6
				// 
				this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label6->Location = System::Drawing::Point(11, 885);
				this->label6->Name = L"label6";
				this->label6->Size = System::Drawing::Size(80, 22);
				this->label6->TabIndex = 2;
				this->label6->Text = L"near";
				this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				// 
				// GripAimingForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->CancelButton = this->exitButton;
				this->ClientSize = System::Drawing::Size(1532, 1106);
				this->Controls->Add(this->groupBox2);
				this->Controls->Add(this->exitButton);
				this->Controls->Add(this->groupBox1);
				this->Controls->Add(this->boresightGroup0);
				this->Controls->Add(this->groupBox4);
				this->Controls->Add(this->groupBox3);
				this->Controls->Add(this->intializing);
				this->Controls->Add(this->instructions);
				this->ForeColor = System::Drawing::SystemColors::ActiveCaption;
				this->Name = L"GripAimingForm";
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
				this->Text = L"Grip Aiming Tool";
				this->Shown += gcnew System::EventHandler(this, &GripAimingForm::GripAimingForm_Shown);
				this->boresightGroup0->ResumeLayout(false);
				this->groupBox4->ResumeLayout(false);
				this->groupBox4->PerformLayout();
				this->groupBox6->ResumeLayout(false);
				this->groupBox6->PerformLayout();
				this->groupBox3->ResumeLayout(false);
				this->groupBox1->ResumeLayout(false);
				this->groupBox2->ResumeLayout(false);
				this->ResumeLayout(false);
				this->PerformLayout();

			}
#pragma endregion

		public:

			OpenGLWindow	*boresightWindow;
			Viewpoint		*boresightViewpoint;

			OpenGLWindow	*tabletWindow;
			Viewpoint		*tabletViewpoint;

			OpenGLWindow	*distanceWindow;
			OrthoViewpoint	*distanceViewpoint;
			Patch			*lowerExclusionZone, *upperExclusionZone;
			Assembly		*distanceBars;
			Sphere			**objectMarker;

			Assembly		*fovSweetSpot;

			static double	wide_fov = 70.0;
			static double	zoom_fov = 30.0;

			static double crosshair_thickness = 2.0;
			static double crosshair_radius = 600.0;
			static double wide_crosshair_distance = 1000.0;
			static double zoom_crosshair_distance = 2400.0;

			static bool		show_all = false;

			static double	minDisplayDistance = 1000.0;
			static double	maxDisplayDistance = 5000.0;
			static double	minAcceptableDistance = 1800.0;
			static double	maxAcceptableDistance = 4000.0;

			Assembly		*markerSpheres;
			static double	markerRadius = 30.0;
			Sphere			*targetSphere;
			static double	targetRadius = 200.0;

			static int		firstTablet = 8;
			static int		lastTablet = 11;

			static int lowerMastMarker = 11;
			static int upperMastMarker = 10;
			static int leftTabletMarker = 9;
			static int rightTabletMarker = 8;

			GripTablet		*tablet;
			static double	tabletMarkerRadius = 30.0;

			int unit;

		private: void SetLighting( void ) {

					 GLfloat fintensity = 0.75;
					 GLfloat ambient = fintensity;
					 GLfloat diffuse = fintensity;
					 GLfloat specular = fintensity;

					 // Light definition
					 GLfloat LightPosition[] = { 10000.0, 0.0, 0.0, 0.0 };
					 GLfloat LightAmbient[]  = { ambient, ambient, ambient, 1.0};
					 GLfloat LightDiffuse[]  = { diffuse, diffuse, diffuse, 1.0};
					 GLfloat LightSpecular[] = { 0.0, 0.0, 0.0, 1.0};

					 glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
					 glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
					 glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
					 glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

					 glEnable( GL_LIGHT0 );

					 glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
					 glEnable( GL_BLEND );

					 glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
					 glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );

				 }

		private: void EraseWindow( OpenGLWindow *window, double r, double g, double b, double transparency ) {
					 window->Activate();
					 window->Clear( r, g, b, transparency );
					 window->Swap();
				 }
		private: void EraseWindow( OpenGLWindow *window ) {
					 window->Activate();
					 window->Clear( 0.1, 0.1, 0.3, 1.0 );
					 window->Swap();
				 }

		private: void DrawDistance( OpenGLWindow *window ) {
					 window->Activate();
					 window->Clear( 0.65, 0.85, 0.65, 1.0 );
					 distanceViewpoint->Apply( window );
					 SetLighting();
					 lowerExclusionZone->Draw();
					 upperExclusionZone->Draw();
					 distanceBars->Draw();
					 window->Swap();
				 }

		private: void DrawBoresight( OpenGLWindow *window ) {
					 window->Activate();
					 window->Clear( 0.0, 0.0, 0.05, 1.0 );
					 // Draw the other objects with the hopes of seeing specular reflections. 
					 boresightViewpoint->Apply( window );
					 SetLighting();
					 fovSweetSpot->Draw();
					 markerSpheres->Draw();
					 targetSphere->Draw();
					 window->Swap();
				 }

		private: void DrawObject( OpenGLWindow *window, Viewpoint *viewpoint, OpenGLObject *object ) {
					 window->Activate();
					 window->Clear( 0.1, 0.1, 0.3, 1.0 );
					 // Draw the other objects with the hopes of seeing specular reflections. 
					 viewpoint->Apply( window );
					 SetLighting();
					 object->Draw();
					 window->Swap();
				 }


		private: void Redraw( void ) {

					 static VectorsMixin vm;
					 MarkerFrame frame;

					 tracker->GetCurrentMarkerFrameIntrinsic( frame, unit );
					 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
						 markerSpheres->component[mrk]->enabled = frame.marker[mrk].visibility;
						 distanceBars->component[mrk]->enabled = frame.marker[mrk].visibility;
						 if ( frame.marker[mrk].visibility	) {
							 objectMarker[mrk]->SetColor( markerColor[mrk] );
							 markerSpheres->component[mrk]->SetPosition( frame.marker[mrk].position );
							 distanceBars->component[mrk]->SetPosition( mrk, frame.marker[mrk].position[Y], 5.0 );
						 }
						 else objectMarker[mrk]->SetColor( BLACK );

						 if ( frame.marker[lowerMastMarker].visibility && frame.marker[upperMastMarker].visibility ) {
							 Vector3 average;
							 vm.AddVectors( average, frame.marker[lowerMastMarker].position, frame.marker[upperMastMarker].position );
							 vm.ScaleVector( average, average, 0.5 );
							 targetSphere->SetPosition( average );
							 targetSphere->Enable();
						 }
						 else targetSphere->Disable();
					 }
					 DrawDistance( distanceWindow );
					 DrawBoresight( boresightWindow );

					 if ( seatedButton->Checked ) {
						 tablet->SetOrientation( 0.0, 10.0, 180.0 );
						 tablet->SetPosition( 250.0, -200.0, 0.0 );
					 }
					 else {
						 tablet->SetOrientation( - 20.0, 90.0, 0.0 );
						 tablet->SetPosition( -220.0, 200.0, 0.0 );
					 }


					 DrawObject( tabletWindow, tabletViewpoint, tablet );

					 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
						 if ( frame.marker[mrk].visibility ) {
							 if ( stabilityMeter[mrk]->Value < 10 ) stabilityMeter[mrk]->Value++;
						 }
						 else {
							 if ( stabilityMeter[mrk]->Value > 0 ) stabilityMeter[mrk]->Value--;
						 }
					 }

				 }

		private: System::Void GripAimingForm_Shown(System::Object^  sender, System::EventArgs^  e) {

					 char *init_file = "GripAimingTool.ini";  
					 fOutputDebugString( "GripAimingForm (%p): Parsing %s.\n", this, init_file );
					 FILE *fp = fopen( init_file, "r" );
					 if ( !fp ) fAbortMessage( "Grip Aiming Tool", "Unable to open configuration file %s", init_file );
					 fscanf( fp, "%lf %lf %lf %lf", &minDisplayDistance, &minAcceptableDistance, &maxAcceptableDistance, &maxDisplayDistance );
					 fclose( fp );
					 fOutputDebugString( "Limits: %f  %f  %f  %f\n",  minDisplayDistance, minAcceptableDistance, maxAcceptableDistance, maxDisplayDistance );


					 intializing->Visible = true;
					 instructions->SelectionStart = 5000;
					 instructions->SelectionLength = 0;
					 Refresh();
					 intializing->Visible = false;

					 // The boresight window shows the markers from the perspective of the CODA location.
					 boresightWindow = CreateOpenGLWindowInForm( boresightPanel0 );
					 boresightViewpoint =  new Viewpoint( 6.0, wide_fov, 1000.0, 6000.0);
					 // When then CODA is in it's 0 orientation, it points along
					 // the positive Y axis. This attitude reflects that in the 
					 // representation of the CODA bar.
					 boresightViewpoint->SetAttitude( 0.0, - 90.0, 0.0 );
					 // The origin of the CODA intrinsic coordinate system is the 
					 // center of the A camera.
					 boresightViewpoint->SetOffset( - 400.0, 0.0, 0.0 );

					 // Create a targeting scope.
					 fovSweetSpot = new Assembly();

					 Annulus *ring = new Annulus( crosshair_radius, crosshair_thickness );
					 ring->SetColor( GRAY );
					 ring->SetOrientation( 0.0, 90.0, 0.0 );
					 fovSweetSpot->AddComponent( ring );

					 Bar *crosshair = new Bar( 2.0 * crosshair_radius, crosshair_thickness, crosshair_thickness );
					 crosshair->SetPosition( 0.0, - crosshair_radius, 0.0 );
					 crosshair->SetOrientation( 0.0, 90.0, 0.0 );
					 crosshair->SetColor( GRAY );
					 fovSweetSpot->AddComponent( crosshair );

					 crosshair = new Bar( 2.0 * crosshair_radius, crosshair_thickness, crosshair_thickness );
					 crosshair->SetPosition( crosshair_radius, 0.0, 0.0 );
					 crosshair->SetOrientation( 0.0, 0.0, 90.0 );
					 crosshair->SetColor( GRAY );
					 fovSweetSpot->AddComponent( crosshair );

					 fovSweetSpot->SetOrientation( 0.0, 90.0, 0.0 );
					 fovSweetSpot->SetOffset( -400.0, 0.0, 0.0 );
					 fovSweetSpot->SetPosition( 0.0, 1000.0, 0.0 );

					 // Create objects for the distance display.
					 distanceWindow = CreateOpenGLWindowInForm( distancePanel0, boresightWindow->hRC );
					 distanceViewpoint = new OrthoViewpoint( firstTablet - 1.0, lastTablet + 1.0, minDisplayDistance, maxDisplayDistance );
					 lowerExclusionZone = new Patch( lastTablet - firstTablet + 2.0, minAcceptableDistance - minDisplayDistance );
					 lowerExclusionZone->SetOffset( 0.0, ( minAcceptableDistance - minDisplayDistance ) / 2.0, 0.0 );
					 lowerExclusionZone->SetPosition( ( lastTablet + firstTablet ) / 2.0, minDisplayDistance, - 10.0 );
					 lowerExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );
					 upperExclusionZone = new Patch( lastTablet - firstTablet + 2.0, maxDisplayDistance - maxAcceptableDistance );
					 upperExclusionZone->SetOffset( 0.0, - ( maxDisplayDistance - maxAcceptableDistance ) / 2.0, 0.0 );
					 upperExclusionZone->SetPosition(( lastTablet + firstTablet ) / 2.0, maxDisplayDistance, - 10.0 );
					 upperExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );

					 // The tablet window is where we show the visibility
					 // of the reference markers supperimposed on a 3D representation
					 // of the GRIP workspace tablet.
					 tabletWindow = CreateOpenGLWindowInForm( tabletPanel0, boresightWindow->hRC );
					 tabletViewpoint = new Viewpoint( 6.0, 30.0, 10.0, 10000.0);
					 tabletViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
					 tabletViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

					 // Create a set of spheres to be shown in the boresight view.
					 markerSpheres = new Assembly();
					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 Sphere *sphere = new Sphere( markerRadius );
						 sphere->SetPosition( mrk  * 100.0, minDisplayDistance + mrk  * 100.0, mrk * 100.0 );
						 markerSpheres->AddComponent( sphere );
					 }
					 targetSphere = new Sphere( targetRadius );
					 targetSphere->SetColor( 0.2, 1.0, 0.2, 0.35 );
					 targetSphere->SetPosition( 0.0, minDisplayDistance + 9  * 100.0, 0.0 );
					 targetSphere->SetAttitude( 90.0, 0.0, 0.0 );

					 // These bars indicate the distance of the marker in depth 
					 // from the position of the CODA unit.
					 distanceBars = new Assembly();
					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 Slab *bar = new Slab( 1.0, 30.0, 20.0 );
						 bar->SetPosition( 0.0, minDisplayDistance + ( mrk + 2)  * 100.0, 5.0 );
						 bar->SetAttitude( 0.0, 0.0, 0.0 );
						 distanceBars->AddComponent( bar );
					 }

					 // Finally, we have a set of marker spheres that are superimposed on the 
					 // objects to show visibility.
					 objectMarker = (Sphere **) calloc( DEX_MARKERS, sizeof(Sphere *) );
					 fAbortMessageOnCondition( objectMarker == 0, "Grip Aiming Form", "Error allocating memory for marker spheres." );
					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 objectMarker[mrk] = new Sphere( tabletMarkerRadius );
					 }

					 // The tablet is the one and only object for which we show visibility.
					 tablet = new GripTablet();
					 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
						 objectMarker[mrk]->SetRadius( tabletMarkerRadius );
						 tablet->AddComponent( objectMarker[mrk] );
					 }
					 objectMarker[leftTabletMarker]->SetPosition(    - 220.0,   -270.0,  35.0 );
					 objectMarker[rightTabletMarker]->SetPosition(     220.0,   -270.0,  35.0 );
					 objectMarker[upperMastMarker]->SetPosition(     - 220.0,   -140.0, 600.0 );
					 objectMarker[lowerMastMarker]->SetPosition(     - 220.0,   -140.0, 120.0 );


					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 objectMarker[mrk]->SetColor( markerColor[mrk] );
						 markerSpheres->component[mrk]->SetColor( markerColor[mrk] );
						 distanceBars->component[mrk]->SetColor( markerColor[mrk] );
						 objectMarker[mrk]->Disable();
						 markerSpheres->component[mrk]->Disable();
						 distanceBars->component[mrk]->Disable();
					 }
					 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
						 objectMarker[mrk]->Enable();
						 markerSpheres->component[mrk]->Enable();
						 distanceBars->component[mrk]->Enable();
					 }

					 for ( int unit = 0; unit < DEX_CODA_UNITS; unit++ ) {
						 EraseWindow( boresightWindow );
						 EraseWindow( tabletWindow );
						 EraseWindow( distanceWindow );
					 }


					 stabilityMeter[leftTabletMarker] = leftStabilityMeter0;
					 stabilityMeter[rightTabletMarker] = rightStabilityMeter0;
					 stabilityMeter[upperMastMarker] = upperStabilityMeter0;
					 stabilityMeter[lowerMastMarker] = lowerStabilityMeter0;

					 Redraw();
					 CreateRefreshTimer( 500 );
					 StartRefreshTimer();



				 }
				 // A timer to handle animations and screen refresh, and associated actions.
				 static System::Windows::Forms::Timer^ refreshTimer;
				 void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
					 StopRefreshTimer();
					 Redraw();
					 StartRefreshTimer();
				 }
				 void CreateRefreshTimer( int interval ) {
					 refreshTimer = gcnew( System::Windows::Forms::Timer );
					 refreshTimer->Interval = interval;
					 refreshTimer->Tick += gcnew EventHandler( this, &GripAimingForm::OnTimerElapsed );
				 }
				 void StartRefreshTimer( void ) {
					 refreshTimer->Start();
				 }
				 void StopRefreshTimer( void ) {
					 refreshTimer->Stop();
				 }		

		private: System::Void cameraButton0_Click(System::Object^  sender, System::EventArgs^  e) {
					 unit = 0;
					 cameraButton0->BackColor = System::Drawing::SystemColors::Highlight;
					 cameraButton1->BackColor = System::Drawing::SystemColors::Control;
				 }
		private: System::Void cameraButton1_Click(System::Object^  sender, System::EventArgs^  e) {
					 unit = 1;
					 cameraButton1->BackColor = System::Drawing::SystemColors::Highlight;
					 cameraButton0->BackColor = System::Drawing::SystemColors::Control;
				 }
		private: System::Void exitButton_Click(System::Object^  sender, System::EventArgs^  e) {
					 Close();
				 }

		private: System::Void wideButton_Click(System::Object^  sender, System::EventArgs^  e) {
					 boresightViewpoint->fov = wide_fov;
					 fovSweetSpot->SetPosition( 0.0, wide_crosshair_distance, 0.0 );
					 wideButton->BackColor = System::Drawing::SystemColors::Highlight;
					 narrowButton->BackColor = System::Drawing::SystemColors::Control;
				 }

		private: System::Void narrowButton_Click(System::Object^  sender, System::EventArgs^  e) {
					 boresightViewpoint->fov = zoom_fov;
					 fovSweetSpot->SetPosition( 0.0, zoom_crosshair_distance, 0.0 );
					 narrowButton->BackColor = System::Drawing::SystemColors::Highlight;
					 wideButton->BackColor = System::Drawing::SystemColors::Control;
				 }
		};

	}

