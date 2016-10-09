// GiveFocusDamnIt.cpp : Defines the entry point for the console application.
//

#include "Windows.h"
#include "../Useful/WindowFocus.h"
#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"

#define MAX_RETRIES 10

int main(int argc, char *argv[])
{

	fAbortMessageOnCondition( (argc < 2), "GiveFocus", 
		"GiveFocus requires the name of a window as a command-line argument." );

	char *window_name = argv[1];
	HWND hwnd;
	int tries;
	for ( tries = 0; tries < MAX_RETRIES; tries++ ) {
		if ( (hwnd = FindWindow( NULL, window_name )) ) break;
		Sleep( 200 );
	}
	fAbortMessageOnCondition( (tries >= MAX_RETRIES), "GiveFocus",
		"Window named %s not found after %d retries.", window_name, tries );

	GiveExclusiveFocus( hwnd );

	return 0;
}

