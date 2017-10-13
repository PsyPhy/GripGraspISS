#pragma once

#include "../Grip/GripPackets.h"
#include "../Useful/fOutputDebugString.h"
#include "../DexServices/DexServices.h"
#include "../GripGraspVersionControl/GripGraspVersionControl.h"
#include "GraspScripts.h"

namespace GraspGUI {

	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	// We need InteropServics in order to convert a String to a char *.
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// The main form for the Grasp GUI.
	/// </summary>
	public ref class GraspDesktop : public System::Windows::Forms::Form
	{

	protected:

		Grasp::DexServicesByProxy	*dex;

		String^	rootDirectory;
		String^ execDirectory;
		String^ scriptDirectory;
		String^ resultsPath;
		String^ resultsDirectory;
		String^ instructionsDirectory;

		List<GraspGUI::Record ^> ^actionList;
		int nextActionToOutput;

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

	protected: System::Windows::Forms::Button^  historyButton;


	protected: 

		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;

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
		GraspDesktop( String^ results_path )
		{

			// Caller can specify a path to the results directory.
			resultsPath = gcnew String( results_path );

			// The lists of subjects, protocols, tasks and steps are dynamically
			//  read from text files. Here we initialize the lists.
			// I treat the lists as arrays and define a separate counter to keep
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

			actionList = gcnew List<GraspGUI::Record ^>();
			nextActionToOutput = 0;

			// It is convenient to define the root directory and predefine the various subdirectories.
			// The web browser tool used to display instructions requires the full path to the file.
			// So here we constuct the absolute path to the current (root) directory and to the instructions.
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
			// Define the name of the subdirectory for today's results.
			SYSTEMTIME st;
			GetSystemTime( &st );
			char datestr[MAX_PATH];
			sprintf( datestr, "%02d%02d%02d", st.wYear - 2000, st.wMonth, st.wDay );
			String ^dateString = gcnew String( datestr );
			resultsDirectory = resultsPath + "\\" +  dateString + "\\";

			// Standard Windows Forms initialization.
			InitializeComponent();

			// Initialize what buttons are visible.
			ShowLogon();
		}
		String^ DateTimeString( void );

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

	protected:
		System::Windows::Forms::CheckBox^  unitTestingMode;
		System::Windows::Forms::Button^  repeatButton;
		System::Windows::Forms::GroupBox^  navigatorGroupBox;
		System::Windows::Forms::GroupBox^  subjectGroupBox;
		System::Windows::Forms::ListBox^  subjectListBox;
		System::Windows::Forms::GroupBox^  taskGroupBox;
		System::Windows::Forms::ListBox^  taskListBox;
		// Originally I used a listBox for the protocols, but I changed it later to a ComboBox.
		// That is why the variable name is "protocolListBox" instead of "protocolComboBox".
		System::Windows::Forms::GroupBox^  protocolGroupBox;
		System::Windows::Forms::ComboBox^  protocolListBox;

		System::Windows::Forms::Button^  statusButton;
		System::Windows::Forms::Button^  quitButton;

		System::Windows::Forms::GroupBox^  instructionsGroupBox;
		System::Windows::Forms::WebBrowser^  instructionViewer;
		System::Windows::Forms::GroupBox^  htmlGroupBox;
		System::Windows::Forms::GroupBox^  stepHeaderGroupBox;
		System::Windows::Forms::TextBox^  stepCounterTextBox;
		System::Windows::Forms::TextBox^  stepHeaderTextBox;

		System::Windows::Forms::GroupBox^  normalNavigationGroupBox;
		System::Windows::Forms::Button^  previousButton;
		System::Windows::Forms::Button^  nextButton;

		System::Windows::Forms::GroupBox^  commandNavigationGroupBox;
		System::Windows::Forms::Button^  executeButton;
		System::Windows::Forms::Button^  execBackButton;
		System::Windows::Forms::Button^  execSkipButton;

		System::Windows::Forms::GroupBox^  errorNavigationGroupBox;
		System::Windows::Forms::TextBox^  errorCodeTextBox;
		System::Windows::Forms::Label^  errorCodeLabel;
		System::Windows::Forms::Button^  retryButton;
		System::Windows::Forms::Button^  restartButton;
		System::Windows::Forms::Button^  ignoreButton;
		System::Windows::Forms::GroupBox^  dexStatusGroupBox;
		System::Windows::Forms::GroupBox^  stepProgressGroupBox;
		System::Windows::Forms::TextBox^  trialsRemainingTextBox;
		System::Windows::Forms::Label^  trialsRemainingLabel;
		System::Windows::Forms::Label^  scriptEngineLabel;
		System::Windows::Forms::TextBox^  scriptEngineTextBox;
		System::Windows::Forms::Label^  label1;
		System::Windows::Forms::TextBox^  programStateTextBox;
		System::Windows::Forms::TextBox^  programStateEnumTextBox;

		System::Windows::Forms::GroupBox^  commandGroupBox;
		System::Windows::Forms::TextBox^  commandTextBox;
		System::Windows::Forms::Label^  hmdVisibilityLabel;
		System::Windows::Forms::ProgressBar^  hmdVisibilityBar;
		System::Windows::Forms::Label^  handVisibilityLabel;
		System::Windows::Forms::ProgressBar^  handVisibilityBar;
		System::Windows::Forms::Label^  chestVisibilityLabel;
		System::Windows::Forms::ProgressBar^  chestVisibilityBar;
		System::Windows::Forms::Label^  visibleMarkersLabel;

		System::Windows::Forms::TextBox^  snapshotsTextBox;
		System::Windows::Forms::Label^  snapshotsLabel;
		System::Windows::Forms::Label^  errorCodeNote;
		System::Windows::Forms::TextBox^  packetTimeTextBox;

	protected:
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GraspDesktop::typeid));
			this->navigatorGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->packetTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->statusButton = (gcnew System::Windows::Forms::Button());
			this->quitButton = (gcnew System::Windows::Forms::Button());
			this->taskGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->taskListBox = (gcnew System::Windows::Forms::ListBox());
			this->dexStatusGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->scriptEngineTextBox = (gcnew System::Windows::Forms::TextBox());
			this->scriptEngineLabel = (gcnew System::Windows::Forms::Label());
			this->hmdVisibilityLabel = (gcnew System::Windows::Forms::Label());
			this->hmdVisibilityBar = (gcnew System::Windows::Forms::ProgressBar());
			this->handVisibilityLabel = (gcnew System::Windows::Forms::Label());
			this->handVisibilityBar = (gcnew System::Windows::Forms::ProgressBar());
			this->chestVisibilityLabel = (gcnew System::Windows::Forms::Label());
			this->chestVisibilityBar = (gcnew System::Windows::Forms::ProgressBar());
			this->visibleMarkersLabel = (gcnew System::Windows::Forms::Label());
			this->snapshotsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->snapshotsLabel = (gcnew System::Windows::Forms::Label());
			this->protocolGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->protocolListBox = (gcnew System::Windows::Forms::ComboBox());
			this->subjectGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->subjectListBox = (gcnew System::Windows::Forms::ListBox());
			this->instructionsGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->stepProgressGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->programStateEnumTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->programStateTextBox = (gcnew System::Windows::Forms::TextBox());
			this->trialsRemainingTextBox = (gcnew System::Windows::Forms::TextBox());
			this->trialsRemainingLabel = (gcnew System::Windows::Forms::Label());
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
			this->errorCodeNote = (gcnew System::Windows::Forms::Label());
			this->commandGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->commandTextBox = (gcnew System::Windows::Forms::TextBox());
			this->instructionViewer = (gcnew System::Windows::Forms::WebBrowser());
			this->historyButton = (gcnew System::Windows::Forms::Button());
			this->navigatorGroupBox->SuspendLayout();
			this->taskGroupBox->SuspendLayout();
			this->dexStatusGroupBox->SuspendLayout();
			this->protocolGroupBox->SuspendLayout();
			this->subjectGroupBox->SuspendLayout();
			this->instructionsGroupBox->SuspendLayout();
			this->stepProgressGroupBox->SuspendLayout();
			this->errorNavigationGroupBox->SuspendLayout();
			this->commandNavigationGroupBox->SuspendLayout();
			this->normalNavigationGroupBox->SuspendLayout();
			this->stepHeaderGroupBox->SuspendLayout();
			this->htmlGroupBox->SuspendLayout();
			this->commandGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// navigatorGroupBox
			// 
			this->navigatorGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->navigatorGroupBox->Controls->Add(this->historyButton);
			this->navigatorGroupBox->Controls->Add(this->packetTimeTextBox);
			this->navigatorGroupBox->Controls->Add(this->statusButton);
			this->navigatorGroupBox->Controls->Add(this->quitButton);
			this->navigatorGroupBox->Controls->Add(this->taskGroupBox);
			this->navigatorGroupBox->Controls->Add(this->protocolGroupBox);
			this->navigatorGroupBox->Controls->Add(this->subjectGroupBox);
			this->navigatorGroupBox->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->navigatorGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->navigatorGroupBox->Location = System::Drawing::Point(5, 7);
			this->navigatorGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->navigatorGroupBox->Name = L"navigatorGroupBox";
			this->navigatorGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->navigatorGroupBox->Size = System::Drawing::Size(611, 990);
			this->navigatorGroupBox->TabIndex = 5;
			this->navigatorGroupBox->TabStop = false;
			this->navigatorGroupBox->Text = L"Navigator";
			// 
			// packetTimeTextBox
			// 
			this->packetTimeTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->packetTimeTextBox->Location = System::Drawing::Point(476, 13);
			this->packetTimeTextBox->Name = L"packetTimeTextBox";
			this->packetTimeTextBox->Size = System::Drawing::Size(132, 19);
			this->packetTimeTextBox->TabIndex = 21;
			this->packetTimeTextBox->Text = L"00:00:00";
			this->packetTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->packetTimeTextBox->Visible = false;
			// 
			// statusButton
			// 
			this->statusButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->statusButton->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->statusButton->Location = System::Drawing::Point(239, 926);
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
			this->quitButton->Location = System::Drawing::Point(40, 925);
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
			this->taskGroupBox->Controls->Add(this->dexStatusGroupBox);
			this->taskGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->taskGroupBox->Location = System::Drawing::Point(6, 337);
			this->taskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Name = L"taskGroupBox";
			this->taskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->taskGroupBox->Size = System::Drawing::Size(599, 568);
			this->taskGroupBox->TabIndex = 7;
			this->taskGroupBox->TabStop = false;
			this->taskGroupBox->Text = L"Task";
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
			// dexStatusGroupBox
			// 
			this->dexStatusGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->dexStatusGroupBox->Controls->Add(this->scriptEngineTextBox);
			this->dexStatusGroupBox->Controls->Add(this->scriptEngineLabel);
			this->dexStatusGroupBox->Controls->Add(this->hmdVisibilityLabel);
			this->dexStatusGroupBox->Controls->Add(this->hmdVisibilityBar);
			this->dexStatusGroupBox->Controls->Add(this->handVisibilityLabel);
			this->dexStatusGroupBox->Controls->Add(this->handVisibilityBar);
			this->dexStatusGroupBox->Controls->Add(this->chestVisibilityLabel);
			this->dexStatusGroupBox->Controls->Add(this->chestVisibilityBar);
			this->dexStatusGroupBox->Controls->Add(this->visibleMarkersLabel);
			this->dexStatusGroupBox->Controls->Add(this->snapshotsTextBox);
			this->dexStatusGroupBox->Controls->Add(this->snapshotsLabel);
			this->dexStatusGroupBox->Enabled = false;
			this->dexStatusGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->dexStatusGroupBox->Location = System::Drawing::Point(23, 532);
			this->dexStatusGroupBox->Name = L"dexStatusGroupBox";
			this->dexStatusGroupBox->Size = System::Drawing::Size(599, 68);
			this->dexStatusGroupBox->TabIndex = 20;
			this->dexStatusGroupBox->TabStop = false;
			this->dexStatusGroupBox->Visible = false;
			// 
			// scriptEngineTextBox
			// 
			this->scriptEngineTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->scriptEngineTextBox->Location = System::Drawing::Point(178, 26);
			this->scriptEngineTextBox->Name = L"scriptEngineTextBox";
			this->scriptEngineTextBox->Size = System::Drawing::Size(40, 30);
			this->scriptEngineTextBox->TabIndex = 25;
			this->scriptEngineTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// scriptEngineLabel
			// 
			this->scriptEngineLabel->Location = System::Drawing::Point(116, 18);
			this->scriptEngineLabel->Name = L"scriptEngineLabel";
			this->scriptEngineLabel->Size = System::Drawing::Size(63, 47);
			this->scriptEngineLabel->TabIndex = 24;
			this->scriptEngineLabel->Text = L"Script Engine";
			this->scriptEngineLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// hmdVisibilityLabel
			// 
			this->hmdVisibilityLabel->Location = System::Drawing::Point(496, 19);
			this->hmdVisibilityLabel->Name = L"hmdVisibilityLabel";
			this->hmdVisibilityLabel->Size = System::Drawing::Size(60, 18);
			this->hmdVisibilityLabel->TabIndex = 23;
			this->hmdVisibilityLabel->Text = L"HMD";
			// 
			// hmdVisibilityBar
			// 
			this->hmdVisibilityBar->Location = System::Drawing::Point(502, 41);
			this->hmdVisibilityBar->Maximum = 8;
			this->hmdVisibilityBar->Name = L"hmdVisibilityBar";
			this->hmdVisibilityBar->Size = System::Drawing::Size(76, 15);
			this->hmdVisibilityBar->TabIndex = 22;
			this->hmdVisibilityBar->Value = 5;
			// 
			// handVisibilityLabel
			// 
			this->handVisibilityLabel->Location = System::Drawing::Point(405, 18);
			this->handVisibilityLabel->Name = L"handVisibilityLabel";
			this->handVisibilityLabel->Size = System::Drawing::Size(60, 18);
			this->handVisibilityLabel->TabIndex = 21;
			this->handVisibilityLabel->Text = L"Hand";
			// 
			// handVisibilityBar
			// 
			this->handVisibilityBar->Location = System::Drawing::Point(411, 41);
			this->handVisibilityBar->Maximum = 8;
			this->handVisibilityBar->Name = L"handVisibilityBar";
			this->handVisibilityBar->Size = System::Drawing::Size(76, 15);
			this->handVisibilityBar->TabIndex = 20;
			this->handVisibilityBar->Value = 5;
			// 
			// chestVisibilityLabel
			// 
			this->chestVisibilityLabel->Location = System::Drawing::Point(312, 18);
			this->chestVisibilityLabel->Name = L"chestVisibilityLabel";
			this->chestVisibilityLabel->Size = System::Drawing::Size(60, 18);
			this->chestVisibilityLabel->TabIndex = 19;
			this->chestVisibilityLabel->Text = L"Chest";
			// 
			// chestVisibilityBar
			// 
			this->chestVisibilityBar->Location = System::Drawing::Point(318, 41);
			this->chestVisibilityBar->Maximum = 8;
			this->chestVisibilityBar->Name = L"chestVisibilityBar";
			this->chestVisibilityBar->Size = System::Drawing::Size(76, 15);
			this->chestVisibilityBar->TabIndex = 18;
			this->chestVisibilityBar->Value = 5;
			// 
			// visibleMarkersLabel
			// 
			this->visibleMarkersLabel->Location = System::Drawing::Point(235, 20);
			this->visibleMarkersLabel->Name = L"visibleMarkersLabel";
			this->visibleMarkersLabel->Size = System::Drawing::Size(90, 42);
			this->visibleMarkersLabel->TabIndex = 17;
			this->visibleMarkersLabel->Text = L"Visible Markers";
			// 
			// snapshotsTextBox
			// 
			this->snapshotsTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->snapshotsTextBox->Location = System::Drawing::Point(64, 26);
			this->snapshotsTextBox->Name = L"snapshotsTextBox";
			this->snapshotsTextBox->Size = System::Drawing::Size(40, 30);
			this->snapshotsTextBox->TabIndex = 16;
			this->snapshotsTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// snapshotsLabel
			// 
			this->snapshotsLabel->Location = System::Drawing::Point(6, 18);
			this->snapshotsLabel->Name = L"snapshotsLabel";
			this->snapshotsLabel->Size = System::Drawing::Size(60, 47);
			this->snapshotsLabel->TabIndex = 15;
			this->snapshotsLabel->Text = L"Snap- shots";
			this->snapshotsLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// protocolGroupBox
			// 
			this->protocolGroupBox->Controls->Add(this->protocolListBox);
			this->protocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->protocolGroupBox->Location = System::Drawing::Point(6, 232);
			this->protocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Name = L"protocolGroupBox";
			this->protocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->protocolGroupBox->Size = System::Drawing::Size(599, 97);
			this->protocolGroupBox->TabIndex = 6;
			this->protocolGroupBox->TabStop = false;
			this->protocolGroupBox->Text = L"Subsession";
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
			this->subjectGroupBox->Location = System::Drawing::Point(6, 26);
			this->subjectGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectGroupBox->Name = L"subjectGroupBox";
			this->subjectGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->subjectGroupBox->Size = System::Drawing::Size(599, 198);
			this->subjectGroupBox->TabIndex = 5;
			this->subjectGroupBox->TabStop = false;
			this->subjectGroupBox->Text = L"User ID";
			// 
			// subjectListBox
			// 
			this->subjectListBox->ColumnWidth = 186;
			this->subjectListBox->Font = (gcnew System::Drawing::Font(L"Candara", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->subjectListBox->FormattingEnabled = true;
			this->subjectListBox->ItemHeight = 37;
			this->subjectListBox->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"Subject 1", L"Subject 2", L"Subject 3", 
				L"Subject 4", L"Subject 5", L"Subject 6", L"Subject 7", L"Subject 8", L"Subject 9", L"Subject 10", L"Maintenance"});
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
			this->instructionsGroupBox->Controls->Add(this->stepProgressGroupBox);
			this->instructionsGroupBox->Controls->Add(this->errorNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->commandNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->normalNavigationGroupBox);
			this->instructionsGroupBox->Controls->Add(this->stepHeaderGroupBox);
			this->instructionsGroupBox->Controls->Add(this->htmlGroupBox);
			this->instructionsGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->instructionsGroupBox->Location = System::Drawing::Point(622, 7);
			this->instructionsGroupBox->Name = L"instructionsGroupBox";
			this->instructionsGroupBox->Size = System::Drawing::Size(605, 990);
			this->instructionsGroupBox->TabIndex = 7;
			this->instructionsGroupBox->TabStop = false;
			this->instructionsGroupBox->Text = L"Instructions";
			// 
			// stepProgressGroupBox
			// 
			this->stepProgressGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->stepProgressGroupBox->Controls->Add(this->programStateEnumTextBox);
			this->stepProgressGroupBox->Controls->Add(this->label1);
			this->stepProgressGroupBox->Controls->Add(this->programStateTextBox);
			this->stepProgressGroupBox->Controls->Add(this->trialsRemainingTextBox);
			this->stepProgressGroupBox->Controls->Add(this->trialsRemainingLabel);
			this->stepProgressGroupBox->Enabled = false;
			this->stepProgressGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->stepProgressGroupBox->Location = System::Drawing::Point(7, 911);
			this->stepProgressGroupBox->Name = L"stepProgressGroupBox";
			this->stepProgressGroupBox->Size = System::Drawing::Size(591, 68);
			this->stepProgressGroupBox->TabIndex = 21;
			this->stepProgressGroupBox->TabStop = false;
			this->stepProgressGroupBox->Visible = false;
			// 
			// programStateEnumTextBox
			// 
			this->programStateEnumTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->programStateEnumTextBox->Location = System::Drawing::Point(122, 26);
			this->programStateEnumTextBox->Name = L"programStateEnumTextBox";
			this->programStateEnumTextBox->Size = System::Drawing::Size(289, 30);
			this->programStateEnumTextBox->TabIndex = 19;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(14, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(50, 47);
			this->label1->TabIndex = 18;
			this->label1->Text = L"State";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// programStateTextBox
			// 
			this->programStateTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->programStateTextBox->Location = System::Drawing::Point(70, 26);
			this->programStateTextBox->Name = L"programStateTextBox";
			this->programStateTextBox->Size = System::Drawing::Size(46, 30);
			this->programStateTextBox->TabIndex = 17;
			this->programStateTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// trialsRemainingTextBox
			// 
			this->trialsRemainingTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->trialsRemainingTextBox->Location = System::Drawing::Point(538, 22);
			this->trialsRemainingTextBox->Name = L"trialsRemainingTextBox";
			this->trialsRemainingTextBox->Size = System::Drawing::Size(34, 30);
			this->trialsRemainingTextBox->TabIndex = 16;
			this->trialsRemainingTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// trialsRemainingLabel
			// 
			this->trialsRemainingLabel->Location = System::Drawing::Point(436, 16);
			this->trialsRemainingLabel->Name = L"trialsRemainingLabel";
			this->trialsRemainingLabel->Size = System::Drawing::Size(96, 47);
			this->trialsRemainingLabel->TabIndex = 15;
			this->trialsRemainingLabel->Text = L"Trials Remaining";
			this->trialsRemainingLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->errorNavigationGroupBox->Location = System::Drawing::Point(7, 911);
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
			this->commandNavigationGroupBox->Location = System::Drawing::Point(7, 911);
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
			this->executeButton->Text = L"Start";
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
			this->normalNavigationGroupBox->Location = System::Drawing::Point(8, 911);
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
			this->stepHeaderGroupBox->Location = System::Drawing::Point(7, 26);
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
			this->stepCounterTextBox->Location = System::Drawing::Point(481, 20);
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
			this->stepHeaderTextBox->Size = System::Drawing::Size(454, 34);
			this->stepHeaderTextBox->TabIndex = 0;
			// 
			// htmlGroupBox
			// 
			this->htmlGroupBox->Controls->Add(this->errorCodeNote);
			this->htmlGroupBox->Controls->Add(this->commandGroupBox);
			this->htmlGroupBox->Controls->Add(this->instructionViewer);
			this->htmlGroupBox->Location = System::Drawing::Point(7, 92);
			this->htmlGroupBox->Name = L"htmlGroupBox";
			this->htmlGroupBox->Size = System::Drawing::Size(591, 813);
			this->htmlGroupBox->TabIndex = 5;
			this->htmlGroupBox->TabStop = false;
			// 
			// errorCodeNote
			// 
			this->errorCodeNote->ForeColor = System::Drawing::Color::DarkMagenta;
			this->errorCodeNote->Location = System::Drawing::Point(23, 737);
			this->errorCodeNote->Name = L"errorCodeNote";
			this->errorCodeNote->Size = System::Drawing::Size(544, 68);
			this->errorCodeNote->TabIndex = 22;
			this->errorCodeNote->Text = L"Note: The code given below is the number of the PAGE that is being displayed onbo" 
				L"ard. To know what error caused this page to be presented you must ask the crewme" 
				L"mber.";
			this->errorCodeNote->Visible = false;
			// 
			// commandGroupBox
			// 
			this->commandGroupBox->Controls->Add(this->commandTextBox);
			this->commandGroupBox->Location = System::Drawing::Point(8, 607);
			this->commandGroupBox->Name = L"commandGroupBox";
			this->commandGroupBox->Size = System::Drawing::Size(575, 123);
			this->commandGroupBox->TabIndex = 3;
			this->commandGroupBox->TabStop = false;
			this->commandGroupBox->Text = L"External Command";
			this->commandGroupBox->Visible = false;
			// 
			// commandTextBox
			// 
			this->commandTextBox->Location = System::Drawing::Point(10, 25);
			this->commandTextBox->Multiline = true;
			this->commandTextBox->Name = L"commandTextBox";
			this->commandTextBox->Size = System::Drawing::Size(555, 92);
			this->commandTextBox->TabIndex = 0;
			// 
			// instructionViewer
			// 
			this->instructionViewer->AllowWebBrowserDrop = false;
			this->instructionViewer->IsWebBrowserContextMenuEnabled = false;
			this->instructionViewer->Location = System::Drawing::Point(10, 25);
			this->instructionViewer->MinimumSize = System::Drawing::Size(20, 20);
			this->instructionViewer->Name = L"instructionViewer";
			this->instructionViewer->ScrollBarsEnabled = false;
			this->instructionViewer->Size = System::Drawing::Size(564, 773);
			this->instructionViewer->TabIndex = 2;
			this->instructionViewer->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
			this->instructionViewer->WebBrowserShortcutsEnabled = false;
			this->instructionViewer->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler(this, &GraspDesktop::instructionViewer_DocumentCompleted);
			// 
			// historyButton
			// 
			this->historyButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyButton->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->historyButton->Location = System::Drawing::Point(438, 925);
			this->historyButton->Margin = System::Windows::Forms::Padding(4);
			this->historyButton->Name = L"historyButton";
			this->historyButton->Size = System::Drawing::Size(127, 44);
			this->historyButton->TabIndex = 22;
			this->historyButton->Text = L"History";
			this->historyButton->UseVisualStyleBackColor = true;
			this->historyButton->Click += gcnew System::EventHandler(this, &GraspDesktop::historyButton_Click);
			// 
			// GraspDesktop
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(1232, 999);
			this->Controls->Add(this->instructionsGroupBox);
			this->Controls->Add(this->navigatorGroupBox);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ForeColor = System::Drawing::SystemColors::ControlText;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Location = System::Drawing::Point(10, 10);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GraspDesktop";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GRASP";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GraspDesktop::GraspDesktop_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &GraspDesktop::GraspDesktop_Shown);
			this->navigatorGroupBox->ResumeLayout(false);
			this->navigatorGroupBox->PerformLayout();
			this->taskGroupBox->ResumeLayout(false);
			this->dexStatusGroupBox->ResumeLayout(false);
			this->dexStatusGroupBox->PerformLayout();
			this->protocolGroupBox->ResumeLayout(false);
			this->subjectGroupBox->ResumeLayout(false);
			this->instructionsGroupBox->ResumeLayout(false);
			this->stepProgressGroupBox->ResumeLayout(false);
			this->stepProgressGroupBox->PerformLayout();
			this->errorNavigationGroupBox->ResumeLayout(false);
			this->errorNavigationGroupBox->PerformLayout();
			this->commandNavigationGroupBox->ResumeLayout(false);
			this->commandNavigationGroupBox->PerformLayout();
			this->normalNavigationGroupBox->ResumeLayout(false);
			this->stepHeaderGroupBox->ResumeLayout(false);
			this->stepHeaderGroupBox->PerformLayout();
			this->htmlGroupBox->ResumeLayout(false);
			this->commandGroupBox->ResumeLayout(false);
			this->commandGroupBox->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	protected: 
		// Here I create my own MessageBox method. So far I am using the standard MessageBox, but
		//  I place it here so that later I can change it to use a larger font.
		System::Windows::Forms::DialogResult MessageBox( String^ message, String^ caption, MessageBoxButtons buttons ) {
			return( MessageBox::Show( message, caption, buttons ) );
		}

		//
		// Methods to communicate progress information to ground via DEX.
		//
		void ConnectToDEX ( void ) {
			// Connect to DEX so that we can send info about the current subject, protocol, etc. to ground.
			fOutputDebugString( "Connecting to DEX ... " );
			dex = new Grasp::DexServicesByProxy();
			char *results_path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( resultsDirectory ).ToPointer();
			char dxl_file[MAX_PATHLENGTH];
			strcpy( dxl_file, results_path );
			strcat( dxl_file, "GraspGUI.dxl" );
			System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( results_path ) );

			dex->Initialize( dxl_file );
			dex->Connect();
			fOutputDebugString( "OK.\n" );
		}

		void SendProgressInfo( void ) {
			int subjectID = (( currentSubject >= 0 ) ? subjectList[currentSubject]->number : 0 );
			int protocolID = (( currentProtocol >= 0 ) ? protocolList[currentProtocol]->number : 0 );
			int taskID = (( currentTask >= 0 ) ? taskList[currentTask]->number : 0 );
			int stepID = (( currentStep >= 0 ) ? stepList[currentStep]->number : 0 );
			dex->SendTaskInfo(  subjectID, protocolID, taskID, stepID, stepExecutionState );

			// We keep track of all tasks that are started and when they stop.
			// While in the GUI we circulate through the list of such events and send
			// them to ground via telemtry as science packets. 
			// If the user spends enough time idling in the GUI, we will eventually see
			// evidence of everything that was done today, even if some of it happened 
			// during LOS.
			if ( actionList->Count > 0 ) {

				GraspActionSlice slice;

				strncpy( slice.ID, "GRSPGUI", sizeof( slice.ID ) );
				for ( unsigned int j = 0; j < ITEMS_IN_SLICE; j++ ) {
					Record ^next = actionList[ nextActionToOutput ];
					slice.record[j].time = next->time;
					slice.record[j].subject = next->subject;
					slice.record[j].protocol = next->protocol;
					slice.record[j].task = next->task;
					slice.record[j].step = next->step;
					slice.record[j].code = next->code;
					nextActionToOutput = ( nextActionToOutput + 1 ) % actionList->Count;
				}
				dex->AddClientSlice( (unsigned char *) &slice, sizeof( slice ) );
			}
		}

		void DisconnectFromDEX ( void ) {
			dex->ResetTaskInfo();
			dex->Disconnect();
			dex->Release();
		}

		System::Void CancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
			// Close the form and exit without any further fanfare.
			Close();
		}

		System::Void TweakForm( void ) {
			// The text box used to hold the header is clipping descenders on lowercase letters.
			// From the internet I found that one needs to turn AutoSize off to change the height.
			// Since I can't do that in the designer, I do it here.
			stepHeaderTextBox->AutoSize = false;
			stepHeaderTextBox->Height = 36;
			stepHeaderTextBox->Text = "";
#ifdef _DEBUG
			// There is a check box that can be used to put the GUI into unit testing mode.
			// In this mode, external commands are not actually executed. Rather, a popup window
			//  is shown allowing the user to simulate return from the command with different error codes.
			// We only show this checkbox in debug builds.
			unitTestingMode->Visible = true;
#endif
		}

		System::Void InitializeMenus( void ) {
			// Initialize the login and task selection menus.
			subjectListBox->Items->Clear();
			protocolListBox->Items->Clear(); 
			protocolListBox->Items->Add( "Please select a User ID ..." );
			taskListBox->Items->Clear();
			ParseSubjectFile( scriptDirectory + "Subjects.sbj" );
			// Show a welcome screen.
			instructionViewer->Navigate( instructionsDirectory + "GraspWelcome.html" );
		}	
		System::Void InitializeForm( void ) {
			TweakForm();
			InitializeMenus();
			// Show progess on ground.
			CreateRefreshTimer( 3000 );
			StartRefreshTimer();
		}

		virtual bool VerifyInterruptStep( void ) {
			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( verifyNext || ( currentStep > 0 && currentStep < nSteps - 1 ) ) {
				System::Windows::Forms::DialogResult response;
				response = MessageBox( "Are you sure you want to interrupt the current step?", "GraspGUI", MessageBoxButtons::YesNo );
				if ( response == System::Windows::Forms::DialogResult::No ) {
					taskListBox->SelectedIndex = currentTask;
					return false;
				}
				verifyNext = false;
				normalNavigationGroupBox->Visible = true;
				normalNavigationGroupBox->Enabled = true;
				commandNavigationGroupBox->Visible = false;
				commandNavigationGroupBox->Enabled = false;
			}
			return true;
		}

		System::Void subjectListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			// If the subject that is selected already is clicked, do nothing.
			if ( subjectListBox->SelectedIndex == currentSubject ) return;
			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( !VerifyInterruptStep() ) return;
			currentSubject = subjectListBox->SelectedIndex;
			// If a subject has been selected, load his or her list of subsessions (protocols). Otherwise clear the subsession menu.
			if ( currentSubject >= 0 ) ParseSessionFile( scriptDirectory + subjectList[currentSubject]->file );
			else protocolListBox->Items->Clear();
			// Don't preselect a protocol.
			currentProtocol = -1;
			// And because no protocol is selected there can be no task or step selected.
			taskListBox->Items->Clear(); 
			currentTask = -1;
			currentStep = -1;
			ShowLogon();
		}

		System::Void protocolListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			// If the protocol that is already selected is clicked, then do nothing.
			if ( protocolListBox->SelectedIndex == currentProtocol ) return;

			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( !VerifyInterruptStep() ) return;

			currentProtocol = protocolListBox->SelectedIndex;
			if ( currentProtocol >= 0 ) ParseProtocolFile( scriptDirectory + protocolList[currentProtocol]->file );
			else taskListBox->Items->Clear();

			if ( taskListBox->Items->Count > 0 ) {
				// Preselect the first task in the newly selected protocol.
				currentStep = 0;
				currentTask = 0;
				taskListBox->SelectedIndex = 0; 
			}
			else {
				currentStep = -1;
				currentTask = -1;
				taskListBox->SelectedIndex = -1; 
			}
		}

		System::Void taskListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

			// Check if the subject was supposed to execute a command and if so, verify that
			// they really want to leave the current step without doing so.
			if ( !VerifyInterruptStep() ) return;

			currentTask = taskListBox->SelectedIndex;

			// If no task is selected, then we continue to show the logon page.
			if ( taskListBox->SelectedIndex < 0 ) {
				ShowLogon();
				return;
			}

			stepHeaderTextBox->Text = taskListBox->Text;
			if (  taskList[taskListBox->SelectedIndex]->type->StartsWith( "SCRIPT" ) ) {
				// Read a series of steps from a script file.
				ParseTaskFile( scriptDirectory + taskList[taskListBox->SelectedIndex]->task_file );
			}
			else {
				// Create a task with a single step and use the info from the task entry to define the step.
				stepList[0] = gcnew Step();
				stepList[0]->number = 1;
				stepList[0]->type = taskList[taskListBox->SelectedIndex]->type;
				if ( taskList[taskListBox->SelectedIndex]->type->StartsWith( "INSTRUCTION" ) ) {
					stepList[0]->instruction = taskList[taskListBox->SelectedIndex]->isolated_step->instruction;
				}
				else if ( taskList[taskListBox->SelectedIndex]->type->StartsWith( "COMMAND" )  
					|| taskList[taskListBox->SelectedIndex]->type->StartsWith( "SYSTEM" ) 
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

			// Disconnect from DEX telemetry services so that the task program can connect.
			dex->Disconnect();

			// Construct the command line, adding results filename root and task info.
			int subjectID = (( currentSubject >= 0 ) ? subjectList[currentSubject]->number : 0 );
			int protocolID = (( currentProtocol >= 0 ) ? protocolList[currentProtocol]->number : 0 );
			int taskID = (( currentTask >= 0 ) ? taskList[currentTask]->number : 0 );
			int stepID = (( currentStep >= 0 ) ? stepList[currentStep]->number : 0 );
			String^ cmdline = "Executables\\GraspHardwareStatus.exe --output=" 
				+ resultsDirectory 
				+ "STATUS_" 
				+ DateTimeString()
				+ " --user=" + subjectID
				+ " --protocol=" + protocolID
				+ " --task=" + taskID
				+ " --step=" + stepID;
			// If the cookie file NoCoda.flg is present, then add a commandline argument to inhibit CODA use.
			if ( FileExists( "NoCoda.flg" ) ) cmdline = cmdline + " --nocoda";

			char *cmd = (char*)(void*)Marshal::StringToHGlobalAnsi( cmdline ).ToPointer() ; 
			WinExec( cmd, SW_SHOW );
			Marshal::FreeHGlobal( IntPtr( cmd ) );

			// Reconnect to DEX for telemetry.
			dex->Connect();
		}

	protected:

		// The following methods deal with navigating through the scripts.
		// I have chosen to group them together in Scripts.cpp, even the ones
		//  initially generated automatically by the Forms designer.

		// Parse the file containing the subject names, IDs and protocols.
		void ParseSubjectFile( String^ filename );
		void ParseSessionFile( String^ filename );
		void ParseProtocolFile( String^ filename );
		void ParseTaskFile( String^ filename );
		// Run the buttons and menus in the GUI.
		void ShowStep( void );
		void ShowLogon( void );
		void ShowHistory( void );
		void nextButton_Click(System::Object^  sender, System::EventArgs^  e);
		void skipButton_Click(System::Object^  sender, System::EventArgs^  e);
		void previousButton_Click(System::Object^  sender, System::EventArgs^  e);
		void startButton_Click(System::Object^  sender, System::EventArgs^  e);
		void SelectNextTask ( void );
		void instructionViewer_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e);
		void retryButton_Click(System::Object^  sender, System::EventArgs^  e);
		void restartButton_Click(System::Object^  sender, System::EventArgs^  e);
		void ignoreButton_Click(System::Object^  sender, System::EventArgs^  e);
		void WriteActionRecord( Record ^record );
		void ReadPastActions( void );

	protected:

		virtual System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			DisconnectFromDEX();
		}

		virtual System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {
			// Make sure that the results directory is present.
			try {
				if ( !Directory::Exists( resultsDirectory ) ) Directory::CreateDirectory( resultsDirectory );
			}
			catch ( Exception^ e ) 
			{
				e = e;
				fAbortMessage( "GraspGUI", "Error creating results directory." );
			}
			// Connect to DEX for telemetry of housekeeping info.
			ConnectToDEX();
			InitializeForm();
			ReadPastActions();
		}

		virtual void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			SendProgressInfo();
		}

	// Add an 'About ...' item to the system menu. 
	#define SYSMENU_ABOUT_ID 0x01

	protected:  virtual void OnHandleCreated( System::EventArgs^ e) override {	

					// Do what one would normally do when the handle is created.
					Form::OnHandleCreated( e );

					// Get a handle to a copy of this form's system (window) menu
					HWND hWnd;
					hWnd = static_cast<HWND>( Handle.ToPointer() );
					HMENU hSysMenu = GetSystemMenu( hWnd, false );
					// Add a separator
					AppendMenu(hSysMenu, MF_SEPARATOR, 0, "" );
					// Add the About menu item
					AppendMenu(hSysMenu, MF_STRING, SYSMENU_ABOUT_ID, "&About …");

				}

	protected:  virtual void WndProc(System::Windows::Forms::Message% m) override {	
					// Test if the About item was selected from the system menu
					if ((m.Msg == WM_SYSCOMMAND) && ((int)m.WParam == SYSMENU_ABOUT_ID))
					{
						fMessageBox( MB_OK, "GraspGUI Version Info", "Source Release:  %s\n         Build Info:  %s", GripGraspSourceRelease, GripGraspBuildInfo );
						return;
					}
					// Do what one would normally do.
					Form::WndProc( m );
				}


	private: System::Void historyButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 ShowHistory();
		 }
};

////////////////////////////////////////////////////////////////////////////////////////////////


}

