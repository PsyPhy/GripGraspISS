// MinimizeAll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM ignore ) {
	ShowWindow( hWnd, SW_FORCEMINIMIZE );
	return( TRUE );
}

int main(int argc, char* argv[])
{
	EnumWindows( EnumWindowsProc, NULL );
	return 0;
}

