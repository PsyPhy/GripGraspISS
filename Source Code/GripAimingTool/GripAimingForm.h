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
				this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
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
				this->boresightGroup0->SuspendLayout();
				this->groupBox4->SuspendLayout();
				this->groupBox6->SuspendLayout();
				this->groupBox3->SuspendLayout();
				this->groupBox1->SuspendLayout();
				this->SuspendLayout();
				// 
				// instructions
				// 
				this->instructions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->instructions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->instructions->Location = System::Drawing::Point(12, 12);
				this->instructions->Multiline = true;
				this->instructions->Name = L"instructions";
				this->instructions->ReadOnly = true;
				this->instructions->Size = System::Drawing::Size(1508, 143);
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
				this->boresightGroup0->Location = System::Drawing::Point(107, 170);
				this->boresightGroup0->Name = L"boresightGroup0";
				this->boresightGroup0->Size = System::Drawing::Size(1153, 924);
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
				this->boresightPanel0->Size = System::Drawing::Size(1133, 886);
				this->boresightPanel0->TabIndex = 0;
				// 
				// groupBox4
				// 
				this->groupBox4->Controls->Add(this->groupBox6);
				this->groupBox4->Controls->Add(this->tabletPanel0);
				this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox4->Location = System::Drawing::Point(1266, 170);
				this->groupBox4->Name = L"groupBox4";
				this->groupBox4->Size = System::Drawing::Size(254, 357);
				this->groupBox4->TabIndex = 8;
				this->groupBox4->TabStop = false;
				this->groupBox4->Text = L"Reference Markers";
				// 
				// groupBox6
				// 
				this->groupBox6->Controls->Add(this->leftStabilityMeter0);
				this->groupBox6->Controls->Add(this->rightStabilityMeter0);
				this->groupBox6->Controls->Add(this->lowerStabilityMeter0);
				this->groupBox6->Controls->Add(this->upperStabilityMeter0);
				this->groupBox6->Location = System::Drawing::Point(8, 225);
				this->groupBox6->Name = L"groupBox6";
				this->groupBox6->Size = System::Drawing::Size(236, 116);
				this->groupBox6->TabIndex = 1;
				this->groupBox6->TabStop = false;
				this->groupBox6->Text = L"Stability";
				// 
				// leftStabilityMeter0
				// 
				this->leftStabilityMeter0->BackColor = System::Drawing::SystemColors::ControlDark;
				this->leftStabilityMeter0->ForeColor = System::Drawing::Color::Blue;
				this->leftStabilityMeter0->Location = System::Drawing::Point(134, 65);
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
				this->rightStabilityMeter0->Location = System::Drawing::Point(134, 27);
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
				this->tabletPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->tabletPanel0->Location = System::Drawing::Point(8, 25);
				this->tabletPanel0->Name = L"tabletPanel0";
				this->tabletPanel0->Size = System::Drawing::Size(230, 186);
				this->tabletPanel0->TabIndex = 0;
				// 
				// groupBox3
				// 
				this->groupBox3->Controls->Add(this->distancePanel0);
				this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox3->ForeColor = System::Drawing::SystemColors::ActiveCaption;
				this->groupBox3->Location = System::Drawing::Point(12, 170);
				this->groupBox3->Name = L"groupBox3";
				this->groupBox3->Size = System::Drawing::Size(89, 924);
				this->groupBox3->TabIndex = 7;
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
				this->distancePanel0->Size = System::Drawing::Size(69, 885);
				this->distancePanel0->TabIndex = 0;
				// 
				// groupBox1
				// 
				this->groupBox1->Controls->Add(this->cameraButton1);
				this->groupBox1->Controls->Add(this->cameraButton0);
				this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox1->Location = System::Drawing::Point(1270, 549);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Size = System::Drawing::Size(239, 353);
				this->groupBox1->TabIndex = 9;
				this->groupBox1->TabStop = false;
				this->groupBox1->Text = L"Tracking Camera";
				// 
				// cameraButton1
				// 
				this->cameraButton1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 72, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->cameraButton1->ForeColor = System::Drawing::SystemColors::ControlText;
				this->cameraButton1->Location = System::Drawing::Point(21, 199);
				this->cameraButton1->Name = L"cameraButton1";
				this->cameraButton1->Size = System::Drawing::Size(196, 148);
				this->cameraButton1->TabIndex = 12;
				this->cameraButton1->Text = L"2";
				this->cameraButton1->UseVisualStyleBackColor = true;
				this->cameraButton1->Click += gcnew System::EventHandler(this, &GripAimingForm::cameraButton1_Click);
				// 
				// cameraButton0
				// 
				this->cameraButton0->BackColor = System::Drawing::SystemColors::Highlight;
				this->cameraButton0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 72, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->cameraButton0->ForeColor = System::Drawing::SystemColors::ControlText;
				this->cameraButton0->Location = System::Drawing::Point(21, 45);
				this->cameraButton0->Name = L"cameraButton0";
				this->cameraButton0->Size = System::Drawing::Size(196, 148);
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
				this->exitButton->Location = System::Drawing::Point(1320, 949);
				this->exitButton->Name = L"exitButton";
				this->exitButton->Size = System::Drawing::Size(146, 89);
				this->exitButton->TabIndex = 10;
				this->exitButton->Text = L"Exit";
				this->exitButton->UseVisualStyleBackColor = true;
				this->exitButton->Click += gcnew System::EventHandler(this, &GripAimingForm::exitButton_Click);
				// 
				// GripAimingForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->CancelButton = this->exitButton;
				this->ClientSize = System::Drawing::Size(1532, 1106);
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
				this->groupBox6->ResumeLayout(false);
				this->groupBox3->ResumeLayout(false);
				this->groupBox1->ResumeLayout(false);
				this->ResumeLayout(false);
				this->PerformLayout();

			}
#pragma endregion


			OpenGLWindow	*boresightWindow;
			Viewpoint		*boresightViewpoint;

			OpenGLWindow	*tabletWindow;
			Viewpoint		*tabletViewpoint;

			OpenGLWindow	*distanceWindow;
			OrthoViewpoint	*distanceViewpoint;
			Patch			*lowerExclusionZone, *upperExclusionZone;
			Assembly		*distanceBars;
			Sphere			**objectMarker;


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
					 DrawDistance( distanceWindow );
					 DrawBoresight( boresightWindow );
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

					 intializing->Visible = true;
					 instructions->SelectionStart = 5000;
					 instructions->SelectionLength = 0;
					 Refresh();
					 tracker->Initialize( "CodaRTnet.ini" );
					 intializing->Visible = false;

					 boresightWindow = CreateOpenGLWindowInForm( boresightPanel0 );
					 boresightViewpoint =  new Viewpoint( 6.0, 70.0, 1000.0, 6000.0);
					 // When then CODA is in it's 0 orientation, it points along
					 // the positive Y axis. This attitude reflects that in the 
					 // representation of the CODA bar.
					 boresightViewpoint->SetAttitude( 0.0, - 90.0, 0.0 );
					 // The origin of the CODA intrinsic coordinate system is the 
					 // center of the A camera.
					 boresightViewpoint->SetOffset( - 400.0, 0.0, 0.0 );

					 tabletWindow = CreateOpenGLWindowInForm( tabletPanel0, boresightWindow->hRC );

					 // Create objects for the distance display.
					 distanceWindow = CreateOpenGLWindowInForm( distancePanel0, boresightWindow->hRC );
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
						 objectMarker[mrk] = new Sphere( tabletMarkerRadius );
					 }

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


					 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
						 objectMarker[mrk]->SetColor( markerColor[mrk] );
						 markerSpheres->component[mrk]->SetColor( markerColor[mrk] );
						 distanceBars->component[mrk]->SetColor( markerColor[mrk] );
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
};

	}

