#pragma once

#include <stdio.h>
#include "../GraspMMIUtilities/GraspMMIUtilities.h"
#include "../GraspGUI/GraspScripts.h"

#include "../GripGraspVersionControl/GripGraspVersionControl.h"

#define MAX_SLICES	(8*60*60*10)


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

		//
		// Parameters that should be initialized by the caller before running the form.
		//

		int	TimebaseOffset;				// Offset to convert GPS time to UTC.
		String^ packetCacheFileRoot;	// Path to the packet cache files.
		String^ scriptDirectory;		// Path to root file of the GUI menu tree.

		//
		// Arrays to hold the data traces.
		//
		GraspRealtimeDataSlice *graspDataSlice;
		unsigned long	nDataSlices;
		GraspHousekeepingSlice *graspHousekeepingSlice;
		unsigned long	nHousekeepingSlices;

		//
		// Maintain the state of the history parser between refreshes
		// so that we don't have to start from scratch each time.
		// This allows the user to collapse some nodes and they will stay collapsed
		// unless a new task comes in.
		int current_subject;
		int current_protocol;
		int current_task;
		int current_step;
		int first_step;
		TreeNode^ current_task_leaf;
		TreeNode^ previous_task_leaf;
		double current_task_start_time;
		unsigned int task_tree_current_index;

	public:

		GraspMMIHistoryForm(void) :
		  nHousekeepingSlices( 0 )
		  {
			  InitializeComponent();
			  // Allocate memory for display traces.
			  graspHousekeepingSlice = (GraspHousekeepingSlice *) malloc( MAX_SLICES * sizeof( GraspHousekeepingSlice ) );
			  if ( !graspHousekeepingSlice ) fAbortMessage( "GraspMMI", "Error allocating %d frames for Grasp Housekeeping Data", MAX_SLICES );
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
			ReadTelemetryCache( packetCacheFileRoot );
			BuildHistoryTree();
			StartRefreshTimer();

		}
		String^ CreateDurationString( double interval ) {
			char label[256];
			int hours = (int) floor( interval ) / (60 * 60);
			int minutes = ( (int) floor( interval ) % (60 * 60)) / 60;
			int seconds = ( (int) floor( interval ) % 60 );
			sprintf( label, "%02d:%02d:%02d", hours, minutes, seconds );
			return gcnew String( label );
		}
		String^ CreateTimeString( double seconds ) {
			char label[256];
			int day_first = (int) floor(( seconds + TimebaseOffset )) / (24 * 60 * 60);
			int since_midnight = ((int) floor( seconds ) + TimebaseOffset ) % (24 * 60 * 60);
			int hour = since_midnight / (60 * 60);
			int minute = (since_midnight % (60 * 60)) / 60;
			int second = (since_midnight % 60);
			sprintf( label, "%02d:%02d:%02d", hour, minute, second );
			return gcnew String( label );
		}

	private: System::Windows::Forms::TreeView^  visibleHistoryTree;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TreeView^  historyTree;
	private: System::ComponentModel::IContainer^  components;

	private: System::Windows::Forms::ContextMenuStrip^  taskTreeContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  rebuildTree;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
	private: System::Windows::Forms::ToolStripMenuItem^  clearItemErrorHighlight;
	private: System::Windows::Forms::ToolStripMenuItem^  clearAllErrorHighlights;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"Node1"));
			System::Windows::Forms::TreeNode^  treeNode2 = (gcnew System::Windows::Forms::TreeNode(L"Node2"));
			System::Windows::Forms::TreeNode^  treeNode3 = (gcnew System::Windows::Forms::TreeNode(L"Node3"));
			System::Windows::Forms::TreeNode^  treeNode4 = (gcnew System::Windows::Forms::TreeNode(L"This tree will be hidden.", gcnew cli::array< System::Windows::Forms::TreeNode^  >(3) {treeNode1, 
				treeNode2, treeNode3}));
			System::Windows::Forms::TreeNode^  treeNode5 = (gcnew System::Windows::Forms::TreeNode(L"Node1"));
			System::Windows::Forms::TreeNode^  treeNode6 = (gcnew System::Windows::Forms::TreeNode(L"Node2"));
			System::Windows::Forms::TreeNode^  treeNode7 = (gcnew System::Windows::Forms::TreeNode(L"Node3"));
			System::Windows::Forms::TreeNode^  treeNode8 = (gcnew System::Windows::Forms::TreeNode(L"Node0", gcnew cli::array< System::Windows::Forms::TreeNode^  >(3) {treeNode5, 
				treeNode6, treeNode7}));
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->historyTree = (gcnew System::Windows::Forms::TreeView());
			this->visibleHistoryTree = (gcnew System::Windows::Forms::TreeView());
			this->taskTreeContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->rebuildTree = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->clearItemErrorHighlight = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearAllErrorHighlights = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox3->SuspendLayout();
			this->taskTreeContextMenu->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->historyTree);
			this->groupBox3->Controls->Add(this->visibleHistoryTree);
			this->groupBox3->Location = System::Drawing::Point(13, 12);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(437, 874);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"History";
			// 
			// historyTree
			// 
			this->historyTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyTree->Location = System::Drawing::Point(126, 0);
			this->historyTree->Name = L"historyTree";
			treeNode1->Name = L"Node1";
			treeNode1->Text = L"Node1";
			treeNode2->Name = L"Node2";
			treeNode2->Text = L"Node2";
			treeNode3->Name = L"Node3";
			treeNode3->Text = L"Node3";
			treeNode4->Name = L"Node0";
			treeNode4->Text = L"This tree will be hidden.";
			this->historyTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode4});
			this->historyTree->Size = System::Drawing::Size(311, 36);
			this->historyTree->TabIndex = 0;
			this->historyTree->Visible = false;
			// 
			// visibleHistoryTree
			// 
			this->visibleHistoryTree->ContextMenuStrip = this->taskTreeContextMenu;
			this->visibleHistoryTree->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->visibleHistoryTree->Location = System::Drawing::Point(6, 21);
			this->visibleHistoryTree->Name = L"visibleHistoryTree";
			treeNode5->Name = L"Node1";
			treeNode5->Text = L"Node1";
			treeNode6->Name = L"Node2";
			treeNode6->Text = L"Node2";
			treeNode7->Name = L"Node3";
			treeNode7->Text = L"Node3";
			treeNode8->Name = L"Node0";
			treeNode8->Text = L"Node0";
			this->visibleHistoryTree->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode8});
			this->visibleHistoryTree->Size = System::Drawing::Size(425, 845);
			this->visibleHistoryTree->TabIndex = 1;
			this->visibleHistoryTree->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &GraspMMIHistoryForm::visibleHistoryTree_NodeMouseClick);
			// 
			// taskTreeContextMenu
			// 
			this->taskTreeContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->rebuildTree, 
				this->toolStripSeparator4, this->clearItemErrorHighlight, this->clearAllErrorHighlights});
			this->taskTreeContextMenu->Name = L"taskTreeContextMenu";
			this->taskTreeContextMenu->ShowImageMargin = false;
			this->taskTreeContextMenu->Size = System::Drawing::Size(179, 76);
			this->taskTreeContextMenu->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &GraspMMIHistoryForm::taskTreeContextMenu_ItemClicked);
			// 
			// rebuildTree
			// 
			this->rebuildTree->Name = L"rebuildTree";
			this->rebuildTree->Size = System::Drawing::Size(178, 22);
			this->rebuildTree->Text = L"Rebuild Tree";
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(175, 6);
			// 
			// clearItemErrorHighlight
			// 
			this->clearItemErrorHighlight->Name = L"clearItemErrorHighlight";
			this->clearItemErrorHighlight->Size = System::Drawing::Size(178, 22);
			this->clearItemErrorHighlight->Text = L"Clear This Error Highlight";
			// 
			// clearAllErrorHighlights
			// 
			this->clearAllErrorHighlights->Name = L"clearAllErrorHighlights";
			this->clearAllErrorHighlights->Size = System::Drawing::Size(178, 22);
			this->clearAllErrorHighlights->Text = L"Clear All Error Highlights";
			// 
			// GraspMMIHistoryForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 17);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(462, 897);
			this->Controls->Add(this->groupBox3);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
			this->Name = L"GraspMMIHistoryForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspMMI History";
			this->Shown += gcnew System::EventHandler(this, &GraspMMIHistoryForm::Form_Shown);
			this->groupBox3->ResumeLayout(false);
			this->taskTreeContextMenu->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

		void ReadTelemetryCache( String^ root );
		void BuildHistoryTree( void );
		void RebuildHistoryTree( void );

		int  ParseLine( char *token[MAX_TOKENS], char *line );
		void ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename );
		void ParseSessionFile( System::Windows::Forms::TreeNode^  parent, String^ filename );
		void ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename );

	private: System::Void Form_Shown(System::Object^  sender, System::EventArgs^  e) {
				 ParseSubjectFile( historyTree, scriptDirectory + gcnew String( "Subjects.sbj" ) );
				 visibleHistoryTree->Nodes->Clear();
				 CreateRefreshTimer( 2000 );
				 StartRefreshTimer();
				 ParseSubjectFile( historyTree, gcnew String( "Scripts\\Subjects.sbj" ) );
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
						char pwd[MAX_PATH];
						int bytes = GetCurrentDirectory( sizeof( pwd ), pwd );

						// We need InteropServics in order to convert a String to a char *.
						using namespace System::Runtime::InteropServices;

						// Converts the String into a char *.
						// Don't forget to free it when exiting.
						char *cache_root = (char*)(void*)Marshal::StringToHGlobalAnsi( packetCacheFileRoot ).ToPointer();
						char hk_file[MAX_PATHLENGTH];

						// Create the paths to the housekeeping packet file, based on the root and the packet type.
						CreateGripPacketCacheFilename( hk_file, sizeof( hk_file ), GRIP_HK_BULK_PACKET, cache_root );

						fMessageBox( MB_OK, 
							"GraspMMIHistory Version Info", "Source Release:  %s\n         Build Info:  %s\n\nPacket Cache:\n\n  %s\n\nExecuting in:\n\n  %s\n", 
							GripGraspSourceRelease, GripGraspBuildInfo, hk_file, pwd );
						// Release the memory used to create the ANSI string.
						Marshal::FreeHGlobal( IntPtr( cache_root ) );

						return;
					}
					// Do what one would normally do.
					Form::WndProc( m );
				}


	private: 
		System::Void visibleHistoryTree_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
			// Normal behavior for a left click is to select the node, but right click leaves the selection unchanged.
			// Here we force any click to select the node, which is convenient for setting the target of the context menu.
			visibleHistoryTree->SelectedNode = e->Node;
			if ( e->Node->Text->EndsWith( "!" ) && e->Node->ForeColor == System::Drawing::Color::Red ) clearItemErrorHighlight->Enabled = true;
			else clearItemErrorHighlight->Enabled = false;

		}
		System::Void taskTreeContextMenu_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			TreeNode^ node = visibleHistoryTree->SelectedNode;
			StopRefreshTimer();
			if ( e->ClickedItem == clearItemErrorHighlight ) {
				// Completed nodes that have errors persist as red in the tree.
				// Here we give the user the possibility to clear the red color as a way of acknowledging the error.
				// This makes newly occuring errors stand out more.
				if ( node->Text->EndsWith( "!" ) && node->ForeColor == System::Drawing::Color::Red ) {
					node->ForeColor = System::Drawing::SystemColors::WindowText;
				}
			}
			else if (  e->ClickedItem == clearAllErrorHighlights ) {
				// They can all be cleared as once.
				ClearAllErrorHighlights();
			}
			else if (  e->ClickedItem == rebuildTree ) {
				// Rebuilding the tree restores all of the error hightlights.
				RebuildHistoryTree();
			}
			StartRefreshTimer();
		}		 

		System::Void ClearAllErrorHighlights( void ) {
			StopRefreshTimer();
			for ( int i = 0; i < visibleHistoryTree->Nodes->Count; i++ ) {
				TreeNode^ subject_node = visibleHistoryTree->Nodes[i];
				for ( int i = 0; i < subject_node->Nodes->Count; i++ ) {
					TreeNode^ protocol_node = subject_node->Nodes[i];
					for ( int i = 0; i < protocol_node->Nodes->Count; i++ ) {
						TreeNode^ task_node = protocol_node->Nodes[i];
						for ( int i = 0; i < task_node->Nodes->Count; i++ ) {
							TreeNode^ task_leaf = task_node->Nodes[i];
							if ( task_leaf->Text->EndsWith( "!" ) && task_leaf->ForeColor == System::Drawing::Color::Red ) {
								task_leaf->ForeColor = System::Drawing::SystemColors::WindowText;
							}
						}
					}
				}
			}
			StartRefreshTimer();
		}		 

	};
}

