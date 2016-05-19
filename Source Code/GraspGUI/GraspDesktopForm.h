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

	private:


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
			this->NavigatorGroupBox->SuspendLayout();
			this->ProgressBox->SuspendLayout();
			this->TaskGroupBox->SuspendLayout();
			this->ProtocolGroupBox->SuspendLayout();
			this->SubjectGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// NavigatorGroupBox
			// 
			this->NavigatorGroupBox->BackColor = System::Drawing::SystemColors::Window;
			this->NavigatorGroupBox->Controls->Add(this->ProgressBox);
			this->NavigatorGroupBox->Controls->Add(this->TaskGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->ProtocolGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->SubjectGroupBox);
			this->NavigatorGroupBox->Controls->Add(this->button1);
			this->NavigatorGroupBox->Controls->Add(this->LogonNext);
			this->NavigatorGroupBox->Controls->Add(this->WelcomeLabel);
			this->NavigatorGroupBox->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->NavigatorGroupBox->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->NavigatorGroupBox->Location = System::Drawing::Point(13, 13);
			this->NavigatorGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Name = L"NavigatorGroupBox";
			this->NavigatorGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->NavigatorGroupBox->Size = System::Drawing::Size(572, 1002);
			this->NavigatorGroupBox->TabIndex = 5;
			this->NavigatorGroupBox->TabStop = false;
			this->NavigatorGroupBox->Text = L"Navigator";
			// 
			// ProgressBox
			// 
			this->ProgressBox->Controls->Add(this->stepListBox);
			this->ProgressBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ProgressBox->Location = System::Drawing::Point(13, 692);
			this->ProgressBox->Margin = System::Windows::Forms::Padding(4);
			this->ProgressBox->Name = L"ProgressBox";
			this->ProgressBox->Padding = System::Windows::Forms::Padding(4);
			this->ProgressBox->Size = System::Drawing::Size(531, 196);
			this->ProgressBox->TabIndex = 8;
			this->ProgressBox->TabStop = false;
			this->ProgressBox->Text = L"Task Progress";
			// 
			// stepListBox
			// 
			this->stepListBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stepListBox->FormattingEnabled = true;
			this->stepListBox->ItemHeight = 36;
			this->stepListBox->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Installation", L"Tracker Alignment", L"Readiness Check", 
				L"Block 1", L"Block 2", L"Block 3", L"Block 4", L"Pause", L"Block 5", L"Block 6", L"Block 7", L"Block 8", L"(end of protocol)"});
			this->stepListBox->Location = System::Drawing::Point(11, 34);
			this->stepListBox->Margin = System::Windows::Forms::Padding(4);
			this->stepListBox->Name = L"stepListBox";
			this->stepListBox->Size = System::Drawing::Size(507, 148);
			this->stepListBox->TabIndex = 5;
			// 
			// TaskGroupBox
			// 
			this->TaskGroupBox->Controls->Add(this->taskListBox);
			this->TaskGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TaskGroupBox->Location = System::Drawing::Point(13, 437);
			this->TaskGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Name = L"TaskGroupBox";
			this->TaskGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->TaskGroupBox->Size = System::Drawing::Size(536, 241);
			this->TaskGroupBox->TabIndex = 7;
			this->TaskGroupBox->TabStop = false;
			this->TaskGroupBox->Text = L"Task";
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
			this->taskListBox->Size = System::Drawing::Size(512, 184);
			this->taskListBox->TabIndex = 5;
			// 
			// ProtocolGroupBox
			// 
			this->ProtocolGroupBox->Controls->Add(this->floatingRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->seatedRadioButton);
			this->ProtocolGroupBox->Controls->Add(this->protocolListBox);
			this->ProtocolGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ProtocolGroupBox->Location = System::Drawing::Point(13, 226);
			this->ProtocolGroupBox->Margin = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Name = L"ProtocolGroupBox";
			this->ProtocolGroupBox->Padding = System::Windows::Forms::Padding(4);
			this->ProtocolGroupBox->Size = System::Drawing::Size(536, 197);
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
			this->seatedRadioButton->Location = System::Drawing::Point(32, 28);
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
			this->protocolListBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Visual - Manual", L"Visual - Visual", L"Manual - Manual"});
			this->protocolListBox->Location = System::Drawing::Point(15, 70);
			this->protocolListBox->Margin = System::Windows::Forms::Padding(4);
			this->protocolListBox->Name = L"protocolListBox";
			this->protocolListBox->Size = System::Drawing::Size(508, 112);
			this->protocolListBox->TabIndex = 5;
			// 
			// SubjectGroupBox
			// 
			this->SubjectGroupBox->Controls->Add(this->subjectListBox);
			this->SubjectGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->SubjectGroupBox->Location = System::Drawing::Point(13, 72);
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
			this->subjectListBox->ItemHeight = 36;
			this->subjectListBox->Items->AddRange(gcnew cli::array< System::Object^  >(14) {L"Subject 1", L"Subject 2", L"Subject 3", 
				L"Subject 4", L"Subject 5", L"Subject 6", L"Subject 7", L"Subject 8", L"Subject 9", L"Subject 10", L"Subject 11", L"Subject 12", 
				L"Demo", L"Maintenance"});
			this->subjectListBox->Location = System::Drawing::Point(11, 34);
			this->subjectListBox->Margin = System::Windows::Forms::Padding(4);
			this->subjectListBox->Name = L"subjectListBox";
			this->subjectListBox->Size = System::Drawing::Size(512, 112);
			this->subjectListBox->TabIndex = 5;
			// 
			// button1
			// 
			this->button1->Enabled = false;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(122, 900);
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
			this->LogonNext->Location = System::Drawing::Point(323, 899);
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
			this->WelcomeLabel->Size = System::Drawing::Size(301, 36);
			this->WelcomeLabel->TabIndex = 0;
			this->WelcomeLabel->Text = L"Welcome to GRASP";
			this->WelcomeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// GraspDesktop
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(597, 988);
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
			this->NavigatorGroupBox->PerformLayout();
			this->ProgressBox->ResumeLayout(false);
			this->TaskGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->ResumeLayout(false);
			this->ProtocolGroupBox->PerformLayout();
			this->SubjectGroupBox->ResumeLayout(false);
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
	private: System::Void LogonNext_Click(System::Object^  sender, System::EventArgs^  e) {

				 // Inhibit activity on the menus.
				 NavigatorGroupBox->Enabled = false;

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
				 NavigatorGroupBox->Enabled = true;


			 }

			 
	private: System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) {
				// RefreshAnimations();
				CreateRefreshTimer( 1000 );
				StartRefreshTimer();
}
	private: System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 StopRefreshTimer();
				 // KillAnimations();
			 }
	private: System::Void GraspDesktop_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 // RefreshAnimations();
			 }
};
}

