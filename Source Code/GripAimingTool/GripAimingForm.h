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

			GripAimingForm ( Tracker *tracker )
			{
				InitializeComponent();

				stabilityMeter = gcnew array<System::Windows::Forms::ProgressBar^, 2>(2,DEX_MARKERS);

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

		private: System::Windows::Forms::GroupBox^  groupBox1;
		private: System::Windows::Forms::GroupBox^  groupBox5;
		private: System::Windows::Forms::GroupBox^  groupBox4;
		private: System::Windows::Forms::GroupBox^  groupBox3;
		private: System::Windows::Forms::GroupBox^  boresightGroup0;
		private: System::Windows::Forms::Panel^  boresightPanel0;
		private: System::Windows::Forms::Panel^  manipulandumPanel0;
		private: System::Windows::Forms::Panel^  tabletPanel0;
		private: System::Windows::Forms::Panel^  distancePanel0;
		private: System::Windows::Forms::TextBox^  instructions;
		private: System::Windows::Forms::GroupBox^  groupBox2;
		private: System::Windows::Forms::GroupBox^  groupBox7;
		private: System::Windows::Forms::Panel^  manipulandumPanel1;
		private: System::Windows::Forms::GroupBox^  groupBox9;
		private: System::Windows::Forms::Panel^  tabletPanel1;
		private: System::Windows::Forms::GroupBox^  groupBox10;
		private: System::Windows::Forms::Panel^  distancePanel1;
		private: System::Windows::Forms::GroupBox^  groupBox11;
		private: System::Windows::Forms::Panel^  boresightPanel1;
		private: System::Windows::Forms::TextBox^  intializing;
		private: System::Windows::Forms::GroupBox^  groupBox6;
		private: System::Windows::Forms::ProgressBar^  leftStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  rightStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  lowerStabilityMeter0;
		private: System::Windows::Forms::ProgressBar^  upperStabilityMeter0;
		private: System::Windows::Forms::GroupBox^  groupBox8;
		private: System::Windows::Forms::ProgressBar^  leftStabilityMeter1;
		private: System::Windows::Forms::ProgressBar^  rightStabilityMeter1;
		private: System::Windows::Forms::ProgressBar^  lowerStabilityMeter1;
		private: System::Windows::Forms::ProgressBar^  upperStabilityMeter1;


				 // An array of progress bars to simplify the coding.
		private: array<System::Windows::Forms::ProgressBar^, 2 >^ stabilityMeter;


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
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->boresightGroup0 = (gcnew System::Windows::Forms::GroupBox());
				this->boresightPanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
				this->manipulandumPanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
				this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
				this->leftStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->rightStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->lowerStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->upperStabilityMeter0 = (gcnew System::Windows::Forms::ProgressBar());
				this->tabletPanel0 = (gcnew System::Windows::Forms::Panel());
				this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
				this->distancePanel0 = (gcnew System::Windows::Forms::Panel());
				this->instructions = (gcnew System::Windows::Forms::TextBox());
				this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
				this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
				this->manipulandumPanel1 = (gcnew System::Windows::Forms::Panel());
				this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
				this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
				this->leftStabilityMeter1 = (gcnew System::Windows::Forms::ProgressBar());
				this->rightStabilityMeter1 = (gcnew System::Windows::Forms::ProgressBar());
				this->lowerStabilityMeter1 = (gcnew System::Windows::Forms::ProgressBar());
				this->upperStabilityMeter1 = (gcnew System::Windows::Forms::ProgressBar());
				this->tabletPanel1 = (gcnew System::Windows::Forms::Panel());
				this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
				this->distancePanel1 = (gcnew System::Windows::Forms::Panel());
				this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
				this->boresightPanel1 = (gcnew System::Windows::Forms::Panel());
				this->intializing = (gcnew System::Windows::Forms::TextBox());
				this->groupBox1->SuspendLayout();
				this->boresightGroup0->SuspendLayout();
				this->groupBox5->SuspendLayout();
				this->groupBox4->SuspendLayout();
				this->groupBox6->SuspendLayout();
				this->groupBox3->SuspendLayout();
				this->groupBox2->SuspendLayout();
				this->groupBox7->SuspendLayout();
				this->groupBox9->SuspendLayout();
				this->groupBox8->SuspendLayout();
				this->groupBox10->SuspendLayout();
				this->groupBox11->SuspendLayout();
				this->SuspendLayout();
				// 
				// groupBox1
				// 
				this->groupBox1->Controls->Add(this->boresightGroup0);
				this->groupBox1->Controls->Add(this->groupBox5);
				this->groupBox1->Controls->Add(this->groupBox4);
				this->groupBox1->Controls->Add(this->groupBox3);
				this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox1->Location = System::Drawing::Point(12, 12);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Size = System::Drawing::Size(750, 929);
				this->groupBox1->TabIndex = 0;
				this->groupBox1->TabStop = false;
				this->groupBox1->Text = L"Tracking Camera 1";
				// 
				// boresightGroup0
				// 
				this->boresightGroup0->Controls->Add(this->boresightPanel0);
				this->boresightGroup0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
					System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				this->boresightGroup0->Location = System::Drawing::Point(103, 28);
				this->boresightGroup0->Name = L"boresightGroup0";
				this->boresightGroup0->Size = System::Drawing::Size(641, 678);
				this->boresightGroup0->TabIndex = 0;
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
				this->boresightPanel0->Size = System::Drawing::Size(621, 640);
				this->boresightPanel0->TabIndex = 0;
				// 
				// groupBox5
				// 
				this->groupBox5->Controls->Add(this->manipulandumPanel0);
				this->groupBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox5->Location = System::Drawing::Point(408, 712);
				this->groupBox5->Name = L"groupBox5";
				this->groupBox5->Size = System::Drawing::Size(336, 211);
				this->groupBox5->TabIndex = 2;
				this->groupBox5->TabStop = false;
				this->groupBox5->Text = L"Mobile Markers";
				// 
				// manipulandumPanel0
				// 
				this->manipulandumPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->manipulandumPanel0->Location = System::Drawing::Point(8, 25);
				this->manipulandumPanel0->Name = L"manipulandumPanel0";
				this->manipulandumPanel0->Size = System::Drawing::Size(320, 179);
				this->manipulandumPanel0->TabIndex = 1;
				// 
				// groupBox4
				// 
				this->groupBox4->Controls->Add(this->groupBox6);
				this->groupBox4->Controls->Add(this->tabletPanel0);
				this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox4->Location = System::Drawing::Point(8, 712);
				this->groupBox4->Name = L"groupBox4";
				this->groupBox4->Size = System::Drawing::Size(384, 211);
				this->groupBox4->TabIndex = 1;
				this->groupBox4->TabStop = false;
				this->groupBox4->Text = L"Reference Markers";
				// 
				// groupBox6
				// 
				this->groupBox6->Controls->Add(this->leftStabilityMeter0);
				this->groupBox6->Controls->Add(this->rightStabilityMeter0);
				this->groupBox6->Controls->Add(this->lowerStabilityMeter0);
				this->groupBox6->Controls->Add(this->upperStabilityMeter0);
				this->groupBox6->Location = System::Drawing::Point(259, 17);
				this->groupBox6->Name = L"groupBox6";
				this->groupBox6->Size = System::Drawing::Size(119, 185);
				this->groupBox6->TabIndex = 1;
				this->groupBox6->TabStop = false;
				this->groupBox6->Text = L"Stability";
				// 
				// leftStabilityMeter0
				// 
				this->leftStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->leftStabilityMeter0->ForeColor = System::Drawing::Color::Blue;
				this->leftStabilityMeter0->Location = System::Drawing::Point(11, 141);
				this->leftStabilityMeter0->Maximum = 10;
				this->leftStabilityMeter0->Name = L"leftStabilityMeter0";
				this->leftStabilityMeter0->RightToLeftLayout = true;
				this->leftStabilityMeter0->Size = System::Drawing::Size(96, 24);
				this->leftStabilityMeter0->Step = 1;
				this->leftStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->leftStabilityMeter0->TabIndex = 8;
				this->leftStabilityMeter0->Value = 5;
				// 
				// rightStabilityMeter0
				// 
				this->rightStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->rightStabilityMeter0->ForeColor = System::Drawing::Color::Yellow;
				this->rightStabilityMeter0->Location = System::Drawing::Point(11, 103);
				this->rightStabilityMeter0->Maximum = 10;
				this->rightStabilityMeter0->Name = L"rightStabilityMeter0";
				this->rightStabilityMeter0->RightToLeftLayout = true;
				this->rightStabilityMeter0->Size = System::Drawing::Size(96, 24);
				this->rightStabilityMeter0->Step = 1;
				this->rightStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->rightStabilityMeter0->TabIndex = 7;
				this->rightStabilityMeter0->Value = 5;
				// 
				// lowerStabilityMeter0
				// 
				this->lowerStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->lowerStabilityMeter0->ForeColor = System::Drawing::Color::Cyan;
				this->lowerStabilityMeter0->Location = System::Drawing::Point(11, 65);
				this->lowerStabilityMeter0->Maximum = 10;
				this->lowerStabilityMeter0->Name = L"lowerStabilityMeter0";
				this->lowerStabilityMeter0->RightToLeftLayout = true;
				this->lowerStabilityMeter0->Size = System::Drawing::Size(96, 24);
				this->lowerStabilityMeter0->Step = 1;
				this->lowerStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->lowerStabilityMeter0->TabIndex = 6;
				this->lowerStabilityMeter0->Value = 5;
				// 
				// upperStabilityMeter0
				// 
				this->upperStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->upperStabilityMeter0->ForeColor = System::Drawing::Color::DeepPink;
				this->upperStabilityMeter0->Location = System::Drawing::Point(11, 27);
				this->upperStabilityMeter0->Maximum = 10;
				this->upperStabilityMeter0->Name = L"upperStabilityMeter0";
				this->upperStabilityMeter0->RightToLeftLayout = true;
				this->upperStabilityMeter0->Size = System::Drawing::Size(96, 24);
				this->upperStabilityMeter0->Step = 1;
				this->upperStabilityMeter0->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->upperStabilityMeter0->TabIndex = 5;
				this->upperStabilityMeter0->Value = 5;
				// 
				// tabletPanel0
				// 
				this->tabletPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->tabletPanel0->Location = System::Drawing::Point(8, 25);
				this->tabletPanel0->Name = L"tabletPanel0";
				this->tabletPanel0->Size = System::Drawing::Size(236, 178);
				this->tabletPanel0->TabIndex = 0;
				// 
				// groupBox3
				// 
				this->groupBox3->Controls->Add(this->distancePanel0);
				this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox3->ForeColor = System::Drawing::SystemColors::ActiveCaption;
				this->groupBox3->Location = System::Drawing::Point(8, 28);
				this->groupBox3->Name = L"groupBox3";
				this->groupBox3->Size = System::Drawing::Size(89, 678);
				this->groupBox3->TabIndex = 1;
				this->groupBox3->TabStop = false;
				this->groupBox3->Text = L"Distance";
				// 
				// distancePanel0
				// 
				this->distancePanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->distancePanel0->Location = System::Drawing::Point(10, 26);
				this->distancePanel0->Name = L"distancePanel0";
				this->distancePanel0->Size = System::Drawing::Size(69, 639);
				this->distancePanel0->TabIndex = 0;
				// 
				// instructions
				// 
				this->instructions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->instructions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->instructions->Location = System::Drawing::Point(16, 947);
				this->instructions->Multiline = true;
				this->instructions->Name = L"instructions";
				this->instructions->ReadOnly = true;
				this->instructions->Size = System::Drawing::Size(1504, 147);
				this->instructions->TabIndex = 1;
				this->instructions->Text = resources->GetString(L"instructions.Text");
				// 
				// groupBox2
				// 
				this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				this->groupBox2->Controls->Add(this->groupBox7);
				this->groupBox2->Controls->Add(this->groupBox9);
				this->groupBox2->Controls->Add(this->groupBox10);
				this->groupBox2->Controls->Add(this->groupBox11);
				this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox2->Location = System::Drawing::Point(770, 12);
				this->groupBox2->Name = L"groupBox2";
				this->groupBox2->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
				this->groupBox2->Size = System::Drawing::Size(750, 929);
				this->groupBox2->TabIndex = 4;
				this->groupBox2->TabStop = false;
				this->groupBox2->Text = L"Tracking Camera 2";
				// 
				// groupBox7
				// 
				this->groupBox7->Controls->Add(this->manipulandumPanel1);
				this->groupBox7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox7->Location = System::Drawing::Point(408, 712);
				this->groupBox7->Name = L"groupBox7";
				this->groupBox7->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox7->Size = System::Drawing::Size(336, 211);
				this->groupBox7->TabIndex = 2;
				this->groupBox7->TabStop = false;
				this->groupBox7->Text = L"Mobile Markers";
				// 
				// manipulandumPanel1
				// 
				this->manipulandumPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->manipulandumPanel1->Location = System::Drawing::Point(8, 25);
				this->manipulandumPanel1->Name = L"manipulandumPanel1";
				this->manipulandumPanel1->Size = System::Drawing::Size(320, 179);
				this->manipulandumPanel1->TabIndex = 1;
				// 
				// groupBox9
				// 
				this->groupBox9->Controls->Add(this->groupBox8);
				this->groupBox9->Controls->Add(this->tabletPanel1);
				this->groupBox9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox9->Location = System::Drawing::Point(10, 712);
				this->groupBox9->Name = L"groupBox9";
				this->groupBox9->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox9->Size = System::Drawing::Size(384, 211);
				this->groupBox9->TabIndex = 1;
				this->groupBox9->TabStop = false;
				this->groupBox9->Text = L"Reference Markers";
				// 
				// groupBox8
				// 
				this->groupBox8->Controls->Add(this->leftStabilityMeter1);
				this->groupBox8->Controls->Add(this->rightStabilityMeter1);
				this->groupBox8->Controls->Add(this->lowerStabilityMeter1);
				this->groupBox8->Controls->Add(this->upperStabilityMeter1);
				this->groupBox8->Location = System::Drawing::Point(239, 19);
				this->groupBox8->Name = L"groupBox8";
				this->groupBox8->Size = System::Drawing::Size(119, 185);
				this->groupBox8->TabIndex = 2;
				this->groupBox8->TabStop = false;
				this->groupBox8->Text = L"Stability";
				// 
				// leftStabilityMeter1
				// 
				this->leftStabilityMeter1->BackColor = System::Drawing::SystemColors::ControlDark;
				this->leftStabilityMeter1->ForeColor = System::Drawing::Color::Blue;
				this->leftStabilityMeter1->Location = System::Drawing::Point(11, 141);
				this->leftStabilityMeter1->Maximum = 10;
				this->leftStabilityMeter1->Name = L"leftStabilityMeter1";
				this->leftStabilityMeter1->RightToLeftLayout = true;
				this->leftStabilityMeter1->Size = System::Drawing::Size(96, 24);
				this->leftStabilityMeter1->Step = 1;
				this->leftStabilityMeter1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->leftStabilityMeter1->TabIndex = 8;
				this->leftStabilityMeter1->Value = 5;
				// 
				// rightStabilityMeter1
				// 
				this->rightStabilityMeter1->BackColor = System::Drawing::SystemColors::ControlDark;
				this->rightStabilityMeter1->ForeColor = System::Drawing::Color::Yellow;
				this->rightStabilityMeter1->Location = System::Drawing::Point(11, 103);
				this->rightStabilityMeter1->Maximum = 10;
				this->rightStabilityMeter1->Name = L"rightStabilityMeter1";
				this->rightStabilityMeter1->RightToLeftLayout = true;
				this->rightStabilityMeter1->Size = System::Drawing::Size(96, 24);
				this->rightStabilityMeter1->Step = 1;
				this->rightStabilityMeter1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->rightStabilityMeter1->TabIndex = 7;
				this->rightStabilityMeter1->Value = 5;
				// 
				// lowerStabilityMeter1
				// 
				this->lowerStabilityMeter1->BackColor = System::Drawing::SystemColors::ControlDark;
				this->lowerStabilityMeter1->ForeColor = System::Drawing::Color::Cyan;
				this->lowerStabilityMeter1->Location = System::Drawing::Point(11, 65);
				this->lowerStabilityMeter1->Maximum = 10;
				this->lowerStabilityMeter1->Name = L"lowerStabilityMeter1";
				this->lowerStabilityMeter1->RightToLeftLayout = true;
				this->lowerStabilityMeter1->Size = System::Drawing::Size(96, 24);
				this->lowerStabilityMeter1->Step = 1;
				this->lowerStabilityMeter1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->lowerStabilityMeter1->TabIndex = 6;
				this->lowerStabilityMeter1->Value = 5;
				// 
				// upperStabilityMeter1
				// 
				this->upperStabilityMeter1->BackColor = System::Drawing::SystemColors::ControlDark;
				this->upperStabilityMeter1->ForeColor = System::Drawing::Color::DeepPink;
				this->upperStabilityMeter1->Location = System::Drawing::Point(11, 27);
				this->upperStabilityMeter1->Maximum = 10;
				this->upperStabilityMeter1->Name = L"upperStabilityMeter1";
				this->upperStabilityMeter1->RightToLeftLayout = true;
				this->upperStabilityMeter1->Size = System::Drawing::Size(96, 24);
				this->upperStabilityMeter1->Step = 1;
				this->upperStabilityMeter1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->upperStabilityMeter1->TabIndex = 5;
				this->upperStabilityMeter1->Value = 5;
				// 
				// tabletPanel1
				// 
				this->tabletPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->tabletPanel1->Location = System::Drawing::Point(8, 25);
				this->tabletPanel1->Name = L"tabletPanel1";
				this->tabletPanel1->Size = System::Drawing::Size(256, 178);
				this->tabletPanel1->TabIndex = 0;
				// 
				// groupBox10
				// 
				this->groupBox10->Controls->Add(this->distancePanel1);
				this->groupBox10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox10->Location = System::Drawing::Point(655, 28);
				this->groupBox10->Name = L"groupBox10";
				this->groupBox10->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox10->Size = System::Drawing::Size(89, 678);
				this->groupBox10->TabIndex = 1;
				this->groupBox10->TabStop = false;
				this->groupBox10->Text = L"Distance";
				// 
				// distancePanel1
				// 
				this->distancePanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->distancePanel1->Location = System::Drawing::Point(10, 26);
				this->distancePanel1->Name = L"distancePanel1";
				this->distancePanel1->Size = System::Drawing::Size(69, 639);
				this->distancePanel1->TabIndex = 0;
				// 
				// groupBox11
				// 
				this->groupBox11->Controls->Add(this->boresightPanel1);
				this->groupBox11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox11->Location = System::Drawing::Point(8, 28);
				this->groupBox11->Name = L"groupBox11";
				this->groupBox11->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox11->Size = System::Drawing::Size(641, 678);
				this->groupBox11->TabIndex = 0;
				this->groupBox11->TabStop = false;
				this->groupBox11->Text = L"Boresight";
				// 
				// boresightPanel1
				// 
				this->boresightPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->boresightPanel1->Location = System::Drawing::Point(10, 26);
				this->boresightPanel1->Name = L"boresightPanel1";
				this->boresightPanel1->Size = System::Drawing::Size(621, 640);
				this->boresightPanel1->TabIndex = 0;
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
				// GripAimingForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(1532, 1106);
				this->Controls->Add(this->intializing);
				this->Controls->Add(this->groupBox2);
				this->Controls->Add(this->instructions);
				this->Controls->Add(this->groupBox1);
				this->ForeColor = System::Drawing::SystemColors::ActiveCaption;
				this->Name = L"GripAimingForm";
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
				this->Text = L"Grip Aiming Tool";
				this->Shown += gcnew System::EventHandler(this, &GripAimingForm::GripAimingForm_Shown);
				this->groupBox1->ResumeLayout(false);
				this->boresightGroup0->ResumeLayout(false);
				this->groupBox5->ResumeLayout(false);
				this->groupBox4->ResumeLayout(false);
				this->groupBox6->ResumeLayout(false);
				this->groupBox3->ResumeLayout(false);
				this->groupBox2->ResumeLayout(false);
				this->groupBox7->ResumeLayout(false);
				this->groupBox9->ResumeLayout(false);
				this->groupBox8->ResumeLayout(false);
				this->groupBox10->ResumeLayout(false);
				this->groupBox11->ResumeLayout(false);
				this->ResumeLayout(false);
				this->PerformLayout();

			}
#pragma endregion


			OpenGLWindow	**boresightWindow;
			Viewpoint		*boresightViewpoint;

			OpenGLWindow	**tabletWindow;
			OpenGLWindow	**manipulandumWindow;
			Viewpoint		*objectViewpoint;
			Viewpoint		*tabletViewpoint;

			OpenGLWindow **distanceWindow;
			OrthoViewpoint *distanceViewpoint;
			Patch *lowerExclusionZone, *upperExclusionZone;
			Assembly	*distanceBars;
			Sphere		**objectMarker;


			static double	minDisplayDistance = 1000.0;
			static double	maxDisplayDistance = 5000.0;
			static double	minAcceptableDistance = 1800.0;
			static double	maxAcceptableDistance = 4000.0;

			Assembly		*markerSpheres;
			static double	markerRadius = 20.0;
			Sphere			*targetSphere;
			static double	targetRadius = 200.0;

			static int		firstManipulandum = 0;
			static int		lastManipulandum = 7;

			static int		firstTablet = 8;
			static int		lastTablet = 11;

			static int		firstWrist = 12;
			static int		lastWrist = 19;

			static int lowerMastMarker = 11;
			static int upperMastMarker = 10;
			static int leftTabletMarker = 8;
			static int rightTabletMarker = 9;

			GripTablet			*tablet;
			GripManipulandum	*manipulandum;
			GripWrist			*wrist;
			Assembly			*mobile;

			static double	objectMarkerRadius = 3.0;
			static double	tabletMarkerRadius = 30.0;

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
					 window->Clear( 0.85, 0.85, 0.85, 1.0 );
					 // Draw the other objects with the hopes of seeing specular reflections. 
					 boresightViewpoint->Apply( window );
					 SetLighting();
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
					 for ( int unit = 0; unit < DEX_CODA_UNITS; unit++ ) {
						 tracker->GetCurrentMarkerFrameIntrinsic( frame, unit );
						 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
							 markerSpheres->component[mrk]->enabled = frame.marker[mrk].visibility;
							 distanceBars->component[mrk]->enabled = frame.marker[mrk].visibility;
							 if ( frame.marker[mrk].visibility	) {
								 objectMarker[mrk]->SetColor( markerColor[mrk] );
								 markerSpheres->component[mrk]->SetPosition( frame.marker[mrk].position );
								 distanceBars->component[mrk]->SetPosition( 0.0, frame.marker[mrk].position[Y], 5.0 );
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
						 DrawDistance( distanceWindow[unit] );
						 DrawBoresight( boresightWindow[unit] );
						 DrawObject( manipulandumWindow[unit], objectViewpoint, mobile );
						 DrawObject( tabletWindow[unit], tabletViewpoint, tablet );

						 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
							 if ( frame.marker[mrk].visibility && stabilityMeter[unit, mrk]->Value < 10 ) stabilityMeter[unit, mrk]->Value++;
							 else if ( stabilityMeter[unit, mrk]->Value > 0 ) stabilityMeter[unit, mrk]->Value--;
						 }

					 }
				 }

		private: System::Void GripAimingForm_Shown(System::Object^  sender, System::EventArgs^  e) {

					 intializing->Visible = true;
					 instructions->SelectionStart = 5000;
					 instructions->SelectionLength = 0;
					 Refresh();
					 tracker->Initialize( "CodaRTnet.ini" );
					 intializing->Visible = false;

					 // Create the required OpenGLWindows each linked to a pane in the Form.
					 // Forms does not allow mixed managed and unmanaged arrays.
					 // So to have arrays of the following, they have to be 
					 // allocated at runtime. 
					 boresightWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
					 boresightWindow[0] = CreateOpenGLWindowInForm( boresightPanel0 );
					 boresightWindow[1] = CreateOpenGLWindowInForm( boresightPanel1, boresightWindow[0]->hRC );
					 boresightViewpoint =  new Viewpoint( 6.0, 70.0, 1000.0, 6000.0);
					 // When then CODA is in it's 0 orientation, it points along
					 // the positive Y axis. This attitude reflects that in the 
					 // representation of the CODA bar.
					 boresightViewpoint->SetAttitude( 0.0, - 90.0, 0.0 );
					 // The origin of the CODA intrinsic coordinate system is the 
					 // center of the A camera.
					 boresightViewpoint->SetOffset( - 400.0, 0.0, 0.0 );

					 tabletWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
					 tabletWindow[0] = CreateOpenGLWindowInForm( tabletPanel0, boresightWindow[0]->hRC );
					 tabletWindow[1] = CreateOpenGLWindowInForm( tabletPanel1, tabletWindow[0]->hRC );
					 manipulandumWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
					 manipulandumWindow[0] = CreateOpenGLWindowInForm( manipulandumPanel0, boresightWindow[0]->hRC );
					 manipulandumWindow[1] = CreateOpenGLWindowInForm( manipulandumPanel1, manipulandumWindow[0]->hRC );

					 // Create objects for the distance display.
					 distanceWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
					 distanceWindow[0] = CreateOpenGLWindowInForm( distancePanel0, boresightWindow[0]->hRC );
					 distanceWindow[1] = CreateOpenGLWindowInForm( distancePanel1, distanceWindow[0]->hRC );
					 distanceViewpoint = new OrthoViewpoint( -1.0, 1.0, minDisplayDistance, maxDisplayDistance );
					 lowerExclusionZone = new Patch( 2.0, minAcceptableDistance - minDisplayDistance );
					 lowerExclusionZone->SetOffset( 0.0, ( minAcceptableDistance - minDisplayDistance ) / 2.0, 0.0 );
					 lowerExclusionZone->SetPosition( 0.0, minDisplayDistance, - 10.0 );
					 lowerExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );
					 upperExclusionZone = new Patch( 2.0, maxDisplayDistance - maxAcceptableDistance );
					 upperExclusionZone->SetOffset( 0.0, - ( maxDisplayDistance - maxAcceptableDistance ) / 2.0, 0.0 );
					 upperExclusionZone->SetPosition( 0.0, maxDisplayDistance, - 10.0 );
					 upperExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );

					 distanceBars = new Assembly();
					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 Slab *bar = new Slab( 1.0, 30.0, 20.0 );
						 bar->SetPosition( 0.0, minDisplayDistance + ( mrk + 2)  * 100.0, 5.0 );
						 bar->SetAttitude( 0.0, 0.0, 0.0 );
						 distanceBars->AddComponent( bar );
					 }

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

					 objectMarker = (Sphere **) calloc( DEX_MARKERS, sizeof(Sphere *) );
					 fAbortMessageOnCondition( objectMarker == 0, "Grip Aiming Form", "Error allocating memory for marker spheres." );
					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 objectMarker[mrk] = new Sphere( objectMarkerRadius );
					 }

					 objectViewpoint = new Viewpoint( 6.0, 2.5, 10.0, 10000.0);
					 objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
					 objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

					 manipulandum = new GripManipulandum();
					 manipulandum->SetAttitude( 0.0, 0.0, - 90.0 );
					 for ( int mrk = firstManipulandum; mrk <= lastManipulandum; mrk++ ) {
						 manipulandum->AddComponent( objectMarker[mrk] );
					 }

					 objectMarker[firstManipulandum + 0]->SetPosition( 13.0, -20.0, -9.0 );
					 objectMarker[firstManipulandum + 1]->SetPosition( 13.0, -20.0,  9.0 );
					 objectMarker[firstManipulandum + 2]->SetPosition( 13.0,   0.0, -9.0 );
					 objectMarker[firstManipulandum + 3]->SetPosition( 13.0,   0.0,  9.0 );
					 objectMarker[firstManipulandum + 4]->SetPosition( 9.0,   20.0, -9.0 );
					 objectMarker[firstManipulandum + 5]->SetPosition( 9.0,   20.0,  9.0 );
					 objectMarker[firstManipulandum + 6]->SetPosition( -10.0, 25.0, -9.0 );
					 objectMarker[firstManipulandum + 7]->SetPosition( -10.0, 25.0,  9.0 );

					 tabletViewpoint = new Viewpoint( 6.0, 30.0, 10.0, 10000.0);
					 tabletViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
					 tabletViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

					 tablet = new GripTablet();
					 tablet->SetOrientation( 0.0, 0.0, 180.0 );
					 tablet->SetPosition( 250.0, -200.0, 0.0 );
					 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
						 objectMarker[mrk]->SetRadius( tabletMarkerRadius );
						 tablet->AddComponent( objectMarker[mrk] );
					 }
					 objectMarker[leftTabletMarker]->SetPosition(    - 220.0,   -250.0,  35.0 );
					 objectMarker[rightTabletMarker]->SetPosition(     220.0,   -250.0,  35.0 );
					 objectMarker[upperMastMarker]->SetPosition(     - 220.0,   -100.0, 600.0 );
					 objectMarker[lowerMastMarker]->SetPosition(     - 220.0,   -100.0, 100.0 );

					 wrist = new GripWrist();
					 wrist->SetOrientation( -90.0, 0.0, 0.0 );
					 for ( int mrk = firstWrist; mrk <= lastWrist; mrk++ ) {
						 wrist->AddComponent( objectMarker[mrk] );
					 }
					 objectMarker[firstWrist + 0]->SetPosition( -24.0, -20.0,  -20.0 );
					 objectMarker[firstWrist + 1]->SetPosition(  -8.0, -20.0,  -30.0 );
					 objectMarker[firstWrist + 2]->SetPosition(   8.0, -20.0,  -30.0 );
					 objectMarker[firstWrist + 3]->SetPosition(  24.0, -20.0,  -20.0 );
					 objectMarker[firstWrist + 4]->SetPosition( -18.0,  20.0,  -15.0 );
					 objectMarker[firstWrist + 5]->SetPosition(  -6.0,  20.0,  -25.0 );
					 objectMarker[firstWrist + 6]->SetPosition(   6.0,  20.0,  -25.0 );
					 objectMarker[firstWrist + 7]->SetPosition(  18.0,  20.0,  -15.0 );

					 wrist->SetPosition( 30.0, 0.0, 0.0 );
					 manipulandum->SetPosition( -45.0, 0.0, 0.0 );
					 mobile = new Assembly();
					 mobile->AddComponent( wrist );
					 mobile->AddComponent( manipulandum );

					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 objectMarker[mrk]->SetColor( markerColor[mrk] );
						 markerSpheres->component[mrk]->SetColor( markerColor[mrk] );
						 distanceBars->component[mrk]->SetColor( markerColor[mrk] );
					 }

					 for ( int unit = 0; unit < DEX_CODA_UNITS; unit++ ) {
						 EraseWindow( boresightWindow[unit] );
						 EraseWindow( tabletWindow[unit] );
						 EraseWindow( manipulandumWindow[unit] );
					 }


					 stabilityMeter[0, leftTabletMarker] = leftStabilityMeter0;
					 stabilityMeter[0, rightTabletMarker] = rightStabilityMeter0;
					 stabilityMeter[0, upperMastMarker] = upperStabilityMeter0;
					 stabilityMeter[0, lowerMastMarker] = lowerStabilityMeter0;
					 stabilityMeter[1, leftTabletMarker] = leftStabilityMeter1;
					 stabilityMeter[1, rightTabletMarker] = rightStabilityMeter1;
					 stabilityMeter[1, upperMastMarker] = upperStabilityMeter1;
					 stabilityMeter[1, lowerMastMarker] = lowerStabilityMeter1;

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

		};

	}

