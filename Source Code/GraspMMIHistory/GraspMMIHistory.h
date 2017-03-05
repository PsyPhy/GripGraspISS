#pragma once

namespace GraspMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class GraspMMIHistoryForm : public System::Windows::Forms::Form
	{
	public:
		GraspMMIHistoryForm(void)
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
		~GraspMMIHistoryForm()
		{
			if (components)
			{
				delete components;
			}
		}

		// A timer to trigger new polling for packets after a delay.
		System::Windows::Forms::Timer^ timer;
		void CreateRefreshTimer( int interval ) {
			timer = gcnew System::Windows::Forms::Timer;
			timer->Interval = interval;
			timer->Tick += gcnew EventHandler( this, &GraspMMI::GraspMMIHistoryForm::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			timer->Start();
		}
		void StopRefreshTimer( void ) {
			timer->Stop();
		}
		// This is what we do when the timer goes off.
		void OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {

			// Stop the timer so that it does not retrigger until we are done refreshing.
			StopRefreshTimer();
			fOutputDebugString( "\n" );
			fOutputDebugString( "Timer triggered.\n" );
			// Get the realtime science data packets, if any. 
			// ReadTelemetryCache( packetCacheFileRoot );
			StartRefreshTimer();

		}

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TreeView^  historyTree;


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
			System::Windows::Forms::TreeNode^  treeNode5 = (gcnew System::Windows::Forms::TreeNode(L"Node1"));
			System::Windows::Forms::TreeNode^  treeNode6 = (gcnew System::Windows::Forms::TreeNode(L"Node2"));
			System::Windows::Forms::TreeNode^  treeNode7 = (gcnew System::Windows::Forms::TreeNode(L"Node3"));
			System::Windows::Forms::TreeNode^  treeNode8 = (gcnew System::Windows::Forms::TreeNode(L"Node0", gcnew cli::array< System::Windows::Forms::TreeNode^  >(3) {treeNode5, 
				treeNode6, treeNode7}));
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->historyTree = (gcnew System::Windows::Forms::TreeView());
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox3
			// 
			this->groupBox3->AutoSize = true;
			this->groupBox3->Controls->Add(this->historyTree);
			this->groupBox3->Location = System::Drawing::Point(15, 12);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(808, 1083);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"History";
			// 
			// historyTree
			// 
			this->historyTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyTree->Location = System::Drawing::Point(6, 26);
			this->historyTree->Name = L"historyTree";
			treeNode5->Name = L"Node1";
			treeNode5->Text = L"Node1";
			treeNode6->Name = L"Node2";
			treeNode6->Text = L"Node2";
			treeNode7->Name = L"Node3";
			treeNode7->Text = L"Node3";
			treeNode8->Name = L"Node0";
			treeNode8->Text = L"Node0";
			this->historyTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode8});
			this->historyTree->Size = System::Drawing::Size(796, 1031);
			this->historyTree->TabIndex = 0;
			// 
			// GraspMMIHistoryForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(835, 1060);
			this->Controls->Add(this->groupBox3);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
			this->Name = L"GraspMMIHistoryForm";
			this->Text = L"GraspMMI History - Incomplete test program.";
			this->Shown += gcnew System::EventHandler(this, &GraspMMIHistoryForm::Form_Shown);
			this->groupBox3->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		int  ParseLine( char *token[MAX_TOKENS], char *line );
		void ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename );
		void ParseSessionFile( System::Windows::Forms::TreeNode^  parent, String^ filename );
		void ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename );

	private: System::Void Form_Shown(System::Object^  sender, System::EventArgs^  e) {
				 CreateRefreshTimer( 2000 );
				 //StartRefreshTimer();
				 ParseSubjectFile( historyTree, gcnew String( "Scripts\\Subjects.sbj" ) );
			 }

};
}

