#pragma once

namespace GraspGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// The main form for the Grasp GUI.
	/// </summary>
	public ref class GraspDesktop : public System::Windows::Forms::Form
	{
	public:
		GraspDesktop(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GraspDesktop()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  MainLabel;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Panel^  panel3;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel5;
	private: System::Windows::Forms::GroupBox^  NavigatorGroupBox;
	private: System::Windows::Forms::Label^  WelcomeLabel;
	private: System::Windows::Forms::Button^  LogonNext;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::GroupBox^  SubjectGroupBox;
	private: System::Windows::Forms::ListBox^  subjectListBox;
	private: System::Windows::Forms::GroupBox^  TaskGroupBox;
	private: System::Windows::Forms::ListBox^  taskListBox;
	private: System::Windows::Forms::GroupBox^  ProtocolGroupBox;
	private: System::Windows::Forms::ListBox^  protocolListBox;
	private: System::Windows::Forms::RadioButton^  floatingRadioButton;
	private: System::Windows::Forms::RadioButton^  seatedRadioButton;
	private: System::Windows::Forms::GroupBox^  ProgressBox;
	private: System::Windows::Forms::ListBox^  stepListBox;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::Panel^  columbusPanel;

	private: System::Windows::Forms::Panel^  panel8;
	private: System::Windows::Forms::Panel^  panel7;
	private: System::Windows::Forms::Panel^  panel6;

	protected: 

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
			this->MainLabel = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->columbusPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->panel8 = (gcnew System::Windows::Forms::Panel());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->panel7 = (gcnew System::Windows::Forms::Panel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->panel6 = (gcnew System::Windows::Forms::Panel());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->NavigatorGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->ProgressBox = (gcnew System::Windows::Forms::GroupBox());
			this->stepListBox = (gcnew System::Windows::Forms::ListBox());
			this->TaskGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->taskListBox = (gcnew System::Windows::Forms::ListBox());
			this->ProtocolGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->floatingRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->seatedRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->protocolListBox = (gcnew System::Windows::Forms::ListBox());
			this->SubjectGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->subjectListBox = (gcnew System::Windows::Forms::ListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->LogonNext = (gcnew System::Windows::Forms::Button());
			this->WelcomeLabel = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->NavigatorGroupBox->SuspendLayout();
			this->ProgressBox->SuspendLayout();
			this->TaskGroupBox->SuspendLayout();
			this->ProtocolGroupBox->SuspendLayout();
			this->SubjectGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// MainLabel
			// 
			this->MainLabel->AutoSize = true;
			this->MainLabel->Font = (gcnew System::Drawing::Font(L"Vrinda", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->MainLabel->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->MainLabel->Location = System::Drawing::Point(355, 4);
			this->MainLabel->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->MainLabel->Name = L"MainLabel";
			this->MainLabel->Size = System::Drawing::Size(0, 32);
			this->MainLabel->TabIndex = 0;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->groupBox8);
			this->groupBox1->Controls->Add(this->panel5);
			this->groupBox1->Controls->Add(this->MainLabel);
			this->groupBox1->Location = System::Drawing::Point(20, 20);
			this->groupBox1->Margin = System::Windows::Forms::Padding(4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(4);
			this->groupBox1->Size = System::Drawing::Size(977, 577);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"HMD Display";
			// 
			// groupBox8
			// 
			this->groupBox8->Location = System::Drawing::Point(8, 526);
			this->groupBox8->Margin = System::Windows::Forms::Padding(4);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Padding = System::Windows::Forms::Padding(4);
			this->groupBox8->Size = System::Drawing::Size(960, 47);
			this->groupBox8->TabIndex = 6;
			this->groupBox8->TabStop = false;
			// 
			// panel5
			// 
			this->panel5->Location = System::Drawing::Point(8, 27);
			this->panel5->Margin = System::Windows::Forms::Padding(4);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(960, 492);
			this->panel5->TabIndex = 1;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->groupBox7);
			this->groupBox2->Controls->Add(this->groupBox6);
			this->groupBox2->Controls->Add(this->groupBox5);
			this->groupBox2->Controls->Add(this->groupBox4);
			this->groupBox2->Controls->Add(this->groupBox3);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Location = System::Drawing::Point(20, 604);
			this->groupBox2->Margin = System::Windows::Forms::Padding(4);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(4);
			this->groupBox2->Size = System::Drawing::Size(977, 411);
			this->groupBox2->TabIndex = 4;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Tracking Status";
			// 
			// groupBox7
			// 
			this->groupBox7->Location = System::Drawing::Point(8, 353);
			this->groupBox7->Margin = System::Windows::Forms::Padding(4);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Padding = System::Windows::Forms::Padding(4);
			this->groupBox7->Size = System::Drawing::Size(960, 47);
			this->groupBox7->TabIndex = 5;
			this->groupBox7->TabStop = false;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->columbusPanel);
			this->groupBox6->Location = System::Drawing::Point(8, 23);
			this->groupBox6->Margin = System::Windows::Forms::Padding(4);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Padding = System::Windows::Forms::Padding(4);
			this->groupBox6->Size = System::Drawing::Size(699, 323);
			this->groupBox6->TabIndex = 4;
			this->groupBox6->TabStop = false;
			// 
			// columbusPanel
			// 
			this->columbusPanel->Location = System::Drawing::Point(12, 21);
			this->columbusPanel->Margin = System::Windows::Forms::Padding(4);
			this->columbusPanel->Name = L"columbusPanel";
			this->columbusPanel->Size = System::Drawing::Size(679, 287);
			this->columbusPanel->TabIndex = 5;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->panel8);
			this->groupBox5->Controls->Add(this->panel3);
			this->groupBox5->Location = System::Drawing::Point(715, 243);
			this->groupBox5->Margin = System::Windows::Forms::Padding(4);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Padding = System::Windows::Forms::Padding(4);
			this->groupBox5->Size = System::Drawing::Size(253, 102);
			this->groupBox5->TabIndex = 3;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Chest";
			// 
			// panel8
			// 
			this->panel8->Location = System::Drawing::Point(131, 21);
			this->panel8->Margin = System::Windows::Forms::Padding(4);
			this->panel8->Name = L"panel8";
			this->panel8->Size = System::Drawing::Size(107, 73);
			this->panel8->TabIndex = 1;
			// 
			// panel3
			// 
			this->panel3->Location = System::Drawing::Point(16, 21);
			this->panel3->Margin = System::Windows::Forms::Padding(4);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(107, 73);
			this->panel3->TabIndex = 0;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->panel7);
			this->groupBox4->Controls->Add(this->panel2);
			this->groupBox4->Location = System::Drawing::Point(715, 133);
			this->groupBox4->Margin = System::Windows::Forms::Padding(4);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Padding = System::Windows::Forms::Padding(4);
			this->groupBox4->Size = System::Drawing::Size(253, 102);
			this->groupBox4->TabIndex = 2;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Tool";
			// 
			// panel7
			// 
			this->panel7->Location = System::Drawing::Point(131, 22);
			this->panel7->Margin = System::Windows::Forms::Padding(4);
			this->panel7->Name = L"panel7";
			this->panel7->Size = System::Drawing::Size(107, 73);
			this->panel7->TabIndex = 1;
			// 
			// panel2
			// 
			this->panel2->Location = System::Drawing::Point(16, 22);
			this->panel2->Margin = System::Windows::Forms::Padding(4);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(107, 73);
			this->panel2->TabIndex = 0;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->panel6);
			this->groupBox3->Controls->Add(this->panel1);
			this->groupBox3->Location = System::Drawing::Point(715, 23);
			this->groupBox3->Margin = System::Windows::Forms::Padding(4);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Padding = System::Windows::Forms::Padding(4);
			this->groupBox3->Size = System::Drawing::Size(253, 102);
			this->groupBox3->TabIndex = 1;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"HMD";
			// 
			// panel6
			// 
			this->panel6->Location = System::Drawing::Point(131, 21);
			this->panel6->Margin = System::Windows::Forms::Padding(4);
			this->panel6->Name = L"panel6";
			this->panel6->Size = System::Drawing::Size(107, 73);
			this->panel6->TabIndex = 2;
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(16, 21);
			this->panel1->Margin = System::Windows::Forms::Padding(4);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(107, 73);
			this->panel1->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Vrinda", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->label1->Location = System::Drawing::Point(355, 4);
			this->label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 32);
			this->label1->TabIndex = 0;
			// 
			// NavigatorGroupBox
			// 
			this->NavigatorGroupBox->Controls->Add(this->ProgressBox);
			this->NavigatorGroupBox->Controls->Add(this->TaskGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->ProtocolGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->SubjectGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->button1);
			this->NavigatorGroupBox->Controls->Add(this->LogonNext);
			this->NavigatorGroupBox->Controls->Add(this->WelcomeLabel);
			this->NavigatorGroupBox->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->NavigatorGroupBox->Location = System::Drawing::Point(1005, 23);
			this->NavigatorGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Name = L"NavigatorGroupBox";
			this->NavigatorGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Size = System::Drawing::Size(561, 992);
			this->NavigatorGroupBox->TabIndex = 5;
			this->NavigatorGroupBox->TabStop = false;
			this->NavigatorGroupBox->Text = L"Navigator";
			// 
			// ProgressBox
			// 
			this->ProgressBox->Controls->Add(this->stepListBox);
			this->ProgressBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ProgressBox->Location = System::Drawing::Point(13, 693);
			this->ProgressBox->Margin = System::Windows::Forms::Padding(4);
			this->ProgressBox->Name = L"ProgressBox";
			this->ProgressBox->Padding = System::Windows::Forms::Padding(4);
			this->ProgressBox->Size = System::Drawing::Size(531, 231);
			this->ProgressBox->TabIndex = 8;
			this->ProgressBox->TabStop = false;
			this->ProgressBox->Text = L"Task Progress";
			// 
			// stepListBox
			// 
			this->stepListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepListBox->FormattingEnabled = true;
			this->stepListBox->ItemHeight = 29;
			this->stepListBox->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Installation", L"Tracker Alignment", L"Readiness Check", 
				L"Block 1", L"Block 2", L"Block 3", L"Block 4", L"Pause", L"Block 5", L"Block 6", L"Block 7", L"Block 8", L"(end of protocol)"});
			this->stepListBox->Location = System::Drawing::Point(11, 34);
			this->stepListBox->Margin = System::Windows::Forms::Padding(4);
			this->stepListBox->Name = L"stepListBox";
			this->stepListBox->Size = System::Drawing::Size(507, 178);
			this->stepListBox->TabIndex = 5;
			// 
			// TaskGroupBox
			// 
			this->TaskGroupBox->Controls->Add(this->taskListBox);
			this->TaskGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TaskGroupBox->Location = System::Drawing::Point(13, 419);
			this->TaskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Name = L"TaskGroupBox";
			this->TaskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Size = System::Drawing::Size(536, 256);
			this->TaskGroupBox->TabIndex = 7;
			this->TaskGroupBox->TabStop = false;
			this->TaskGroupBox->Text = L"Task";
			// 
			// taskListBox
			// 
			this->taskListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskListBox->FormattingEnabled = true;
			this->taskListBox->ItemHeight = 29;
			this->taskListBox->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Installation", L"Tracker Alignment", L"Readiness Check", 
				L"Block 1", L"Block 2", L"Block 3", L"Block 4", L"Pause", L"Block 5", L"Block 6", L"Block 7", L"Block 8", L"(end of protocol)"});
			this->taskListBox->Location = System::Drawing::Point(11, 34);
			this->taskListBox->Margin = System::Windows::Forms::Padding(4);
			this->taskListBox->Name = L"taskListBox";
			this->taskListBox->Size = System::Drawing::Size(512, 207);
			this->taskListBox->TabIndex = 5;
			// 
			// ProtocolGroupBox
			// 
			this->ProtocolGroupBox->Controls->Add(this->floatingRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->seatedRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->protocolListBox);
			this->ProtocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ProtocolGroupBox->Location = System::Drawing::Point(13, 227);
			this->ProtocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Name = L"ProtocolGroupBox";
			this->ProtocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Size = System::Drawing::Size(536, 174);
			this->ProtocolGroupBox->TabIndex = 6;
			this->ProtocolGroupBox->TabStop = false;
			this->ProtocolGroupBox->Text = L"Protocol";
			// 
			// floatingRadioButton
			// 
			this->floatingRadioButton->AutoSize = true;
			this->floatingRadioButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->floatingRadioButton->Location = System::Drawing::Point(191, 28);
			this->floatingRadioButton->Margin = System::Windows::Forms::Padding(4);
			this->floatingRadioButton->Name = L"floatingRadioButton";
			this->floatingRadioButton->Size = System::Drawing::Size(231, 33);
			this->floatingRadioButton->TabIndex = 7;
			this->floatingRadioButton->TabStop = true;
			this->floatingRadioButton->Text = L"Quasi-Freefloating";
			this->floatingRadioButton->UseVisualStyleBackColor = true;
			// 
			// seatedRadioButton
			// 
			this->seatedRadioButton->AutoSize = true;
			this->seatedRadioButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->seatedRadioButton->Location = System::Drawing::Point(32, 28);
			this->seatedRadioButton->Margin = System::Windows::Forms::Padding(4);
			this->seatedRadioButton->Name = L"seatedRadioButton";
			this->seatedRadioButton->Size = System::Drawing::Size(108, 33);
			this->seatedRadioButton->TabIndex = 6;
			this->seatedRadioButton->TabStop = true;
			this->seatedRadioButton->Text = L"Seated";
			this->seatedRadioButton->UseVisualStyleBackColor = true;
			// 
			// protocolListBox
			// 
			this->protocolListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolListBox->FormattingEnabled = true;
			this->protocolListBox->ItemHeight = 29;
			this->protocolListBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Visual - Manual", L"Visual - Visual", L"Manual - Manual"});
			this->protocolListBox->Location = System::Drawing::Point(15, 70);
			this->protocolListBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolListBox->Name = L"protocolListBox";
			this->protocolListBox->Size = System::Drawing::Size(508, 91);
			this->protocolListBox->TabIndex = 5;
			// 
			// SubjectGroupBox
			// 
			this->SubjectGroupBox->Controls->Add(this->subjectListBox);
			this->SubjectGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->SubjectGroupBox->Location = System::Drawing::Point(13, 69);
			this->SubjectGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->SubjectGroupBox->Name = L"SubjectGroupBox";
			this->SubjectGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->SubjectGroupBox->Size = System::Drawing::Size(536, 140);
			this->SubjectGroupBox->TabIndex = 5;
			this->SubjectGroupBox->TabStop = false;
			this->SubjectGroupBox->Text = L"User ID";
			// 
			// subjectListBox
			// 
			this->subjectListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectListBox->FormattingEnabled = true;
			this->subjectListBox->ItemHeight = 29;
			this->subjectListBox->Items->AddRange(gcnew cli::array< System::Object^  >(14) {L"Subject 1", L"Subject 2", L"Subject 3", 
				L"Subject 4", L"Subject 5", L"Subject 6", L"Subject 7", L"Subject 8", L"Subject 9", L"Subject 10", L"Subject 11", L"Subject 12", 
				L"Demo", L"Maintenance"});
			this->subjectListBox->Location = System::Drawing::Point(11, 34);
			this->subjectListBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectListBox->Name = L"subjectListBox";
			this->subjectListBox->Size = System::Drawing::Size(512, 91);
			this->subjectListBox->TabIndex = 5;
			// 
			// button1
			// 
			this->button1->Enabled = false;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(118, 934);
			this->button1->Margin = System::Windows::Forms::Padding(4);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(127, 44);
			this->button1->TabIndex = 2;
			this->button1->Text = L"<< Back";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// LogonNext
			// 
			this->LogonNext->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->LogonNext->Location = System::Drawing::Point(319, 933);
			this->LogonNext->Margin = System::Windows::Forms::Padding(4);
			this->LogonNext->Name = L"LogonNext";
			this->LogonNext->Size = System::Drawing::Size(127, 44);
			this->LogonNext->TabIndex = 1;
			this->LogonNext->Text = L"Next >>";
			this->LogonNext->UseVisualStyleBackColor = true;
			this->LogonNext->Click += gcnew System::EventHandler(this, &GraspDesktop::LogonNext_Click);
			// 
			// WelcomeLabel
			// 
			this->WelcomeLabel->AutoSize = true;
			this->WelcomeLabel->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->WelcomeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->WelcomeLabel->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->WelcomeLabel->Location = System::Drawing::Point(149, 22);
			this->WelcomeLabel->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->WelcomeLabel->Name = L"WelcomeLabel";
			this->WelcomeLabel->Size = System::Drawing::Size(245, 29);
			this->WelcomeLabel->TabIndex = 0;
			this->WelcomeLabel->Text = L"Welcome to GRASP";
			this->WelcomeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// GraspDesktop
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1576, 1028);
			this->Controls->Add(this->NavigatorGroupBox);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspDesktop";
			this->Text = L"GRASP@ISS";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspDesktop::GraspDesktop_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspDesktop::GraspDesktop_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->NavigatorGroupBox->ResumeLayout(false);
			this->NavigatorGroupBox->PerformLayout();
			this->ProgressBox->ResumeLayout(false);
			this->TaskGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->PerformLayout();
			this->SubjectGroupBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	private:
		void InitializeAnimations( void );
		void KillAnimations( void );


	private: System::Windows::Forms::DialogResult MessageBox( String^ message, String^ caption, MessageBoxButtons buttons ) {
				// Here I create my own MessageBox method. So far I am using the standard MessageBox, but
				//  I place it here so that later I can change it to use a larger font.
				 return( MessageBox::Show( message, caption, buttons ) );
			 }
	private: System::Void CancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 // Close the form and exit without any further fanfare.
				 Close();
			 }
	private: System::Void LogonNext_Click(System::Object^  sender, System::EventArgs^  e) {
				 // Check that we have all the information that we need to proceed.
				 if ( subjectListBox->SelectedIndex < 0 ) MessageBox( "Please select a User ID.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( !seatedRadioButton->Checked && !floatingRadioButton->Checked ) MessageBox( "Please select Seated or Quasi-Freefloating.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( protocolListBox->SelectedIndex < 0 ) MessageBox( "Please select a Protocol.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( taskListBox->SelectedIndex < 0 ) MessageBox( "Please select a Task.", "GRASP@ISS", MessageBoxButtons::OK );
				 // If all is good, go on to do the specified task.
				 else {
					 // Hide the task navigator.
					 NavigatorGroupBox->Hide();
					 // Run the task.
					 String^ msg = "       Running GRASP\n"
						 + "\n  Subject:   " + subjectListBox->SelectedItem->ToString()
						 + "\n Posture:   " + ( seatedRadioButton->Checked ? seatedRadioButton->Text : floatingRadioButton->Text )
						 + "\nProtocol:   " + protocolListBox->SelectedItem->ToString()
						 + "\n     Task:   " + taskListBox->SelectedItem->ToString();
					 MessageBox( msg, "GRASP@ISS", MessageBoxButtons::OK );
					 // Return to the Navigator.
					 NavigatorGroupBox->Show();

				 }

			 }
	private: System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {
				 InitializeAnimations();
			 }
	private: System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 KillAnimations();
			 }
};
}

