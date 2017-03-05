// GraspMMI.cpp : main project file.

#include "stdafx.h"
#include "../GraspGUI/GraspDesktopForm.h"
#include "GraspMMIMirror.h"

using namespace GraspGUI;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	String^ packetRoot;
	if ( args->Length > 0 ) packetRoot = args[0];
	else packetRoot = gcnew String( "GraspPackets" );

	// Create the main window and run it
	GraspMMIMirror^ mmi = gcnew GraspMMIMirror();
	mmi->packetRoot = packetRoot;
	Application::Run( mmi );
	return 0;
}
