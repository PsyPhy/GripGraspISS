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
#include "../GraspMMIUtilities/GraspData.h"
#include "../GraspMMIUtilities/GripGLObjects.h"
#include "../GraspMMIUtilities/GripData.h"

#define BODIES 3
#define HMD 0
#define HAND 1
#define CHEST 2

#define CODA_UNITS  2

double codaMinimumDistance980 = 1500.0;
double codaMinimumDistance992 = 1750.0;

PsyPhy::Vector3 tabletPositionSeated = { 0.0, -200.0, 1150 };
PsyPhy::Vector3 tabletPositionSupine = { 0.0, 0.0, 1300 };
PsyPhy::Vector3 chairPosition = { -300.0, -200.0, 1900 };
PsyPhy::Vector3 platePosition = { 0.0, 0.0, 1300 };

PsyPhy::Vector3 codaNominalPosition[2] = {{ -800.0, 600.0, -500.0 }, { 0.0, 900.0, -500.0 }};

int n_manipulandum_markers = 8;
int n_wrist_markers = 8;
int n_tablet_markers = 4;
int first_manipulandum_marker = 0;
int first_wrist_marker = 13;
int first_tablet_marker = 9;

namespace GraspMMIColumbus {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Printing;
	// We need InteropServics in order to convert a String to a char *.
	using namespace System::Runtime::InteropServices;

	using namespace PsyPhy;
	using namespace Grasp;
	using namespace Grip;


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
			gripData = new GripData();

			locationList.Clear();
			locationList.Add( "ONE", "1" );
			locationList.Add( "two", "2" );

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
	private: System::Windows::Forms::GroupBox^  topGroupBox;
	private: System::Windows::Forms::Panel^  topPanel;
	private: System::Windows::Forms::GroupBox^  nodeGroupBox;
	private: System::Windows::Forms::Panel^  nodePanel;
	private: System::Windows::Forms::GroupBox^  sideGroupBox;
	private: System::Windows::Forms::Panel^  sidePanel;
	private: System::Windows::Forms::GroupBox^  configurationGroupBox;
	private: System::Windows::Forms::RadioButton^  gripSupineButton;
	private: System::Windows::Forms::RadioButton^  gripSeatedButton;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::RadioButton^  graspFreefloatingButton;
	private: System::Windows::Forms::RadioButton^  graspSeatedButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::OpenFileDialog^  openGripAlignment;
	private: System::Windows::Forms::OpenFileDialog^  openGraspAlignment;
	private: System::Windows::Forms::Button^  loadGraspButton;
	private: System::Windows::Forms::Button^  loadGripButton;
	private: System::Windows::Forms::CheckBox^  axesCheckBox;
	private: System::Windows::Forms::GroupBox^  coda0GroupBox;
	private: System::Windows::Forms::Panel^  coda0Panel;
	private: System::Windows::Forms::GroupBox^  coda1GroupBox;
	private: System::Windows::Forms::Panel^  coda1Panel;
	private: System::Windows::Forms::Button^  loadGraspTrajectoryButton;
	private: System::Windows::Forms::OpenFileDialog^  openGraspTrajectory;
	private: System::Windows::Forms::Button^  printButton;
	private: System::Windows::Forms::FolderBrowserDialog^  screenshotFolderDialog;
	private: System::Windows::Forms::Button^  selectScreenshotButton;

	private: System::Windows::Forms::Label^  loadingLabel;
	private: System::Windows::Forms::CheckBox^  proximityCheckBox;
	private: System::Windows::Forms::RadioButton^  threshold992RadioButton;
	private: System::Windows::Forms::RadioButton^  threshold980RadioButton;
	private: System::Windows::Forms::Button^  loadGripTrajectoryButton;
	private: System::Windows::Forms::CheckBox^  bodyCheckBox;
	private: System::Windows::Forms::CheckBox^  markerStructureCheckBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::CheckBox^  nominalCodaCheckBox;
	private: System::Windows::Forms::CheckBox^  tabletCheckBox;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::OpenFileDialog^  openGripTrajectory;
	private: System::Windows::Forms::CheckBox^  gripTracerCheckBox;
	private: System::Windows::Forms::CheckBox^  graspTracerCheckBox;



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
			this->topGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->topPanel = (gcnew System::Windows::Forms::Panel());
			this->nodeGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->nodePanel = (gcnew System::Windows::Forms::Panel());
			this->sideGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->sidePanel = (gcnew System::Windows::Forms::Panel());
			this->configurationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->tabletCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->bodyCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->markerStructureCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->loadGripTrajectoryButton = (gcnew System::Windows::Forms::Button());
			this->loadGraspTrajectoryButton = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->loadGraspButton = (gcnew System::Windows::Forms::Button());
			this->graspFreefloatingButton = (gcnew System::Windows::Forms::RadioButton());
			this->loadGripButton = (gcnew System::Windows::Forms::Button());
			this->graspSeatedButton = (gcnew System::Windows::Forms::RadioButton());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->gripSupineButton = (gcnew System::Windows::Forms::RadioButton());
			this->gripSeatedButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->threshold992RadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->threshold980RadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->proximityCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->axesCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->openGripAlignment = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openGraspAlignment = (gcnew System::Windows::Forms::OpenFileDialog());
			this->coda0GroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->coda0Panel = (gcnew System::Windows::Forms::Panel());
			this->coda1GroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->coda1Panel = (gcnew System::Windows::Forms::Panel());
			this->openGraspTrajectory = (gcnew System::Windows::Forms::OpenFileDialog());
			this->printButton = (gcnew System::Windows::Forms::Button());
			this->screenshotFolderDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->selectScreenshotButton = (gcnew System::Windows::Forms::Button());
			this->loadingLabel = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->nominalCodaCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->openGripTrajectory = (gcnew System::Windows::Forms::OpenFileDialog());
			this->graspTracerCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->gripTracerCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->endconeGroupBox->SuspendLayout();
			this->topGroupBox->SuspendLayout();
			this->nodeGroupBox->SuspendLayout();
			this->sideGroupBox->SuspendLayout();
			this->configurationGroupBox->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->coda0GroupBox->SuspendLayout();
			this->coda1GroupBox->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// endconeGroupBox
			// 
			this->endconeGroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->endconeGroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->endconeGroupBox->Controls->Add(this->endconePanel);
			this->endconeGroupBox->Location = System::Drawing::Point(793, 353);
			this->endconeGroupBox->Name = L"endconeGroupBox";
			this->endconeGroupBox->Size = System::Drawing::Size(316, 339);
			this->endconeGroupBox->TabIndex = 0;
			this->endconeGroupBox->TabStop = false;
			this->endconeGroupBox->Text = L"From End Cone";
			this->endconeGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::endconeGroupBox_SizeChanged);
			// 
			// endconePanel
			// 
			this->endconePanel->Location = System::Drawing::Point(10, 20);
			this->endconePanel->Name = L"endconePanel";
			this->endconePanel->Size = System::Drawing::Size(300, 300);
			this->endconePanel->TabIndex = 0;
			this->endconePanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::endconePanel_SizeChanged);
			// 
			// topGroupBox
			// 
			this->topGroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->topGroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->topGroupBox->Controls->Add(this->topPanel);
			this->topGroupBox->Location = System::Drawing::Point(171, 7);
			this->topGroupBox->Name = L"topGroupBox";
			this->topGroupBox->Size = System::Drawing::Size(616, 339);
			this->topGroupBox->TabIndex = 1;
			this->topGroupBox->TabStop = false;
			this->topGroupBox->Text = L"Top View";
			this->topGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::topGroupBox_SizeChanged);
			// 
			// topPanel
			// 
			this->topPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->topPanel->Location = System::Drawing::Point(10, 20);
			this->topPanel->Name = L"topPanel";
			this->topPanel->Size = System::Drawing::Size(600, 300);
			this->topPanel->TabIndex = 1;
			this->topPanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::topPanel_SizeChanged);
			// 
			// nodeGroupBox
			// 
			this->nodeGroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->nodeGroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->nodeGroupBox->Controls->Add(this->nodePanel);
			this->nodeGroupBox->Location = System::Drawing::Point(1115, 350);
			this->nodeGroupBox->Name = L"nodeGroupBox";
			this->nodeGroupBox->Size = System::Drawing::Size(316, 339);
			this->nodeGroupBox->TabIndex = 2;
			this->nodeGroupBox->TabStop = false;
			this->nodeGroupBox->Text = L"From Node";
			this->nodeGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::nodeGroupBox_SizeChanged);
			// 
			// nodePanel
			// 
			this->nodePanel->Location = System::Drawing::Point(10, 20);
			this->nodePanel->Name = L"nodePanel";
			this->nodePanel->Size = System::Drawing::Size(300, 300);
			this->nodePanel->TabIndex = 1;
			this->nodePanel->Resize += gcnew System::EventHandler(this, &GraspMMIColumbusForm::nodePanel_Resize);
			// 
			// sideGroupBox
			// 
			this->sideGroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->sideGroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->sideGroupBox->Controls->Add(this->sidePanel);
			this->sideGroupBox->Location = System::Drawing::Point(171, 353);
			this->sideGroupBox->Name = L"sideGroupBox";
			this->sideGroupBox->Size = System::Drawing::Size(616, 339);
			this->sideGroupBox->TabIndex = 3;
			this->sideGroupBox->TabStop = false;
			this->sideGroupBox->Text = L"Side View";
			this->sideGroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::sideGroupBox_SizeChanged);
			// 
			// sidePanel
			// 
			this->sidePanel->Location = System::Drawing::Point(10, 20);
			this->sidePanel->Name = L"sidePanel";
			this->sidePanel->Size = System::Drawing::Size(600, 300);
			this->sidePanel->TabIndex = 1;
			this->sidePanel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::sidePanel_SizeChanged);
			// 
			// configurationGroupBox
			// 
			this->configurationGroupBox->Controls->Add(this->gripTracerCheckBox);
			this->configurationGroupBox->Controls->Add(this->graspTracerCheckBox);
			this->configurationGroupBox->Controls->Add(this->tabletCheckBox);
			this->configurationGroupBox->Controls->Add(this->bodyCheckBox);
			this->configurationGroupBox->Controls->Add(this->markerStructureCheckBox);
			this->configurationGroupBox->Controls->Add(this->loadGripTrajectoryButton);
			this->configurationGroupBox->Controls->Add(this->loadGraspTrajectoryButton);
			this->configurationGroupBox->Controls->Add(this->label2);
			this->configurationGroupBox->Controls->Add(this->loadGraspButton);
			this->configurationGroupBox->Controls->Add(this->graspFreefloatingButton);
			this->configurationGroupBox->Controls->Add(this->loadGripButton);
			this->configurationGroupBox->Controls->Add(this->graspSeatedButton);
			this->configurationGroupBox->Controls->Add(this->label1);
			this->configurationGroupBox->Controls->Add(this->gripSupineButton);
			this->configurationGroupBox->Controls->Add(this->gripSeatedButton);
			this->configurationGroupBox->Location = System::Drawing::Point(12, 12);
			this->configurationGroupBox->Name = L"configurationGroupBox";
			this->configurationGroupBox->Size = System::Drawing::Size(153, 415);
			this->configurationGroupBox->TabIndex = 4;
			this->configurationGroupBox->TabStop = false;
			this->configurationGroupBox->Text = L"Configuration";
			// 
			// tabletCheckBox
			// 
			this->tabletCheckBox->AutoSize = true;
			this->tabletCheckBox->Location = System::Drawing::Point(20, 157);
			this->tabletCheckBox->Name = L"tabletCheckBox";
			this->tabletCheckBox->Size = System::Drawing::Size(56, 17);
			this->tabletCheckBox->TabIndex = 17;
			this->tabletCheckBox->Text = L"Tablet";
			this->tabletCheckBox->UseVisualStyleBackColor = true;
			this->tabletCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// bodyCheckBox
			// 
			this->bodyCheckBox->AutoSize = true;
			this->bodyCheckBox->Location = System::Drawing::Point(18, 371);
			this->bodyCheckBox->Name = L"bodyCheckBox";
			this->bodyCheckBox->Size = System::Drawing::Size(103, 17);
			this->bodyCheckBox->TabIndex = 16;
			this->bodyCheckBox->Text = L"Head and Torso";
			this->bodyCheckBox->UseVisualStyleBackColor = true;
			this->bodyCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// markerStructureCheckBox
			// 
			this->markerStructureCheckBox->AutoSize = true;
			this->markerStructureCheckBox->Location = System::Drawing::Point(18, 348);
			this->markerStructureCheckBox->Name = L"markerStructureCheckBox";
			this->markerStructureCheckBox->Size = System::Drawing::Size(110, 17);
			this->markerStructureCheckBox->TabIndex = 15;
			this->markerStructureCheckBox->Text = L"Marker Structures";
			this->markerStructureCheckBox->UseVisualStyleBackColor = true;
			this->markerStructureCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// loadGripTrajectoryButton
			// 
			this->loadGripTrajectoryButton->Location = System::Drawing::Point(18, 117);
			this->loadGripTrajectoryButton->Name = L"loadGripTrajectoryButton";
			this->loadGripTrajectoryButton->Size = System::Drawing::Size(101, 33);
			this->loadGripTrajectoryButton->TabIndex = 12;
			this->loadGripTrajectoryButton->Text = L"Load Trajectory";
			this->loadGripTrajectoryButton->UseVisualStyleBackColor = true;
			this->loadGripTrajectoryButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGripTrajectoryButton_Click);
			// 
			// loadGraspTrajectoryButton
			// 
			this->loadGraspTrajectoryButton->Location = System::Drawing::Point(18, 309);
			this->loadGraspTrajectoryButton->Name = L"loadGraspTrajectoryButton";
			this->loadGraspTrajectoryButton->Size = System::Drawing::Size(101, 33);
			this->loadGraspTrajectoryButton->TabIndex = 11;
			this->loadGraspTrajectoryButton->Text = L"Load Trajectory";
			this->loadGraspTrajectoryButton->UseVisualStyleBackColor = true;
			this->loadGraspTrajectoryButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGraspTrajectoryButton_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(18, 205);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(49, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"GRASP";
			// 
			// loadGraspButton
			// 
			this->loadGraspButton->Location = System::Drawing::Point(18, 270);
			this->loadGraspButton->Name = L"loadGraspButton";
			this->loadGraspButton->Size = System::Drawing::Size(101, 33);
			this->loadGraspButton->TabIndex = 10;
			this->loadGraspButton->Text = L"Load Alignment";
			this->loadGraspButton->UseVisualStyleBackColor = true;
			this->loadGraspButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGraspButton_Click);
			// 
			// graspFreefloatingButton
			// 
			this->graspFreefloatingButton->AutoSize = true;
			this->graspFreefloatingButton->Location = System::Drawing::Point(35, 247);
			this->graspFreefloatingButton->Name = L"graspFreefloatingButton";
			this->graspFreefloatingButton->Size = System::Drawing::Size(80, 17);
			this->graspFreefloatingButton->TabIndex = 4;
			this->graspFreefloatingButton->TabStop = true;
			this->graspFreefloatingButton->Text = L"Freefloating";
			this->graspFreefloatingButton->UseVisualStyleBackColor = true;
			this->graspFreefloatingButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::graspButton_Click);
			// 
			// loadGripButton
			// 
			this->loadGripButton->Location = System::Drawing::Point(18, 78);
			this->loadGripButton->Name = L"loadGripButton";
			this->loadGripButton->Size = System::Drawing::Size(101, 33);
			this->loadGripButton->TabIndex = 9;
			this->loadGripButton->Text = L"Load Alignment";
			this->loadGripButton->UseVisualStyleBackColor = true;
			this->loadGripButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::loadGripButton_Click);
			// 
			// graspSeatedButton
			// 
			this->graspSeatedButton->AutoSize = true;
			this->graspSeatedButton->Location = System::Drawing::Point(35, 224);
			this->graspSeatedButton->Name = L"graspSeatedButton";
			this->graspSeatedButton->Size = System::Drawing::Size(59, 17);
			this->graspSeatedButton->TabIndex = 3;
			this->graspSeatedButton->TabStop = true;
			this->graspSeatedButton->Text = L"Seated";
			this->graspSeatedButton->UseVisualStyleBackColor = true;
			this->graspSeatedButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::graspButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(18, 17);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(37, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"GRIP";
			// 
			// gripSupineButton
			// 
			this->gripSupineButton->AutoSize = true;
			this->gripSupineButton->Location = System::Drawing::Point(34, 56);
			this->gripSupineButton->Name = L"gripSupineButton";
			this->gripSupineButton->Size = System::Drawing::Size(58, 17);
			this->gripSupineButton->TabIndex = 1;
			this->gripSupineButton->TabStop = true;
			this->gripSupineButton->Text = L"Supine";
			this->gripSupineButton->UseVisualStyleBackColor = true;
			this->gripSupineButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::gripButton_Click);
			// 
			// gripSeatedButton
			// 
			this->gripSeatedButton->AutoSize = true;
			this->gripSeatedButton->Location = System::Drawing::Point(34, 33);
			this->gripSeatedButton->Name = L"gripSeatedButton";
			this->gripSeatedButton->Size = System::Drawing::Size(59, 17);
			this->gripSeatedButton->TabIndex = 0;
			this->gripSeatedButton->TabStop = true;
			this->gripSeatedButton->Text = L"Seated";
			this->gripSeatedButton->UseVisualStyleBackColor = true;
			this->gripSeatedButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::gripButton_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->threshold992RadioButton);
			this->groupBox2->Controls->Add(this->threshold980RadioButton);
			this->groupBox2->Controls->Add(this->proximityCheckBox);
			this->groupBox2->Controls->Add(this->axesCheckBox);
			this->groupBox2->Location = System::Drawing::Point(14, 510);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(153, 105);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Objects";
			// 
			// threshold992RadioButton
			// 
			this->threshold992RadioButton->AutoSize = true;
			this->threshold992RadioButton->Checked = true;
			this->threshold992RadioButton->Location = System::Drawing::Point(33, 84);
			this->threshold992RadioButton->Name = L"threshold992RadioButton";
			this->threshold992RadioButton->Size = System::Drawing::Size(52, 17);
			this->threshold992RadioButton->TabIndex = 7;
			this->threshold992RadioButton->TabStop = true;
			this->threshold992RadioButton->Text = L"0.992";
			this->threshold992RadioButton->UseVisualStyleBackColor = true;
			this->threshold992RadioButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// threshold980RadioButton
			// 
			this->threshold980RadioButton->AutoSize = true;
			this->threshold980RadioButton->Location = System::Drawing::Point(33, 61);
			this->threshold980RadioButton->Name = L"threshold980RadioButton";
			this->threshold980RadioButton->Size = System::Drawing::Size(52, 17);
			this->threshold980RadioButton->TabIndex = 6;
			this->threshold980RadioButton->TabStop = true;
			this->threshold980RadioButton->Text = L"0.980";
			this->threshold980RadioButton->UseVisualStyleBackColor = true;
			this->threshold980RadioButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// proximityCheckBox
			// 
			this->proximityCheckBox->AutoSize = true;
			this->proximityCheckBox->Location = System::Drawing::Point(10, 42);
			this->proximityCheckBox->Name = L"proximityCheckBox";
			this->proximityCheckBox->Size = System::Drawing::Size(109, 17);
			this->proximityCheckBox->TabIndex = 5;
			this->proximityCheckBox->Text = L"Proximity Spheres";
			this->proximityCheckBox->UseVisualStyleBackColor = true;
			this->proximityCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			this->proximityCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// axesCheckBox
			// 
			this->axesCheckBox->AutoSize = true;
			this->axesCheckBox->Location = System::Drawing::Point(10, 19);
			this->axesCheckBox->Name = L"axesCheckBox";
			this->axesCheckBox->Size = System::Drawing::Size(134, 17);
			this->axesCheckBox->TabIndex = 3;
			this->axesCheckBox->Text = L"Reference Frame Axes";
			this->axesCheckBox->UseVisualStyleBackColor = true;
			this->axesCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// openGripAlignment
			// 
			this->openGripAlignment->DefaultExt = L"txt";
			this->openGripAlignment->RestoreDirectory = true;
			// 
			// openGraspAlignment
			// 
			this->openGraspAlignment->DefaultExt = L"dat";
			this->openGraspAlignment->Filter = L"Grasp Alignment|*.dat|All Files|*.*";
			// 
			// coda0GroupBox
			// 
			this->coda0GroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->coda0GroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->coda0GroupBox->Controls->Add(this->coda0Panel);
			this->coda0GroupBox->Location = System::Drawing::Point(793, 7);
			this->coda0GroupBox->Name = L"coda0GroupBox";
			this->coda0GroupBox->Size = System::Drawing::Size(316, 339);
			this->coda0GroupBox->TabIndex = 3;
			this->coda0GroupBox->TabStop = false;
			this->coda0GroupBox->Text = L"From Coda 1";
			this->coda0GroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::coda0GroupBox_SizeChanged);
			// 
			// coda0Panel
			// 
			this->coda0Panel->Location = System::Drawing::Point(10, 20);
			this->coda0Panel->Name = L"coda0Panel";
			this->coda0Panel->Size = System::Drawing::Size(300, 300);
			this->coda0Panel->TabIndex = 1;
			this->coda0Panel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::coda0Panel_SizeChanged);
			// 
			// coda1GroupBox
			// 
			this->coda1GroupBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->coda1GroupBox->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->coda1GroupBox->Controls->Add(this->coda1Panel);
			this->coda1GroupBox->Location = System::Drawing::Point(1115, 7);
			this->coda1GroupBox->Name = L"coda1GroupBox";
			this->coda1GroupBox->Size = System::Drawing::Size(316, 339);
			this->coda1GroupBox->TabIndex = 4;
			this->coda1GroupBox->TabStop = false;
			this->coda1GroupBox->Text = L"From Coda 2";
			this->coda1GroupBox->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::coda1GroupBox_SizeChanged);
			// 
			// coda1Panel
			// 
			this->coda1Panel->Location = System::Drawing::Point(10, 20);
			this->coda1Panel->Name = L"coda1Panel";
			this->coda1Panel->Size = System::Drawing::Size(300, 300);
			this->coda1Panel->TabIndex = 1;
			this->coda1Panel->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::coda1Panel_SizeChanged);
			// 
			// openGraspTrajectory
			// 
			this->openGraspTrajectory->DefaultExt = L"pse";
			this->openGraspTrajectory->Filter = L"PSE Files|*.pse|All Files|*.*";
			// 
			// printButton
			// 
			this->printButton->Location = System::Drawing::Point(33, 660);
			this->printButton->Name = L"printButton";
			this->printButton->Size = System::Drawing::Size(101, 33);
			this->printButton->TabIndex = 6;
			this->printButton->Text = L"Save";
			this->printButton->UseVisualStyleBackColor = true;
			this->printButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::button1_Click);
			// 
			// selectScreenshotButton
			// 
			this->selectScreenshotButton->Location = System::Drawing::Point(33, 621);
			this->selectScreenshotButton->Name = L"selectScreenshotButton";
			this->selectScreenshotButton->Size = System::Drawing::Size(101, 33);
			this->selectScreenshotButton->TabIndex = 7;
			this->selectScreenshotButton->Text = L"Screeshot Folder";
			this->selectScreenshotButton->UseVisualStyleBackColor = true;
			this->selectScreenshotButton->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::selectScreenshotButton_Click);
			// 
			// loadingLabel
			// 
			this->loadingLabel->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->loadingLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->loadingLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 48, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->loadingLabel->ForeColor = System::Drawing::Color::Red;
			this->loadingLabel->Location = System::Drawing::Point(420, 300);
			this->loadingLabel->Name = L"loadingLabel";
			this->loadingLabel->Size = System::Drawing::Size(800, 200);
			this->loadingLabel->TabIndex = 8;
			this->loadingLabel->Text = L"Loading Trajectory";
			this->loadingLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->loadingLabel->Visible = false;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Controls->Add(this->nominalCodaCheckBox);
			this->groupBox1->Location = System::Drawing::Point(14, 433);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(151, 74);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"CODA Placement";
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"Nominal\t\t1, 2, 3, 4, 5, 6"});
			this->comboBox1->Location = System::Drawing::Point(6, 42);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(135, 21);
			this->comboBox1->TabIndex = 13;
			// 
			// nominalCodaCheckBox
			// 
			this->nominalCodaCheckBox->AutoSize = true;
			this->nominalCodaCheckBox->Checked = true;
			this->nominalCodaCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->nominalCodaCheckBox->Location = System::Drawing::Point(19, 19);
			this->nominalCodaCheckBox->Name = L"nominalCodaCheckBox";
			this->nominalCodaCheckBox->Size = System::Drawing::Size(61, 17);
			this->nominalCodaCheckBox->TabIndex = 12;
			this->nominalCodaCheckBox->Text = L"Presets";
			this->nominalCodaCheckBox->UseVisualStyleBackColor = true;
			this->nominalCodaCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// openGripTrajectory
			// 
			this->openGripTrajectory->Filter = L"Pose Files|*.RT.csv|All Files|*.*";
			// 
			// graspTracerCheckBox
			// 
			this->graspTracerCheckBox->AutoSize = true;
			this->graspTracerCheckBox->Location = System::Drawing::Point(17, 394);
			this->graspTracerCheckBox->Name = L"graspTracerCheckBox";
			this->graspTracerCheckBox->Size = System::Drawing::Size(98, 17);
			this->graspTracerCheckBox->TabIndex = 18;
			this->graspTracerCheckBox->Text = L"Marker Tracers";
			this->graspTracerCheckBox->UseVisualStyleBackColor = true;
			this->graspTracerCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// gripTracerCheckBox
			// 
			this->gripTracerCheckBox->AutoSize = true;
			this->gripTracerCheckBox->Location = System::Drawing::Point(20, 178);
			this->gripTracerCheckBox->Name = L"gripTracerCheckBox";
			this->gripTracerCheckBox->Size = System::Drawing::Size(98, 17);
			this->gripTracerCheckBox->TabIndex = 19;
			this->gripTracerCheckBox->Text = L"Marker Tracers";
			this->gripTracerCheckBox->UseVisualStyleBackColor = true;
			this->gripTracerCheckBox->Click += gcnew System::EventHandler(this, &GraspMMIColumbusForm::Button_Click);
			// 
			// GraspMMIColumbusForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1438, 707);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->loadingLabel);
			this->Controls->Add(this->selectScreenshotButton);
			this->Controls->Add(this->printButton);
			this->Controls->Add(this->coda1GroupBox);
			this->Controls->Add(this->coda0GroupBox);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->configurationGroupBox);
			this->Controls->Add(this->sideGroupBox);
			this->Controls->Add(this->endconeGroupBox);
			this->Controls->Add(this->nodeGroupBox);
			this->Controls->Add(this->topGroupBox);
			this->Location = System::Drawing::Point(10, 100);
			this->Name = L"GraspMMIColumbusForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"GraspMMI Columbus";
			this->Shown += gcnew System::EventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_Shown);
			this->SizeChanged += gcnew System::EventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_SizeChanged);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_Paint);
			this->Resize += gcnew System::EventHandler(this, &GraspMMIColumbusForm::GraspMMIColumbusForm_SizeChanged);
			this->endconeGroupBox->ResumeLayout(false);
			this->topGroupBox->ResumeLayout(false);
			this->nodeGroupBox->ResumeLayout(false);
			this->sideGroupBox->ResumeLayout(false);
			this->configurationGroupBox->ResumeLayout(false);
			this->configurationGroupBox->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->coda0GroupBox->ResumeLayout(false);
			this->coda1GroupBox->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private:

		SortedList	locationList;

		String^ AlignmentFilename;
		String^ TrajectoryFilename;

		OpenGLWindow	*topWindow, *sideWindow, *endconeWindow, *nodeWindow;
		OpenGLWindow	**codaWindow;

		Viewpoint		*topViewpoint, *sideViewpoint, *endconeViewpoint, *nodeViewpoint, **codaViewpoint;
		Viewpoint		*sideOrthoViewpoint;
		Viewpoint		*topOrthoViewpoint;

		// A class that provides methods for making a lot of the 
		// OpenGLObjects that we need for displaying the status.
		GraspGLObjects *renderer;

		// A visual representation of each GRASP marker structure.
		MarkerStructureGLObject *hmd;
		MarkerStructureGLObject *hand;
		MarkerStructureGLObject *chest;

		// Visual representations of the GRIP hardware.
		Assembly	*chair;
		GripTablet	*tabletLocal, *tabletSeated, *tabletSupine;
		Assembly	*manipulandum;

		Coda		**coda;
		Assembly	*codas;

		Coda		**presetCoda;
		Assembly	*presetCodas;

		Cylinder *chestEmissionCone;
		Cylinder *handEmissionCone;
		Cylinder *hmdEmissionCone;

		Assembly *graspTracers;
		Assembly **graspTracerUnit;
		Cord  ***hmdPath;
		Cord  ***handPath;
		Cord  ***chestPath;

		Assembly *gripTracers;
		Assembly **gripTracerUnit;
		Cord  ***manipulandumPath;
		Cord  ***wristPath;
		Cord  ***tabletPath;

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
		Assembly *racks;

		GraspData		*graspData;
		GripData		*gripData;
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

			AlignmentFilename = "(none)";
			TrajectoryFilename = "(none)";

			// Create the required OpenGLWindows each linked to a pane in the Form.
			sideWindow = CreateOpenGLWindowInForm( sidePanel );
			topWindow = CreateOpenGLWindowInForm( topPanel );
			endconeWindow = CreateOpenGLWindowInForm( endconePanel );
			nodeWindow = CreateOpenGLWindowInForm( nodePanel );
			codaWindow = (OpenGLWindow **) calloc( CODA_UNITS, sizeof( *codaWindow ) );
			codaWindow[0] = CreateOpenGLWindowInForm( coda0Panel );
			codaWindow[1] = CreateOpenGLWindowInForm( coda1Panel );

			// Create a viewpoint that looks at the Columbus module from the side.
			// This is the cannonical viewpoint for an object at zero position and orientation.
			topViewpoint = new Viewpoint( 6.0, 20.0, 10.0, 15000.0);
			topViewpoint->SetPosition( 0.0, 6000.0, 0.0 );
			topViewpoint->SetOrientation( -90.0, - 90.0, 180.0 );

			topOrthoViewpoint = new Viewpoint( 6.0, 15.0, 8000.0, 15000.0);
			topOrthoViewpoint->SetPosition( 0.0, 10000.0, 0.0 );
			topOrthoViewpoint->SetOrientation( -90.0, - 90.0, 180.0 );
			//topOrthoViewpoint = new OrthoViewpoint( - 2500, 2500, -1250, 1250, 10, 10000 );
			//topOrthoViewpoint->SetPosition( 0.0, - 6000.0, 0.0 );
			//topOrthoViewpoint->SetOrientation( 0.0, - 90.0, 0.0 );

			sideViewpoint = new Viewpoint( 6.0, 20.0, 10.0, 15000.0);
			sideViewpoint->SetPosition( - 6000.0, 0.0, 0.0 );
			sideViewpoint->SetOrientation( 0.0, 0.0, 90.0 );

			sideOrthoViewpoint = new Viewpoint( 6.0, 15.0, 9000.0, 15000.0);
			sideOrthoViewpoint->SetPosition( -10000.0, 0.0, 0.0 );
			sideOrthoViewpoint->SetOrientation( 0.0, 0.0, 90.0 );
			//sideOrthoViewpoint = new OrthoViewpoint( - 2500, 2500, -1250, 1250, 10, 10000 );
			//sideOrthoViewpoint->SetPosition( -5000.0, 0.0, 0.0 );
			//sideOrthoViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );

			endconeViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 8000.0);
			endconeViewpoint->SetPosition( 0.0, 0.0, -3000.0 );
			endconeViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

			nodeViewpoint = new Viewpoint( 6.0, 45.0, 10.0, 8000.0);
			nodeViewpoint->SetPosition( 0.0, 0.0, 3000.0 );
			nodeViewpoint->SetOrientation( 0.0, 0.0, 0.0 );

			// Create objects representing the racks.
			double rack_bar_thickness = 10.0;
			double rack_height = 1980.0;
			double rack_width = 960.0;
			double rack_on_centers = 1000.0;

			racks = new Assembly();

			WindowFrame *rack;
			for ( int r = -2; r < 2; r++ ) {
				rack = new WindowFrame( rack_width, rack_height, rack_bar_thickness );
				rack->SetOffset( rack_on_centers / 2.0, 0.0, 0.0 );
				rack->SetOrientation( 0.0, 0.0, 90.0 );
				rack->SetColor( 0.5, 0.0, 0.5 );
				rack->SetPosition( rack_on_centers, 0.0, r * rack_on_centers );
				racks->AddComponent( rack );
			}

			for ( int r = -2; r < 2; r++ ) {
				rack = new WindowFrame( rack_width, rack_height, rack_bar_thickness );
				rack->SetOffset( rack_on_centers / 2.0, 0.0, 0.0 );
				rack->SetOrientation( 90.0, 90.0, 0.0 );
				rack->SetColor( 0.0, 0.5, 0.5 );
				rack->SetPosition( 0.0, - rack_on_centers, r * rack_on_centers );
				racks->AddComponent( rack );
			}
			//rack = new WindowFrame( 2000.0, 2000.0, rack_bar_thickness );
			//rack->SetPosition( 0.0, 0.0, - 2000.0 );


			// Create the OpenGLObjects that depict the marker array structure.
			renderer = new Grasp::GraspGLObjects();

			// Now create the objects for the VR rendering for GRASP.
			renderer->CreateVRObjects();
			renderer->PlaceVRObjects();
			renderer->CreateAuxiliaryObjects();

			codas = new Assembly();
			coda = (Coda **) calloc( CODA_UNITS, sizeof( *coda ) );
			for (int unit = 0; unit < CODA_UNITS; unit++ ) {
				coda[unit] = new Coda();
				codas->AddComponent( coda[unit] );
			}
			coda[0]->SetColor( .5, .1, .1 );
			coda[1]->SetColor( .1, .1, .6 );
			coda[0]->proximity->SetColor( 1.0, 0.5, 0.5, 0.2 );
			coda[1]->proximity->SetColor( 0.5, 0.5, 1.0, 0.2 );
			coda[0]->fov->SetColor( 1.0, 0.0, 0.0, 0.5 );
			coda[1]->fov->SetColor( 0.0, 0.0, 1.0, 0.5 );

			presetCodas = new Assembly();
			presetCoda = (Coda **) calloc( CODA_UNITS, sizeof( *presetCoda ) );
			for (int unit = 0; unit < CODA_UNITS; unit++ ) {
				presetCoda[unit] = new Coda();
				presetCoda[unit]->SetOffset( 0.0, 0.0, 0.0 );
				presetCodas->AddComponent( presetCoda[unit] );
			}
			presetCoda[0]->SetColor( .5, .1, .1 );
			presetCoda[1]->SetColor( .1, .1, .6 );
			presetCoda[0]->proximity->SetColor( 1.0, 0.5, 0.50, 0.2 );
			presetCoda[1]->proximity->SetColor( 0.5, 0.5, 1.0, 0.2 );
			presetCoda[0]->fov->SetColor( 1.0, 0.0, 0.0, 0.5 );
			presetCoda[1]->fov->SetColor( 0.0, 0.0, 1.0, 0.5 );
			presetCoda[0]->SetOffset ( 0.0, 0.0, 0.0 );
			presetCoda[1]->SetOffset ( 0.0, 0.0, 0.0 );


			for ( int unit = 0; unit < CODA_UNITS; unit++ ) presetCoda[unit]->SetPosition( codaNominalPosition[unit] );

			// VR Objects to represent the marker structures.
			hmd = renderer->CreateHmdMarkerStructure( "Bdy\\HMD.bdy" );
			hand = renderer->CreateHandMarkerStructure( "Bdy\\Hand.bdy" );
			chest = renderer->CreateChestMarkerStructure( "Bdy\\Chest.bdy" );

			hmd->Disable();
			hand->Disable();
			chest->Disable();

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
			handEmissionCone->Disable();

			Cylinder *axis;
			double axis_thickness = 5.0;
			double axis_length = 1800.0;

			// Create a set of visible axes at the origin.
			axes = new Assembly();
			axis = new Cylinder( axis_thickness, axis_thickness, axis_length );
			axis->SetOffset( 0.0, 0.0, axis_length / 2.0 );
			axis->SetOrientation( 0.0, 0.0, -90.0 );
			axis->SetColor( RED );
			axes->AddComponent( axis );
			axis = new Cylinder( axis_thickness, axis_thickness, axis_length );
			axis->SetOffset( 0.0, 0.0, axis_length / 2.0 );
			axis->SetOrientation( 0.0, 90.0, 0.0 );
			axis->SetColor( GREEN );
			axes->AddComponent( axis );
			axis = new Cylinder( axis_thickness, axis_thickness, axis_length );
			axis->SetOffset( 0.0, 0.0, axis_length / 2.0 );
			axis->SetColor( BLUE );
			axes->AddComponent( axis );


			// Create viewpoints that look at Columbus from the point of view of the CODA.
			// When a CODA is at the null orientation (according to it's internal alignment
			// transformations, it is looking down the positive Y axis, but in the OpenGL
			// system, the null orientation is looking down the negative Z axis. To make it
			// so that we can apply the orientation reported by the CODA system directly, we
			// apply an 'attitude' to the viewpoint. This is the same attitude set for the 
			// graphical representation of the coda (see GraspGLObjects). 
			codaViewpoint = (Viewpoint **) calloc( CODA_UNITS, sizeof( *codaViewpoint ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				codaViewpoint[unit] =  new Viewpoint( 6.0, 85.0, 10.0, 15000.0);
				codaViewpoint[unit]->SetAttitude( coda[unit]->attitude );
				codaViewpoint[unit]->SetOffset( coda[unit]->offset );
			}

			tabletLocal = new GripTablet();
			tabletLocal->SetColor( phantom_tablet_color );

			tabletSeated = new GripTablet();
			tabletSeated->SetPosition( tabletPositionSeated );
			tabletSeated->SetOrientation( 0.0, 0.0, 180.0 );
			tabletSeated->Disable();
			tabletSupine = new GripTablet();
			tabletSupine->SetPosition( tabletPositionSupine );
			tabletSupine->SetOrientation( 0.0, 90.0, 0.0 );
			tabletSupine->Disable();

			// Paths follow by the markers.
			// Each 'graspTracerUnit' contains the three paths linked to a given CODA unit.
			// The variable 'graspTracers' points to both unit-specific structure.
			graspTracers = new Assembly();
			graspTracerUnit = (Assembly **) calloc( CODA_UNITS, sizeof( *graspTracerUnit ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) graspTracerUnit[unit] = new Assembly();

			hmdPath = (Cord ***) calloc( CODA_UNITS, sizeof( **hmdPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				hmdPath[unit] = (Cord **) calloc( hmd->nModelMarkers, sizeof( *hmdPath ) );
				for ( int i = 0; i < hmd->nModelMarkers; i++ ) {
					hmdPath[unit][i] = new Cord();
					hmdPath[unit][i]->SetColor( unit == 0 ? RED : MAGENTA );
					graspTracerUnit[unit]->AddComponent( hmdPath[unit][i] );
				}
			}
			handPath = (Cord ***) calloc( CODA_UNITS, sizeof( **handPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				handPath[unit] = (Cord **) calloc( hmd->nModelMarkers, sizeof( *handPath ) );
				for ( int i = 0; i < hand->nModelMarkers; i++ ) {
					handPath[unit][i] = new Cord();
					handPath[unit][i]->SetColor( unit == 0 ? GREEN : YELLOW );
					graspTracerUnit[unit]->AddComponent( handPath[unit][i] );
				}
			}
			chestPath = (Cord ***) calloc( CODA_UNITS, sizeof( **chestPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				chestPath[unit] = (Cord **) calloc( chest->nModelMarkers, sizeof( *chestPath ) );
				for ( int i = 0; i < hmd->nModelMarkers; i++ ) {
					chestPath[unit][i] = new Cord();
					chestPath[unit][i]->SetColor( unit == 0 ? BLUE : CYAN );
					graspTracerUnit[unit]->AddComponent( chestPath[unit][i] );
				}
			}
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) graspTracers->AddComponent( graspTracerUnit[unit] );

			// Paths follow by the markers.
			// Each 'gripTracerUnit' contains the three paths linked to a given CODA unit.
			// The variable 'graspTracers' points to both unit-specific structure.

			gripTracers = new Assembly();
			gripTracerUnit = (Assembly **) calloc( CODA_UNITS, sizeof( *gripTracerUnit ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) gripTracerUnit[unit] = new Assembly();

			manipulandumPath = (Cord ***) calloc( CODA_UNITS, sizeof( **manipulandumPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				manipulandumPath[unit] = (Cord **) calloc( n_manipulandum_markers, sizeof( *manipulandumPath ) );
				for ( int i = 0; i < n_manipulandum_markers; i++ ) {
					manipulandumPath[unit][i] = new Cord();
					gripTracerUnit[unit]->AddComponent( manipulandumPath[unit][i] );
					manipulandumPath[unit][i]->SetColor( unit == 0 ? RED : MAGENTA );
				}
			}
			wristPath = (Cord ***) calloc( CODA_UNITS, sizeof( **wristPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				wristPath[unit] = (Cord **) calloc( n_wrist_markers, sizeof( *wristPath ) );
				for ( int i = 0; i < n_wrist_markers; i++ ) {
					wristPath[unit][i] = new Cord();
					gripTracerUnit[unit]->AddComponent( wristPath[unit][i] );
					wristPath[unit][i]->SetColor( unit == 0 ? GREEN : YELLOW );
				}
			}
			tabletPath = (Cord ***) calloc( CODA_UNITS, sizeof( **tabletPath ) );
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				tabletPath[unit] = (Cord **) calloc( n_wrist_markers, sizeof( *tabletPath ) );
				for ( int i = 0; i < n_tablet_markers; i++ ) {
					tabletPath[unit][i] = new Cord();
					gripTracerUnit[unit]->AddComponent( tabletPath[unit][i] );
					wristPath[unit][i]->SetColor( unit == 0 ? BLUE : CYAN );
				}
			}
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) gripTracers->AddComponent( gripTracerUnit[unit] );


			// Create an anchor that we can rotate and displace to take into account
			// where the origin is with respect to the ISS module for a given hardware configuration.
			ensemble = new Assembly();
			ensemble->AddComponent( gripTracers );
			ensemble->AddComponent( graspTracers );
			ensemble->AddComponent( axes );
			ensemble->AddComponent( codas );
			ensemble->AddComponent( hmd );
			ensemble->AddComponent( hand );
			ensemble->AddComponent( chest );
			ensemble->AddComponent( tabletLocal );
			ensemble->AddComponent( renderer->head );
			ensemble->AddComponent( renderer->torso );

			// These other items are drawn in the ISS reference frame.
			iss = new Assembly();
			iss->AddComponent( racks );
			iss->AddComponent( presetCodas );
			iss->AddComponent( tabletSupine );
			iss->AddComponent( tabletSeated );


			LayoutVR();
			Render();

		}

		void LookAtFrom( Viewpoint *object, const Vector3 target, Vector3 from, const Vector3 up ) {

			static VectorsMixin vm;
			Matrix3x3 back;

			// Point a CODA at a location;
			vm.SubtractVectors( back[Z], from, target );
			vm.NormalizeVector( back[Z] );
			vm.ComputeCrossProduct( back[X], up, back[Z] );
			vm.ComputeCrossProduct( back[Y], back[Z], back[X] );
			object->SetPosition( from );
			object->SetOrientation( back );

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

			// If we get this call before the graphics items are created
			// then ignore and return.
			if ( ensemble == nullptr ) return;

			// Rotate and displace the ensemble of objects depending on
			// which hardware configuration was used to align the CODA_UNITS.
			if ( gripSeatedButton->Checked ) {
				ensemble->SetPosition( tabletPositionSeated );
				ensemble->SetOrientation( 0.0, 0.0, 180.0 );
				presetCoda[0]->PointYAt( tabletPositionSeated, vm->iVector );
				presetCoda[1]->PointYAt( tabletPositionSeated, vm->jVector );

			}
			else if ( gripSupineButton->Checked ) {
				ensemble->SetPosition( tabletPositionSupine );
				ensemble->SetOrientation( 0.0, 90.0, 0.0 );
				presetCoda[0]->PointYAt( tabletPositionSupine, vm->iVector );
				presetCoda[1]->PointYAt( tabletPositionSupine, vm->jVector );
			}
			else if ( graspSeatedButton->Checked ) {
				ensemble->SetPosition( 0.0, -400.0, 1800.0 );
				ensemble->SetOrientation( 0.0, 0.0, 0.0 );
				presetCoda[0]->PointYAt( chairPosition, vm->iVector );
				presetCoda[1]->PointYAt( chairPosition, vm->jVector );
			}
			else if ( graspFreefloatingButton->Checked ) {
				ensemble->SetPosition( 0.0, 0.0, 1500.0 );
				ensemble->SetOrientation( 180.0, - 26.0, 0.0 );
				presetCoda[0]->PointYAt( platePosition, vm->iVector );
				presetCoda[1]->PointYAt( platePosition, vm->jVector );
			}

			codas->enabled = !( presetCodas->enabled = nominalCodaCheckBox->Checked );
			hmd->enabled = hand->enabled = chest->enabled = markerStructureCheckBox->Checked;
			renderer->head->enabled = bodyCheckBox->Checked;
			renderer->torso->enabled = bodyCheckBox->Checked;
			axes->enabled = axesCheckBox->Checked;
			tabletLocal->enabled = tabletCheckBox->Checked;
			gripTracers->enabled = gripTracerCheckBox->Checked;
			graspTracers->enabled = graspTracerCheckBox->Checked;

			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				coda[unit]->proximity->enabled = presetCoda[unit]->proximity->enabled = proximityCheckBox->Checked;
				if ( threshold980RadioButton->Checked ) {
					coda[unit]->proximity->SetRadius( codaMinimumDistance980 );
					presetCoda[unit]->proximity->SetRadius( codaMinimumDistance980 );
				}
				else {
					coda[unit]->proximity->SetRadius( codaMinimumDistance992 );
					presetCoda[unit]->proximity->SetRadius( codaMinimumDistance992 );
				}
			}

			// This is a hack. The cone looks better for certain roll orientations.
			presetCoda[0]->proximity->SetAttitude( 0.0, 0.0, 0.0 );
			presetCoda[1]->proximity->SetAttitude( 0.0, 0.0, 0.0 );
			coda[0]->proximity->SetAttitude( 0.0, 0.0, 0.0 );
			coda[1]->proximity->SetAttitude( 0.0, 0.0, 0.0 );
			handEmissionCone->SetAttitude( 0.0, 0.0, 0.0 );
			RenderWindow( topWindow, topOrthoViewpoint );

			// This is a hack. The cone looks better for certain roll orientations.
			handEmissionCone->SetAttitude( 90.0, 0.0, 0.0 );
			coda[0]->proximity->SetAttitude( 90.0, 0.0, 0.0 );
			coda[1]->proximity->SetAttitude( 90.0, 0.0, 0.0 );
			presetCoda[0]->proximity->SetAttitude( 90.0, 0.0, 0.0 );
			presetCoda[1]->proximity->SetAttitude( 90.0, 0.0, 0.0 );
			RenderWindow( sideWindow, sideOrthoViewpoint );

			// Don't draw the proximity spheres or the line of sight in any other view.
			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				coda[unit]->proximity->Disable();
				coda[unit]->rays->Disable();
				presetCoda[unit]->proximity->Disable();
				presetCoda[unit]->rays->Disable();
			}

			// Another hack. Don't draw the cone when looking from either end.
			handEmissionCone->Disable();
			RenderWindow( endconeWindow, endconeViewpoint );
			RenderWindow( nodeWindow, nodeViewpoint );

			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				coda[unit]->bar->Disable();
				presetCoda[unit]->bar->Disable();
			}

			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				int other = abs( unit - 1 );
				gripTracerUnit[other]->Disable();
				graspTracerUnit[other]->Disable();

				if ( nominalCodaCheckBox->Checked ) {
					presetCoda[unit]->fov->Enable();
					racks->Disable();
					codaViewpoint[unit]->SetPosition( presetCoda[unit]->position );
					codaViewpoint[unit]->SetOrientation( presetCoda[unit]->orientation );
					RenderWindow( codaWindow[unit], codaViewpoint[unit] );
					racks->Enable();
					presetCoda[unit]->fov->Disable();
				}
				else {
					coda[unit]->fov->Enable();
					codaViewpoint[unit]->SetPosition( coda[unit]->position );
					codaViewpoint[unit]->SetOrientation( coda[unit]->orientation );
					RenderWindowLocal( codaWindow[unit], codaViewpoint[unit] );
					coda[unit]->fov->Disable();
				}
				gripTracerUnit[other]->Enable();
				graspTracerUnit[other]->Enable();

			}

			for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
				coda[unit]->bar->Enable();
				coda[unit]->rays->Enable();
				presetCoda[unit]->bar->Enable();
				presetCoda[unit]->rays->Enable();
			}

			Text = AlignmentFilename + " | " + TrajectoryFilename;

		}


	private: System::Void ReadSingleGripCodaAlignment( FILE *fp, int unit ) {

				 // Read one line of a GRIP alignment record, containing the alignment for one CODA.
				 // It is used below to read the contents of the file that contains both alignments.

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
				 coda[unit]->SetPosition( p );
				 coda[unit]->SetOrientation( R );
			 }

	private: System::Void LoadGripCodaAlignment( void ) {

				 // Read a file containing the CODA alignment transformations sent by GRIP.

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
				 AlignmentFilename = openGripAlignment->SafeFileName;
				 nominalCodaCheckBox->Checked = false;

			 }

	private: System::Void LoadGraspCodaAlignment ( void ) {

				 // Read a file containing the CODA alignment transformations stored by GRASP.

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

					 coda[0]->SetPosition( p );
					 vm->InvertMatrix( inv, r );
					 coda[0]->SetOrientation( inv );

					 // Serial Number
					 fgets( field, sizeof(field), fp );
					 // Offset
					 fscanf( fp, "Offset1=%lf,%lf,%lf\n", &p[X], &p[Y], &p[Z] );
					 // Rotation
					 fscanf( fp, "TransformX1=%lf,%lf,%lf\n", &r[X][X], &r[X][Y], &r[X][Z] );
					 fscanf( fp, "TransformY1=%lf,%lf,%lf\n", &r[Y][X], &r[Y][Y], &r[Y][Z] );
					 fscanf( fp, "TransformZ1=%lf,%lf,%lf\n", &r[Z][X], &r[Z][Y], &r[Z][Z] );

					 coda[1]->SetPosition( p );
					 vm->InvertMatrix( inv, r );
					 coda[1]->SetOrientation( inv );

					 fclose( fp );
				 }
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( path ) );
				 nominalCodaCheckBox->Checked = false;
				 AlignmentFilename = openGraspAlignment->SafeFileName;
			 }

	private: System::Void loadGripButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 LoadGripCodaAlignment();
				 // Guess which GRIP configuration we are in, based on the orientation of CODA 2.
				 if ( coda[1]->orientation[0][0] > 0 ) gripSupineButton->Checked = true;
				 else gripSeatedButton->Checked = true;
				 gripButton_Click( sender, e );
			 }
	private: System::Void loadGraspButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 LoadGraspCodaAlignment();
				 // Guess which GRASP configuration we are in, based on the orientation of CODA 2.
				 if ( coda[1]->orientation[0][0] > 0 ) graspSeatedButton->Checked = true;
				 else graspFreefloatingButton->Checked = true;
				 graspButton_Click( sender, e );
			 }

	private: System::Void Button_Click(System::Object^  sender, System::EventArgs^  e) {
				 Render();
			 }

	private: System::Void loadGripTrajectoryButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 openGripTrajectory->ShowDialog();
				 loadingLabel->Visible = true;
				 Refresh();

				 char *path[CODA_UNITS];
				 FILE *fp;
				 
				 path[0] = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( openGripTrajectory->FileName->Replace( "RT.csv", "1.csv" ) ).ToPointer();
				 path[1] = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( openGripTrajectory->FileName->Replace( "RT.csv", "2.csv" ) ).ToPointer();

				 for ( int unit = 0; unit < CODA_UNITS; unit++ ){
					 fp = fopen( path[unit], "r" );
					 if ( !fp ) {
						 fMessageBox( MB_OK, "GRIP/GRASP", "Error opening GRIP trajectory %s for read.", path );
					 }
					 else {
						 gripData->LoadGripDataUnit( fp, unit );
						 fclose( fp );
					 }
					 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( path[unit] ) );
				 }

				 for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
					 for ( int i = first_manipulandum_marker, j = 0; j < n_manipulandum_markers; i++, j++ ) {
						 manipulandumPath[unit][j]->Load( gripData->visibleMarker[unit][i], gripData->nVisibleSamples[unit][i] );
					 }
					 for ( int i = first_wrist_marker, j = 0; j < n_wrist_markers; i++, j++ ) {
						 wristPath[unit][j]->Load( gripData->visibleMarker[unit][i], gripData->nVisibleSamples[unit][i] );
					 }
					 for ( int i = first_tablet_marker, j = 0; j < n_tablet_markers; i++, j++ ) {
						 tabletPath[unit][j]->Load( gripData->visibleMarker[unit][i], gripData->nVisibleSamples[unit][i] );
					 }
				 }

				 TrajectoryFilename = openGripTrajectory->SafeFileName->Replace( "RT.csv", "" ) ;
				 gripTracerCheckBox->Checked = true;
				 graspTracerCheckBox->Checked = false;
				 loadingLabel->Visible = false;

			 }

	private: System::Void loadGraspTrajectoryButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 openGraspTrajectory->ShowDialog();
				 loadingLabel->Visible = true;
				 Refresh();

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

				 for ( int unit = 0; unit < CODA_UNITS; unit++ ) {
					 for ( int i = 0; i < hmd->nModelMarkers; i++ ) {
						 hmdPath[unit][i]->Load( graspData->visibleMarker[unit][hmd->modelMarker[i].id], graspData->nVisibleSamples[unit][hmd->modelMarker[i].id] );
					 }
					 for ( int i = 0; i < hand->nModelMarkers; i++ ) {
						 handPath[unit][i]->Load( graspData->visibleMarker[unit][hand->modelMarker[i].id], graspData->nVisibleSamples[unit][hand->modelMarker[i].id] );
					 }
					 for ( int i = 0; i < chest->nModelMarkers; i++ ) {
						 chestPath[unit][i]->Load( graspData->visibleMarker[unit][chest->modelMarker[i].id], graspData->nVisibleSamples[unit][chest->modelMarker[i].id] );
					 }
				 }
				 TrajectoryFilename = openGraspTrajectory->SafeFileName;
				 markerStructureCheckBox->Checked = false;
				 gripTracerCheckBox->Checked = false;
				 graspTracerCheckBox->Checked = true;
				 loadingLabel->Visible = false;

				 Render();

			 }
	private: System::Void LayoutVR( void ) {

				 // Need to undefine the X and Y macro defs to be able to access Point members.
#undef X
#undef Y
				 int spacer = 3;
				 int x = this->configurationGroupBox->Location.X + this->configurationGroupBox->Size.Width + spacer;
				 int upper = this->configurationGroupBox->Location.Y;
				 int lower = this->configurationGroupBox->Location.Y + this->topGroupBox->Size.Height + spacer;
				 int w = ( this->ClientSize.Width - x - 3 * spacer ) / 4 ;
				 int h = ( this->ClientSize.Height - upper - 2 * spacer ) / 2 ;
				 this->topGroupBox->Size = System::Drawing::Size( 2 * w, h );
				 this->sideGroupBox->Size = System::Drawing::Size( 2 * w, h );
				 this->coda0GroupBox->Size = System::Drawing::Size( w, h );
				 this->coda1GroupBox->Size = System::Drawing::Size( w, h );
				 this->endconeGroupBox->Size = System::Drawing::Size( w, h );
				 this->nodeGroupBox->Size = System::Drawing::Size( w, h );

				 Refresh();
				 this->topGroupBox->Location = System::Drawing::Point( x , upper );
				 this->sideGroupBox->Location = System::Drawing::Point( x, lower  );

				 x = x + this->topGroupBox->Size.Width + spacer;
				 this->coda0GroupBox->Location = System::Drawing::Point( x, upper );
				 this->endconeGroupBox->Location = System::Drawing::Point( x, lower );

				 x = x + this->coda0GroupBox->Size.Width + spacer;
				 this->coda1GroupBox->Location = System::Drawing::Point( x, upper );
				 this->nodeGroupBox->Location = System::Drawing::Point( x, lower );
			 }

	private: System::Void GraspMMIColumbusForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 Render();
			 }
	private: System::Void GraspMMIColumbusForm_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 LayoutVR();
			 }

#define BORDER 10

	private:

		//
		// When a group box is resized, resize the panel that it contains.
		// There should be a better way to do this.
		//
		System::Void topGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->topPanel->Size = System::Drawing::Size( this->topGroupBox->ClientSize.Width - 2 * BORDER,this->topGroupBox->ClientSize.Height - 3 * BORDER ) ;
		}
		System::Void sideGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->sidePanel->Size = System::Drawing::Size( this->sideGroupBox->ClientSize.Width - 2 * BORDER,this->sideGroupBox->ClientSize.Height  - 3 * BORDER ) ;
		}
		System::Void coda0GroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->coda0Panel->Size = System::Drawing::Size( this->coda0GroupBox->ClientSize.Width - 2 * BORDER,this->coda0GroupBox->ClientSize.Height - 3 * BORDER ) ;
		}
		System::Void endconeGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->endconePanel->Size = System::Drawing::Size( this->endconeGroupBox->ClientSize.Width - 2 * BORDER,this->endconeGroupBox->ClientSize.Height  - 3 * BORDER ) ;
		}
		System::Void coda1GroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->coda1Panel->Size = System::Drawing::Size( this->coda1GroupBox->ClientSize.Width - 2 * BORDER,this->coda1GroupBox->ClientSize.Height  - 3 * BORDER );
		}
		System::Void nodeGroupBox_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			this->nodePanel->Size = System::Drawing::Size( this->nodeGroupBox->ClientSize.Width - 2 * BORDER,this->nodeGroupBox->ClientSize.Height  - 3 * BORDER  ) ;
		}
		//
		// When the panels holding VR windows are resized, we need to resize the windows associated with the OpenGLWindows.
		// I really wish that we did not need to specify that here, but I haven't found the solution.
		//
		System::Void topPanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( topWindow->hWnd, HWND_TOP, 0, 0, this->topPanel->ClientSize.Width, this->topPanel->ClientSize.Height, SWP_NOMOVE );
		}
		System::Void sidePanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( sideWindow->hWnd, HWND_TOP, 0, 0, this->sidePanel->ClientSize.Width, this->sidePanel->ClientSize.Height, SWP_NOMOVE );
		}
		System::Void coda0Panel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( codaWindow[0]->hWnd, HWND_TOP, 0, 0, this->coda0Panel->ClientSize.Width, this->coda0Panel->ClientSize.Height, SWP_NOMOVE );
		}
		System::Void coda1Panel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( codaWindow[1]->hWnd, HWND_TOP, 0, 0, this->coda1Panel->ClientSize.Width, this->coda1Panel->ClientSize.Height, SWP_NOMOVE );
		}
		System::Void endconePanel_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( endconeWindow->hWnd, HWND_TOP, 0, 0, this->endconePanel->ClientSize.Width, this->endconePanel->ClientSize.Height, SWP_NOMOVE );
		}
		System::Void nodePanel_Resize(System::Object^  sender, System::EventArgs^  e) {
			SetWindowPos( nodeWindow->hWnd, HWND_TOP, 0, 0, this->nodePanel->ClientSize.Width, this->nodePanel->ClientSize.Height, SWP_NOMOVE );
		}


		//protected: virtual void WndProc(Message% m) override {
		//			   // The following is a real kludge.
		//			   // When the Form gets maximized and then restored, my VR windows don't full redraw. 
		//			   // I don't know why. But if the Form is minimized and then restored, they redraw
		//			   // just fine. So what I do here is that I intercept the restore event. If the current
		//			   // state is maximized, I return it to Normal, then minimize and then restore again.
		//			   // This makes my VR windows redraw after returning from maximized.

		//			   if( m.Msg == 0x0112 ) // WM_SYSCOMMAND
		//			   {
		//				   // Check your window state here
		//				   if (m.WParam == IntPtr( SC_RESTORE ) ) 
		//				   {
		//					   if ( WindowState == FormWindowState::Maximized ) {
		//						   WindowState = FormWindowState::Normal;
		//						   Form::WndProc(m);
		//						   WindowState = FormWindowState::Minimized;
		//						   Form::WndProc(m);
		//					   }
		//				   }
		//			   }
		//			   Form::WndProc(m);
		//		   }

	private: 

		Bitmap^ memoryImage;
		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

			Graphics^ myGraphics = this->CreateGraphics();
			memoryImage = gcnew Bitmap(this->Size.Width, this->Size.Height, myGraphics);
			Graphics^ memoryGraphics = Graphics::FromImage( memoryImage );
			memoryGraphics->CopyFromScreen( this->Location.X, this->Location.Y, 0, 0, this->Size );
			memoryImage->Save( screenshotFolderDialog->SelectedPath + "\\" + TrajectoryFilename + ".png" );

		}
		System::Void selectScreenshotButton_Click(System::Object^  sender, System::EventArgs^  e) {
			screenshotFolderDialog->ShowDialog();
		}

	private: System::Void gripButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 tabletCheckBox->Checked = true;
				 markerStructureCheckBox->Checked = false;
				 bodyCheckBox->Checked = false;
				 Button_Click( sender, e );

			 }

	private: System::Void graspButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 tabletCheckBox->Checked = false;
				 markerStructureCheckBox->Checked = true;
				 bodyCheckBox->Checked = true;
				 Button_Click( sender, e );

			 }


};

}

