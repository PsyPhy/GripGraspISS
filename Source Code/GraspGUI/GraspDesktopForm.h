#pragma once

#include "GraspScripts.h"
#include "../DexServices/DexServices.h"

namespace GraspGUI {

	using namespace System;
	using namespace System::IO;
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

		Grasp::DexServices	*dex;

		String^	rootDirectory;
		String^ execDirectory;
		String^ scriptDirectory;
		String^ resultsDirectory;
		String^ instructionsDirectory;

		array<GraspGUI::Subject ^> ^subjectList;
		int nSubjects;
		int currentSubject;
		array<GraspGUI::Protocol ^> ^protocolList;
		int nProtocols;
		int currentProtocol;
		array<GraspGUI::Task ^> ^taskList;
		int nTasks;
		int currentTask;
		array<GraspGUI::Step ^> ^stepList;
		int nSteps;
		int currentStep;
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
		// Indicate the state of a step that involves an external command.
		unsigned short stepExecutionState;

	private: System::Windows::Forms::CheckBox^  unitTestingMode;
	private: System::Windows::Forms::Button^  repeatButton;


		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void SendProgressInfo( void ) {
			int subjectID = (( currentSubject >= 0 ) ? subjectList[currentSubject]->number : 0 );
			int protocolID = (( currentProtocol >= 0 ) ? protocolList[currentProtocol]->number : 0 );
			int taskID = (( currentTask >= 0 ) ? taskList[currentTask]->number : 0 );
			int stepID = (( currentStep >= 0 ) ? stepList[currentStep]->number : 0 );
			dex->SendTaskInfo(  subjectID, protocolID, taskID, stepID, stepExecutionState );
		}
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			SendProgressInfo();
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
		GraspDesktop( void )
		{

			// The lists of subjects, protocols, tasks and steps are dynamically
			//  read from text files. Here we initialize the lists.
			// I treating the lists as arrays and defining a separate counter to keep
			//  track of how many items in each list. There is probably a way to treat
			//  the arrays as actual lists with dynamic size, but I don't remember how.
			subjectList = gcnew array<GraspGUI::Subject ^>(MAX_SUBJECTS);
			nSubjects = 0;
			currentSubject = -1;
			protocolList = gcnew array<GraspGUI::Protocol ^>(MAX_PROTOCOLS);
			nProtocols = 0;
			currentProtocol = -1;
			taskList = gcnew array<GraspGUI::Task ^>(MAX_TASKS);
			nTasks = 0;
			currentTask = -1;
			stepList = gcnew array<GraspGUI::Step ^>(MAX_STEPS);
			nSteps = 0;
			currentStep = -1;
			cueStepCommand = false;

			// It is convenient to define the root directory and predefine the various subdirectories.
			// The web browser tool used to display instructions requires the full path to the file.
			// So here we constuct the path to the current (root) directory and to the instructions.

			SYSTEMTIME st;
			GetSystemTime( &st );

			char root_string[MAX_PATH];
			int bytes = GetCurrentDirectory( sizeof( root_string ), root_string );
			fAbortMessageOnCondition( bytes > MAX_PATH, "GraspGUI", "Path to current directory is too long." );
			rootDirectory = gcnew String( root_string ) + "\\";
			instructionsDirectory = rootDirectory + "Instructions\\";
			// Grasp.exe and other programs that may be called do not handle file paths that include spaces.
			// While we can control that paths below the Grasp root directory do not have spaces, we have no control
			// on the directories between the file system root and the Grasp root directory. So we do not
			// add the absolute path to the root directory to the other file paths defined below.
			scriptDirectory =  "Scripts\\";
			execDirectory =  "Executables\\";
			char datestr[MAX_PATH];
			sprintf( datestr, "%02d%02d%02d", st.wYear - 2000, st.wMonth, st.wDay );
			String ^dateString = gcnew String( datestr );
			resultsDirectory = "Results\\" +  dateString + "\\";
			try {
				if ( !Directory::Exists( resultsDirectory ) ) Directory::CreateDirectory( resultsDirectory );
			}
			catch ( Exception^ e ) 
			{
				e = e;
				fAbortMessage( "GraspGUI", "Error creating results directory." );
			}

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
	// Originally I used a listBox for the protocols, but I changed it later to a ComboBox.
	// That is why the variable name is "protocolListBox" instead of "protocolComboBox".
	private: System::Windows::Forms::GroupBox^  protocolGroupBox;
	private: System::Windows::Forms::ComboBox^  protocolListBox;

	private: System::Windows::Forms::Button^  statusButton;
	private: System::Windows::Forms::Button^  quitButton;

	private: System::Windows::Forms::GroupBox^  instructionsGroupBox;
	private: System::Windows::Forms::WebBrowser^  instructionViewer;
	private: System::Windows::Forms::GroupBox^  htmlGroupBox;
	private: System::Windows::Forms::GroupBox^  stepHeaderGroupBox;
	private: System::Windows::Forms::TextBox^  stepCounterTextBox;
	private: System::Windows::Forms::TextBox^  stepHeaderTextBox;

	private: System::Windows::Forms::GroupBox^  normalNavigationGroupBox;
	private: System::Windows::Forms::Button^  previousButton;
	private: System::Windows::Forms::Button^  nextButton;

	private: System::Windows::Forms::GroupBox^  commandNavigationGroupBox;
	private: System::Windows::Forms::Button^  executeButton;
	private: System::Windows::Forms::Button^  execBackButton;
	private: System::Windows::Forms::Button^  execSkipButton;

	private: System::Windows::Forms::GroupBox^  errorNavigationGroupBox;
	private: System::Windows::Forms::TextBox^  errorCodeTextBox;
	private: System::Windows::Forms::Label^  errorCodeLabel;
	private: System::Windows::Forms::Button^  retryButton;
	private: System::Windows::Forms::Button^  restartButton;
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
			this->protocolListBox = (gcnew System::Windows::Forms::ComboBox());
			this->subjectGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->subjectListBox = (gcnew System::Windows::Forms::ListBox());
			this->instructionsGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->errorNavigationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->errorCodeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->errorCodeLabel = (gcnew System::Windows::Forms::Label());
			this->retryButton = (gcnew System::Windows::Forms::Button());
			this->restartButton = (gcnew System::Windows::Forms::Button());
			this->ignoreButton = (gcnew System::Windows::Forms::Button());
			this->commandNavigationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->unitTestingMode = (gcnew System::Windows::Forms::CheckBox());
			this->executeButton = (gcnew System::Windows::Forms::Button());
			this->execBackButton = (gcnew System::Windows::Forms::Button());
			this->execSkipButton = (gcnew System::Windows::Forms::Button());
			this->normalNavigationGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->repeatButton = (gcnew System::Windows::Forms::Button());
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
			this->commandNavigationGroupBox->SuspendLayout();
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
			this->navigatorGroupBox->Size = System::Drawing::Size(620, 990);
			this->navigatorGroupBox->TabIndex = 5;
			this->navigatorGroupBox->TabStop = false;
			this->navigatorGroupBox->Text = L"Navigator";
			// 
			// statusButton
			// 
			this->statusButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->statusButton->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->statusButton->Location = System::Drawing::Point(340, 926);
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
			this->quitButton->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->quitButton->Location = System::Drawing::Point(148, 926);
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
			this->taskGroupBox->Location = System::Drawing::Point(13, 337);
			this->taskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Name = L"taskGroupBox";
			this->taskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Size = System::Drawing::Size(599, 568);
			this->taskGroupBox->TabIndex = 7;
			this->taskGroupBox->TabStop = false;
			this->taskGroupBox->Text = L"Task";
			this->taskGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::TaskGroupBox_Enter);
			// 
			// taskListBox
			// 
			this->taskListBox->Font = (gcnew System::Drawing::Font(L"Candara", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskListBox->FormattingEnabled = true;
			this->taskListBox->ItemHeight = 37;
			this->taskListBox->Location = System::Drawing::Point(11, 34);
			this->taskListBox->Margin = System::Windows::Forms::Padding(4);
			this->taskListBox->Name = L"taskListBox";
			this->taskListBox->Size = System::Drawing::Size(580, 522);
			this->taskListBox->TabIndex = 5;
			this->taskListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspDesktop::taskListBox_SelectedIndexChanged);
			// 
			// protocolGroupBox
			// 
			this->protocolGroupBox->Controls->Add(this->protocolListBox);
			this->protocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolGroupBox->Location = System::Drawing::Point(13, 232);
			this->protocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Name = L"protocolGroupBox";
			this->protocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Size = System::Drawing::Size(599, 97);
			this->protocolGroupBox->TabIndex = 6;
			this->protocolGroupBox->TabStop = false;
			this->protocolGroupBox->Text = L"Subsession";
			this->protocolGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::ProtocolGroupBox_Enter);
			// 
			// protocolListBox
			// 
			this->protocolListBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->protocolListBox->Font = (gcnew System::Drawing::Font(L"Candara", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolListBox->FormattingEnabled = true;
			this->protocolListBox->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"Please select a User ID ..."});
			this->protocolListBox->Location = System::Drawing::Point(11, 34);
			this->protocolListBox->Name = L"protocolListBox";
			this->protocolListBox->Size = System::Drawing::Size(579, 45);
			this->protocolListBox->TabIndex = 6;
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
			this->subjectGroupBox->Size = System::Drawing::Size(599, 198);
			this->subjectGroupBox->TabIndex = 5;
			this->subjectGroupBox->TabStop = false;
			this->subjectGroupBox->Text = L"User ID";
			this->subjectGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::SubjectGroupBox_Enter);
			// 
			// subjectListBox
			// 
			this->subjectListBox->ColumnWidth = 186;
			this->subjectListBox->Font = (gcnew System::Drawing::Font(L"Candara", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectListBox->FormattingEnabled = true;
			this->subjectListBox->ItemHeight = 37;
			this->subjectListBox->Location = System::Drawing::Point(11, 34);
			this->subjectListBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectListBox->MultiColumn = true;
			this->subjectListBox->Name = L"subjectListBox";
			this->subjectListBox->Size = System::Drawing::Size(580, 152);
			this->subjectListBox->TabIndex = 5;
			this->subjectListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &GraspDesktop::subjectListBox_SelectedIndexChanged);
			// 
			// instructionsGroupBox
			// 
			this->instructionsGroupBox->Controls->Add(this->errorNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->commandNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->normalNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->stepHeaderGroupBox);
			this->instructionsGroupBox->Controls->Add(this->htmlGroupBox);
			this->instructionsGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->instructionsGroupBox->Location = System::Drawing::Point(650, 13);
			this->instructionsGroupBox->Name = L"instructionsGroupBox";
			this->instructionsGroupBox->Size = System::Drawing::Size(620, 990);
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
			this->errorNavigationGroupBox->Location = System::Drawing::Point(14, 911);
			this->errorNavigationGroupBox->Name = L"errorNavigationGroupBox";
			this->errorNavigationGroupBox->Size = System::Drawing::Size(591, 68);
			this->errorNavigationGroupBox->TabIndex = 18;
			this->errorNavigationGroupBox->TabStop = false;
			this->errorNavigationGroupBox->Visible = false;
			// 
			// errorCodeTextBox
			// 
			this->errorCodeTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->errorCodeTextBox->Location = System::Drawing::Point(124, 21);
			this->errorCodeTextBox->Name = L"errorCodeTextBox";
			this->errorCodeTextBox->Size = System::Drawing::Size(100, 38);
			this->errorCodeTextBox->TabIndex = 16;
			this->errorCodeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// errorCodeLabel
			// 
			this->errorCodeLabel->AutoSize = true;
			this->errorCodeLabel->Location = System::Drawing::Point(17, 30);
			this->errorCodeLabel->Name = L"errorCodeLabel";
			this->errorCodeLabel->Size = System::Drawing::Size(91, 20);
			this->errorCodeLabel->TabIndex = 15;
			this->errorCodeLabel->Text = L"Error Code";
			// 
			// retryButton
			// 
			this->retryButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->retryButton->Location = System::Drawing::Point(480, 18);
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
			this->restartButton->Location = System::Drawing::Point(240, 18);
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
			this->ignoreButton->Location = System::Drawing::Point(360, 18);
			this->ignoreButton->Margin = System::Windows::Forms::Padding(4);
			this->ignoreButton->Name = L"ignoreButton";
			this->ignoreButton->Size = System::Drawing::Size(104, 44);
			this->ignoreButton->TabIndex = 12;
			this->ignoreButton->Text = L"Ignore";
			this->ignoreButton->UseVisualStyleBackColor = true;
			this->ignoreButton->Click += gcnew System::EventHandler(this, &GraspDesktop::ignoreButton_Click);
			// 
			// commandNavigationGroupBox
			// 
			this->commandNavigationGroupBox->Controls->Add(this->unitTestingMode);
			this->commandNavigationGroupBox->Controls->Add(this->executeButton);
			this->commandNavigationGroupBox->Controls->Add(this->execBackButton);
			this->commandNavigationGroupBox->Controls->Add(this->execSkipButton);
			this->commandNavigationGroupBox->Location = System::Drawing::Point(14, 911);
			this->commandNavigationGroupBox->Name = L"commandNavigationGroupBox";
			this->commandNavigationGroupBox->Size = System::Drawing::Size(591, 68);
			this->commandNavigationGroupBox->TabIndex = 17;
			this->commandNavigationGroupBox->TabStop = false;
			// 
			// unitTestingMode
			// 
			this->unitTestingMode->AutoSize = true;
			this->unitTestingMode->Location = System::Drawing::Point(311, 31);
			this->unitTestingMode->Name = L"unitTestingMode";
			this->unitTestingMode->Size = System::Drawing::Size(113, 24);
			this->unitTestingMode->TabIndex = 15;
			this->unitTestingMode->Text = L"unit testing";
			this->unitTestingMode->UseVisualStyleBackColor = true;
			this->unitTestingMode->Visible = false;
			// 
			// executeButton
			// 
			this->executeButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->executeButton->Location = System::Drawing::Point(449, 17);
			this->executeButton->Margin = System::Windows::Forms::Padding(4);
			this->executeButton->Name = L"executeButton";
			this->executeButton->Size = System::Drawing::Size(127, 44);
			this->executeButton->TabIndex = 14;
			this->executeButton->Text = L"Execute";
			this->executeButton->UseVisualStyleBackColor = true;
			this->executeButton->Click += gcnew System::EventHandler(this, &GraspDesktop::startButton_Click);
			// 
			// execBackButton
			// 
			this->execBackButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->execBackButton->Location = System::Drawing::Point(12, 17);
			this->execBackButton->Margin = System::Windows::Forms::Padding(4);
			this->execBackButton->Name = L"execBackButton";
			this->execBackButton->Size = System::Drawing::Size(127, 44);
			this->execBackButton->TabIndex = 13;
			this->execBackButton->Text = L"<< Back";
			this->execBackButton->UseVisualStyleBackColor = true;
			this->execBackButton->Click += gcnew System::EventHandler(this, &GraspDesktop::previousButton_Click);
			// 
			// execSkipButton
			// 
			this->execSkipButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->execSkipButton->Location = System::Drawing::Point(147, 17);
			this->execSkipButton->Margin = System::Windows::Forms::Padding(4);
			this->execSkipButton->Name = L"execSkipButton";
			this->execSkipButton->Size = System::Drawing::Size(127, 44);
			this->execSkipButton->TabIndex = 12;
			this->execSkipButton->Text = L"Skip >>";
			this->execSkipButton->UseVisualStyleBackColor = true;
			this->execSkipButton->Click += gcnew System::EventHandler(this, &GraspDesktop::skipButton_Click);
			// 
			// normalNavigationGroupBox
			// 
			this->normalNavigationGroupBox->Controls->Add(this->repeatButton);
			this->normalNavigationGroupBox->Controls->Add(this->previousButton);
			this->normalNavigationGroupBox->Controls->Add(this->nextButton);
			this->normalNavigationGroupBox->Location = System::Drawing::Point(14, 911);
			this->normalNavigationGroupBox->Name = L"normalNavigationGroupBox";
			this->normalNavigationGroupBox->Size = System::Drawing::Size(591, 68);
			this->normalNavigationGroupBox->TabIndex = 12;
			this->normalNavigationGroupBox->TabStop = false;
			// 
			// repeatButton
			// 
			this->repeatButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->repeatButton->Location = System::Drawing::Point(326, 19);
			this->repeatButton->Name = L"repeatButton";
			this->repeatButton->Size = System::Drawing::Size(124, 42);
			this->repeatButton->TabIndex = 14;
			this->repeatButton->Text = L"Repeat";
			this->repeatButton->UseVisualStyleBackColor = true;
			this->repeatButton->Click += gcnew System::EventHandler(this, &GraspDesktop::repeatButton_Click);
			this->repeatButton->Visible = false;
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
			this->nextButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->nextButton->Location = System::Drawing::Point(457, 17);
			this->nextButton->Margin = System::Windows::Forms::Padding(4);
			this->nextButton->Name = L"nextButton";
			this->nextButton->Size = System::Drawing::Size(127, 44);
			this->nextButton->TabIndex = 12;
			this->nextButton->Text = L"Next >>";
			this->nextButton->UseVisualStyleBackColor = true;
			this->nextButton->Click += gcnew System::EventHandler(this, &GraspDesktop::nextButton_Click);
			// 
			// stepHeaderGroupBox
			// 
			this->stepHeaderGroupBox->Controls->Add(this->stepCounterTextBox);
			this->stepHeaderGroupBox->Controls->Add(this->stepHeaderTextBox);
			this->stepHeaderGroupBox->Location = System::Drawing::Point(14, 26);
			this->stepHeaderGroupBox->Margin = System::Windows::Forms::Padding(0);
			this->stepHeaderGroupBox->Name = L"stepHeaderGroupBox";
			this->stepHeaderGroupBox->Size = System::Drawing::Size(591, 67);
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
			this->htmlGroupBox->Size = System::Drawing::Size(591, 813);
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
			this->instructionViewer->Size = System::Drawing::Size(568, 773);
			this->instructionViewer->TabIndex = 2;
			this->instructionViewer->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
			this->instructionViewer->WebBrowserShortcutsEnabled = false;
			this->instructionViewer->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler(this, &GraspDesktop::instructionViewer_DocumentCompleted);
			// 
			// GraspDesktop
			// 
			this->AcceptButton = this->executeButton;
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(1282, 1015);
			this->Controls->Add(this->instructionsGroupBox);
			this->Controls->Add(this->navigatorGroupBox);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ForeColor = System::Drawing::SystemColors::ControlText;
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspDesktop";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GRASP";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspDesktop::GraspDesktop_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspDesktop::GraspDesktop_Shown);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GraspDesktop::GraspDesktop_Paint);
			this->navigatorGroupBox->ResumeLayout(false);
			this->taskGroupBox->ResumeLayout(false);
			this->protocolGroupBox->ResumeLayout(false);
			this->subjectGroupBox->ResumeLayout(false);
			this->instructionsGroupBox->ResumeLayout(false);
			this->errorNavigationGroupBox->ResumeLayout(false);
			this->errorNavigationGroupBox->PerformLayout();
			this->commandNavigationGroupBox->ResumeLayout(false);
			this->commandNavigationGroupBox->PerformLayout();
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
		System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			dex->SendTaskInfo(  0, 0, 0, 0 );
			fOutputDebugString( "dex->SendTaskInfo( 0, 0, 0, 0 );\n" );
			dex->Disconnect();
			dex->Release();
		}

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

			// The text box used to hold the header is clipping descenders on lowercase letters.
			// From the internet I found that one needs to turn AutoSize off to change the height.
			// Since I can't do that in the designer, I do it here.
			stepHeaderTextBox->AutoSize = false;
			stepHeaderTextBox->Height = 36;
			stepHeaderTextBox->Text = "";
			// Initialize the login and task selection menus.
			subjectListBox->Items->Clear();
			protocolListBox->Items->Clear(); 
			protocolListBox->Items->Add( "Please select a User ID ..." );
			taskListBox->Items->Clear();
			ParseSubjectFile( scriptDirectory + "Subjects.sbj" );
			// Show a welcome screen.
			instructionViewer->Navigate( instructionsDirectory + "GraspWelcome.html" );

#ifdef _DEBUG
			// There is a check box that can be used to put the GUI into unit testing mode.
			// In this mode, external commands are not actually executed. Rather, a popup window
			//  is shown allowing the user to simulate return from the command with different error codes.
			// We only show this checkbox in debug builds.
			unitTestingMode->Visible = true;
#endif

			// Connect to DEX so that we can send info about the current subject, protocol, etc. to ground.
			fOutputDebugString( "Connecting to DEX ... " );
			dex = new Grasp::DexServices();
			dex->Initialize( "GraspGUI.dxl" );
			dex->Connect();
			fOutputDebugString( "OK.\n" );
			// Show progess on ground.
			CreateRefreshTimer( 1000 );
			StartRefreshTimer();

		}

		System::Void subjectListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			// If the subject that is selected already is clicked, do nothing.
			if ( subjectListBox->SelectedIndex == currentSubject ) return;
			// Check if a task is running and if so, request confirmation.
			else if ( currentStep > 0 ) {
				System::Windows::Forms::DialogResult response;				
				response = MessageBox( "Are you sure that you want to interrupt the currently running task?", "Grasp@ISS", MessageBoxButtons::YesNo );
				if ( response == System::Windows::Forms::DialogResult::Yes ) {
					subjectListBox->SelectedIndex = currentSubject;
					return;
				}
			}
			currentSubject = subjectListBox->SelectedIndex;
			if ( currentSubject >= 0 ) {
				ParseSessionFile( scriptDirectory + subjectList[currentSubject]->file );
				currentProtocol = -1;
				taskListBox->Items->Clear(); currentTask = -1;
			}
			ShowLogon();
		}

		System::Void protocolListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			// If the protocol that is selected already is clicked, do nothing.
			// if ( protocolListBox->SelectedIndex == currentProtocol ) return;
			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( verifyNext || currentStep > 0 ) {
				System::Windows::Forms::DialogResult response;
				response = MessageBox( "Are you sure you want to interrupt the current step?", "Grasp@ISS", MessageBoxButtons::YesNo );
				if ( response == System::Windows::Forms::DialogResult::No ) {
					protocolListBox->SelectedIndex = currentProtocol;
					return;
				}
				verifyNext = false;
				normalNavigationGroupBox->Visible = true;
				normalNavigationGroupBox->Enabled = true;
				commandNavigationGroupBox->Visible = false;
				commandNavigationGroupBox->Enabled = false;
			}
			currentProtocol = protocolListBox->SelectedIndex;
			ParseProtocolFile( scriptDirectory + protocolList[currentProtocol]->file );
			// Preselect the first task in the newly selected protocol.
			stepHeaderTextBox->Text = taskListBox->Text;
			currentStep = 0;
			currentTask = 0;
			taskListBox->SelectedIndex = 0; 
		}

		System::Void taskListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			if ( taskListBox->SelectedIndex < 0 ) {
				ShowLogon();
				return;
			}
			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( verifyNext || ( currentStep > 0 && currentStep < nSteps - 1 ) ) {
				System::Windows::Forms::DialogResult response;
				response = MessageBox( "Are you sure you want to interrupt the current step?", "Grasp@ISS", MessageBoxButtons::YesNo );
				if ( response == System::Windows::Forms::DialogResult::No ) {
					taskListBox->SelectedIndex = currentTask;
					return;
				}
				verifyNext = false;
				normalNavigationGroupBox->Visible = true;
				normalNavigationGroupBox->Enabled = true;
				commandNavigationGroupBox->Visible = false;
				commandNavigationGroupBox->Enabled = false;
			}
			currentTask = taskListBox->SelectedIndex;
			stepHeaderTextBox->Text = taskListBox->Text;
			if (  !taskList[taskListBox->SelectedIndex]->type->CompareTo( "SCRIPT" ) ) {
				// Read a series of steps from a script file.
				ParseTaskFile( scriptDirectory + taskList[taskListBox->SelectedIndex]->task_file );
			}
			else {
				// Create a task with a single step and use the info from the task entry to define the step.
				stepList[0] = gcnew Step();
				stepList[0]->number = 1;
				stepList[0]->type = taskList[taskListBox->SelectedIndex]->type;
				if ( !taskList[taskListBox->SelectedIndex]->type->CompareTo( "INSTRUCTION" ) ) {
					stepList[0]->instruction = taskList[taskListBox->SelectedIndex]->isolated_step->instruction;
				}
				else if ( !taskList[taskListBox->SelectedIndex]->type->CompareTo( "COMMAND" ) 
							|| !taskList[taskListBox->SelectedIndex]->type->CompareTo( "COMMAND@" ) 
							|| !taskList[taskListBox->SelectedIndex]->type->CompareTo( "SYSTEM" ) 
							|| !taskList[taskListBox->SelectedIndex]->type->CompareTo( "SYSTEM@" ) 
					) {
					stepList[0]->command = taskList[taskListBox->SelectedIndex]->isolated_step->command;
					stepList[0]->ready = taskList[taskListBox->SelectedIndex]->isolated_step->ready;
					stepList[0]->running = taskList[taskListBox->SelectedIndex]->isolated_step->running;
					stepList[0]->exit = taskList[taskListBox->SelectedIndex]->isolated_step->exit;
				}
				nSteps = 1;
			}
			currentStep = 0;
			ShowStep();
		}

		System::Void statusButton_Click(System::Object^  sender, System::EventArgs^  e) {
			system( "Executables\\GraspHardwareStatus.exe" );
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
		void skipButton_Click(System::Object^  sender, System::EventArgs^  e);
		void previousButton_Click(System::Object^  sender, System::EventArgs^  e);
		void startButton_Click(System::Object^  sender, System::EventArgs^  e);
		void SelectNextTask ( void );
		void instructionViewer_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e);
		void retryButton_Click(System::Object^  sender, System::EventArgs^  e);
		void restartButton_Click(System::Object^  sender, System::EventArgs^  e);
		void ignoreButton_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void repeatButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
};
}

