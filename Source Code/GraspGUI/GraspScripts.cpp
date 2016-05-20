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
	while ( tkn && n < MAX_TOKENS - 1 ) {
		/* Skip to first non-white character. */
		while ( iswhite( *tkn ) && *tkn ) tkn++;
		/* Find the next delimiter */
		for ( chr = tkn; *chr; chr++ ) {
			if ( *chr == ';' ) break;
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
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	protocolListBox->Items->Clear();
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 3 ) {
			fAbortMessageOnCondition( nSubjects == MAX_PROTOCOLS, "GraspGUI", "Too many items (n > %d) in session file %s.", MAX_SESSIONS, fn );
			int number;
			sscanf( token[0], "%d", &number );
			protocolList[nProtocols] = gcnew Protocol( number, token[1], token[2] );
			protocolListBox->Items->Add( protocolList[nProtocols]->name );
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
		else if ( tokens == 4 ) {
			fAbortMessageOnCondition( nSubjects == MAX_TASKS, "GraspGUI", "Too many items (n > %d) in protocol file %s.", MAX_PROTOCOLS, fn );
			int number;
			sscanf( token[0], "%d", &number );
			taskList[nTasks] = gcnew Task( number, token[1], token[2], token[3] );
			taskListBox->Items->Add( taskList[nTasks]->name );
			nTasks++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in protocol file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}

void GraspDesktop::ParseInstructionFile( String ^filename ) {

	static char linebuffer[10240];
	int tokens;
	char *token[MAX_TOKENS];
	// Converst the String filename into a char *.
	// Don't forget to free it when exiting.
	char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	FILE *fp = fopen( fn, "r" );
	fAbortMessageOnCondition( !fp, "GraspDesktop", "Error opening %s for reading.", fn );
	nPages = 0;
	while ( fgets( linebuffer, sizeof( linebuffer ), fp ) ) {
		OutputDebugString( linebuffer );
		tokens = ParseLine( token, linebuffer );
		if ( token[0][0] == '#' ) fOutputDebugString( "Comment: %s\n", linebuffer );
		else if ( tokens == 2 ) {
			fAbortMessageOnCondition( nPages == MAX_PAGES, "GraspGUI", "Too many items (n > %d) in instruction file %s.", MAX_PAGES, fn );
			int number;
			sscanf( token[0], "%d", &number );
			pageList[nPages] = gcnew Page( number, token[1] );
			nPages++;
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in instruction file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}

void GraspDesktop::RunPages( void ) {
	currentPage = 0;
	nextButton->Enabled = true;
	previousButton->Enabled = false;
	instructionViewer->Navigate( "C:/Users/Joe/Desktop/GRASPonISS/Instructions/" + pageList[currentPage]->file );
}


