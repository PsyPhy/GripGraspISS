// GraspScripts.cpp
//

#include "stdafx.h"
#include "GraspDesktopForm.h"
#include "GraspScripts.h"

using namespace GraspGUI;
// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

#define PARSER_BUFFERS	16
#define iswhite( c ) ( c <= 0x20 || c >= 0x7f )

int GraspDesktop::ParseLine( char *token[MAX_TOKENS], char *line ) {

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

void GraspDesktop::ParseSubjectFile( String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	subjectListBox->Items->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 4 ) {
			fAbortMessageOnCondition( nSubjects == MAX_SUBJECTS, "GraspGUI", "Too many subjects (n > %d) in subject file %s.", MAX_SUBJECTS, fn );
			int number;
			sscanf( token[0], "%d", &number );
			subjectList[nSubjects] = gcnew Subject( number, token[1], token[2], token[3] );
			subjectListBox->Items->Add( subjectList[nSubjects]->name );
			nSubjects++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in subject file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
}

void GraspDesktop::ParseSessionFile( String^ filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Convert the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	protocolListBox->Items->Clear();
	nProtocols = 0;
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 3 ) {
			fAbortMessageOnCondition( nProtocols == MAX_PROTOCOLS, "GraspGUI", "Too many items (n > %d) in session file %s.", MAX_SESSIONS, fn );
			int number;
			sscanf( token[0], "%d", &number );
			protocolList[nProtocols] = gcnew Protocol( number, token[1], token[2] );
			protocolListBox->Items->Add( protocolList[nProtocols]->number + " " + protocolList[nProtocols]->name );
			nProtocols++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in session file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );

}

void GraspDesktop::ParseProtocolFile( String ^filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	taskListBox->Items->Clear();
	nTasks = 0;
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens > 2 || tokens < MAX_TOKENS ) {
			fAbortMessageOnCondition( nSubjects == MAX_TASKS, "GraspGUI", "Too many items (n > %d) in protocol file %s.", MAX_PROTOCOLS, fn );
			int number;
			sscanf( token[0], "%d", &number );
			taskList[nTasks] = gcnew Task( number, token[1], token[2] );
			if ( !strcmp( token[1], "SCRIPT" ) ) {
				// Record the info about the script file.
				taskList[nTasks]->task_file = gcnew String( token[3] );
			}
			else {
				// Create a record of a single-step task.
				// This means recording all the information that defines a step.
				taskList[nTasks]->isolated_step = gcnew Step();
				if ( !strcmp( token[1], "INSTRUCTION" ) ) {
					taskList[nTasks]->isolated_step->type = "INSTRUCTION";
					taskList[nTasks]->isolated_step->instruction = gcnew String( token[3] );
				}
				else if ( !strcmp( token[1], "COMMAND" ) ) {
					taskList[nTasks]->isolated_step->type = "COMMAND";
					taskList[nTasks]->isolated_step->command = gcnew String( token[3] );
					if ( tokens < 5 ) taskList[nTasks]->isolated_step->ready = gcnew String( "StepReady.html" );
					else taskList[nTasks]->isolated_step->ready = gcnew String( token[4] );
					if ( tokens < 6 ) taskList[nTasks]->isolated_step->running = gcnew String( "StepRunning.html" );
					else taskList[nTasks]->isolated_step->running = gcnew String( token[5] );
					if ( tokens < 7 ) {
						taskList[nTasks]->isolated_step->exit = gcnew array<String ^>( 2 );
						taskList[nTasks]->isolated_step->exit[0] = gcnew String( "StepNormalFinish.html" );
						taskList[nTasks]->isolated_step->exit[1] = gcnew String( "StepErrorFinish.html" );
					}
					else {
						taskList[nTasks]->isolated_step->exit = gcnew array<String ^>( tokens - 5 );
						for ( int i = 6, j = 0; i < tokens; i++, j++ ) taskList[nTasks]->isolated_step->exit[j] = gcnew String( token[i] );
					}
				}
			}
			taskListBox->Items->Add( taskList[nTasks]->number + " " + taskList[nTasks]->name );
			nTasks++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in protocol file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}

void GraspDesktop::ParseTaskFile( String ^filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );

	nSteps = 0;
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( tokens == 0 )  fOutputDebugString( "Null string (no tokens)\n" );
		else if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens > 2 || tokens < MAX_TOKENS ) {
			fAbortMessageOnCondition( nSteps == MAX_STEPS, "GraspGUI", "Too many items (n > %d) in task file %s.", MAX_STEPS, fn );
			stepList[nSteps] = gcnew Step();
			int number;
			sscanf( token[0], "%d", &number );
			stepList[nSteps]->number = number;
			stepList[nSteps]->type = gcnew String(  token[1] );
			if ( !stepList[nSteps]->type->CompareTo( "INSTRUCTION" ) ) stepList[nSteps]->instruction = gcnew String( token[2] );
			else if ( !stepList[nSteps]->type->CompareTo( "COMMAND" ) ) {
				stepList[nSteps]->command = gcnew String( token[2] );
				stepList[nSteps]->ready = gcnew String( token[3] );
				stepList[nSteps]->running = gcnew String( token[4] );
				stepList[nSteps]->exit = gcnew array<String ^>( tokens - 4 );
				for ( int i = 5, j = 0; i < tokens; i++, j++ ) stepList[nSteps]->exit[j] = gcnew String( token[i] );
			}
			else fAbortMessage( "GraspGUI", "Unrecognized step type (%s) in task file %s.", token[1], fn );
			nSteps++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in task file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}

void GraspDesktop::previousButton_Click(System::Object^  sender, System::EventArgs^  e) {
	if ( currentStep > 0 ) {
		currentStep--;
		ShowStep();
	}
	if ( currentStep == 0 ) previousButton->Enabled = false;
}

void GraspDesktop::nextButton_Click(System::Object^  sender, System::EventArgs^  e) {
	if ( currentStep >= nSteps - 1 ) {
		// End of this set of instructions.
		SelectNextTask();
	}
	else {
		currentStep++;
		previousButton->Enabled = true;
		ShowStep();
	}
}

void GraspDesktop::skipButton_Click(System::Object^  sender, System::EventArgs^  e) {
	if ( verifyNext ) {
		System::Windows::Forms::DialogResult response;
		response = MessageBox( "Are you sure you want to continue without executing step?", "Grasp@ISS", MessageBoxButtons::YesNo );
		if ( response == System::Windows::Forms::DialogResult::No ) return;
		normalNavigationGroupBox->Visible = true;
		normalNavigationGroupBox->Enabled = true;
	}
	nextButton_Click( sender, e );
}

void GraspDesktop::SelectNextTask ( void ) {
	if ( taskListBox->SelectedIndex >= nTasks - 1 ) {
		MessageBox( "Protocol Completed.\nLogging off.",   "GRASP@ISS", MessageBoxButtons::OK );
		taskListBox->Items->Clear(); currentTask = -1;
		protocolListBox->Items->Clear(); currentProtocol = -1;
		subjectListBox->SelectedIndex = -1; currentSubject = -1;
	}
	else {
		taskListBox->SelectedIndex = taskListBox->SelectedIndex + 1;
	}
}

void GraspDesktop::ShowStep( void ) {
	stepCounterTextBox->Text = (currentStep+1).ToString() + "/" + nSteps.ToString();
	// If we are doing a ShowStep, then make sure that the navigation buttons are enabled.
	normalNavigationGroupBox->Visible = true;
	normalNavigationGroupBox->Enabled = true;
	// Previous button is only active if we are not in the first step.
	previousButton->Enabled = (currentStep > 0);
	previousButton->Visible = true;
	// Hide all the navigation buttons by default.
	// The appropriate ones will then be rendered visible below.
	errorNavigationGroupBox->Visible = false;
	errorNavigationGroupBox->Enabled = false;
	commandNavigationGroupBox->Visible = false;
	commandNavigationGroupBox->Enabled = false;
	// We have to use a indirect method to launch an external command to allow
	// the event loop to run appropriately. The cueStepCommand tells the event
	// loop to execute a command on the next loop. The Execute button will set this to
	// true at the appropriate time, when a command has been cued, but by default it should be false.
	cueStepCommand = false;
	// Handle the step definition, depending on whether it is a command or an instruction.
	if ( !stepList[currentStep]->type->CompareTo( "INSTRUCTION" ) ) {
		// Show the instruction.
		instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->instruction );
		// No need to ask for verification if user tries to skip to the next page.
		verifyNext = false;
	}
	else {
		// Enable the "Execute", "Skip" and "Previous" buttons.
		commandNavigationGroupBox->Visible = true;
		commandNavigationGroupBox->Enabled = true;
		// Hide the normal navigation buttons.
		normalNavigationGroupBox->Visible = false;
		normalNavigationGroupBox->Enabled = false;
		// Ask for verification if user tries to skip to the next page before executing the command.
		verifyNext = true;
		// Show the 'ready' page for the command.
		instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->ready );
	}
}

void GraspDesktop::startButton_Click(System::Object^  sender, System::EventArgs^  e) {
	// Disable the navigation buttons while we are running.
	// This is perhaps not necessary, as the event loop will stop while the command is running.
	commandNavigationGroupBox->Enabled = true;
	// The user has executed the command, so we don't need to ask for verification any longer.
	verifyNext = false;
	// Show the 'running' page.
	instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->running );
	// Tell the system to run the command when the page has finished loading.
	cueStepCommand = true;
}

// Trigger commands as needed after allowing the html viewer to fully load files.
void GraspDesktop::instructionViewer_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e) {
		// Make sure that the windows have refreshed  before executing the command.
		Refresh();
		Application::DoEvents();
		if ( cueStepCommand ) {
			// Don't trigger again on the next DocumentCompleted event.
			cueStepCommand = false;
			// Run the command.
			// For the moment, we don't actually run the command. We pass the command string to TaskProcessUnitTester.exe to simulate running the command.
			char *cmd = (char*)(void*)Marshal::StringToHGlobalAnsi( execDirectory + "TaskProcessUnitTester.exe " + stepList[currentStep]->command ).ToPointer();
			int return_code = system( cmd );
			Marshal::FreeHGlobal( IntPtr( cmd ) );
			// Map exit codes to the results pages defined in the step definition.
			// The absolute value of the code determines which page is shown, i.e.
			// codes -1 and 1 will show the same page, -2 and 2 the same page, etc,
			// but the behavior will be that of a normal exit if positive and an 
			// error exit if negative.
			int exit_possibilities = stepList[currentStep]->exit->Length;
			// If the exit code is higher than the highest defined return page, default to the highest defined return page.
			int exit_choice = min( exit_possibilities - 1, abs( return_code ) );
			// Hide the buttons used to initiate a command.
			commandNavigationGroupBox->Enabled = false;
			commandNavigationGroupBox->Visible = false;
			// If the return code was negative, display the error code and show the navigation buttons that 
			// allow to proceed appropriately in the case of an error exit.
			if ( return_code < 0 ) {
				errorCodeTextBox->Text = return_code.ToString();
				errorNavigationGroupBox->Enabled = true;
				errorNavigationGroupBox->Visible = true;
				// Make sure that the normal navigation buttons are hidden.
				// I think they already are, but to be sure I do it again.
				normalNavigationGroupBox->Visible = false;
				normalNavigationGroupBox->Enabled = false;
			}
			else {
				normalNavigationGroupBox->Visible = true;
				normalNavigationGroupBox->Enabled = true;
			}

			// Show the corresponding page.
			instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->exit[exit_choice] );
		}
}

// Retry the step, if desired, in the case of error exit.
void GraspDesktop::retryButton_Click(System::Object^  sender, System::EventArgs^  e) {
	// Hide the error exit navigation buttons.
	errorNavigationGroupBox->Enabled = false;
	errorNavigationGroupBox->Visible = false;
	// Get ready to restart the same step.
	ShowStep();
}
// Ignore an error exit.
void GraspDesktop::ignoreButton_Click(System::Object^  sender, System::EventArgs^  e) {
	// Hide the error exit navigation buttons.
	errorNavigationGroupBox->Enabled = false;
	errorNavigationGroupBox->Visible = false;
	// Move on to the next page as if nothing happened.
	nextButton_Click( sender, e );
}
// Restart the entire task on an error exit, if desired.
void GraspDesktop::restartButton_Click(System::Object^  sender, System::EventArgs^  e) {
	// Hide the error exit navigation buttons.
	errorNavigationGroupBox->Enabled = false;
	errorNavigationGroupBox->Visible = false;
	// Restart the entire task.
	currentStep = 0;
	ShowStep();
}


void GraspDesktop::ShowLogon( void ) {
	stepCounterTextBox->Text = "";
	stepHeaderTextBox->Text = "";
	errorNavigationGroupBox->Visible = false;
	errorNavigationGroupBox->Enabled = false;
	commandNavigationGroupBox->Visible = false;
	commandNavigationGroupBox->Enabled = false;
	normalNavigationGroupBox->Visible = false;
	normalNavigationGroupBox->Enabled = false;
	instructionViewer->Navigate( instructionsDirectory + "GraspWelcome.html" );
}

	// system( "bin\\WinSCP.com /command \"open ftp://administrator:dex@10.80.12.103\" \"cd DATA1\" \"cd DATA\" \"cd glog\" \"ls\" \"put scripts\\*\" \"exit\" & pause" );
