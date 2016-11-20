// GraspGUI.cpp : main project file.

#include "stdafx.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "GraspDesktopForm.h"

using namespace GraspGUI;

[STAThreadAttribute]
int main( array<System::String ^> ^args )
{

	String^ packetRoot;
	if ( args->Length > 0 ) packetRoot = args[0];
	else packetRoot = gcnew String( "GripPackets" );

	bool run_mmi = true;
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it.
	if ( run_mmi ) {
		GraspMMI^ mmi = gcnew GraspMMI();
		mmi->packetRoot = packetRoot;
		Application::Run( mmi );
	}
	else {
		GraspDesktop^ gui = gcnew GraspDesktop();
		Application::Run( gui );
	}
	return 0;
}



