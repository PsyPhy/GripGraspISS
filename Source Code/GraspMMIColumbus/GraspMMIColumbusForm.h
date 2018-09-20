#pragma once

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"

#include "GraspData.h"

#define BODIES 3
#define HMD 0
#define HAND 1
#define CHEST 2

#define CODAS  2

PsyPhy::Vector3 tabletPositionSeated = { 0.0, -200.0, 1100 };
PsyPhy::Vector3 tabletPositionSupine = { 0.0, 200.0, 1250 };

PsyPhy::Vector3 codaPositionGripSeated[2] = {{ -800.0, 500.0, 2200.0 }, { -300.0, 1000.0, 2200.0 }};
PsyPhy::Vector3 codaPositionGripSupine[2] = {{ 800.0, 2200.0, 400.0 }, { 500.0, 2200.0, 800.0 }};
PsyPhy::Vector3 codaPositionGraspSeated[2] = {{ 1000.0, 650.0, -2800.0 }, { 300.0, 1200.0, -2800.0 }};
PsyPhy::Vector3 codaPositionGraspFloating[2] = {{ -700.0, 450.0, -2800.0 }, { -400.0, 400.0, -2800.0 }};

PsyPhy::Matrix3x3 codaOrientationGripSeated[2] = {{ 0.0, -1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0, 0.0 }, { -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, -1.0, 0.0 }};
PsyPhy::Matrix3x3 codaOrientationGripSupine[2] = {{ 0.0, 0.0, -1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 }};
PsyPhy::Matrix3x3 codaOrientationGraspSeated[2] = {{ 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0,  0.0 }};
PsyPhy::Matrix3x3 codaOrientationGraspFloating[2] = {{ 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0 }, { -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,  0.0 }};

namespace GraspMMIColumbus {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	// We need InteropServics in order to convert a String to a char *.
	using namespace System::Runtime::InteropServices;

	using namespace PsyPhy;
	using namespace Grasp;


	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class GraspMMIColumbusForm : public System::Windows::Forms::Form
	{
	public:
		GraspMMIColumbusForm(void)
		{
			InitializeComponent();
			// Flag that the graphics objects are not yet created.
			ensemble = nullptr;

			vm = new VectorsMixin();
			graspData = new GraspData();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GraspMMIColumbusForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  endconeGroupBox;
	private: System::Windows::Forms::Panel^  endconePanel;
	private: System::Windows::Forms::GroupBox^  topViewGroupBox;
	private: System::Windows::Forms::Panel^  topPanel;
	private: System::Windows::Forms::GroupBox^  nodeGroupBox;
	private: System::Windows::Forms::Panel^  nodePanel;
	private: System::Windows::Forms::GroupBox^  sideGroupBox;
	private: System::Windows::Forms::Panel^  sidePanel;
	private: System::Windows::Forms::GroupBox^  conifigurationGroupBox;
	private: System::Windows::Forms::RadioButton^  gripSupineButton;
	private: System::Windows::Forms::RadioButton^  gripSeatedButton;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::RadioButton^  graspFreefloatingButton;
	private: System::Windows::Forms::RadioButton^  graspSeatedButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::CheckBox^  markerStructureCheckBox;
	private: System::Windows::Forms::OpenFileDialog^  openGripAlignment;
	private: System::Windows::Forms::OpenFileDialog^  openGraspAlignment;
	private: System::Windows::Forms::CheckBox^  nominalCodaCheckBox;
	private: System::Windows::Forms::Button^  loadGraspButton;
	private: System::Windows::Forms::Button^  loadGripButton;
	private: System::Windows::Forms::CheckBox^  tabletCheckBox;
	private: System::Windows::Forms::CheckBox^  axesCheckBox;
	private: System::Windows::Forms::CheckBox^  bodyCheckBox;
	private: System::Windows::Forms::GroupBox^  coda0GroupBox;
	private: System::Windows::Forms::Panel^  coda0Panel;
	private: System::Windows::Forms::GroupBox^  coda1GroupBox;
	private: System::Windows::Forms::Panel^  coda1Panel;
	private: System::Windows::Forms::Button^  loadGraspTrajectoryButton;
	private: System::Windows::Forms::OpenFileDialog^  openGraspTrajectory;

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
			this->endconeGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->endconePanel = (gcnew System::Windows::Forms::Panel());
			this->topViewGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->topPanel = (gcnew System::Windows::Forms::Panel());
			this->nodeGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->nodePanel = (gcnew System::Windows::Forms::Panel());
			this->sideGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->sidePanel = (gcnew System::Windows::Forms::Panel());
			this->conifigurationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->loadGraspTrajectoryButton = (gcnew System::Windows::Forms::Button());
			this->nominalCodaCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->loadGraspButton = (gcnew System::Windows::Forms::Button());
			this->graspFreefloatingButton = (gcnew System::Windows::Forms::RadioButton());
			this->loadGripButton = (gcnew System::Windows::Forms::Button());
			this->graspSeatedButton = (gcnew System::Windows::Forms::RadioButton());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->gripSupineButton = (gcnew System::Windows::Forms::RadioButton());
			this->gripSeatedButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->axesCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->bodyCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->tabletCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->markerStructureCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->openGripAlignment = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openGraspAlignment = (gcnew System::Windows::Forms::OpenFileDialog());
			this->coda0GroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->coda0Panel = (gcnew System::Windows::Forms::Panel());
			this->coda1GroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->coda1Panel = (gcnew System::Windows::Forms::Panel());
			this->openGraspTrajectory = (gcnew System::Windows::Forms::OpenFileDialog());
			this->endconeGroupBox->SuspendLayout();
			this->topViewGroupBox->SuspendLayout();
			this->nodeGroupBox->SuspendLayout();
			this->sideGroupBox->SuspendLayout();
			this->conifigurationGroupBox->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->coda0GroupBox->SuspendLayout();
			this->coda1GroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// endconeGroupBox
			// 
			this->endconeGroupBox->Controls->Add(this->endconePanel);
			this->endconeGroupBox->Location = System::Drawing::Point(974, 576);
			this->endconeGroupBox->Name = L"endconeGroupBox";
			this->endconeGroupBox->Size = System::Drawing::Size(550, 550);
			this->endconeGroupBox->TabIndex = 0;
			this->endconeGroupBox->TabStop = false;
			this->endconeGroupBox->Text = L"From End Cone";
			// 
			// endconePanel
			// 
			this->endconePanel->Location = System::Drawing::Point(10, 20);
			this->endconePanel->Name = L"endconePanel";
			this->endconePanel->Size = System::Drawing::Size(534, 524);
			this->endconePanel->TabIndex = 0;
			// 
			// topViewGroupBox
			// 
			this->topViewGroupBox->Controls->Add(this->topPanel);
			this->topViewGroupBox->Location = System::Drawing::Point(12, 12);
			this->topViewGroupBox->Name = L"topViewGroupBox";
			this->topViewGroupBox->Size = System::Drawing::Size(956, 550);
			this->topViewGroupBox->TabIndex = 1;
			this->topViewGroupBox->TabStop = false;
			this->topViewGroupBox->Text = L"Top View";
			// 
			// topPanel
			// 
			this->topPanel->Location = System::Drawing::Point(10, 20);
			this->topPanel->Name = L"topPanel";
			this->topPanel->Size = System::Drawing::Size(940, 524);
			this->topPanel->TabIndex = 1;
			// 
			// nodeGroupBox
			// 
			this->nodeGroupBox->Controls->Add(this->nodePanel);
			this->nodeGroupBox->Location = System::Drawing::Point(1530, 576);
			this->nodeGroupBox->Name = L"nodeGroupBox";
			this->nodeGroupBox->Size = System::Drawing::Size(550, 550);
			this->nodeGroupBox->TabIndex = 2;
			this->nodeGroupBox->TabStop = false;
			this->nodeGroupBox->Text = L"From Node";
			// 
			// nodePanel
			// 
			this->nodePanel->Location = System::Drawing::Point(10, 20);
			this->nodePanel->Name = L"nodePanel";
			this->nodePanel->Size = System::Drawing::Size(534, 524);
			this->nodePanel->TabIndex = 1;
			// 
			// sideGroupBox
			// 
			this->sideGroupBox->Controls->Add(this->sidePanel);
			this->sideGroupBox->Location = System::Drawing::Point(12, 576);
			this->sideGroupBox->Name = L"sideGroupBox";
			this->sideGroupBox->Size = System::Drawing::Size(956, 550);
			this->sideGroupBox->TabIndex = 3;
			this->sideGroupBox->TabStop = false;
			this->sideGroupBox->Text = L"Side View";
			// 
			// sidePanel
			// 
			this->sidePanel->Location = System::Drawing::Point(10, 20);
			this->sidePanel->Name = L"sidePanel";
			this->sidePanel->Size = System::Drawing::Size(940, 524);
			this->sidePanel->TabIndex = 1;
			// 
			// conifigurationGroupBox
			// 
			this->conifigurationGroupBox->Controls->Add(this->loadGraspTrajectoryButton);
			this->conifigurationGroupBox->Controls->Add(this->nominalCodaCheckBox);
			this->conifigurationGroupBox->Controls->Add(this->label2);
			this->conifigurationGroupBox->Controls->Add(this->loadGraspButton);
			this->conifigurationGroupBox->Controls->Add(this->graspFreefloatingButton);
			this->conifigurationGroupBox->Controls->Add(this->loadGripButton);
			this->conifigurationGroupBox->Controls->Add(this->graspSeatedButton);
			this->conifigurationGroupBox->Controls->Add(this->label1);
			this->conifigurationGroupBox->Controls->Add(this->gripSupineButton);
			this->conifigurationGroupBox->Controls->Add(this->gripSeatedButton);
			this->conifigurationGroupBox->Location = System::Drawing::Point(2086, 12);
			this->conifigurationGroupBox->Name = L"conifigurationGroupBox";
			this->conifigurationGroupBox->Size = System::Drawing::Size(153, 517);
			this->conifigurationGroupBox->TabIndex = 4;
			this->conifigurationGroupBox->TabStop = false;
			this->conifigurationGroupBox->Text = L"Configuration";
			// 
			// loadGraspTrajectoryButton
			// 
			this->loadGraspTrajectoryButton->Location = System::Drawing::Point(6, 310);
			this->loadGraspTrajectoryButton->Name = L"loadGraspTrajectoryButton";
			this->loadGraspTrajectoryButton->Size = System::Drawing::Size(139, 47);
			this->loadGraspTrajectoryButton->TabIndex = 11;
			this->loadGraspTrajectoryButton->Text = L"Load GRASP Trajectory";
			this->loadGraspTrajectoryButton->UseVisualStyleBackColor = true;
			this->loadGraspTrajectoryButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGraspTrajectoryButton_Click);
			// 
			// nominalCodaCheckBox
			// 
			this->nominalCodaCheckBox->AutoSize = true;
			this->nominalCodaCheckBox->Checked = true;
			this->nominalCodaCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->nominalCodaCheckBox->Location = System::Drawing::Point(6, 485);
			this->nominalCodaCheckBox->Name = L"nominalCodaCheckBox";
			this->nominalCodaCheckBox->Size = System::Drawing::Size(132, 17);
			this->nominalCodaCheckBox->TabIndex = 11;
			this->nominalCodaCheckBox->Text = L"Show Nominal CODAs";
			this->nominalCodaCheckBox->UseVisualStyleBackColor = true;
			this->nominalCodaCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(14, 180);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(49, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"GRASP";
			// 
			// loadGraspButton
			// 
			this->loadGraspButton->Location = System::Drawing::Point(6, 257);
			this->loadGraspButton->Name = L"loadGraspButton";
			this->loadGraspButton->Size = System::Drawing::Size(139, 47);
			this->loadGraspButton->TabIndex = 10;
			this->loadGraspButton->Text = L"Load GRASP Alignment";
			this->loadGraspButton->UseVisualStyleBackColor = true;
			this->loadGraspButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGraspButton_Click);
			// 
			// graspFreefloatingButton
			// 
			this->graspFreefloatingButton->AutoSize = true;
			this->graspFreefloatingButton->Location = System::Drawing::Point(27, 230);
			this->graspFreefloatingButton->Name = L"graspFreefloatingButton";
			this->graspFreefloatingButton->Size = System::Drawing::Size(80, 17);
			this->graspFreefloatingButton->TabIndex = 4;
			this->graspFreefloatingButton->TabStop = true;
			this->graspFreefloatingButton->Text = L"Freefloating";
			this->graspFreefloatingButton->UseVisualStyleBackColor = true;
			this->graspFreefloatingButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::graspFreefloatingButton_Click);
			// 
			// loadGripButton
			// 
			this->loadGripButton->Location = System::Drawing::Point(6, 102);
			this->loadGripButton->Name = L"loadGripButton";
			this->loadGripButton->Size = System::Drawing::Size(139, 47);
			this->loadGripButton->TabIndex = 9;
			this->loadGripButton->Text = L"Load GRIP Alignment";
			this->loadGripButton->UseVisualStyleBackColor = true;
			this->loadGripButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGripButton_Click);
			// 
			// graspSeatedButton
			// 
			this->graspSeatedButton->AutoSize = true;
			this->graspSeatedButton->Location = System::Drawing::Point(27, 203);
			this->graspSeatedButton->Name = L"graspSeatedButton";
			this->graspSeatedButton->Size = System::Drawing::Size(59, 17);
			this->graspSeatedButton->TabIndex = 3;
			this->graspSeatedButton->TabStop = true;
			this->graspSeatedButton->Text = L"Seated";
			this->graspSeatedButton->UseVisualStyleBackColor = true;
			this->graspSeatedButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::graspSeatedButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(14, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(37, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"GRIP";
			// 
			// gripSupineButton
			// 
			this->gripSupineButton->AutoSize = true;
			this->gripSupineButton->Location = System::Drawing::Point(27, 75);
			this->gripSupineButton->Name = L"gripSupineButton";
			this->gripSupineButton->Size = System::Drawing::Size(58, 17);
			this->gripSupineButton->TabIndex = 1;
			this->gripSupineButton->TabStop = true;
			this->gripSupineButton->Text = L"Supine";
			this->gripSupineButton->UseVisualStyleBackColor = true;
			this->gripSupineButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::gripSupineButton_Click);
			// 
			// gripSeatedButton
			// 
			this->gripSeatedButton->AutoSize = true;
			this->gripSeatedButton->Location = System::Drawing::Point(27, 48);
			this->gripSeatedButton->Name = L"gripSeatedButton";
			this->gripSeatedButton->Size = System::Drawing::Size(59, 17);
			this->gripSeatedButton->TabIndex = 0;
			this->gripSeatedButton->TabStop = true;
			this->gripSeatedButton->Text = L"Seated";
			this->gripSeatedButton->UseVisualStyleBackColor = true;
			this->gripSeatedButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::gripSeatedButton_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->axesCheckBox);
			this->groupBox2->Controls->Add(this->bodyCheckBox);
			this->groupBox2->Controls->Add(this->tabletCheckBox);
			this->groupBox2->Controls->Add(this->markerStructureCheckBox);
			this->groupBox2->Location = System::Drawing::Point(2086, 576);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(153, 126);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Objects";
			// 
			// axesCheckBox
			// 
			this->axesCheckBox->AutoSize = true;
			this->axesCheckBox->Location = System::Drawing::Point(16, 97);
			this->axesCheckBox->Name = L"axesCheckBox";
			this->axesCheckBox->Size = System::Drawing::Size(134, 17);
			this->axesCheckBox->TabIndex = 3;
			this->axesCheckBox->Text = L"Reference Frame Axes";
			this->axesCheckBox->UseVisualStyleBackColor = true;
			this->axesCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// bodyCheckBox
			// 
			this->bodyCheckBox->AutoSize = true;
			this->bodyCheckBox->Location = System::Drawing::Point(16, 74);
			this->bodyCheckBox->Name = L"bodyCheckBox";
			this->bodyCheckBox->Size = System::Drawing::Size(103, 17);
			this->bodyCheckBox->TabIndex = 2;
			this->bodyCheckBox->Text = L"Head and Torso";
			this->bodyCheckBox->UseVisualStyleBackColor = true;
			this->bodyCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// tabletCheckBox
			// 
			this->tabletCheckBox->AutoSize = true;
			this->tabletCheckBox->Location = System::Drawing::Point(16, 51);
			this->tabletCheckBox->Name = L"tabletCheckBox";
			this->tabletCheckBox->Size = System::Drawing::Size(85, 17);
			this->tabletCheckBox->TabIndex = 1;
			this->tabletCheckBox->Text = L"GRIP Tablet";
			this->tabletCheckBox->UseVisualStyleBackColor = true;
			this->tabletCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// markerStructureCheckBox
			// 
			this->markerStructureCheckBox->AutoSize = true;
			this->markerStructureCheckBox->Location = System::Drawing::Point(16, 28);
			this->markerStructureCheckBox->Name = L"markerStructureCheckBox";
			this->markerStructureCheckBox->Size = System::Drawing::Size(104, 17);
			this->markerStructureCheckBox->TabIndex = 0;
			this->markerStructureCheckBox->Text = L"GRASP Markers";
			this->markerStructureCheckBox->UseVisualStyleBackColor = true;
			this->markerStructureCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// openGripAlignment
			// 
			this->openGripAlignment->DefaultExt = L"txt";
			this->openGripAlignment->RestoreDirectory = true;
			// 
			// openGraspAlignment
			// 
			this->openGraspAlignment->DefaultExt = L"dat";
			// 
			// coda0GroupBox
			// 
			this->coda0GroupBox->Controls->Add(this->coda0Panel);
			this->coda0GroupBox->Location = System::Drawing::Point(974, 12);
			this->coda0GroupBox->Name = L"coda0GroupBox";
			this->coda0GroupBox->Size = System::Drawing::Size(550, 550);
			this->coda0GroupBox->TabIndex = 3;
			this->coda0GroupBox->TabStop = false;
			this->coda0GroupBox->Text = L"From Coda 1";
			// 
			// coda0Panel
			// 
			this->coda0Panel->Location = System::Drawing::Point(10, 20);
			this->coda0Panel->Name = L"coda0Panel";
			this->coda0Panel->Size = System::Drawing::Size(534, 524);
			this->coda0Panel->TabIndex = 1;
			// 
			// coda1GroupBox
			// 
			this->coda1GroupBox->Controls->Add(this->coda1Panel);
			this->coda1GroupBox->Location = System::Drawing::Point(1530, 12);
			this->coda1GroupBox->Name = L"coda1GroupBox";
			this->coda1GroupBox->Size = System::Drawing::Size(550, 550);
			this->coda1GroupBox->TabIndex = 4;
			this->coda1GroupBox->TabStop = false;
			this->coda1GroupBox->Text = L"From Coda 2";
			// 
			// coda1Panel
			// 
			this->coda1Panel->Location = System::Drawing::Point(10, 20);
			this->coda1Panel->Name = L"coda1Panel";
			this->coda1Panel->Size = System::Drawing::Size(534, 524);
			this->coda1Panel->TabIndex = 1;
			// 
			// openGraspTrajectory
			// 
			this->openGraspTrajectory->DefaultExt = L"pse";
			this->openGraspTrajectory->Filter = L"PSE Files|*.pse|All Files|*.*";
			// 
			// GraspMMIColumbusForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(2251, 1138);
			this->Controls->Add(this->coda1GroupBox);
			this->Controls->Add(this->coda0GroupBox);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->conifigurationGroupBox);
			this->Controls->Add(this->sideGroupBox);
			this->Controls->Add(this->endconeGroupBox);
			this->Controls->Add(this->nodeGroupBox);
			this->Controls->Add(this->topViewGroupBox);
			this->Name = L"GraspMMIColumbusForm";
			this->Text = L"GraspMMI Columbus";
			this->Shown += gcnew System::EventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_Shown);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_Paint);
			this->endconeGroupBox->ResumeLayout(false);
			this->topViewGroupBox->ResumeLayout(false);
			this->nodeGroupBox->ResumeLayout(false);
			this->sideGroupBox->ResumeLayout(false);
			this->conifigurationGroupBox->ResumeLayout(false);
			this->conifigurationGroupBox->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->coda0GroupBox->ResumeLayout(false);
			this->coda1GroupBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	private:

		OpenGLWindow *topWindow, *sideWindow, *endconeWindow, *nodeWindow, *coda0Window, *coda1Window;
		Viewpoint *topViewpoint, *sideViewpoint, *endconeViewpoint, *nodeViewpoint, *coda0Viewpoint, *coda1Viewpoint;
		OrthoViewpoint *sideOrthoViewpoint, *topOrthoViewpoint;

		// A class that provides methods for making a lot of the 
		// OpenGLObjects that we need for displaying the status.
		GraspGLObjects *renderer;

		// A visual representation of each GRASP marker structure.
		MarkerStructureGLObject *hmd;
		MarkerStructureGLObject *hand;
		MarkerStructureGLObject *chest;

		// Visual representations of the GRIP hardware.
		Assembly	*chair;
		Assembly	*tablet;
		Assembly	*manipulandum;

		Cylinder *chestEmissionCone;
		Cylinder *handEmissionCone;
		Cylinder *hmdEmissionCone;

		Cord  *hmdPath;
		Cord  *handPath;
		Cord  *chestPath;

		// Bars showing the canonical axes.
		Assembly *axes;
		// The ensemble of hardware. All objects are positioned with respect to this object.
		// Tracker data is referenced to a piece of hardware, not to the ISS. The reference
		// frame for the tracker therefore changes according to the hardware configuration.
		// We can then relocate and reorient all the hardware as a group with respect to the 
		// Columbus Module based on known location of the chair, tablet or freefloating frame when
		// installed in the ISS.
		Assembly *ensemble;
		// A set of objects that visually represent the walls and racks if the ISS.
		Assembly *iss;

		GraspData		*graspData;
		VectorsMixin	*vm;

		// A timer to handle animations and screen refresh, and associated actions.
		// For the moment, I don't believe that it is used.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			Render();
		}
		void CreateRefreshTimer( int interval ) {
			refreshTimer = gcnew( System::Windows::Forms::Timer );
			refreshTimer->Interval = interval;
			refreshTimer->Tick += gcnew EventHandler( this, &GraspMMIColumbusForm::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			refreshTimer->Start();
		}
		void StopRefreshTimer( void ) {
			refreshTimer->Stop();
		}		

		// When the form is show, create all of the graphical objects.
		System::Void GraspMMIColumbusForm_Shown(System::Object^  sender, System::EventArgs^  e) {

			// Create the required OpenGLWindows each linked to a pane in the Form.
			sideWindow = CreateOpenGLWindowInForm( sidePanel );
			topWindow = CreateOpenGLWindowInForm( topPanel );
			endconeWindow = CreateOpenGLWindowInForm( endconePanel );
			nodeWindow = CreateOpenGLWindowInForm( nodePanel );
			coda0Window = CreateOpenGLWindowInForm( coda0Panel );
			coda1Window = CreateOpenGLWindowInForm( coda1Panel );

			// Create a viewpoint that looks at the Columbus module from the side.
			// This is the cannonical viewpoint for an object at zero position and orientation.
			topViewpoint = new Viewpoint( 6.0, 20.0, 10.0, 15000.0);
			topViewpoint->SetPosition( 0.0, 6000.0, 0.0 );
			topViewpoint->SetOrientation( 90.0, 90.0, 0.0 );

			coda0Viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 15000.0);
			coda1Viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 15000.0);

			topOrthoViewpoint = new OrthoViewpoint( - 2500, 2500, -1250, 1250, 10, 10000 );
			topOrthoViewpoint->SetPosition( 0.0, 6000.0, 0.0 );
			topOrthoViewpoint->SetOrientation( 0.0, - 90.0, - 90.0 );

			sideViewpoint = new Viewpoint( 6.0, 20.0, 10.0, 15000.0);
			sideViewpoint->SetPosition( - 6000.0, 0.0, 0.0 );
			sideViewpoint->SetOrientation( 0.0, 0.0, 90.0 );

			sideOrthoViewpoint = new OrthoViewpoint( - 2500, 2500, -1250, 1250, 10, 10000 );
			sideOrthoViewpoint->SetPosition( -5000.0, 0.0, 0.0 );
			sideOrthoViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );

			endconeViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 8000.0);
			endconeViewpoint->SetPosition( 0.0, 0.0, -3000.0 );
			endconeViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

			nodeViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 8000.0);
			nodeViewpoint->SetPosition( 0.0, 0.0, 3000.0 );
			nodeViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

			// Create objects representing the racks.
			iss = new Assembly();
			WindowFrame *rack;
			for ( int r = -2; r < 2; r++ ) {
				rack = new WindowFrame( 980.0, 1980.0, 5.0 );
				rack->SetOffset( 500.0, 0.0, 0.0 );
				rack->SetOrientation( 0.0, 0.0, 90.0 );
				rack->SetColor( 0.5, 0.0, 0.5 );
				rack->SetPosition( 1000.0, 0.0, r * 1000.0 );
				iss->AddComponent( rack );
			}

			for ( int r = -2; r < 2; r++ ) {
				rack = new WindowFrame( 980.0, 1980.0, 5.0 );
				rack->SetOffset( 500.0, 0.0, 0.0 );
				rack->SetOrientation( 90.0, 90.0, 0.0 );
				rack->SetColor( 0.0, 0.5, 0.5 );
				rack->SetPosition( 0.0, -1000.0, r * 1000.0 );
				iss->AddComponent( rack );
			}
			rack = new WindowFrame( 2000.0, 2000.0, 5.0 );
			rack->SetPosition( 0.0, 0.0, - 2000.0 );


			// Create the OpenGLObjects that depict the marker array structure.
			renderer = new Grasp::GraspGLObjects();

			// Now create the objects for the VR rendering for GRASP.
			renderer->CreateVRObjects();
			renderer->PlaceVRObjects();
			renderer->CreateAuxiliaryObjects();

			// VR Objects to represent the marker structures.
			hmd = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
			hand = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
			chest = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );

			// Set nominal values for the pose of each object.
			double nominal_head_height = 500.0;
			double nominal_chest_depth = 200.0;
			double nominal_shoulder_breadth = 450.0;
			double nominal_reach = 600.0;
			hmd->SetPosition( 0.0, nominal_head_height, - nominal_chest_depth );
			hand->SetPosition( nominal_shoulder_breadth / 2.0, 0.7 * nominal_head_height, - nominal_reach - nominal_chest_depth );
			chest->SetPosition( 0.0, 0.0, - nominal_chest_depth );

			renderer->head->SetPosition( 0.0, nominal_head_height, - 0.3 * nominal_chest_depth );
			renderer->torso->SetPosition( 0.0, 0.4 * nominal_head_height, - nominal_chest_depth / 2.0 );

			// Add a cones to the marker structures to show the emission pattern.
			double cone_length = 2000.0;
			handEmissionCone = new Cylinder( 500.0, 0.0, cone_length );
			handEmissionCone->SetOffset( 0.0, 0.0, - cone_length / 2.0  );
			handEmissionCone->SetColor( Translucid( RED ) );
			hand->AddComponent( handEmissionCone );

			// Create objects to represent the GRIP hardware.
			tablet = new Assembly();
			Slab *slab;
			double tablet_width = 500.0;
			double tablet_length = 500.0;
			double tablet_thickness = 100.0;
			GLfloat grip_color[4] = { .5f, .35f, .25f, 0.5f };
			slab = new Slab( tablet_width / 2.0, tablet_length, 0.75 * tablet_thickness );
			slab->SetPosition( 0.265 * tablet_width, 0.0,  tablet_thickness / 2.0 );
			slab->SetColor( grip_color );
			tablet->AddComponent( slab );
			slab = new Slab( tablet_width / 2.0, tablet_length, tablet_thickness );
			slab->SetPosition( - 0.265 * tablet_width, 0.0, 0.0 );
			slab->SetColor( grip_color );
			tablet->AddComponent( slab );

			Assembly *mast = new Assembly();
			Sphere *target;
			double mast_width = 25.0;
			double mast_height = 600.0;
			slab = new Slab( mast_width, mast_width, mast_height );
			slab->SetColor( grip_color );
			slab->SetPosition( 0.0, 0.0, mast_height / 2.0 + tablet_thickness / 2.0 );
			mast->AddComponent( slab );
			for ( int i = 0; i < 11; i++ ) {
				target = new Sphere( 10.0 );
				target->SetColor( Translucid( RED ) );
				target->SetPosition( 0.0, mast_width, i * 50.0 + 100.0 );
				mast->AddComponent( target );
			}
			mast->SetPosition( - 0.45 * tablet_width, - 0.20 * tablet_length, 0.0 );
			tablet->AddComponent( mast );
			tablet->SetAttitude( 0.0, 90.0, 0.0 );
			tablet->SetOffset( tablet_width / 2.0, - tablet_thickness / 2.0, - tablet_length / 2.0 );

			// Create a set of visible axes at the origin.
			axes = new Assembly();
			Cylinder *axis;
			double thickness = 5.0;
			double length = 500.0;
			axis = new Cylinder( thickness, thickness, length );
			axis->SetOffset( 0.0, 0.0, length / 2.0 );
			axis->SetOrientation( 0.0, 0.0, -90.0 );
			axis->SetColor( RED );
			axes->AddComponent( axis );
			axis = new Cylinder( thickness, thickness, length );
			axis->SetOffset( 0.0, 0.0, length / 2.0 );
			axis->SetOrientation( 0.0, 90.0, 0.0 );
			axis->SetColor( GREEN );
			axes->AddComponent( axis );
			axis = new Cylinder( thickness, thickness, length );
			axis->SetOffset( 0.0, 0.0, length / 2.0 );
			axis->SetColor( BLUE );
			axes->AddComponent( axis );

			hmdPath = new Cord( graspData->visibleMarker[0][20], graspData->nVisibleSamples[0][20] );
			hmdPath->SetColor( RED );
			handPath = new Cord( graspData->visibleMarker[0][12], graspData->nVisibleSamples[0][12] );
			handPath->SetColor( GREEN );
			chestPath = new Cord( graspData->visibleMarker[0][18], graspData->nVisibleSamples[0][18] );
			chestPath->SetColor( YELLOW );

			// Create an anchor that we can rotate and displace to take into account
			// where the origin is with respect to the ISS module for a given hardware configuration.
			ensemble = new Assembly();
			ensemble->AddComponent( axes );
			ensemble->AddComponent( renderer->coda[0] );
			ensemble->AddComponent( renderer->coda[1] );
			ensemble->AddComponent( hmd );
			ensemble->AddComponent( hand );
			ensemble->AddComponent( chest );
			ensemble->AddComponent( tablet );
			ensemble->AddComponent( renderer->head );
			ensemble->AddComponent( renderer->torso );
			ensemble->AddComponent( handPath );
			ensemble->AddComponent( hmdPath );
			ensemble->AddComponent( chestPath );

			hmd->Disable();
			hand->Disable();
			chest->Disable();

			Render();

		}

		void LookAtFrom( Viewpoint *viewpoint, const Vector3 target, Vector3 from, const Vector3 up ) {

			static VectorsMixin vm;
			Matrix3x3 back;

			// Viewpoints look at the origin from the position of each CODA.
			vm.SubtractVectors( back[Z], from, target );
			vm.NormalizeVector( back[Z] );
			vm.ComputeCrossProduct( back[X], up, back[Z] );
			vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
			viewpoint->SetPosition( from );
			viewpoint->SetOrientation( back );

		}

		// Draw the 3D graphics.
		void RenderWindow( OpenGLWindow *window, Viewpoint *viewpoint ) {
			window->Activate();
			window->Clear( 0.10, 0.10, 0.30 );
			viewpoint->Apply( window, CYCLOPS );
			renderer->SetLighting();
			iss->Draw();
			ensemble->Draw();
			window->Swap();
		}

		// Draw the 3D graphics.
		void RenderWindow( OpenGLWindow *window, OrthoViewpoint *viewpoint ) {
			window->Activate();
			window->Clear( 0.10, 0.10, 0.30 );
			viewpoint->Apply( window, CYCLOPS );
			renderer->SetLighting();
			iss->Draw();
			ensemble->Draw();
			window->Swap();
		}
		// Draw the 3D graphics in the local reference frame.
		void RenderWindowLocal( OpenGLWindow *window, Viewpoint *viewpoint ) {
			window->Activate();
			window->Clear( 0.10, 0.10, 0.30 );
			viewpoint->Apply( window, CYCLOPS );
			renderer->SetLighting();
			for ( int i = 0; i < ensemble->components; i ++ ) ensemble->component[i]->Draw();
			window->Swap();
		}

		System::Void Render( void ) {

			// When we look at the subject from each of the CODAs, we need to define
			// the roll orientation. We choose the direction pointing up toward the 
			// ceiling. But it has to be expressed in the local reference frame, so 
			// it depends on the hardware configuration.
			Vector3		up;

			// If we get this call before the graphics items are created
			// then ignore and return.
			if ( ensemble == nullptr ) return;

			// Rotate and displace the ensemble of objects depending on
			// which hardware configuration was used to align the CODAs.
			if ( gripSeatedButton->Checked ) {
				ensemble->SetPosition( tabletPositionSeated );
				ensemble->SetOrientation( 0.0, 0.0, 180.0 );
				vm->CopyVector( up, vm->jVector );
				if ( nominalCodaCheckBox->Checked ) {
					renderer->coda[0]->SetPosition( codaPositionGripSeated[0] );
					renderer->coda[0]->SetOrientation( codaOrientationGripSeated[0] );
					renderer->coda[1]->SetPosition( codaPositionGripSeated[1] );
					renderer->coda[1]->SetOrientation( codaOrientationGripSeated[1] );
				}
			}
			else if ( gripSupineButton->Checked ) {
				ensemble->SetPosition( tabletPositionSupine );
				ensemble->SetOrientation( 0.0, 90.0, 0.0 );
				vm->CopyVector( up, vm->kVector );
				if ( nominalCodaCheckBox->Checked ) {
					renderer->coda[0]->SetPosition( codaPositionGripSupine[0] );
					renderer->coda[0]->SetOrientation( codaOrientationGripSupine[0] );
					renderer->coda[1]->SetPosition( codaPositionGripSupine[1] );
					renderer->coda[1]->SetOrientation( codaOrientationGripSupine[1] );
				}
			}
			else if ( graspSeatedButton->Checked ) {
				ensemble->SetPosition( 0.0, -400.0, 1800.0 );
				ensemble->SetOrientation( 0.0, 0.0, 0.0 );
				vm->CopyVector( up, vm->jVector );
				if ( nominalCodaCheckBox->Checked ) {
					renderer->coda[0]->SetPosition( codaPositionGraspSeated[0] );
					renderer->coda[0]->SetOrientation( codaOrientationGraspSeated[0] );
					renderer->coda[1]->SetPosition( codaPositionGraspSeated[1] );
					renderer->coda[1]->SetOrientation( codaOrientationGraspSeated[1] );
				}
			}
			else if ( graspFreefloatingButton->Checked ) {
				ensemble->SetPosition( 0.0, 0.0, 1500.0 );
				ensemble->SetOrientation( 180.0, - 26.0, 0.0 );
				// This is not quite right. The up vector when the alignment
				// is performed on the freefloating plate is not the Y axis
				// because of the tilt of the plate. But this is probably close enough.
				vm->ScaleVector( up, vm->jVector, -1.0 );
				if ( nominalCodaCheckBox->Checked ) {
					renderer->coda[0]->SetPosition( codaPositionGraspFloating[0] );
					renderer->coda[0]->SetOrientation( codaOrientationGraspFloating[0] );
					renderer->coda[1]->SetPosition( codaPositionGraspFloating[1] );
					renderer->coda[1]->SetOrientation( codaOrientationGraspFloating[1] );

				}
			}

			if ( markerStructureCheckBox->Checked ) {
				hmd->Enable();
				hand->Enable();
				chest->Enable();
			}
			else {
				hmd->Disable();
				hand->Disable();
				chest->Disable();
			}

			if ( bodyCheckBox->Checked ) {
				renderer->head->Enable();
				renderer->torso->Enable();
			}
			else {
				renderer->head->Disable();
				renderer->torso->Disable();
			}

			if ( tabletCheckBox->Checked ) tablet->Enable();
			else tablet->Disable();

			if ( axesCheckBox->Checked ) axes->Enable();
			else axes->Disable();

			// This is a hack. The cone looks better for certain roll orientations.
			handEmissionCone->SetAttitude( 0.0, 0.0, 0.0 );
			handEmissionCone->Enable();
			RenderWindow( topWindow, topOrthoViewpoint );

			// This is a hack. The cone looks better for certain roll orientations.
			handEmissionCone->SetAttitude( 90.0, 0.0, 0.0 );
			RenderWindow( sideWindow, sideOrthoViewpoint );

			// Another hack. Don't draw the cone when looking from either end.
			handEmissionCone->Disable();
			RenderWindow( endconeWindow, endconeViewpoint );
			RenderWindow( nodeWindow, nodeViewpoint );

			renderer->coda[0]->Disable();
			LookAtFrom( coda0Viewpoint, vm->zeroVector, renderer->coda[0]->position, up );
			RenderWindowLocal( coda0Window, coda0Viewpoint );
			renderer->coda[0]->Enable();
			renderer->coda[1]->Disable();
			LookAtFrom( coda1Viewpoint, vm->zeroVector, renderer->coda[1]->position, up );
			RenderWindowLocal( coda1Window, coda1Viewpoint );
			renderer->coda[1]->Enable();

		}


		// Read one line of a GRIP alignment record, containing the alignment for one CODA.
		// It is used below to read the contents of the file that contains both alignments.
	private: System::Void ReadSingleGripCodaAlignment( FILE *fp, int unit ) {

				 PsyPhy::Matrix3x3	R;
				 PsyPhy::Vector3	p;
				 char field[1024];

				 // Read alignment.
				 fscanf( fp, "%s %s %s %s %s", field, field, field, field, field );
				 for ( int i = 0; i < 3; i++ ) {
					 for ( int j = 0; j < 3; j++ ) {
						 fscanf( fp, "%lf", &R[j][i] );
					 }
				 }
				 for ( int i = 0; i < 3; i++ ) {
					 fscanf( fp, "%lf", &p[i] );
				 }
				 renderer->coda[unit]->SetPosition( p );
				 renderer->coda[unit]->SetOrientation( R );
			 }

			 // Read a file containing the CODA alignment transformations sent by GRIP.
	private: System::Void LoadGripCodaAlignment( void ) {

				 openGripAlignment->ShowDialog();

				 char *path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( openGripAlignment->FileName ).ToPointer();
				 FILE *fp = fopen( path, "r" );
				 if ( !fp ) {
					 fMessageBox( MB_OK, "GRIP/GRASP", "Error opening GRIP Alignment %s for read.", path );
				 }
				 else {
					 char		line[1024];
					 fOutputDebugString( "File %s open for reading.\n", path );
					 // Skip the header.
					 fgets( line, sizeof( line ), fp );

					 ReadSingleGripCodaAlignment( fp, 0 );
					 ReadSingleGripCodaAlignment( fp, 1 );

					 fclose( fp );
				 }
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( path ) );
				 Text = openGripAlignment->FileName;
				 nominalCodaCheckBox->Checked = false;

			 }

	private: System::Void LoadGraspCodaAlignment ( void ) {

				 openGraspAlignment->ShowDialog();

				 char *path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( openGraspAlignment->FileName ).ToPointer();
				 FILE *fp = fopen( path, "r" );
				 if ( !fp ) {
					 fMessageBox( MB_OK, "GRIP/GRASP", "Error opening GRASP alignment %s for read.", path );
				 }
				 else {
					 char		line[1024];
					 PsyPhy::Matrix3x3	r, inv;
					 PsyPhy::Vector3		p;
					 char field[1024];
					 fOutputDebugString( "File %s open for reading.\n", path );
					 // Skip the header.
					 fgets( line, sizeof( line ), fp );

					 // Serial Number
					 fgets( field, sizeof(field), fp );
					 // Offset
					 fscanf( fp, "Offset0=%lf,%lf,%lf\n", &p[X], &p[Y], &p[Z] );
					 // Rotation
					 fscanf( fp, "TransformX0=%lf,%lf,%lf\n", &r[X][X], &r[X][Y], &r[X][Z] );
					 fscanf( fp, "TransformY0=%lf,%lf,%lf\n", &r[Y][X], &r[Y][Y], &r[Y][Z] );
					 fscanf( fp, "TransformZ0=%lf,%lf,%lf\n", &r[Z][X], &r[Z][Y], &r[Z][Z] );

					 renderer->coda[0]->SetPosition( p );
					 vm->InvertMatrix( inv, r );
					 renderer->coda[0]->SetOrientation( inv );

					 // Serial Number
					 fgets( field, sizeof(field), fp );
					 // Offset
					 fscanf( fp, "Offset1=%lf,%lf,%lf\n", &p[X], &p[Y], &p[Z] );
					 // Rotation
					 fscanf( fp, "TransformX1=%lf,%lf,%lf\n", &r[X][X], &r[X][Y], &r[X][Z] );
					 fscanf( fp, "TransformY1=%lf,%lf,%lf\n", &r[Y][X], &r[Y][Y], &r[Y][Z] );
					 fscanf( fp, "TransformZ1=%lf,%lf,%lf\n", &r[Z][X], &r[Z][Y], &r[Z][Z] );

					 renderer->coda[1]->SetPosition( p );
					 vm->InvertMatrix( inv, r );
					 renderer->coda[1]->SetOrientation( inv );

					 fclose( fp );
				 }
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( path ) );
				 Text = openGraspAlignment->FileName;
				 nominalCodaCheckBox->Checked = false;
			 }




	private: System::Void GraspMMIColumbusForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 Render();
			 }

	private: System::Void loadGripButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 LoadGripCodaAlignment();
				 // Guess which GRIP configuration we are in, based on the orientation of CODA 2.
				 if ( renderer->coda[1]->orientation[0][0] > 0 ) gripSupineButton->Checked = true;
				 else gripSeatedButton->Checked = true;
				 // Show the tablet because it is GRIP.
				 tabletCheckBox->Checked = true;
				 markerStructureCheckBox->Checked = false;
				 Render();
			 }
	private: System::Void loadGraspButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 LoadGraspCodaAlignment();
				 // Guess which GRASP configuration we are in, based on the orientation of CODA 2.
				 if ( renderer->coda[1]->orientation[0][0] > 0 ) graspSeatedButton->Checked = true;
				 else graspFreefloatingButton->Checked = true;
				 // Hide the tablet because it is GRIP.
				 tabletCheckBox->Checked = false;
				 markerStructureCheckBox->Checked = true;
				 Render();
			 }

	private: System::Void Button_Click(System::Object^  sender, System::EventArgs^  e) {
				 Render();
			 }
	private: System::Void gripSeatedButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 tabletCheckBox->Checked = true;
				 markerStructureCheckBox->Checked = false;
				 Button_Click( sender, e );
			 }
	private: System::Void gripSupineButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 tabletCheckBox->Checked = true;
				 markerStructureCheckBox->Checked = false;
				 Button_Click( sender, e );
			 }
	private: System::Void graspSeatedButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 tabletCheckBox->Checked = false;
				 markerStructureCheckBox->Checked = true;
				 Button_Click( sender, e );
			 }
	private: System::Void graspFreefloatingButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 tabletCheckBox->Checked = false;
				 markerStructureCheckBox->Checked = true;
				 Button_Click( sender, e );
			 }

	private: System::Void loadGraspTrajectoryButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 openGraspTrajectory->ShowDialog();

				 char *path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( openGraspTrajectory->FileName ).ToPointer();
				 FILE *fp = fopen( path, "r" );
				 if ( !fp ) {
					 fMessageBox( MB_OK, "GRIP/GRASP", "Error opening GRASP trajectory %s for read.", path );
				 }
				 else {
					 graspData->LoadGraspData( fp );
					 fclose( fp );
				 }
				System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( path ) );

				hmdPath->Reload( graspData->visibleMarker[0][4], graspData->nVisibleSamples[0][4] );
				handPath->Reload( graspData->visibleMarker[0][12], graspData->nVisibleSamples[0][12] );
				chestPath->Reload( graspData->visibleMarker[0][18], graspData->nVisibleSamples[0][18] );

				Render();

			 }
	};
}

