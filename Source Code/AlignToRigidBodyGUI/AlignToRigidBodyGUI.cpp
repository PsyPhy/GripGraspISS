// AlignToRigidBodyGUI.cpp : main project file.

#include "stdafx.h"
#include "SingleObjectAlignForm.h"
#include "MultiObjectAlignForm.h"

using namespace AlignToRigidBodyGUI;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	String ^marker_file = args->Length >= 4 ? args[3] : gcnew String( "CodaAlignmentMarkerData.mrk" );
	String ^alignment_file = args->Length >= 3 ? args[2] : gcnew String( "CodaAlignment.aln" );
	String ^model_file = args->Length >= 2 ? args[1] : gcnew String( "Bdy\\CADHand.bdy" );

	// Create the main window and run it
	Application::Run(gcnew SingleObjectForm( model_file, alignment_file, marker_file ));
	// Create the main window and run it
	Application::Run(gcnew MultiObjectAlignForm());
return 0;
}
