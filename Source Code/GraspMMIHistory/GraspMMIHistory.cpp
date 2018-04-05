// GraspMMIHistory.cpp : main project file.

#include "stdafx.h"
#include "../GraspGUI/GraspScripts.h"
#include "GraspMMIHistory.h"
#include "GraspMMIHistoryStartup.h"

// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

using namespace GraspMMI;
using namespace GraspMMIHistory;
using namespace Grasp;
using namespace PsyPhy;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Default locations for packet buffer files.
	String^ packetRoot = gcnew String( "GraspPackets" );
	String^ scriptDirectory = gcnew String( "Scripts\\" );

	// Parse the command line arguments.
	if ( args->Length > 0 ) packetRoot = args[0];	// Where to look for packets written by GripGroundMonitorClient.exe
	if ( args->Length > 1 ) scriptDirectory = args[1];	// Where to look for the GraspGUI script structure.

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Wait for the first packets.
	GraspMMIHistoryStartup^ startupForm = gcnew GraspMMIHistoryStartup( packetRoot );
	System::Windows::Forms::DialogResult result = startupForm->ShowDialog();

	if ( result != System::Windows::Forms::DialogResult::Cancel ) {
		// Create the main window.
		GraspMMIHistoryForm^	mainForm = gcnew GraspMMIHistoryForm();
		// Set parameters from the command line, or others.
		mainForm->packetCacheFileRoot = packetRoot;
		mainForm->scriptDirectory = scriptDirectory;
		// Run the form.
		Application::Run( mainForm );
	}
	return 0;
}

#define PARSER_BUFFERS	16
#define iswhite( c ) ( c <= 0x20 || c >= 0x7f )

int GraspMMIHistoryForm::ParseLine( char *token[MAX_TOKENS], char *line ) {

	static char return_tokens[PARSER_BUFFERS][1024];
	static int circular = 0;

	char *tkn, *chr;
	unsigned int	 n = 0;

	if ( strlen( line ) > sizeof( return_tokens[circular] ) ) {
		fprintf( stderr, "Line too long.\n%s\n", line );
		exit( -1 );
	}
	strcpy( return_tokens[circular], line );
	tkn = return_tokens[circular];
	while ( *tkn && n < MAX_TOKENS - 1 ) {
		/* Skip to first non-white character. */
		while ( iswhite( *tkn ) && *tkn ) tkn++;
		if ( *tkn == 0 ) break;
		/* Find the next delimiter */
		for ( chr = tkn; *chr; chr++ ) {
			if ( *chr == ';' || *chr == '\n' || *chr == '\r' ) break;
		}
		*chr = 0;
		/* Record this as a valid token. */
		token[n++] = tkn;
		if ( chr >= return_tokens[circular] + strlen( line ) ) {
			token[n] = NULL;
			break;
		}
		tkn = chr + 1;
	}

	/* Last token shall be a null pointer by definition. */
	token[n] = NULL;

	/* Next time around use a different buffer for the strings. */
	circular = ( circular + 1 ) % PARSER_BUFFERS;

	return( n );

}
void GraspMMIHistoryForm::ParseSubjectFile( System::Windows::Forms::TreeView^ tree, String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	System::Windows::Forms::TreeNode^  node;

	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	tree->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 4 ) {
			int number;
			sscanf( token[0], "%d", &number );
			node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[1] ) + " " + gcnew String( token[2] ) ));
			node->Tag = number;
			ParseSessionFile( node, (gcnew String( "Scripts\\" )) + (gcnew String( token[3] )));
			tree->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in subject file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
	System::Windows::Forms::TreeNode^  task = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unrecognized Task" ) ));
	task->Tag = -1;
	System::Windows::Forms::TreeNode^  protocol = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unrecognized Protocol" ) ));
	protocol->Tag = -1;
	System::Windows::Forms::TreeNode^  subject = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unrecognized Subject" ) ));
	subject->Tag = -1;
	protocol->Nodes->Add( task );
	subject->Nodes->Add( protocol );
	tree->Nodes->Add( subject );

}

void GraspMMIHistoryForm::ParseSessionFile( System::Windows::Forms::TreeNode^  subject, String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );

	subject->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 3 ) {
			int number;
			sscanf( token[0], "%d", &number );
			System::Windows::Forms::TreeNode^ node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[1] ) ));
			subject->Nodes->Add( node );
			node->Tag = number;
			ParseProtocolFile( node, (gcnew String( "Scripts\\" )) + (gcnew String( token[2] )));
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in session file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
	System::Windows::Forms::TreeNode^  task = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unecognized Task" ) ));
	task->Tag = -1;
	System::Windows::Forms::TreeNode^  protocol = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unrecognized Protocol" ) ));
	protocol->Tag = -1;
	protocol->Nodes->Add( task );
	subject->Nodes->Add( protocol );

}

void GraspMMIHistoryForm::ParseProtocolFile( System::Windows::Forms::TreeNode^ protocol, String ^filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	protocol->Nodes->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens > 2 || tokens < MAX_TOKENS ) {
			int number;
			sscanf( token[0], "%d", &number );
			System::Windows::Forms::TreeNode^ node = gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[2] ));
			node->Tag = number;
			protocol->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in protocol file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
	System::Windows::Forms::TreeNode^  task = (gcnew System::Windows::Forms::TreeNode( gcnew String( "Unrecognized Task" ) ));
	task->Tag = -1;
	protocol->Nodes->Add( task );

}

// Read the cached packets.
void GraspMMIHistoryForm::ReadTelemetryCache( String^ root ) {
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *filename_root = (char*)(void*)Marshal::StringToHGlobalAnsi( root ).ToPointer();
	nHousekeepingSlices = GetHousekeepingTrace( graspHousekeepingSlice, MAX_SLICES, filename_root );
	Marshal::FreeHGlobal( IntPtr(filename_root) );
}

void GraspMMIHistoryForm::RebuildHistoryTree( void ) {
	visibleHistoryTree->Nodes->Clear();
	task_tree_current_index = 0;
	BuildHistoryTree();
}

void GraspMMIHistoryForm::BuildHistoryTree( void ) {

	// Starting from where we left off, look for the beginning and end of each task.
	for ( unsigned int index = task_tree_current_index; index < nHousekeepingSlices; index++ ) {

		// Get the pertinent data from the housekeeping packet.
		int subject = (int) graspHousekeepingSlice[index].userID;
		int protocol = (int) graspHousekeepingSlice[index].protocolID;
		int task = (int) graspHousekeepingSlice[index].taskID;
		int step = (int) graspHousekeepingSlice[index].stepID;

		// Ignore 0000 combinations and whenever there is missing data.
		if ( subject != 0 && protocol != 0 && task != 0 && subject != MISSING_INT && protocol != MISSING_INT && task != MISSING_INT ) {

			// Look for transitions in the subject, protocol and task IDs.
			if ( task != current_task || protocol != current_protocol || subject != current_subject ) {

				// Tasks that are older than the task just completed are maintained in normal color text.
				if ( previous_task_leaf && previous_task_leaf->ForeColor !=  System::Drawing::Color::Red ) previous_task_leaf->ForeColor = System::Drawing::SystemColors::WindowText;

				// If the task has changed from what was the running task, then we have a new task.
				// Mark when the previous task ended. NB: This may not be correct if we had an LOS.
				if ( current_task_leaf ) {
					if ( current_task_leaf->Text->StartsWith( "Initiated:" ) ) {
						current_task_leaf->Text += 
							"Finished: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) +
							" Duration: " + CreateDurationString( graspHousekeepingSlice[index].absoluteTime - current_task_start_time ) +
							current_task_leaf->Tag;
					}
					// Set color of the task that just completed according to completion code.
					if ( current_task_leaf->Text->EndsWith( "!"  ) ) current_task_leaf->ForeColor = System::Drawing::Color::Red;
					else if ( current_task_leaf->Text->EndsWith( "OK"  ) ) current_task_leaf->ForeColor = System::Drawing::Color::Green;
					// No completion codes were noted, so mark it as done, but neurtrally as to good or bad.
					else current_task_leaf->ForeColor = System::Drawing::Color::Blue;
					// Keep track of this node so that we can change its color back to normal.
					previous_task_leaf = current_task_leaf;
				}

				// Find the node corresponding to the new task.
				// First see if it is present in the tree that is visible to the user.
				TreeNode^ subject_node = nullptr;
				int j;
				for ( j = 0; j < visibleHistoryTree->Nodes->Count; j++ ) {
					if ( (int) visibleHistoryTree->Nodes[j]->Tag == subject ) {
						subject_node = visibleHistoryTree->Nodes[j];
						break;
					}
				}
				// If we did not find it in the tree, get it from the full tree.
				int i;

				if ( !subject_node ) {
					for ( i = 0; i < historyTree->Nodes->Count - 1; i++ ) {
						if (  (int) historyTree->Nodes[i]->Tag == subject ) break;
					}
					subject_node = ( TreeNode^ ) historyTree->Nodes[i]->Clone();
					fAbortMessageOnCondition( !subject_node, "GraspMMI", "Could not find subject ID %d in history tree.", subject );
					// Insert it into the visible tree according to the subject ID number.
					for ( j = 0; j < visibleHistoryTree->Nodes->Count ; j++ ) {
						if ( (int) visibleHistoryTree->Nodes[j]->Tag > subject ) break;
					}
					visibleHistoryTree->Nodes->Insert( j, subject_node );
				}
				TreeNode^ protocol_node = nullptr;
				for ( int i = 0; i < subject_node->Nodes->Count; i++ ) {
					if (  (int) subject_node->Nodes[i]->Tag == protocol ) protocol_node = subject_node->Nodes[i];
				}
				protocol_node = subject_node->Nodes[i];
				fAbortMessageOnCondition( !protocol_node, "GraspMMI", "Could not find protocol ID %d for subject ID %d in history tree.", protocol, subject );
				TreeNode^ task_node = nullptr;
				for ( i = 0; i < protocol_node->Nodes->Count - 1; i++ ) {
					if (  (int) protocol_node->Nodes[i]->Tag == task ) break;
				}
				task_node = protocol_node->Nodes[i];
				fAbortMessageOnCondition( !task_node, "GraspMMI", "Could not find task ID %d for protocol ID %d and subject ID %d in history tree.", task, protocol, subject );

				// Show that a new instance of the task has been selected by creating a new leaf.
				task_node->Nodes->Add( "Selected: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) );
				// Keep track of the leaf so that we can annotate it when the task finishes.
				current_task_leaf = task_node->Nodes[task_node->Nodes->Count - 1];
				// Highlight the active task.
				current_task_leaf->ForeColor = System::Drawing::Color::Purple;
				current_task_leaf->Tag = gcnew String( "" );
				current_task_start_time = graspHousekeepingSlice[index].absoluteTime;
				// When a new instance of a task starts, we want the user to notice. So we expand the node corresponding to that task.
				// But we do not expand the full tree. This allows the user to collapes a protocol (session) that is no longer active
				// and it will stay collapsed.
				subject_node->Expand();
				protocol_node->Expand();
				task_node->Expand();
				// Note the characteristics of the new instance so that we can detect changes.
				current_subject = subject;
				current_protocol = protocol;
				current_task = task;
				current_step = -1;
				first_step = step;
			}
			// Because tasks get selected automatically when the previous task is finished, changes of the task ID 
			// do not necessarily mean that the subject has started the task. We look for some activity to indicate
			// that the task has actually started and we update the indicator and start time accordingly.
			else if ( current_task_leaf->Text->StartsWith( "Selected:" ) && ( step != first_step || graspHousekeepingSlice[index].scriptEngine >= STEP_EXECUTING )) {
				// Activity has been detected, so change from "Selected" to "Initiated" and update the time.
				current_task_leaf->Text = "Initiated: " + CreateTimeString( graspHousekeepingSlice[index].absoluteTime ) + " GMT ";
				current_task_start_time = graspHousekeepingSlice[index].absoluteTime;
				// Change the color to attract the attention of the user.
				current_task_leaf->ForeColor = System::Drawing::Color::Magenta;
			}
			// If a task has been initiated, look for indications that it completed successfully or not.
			else if ( current_task_leaf->Text->StartsWith( "Initiated:" ) && ( graspHousekeepingSlice[index].scriptEngine >= STEP_FINISHED_ABNORMAL )) {
				// A task has exited with an error code.
				// Prepare the tag to be added to the text of the leaf to show that there was a problem during the execution of this task.
				current_task_leaf->Tag = gcnew String( " !!!" );
				// Add a leaf to the task node that provides information about the completion code.
				if ( step != current_step ) {
					int error_code = - ( ( (int) graspHousekeepingSlice[index].scriptEngine ) - STEP_FINISHED_ABNORMAL );
					TreeNode^ node = gcnew TreeNode( "Step " + step +  " exited with error code " + error_code );
					node->ForeColor = System::Drawing::Color::Red;
					current_task_leaf->Nodes->Add( node );
					// Keep track of the step number so that we don't repeat for the same step.
					current_step = step;
				}
			}
			else if ( current_task_leaf->Text->StartsWith( "Initiated:" ) && ( graspHousekeepingSlice[index].scriptEngine >= STEP_FINISHED_NORMAL )) {
				// An normal exit has been detected.
				// Add a leaf to the task node that provides information about the completion code.
				if ( step != current_step ) {
					int error_code = ( ( (int) graspHousekeepingSlice[index].scriptEngine ) - STEP_FINISHED_NORMAL );
					TreeNode^ node = gcnew TreeNode( "Step " + step +  " exited normally with code " + error_code );
					node->ForeColor = System::Drawing::Color::Green;
					current_task_leaf->Nodes->Add( node );
					// Keep track of the step number so that we don't repeat for the same step.
					current_step = step;
				}
			}
		}
	}
	// Keep track of where we left off so that we don't re-traverse the entire tree again on the next refresh cycle.
	task_tree_current_index = nHousekeepingSlices;
}

