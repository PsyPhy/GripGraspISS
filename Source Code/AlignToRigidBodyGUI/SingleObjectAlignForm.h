#pragma once

#include <Windows.h>

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"

#include "../DexServices/DexServices.h"

#include "AlignToRigidBodyGUI.h"

#include "../Useful/ini.h"

namespace AlignToRigidBodyGUI {

#define HAND_MARKER_COLOR { 0.75, 0.5, 0.5, 0.5 }
#define CHEST_MARKER_COLOR { 0.3f, 0.0f, 0.7f, 0.5f }
	static GLfloat  markerColor[][4] = {
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		CHEST_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		HAND_MARKER_COLOR,
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.5f, 1.0f, 1.0f },
		{ 0.5f, 0.0f, 0.7f, 1.0f } };

		using namespace System;
		using namespace System::ComponentModel;
		using namespace System::Collections;
		using namespace System::Windows::Forms;
		using namespace System::Data;
		using namespace System::Drawing;
		using namespace PsyPhy;
		using namespace Grasp;

		/// <summary>
		/// A windows Form that guides an operator through the alignment process.
		/// </summary>
		public ref class SingleObjectForm : public System::Windows::Forms::Form
		{

		private:

			DexServices		*dex;

		protected:

			bool									forceShow;

			PsyPhy::OpenGLWindow					*visibilityWindow1, *visibilityWindow2;
			PsyPhy::OpenGLWindow					*fovWindow1, *fovWindow2;
			PsyPhy::OpenGLWindow					*distanceWindow1, *distanceWindow2;
			PsyPhy::Viewpoint						*codaViewpoint1, *codaViewpoint2, *objectViewpoint;
			PsyPhy::OrthoViewpoint					*distanceViewpoint;
			Grasp::MarkerStructureGLObject			*alignmentObject1, *alignmentObject2;
			Grasp::MarkerStructureGLObject			*visibilityObject1, *visibilityObject2;
			PsyPhy::Assembly						*fovSweetSpot;
			Patch									*lowerExclusionZone, *upperExclusionZone;

			PsyPhy::CodaRTnetTracker				*coda;
			Grasp::GraspGLObjects					*objects;
			Assembly								*distanceBars1, *distanceBars2;

			String^ modelFile;
			String^ filenameRoot;

			static double	minDisplayDistance = 1000.0;
			static double	maxDisplayDistance = 5000.0;
			static double	minAcceptableDistance = 1800.0;
			static double	maxAcceptableDistance = 4000.0;

			double maxPositionError;
			double maxOrientationError;

		private: System::Windows::Forms::GroupBox^  groupBox2;
		private: System::Windows::Forms::Panel^  fovPanel1;
		private: System::Windows::Forms::GroupBox^  groupBox4;
		private: System::Windows::Forms::GroupBox^  groupBox3;
		private: System::Windows::Forms::Panel^  visibilityPanel1;
		private: System::Windows::Forms::Panel^  distancePanel1;
		private: System::Windows::Forms::GroupBox^  groupBox1;
		private: System::Windows::Forms::GroupBox^  groupBox6;
		private: System::Windows::Forms::Panel^  distancePanel2;
		private: System::Windows::Forms::GroupBox^  groupBox5;
		private: System::Windows::Forms::Panel^  fovPanel2;
		private: System::Windows::Forms::GroupBox^  groupBox7;
		private: System::Windows::Forms::Panel^  visibilityPanel2;
		private: System::Windows::Forms::GroupBox^  groupBox8;
		private: System::Windows::Forms::TextBox^  instructionsTextBox;

		private: System::Windows::Forms::GroupBox^	vrGroupBox1;
		private: System::Windows::Forms::Button^	alignButton;
		private: System::Windows::Forms::Button^	cancelButton;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label4;
		private: System::Windows::Forms::Label^  busy;


		private: System::Windows::Forms::Button^  resetButton;


		public:
			SingleObjectForm( String ^model_file, String ^filename_root, Grasp::DexServices *dex, PsyPhy::CodaRTnetTracker *coda ) :
			  maxPositionError( 10.0 ),
				  maxOrientationError( 2.0 ),
				  forceShow( false )
			  {
				  InitializeComponent();
				  modelFile = model_file;
				  filenameRoot = filename_root;
				  this->dex = dex;
				  this->coda = coda;
			  }

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~SingleObjectForm()
			{
				if (components)
				{
					delete components;
				}
			}

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
				this->alignButton = (gcnew System::Windows::Forms::Button());
				this->cancelButton = (gcnew System::Windows::Forms::Button());
				this->vrGroupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
				this->fovPanel1 = (gcnew System::Windows::Forms::Panel());
				this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
				this->distancePanel1 = (gcnew System::Windows::Forms::Panel());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
				this->visibilityPanel1 = (gcnew System::Windows::Forms::Panel());
				this->resetButton = (gcnew System::Windows::Forms::Button());
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
				this->distancePanel2 = (gcnew System::Windows::Forms::Panel());
				this->label3 = (gcnew System::Windows::Forms::Label());
				this->label4 = (gcnew System::Windows::Forms::Label());
				this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
				this->fovPanel2 = (gcnew System::Windows::Forms::Panel());
				this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
				this->visibilityPanel2 = (gcnew System::Windows::Forms::Panel());
				this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
				this->instructionsTextBox = (gcnew System::Windows::Forms::TextBox());
				this->busy = (gcnew System::Windows::Forms::Label());
				this->vrGroupBox1->SuspendLayout();
				this->groupBox2->SuspendLayout();
				this->groupBox4->SuspendLayout();
				this->groupBox3->SuspendLayout();
				this->groupBox1->SuspendLayout();
				this->groupBox6->SuspendLayout();
				this->groupBox5->SuspendLayout();
				this->groupBox7->SuspendLayout();
				this->groupBox8->SuspendLayout();
				this->SuspendLayout();
				// 
				// alignButton
				// 
				this->alignButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->alignButton->Location = System::Drawing::Point(1337, 755);
				this->alignButton->Margin = System::Windows::Forms::Padding(2);
				this->alignButton->Name = L"alignButton";
				this->alignButton->Size = System::Drawing::Size(80, 40);
				this->alignButton->TabIndex = 1;
				this->alignButton->Text = L"OK";
				this->alignButton->UseVisualStyleBackColor = true;
				this->alignButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::alignButton_Click);
				// 
				// cancelButton
				// 
				this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->cancelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->cancelButton->Location = System::Drawing::Point(1248, 755);
				this->cancelButton->Margin = System::Windows::Forms::Padding(2);
				this->cancelButton->Name = L"cancelButton";
				this->cancelButton->Size = System::Drawing::Size(80, 40);
				this->cancelButton->TabIndex = 3;
				this->cancelButton->Text = L"Cancel";
				this->cancelButton->UseVisualStyleBackColor = true;
				this->cancelButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::cancelButton_Click);
				// 
				// vrGroupBox1
				// 
				this->vrGroupBox1->Controls->Add(this->groupBox2);
				this->vrGroupBox1->Controls->Add(this->groupBox4);
				this->vrGroupBox1->Controls->Add(this->groupBox3);
				this->vrGroupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->vrGroupBox1->Location = System::Drawing::Point(11, 8);
				this->vrGroupBox1->Margin = System::Windows::Forms::Padding(2);
				this->vrGroupBox1->Name = L"vrGroupBox1";
				this->vrGroupBox1->Padding = System::Windows::Forms::Padding(2);
				this->vrGroupBox1->Size = System::Drawing::Size(565, 788);
				this->vrGroupBox1->TabIndex = 4;
				this->vrGroupBox1->TabStop = false;
				this->vrGroupBox1->Text = L"Tracker Camera 1";
				// 
				// groupBox2
				// 
				this->groupBox2->Controls->Add(this->fovPanel1);
				this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox2->Location = System::Drawing::Point(104, 291);
				this->groupBox2->Name = L"groupBox2";
				this->groupBox2->Size = System::Drawing::Size(454, 488);
				this->groupBox2->TabIndex = 4;
				this->groupBox2->TabStop = false;
				this->groupBox2->Text = L"Boresight";
				// 
				// fovPanel1
				// 
				this->fovPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->fovPanel1->Location = System::Drawing::Point(5, 24);
				this->fovPanel1->Margin = System::Windows::Forms::Padding(2);
				this->fovPanel1->Name = L"fovPanel1";
				this->fovPanel1->Size = System::Drawing::Size(444, 459);
				this->fovPanel1->TabIndex = 4;
				// 
				// groupBox4
				// 
				this->groupBox4->Controls->Add(this->distancePanel1);
				this->groupBox4->Controls->Add(this->label2);
				this->groupBox4->Controls->Add(this->label1);
				this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox4->Location = System::Drawing::Point(5, 30);
				this->groupBox4->Name = L"groupBox4";
				this->groupBox4->Size = System::Drawing::Size(93, 749);
				this->groupBox4->TabIndex = 5;
				this->groupBox4->TabStop = false;
				this->groupBox4->Text = L"Distance";
				// 
				// distancePanel1
				// 
				this->distancePanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->distancePanel1->Location = System::Drawing::Point(6, 42);
				this->distancePanel1->Name = L"distancePanel1";
				this->distancePanel1->Size = System::Drawing::Size(81, 677);
				this->distancePanel1->TabIndex = 8;
				// 
				// label2
				// 
				this->label2->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label2->Location = System::Drawing::Point(8, 717);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(77, 21);
				this->label2->TabIndex = 10;
				this->label2->Text = L"near";
				this->label2->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
				this->label2->Click += gcnew System::EventHandler(this, &SingleObjectForm::label2_Click);
				// 
				// label1
				// 
				this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label1->Location = System::Drawing::Point(10, 18);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(77, 21);
				this->label1->TabIndex = 9;
				this->label1->Text = L"far";
				this->label1->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
				// 
				// groupBox3
				// 
				this->groupBox3->Controls->Add(this->visibilityPanel1);
				this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox3->Location = System::Drawing::Point(104, 30);
				this->groupBox3->Name = L"groupBox3";
				this->groupBox3->Size = System::Drawing::Size(454, 255);
				this->groupBox3->TabIndex = 0;
				this->groupBox3->TabStop = false;
				this->groupBox3->Text = L"Visibility";
				// 
				// visibilityPanel1
				// 
				this->visibilityPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->visibilityPanel1->Location = System::Drawing::Point(5, 23);
				this->visibilityPanel1->Margin = System::Windows::Forms::Padding(2);
				this->visibilityPanel1->Name = L"visibilityPanel1";
				this->visibilityPanel1->Size = System::Drawing::Size(444, 227);
				this->visibilityPanel1->TabIndex = 1;
				// 
				// resetButton
				// 
				this->resetButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->resetButton->Location = System::Drawing::Point(1159, 755);
				this->resetButton->Margin = System::Windows::Forms::Padding(2);
				this->resetButton->Name = L"resetButton";
				this->resetButton->Size = System::Drawing::Size(80, 40);
				this->resetButton->TabIndex = 17;
				this->resetButton->Text = L"Reset";
				this->resetButton->UseVisualStyleBackColor = true;
				this->resetButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::button1_Click);
				// 
				// groupBox1
				// 
				this->groupBox1->Controls->Add(this->groupBox6);
				this->groupBox1->Controls->Add(this->groupBox5);
				this->groupBox1->Controls->Add(this->groupBox7);
				this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox1->Location = System::Drawing::Point(586, 8);
				this->groupBox1->Margin = System::Windows::Forms::Padding(2);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Padding = System::Windows::Forms::Padding(2);
				this->groupBox1->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
				this->groupBox1->Size = System::Drawing::Size(565, 788);
				this->groupBox1->TabIndex = 6;
				this->groupBox1->TabStop = false;
				this->groupBox1->Text = L"Tracker Camera 2";
				// 
				// groupBox6
				// 
				this->groupBox6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->groupBox6->Controls->Add(this->distancePanel2);
				this->groupBox6->Controls->Add(this->label3);
				this->groupBox6->Controls->Add(this->label4);
				this->groupBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox6->Location = System::Drawing::Point(465, 30);
				this->groupBox6->Name = L"groupBox6";
				this->groupBox6->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox6->Size = System::Drawing::Size(93, 749);
				this->groupBox6->TabIndex = 6;
				this->groupBox6->TabStop = false;
				this->groupBox6->Text = L"Distance";
				// 
				// distancePanel2
				// 
				this->distancePanel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->distancePanel2->Location = System::Drawing::Point(6, 42);
				this->distancePanel2->Name = L"distancePanel2";
				this->distancePanel2->Size = System::Drawing::Size(81, 677);
				this->distancePanel2->TabIndex = 8;
				// 
				// label3
				// 
				this->label3->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label3->Location = System::Drawing::Point(8, 720);
				this->label3->Name = L"label3";
				this->label3->Size = System::Drawing::Size(77, 21);
				this->label3->TabIndex = 12;
				this->label3->Text = L"near";
				this->label3->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
				// 
				// label4
				// 
				this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->label4->Location = System::Drawing::Point(8, 20);
				this->label4->Name = L"label4";
				this->label4->Size = System::Drawing::Size(77, 21);
				this->label4->TabIndex = 11;
				this->label4->Text = L"far";
				this->label4->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
				// 
				// groupBox5
				// 
				this->groupBox5->Controls->Add(this->fovPanel2);
				this->groupBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox5->Location = System::Drawing::Point(10, 291);
				this->groupBox5->Name = L"groupBox5";
				this->groupBox5->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox5->Size = System::Drawing::Size(454, 488);
				this->groupBox5->TabIndex = 4;
				this->groupBox5->TabStop = false;
				this->groupBox5->Text = L"Boresight";
				// 
				// fovPanel2
				// 
				this->fovPanel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->fovPanel2->Location = System::Drawing::Point(5, 24);
				this->fovPanel2->Margin = System::Windows::Forms::Padding(2);
				this->fovPanel2->Name = L"fovPanel2";
				this->fovPanel2->Size = System::Drawing::Size(444, 459);
				this->fovPanel2->TabIndex = 4;
				// 
				// groupBox7
				// 
				this->groupBox7->Controls->Add(this->visibilityPanel2);
				this->groupBox7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox7->Location = System::Drawing::Point(5, 30);
				this->groupBox7->Name = L"groupBox7";
				this->groupBox7->RightToLeft = System::Windows::Forms::RightToLeft::No;
				this->groupBox7->Size = System::Drawing::Size(454, 255);
				this->groupBox7->TabIndex = 0;
				this->groupBox7->TabStop = false;
				this->groupBox7->Text = L"Visibility";
				// 
				// visibilityPanel2
				// 
				this->visibilityPanel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->visibilityPanel2->Location = System::Drawing::Point(5, 23);
				this->visibilityPanel2->Margin = System::Windows::Forms::Padding(2);
				this->visibilityPanel2->Name = L"visibilityPanel2";
				this->visibilityPanel2->Size = System::Drawing::Size(444, 227);
				this->visibilityPanel2->TabIndex = 1;
				// 
				// groupBox8
				// 
				this->groupBox8->Controls->Add(this->instructionsTextBox);
				this->groupBox8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->groupBox8->Location = System::Drawing::Point(1156, 8);
				this->groupBox8->Name = L"groupBox8";
				this->groupBox8->Size = System::Drawing::Size(264, 742);
				this->groupBox8->TabIndex = 18;
				this->groupBox8->TabStop = false;
				this->groupBox8->Text = L"Instructions";
				// 
				// instructionsTextBox
				// 
				this->instructionsTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
					| System::Windows::Forms::AnchorStyles::Left) 
					| System::Windows::Forms::AnchorStyles::Right));
				this->instructionsTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
					System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				this->instructionsTextBox->Location = System::Drawing::Point(6, 30);
				this->instructionsTextBox->Multiline = true;
				this->instructionsTextBox->Name = L"instructionsTextBox";
				this->instructionsTextBox->ReadOnly = true;
				this->instructionsTextBox->Size = System::Drawing::Size(252, 706);
				this->instructionsTextBox->TabIndex = 16;
				// 
				// busy
				// 
				this->busy->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				this->busy->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 48, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->busy->ForeColor = System::Drawing::Color::Maroon;
				this->busy->Location = System::Drawing::Point(256, 271);
				this->busy->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
				this->busy->Name = L"busy";
				this->busy->Size = System::Drawing::Size(912, 265);
				this->busy->TabIndex = 24;
				this->busy->Text = L"Alignment in progress.\r\nPlease wait ...";
				this->busy->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				// 
				// SingleObjectForm
				// 
				this->AcceptButton = this->alignButton;
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
				this->CancelButton = this->cancelButton;
				this->ClientSize = System::Drawing::Size(1425, 807);
				this->Controls->Add(this->busy);
				this->Controls->Add(this->groupBox8);
				this->Controls->Add(this->groupBox1);
				this->Controls->Add(this->resetButton);
				this->Controls->Add(this->vrGroupBox1);
				this->Controls->Add(this->cancelButton);
				this->Controls->Add(this->alignButton);
				this->Location = System::Drawing::Point(20, 40);
				this->Margin = System::Windows::Forms::Padding(2);
				this->Name = L"SingleObjectForm";
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
				this->Text = L"CODA Tracker Alignment";
				this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SingleObjectForm::Form1_FormClosing);
				this->Shown += gcnew System::EventHandler(this, &SingleObjectForm::Form1_Shown);
				this->vrGroupBox1->ResumeLayout(false);
				this->groupBox2->ResumeLayout(false);
				this->groupBox4->ResumeLayout(false);
				this->groupBox3->ResumeLayout(false);
				this->groupBox1->ResumeLayout(false);
				this->groupBox6->ResumeLayout(false);
				this->groupBox5->ResumeLayout(false);
				this->groupBox7->ResumeLayout(false);
				this->groupBox8->ResumeLayout(false);
				this->groupBox8->PerformLayout();
				this->ResumeLayout(false);

			}
#pragma endregion

		private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

					 fOutputDebugString( "Tracker Alignment (%p): Parsing %s.\n", this, "CodaAlignment.ini" );
					 FILE *fp = fopen( "CodaAlignment.ini", "r" );
					 if ( !fp ) fAbortMessage( "GRASP Tracker Alignment", "Unable to open configuration file CodaAlignment.ini" );
					 fscanf( fp, "%lf %lf %lf %lf", &minDisplayDistance, &minAcceptableDistance, &maxAcceptableDistance, &maxDisplayDistance );
					 fclose( fp );
					 fOutputDebugString( "Limits: %f  %f  %f  %f\n",  minDisplayDistance, minAcceptableDistance, maxAcceptableDistance, maxDisplayDistance );

					 // Create the OpenGLObjects that depict the marker array structure.
					 objects = new Grasp::GraspGLObjects( "Grasp.ini" );
					 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
					 alignmentObject1 = objects->CreateChestMarkerStructure( model_file );
					 Sphere *sphere1 = new Sphere( 140.0 );
					 sphere1->SetColor( GREEN );	
					 sphere1->SetOffset( 0.0, 300.0, 0.0 );
					 alignmentObject1->AddComponent( sphere1 );
					 for ( int i = 0; i < alignmentObject1->components - 1; i++ ) {
						 alignmentObject1->component[i]->Disable();
					 }

					 alignmentObject2 = objects->CreateChestMarkerStructure( model_file );
					 Sphere *sphere2 = new Sphere( 140.0 );
					 sphere2->SetColor( GREEN );	
					 sphere2->SetOffset( 0.0, 300.0, 0.0 );
					 alignmentObject2->AddComponent( sphere2 );
					 for ( int i = 0; i < alignmentObject2->components - 1; i++ ) {
						 alignmentObject2->component[i]->Disable();
					 }

					 visibilityObject1 = objects->CreateChestMarkerStructure( model_file );
					 visibilityObject2 = objects->CreateChestMarkerStructure( model_file );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

					 // Create windows and viewpoints to show which markers are visible.
					 visibilityWindow1 = PsyPhy::CreateOpenGLWindowInForm( visibilityPanel1);
					 visibilityWindow2 = PsyPhy::CreateOpenGLWindowInForm( visibilityPanel2, visibilityWindow1->hRC );

					 // Create a viewpoint that looks at the origin from the negative Z axis.
					 // This is the cannonical viewpoint for an object at zero position and orientation.
					 objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
					 objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
					 objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

					 // Create windows and viewpoints to show the distance of the markers from the tracker unit.
					 distanceWindow1 = PsyPhy::CreateOpenGLWindowInForm( distancePanel1);
					 distanceWindow2 = PsyPhy::CreateOpenGLWindowInForm( distancePanel2, distanceWindow1->hRC );

					 // Create a viewpoint that looks at the origin from the negative Z axis.
					 // This is the cannonical viewpoint for an object at zero position and orientation.
					 distanceViewpoint = new OrthoViewpoint( -1.0, alignmentObject1->nModelMarkers, minDisplayDistance, maxDisplayDistance );

					 // Create windows and viewpoints to show what the CODA units are seeing.
					 fovWindow1 = PsyPhy::CreateOpenGLWindowInForm( fovPanel1 );
					 codaViewpoint1 = new Viewpoint( 6.0, 60.0, 10.0, 10000.0);
					 codaViewpoint1->SetOrientation( 90.0, - 90.0, 0.0 );

					 fovWindow2 = PsyPhy::CreateOpenGLWindowInForm( fovPanel2, fovWindow1->hRC );
					 codaViewpoint2 = new Viewpoint( 6.0, 60.0, 10.0, 10000.0);
					 codaViewpoint2->SetOrientation( 0.0, - 90.0, 0.0 );

					 // Create a targeting scope.
					 fovSweetSpot = new Assembly();
					 Hole *hole = new Hole( 500.0,  2000.0, 2000.0 );
					 hole->SetColor( 0.15, 0.0, 0.0, 0.5 );	
					 hole->SetPosition( 0.0, 0.0, -100.0 );
					 fovSweetSpot->AddComponent( hole );

					 hole = new Hole( 100.0,  2000.0, 2000.0 );
					 hole->SetColor( 0.0, 0.0, 0.5, 0.25 );	
					 fovSweetSpot->AddComponent( hole );

					 Bar *bar = new Bar( 2000.0, 5.0, 5.0 );
					 bar->SetPosition( 0.0, -1000.0, 0.0 );
					 bar->SetOrientation( 0.0, 90.0, 0.0 );
					 bar->SetColor( GRAY );
					 fovSweetSpot->AddComponent( bar );
					 bar = new Bar( 2000.0, 5.0, 5.0 );
					 bar->SetPosition( 1000.0, 0.0, 0.0 );
					 bar->SetOrientation( 0.0, 0.0, 90.0 );
					 bar->SetColor( GRAY );
					 fovSweetSpot->AddComponent( bar );

					 fovSweetSpot->SetOrientation( 0.0, 90.0, 0.0 );
					 fovSweetSpot->SetPosition( 0.0, 1000.0, 0.0 );

					 Sphere *sphere = new Sphere( 100.0 );
					 sphere->SetColor( GREEN );	
					 sphere->SetOffset( 0.0, 300.0, 0.0 );
					 alignmentObject1->AddComponent( sphere );
					 alignmentObject2->AddComponent( sphere );

					 // These bars indicate the distance of the marker in depth 
					 // from the position of the CODA unit.
					 distanceBars1 = new Assembly();
					 for ( int i = 0; i < alignmentObject1->nModelMarkers; i++ ) {
						 int mrk = alignmentObject1->modelMarker[i].id;
						 Slab *bar = new Slab( 0.8, 30.0, 20.0 );
						 bar->SetPosition( mrk, minDisplayDistance + ( i + 2)  * 100.0, 5.0 );
						 bar->SetAttitude( 0.0, 0.0, 0.0 );
						 bar->SetColor( BLUE );
						 distanceBars1->AddComponent( bar );
					 }
					 distanceBars2 = new Assembly();
					 for ( int i = 0; i < alignmentObject1->nModelMarkers; i++ ) {
						 int mrk = alignmentObject2->modelMarker[i].id;
						 Slab *bar = new Slab( 0.8, 30.0, 20.0 );
						 bar->SetPosition( mrk, minDisplayDistance + ( i + 2)  * 100.0, 5.0 );
						 bar->SetAttitude( 0.0, 0.0, 0.0 );
						 bar->SetColor( BLUE );
						 distanceBars2->AddComponent( bar );
					 }

					 lowerExclusionZone = new Patch( alignmentObject1->nModelMarkers + 2, minAcceptableDistance - minDisplayDistance );
					 lowerExclusionZone->SetOffset( ( alignmentObject1->nModelMarkers + 2 ) / 2, ( minAcceptableDistance - minDisplayDistance ) / 2.0, 0.0 );
					 lowerExclusionZone->SetPosition( -1.0, minDisplayDistance, - 10.0 );
					 lowerExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );
					 upperExclusionZone = new Patch( alignmentObject1->nModelMarkers + 2, maxDisplayDistance - maxAcceptableDistance );
					 upperExclusionZone->SetOffset( ( alignmentObject1->nModelMarkers + 2 ) / 2, - ( maxDisplayDistance - maxAcceptableDistance ) / 2.0, 0.0 );
					 upperExclusionZone->SetPosition( -1.0, maxDisplayDistance, - 10.0 );
					 upperExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.5 );

					 // Initialize the state of the GL graphics engine.
					 // Some of this will be overridden by the object renderer.
					 glUsefulInitializeDefault();
					 glUsefulDefaultSpecularLighting( 0.75 );

					 // Show the buttons as being inactive.
					 resetButton->Enabled = false;
					 cancelButton->Enabled = false;
					 alignButton->Enabled = false;
					 // Send a message to ground to show our progress.
					 dex->SendSubstep( STARTUP_VISIBILITY );

					 // Show a message while the tracker is initializing.
					 busy->Text = L"Tracker Initializing\r\nPlease wait ...";
					 busy->Visible = true;
					 // Make sure that the windows have refreshed  before starting up the CODA, 
					 // because it will take some time.
					 Refresh();
					 Application::DoEvents();
					 // Wait to make sure that the tracker is up and running so that we can kill it.
					 Sleep( 5000 );

					 coda->Initialize( "CodaRTnet.ini" );

					 // Send a message to ground as part of housekeeping to show our progress.
					 dex->SendSubstep( VISIBILITY );
					 // Send more detailed information as part of a RT science packet.
					 AlignClientBuffer clientBuffer;
					 Vector3	offsets[MAX_UNITS];
					 Matrix3x3	rotations[MAX_UNITS];
					 strncpy( clientBuffer.ID, "ALIGN", sizeof( clientBuffer.ID ) );
					 clientBuffer.prePost = PRE;
					 coda->GetAlignmentTransforms( offsets, rotations );
					 for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
						 coda->CopyVector( clientBuffer.offsets[unit], offsets[unit] );
						 coda->CopyMatrix( clientBuffer.rotations[unit], rotations[unit] );
					 }
					 dex->AddClientSlice( (unsigned char *) &clientBuffer, sizeof( clientBuffer ) );

					 // Hide the tracker message.
					 busy->Visible = false;
					 // Re-enable the Form as being inactive.
					 resetButton->Enabled = true;
					 cancelButton->Enabled = true;
					 alignButton->Enabled = true;				 

					 // Prompt for the required action.
					 instructionsTextBox->Text =  "- Distance\r\n\r\nAdjust Tracker Camera distance so that all markers are in the green zone.\r\n\r\n";
					 instructionsTextBox->Text += "- Visibility\r\n\r\nCheck that all Chest Marker Support markers are visible to each Tracker Camera (all dots green).\r\n\r\n";
					 instructionsTextBox->Text += "- Boresight\r\n\r\nAdjust the orientation of the Tracker Cameras until the green ball is centered in the crosshairs for each Tracker Camera.\r\n\r\nIf no green ball appears, contact COL-CC (if possible) or perform Reset.\r\n\r\n";
					 instructionsTextBox->Text += "When all conditions are met, press 'OK'.";

					 // Start a refresh time that will update the visibility of the LEDs in the GUI display.
					 CreateRefreshTimer( 300 );
					 StartRefreshTimer();

				 }

		private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { 
					 if ( Environment::ExitCode == -2 ) {
						 dex->SendSubstep( ALIGNMENT_ERROR );
						 dex->SendTrackerStatus( TRACKER_ANOMALY ); 
					 }
					 coda->Quit();
				 }

		private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
					 // Send a message to ground to show our progress.
					 dex->SendSubstep( ABORT_REQUESTED );
					 System::Windows::Forms::DialogResult response;
					 response = MessageBox::Show( "Are you sure you want to exit without performing the alignment?", "AlignToRigidBodyGUI", MessageBoxButtons::YesNo );
					 if ( response == System::Windows::Forms::DialogResult::Yes ) {
						 Environment::ExitCode = -3;
						 Close();
					 }
					 //Presumably, were were in the visibility check state and we are returning there.
					 dex->SendSubstep( VISIBILITY );
				 }


		private: System::Void alignButton_Click(System::Object^  sender, System::EventArgs^  e) {

					 // Send a message to ground to show our progress.
					 dex->SendSubstep( SHUTDOWN_VISIBILITY );
					 // Take a picture of the setup as seen from the CODA.
					 dex->SnapPicture( "ALIGNMENT" );

					 StopRefreshTimer();

					 // Remove instruction.
					 instructionsTextBox->Text = "";
					 Refresh();
					 Application::DoEvents();
					 // Show a message while we are busy acquiring and computing the new alignment.
					 busy->Text = L"Alignment in Progress\r\nPlease wait ...";
					 busy->Visible = true;

					 // Show the buttons on the form as being inactive.
					 cancelButton->Enabled = false;
					 alignButton->Enabled = false;
					 resetButton->Enabled = false;
					 Refresh();
					 Application::DoEvents();

					 // Acquire a short burst of marker data to be used to perform the alignment.
					 // Send a message to ground to show our progress.
					 busy->Text = "Acquiring alignment data ...";
					 Sleep( 10 );
					 Refresh();
					 Application::DoEvents();

					 // Send a message to ground to show our progress.
					 dex->SendSubstep( ACQUIRE_PREALIGN );

					 fprintf( stderr, "Starting INTRINSIC acquisition ... " );
					 coda->StartAcquisition( 2.0 );
					 fprintf( stderr, "OK.\nAcquiring " );
					 // Just wait for the acquisition to finish.
					 while ( coda->GetAcquisitionState() ) {
						 Refresh();
						 Application::DoEvents();
						 Sleep( 20 );
					 }
					 coda->StopAcquisition();
					 fprintf( stderr, "OK.\n" );

					 // Log the data acquired in intrinsic coordinates.
					 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
					 String ^rawFilename = filenameRoot + ".prealign.mrk";
					 char *raw_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( rawFilename ).ToPointer();
					 fprintf( stderr, "Writing marker positions acquired prior to alignment ... " );
					 coda->WriteMarkerFile( raw_file );
					 fprintf( stderr, "OK.\n" );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( raw_file ) );

					 // Use a CodaPoseTracker to compute the pose of the marker structure in the intrinsic frame of the CODA unit.
					 // We will then invert the pose to compute the transformation required by each unit.
					 // Send a message to ground to show our progress.
					 dex->SendSubstep( COMPUTE_ALIGNMENT );
					 busy->Text = "Computing transforms ...";
					 Refresh();
					 Application::DoEvents();
					 MarkerFrame avgFrame;
					 MarkerFrame intrinsicFrame;
					 TrackerPose tracker_pose;
					 Pose poses[MAX_UNITS];
					 CodaPoseTracker *codaPoseTracker = new CodaPoseTracker( &intrinsicFrame );
					 // The name of the model file is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
					 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
					 codaPoseTracker->ReadModelMarkerPositions( model_file );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

					 for ( int unit = 0; unit < coda->nUnits; unit++ ) {
						 // Compute the average marker positions for the acquired sample.
						 coda->ComputeAverageMarkerFrame( avgFrame, coda->recordedMarkerFrames[unit], coda->nFrames );
						 coda->ComputeIntrinsicMarkerFrame( intrinsicFrame, unit, avgFrame );
						 // Compute the pose of the rigid body from the average acquired sample.
						 // This may seem strange that we are calling "GetCurrentPose()" when the CODA is not running, but
						 // codaPoseTracker assumes that the linked MarkerFrame has been filled and so it does not actually acquire anything.
						 // Instead, it computes the current pose from the average marker frame computed just above.
						 codaPoseTracker->SetMarkerFrameBuffer( &intrinsicFrame );
						 codaPoseTracker->GetCurrentPose( tracker_pose );
						 // We need to assemble the poses from each CODA unit into a single list to be passed to SetAlignmentFromPoses().
						 coda->CopyPose( poses[unit], tracker_pose.pose );
					 }
					 dex->SendSubstep( INSTALL_ALIGNMENT );
					 busy->Text = "Installing transforms ...";
					 Refresh();
					 Application::DoEvents();
					 // Log the data acquired in aligned coordinates.
					 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
					 String ^alignmentFilename = filenameRoot + ".alignment.dat";
					 char *alignment_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( alignmentFilename ).ToPointer();

					 // This does the real work.
					 coda->SetAlignmentFromPoses( poses, alignment_file );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( alignment_file ) );

					 // Because GraspTrackerDaemon is also the DexServices proxy, and because changing the alignment
					 // for CodaRTnetDaemonTracker causes GraspTrackerDaemon to restart, we need to re-establish the 
					 // connection to the DexServices proxy.
					 dex->Connect();
					 dex->ResendTaskInfo();

					 // Restart and acquire a short burst of marker data to be used to verify the alignment.
					 // Send a message to ground to show our progress.
					 dex->SendSubstep( ACQUIRE_ALIGNED );
					 busy->Text = "Acquiring to confirm ...";
					 Refresh();
					 Application::DoEvents();
					 // Send more details as part of an RT science packet.
					 AlignClientBuffer clientBuffer;
					 Vector3	offsets[MAX_UNITS];
					 Matrix3x3	rotations[MAX_UNITS];
					 strncpy( clientBuffer.ID, "ALIGN", sizeof( clientBuffer.ID ) );
					 clientBuffer.prePost = POST;
					 coda->GetAlignmentTransforms( offsets, rotations );
					 for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
						 coda->CopyVector( clientBuffer.offsets[unit], offsets[unit] );
						 coda->CopyMatrix( clientBuffer.rotations[unit], rotations[unit] );
					 }
					 dex->AddClientSlice( (unsigned char *) &clientBuffer, sizeof( clientBuffer ) );

					 fprintf( stderr, "Acquiring to confirm ..." );
					 coda->StartAcquisition( 5.0 );
					 fprintf( stderr, "OK.\nAcquiring " );
					 // Just wait for the acquisition to finish. 
					 while ( coda->GetAcquisitionState() ) {
						 fprintf( stderr, "." );
						 Application::DoEvents();
						 Sleep( 20 );
					 }
					 coda->StopAcquisition();
					 fprintf( stderr, "OK.\n\n" );

					 // Log the data acquired in aligned coordinates.
					 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
					 String ^alignedFilename = filenameRoot + ".postalign.mrk";
					 char *aligned_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( alignedFilename ).ToPointer();
					 fprintf( stderr, "Writing marker positions acquired in newly aligned coordinates ... " );
					 coda->WriteMarkerFile( aligned_file );
					 fprintf( stderr, "OK.\n" );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( aligned_file ) );

					 dex->SendSubstep( SHOW_RESULTS );

					 // Use a CodaPoseTracker to compute the pose of the marker structure in the newly aligned frame.
					 // We should get position zero and null orientation in each case.
					 char msg[25600] = "Alignment results: \n\n", line[256];
					 bool well_aligned = true;
					 for ( int unit = 0; unit < coda->nUnits; unit++ ) {
						 // Compute the average marker positions for the acquired sample.
						 coda->ComputeAverageMarkerFrame( avgFrame, coda->recordedMarkerFrames[unit], coda->nFrames );
						 // Set the position of the model markers from the average acquired sample.
						 codaPoseTracker->SetMarkerFrameBuffer( &avgFrame );
						 codaPoseTracker->GetCurrentPose( tracker_pose );
						 // Show the computed pose.
						 bool position_ok, orientation_ok;
						 if ( tracker_pose.visible ) {
							 fOutputDebugString( "New Pose: %d %s %s\n", unit, codaPoseTracker->vstr( tracker_pose.pose.position ), codaPoseTracker->qstr( tracker_pose.pose.orientation ) );
							 // Build a string with the poses, in case we need to display after an error.
							 double position_error = codaPoseTracker->VectorNorm( tracker_pose.pose.position );
							 position_ok = position_error < maxPositionError;
							 sprintf( line, "%d %s %6.3f %s\n", unit, codaPoseTracker->vstr( tracker_pose.pose.position ), position_error, ( position_ok ? "OK" : "!!" ));
							 strcat( msg, line );
							 double orientation_error = codaPoseTracker->AngleBetweenOrientations( tracker_pose.pose.orientation, codaPoseTracker->nullQuaternion );
							 orientation_ok = orientation_error < maxOrientationError;
							 sprintf( line, "%d %s %6.3f %s\n", unit,  codaPoseTracker->qstr( tracker_pose.pose.orientation ), orientation_error, ( orientation_ok ? "OK" : "!!" ));
							 strcat( msg, line );
						 }
						 else {
							 position_ok = false;
							 orientation_ok = false;
							 fOutputDebugString( "Tracker Camera %d Computed pose INVALID !!\n", unit  );
							 sprintf( line, "Tracker Camera #%d Computed pose INVALID !!\n", unit  );
							 strcat( msg, line );
						 }
						 // Check that the alignment has been successful.
						 well_aligned &= ( position_ok & orientation_ok );
					 }
					 strcat( msg, "\n\nAre the serial numbers set correctly?\nAre the FTP login and password correct?" );
					 if ( !well_aligned || forceShow ) fMessageBox( MB_OK | MB_ICONEXCLAMATION , "AlignToRigidBodyGUI", msg );
					 if ( !well_aligned ) Environment::ExitCode = -2;
					 else Environment::ExitCode = 0;
					 // Close the form. 
					 Close();

				 }
				 void Render ( void ) {

					 // Update the 3D view of the alignment object.

					 visibilityWindow2->Activate();
					 visibilityWindow2->Clear( 0.10, 0.10, 0.30 );
					 objectViewpoint->Apply( visibilityWindow2, CYCLOPS );
					 visibilityObject2->Draw();
					 visibilityWindow2->Swap();

					 visibilityWindow1->Activate();
					 visibilityWindow1->Clear( 0.10, 0.10, 0.30 );
					 objectViewpoint->Apply( visibilityWindow1, CYCLOPS );
					 visibilityObject1->Draw();
					 visibilityWindow1->Swap();

					 fovWindow2->Activate();
					 fovWindow2->Clear( 0.0, 0.0, 0.0 );
					 codaViewpoint2->Apply( fovWindow2, CYCLOPS );
					 alignmentObject2->Draw();
					 fovSweetSpot->Draw();
					 fovWindow2->Swap();

					 fovWindow1->Activate();
					 fovWindow1->Clear( 0.0, 0.0, 0.0 );
					 codaViewpoint1->Apply( fovWindow1, CYCLOPS );
					 alignmentObject1->Draw();
					 fovSweetSpot->Draw();
					 fovWindow1->Swap();

					 distanceWindow1->Activate();
					 distanceWindow1->Clear( 0.65, 0.85, 0.65, 1.0 );
					 distanceViewpoint->Apply( distanceWindow1, CYCLOPS );

					 //  SetLighting();
					 lowerExclusionZone->Draw();
					 upperExclusionZone->Draw();
					 distanceBars1->Draw();
					 distanceWindow1->Swap();

					 distanceWindow2->Activate();
					 distanceWindow2->Clear( 0.65, 0.85, 0.65, 1.0 );
					 distanceViewpoint->Apply( distanceWindow2, CYCLOPS );
					 // SetLighting();
					 lowerExclusionZone->Draw();
					 upperExclusionZone->Draw();
					 distanceBars2->Draw();
					 distanceWindow2->Swap();
					 distanceViewpoint->Apply( distanceWindow1, CYCLOPS );

				 }

				 // A timer to handle animations and screen refresh, and associated actions.
				 static System::Windows::Forms::Timer^ refreshTimer;
				 void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {

					 // Because the Windows Forms does not allow "mixed" types as instance variables and MarkerFrame 
					 // is a mixed type, we have to create the Pose tracker here each time in order to use a local frame.
					 TrackerPose pose;
					 MarkerFrame codaFrame;
					 CodaPoseTracker *poseTracker = new CodaPoseTracker( &codaFrame );

					 // The name of the model file is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
					 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
					 poseTracker->ReadModelMarkerPositions( model_file );
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

					 coda->GetCurrentMarkerFrameIntrinsic( codaFrame, 0 );
					 visibilityObject1->ShowVisibility( codaFrame, CYAN );
					 alignmentObject1->ShowVisibility( codaFrame, CYAN );
					 for ( int i = 0; i < visibilityObject1->nModelMarkers; i++ ) {
						 if ( codaFrame.marker[ visibilityObject1->modelMarker[i].id ].visibility ) {
							 distanceBars1->component[i]->Enable();
							 distanceBars1->component[i]->SetPosition( i, codaFrame.marker[ visibilityObject1->modelMarker[i].id ].position[Y], 0.0 );
						 }
						 else distanceBars1->component[i]->Disable();
					 }
					 poseTracker->GetCurrentPose( pose );
					 if ( pose.visible ) {
						 alignmentObject1->SetPose( pose.pose );
						 alignmentObject1->Enable();
					 }
					 else alignmentObject1->Disable();
					 coda->GetCurrentMarkerFrameIntrinsic( codaFrame, 1 );
					 visibilityObject2->ShowVisibility( codaFrame, CYAN );
					 alignmentObject2->ShowVisibility( codaFrame, CYAN );
					 for ( int i = 0; i < visibilityObject2->nModelMarkers; i++ ) {
						 if ( codaFrame.marker[ visibilityObject2->modelMarker[i].id ].visibility ) {
							 distanceBars2->component[i]->Enable();
							 distanceBars2->component[i]->SetPosition( i, codaFrame.marker[ visibilityObject2->modelMarker[i].id ].position[Y], 0.0 );
						 }
						 else distanceBars2->component[i]->Disable();
					 }
					 poseTracker->GetCurrentPose( pose );
					 if ( pose.visible ) {
						 alignmentObject2->SetPose( pose.pose );
						 alignmentObject2->Enable();
					 }
					 else alignmentObject2->Disable();

					 Render();

				 }
				 void CreateRefreshTimer( int interval ) {
					 refreshTimer = gcnew( System::Windows::Forms::Timer );
					 refreshTimer->Interval = interval;
					 refreshTimer->Tick += gcnew EventHandler( this, &SingleObjectForm::OnTimerElapsed );
				 }
				 void StartRefreshTimer( void ) {
					 refreshTimer->Start();
				 }
				 void StopRefreshTimer( void ) {
					 refreshTimer->Stop();
				 }		

		private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
					 // Send a message to ground to show our progress.
					 dex->SendSubstep( ABORT_REQUESTED );
					 System::Windows::Forms::DialogResult response;
					 response = MessageBox::Show( "Are you sure you want to reset the alignment?\nIf yes, you must then perform the alignment again.", "AlignToRigidBodyGUI", MessageBoxButtons::YesNo );
					 if ( response == System::Windows::Forms::DialogResult::Yes ) {
						 // Show the buttons on the form as being inactive.
						 cancelButton->Enabled = false;
						 alignButton->Enabled = false;
						 resetButton->Enabled = false;
						 // Show a message while we are busy acquiring and computing the new alignment.
						 busy->Text = L"Reset in Progress\r\nPlease wait ...";
						 busy->Visible = true;
						 Refresh();
						 Application::DoEvents();
						 // Log the data acquired in aligned coordinates.
						 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
						 String ^alignmentFilename = filenameRoot + ".nullalign.dat";
						 char *alignment_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( alignmentFilename ).ToPointer();
						 coda->AnnulAlignment( alignment_file );
						 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( alignment_file ) );
						 // Because GraspTrackerDaemon is also the DexServices proxy, and because changing the alignment
						 // for CodaRTnetDaemonTracker causes GraspTrackerDaemon to restart, we need to re-establish the 
						 // connection to the DexServices proxy.
						 dex->Connect();
						 dex->ResendTaskInfo();
						 cancelButton->Enabled = true;
						 alignButton->Enabled = true;
						 resetButton->Enabled = true;
						 busy->Visible = false;
					 }
					 //Presumably, were were in the visibility check state and we are returning there.
					 dex->SendSubstep( VISIBILITY );
				 }

				 // Provide the means to read a .ini file to set configuration parameters.
				 // This is defined here as static because its address is sent as a callback to a parsing routine.
		private: static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
					 return 1;
				 }

		private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
				 }
		};
}

