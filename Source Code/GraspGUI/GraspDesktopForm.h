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

		array<GraspGUI::Subject ^> ^subjectList;
		int nSubjects;
		array<GraspGUI::Protocol ^> ^protocolList;
		int nProtocols;
		array<GraspGUI::Task ^> ^taskList;
		int nTasks;
		array<GraspGUI::Page ^> ^pageList;
		int nPages;
		int currentPage;

		// Parse the file containing the subject names, IDs and protocols.
		int ParseLine( char *token[], char *line );
		void ParseSubjectFile( String^ filename );
		void ParseSessionFile( String^ filename );
		void ParseProtocolFile( String^ filename );
		void ParseInstructionFile( String^ filename );
		void RunPages( void );

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
			subjectList = gcnew array<GraspGUI::Subject ^>(MAX_SUBJECTS);
			nSubjects = 0;
			protocolList = gcnew array<GraspGUI::Protocol ^>(MAX_PROTOCOLS);
			nProtocols = 0;
			taskList = gcnew array<GraspGUI::Task ^>(MAX_TASKS);
			nTasks = 0;
			pageList = gcnew array<GraspGUI::Page ^>(MAX_PAGES);
			nPages = 0;
			currentPage = 0;
			// Standard Windows Forms initialization.
			InitializeComponent();
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


	private: System::Windows::Forms::GroupBox^  NavigatorGroupBox;
	private: System::Windows::Forms::GroupBox^  SubjectGroupBox;
	private: System::Windows::Forms::ListBox^  subjectListBox;
	private: System::Windows::Forms::GroupBox^  TaskGroupBox;
	private: System::Windows::Forms::ListBox^  taskListBox;
	private: System::Windows::Forms::GroupBox^  ProtocolGroupBox;
	private: System::Windows::Forms::ListBox^  protocolListBox;
	private: System::Windows::Forms::RadioButton^  floatingRadioButton;
	private: System::Windows::Forms::RadioButton^  seatedRadioButton;
	private: System::Windows::Forms::Button^  statusButton;

	private: System::Windows::Forms::Button^  quitButton;
	private: System::Windows::Forms::Button^  previousButton;


	private: System::Windows::Forms::Button^  nextButton;


	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::WebBrowser^  instructionViewer;
	private: System::Windows::Forms::Button^  goButton;

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
			this->NavigatorGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->goButton = (gcnew System::Windows::Forms::Button());
			this->statusButton = (gcnew System::Windows::Forms::Button());
			this->quitButton = (gcnew System::Windows::Forms::Button());
			this->TaskGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->taskListBox = (gcnew System::Windows::Forms::ListBox());
			this->ProtocolGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->floatingRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->seatedRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->protocolListBox = (gcnew System::Windows::Forms::ListBox());
			this->SubjectGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->subjectListBox = (gcnew System::Windows::Forms::ListBox());
			this->previousButton = (gcnew System::Windows::Forms::Button());
			this->nextButton = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->instructionViewer = (gcnew System::Windows::Forms::WebBrowser());
			this->NavigatorGroupBox->SuspendLayout();
			this->TaskGroupBox->SuspendLayout();
			this->ProtocolGroupBox->SuspendLayout();
			this->SubjectGroupBox->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// NavigatorGroupBox
			// 
			this->NavigatorGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->NavigatorGroupBox->Controls->Add(this->goButton);
			this->NavigatorGroupBox->Controls->Add(this->statusButton);
			this->NavigatorGroupBox->Controls->Add(this->quitButton);
			this->NavigatorGroupBox->Controls->Add(this->TaskGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->ProtocolGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->SubjectGroupBox);
			this->NavigatorGroupBox->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->NavigatorGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->NavigatorGroupBox->Location = System::Drawing::Point(13, 13);
			this->NavigatorGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Name = L"NavigatorGroupBox";
			this->NavigatorGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Size = System::Drawing::Size(566, 952);
			this->NavigatorGroupBox->TabIndex = 5;
			this->NavigatorGroupBox->TabStop = false;
			this->NavigatorGroupBox->Text = L"Navigator";
			// 
			// goButton
			// 
			this->goButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->goButton->Location = System::Drawing::Point(403, 892);
			this->goButton->Margin = System::Windows::Forms::Padding(4);
			this->goButton->Name = L"goButton";
			this->goButton->Size = System::Drawing::Size(127, 44);
			this->goButton->TabIndex = 10;
			this->goButton->Text = L"Go";
			this->goButton->UseVisualStyleBackColor = true;
			this->goButton->Click += gcnew System::EventHandler(this, &GraspDesktop::LogonGo_Click);
			// 
			// statusButton
			// 
			this->statusButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->statusButton->Location = System::Drawing::Point(220, 892);
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
			this->quitButton->Location = System::Drawing::Point(37, 892);
			this->quitButton->Margin = System::Windows::Forms::Padding(4);
			this->quitButton->Name = L"quitButton";
			this->quitButton->Size = System::Drawing::Size(127, 44);
			this->quitButton->TabIndex = 8;
			this->quitButton->Text = L"Quit";
			this->quitButton->UseVisualStyleBackColor = true;
			this->quitButton->Click += gcnew System::EventHandler(this, &GraspDesktop::CancelButton_Click);
			// 
			// TaskGroupBox
			// 
			this->TaskGroupBox->Controls->Add(this->taskListBox);
			this->TaskGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TaskGroupBox->Location = System::Drawing::Point(13, 501);
			this->TaskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Name = L"TaskGroupBox";
			this->TaskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Size = System::Drawing::Size(536, 378);
			this->TaskGroupBox->TabIndex = 7;
			this->TaskGroupBox->TabStop = false;
			this->TaskGroupBox->Text = L"Task";
			this->TaskGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::TaskGroupBox_Enter);
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
			// ProtocolGroupBox
			// 
			this->ProtocolGroupBox->Controls->Add(this->floatingRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->seatedRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->protocolListBox);
			this->ProtocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ProtocolGroupBox->Location = System::Drawing::Point(13, 237);
			this->ProtocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Name = L"ProtocolGroupBox";
			this->ProtocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Size = System::Drawing::Size(536, 260);
			this->ProtocolGroupBox->TabIndex = 6;
			this->ProtocolGroupBox->TabStop = false;
			this->ProtocolGroupBox->Text = L"Protocol";
			this->ProtocolGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::ProtocolGroupBox_Enter);
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
			// SubjectGroupBox
			// 
			this->SubjectGroupBox->Controls->Add(this->subjectListBox);
			this->SubjectGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->SubjectGroupBox->Location = System::Drawing::Point(13, 26);
			this->SubjectGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->SubjectGroupBox->Name = L"SubjectGroupBox";
			this->SubjectGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->SubjectGroupBox->Size = System::Drawing::Size(536, 198);
			this->SubjectGroupBox->TabIndex = 5;
			this->SubjectGroupBox->TabStop = false;
			this->SubjectGroupBox->Text = L"User ID";
			this->SubjectGroupBox->Enter += gcnew System::EventHandler(this, &GraspDesktop::SubjectGroupBox_Enter);
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
			// previousButton
			// 
			this->previousButton->Enabled = false;
			this->previousButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->previousButton->Location = System::Drawing::Point(163, 892);
			this->previousButton->Margin = System::Windows::Forms::Padding(4);
			this->previousButton->Name = L"previousButton";
			this->previousButton->Size = System::Drawing::Size(127, 44);
			this->previousButton->TabIndex = 4;
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
			this->nextButton->Location = System::Drawing::Point(329, 892);
			this->nextButton->Margin = System::Windows::Forms::Padding(4);
			this->nextButton->Name = L"nextButton";
			this->nextButton->Size = System::Drawing::Size(127, 44);
			this->nextButton->TabIndex = 3;
			this->nextButton->Text = L"Next >>";
			this->nextButton->UseVisualStyleBackColor = true;
			this->nextButton->Visible = false;
			this->nextButton->Click += gcnew System::EventHandler(this, &GraspDesktop::nextButton_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->groupBox2);
			this->groupBox1->Controls->Add(this->previousButton);
			this->groupBox1->Controls->Add(this->nextButton);
			this->groupBox1->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->groupBox1->Location = System::Drawing::Point(598, 13);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(584, 952);
			this->groupBox1->TabIndex = 7;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Instructions";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->instructionViewer);
			this->groupBox2->Location = System::Drawing::Point(14, 26);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(557, 852);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			// 
			// instructionViewer
			// 
			this->instructionViewer->AllowWebBrowserDrop = false;
			this->instructionViewer->IsWebBrowserContextMenuEnabled = false;
			this->instructionViewer->Location = System::Drawing::Point(8, 34);
			this->instructionViewer->MinimumSize = System::Drawing::Size(20, 20);
			this->instructionViewer->Name = L"instructionViewer";
			this->instructionViewer->ScrollBarsEnabled = false;
			this->instructionViewer->Size = System::Drawing::Size(540, 796);
			this->instructionViewer->TabIndex = 2;
			this->instructionViewer->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
			this->instructionViewer->WebBrowserShortcutsEnabled = false;
			// 
			// GraspDesktop
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(1194, 988);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->NavigatorGroupBox);
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
			this->NavigatorGroupBox->ResumeLayout(false);
			this->TaskGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->PerformLayout();
			this->SubjectGroupBox->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion


	private: System::Windows::Forms::DialogResult MessageBox( String^ message, String^ caption, MessageBoxButtons buttons ) {
				 // Here I create my own MessageBox method. So far I am using the standard MessageBox, but
				 //  I place it here so that later I can change it to use a larger font.
				 return( MessageBox::Show( message, caption, buttons ) );
			 }
	private: System::Void CancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 // Close the form and exit without any further fanfare.
				 Close();
			 }

	private: System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {
				 // RefreshAnimations();
				 instructionViewer->Navigate( "C:/Users/Joe/Desktop/GRASPonISS/" + "Instructions/GraspWelcome.html" );
				 // CreateRefreshTimer( 1000 );
				 // StartRefreshTimer();
				 subjectListBox->Items->Clear();
				 protocolListBox->Items->Clear();
				 taskListBox->Items->Clear();
				 ParseSubjectFile( "Scripts\\Subjects.gsp" );
			 }
	private: System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 // StopRefreshTimer();
				 // KillAnimations();
			 }
	private: System::Void GraspDesktop_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 // RefreshAnimations();
			 }
	private: System::Void SubjectGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void ProtocolGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void TaskGroupBox_Enter(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void subjectListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 seatedRadioButton->Checked = false;
				 floatingRadioButton->Checked = false;
				 int item_index = subjectListBox->SelectedIndex;
				 protocolListBox->ClearSelected();
				 taskListBox->ClearSelected();
				 ParseSessionFile( "Scripts\\" + subjectList[item_index]->file );
			 }
	private: System::Void protocolListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 int item_index = protocolListBox->SelectedIndex;
				 taskListBox->ClearSelected();
				 ParseProtocolFile( "Scripts\\" + protocolList[item_index]->file );
				 taskListBox->SelectedIndex = 0;
			 }
	private: System::Void statusButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 MessageBox( "Will show payload status.", "GRASP@ISS", MessageBoxButtons::OK );
				 // system( "bin\\WinSCP.com /command \"open ftp://administrator:dex@10.80.12.103\" \"cd DATA1\" \"cd DATA\" \"cd glog\" \"ls\" \"get *\" \"exit\" & pause" );
			 }


	private: System::Void RunTask( String^ command );


	private: System::Void LogonGo_Click(System::Object^  sender, System::EventArgs^  e) {

				 // Inhibit activity on the menus.
				 NavigatorGroupBox->Enabled = false;

				 // Check that we have all the information that we need to proceed.
				 if ( subjectListBox->SelectedIndex < 0 ) MessageBox( "Please select a User ID.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( !seatedRadioButton->Checked && !floatingRadioButton->Checked ) MessageBox( "Please select Seated or Quasi-Freefloating.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( protocolListBox->SelectedIndex < 0 ) MessageBox( "Please select a Protocol.", "GRASP@ISS", MessageBoxButtons::OK );
				 else if ( taskListBox->SelectedIndex < 0 ) MessageBox( "Please select a Task.", "GRASP@ISS", MessageBoxButtons::OK );
				 // If all is good, go on to do the specified task.
				 else {
					 // Run the task.
					 instructionViewer->Navigate( "C:/Users/Joe/Desktop/GRASPonISS/Instructions/GraspRunning.html" );
					 int task = taskListBox->SelectedIndex;
					 String^ cmd = ( seatedRadioButton->Checked ?  taskList[task]->seatedCmd : taskList[task]->floatingCmd );
					 if ( cmd->EndsWith( ".gsp" )) {
						 ParseInstructionFile( "Scripts\\" + cmd );
						 RunPages();
					 }
					 else {
						 RunTask( cmd );
						 SelectNextTask();
					 }

					 // Return to the Navigator.
					 NavigatorGroupBox->Show();

				 }
				 NavigatorGroupBox->Enabled = true;
			 }
	private: System::Void nextButton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void previousButton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void GraspDesktop::SelectNextTask ( void );
	private: System::Void taskListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 	instructionViewer->Navigate( "C:/Users/Joe/Desktop/GRASPonISS/Instructions/GraspReady.html" );
}
};
}

