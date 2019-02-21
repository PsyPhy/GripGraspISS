// GraspTrackerDaemon.cpp : main project file.

#include "stdafx.h"
#include "GraspTrackerDaemonForm.h"

using namespace GraspTrackerDaemon;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Form1^ form = gcnew Form1( args );
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--hide" ) ) form->autohide = true;
		if ( args[i]->StartsWith( "--epm" ) ) {
			// Look for the DEX telemetry server on the EPM LAN,
			// not the ETD LAN.
			form->dexServicesHost = DEX_EPM_SERVER;
			// There is also a specific .ini file when working over the EPM LAN
			// that tells the tracker the IP address of the CodaRTnet server.
			// It should be the same as the dexServicesHost, but they are 
			// two different services and so they could conceivably be different.
			form->codaInitFile = "EpmRTnet.ini";
		}
	}
	Application::Run( form );
	return 0;
}
	