// WinPhantomTypist.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinPhantomTypist.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INPUT KeyDownInput;
INPUT KeyUpInput;

bool french = true;

void send_stroke ( char c ) {
	KeyDownInput.ki.time = 0;
	KeyDownInput.ki.dwExtraInfo = 0;
	KeyDownInput.ki.dwFlags = 0;
	KeyDownInput.ki.wScan = 0;
	KeyDownInput.type = INPUT_KEYBOARD;

	KeyUpInput.ki.time = 0;
	KeyUpInput.ki.dwExtraInfo = 0;
	KeyUpInput.ki.dwFlags = KEYEVENTF_KEYUP;
	KeyUpInput.ki.wScan = 0;
	KeyUpInput.type = INPUT_KEYBOARD;

	KeyDownInput.ki.wVk = c;
	SendInput( 1, &KeyDownInput, sizeof( KeyUpInput ) );
	Sleep( 20 );
	KeyUpInput.ki.wVk = c;
	SendInput( 1, &KeyUpInput, sizeof( KeyUpInput ) );
}

void type_sentence ( char *sentence ) {

	for ( unsigned int letter = 0; letter < strlen( sentence ); letter++ ) {

		printf( "%c\n", toupper( sentence[letter] ) );
		bool shift = false;
		unsigned int c = sentence[letter];
		if ( c == '\n' ) {
			send_stroke( VK_RETURN );
			Sleep( 1000 );
		}
		else {
			if ( c >= 0x41 && c <= 0x5A ) shift = true;
			if ( french && c >= 0x30 && c <= 0x39 ) shift = true;

			c = toupper( c );	
			if ( c == ' ' ) c = VK_SPACE;
			if ( c == '.' ) c = VK_OEM_PERIOD, shift = french;
			if ( c == ',' ) c = VK_OEM_COMMA;
			if ( c == '/' ) c = VK_OEM_2;
			if ( c == ':' ) c = VK_OEM_1, shift = true;
			if ( c == '-' ) c = VK_SUBTRACT;

			if ( shift ) {
				KeyDownInput.ki.wVk = VK_LSHIFT;
				SendInput( 1, &KeyDownInput, sizeof( KeyUpInput ) );
			}

			send_stroke( (char) ( c & 0xFF ) );

			if ( shift ) {
				KeyUpInput.ki.wVk = VK_LSHIFT;
				SendInput( 1, &KeyUpInput, sizeof( KeyUpInput ) );
			}
		}
	}
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.

	char *message_text = 
		"This tool will attempt a clean shutdown of the GRIP hardware." 
		" It will open a 'puTTYtel' telnet window and automatically send messages to GRIP.\n\n"
		" To perform this operation, press 'OK' and wait for the 'puTTYtel' terminal window to terminate.\n\n"
		"Please **DO NOT** click with the mouse or type any characters until the 'puTTYtel' window has dissappeared.";
	int response = MessageBox( NULL, message_text, "DEX Terminator", MB_OKCANCEL );

	if ( response == IDOK ) {

		// Start up the telnet client and connect to DEX.
		system( "Start bin\\puttytel 10.80.12.103" );
		Sleep( 1000 );

		// Output the password for the telnet connection.
		type_sentence( "administrator\n" );
		type_sentence( "dex\n" );
		type_sentence( "cd ..\n" );
		type_sentence( "cd ..\n" );
		type_sentence( "cd Utils\n" );
		type_sentence( "pskill GraspASW\n" );
		Sleep( 1000 );
		type_sentence( "sync\n" );
		Sleep( 3000 );
		type_sentence( "shutdown -s -f -t 2 \n" );
		Sleep( 5000 );
		char *message = 
			"\nAttempt to shutdown GRIP has completed.\n"
			"You may now powerdown the GRIP hardware.";

		MessageBox( NULL, message, "DEX Terminator", MB_OK );

	}

	return( 0 );
}

