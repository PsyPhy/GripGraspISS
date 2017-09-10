// TweakRigidBodyModel.cpp : main project file.

#include "stdafx.h"
#include "TweakModelForm.h"

using namespace TweakRigidBodyModel;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	fAbortMessageOnCondition( ( args->Length < 3 ), "TweakRigidBodyModel", "Usage: input_file   pitch_angle   output_file" );	

	// Body file is the first command line parameter.
	char *body_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( args[0] ).ToPointer();
	// Second parameter is the number of degrees to tilt in pitch. 
	// Should be 0.0 except when creating tilted version of chest markers for alignment on the chair.
	double tilt_angle = Convert::ToDouble( args[1] );
	// Third parameter is the name of the output file.
	char *output_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( args[2] ).ToPointer();
	Application::Run(gcnew TweakModelForm( body_file, tilt_angle, output_file ));
	System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( body_file ) );
	System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( output_file ) );

	return 0;
}
