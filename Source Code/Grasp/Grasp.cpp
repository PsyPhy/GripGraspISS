// Grasp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
// Interface to the Oculus and Windows
#include "../OculusInterface/OculusViewpoint.h"
#include "../GraspVR/GraspGLObjects.h"
#include "../GraspVR/GraspVR.h"
#include "Grasp.h"

using namespace Grasp;
GraspVR vr;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	vr.Initialize( hInstance );
	vr.DebugLoop();
	vr.Release();
}


