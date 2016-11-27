// GraspTelemetryDaemon.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"
#include "../DexServices/DexServices.h"

using namespace GraspTelemetryDaemon;

#define MAX_SLICES	(24*60*60*10)
GraspRealtimeDataSlice graspDataSlice[MAX_SLICES];

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Read the cached packets.
	// This is here temporarily just for testing.


	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}
