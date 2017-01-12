// GraspMMIHistory.cpp : main project file.

#include "stdafx.h"
#include "../GraspGUI/GraspScripts.h"
#include "GraspMMIHistory.h"

using namespace GraspMMI;
using namespace System::Runtime::InteropServices;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew GraspMMIHistoryForm());
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
			// subjectList[nSubjects] = gcnew Subject( number, token[1], token[2], token[3] );
			System::Windows::Forms::TreeNode^  node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[1] ) + " " + gcnew String( token[2] ) ));
			node->Tag = number;
			ParseSessionFile( node, (gcnew String( "GraspScripts\\" )) + (gcnew String( token[3] )));
			tree->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in subject file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
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
			// protocolList[nProtocols] = gcnew Protocol( number, token[1], token[2] );
			System::Windows::Forms::TreeNode^ node = (gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[1] ) ));
			subject->Nodes->Add( node );
			node->Tag = number;
			ParseProtocolFile( node, (gcnew String( "GraspScripts\\" )) + (gcnew String( token[2] )));
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in session file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );

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
			//taskList[nTasks] = gcnew Task( number, token[1], token[2] );
			System::Windows::Forms::TreeNode^ node = gcnew System::Windows::Forms::TreeNode( gcnew String( token[0] ) + " " + gcnew String( token[2] ));
			node->Tag = number;
			protocol->Nodes->Add( node );
		}
		else fAbortMessageOnCondition( (tokens != 0), "GraspGUI", "Invalid number of tokens (%d) in protocol file.\n\n  %s", tokens, linebuffer );
	}
	fclose( fp );
	Marshal::FreeHGlobal( IntPtr(fn) );
}
