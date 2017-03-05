#pragma once

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaLegacyPolledTracker.h"
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"
#include "../VectorsMixin/VectorsMixin.h"

#define ROD_RADIUS	5.0
#define ROD_LENGTH	800.0
#define WORLD_ZOOM 35.0
#define CODA_ZOOM 45.0
#define MARKER_SIZE 20.0
#define N_SAMPLES 500

char *iniFilename = "AlignToAxisMarkers.ini";

namespace AlignToAxisMarkersGUI {

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
	public ref class AlignToAxisMarkersForm : public System::Windows::Forms::Form
	{
	private:

		PsyPhy::OpenGLWindow					*fovWindow1, *fovWindow2, *worldWindow;
		PsyPhy::Viewpoint						*codaViewpoint, *worldViewpoint;
		PsyPhy::Assembly						*fovSweetSpot;
		PsyPhy::CodaLegacyPolledTracker			*coda;
		PsyPhy::Sphere							*ball;
		PsyPhy::Box								*block;

		PsyPhy::Cylinder						*xBar, *yBar, *zBar;
		PsyPhy::Patch							*xyPatch, *xzPatch, *yzPatch;




	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Panel^  worldPanel;
	private: System::Windows::Forms::GroupBox^  fovGroupBox1;
	private: System::Windows::Forms::Panel^  fovPanel1;
	private: System::Windows::Forms::GroupBox^  fovGroupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::ComboBox^  maxZ;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::ComboBox^  minZ;
	private: System::Windows::Forms::ComboBox^  origin;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::ComboBox^  maxY;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  minY;
	private: System::Windows::Forms::ComboBox^  maxX;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  minX;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  yz;
	private: System::Windows::Forms::Button^  goButton;

	private: System::Windows::Forms::RadioButton^  xz;
	private: System::Windows::Forms::RadioButton^  xy;
	private: System::Windows::Forms::CheckBox^  showYZ;
	private: System::Windows::Forms::CheckBox^  showXZ;
	private: System::Windows::Forms::CheckBox^  showXY;
	private: System::Windows::Forms::Panel^  fovPanel2;

	public:
		AlignToAxisMarkersForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AlignToAxisMarkersForm()
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
			this->fovGroupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->fovPanel1 = (gcnew System::Windows::Forms::Panel());
			this->fovGroupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->fovPanel2 = (gcnew System::Windows::Forms::Panel());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->worldPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->maxZ = (gcnew System::Windows::Forms::ComboBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->minZ = (gcnew System::Windows::Forms::ComboBox());
			this->origin = (gcnew System::Windows::Forms::ComboBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->maxY = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->minY = (gcnew System::Windows::Forms::ComboBox());
			this->maxX = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->minX = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->yz = (gcnew System::Windows::Forms::RadioButton());
			this->goButton = (gcnew System::Windows::Forms::Button());
			this->xz = (gcnew System::Windows::Forms::RadioButton());
			this->xy = (gcnew System::Windows::Forms::RadioButton());
			this->showXY = (gcnew System::Windows::Forms::CheckBox());
			this->showXZ = (gcnew System::Windows::Forms::CheckBox());
			this->showYZ = (gcnew System::Windows::Forms::CheckBox());
			this->fovGroupBox1->SuspendLayout();
			this->fovGroupBox2->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// fovGroupBox1
			// 
			this->fovGroupBox1->Controls->Add(this->fovPanel1);
			this->fovGroupBox1->Location = System::Drawing::Point(10, 12);
			this->fovGroupBox1->Name = L"fovGroupBox1";
			this->fovGroupBox1->Size = System::Drawing::Size(1100, 995);
			this->fovGroupBox1->TabIndex = 0;
			this->fovGroupBox1->TabStop = false;
			this->fovGroupBox1->Text = L"CODA 1";
			// 
			// fovPanel1
			// 
			this->fovPanel1->Location = System::Drawing::Point(5, 24);
			this->fovPanel1->Name = L"fovPanel1";
			this->fovPanel1->Size = System::Drawing::Size(1090, 965);
			this->fovPanel1->TabIndex = 0;
			// 
			// fovGroupBox2
			// 
			this->fovGroupBox2->Controls->Add(this->fovPanel2);
			this->fovGroupBox2->Location = System::Drawing::Point(1129, 12);
			this->fovGroupBox2->Name = L"fovGroupBox2";
			this->fovGroupBox2->Size = System::Drawing::Size(710, 540);
			this->fovGroupBox2->TabIndex = 1;
			this->fovGroupBox2->TabStop = false;
			this->fovGroupBox2->Text = L"CODA 2";
			// 
			// fovPanel2
			// 
			this->fovPanel2->Location = System::Drawing::Point(13, 27);
			this->fovPanel2->Name = L"fovPanel2";
			this->fovPanel2->Size = System::Drawing::Size(684, 496);
			this->fovPanel2->TabIndex = 0;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->worldPanel);
			this->groupBox2->Location = System::Drawing::Point(1129, 554);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(498, 453);
			this->groupBox2->TabIndex = 18;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"World Coordinates";
			// 
			// worldPanel
			// 
			this->worldPanel->Location = System::Drawing::Point(5, 25);
			this->worldPanel->Name = L"worldPanel";
			this->worldPanel->Size = System::Drawing::Size(488, 422);
			this->worldPanel->TabIndex = 0;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->groupBox4);
			this->groupBox5->Controls->Add(this->groupBox3);
			this->groupBox5->Controls->Add(this->groupBox1);
			this->groupBox5->Location = System::Drawing::Point(1633, 554);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(206, 453);
			this->groupBox5->TabIndex = 20;
			this->groupBox5->TabStop = false;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->showYZ);
			this->groupBox4->Controls->Add(this->showXZ);
			this->groupBox4->Controls->Add(this->showXY);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(6, 222);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(194, 70);
			this->groupBox4->TabIndex = 21;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Show Plane";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->maxZ);
			this->groupBox3->Controls->Add(this->label6);
			this->groupBox3->Controls->Add(this->label7);
			this->groupBox3->Controls->Add(this->minZ);
			this->groupBox3->Controls->Add(this->origin);
			this->groupBox3->Controls->Add(this->label5);
			this->groupBox3->Controls->Add(this->maxY);
			this->groupBox3->Controls->Add(this->label3);
			this->groupBox3->Controls->Add(this->label4);
			this->groupBox3->Controls->Add(this->minY);
			this->groupBox3->Controls->Add(this->maxX);
			this->groupBox3->Controls->Add(this->label2);
			this->groupBox3->Controls->Add(this->label1);
			this->groupBox3->Controls->Add(this->minX);
			this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox3->Location = System::Drawing::Point(6, 10);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(193, 206);
			this->groupBox3->TabIndex = 22;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Axis Markers";
			// 
			// maxZ
			// 
			this->maxZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->maxZ->FormattingEnabled = true;
			this->maxZ->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->maxZ->Location = System::Drawing::Point(136, 155);
			this->maxZ->Name = L"maxZ";
			this->maxZ->Size = System::Drawing::Size(53, 33);
			this->maxZ->TabIndex = 31;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(104, 158);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(30, 25);
			this->label6->TabIndex = 30;
			this->label6->Text = L"to";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(5, 158);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(32, 25);
			this->label7->TabIndex = 29;
			this->label7->Text = L"Z:";
			// 
			// minZ
			// 
			this->minZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->minZ->FormattingEnabled = true;
			this->minZ->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->minZ->Location = System::Drawing::Point(46, 155);
			this->minZ->Name = L"minZ";
			this->minZ->Size = System::Drawing::Size(53, 33);
			this->minZ->TabIndex = 28;
			// 
			// origin
			// 
			this->origin->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->origin->FormattingEnabled = true;
			this->origin->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->origin->Location = System::Drawing::Point(107, 25);
			this->origin->Name = L"origin";
			this->origin->Size = System::Drawing::Size(53, 33);
			this->origin->TabIndex = 27;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->ForeColor = System::Drawing::SystemColors::ControlText;
			this->label5->Location = System::Drawing::Point(24, 28);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(77, 25);
			this->label5->TabIndex = 26;
			this->label5->Text = L"Origin:";
			// 
			// maxY
			// 
			this->maxY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->maxY->FormattingEnabled = true;
			this->maxY->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->maxY->Location = System::Drawing::Point(136, 113);
			this->maxY->Name = L"maxY";
			this->maxY->Size = System::Drawing::Size(53, 33);
			this->maxY->TabIndex = 25;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(104, 116);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(30, 25);
			this->label3->TabIndex = 24;
			this->label3->Text = L"to";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(5, 116);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(33, 25);
			this->label4->TabIndex = 23;
			this->label4->Text = L"Y:";
			// 
			// minY
			// 
			this->minY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->minY->FormattingEnabled = true;
			this->minY->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->minY->Location = System::Drawing::Point(46, 113);
			this->minY->Name = L"minY";
			this->minY->Size = System::Drawing::Size(53, 33);
			this->minY->TabIndex = 22;
			// 
			// maxX
			// 
			this->maxX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->maxX->FormattingEnabled = true;
			this->maxX->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->maxX->Location = System::Drawing::Point(136, 68);
			this->maxX->Name = L"maxX";
			this->maxX->Size = System::Drawing::Size(53, 33);
			this->maxX->TabIndex = 21;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(104, 71);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(30, 25);
			this->label2->TabIndex = 20;
			this->label2->Text = L"to";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(5, 71);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(34, 25);
			this->label1->TabIndex = 19;
			this->label1->Text = L"X:";
			// 
			// minX
			// 
			this->minX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->minX->FormattingEnabled = true;
			this->minX->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1", L"2", L"3", L"4", L"5", L"6"});
			this->minX->Location = System::Drawing::Point(46, 68);
			this->minX->Name = L"minX";
			this->minX->Size = System::Drawing::Size(53, 33);
			this->minX->TabIndex = 18;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->yz);
			this->groupBox1->Controls->Add(this->goButton);
			this->groupBox1->Controls->Add(this->xz);
			this->groupBox1->Controls->Add(this->xy);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(6, 301);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(194, 148);
			this->groupBox1->TabIndex = 20;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Alignment Vectors";
			// 
			// yz
			// 
			this->yz->AutoSize = true;
			this->yz->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->yz->Location = System::Drawing::Point(127, 35);
			this->yz->Name = L"yz";
			this->yz->Size = System::Drawing::Size(58, 29);
			this->yz->TabIndex = 17;
			this->yz->TabStop = true;
			this->yz->Text = L"YZ";
			this->yz->UseVisualStyleBackColor = true;
			// 
			// goButton
			// 
			this->goButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->goButton->Location = System::Drawing::Point(36, 75);
			this->goButton->Name = L"goButton";
			this->goButton->Size = System::Drawing::Size(131, 55);
			this->goButton->TabIndex = 16;
			this->goButton->Text = L"GO";
			this->goButton->UseVisualStyleBackColor = true;
			this->goButton->Click += gcnew System::EventHandler(this, &AlignToAxisMarkersForm::goButton_Click);
			// 
			// xz
			// 
			this->xz->AutoSize = true;
			this->xz->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->xz->Location = System::Drawing::Point(68, 35);
			this->xz->Name = L"xz";
			this->xz->Size = System::Drawing::Size(59, 29);
			this->xz->TabIndex = 15;
			this->xz->TabStop = true;
			this->xz->Text = L"XZ";
			this->xz->UseVisualStyleBackColor = true;
			// 
			// xy
			// 
			this->xy->AutoSize = true;
			this->xy->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->xy->Location = System::Drawing::Point(10, 35);
			this->xy->Name = L"xy";
			this->xy->Size = System::Drawing::Size(60, 29);
			this->xy->TabIndex = 14;
			this->xy->TabStop = true;
			this->xy->Text = L"XY";
			this->xy->UseVisualStyleBackColor = true;
			// 
			// showXY
			// 
			this->showXY->AutoSize = true;
			this->showXY->Location = System::Drawing::Point(11, 30);
			this->showXY->Name = L"showXY";
			this->showXY->Size = System::Drawing::Size(52, 24);
			this->showXY->TabIndex = 0;
			this->showXY->Text = L"XY";
			this->showXY->UseVisualStyleBackColor = true;
			// 
			// showXZ
			// 
			this->showXZ->AutoSize = true;
			this->showXZ->Location = System::Drawing::Point(71, 30);
			this->showXZ->Name = L"showXZ";
			this->showXZ->Size = System::Drawing::Size(51, 24);
			this->showXZ->TabIndex = 1;
			this->showXZ->Text = L"XZ";
			this->showXZ->UseVisualStyleBackColor = true;
			// 
			// showYZ
			// 
			this->showYZ->AutoSize = true;
			this->showYZ->Location = System::Drawing::Point(131, 30);
			this->showYZ->Name = L"showYZ";
			this->showYZ->Size = System::Drawing::Size(50, 24);
			this->showYZ->TabIndex = 2;
			this->showYZ->Text = L"YZ";
			this->showYZ->UseVisualStyleBackColor = true;
			// 
			// AlignToAxisMarkersForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1848, 1023);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->fovGroupBox2);
			this->Controls->Add(this->fovGroupBox1);
			this->Name = L"AlignToAxisMarkersForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"AlgnToAxisMarkersGUI";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &AlignToAxisMarkersForm::AlignToAxisMarkersForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &AlignToAxisMarkersForm::AlignToAxisMarkersForm_Shown);
			this->fovGroupBox1->ResumeLayout(false);
			this->fovGroupBox2->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: int SelectMarkerColor( int mrk ) {
				 if ( mrk == minX->SelectedIndex || mrk == maxX->SelectedIndex ) {
					 if ( mrk == minY->SelectedIndex || mrk == maxY->SelectedIndex ) return ( YELLOW );
					 else if ( mrk == minZ->SelectedIndex || mrk == maxZ->SelectedIndex ) return ( MAGENTA );
					 else return( RED );
				 }
				 else if ( mrk == minY->SelectedIndex || mrk == maxY->SelectedIndex ) {
					 if ( mrk == minZ->SelectedIndex || mrk == maxZ->SelectedIndex ) return ( CYAN );
					 else return( GREEN );
				 }
				 else if ( mrk == minZ->SelectedIndex || mrk == maxZ->SelectedIndex ) return( BLUE );
				 else if ( mrk == origin->SelectedIndex ) return( ORANGE );
				 else return( GRAY );
			 }

	private: void DrawMarkers( int unit, bool intrinsic ) {
				 MarkerFrame codaFrame;
				 if (intrinsic ) coda->GetCurrentMarkerFrameIntrinsic( codaFrame, unit );
				 else coda->GetCurrentMarkerFrameUnit( codaFrame, unit );
				 for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					 if ( codaFrame.marker[mrk].visibility ) {
						 // Draw the negative of each axis as a cube.
						 if ( mrk == maxX->SelectedIndex ||  mrk == maxY->SelectedIndex ||  mrk == maxZ->SelectedIndex ) {
							 block->SetColor( Translucid( SelectMarkerColor( mrk ) ) );
							 block->SetPosition( codaFrame.marker[mrk].position );
							 block->Draw();
						 }
						 else {
							 if ( mrk == origin->SelectedIndex ) ball->SetRadius( MARKER_SIZE * 1.25 );
							 else ball->SetRadius( MARKER_SIZE );
							 ball->SetColor( Translucid( SelectMarkerColor( mrk ) ) );
							 ball->SetPosition( codaFrame.marker[mrk].position );
							 ball->Draw();
						 }
					 }
				 }
			 }

	private: void Render ( void ) {

				 // Update the 3D view of the alignment object.
				 fovWindow2->Activate();
				 fovWindow2->Clear( 0.0, 0.0, 0.0 );
				 codaViewpoint->Apply( fovWindow2, CYCLOPS );
				 if ( coda->nUnits > 1 ) DrawMarkers( 1, true );
				 // fovSweetSpot->Draw();
				 fovWindow2->Swap();

				 fovWindow1->Activate();
				 fovWindow1->Clear( 0.0, 0.0, 0.0 );
				 codaViewpoint->Apply( fovWindow1, CYCLOPS );
				 DrawMarkers( 0, true );
				 // fovSweetSpot->Draw();
				 fovWindow1->Swap();

				 worldWindow->Activate();
				 worldWindow->Clear( 0.0, 0.0, 0.0 );
				 worldViewpoint->Apply( worldWindow, CYCLOPS );

				 if (showXY->Checked) xyPatch->Draw();
				 if (showXZ->Checked) xzPatch->Draw();
				 if (showYZ->Checked) yzPatch->Draw();

				 xBar->Draw();
				 yBar->Draw();
				 zBar->Draw();
				 DrawMarkers( 0, false );
				 if ( coda->nUnits > 1 ) DrawMarkers( 1, false );
				 // fovSweetSpot->Draw();
				 worldWindow->Swap();

			 }

	private:

		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			Render();
		}
		void CreateRefreshTimer( int interval ) {
			refreshTimer = gcnew( System::Windows::Forms::Timer );
			refreshTimer->Interval = interval;
			refreshTimer->Tick += gcnew EventHandler( this, &AlignToAxisMarkersForm::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			refreshTimer->Start();
		}
		void StopRefreshTimer( void ) {
			refreshTimer->Stop();
		}		

	private: 

		void ReadMarkerAssignments( void ) {

			FILE *fp = fopen( iniFilename, "r" );
			int o = -1, xm = -1, xp = -1, ym = -1, yp = -1, zm = -1, zp = -1;

			if ( !fp ) {
				fMessageBox( MB_OK, "AlignToAxisMarkers", "Error opening %s for read.", iniFilename );
			}
			else {		
				fscanf( fp, "%d   %d %d   %d %d   %d %d", &o, &xm, &xp, &ym, &yp, &zm, &zp );
				origin->SelectedIndex = o;
				minX->SelectedIndex = xm;
				maxX->SelectedIndex = xp;
				minY->SelectedIndex = ym;
				maxY->SelectedIndex = yp;
				minZ->SelectedIndex = zm;
				maxZ->SelectedIndex = zp;
				fclose( fp );
			}
		}

		void WriteMarkerAssignments( void ) {

			FILE *fp = fopen( iniFilename, "w" );
			if ( !fp ) {
				fMessageBox( MB_OK, "AlignToAxisMarkers", "Error opening %s for write.", iniFilename );
			}
			else {
				fprintf( fp, "%d   %d %d   %d %d   %d %d\n", 
					origin->SelectedIndex ,
					minX->SelectedIndex ,
					maxX->SelectedIndex,
					minY->SelectedIndex,
					maxY->SelectedIndex,
					minZ->SelectedIndex,
					maxZ->SelectedIndex );
				fclose( fp );
			}
		}

	private: System::Void AlignToAxisMarkersForm_Shown(System::Object^  sender, System::EventArgs^  e) {

				 Enabled = false;

				 // Create windows and viewpoints to show what the CODA units are seeing.
				 fovWindow1 = CreateOpenGLWindowInForm( fovPanel1 );
				 fovWindow2 = CreateOpenGLWindowInForm( fovPanel2, fovWindow1->hRC );
				 worldWindow = CreateOpenGLWindowInForm( worldPanel, fovWindow1->hRC );

				 // Create a virtual viewing portal into the workspace to show where we would like
				 // to have the markers for the alignment.
				 fovSweetSpot = new PsyPhy::Assembly();
				 Hole *hole = new Hole( 500.0,  2000.0, 2000.0 );
				 hole->SetColor( 0.15, 0.0, 0.0, 0.75 );	
				 hole->SetPosition( 0.0, 0.0, -100.0 );
				 fovSweetSpot->AddComponent( hole );
				 hole = new Hole( 100.0,  2000.0, 2000.0 );
				 hole->SetColor( 0.0, 0.0, 0.5, 0.5 );	
				 fovSweetSpot->AddComponent( hole );
				 // Crosshairs
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

				 // Create a viewpoint that looks from the origin along the positive Y axis.
				 // This is the view from the CODA in intrinsic coordinates.
				 codaViewpoint = new Viewpoint( 6.0, CODA_ZOOM, 10.0, 10000.0);
				 // CODA cx1 intrinsic coordinate system has Y for the depth dimension.
				 // So we rotate the viewpoint to look down the Y axis.
				 codaViewpoint->SetOrientation( 0.0, - 90.0, 0.0 );
				 // CODA cx1 intrinsic origin is at the center of camera A, which is offset
				 // about 30 cm from it's center. So we shift the viewpoint so that when the
				 // markers are in the center of the field of view, they are centered in the
				 // 3D volume displayed on the screen.
				 codaViewpoint->SetPosition( -300.0, 0.0, 0.0 );

				 // Look down on the world coordinate system.
				 worldViewpoint = new Viewpoint( 6.0, 35.0, 10.0, 10000.0 );
				 worldViewpoint->SetPosition( 500.0, 2000.0, 3000.0 );
				 worldViewpoint->SetOrientation( 0.0, WORLD_ZOOM, -5.0 );

				 // Initialize the state of the GL graphics engine.
				 // Some of this will be overridden by the object renderer.
				 glUsefulInitializeDefault();
				 glUsefulDefaultSpecularLighting( 0.75 );

				 ball = new Sphere( MARKER_SIZE );
				 block = new Box( MARKER_SIZE, MARKER_SIZE, MARKER_SIZE );

				 xBar = new Cylinder( ROD_RADIUS, ROD_RADIUS, ROD_LENGTH );
				 xBar->SetColor( RED );
				 xBar->SetOrientation( 0.0, 0.0, -90.0 );
				 xBar->SetOffset( 0.0, 0.0, ROD_LENGTH / 2.0 );
				 yBar = new Cylinder( ROD_RADIUS, ROD_RADIUS, ROD_LENGTH );
				 yBar->SetColor( GREEN );
				 yBar->SetOrientation( 0.0, 90.0, 0.0 );
				 yBar->SetOffset( 0.0, 0.0, ROD_LENGTH / 2.0 );
				 zBar = new Cylinder( ROD_RADIUS, ROD_RADIUS, ROD_LENGTH );
				 zBar->SetColor( BLUE );
				 zBar->SetOffset( 0.0, 0.0, ROD_LENGTH / 2.0 );

				 xyPatch = new Patch( ROD_LENGTH, ROD_LENGTH );
				 xyPatch->SetOffset( ROD_LENGTH / 2.0, ROD_LENGTH / 2.0, 0.0 );
				 xyPatch->SetColor( 1.0, 1.0, 1.0, 0.1 );

				 xzPatch = new Patch( ROD_LENGTH, ROD_LENGTH );
				 xzPatch->SetOffset( ROD_LENGTH / 2.0, ROD_LENGTH / 2.0, 0.0 );
				 xzPatch->SetOrientation( 0.0, -90.0, 0.0 );
				 xzPatch->SetColor( 1.0, 1.0, 1.0, 0.1 );

				 yzPatch = new Patch( ROD_LENGTH, ROD_LENGTH );
				 yzPatch->SetOffset( ROD_LENGTH / 2.0, ROD_LENGTH / 2.0, 0.0 );
				 yzPatch->SetOrientation( 0.0, 0.0, 90.0 );
				 yzPatch->SetColor( 1.0, 1.0, 1.0, 0.1 );

				 // Drop boxes to select which markers define each axis.
				 origin->Items->Clear();
				 minX->Items->Clear();
				 maxX->Items->Clear();
				 minY->Items->Clear();
				 maxY->Items->Clear();
				 minZ->Items->Clear();
				 maxZ->Items->Clear();
				 for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					 origin->Items->Add( mrk );
					 minX->Items->Add( mrk );
					 maxX->Items->Add( mrk );
					 minY->Items->Add( mrk );
					 maxY->Items->Add( mrk );
					 minZ->Items->Add( mrk );
					 maxZ->Items->Add( mrk );
				 }

				 ReadMarkerAssignments();

				 // Create the CODA tracker.
				 coda = new CodaLegacyPolledTracker();
				 // Start up the CODA tracker.
				 coda->Initialize();

				 CreateRefreshTimer( 300 );
				 StartRefreshTimer();

				 Enabled = true;

			 }

	private: System::Void AlignToAxisMarkersForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 Enabled = false;
				 WriteMarkerAssignments();
				 coda->Quit();
			 }

	private: System::Void goButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 static MarkerFrame codaFrame[N_SAMPLES];
				 MarkerFrame averageFrame;

				 Vector3	i_vector, j_vector, k_vector;
				 Vector3	x_vector, y_vector, z_vector;
				 Vector3	offset;
				 Matrix3x3	rotation;

				 Enabled = false;
				 if ( xy->Checked && ( minX->SelectedIndex < 0 || maxX->SelectedIndex < 0 || minY->SelectedIndex < 0 || maxY->SelectedIndex < 0 ) ) {
					fMessageBox( MB_OK | MB_ICONEXCLAMATION, "AlignToAxisMarkersGUI", "X and Y axis markers not defined." );
				 }
				 else if ( xz->Checked && (minX->SelectedIndex < 0 || maxX->SelectedIndex < 0 || minZ->SelectedIndex < 0 || maxZ->SelectedIndex < 0 )) {
					fMessageBox( MB_OK | MB_ICONEXCLAMATION, "AlignToAxisMarkersGUI", "X and Z axis markers not defined." );
				 }
				 else if ( xy->Checked ) {
					 for ( int unit = 0; unit < coda->nUnits; unit++ ) {
						 for ( int sample = 0; sample < N_SAMPLES; sample++ ) {
							 coda->GetCurrentMarkerFrameIntrinsic( codaFrame[sample], unit );
							 Refresh();
							 Sleep( 5 );
						 }
						 coda->ComputeAverageMarkerFrame( averageFrame, codaFrame, N_SAMPLES );
						 coda->SubtractVectors( x_vector, averageFrame.marker[maxX->SelectedIndex].position, averageFrame.marker[minX->SelectedIndex].position ); 
						 coda->SubtractVectors( y_vector, averageFrame.marker[maxY->SelectedIndex].position, averageFrame.marker[minY->SelectedIndex].position ); 
						 coda->SubtractVectors( z_vector, averageFrame.marker[maxZ->SelectedIndex].position, averageFrame.marker[minZ->SelectedIndex].position ); 
						 if ( xy->Checked ) {
							 coda->CopyVector( i_vector, x_vector );
							 coda->NormalizeVector( i_vector );
							 coda->ComputeCrossProduct( k_vector, i_vector, y_vector );
							 coda->NormalizeVector( k_vector );
							 coda->ComputeCrossProduct( j_vector, k_vector, i_vector );
							 coda->NormalizeVector( j_vector );
							 for ( int i = 0; i < 3; i++ ) {
								 rotation[i][X] = i_vector[i];
								 rotation[i][Y] = j_vector[i];
								 rotation[i][Z] = k_vector[i];
							 }
							 coda->MultiplyVector( offset, averageFrame.marker[origin->SelectedIndex].position, rotation );
							 coda->ScaleVector( offset, offset, -1.0 );
							 coda->SetUnitTransform( unit, offset, rotation );
						 }
					 }
				 }
				 else if ( xz->Checked ) fMessageBox( MB_OK | MB_ICONEXCLAMATION, "AlignToAxisMarkersGUI", "Alignment to XZ plane not yet implemented." );
				 else if ( yz->Checked ) fMessageBox( MB_OK | MB_ICONEXCLAMATION, "AlignToAxisMarkersGUI", "Alignment to YZ plane not yet implemented." );
				 else fMessageBox( MB_OK | MB_ICONEXCLAMATION, "AlignToAxisMarkersGUI", "Please select which axes to use for the alignment.\nNB: Only XY is currently implemented." );
				 Enabled = true;

			 }
};
	}

