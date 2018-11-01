///
/// Module:	GraspMMI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		9 January 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2014, 2015, 2016, 2017 PsyPhy Consulting
///

/// GraspMMIDataPlots graphical application.
/// Packets from DEX are placed at a location by the DexGroundMonitorClient module, which is 
///  a separate process that talks to the EPM CLWS via tcp/ip to retrieve the packets.
/// This module monitors the defined data directory for the presence of GRASP realtime data packets
///  and graphs the data on the screen. 

#include "stdafx.h"

#include "GraspMMIGraphsForm.h"
#include "GraspMMIDataPlotsStartup.h"

using namespace GraspMMI;
using namespace Grasp;
using namespace PsyPhy;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	//String^ scriptDirectory = gcnew String( "Scripts\\" );

	// Create the main window.
	GraspMMIGraphsForm^	mainForm = gcnew GraspMMIGraphsForm();
	//mainForm->scriptDirectory = scriptDirectory;

	// Parse the command line arguments.
	//if ( args->Length > 1 ) scriptDirectory = args[1];	// Where to look for the GraspGUI script structure.
	if ( args->Length > 0 ) {
		String^ packetRoot = args[0];	// Where to look for packets written by GripGroundMonitorClient.exe
		// Wait for the first packets.
		GraspMMIDataPlotsStartup^ startupForm = gcnew GraspMMIDataPlotsStartup( packetRoot );
		System::Windows::Forms::DialogResult result = startupForm->ShowDialog();
		if ( result != System::Windows::Forms::DialogResult::Cancel ) {
			mainForm->packetCacheFileRoot = packetRoot;
			Application::Run( mainForm );
		}
	}
	else {
		mainForm->packetCacheFileRoot = nullptr;
		Application::Run( mainForm );
	}

	return 0;
}
