#pragma once

#include "GraspScripts.h"

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

	private:

		String^	rootDirectory;
		String^ execDirectory;
		String^ scriptDirectory;
		String^ instructionsDirectory;

		array<GraspGUI::Subject ^> ^subjectList;
		int nSubjects;
		array<GraspGUI::Protocol ^> ^protocolList;
		int nProtocols;
		array<GraspGUI::Task ^> ^taskList;
		int nTasks;
		array<GraspGUI::Step ^> ^stepList;
		int nSteps;
		int currentStep;
		// Record the completion code.
		int stepCompletionCode;
		// Keep track of whether the tasks was executed or not.
		// If the command was not executed, then verify before allowing the 
		// user to skip to the next step.
		bool verifyNext;
		// When a step is shown that includes a command we must show an html page, 
		// request confirmation, show another html page and then execute the command.
		// While the command is executing, the event loop is paused. But the
		// loading of the html page can take several cycles and the event loop has to 
		// run in order to be able to see the pages. This flag allows us to "cue up" the 
		// command, run the event loop until everything is visible, and then run the command.
		bool cueStepCommand;

		// A timer to handle animations and screen refresh, and associated actions.
		static Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			// RefreshAnimations();
		}
		void CreateRefreshTimer( int interval ) {
			refreshTimer = gcnew( System::Windows::Forms::Timer );
			refreshTimer->Interval = interval;
			refreshTimer->Tick += gcnew EventHandler( this, &GraspGUI::GraspDesktop::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			refreshTimer->Start();
		}
		void StopRefreshTimer( void ) {
			refreshTimer->Stop();
		}		


	public:
		GraspDesktop(void)
		{
			// The lists of subjects, protocols, tasks and steps are dynamically
			//  read from text files. Here we initialize the lists.
			// I treating the lists as arrays and defining a separate counter to keep
			//  track of how many items in each list. There is probably a way to treat
			//  the arrays as actual lists with dynamic size, but I don't remember how.
			subjectList = gcnew array<GraspGUI::Subject ^>(MAX_SUBJECTS);
			nSubjects = 0;
			protocolList = gcnew array<GraspGUI::Protocol ^>(MAX_PROTOCOLS);
			nProtocols = 0;
			taskList = gcnew array<GraspGUI::Task ^>(MAX_TASKS);
			nTasks = 0;
			stepList = gcnew array<GraspGUI::Step ^>(MAX_STEPS);
			nSteps = 0;
			currentStep = 0;
			stepCompletionCode = 0;
			cueStepCommand = false;

			// It is convenient to define the root directory and predefine the various subdirectories.
			// The web browser tool used to display instructions requires the full path to the file.
			// So here we constuct the path to the current (root) directory and to the instructions.
			char root_string[MAX_PATH];
			int bytes = GetCurrentDirectory( sizeof( root_string ), root_string );
			fAbortMessageOnCondition( bytes > MAX_PATH, "GraspGUI", "Path to current directory is too long." );
			rootDirectory = gcnew String( root_string ) + "\\";
			instructionsDirectory = rootDirectory + "Instructions\\";
			scriptDirectory =  rootDirectory + "Scripts\\";
			execDirectory =  rootDirectory + "bin\\";

			// Standard Windows Forms initialization.
			InitializeComponent();

			// Initialize what buttons are visible.
			ShowLogon();
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
	private: System::Windows::Forms::GroupBox^  navigatorGroupBox;
	private: System::Windows::Forms::GroupBox^  subjectGroupBox;
	private: System::Windows::Forms::ListBox^  subjectListBox;
	private: System::Windows::Forms::GroupBox^  taskGroupBox;
	private: System::Windows::Forms::ListBox^  taskListBox;
	private: System::Windows::Forms::GroupBox^  protocolGroupBox;
	private: System::Windows::Forms::ListBox^  protocolListBox;
	private: System::Windows::Forms::RadioButton^  floatingRadioButton;
	private: System::Windows::Forms::RadioButton^  seatedRadioButton;
	private: System::Windows::Forms::Button^  statusButton;
	private: System::Windows::Forms::Button^  quitButton;


	private: System::Windows::Forms::GroupBox^  instructionsGroupBox;
	private: System::Windows::Forms::WebBrowser^  instructionViewer;
	private: System::Windows::Forms::GroupBox^  htmlGroupBox;
	private: System::Windows::Forms::GroupBox^  stepHeaderGroupBox;
	private: System::Windows::Forms::TextBox^  stepCounterTextBox;
	private: System::Windows::Forms::TextBox^  stepHeaderTextBox;

	private: System::Windows::Forms::GroupBox^  normalNavigationGroupBox;
	private: System::Windows::Forms::Button^  startButton;
	private: System::Windows::Forms::Button^  previousButton;
	private: System::Windows::Forms::Button^  nextButton;
	private: System::Windows::Forms::GroupBox^  errorNavigationGroupBox;
	private: System::Windows::Forms::Button^  retryButton;
	private: System::Windows::Forms::Button^  restartButton;
	private: System::Windows::Forms::TextBox^  errorCodeTextBox;
	private: System::Windows::Forms::Label^  errorCodeLabel;


	private: System::Windows::Forms::Button^  ignoreButton;


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
			this->navigatorGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->statusButton = (gcnew System::Windows::Forms::Button());
			this->quitButton = (gcnew System::Windows::Forms::Button());
			this->taskGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->taskListBox = (gcnew System::Windows::Forms::ListBox());
			this->protocolGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->floatingRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->seatedRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->protocolListBox = (gcnew System::Windows::Forms::ListBox());
			this->subjectGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->subjectListBox = (gcnew System::Windows::Forms::ListBox());
			this->instructionsGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->errorNavigationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->errorCodeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->errorCodeLabel = (gcnew System::Windows::Forms::Label());
			this->retryButton = (gcnew System::Windows::Forms::Button());
			this->restartButton = (gcnew System::Windows::Forms::Button());
			this->ignoreButton = (gcnew System::Windows::Forms::Button());
			this->normalNavigationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->startButton = (gcnew System::Windows::Forms::Button());
			this->previousButton = (gcnew System::Windows::Forms::Button());
			this->nextButton = (gcnew System::Windows::Forms::Button());
			this->stepHeaderGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->stepCounterTextBox = (gcnew System::Windows::Forms::TextBox());
			this->stepHeaderTextBox = (gcnew System::Windows::Forms::TextBox());
			this->htmlGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->instructionViewer = (gcnew System::Windows::Forms::WebBrowser());
			this->navigatorGroupBox->SuspendLayout();
			this->taskGroupBox->SuspendLayout();
			this->protocolGroupBox->SuspendLayout();
			this->subjectGroupBox->SuspendLayout();
			this->instructionsGroupBox->SuspendLayout();
			this->errorNavigationGroupBox->SuspendLayout();
			this->normalNavigationGroupBox->SuspendLayout();
			this->stepHeaderGroupBox->SuspendLayout();
			this->htmlGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// navigatorGroupBox
			// 
			this->navigatorGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->navigatorGroupBox->Controls->Add(this->statusButton);
			this->navigatorGroupBox->Controls->Add(this->quitButton);
			this->navigatorGroupBox->Controls->Add(this->taskGroupBox);
			this->navigatorGroupBox->Controls->Add(this->protocolGroupBox);
			this->navigatorGroupBox->Controls->Add(this->subjectGroupBox);
			this->navigatorGroupBox->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->navigatorGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->navigatorGroupBox->Location = System::Drawing::Point(13, 13);
			this->navigatorGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->navigatorGroupBox->Name = L"navigatorGroupBox";
			this->navigatorGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->navigatorGroupBox->Size = System::Drawing::Size(566, 952);
			this->navigatorGroupBox->TabIndex = 5;
			this->navigatorGroupBox->TabStop = false;
			this->navigatorGroupBox->Text = L"Navigator";
			// 
			// statusButton
			// 
			this->statusButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->statusButton->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->statusButton->Location = System::Drawing::Point(316, 895);
			this->statusButton->Margin = System::Windows::Forms::Padding(4);
			this->statusButton->Name = L"statusButton";
			this->statusButton->Size = System::Drawing::Size(127, 44);
			this->statusButton->TabIndex = 9;
			this->statusButton->Text = L"Status";
			this->statusButton->UseVisualStyleBackColor = true;
			this->statusButton->Click += gcnew System::EventHandler(this, &GraspDesktop::statusButton_Click);
			// 
			// quitButton
			// 
			this->quitButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->quitButton->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->quitButton->Location = System::Drawing::Point(124, 895);
			this->quitButton->Margin = System::Windows::Forms::Padding(4);
			this->quitButton->Name = L"quitButton";
			this->quitButton->Size = System::Drawing::Size(127, 44);
			this->quitButton->TabIndex = 8;
			this->quitButton->Text = L"Quit";
			this->quitButton->UseVisualStyleBackColor = true;
			this->quitButton->Click += gcnew System::EventHandler(this, &GraspDesktop::CancelButton_Click);
			// 
			// taskGroupBox
			// 
			this->taskGroupBox->Controls->Add(this->taskListBox);
			this->taskGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskGroupBox->Location = System::Drawing::Point(13, 501);
			this->taskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Name = L"taskGroupBox";
			this->taskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Size = System::Drawing::Size(536, 378);
			this->taskGroupBox->TabIndex = 7;
			this->taskGroupBox->TabStop = false;
			this->taskGroupBox->Text = L"Task";
			this->taskGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::TaskGroupBox_Enter);
			// 
			// taskListBox
			// 
			this->taskListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskListBox->FormattingEnabled = true;
			this->taskListBox->ItemHeight = 36;
			this->taskListBox->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Installation", L"Tracker Alignment", L"Readiness Check", 
				L"Block 1", L"Block 2", L"Block 3", L"Block 4", L"Pause", L"Block 5", L"Block 6", L"Block 7", L"Block 8", L"(end of protocol)"});
			this->taskListBox->Location = System::Drawing::Point(11, 34);
			this->taskListBox->Margin = System::Windows::Forms::Padding(4);
			this->taskListBox->Name = L"taskListBox";
			this->taskListBox->Size = System::Drawing::Size(512, 328);
			this->taskListBox->TabIndex = 5;
			this->taskListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspDesktop::taskListBox_SelectedIndexChanged);
			// 
			// protocolGroupBox
			// 
			this->protocolGroupBox->Controls->Add(this->floatingRadioButton);
			this->protocolGroupBox->Controls->Add(this->seatedRadioButton);
			this->protocolGroupBox->Controls->Add(this->protocolListBox);
			this->protocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolGroupBox->Location = System::Drawing::Point(13, 237);
			this->protocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Name = L"protocolGroupBox";
			this->protocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Size = System::Drawing::Size(536, 260);
			this->protocolGroupBox->TabIndex = 6;
			this->protocolGroupBox->TabStop = false;
			this->protocolGroupBox->Text = L"Protocol";
			this->protocolGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::ProtocolGroupBox_Enter);
			// 
			// floatingRadioButton
			// 
			this->floatingRadioButton->AutoSize = true;
			this->floatingRadioButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->floatingRadioButton->Location = System::Drawing::Point(191, 40);
			this->floatingRadioButton->Margin = System::Windows::Forms::Padding(4);
			this->floatingRadioButton->Name = L"floatingRadioButton";
			this->floatingRadioButton->Size = System::Drawing::Size(280, 40);
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
			this->seatedRadioButton->Location = System::Drawing::Point(32, 40);
			this->seatedRadioButton->Margin = System::Windows::Forms::Padding(4);
			this->seatedRadioButton->Name = L"seatedRadioButton";
			this->seatedRadioButton->Size = System::Drawing::Size(129, 40);
			this->seatedRadioButton->TabIndex = 6;
			this->seatedRadioButton->TabStop = true;
			this->seatedRadioButton->Text = L"Seated";
			this->seatedRadioButton->UseVisualStyleBackColor = true;
			this->seatedRadioButton->CheckedChanged += gcnew System::EventHandler(this, &GraspDesktop::seatedRadioButton_CheckedChanged);
			// 
			// protocolListBox
			// 
			this->protocolListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolListBox->FormattingEnabled = true;
			this->protocolListBox->ItemHeight = 36;
			this->protocolListBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Visual - Manual", L"Visual - Visual", L"Manual - Manual", 
				L"Utilities"});
			this->protocolListBox->Location = System::Drawing::Point(11, 96);
			this->protocolListBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolListBox->Name = L"protocolListBox";
			this->protocolListBox->Size = System::Drawing::Size(508, 148);
			this->protocolListBox->TabIndex = 5;
			this->protocolListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspDesktop::protocolListBox_SelectedIndexChanged);
			// 
			// subjectGroupBox
			// 
			this->subjectGroupBox->Controls->Add(this->subjectListBox);
			this->subjectGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectGroupBox->Location = System::Drawing::Point(13, 26);
			this->subjectGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectGroupBox->Name = L"subjectGroupBox";
			this->subjectGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->subjectGroupBox->Size = System::Drawing::Size(536, 198);
			this->subjectGroupBox->TabIndex = 5;
			this->subjectGroupBox->TabStop = false;
			this->subjectGroupBox->Text = L"User ID";
			this->subjectGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::SubjectGroupBox_Enter);
			// 
			// subjectListBox
			// 
			this->subjectListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectListBox->FormattingEnabled = true;
			this->subjectListBox->ItemHeight = 36;
			this->subjectListBox->Items->AddRange(gcnew cli::array< System::Object^  >(14) {L"Subject 1", L"Subject 2", L"Subject 3", 
				L"Subject 4", L"Subject 5", L"Subject 6", L"Subject 7", L"Subject 8", L"Subject 9", L"Subject 10", L"Subject 11", L"Subject 12", 
				L"Demo", L"Maintenance"});
			this->subjectListBox->Location = System::Drawing::Point(11, 34);
			this->subjectListBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectListBox->Name = L"subjectListBox";
			this->subjectListBox->Size = System::Drawing::Size(512, 148);
			this->subjectListBox->TabIndex = 5;
			this->subjectListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspDesktop::subjectListBox_SelectedIndexChanged);
			// 
			// instructionsGroupBox
			// 
			this->instructionsGroupBox->Controls->Add(this->errorNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->normalNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->stepHeaderGroupBox);
			this->instructionsGroupBox->Controls->Add(this->htmlGroupBox);
			this->instructionsGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->instructionsGroupBox->Location = System::Drawing::Point(598, 13);
			this->instructionsGroupBox->Name = L"instructionsGroupBox";
			this->instructionsGroupBox->Size = System::Drawing::Size(584, 952);
			this->instructionsGroupBox->TabIndex = 7;
			this->instructionsGroupBox->TabStop = false;
			this->instructionsGroupBox->Text = L"Instructions";
			// 
			// errorNavigationGroupBox
			// 
			this->errorNavigationGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->errorNavigationGroupBox->Controls->Add(this->errorCodeTextBox);
			this->errorNavigationGroupBox->Controls->Add(this->errorCodeLabel);
			this->errorNavigationGroupBox->Controls->Add(this->retryButton);
			this->errorNavigationGroupBox->Controls->Add(this->restartButton);
			this->errorNavigationGroupBox->Controls->Add(this->ignoreButton);
			this->errorNavigationGroupBox->Enabled = false;
			this->errorNavigationGroupBox->ForeColor = System::Drawing::Color::DarkMagenta;
			this->errorNavigationGroupBox->Location = System::Drawing::Point(14, 878);
			this->errorNavigationGroupBox->Name = L"errorNavigationGroupBox";
			this->errorNavigationGroupBox->Size = System::Drawing::Size(556, 68);
			this->errorNavigationGroupBox->TabIndex = 15;
			this->errorNavigationGroupBox->TabStop = false;
			this->errorNavigationGroupBox->Visible = false;
			// 
			// errorCodeTextBox
			// 
			this->errorCodeTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->errorCodeTextBox->Location = System::Drawing::Point(114, 20);
			this->errorCodeTextBox->Name = L"errorCodeTextBox";
			this->errorCodeTextBox->Size = System::Drawing::Size(100, 38);
			this->errorCodeTextBox->TabIndex = 16;
			this->errorCodeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// errorCodeLabel
			// 
			this->errorCodeLabel->AutoSize = true;
			this->errorCodeLabel->Location = System::Drawing::Point(17, 29);
			this->errorCodeLabel->Name = L"errorCodeLabel";
			this->errorCodeLabel->Size = System::Drawing::Size(91, 20);
			this->errorCodeLabel->TabIndex = 15;
			this->errorCodeLabel->Text = L"Error Code";
			// 
			// retryButton
			// 
			this->retryButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->retryButton->Location = System::Drawing::Point(221, 17);
			this->retryButton->Margin = System::Windows::Forms::Padding(4);
			this->retryButton->Name = L"retryButton";
			this->retryButton->Size = System::Drawing::Size(104, 44);
			this->retryButton->TabIndex = 14;
			this->retryButton->Text = L"Retry";
			this->retryButton->UseVisualStyleBackColor = true;
			this->retryButton->Click += gcnew System::EventHandler(this, &GraspDesktop::retryButton_Click);
			// 
			// restartButton
			// 
			this->restartButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->restartButton->Location = System::Drawing::Point(445, 17);
			this->restartButton->Margin = System::Windows::Forms::Padding(4);
			this->restartButton->Name = L"restartButton";
			this->restartButton->Size = System::Drawing::Size(104, 44);
			this->restartButton->TabIndex = 13;
			this->restartButton->Text = L"Restart";
			this->restartButton->UseVisualStyleBackColor = true;
			this->restartButton->Click += gcnew System::EventHandler(this, &GraspDesktop::restartButton_Click);
			// 
			// ignoreButton
			// 
			this->ignoreButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ignoreButton->Location = System::Drawing::Point(333, 17);
			this->ignoreButton->Margin = System::Windows::Forms::Padding(4);
			this->ignoreButton->Name = L"ignoreButton";
			this->ignoreButton->Size = System::Drawing::Size(104, 44);
			this->ignoreButton->TabIndex = 12;
			this->ignoreButton->Text = L"Ignore";
			this->ignoreButton->UseVisualStyleBackColor = true;
			this->ignoreButton->Click += gcnew System::EventHandler(this, &GraspDesktop::ignoreButton_Click);
			// 
			// normalNavigationGroupBox
			// 
			this->normalNavigationGroupBox->Controls->Add(this->startButton);
			this->normalNavigationGroupBox->Controls->Add(this->previousButton);
			this->normalNavigationGroupBox->Controls->Add(this->nextButton);
			this->normalNavigationGroupBox->Location = System::Drawing::Point(14, 878);
			this->normalNavigationGroupBox->Name = L"normalNavigationGroupBox";
			this->normalNavigationGroupBox->Size = System::Drawing::Size(556, 68);
			this->normalNavigationGroupBox->TabIndex = 12;
			this->normalNavigationGroupBox->TabStop = false;
			// 
			// startButton
			// 
			this->startButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->startButton->Location = System::Drawing::Point(215, 17);
			this->startButton->Margin = System::Windows::Forms::Padding(4);
			this->startButton->Name = L"startButton";
			this->startButton->Size = System::Drawing::Size(127, 44);
			this->startButton->TabIndex = 14;
			this->startButton->Text = L"Start";
			this->startButton->UseVisualStyleBackColor = true;
			this->startButton->Click += gcnew System::EventHandler(this, &GraspDesktop::startButton_Click);
			// 
			// previousButton
			// 
			this->previousButton->Enabled = false;
			this->previousButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->previousButton->Location = System::Drawing::Point(12, 17);
			this->previousButton->Margin = System::Windows::Forms::Padding(4);
			this->previousButton->Name = L"previousButton";
			this->previousButton->Size = System::Drawing::Size(127, 44);
			this->previousButton->TabIndex = 13;
			this->previousButton->Text = L"<< Back";
			this->previousButton->UseVisualStyleBackColor = true;
			this->previousButton->Visible = false;
			this->previousButton->Click += gcnew System::EventHandler(this, &GraspDesktop::previousButton_Click);
			// 
			// nextButton
			// 
			this->nextButton->Enabled = false;
			this->nextButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->nextButton->Location = System::Drawing::Point(418, 17);
			this->nextButton->Margin = System::Windows::Forms::Padding(4);
			this->nextButton->Name = L"nextButton";
			this->nextButton->Size = System::Drawing::Size(127, 44);
			this->nextButton->TabIndex = 12;
			this->nextButton->Text = L"Next >>";
			this->nextButton->UseVisualStyleBackColor = true;
			this->nextButton->Visible = false;
			this->nextButton->Click += gcnew System::EventHandler(this, &GraspDesktop::nextButton_Click);
			// 
			// stepHeaderGroupBox
			// 
			this->stepHeaderGroupBox->Controls->Add(this->stepCounterTextBox);
			this->stepHeaderGroupBox->Controls->Add(this->stepHeaderTextBox);
			this->stepHeaderGroupBox->Location = System::Drawing::Point(14, 22);
			this->stepHeaderGroupBox->Margin = System::Windows::Forms::Padding(0);
			this->stepHeaderGroupBox->Name = L"stepHeaderGroupBox";
			this->stepHeaderGroupBox->Size = System::Drawing::Size(557, 67);
			this->stepHeaderGroupBox->TabIndex = 6;
			this->stepHeaderGroupBox->TabStop = false;
			// 
			// stepCounterTextBox
			// 
			this->stepCounterTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->stepCounterTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepCounterTextBox->Location = System::Drawing::Point(450, 20);
			this->stepCounterTextBox->Name = L"stepCounterTextBox";
			this->stepCounterTextBox->Size = System::Drawing::Size(98, 34);
			this->stepCounterTextBox->TabIndex = 1;
			this->stepCounterTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// stepHeaderTextBox
			// 
			this->stepHeaderTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->stepHeaderTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepHeaderTextBox->Location = System::Drawing::Point(10, 20);
			this->stepHeaderTextBox->Name = L"stepHeaderTextBox";
			this->stepHeaderTextBox->Size = System::Drawing::Size(434, 34);
			this->stepHeaderTextBox->TabIndex = 0;
			// 
			// htmlGroupBox
			// 
			this->htmlGroupBox->Controls->Add(this->instructionViewer);
			this->htmlGroupBox->Location = System::Drawing::Point(14, 92);
			this->htmlGroupBox->Name = L"htmlGroupBox";
			this->htmlGroupBox->Size = System::Drawing::Size(557, 787);
			this->htmlGroupBox->TabIndex = 5;
			this->htmlGroupBox->TabStop = false;
			// 
			// instructionViewer
			// 
			this->instructionViewer->AllowWebBrowserDrop = false;
			this->instructionViewer->IsWebBrowserContextMenuEnabled = false;
			this->instructionViewer->Location = System::Drawing::Point(8, 25);
			this->instructionViewer->MinimumSize = System::Drawing::Size(20, 20);
			this->instructionViewer->Name = L"instructionViewer";
			this->instructionViewer->ScrollBarsEnabled = false;
			this->instructionViewer->Size = System::Drawing::Size(540, 755);
			this->instructionViewer->TabIndex = 2;
			this->instructionViewer->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
			this->instructionViewer->WebBrowserShortcutsEnabled = false;
			this->instructionViewer->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler(this, &GraspDesktop::instructionViewer_DocumentCompleted);
			// 
			// GraspDesktop
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(1194, 988);
			this->Controls->Add(this->instructionsGroupBox);
			this->Controls->Add(this->navigatorGroupBox);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ForeColor = System::Drawing::SystemColors::ControlText;
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspDesktop";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GRASP@ISS";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspDesktop::GraspDesktop_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspDesktop::GraspDesktop_Shown);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GraspDesktop::GraspDesktop_Paint);
			this->navigatorGroupBox->ResumeLayout(false);
			this->taskGroupBox->ResumeLayout(false);
			this->protocolGroupBox->ResumeLayout(false);
			this->protocolGroupBox->PerformLayout();
			this->subjectGroupBox->ResumeLayout(false);
			this->instructionsGroupBox->ResumeLayout(false);
			this->errorNavigationGroupBox->ResumeLayout(false);
			this->errorNavigationGroupBox->PerformLayout();
			this->normalNavigationGroupBox->ResumeLayout(false);
			this->stepHeaderGroupBox->ResumeLayout(false);
			this->stepHeaderGroupBox->PerformLayout();
			this->htmlGroupBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

		// The following declarations were generated automatically by the Forms designer.
		// I am not sure that they are absolutely necessary.
	private: 
		System::Void GraspDesktop_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {}
		System::Void SubjectGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {}
		System::Void ProtocolGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {}
		System::Void TaskGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {}

		System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {}

		// Here I create my own MessageBox method. So far I am using the standard MessageBox, but
		//  I place it here so that later I can change it to use a larger font.
		System::Windows::Forms::DialogResult MessageBox( String^ message, String^ caption, MessageBoxButtons buttons ) {
			return( MessageBox::Show( message, caption, buttons ) );
		}
		System::Void CancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
			// Close the form and exit without any further fanfare.
			Close();
		}

		System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {
			instructionViewer->Navigate( instructionsDirectory + "GraspWelcome.html" );
			subjectListBox->Items->Clear();
			protocolListBox->Items->Clear();
			taskListBox->Items->Clear();
			ParseSubjectFile( scriptDirectory + "Subjects.gsp" );
		}

		System::Void subjectListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			seatedRadioButton->Checked = false;
			floatingRadioButton->Checked = false;
			currentSubject = subjectListBox->SelectedIndex;
			protocolListBox->Items->Clear();
			taskListBox->Items->Clear();
			ShowLogon();
		}

		System::Void seatedRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			// Try to show the session file, but only if the subject has already been selected.
			protocolListBox->Items->Clear();
			taskListBox->Items->Clear();
			if ( subjectListBox->SelectedIndex >= 0 ) ParseSessionFile( scriptDirectory + subjectList[ subjectListBox->SelectedIndex ]->file );
			ShowLogon();
		}

		System::Void protocolListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			// Try to parse the protocol file, but only if the subject has already been selected.
			if ( subjectListBox->SelectedIndex < 0 ) ShowLogon();
			else {
				taskListBox->Items->Clear();
				int item_index = protocolListBox->SelectedIndex;
				if ( seatedRadioButton->Checked ) ParseProtocolFile( scriptDirectory + protocolList[item_index]->seated );
				else ParseProtocolFile( scriptDirectory + protocolList[item_index]->floating );
				// Preselect the first task in the newly selected protocol.
				taskListBox->SelectedIndex = 0;
				ShowStep();
			}
		}

		System::Void taskListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			stepHeaderTextBox->Text = taskListBox->Text;
			ParseTaskFile( scriptDirectory + taskList[taskListBox->SelectedIndex]->file );
			ShowStep();
		}

		System::Void statusButton_Click(System::Object^  sender, System::EventArgs^  e) {
			MessageBox( "Will show payload status.", "GRASP@ISS", MessageBoxButtons::OK );
			// system( "bin\\WinSCP.com /command \"open ftp://administrator:dex@10.80.12.103\" \"cd DATA1\" \"cd DATA\" \"cd glog\" \"ls\" \"get *\" \"exit\" & pause" );
		}

	private:

		// The following methods deal with navigating through the scripts.
		// I have chosen to group them together in GraspScripts.cpp, even the ones
		//  initially generated automatically by the Forms designer.

		// Parse the file containing the subject names, IDs and protocols.
		int ParseLine( char *token[], char *line );
		void ParseSubjectFile( String^ filename );
		void ParseSessionFile( String^ filename );
		void ParseProtocolFile( String^ filename );
		void ParseTaskFile( String^ filename );
		// Run the buttons and menus in the GUI.
		void ShowStep( void );
		void ShowLogon( void );
		void nextButton_Click(System::Object^  sender, System::EventArgs^  e);
		void previousButton_Click(System::Object^  sender, System::EventArgs^  e);
		void startButton_Click(System::Object^  sender, System::EventArgs^  e);
		void SelectNextTask ( void );
		void instructionViewer_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e);
		void retryButton_Click(System::Object^  sender, System::EventArgs^  e);
		void restartButton_Click(System::Object^  sender, System::EventArgs^  e);
		void ignoreButton_Click(System::Object^  sender, System::EventArgs^  e);

	};
}

