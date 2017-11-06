// SendGraspData.cpp : main project file.

#include "stdafx.h"
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
// We need InteropServics in order to convert a String to a char *.
using namespace System::Runtime::InteropServices;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{

	System::Windows::Forms::FolderBrowserDialog^  folder_dialog = gcnew FolderBrowserDialog;

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	char *cmd;

	String^ filename_root;
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--output" ) ) filename_root = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
	}
	int i = filename_root->LastIndexOf( '\\' );
	String^ preselect = filename_root->Substring( 0, i );

	folder_dialog->ShowNewFolderButton = false;
	folder_dialog->RootFolder = System::Environment::SpecialFolder::MyComputer;
	folder_dialog->SelectedPath = preselect;
	folder_dialog->Description = gcnew String( "Select GRASP session to transfer." );
	DialogResult response = folder_dialog->ShowDialog();
	if ( response == DialogResult::OK ) {
		String^ selected_path = folder_dialog->SelectedPath;
		String^ Command = "Executables\\GraspPackData.bat --output=" + selected_path + "\\xxx";
		cmd = (char*)(void*)Marshal::StringToHGlobalAnsi( Command ).ToPointer();
		// Execute the command that we have constructed.
		int return_code = system( cmd );
		// Negative return codes can come back as positive numbers in a unsigned char.
		// So convert larger return codes to negative values.
		if ( return_code > 127 ) return_code -= 128;
		Marshal::FreeHGlobal( IntPtr( cmd ) );
		return return_code;
	}
	else return( 0 );
}
