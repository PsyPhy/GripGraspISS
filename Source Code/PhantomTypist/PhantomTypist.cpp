// PhantomTypist.cpp : Defines the entry point for the console application.
//

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

INPUT KeyDownInput;
INPUT KeyUpInput;

void send_stroke ( char c ) {
	KeyDownInput.ki.wVk = c;
	SendInput( 1, &KeyDownInput, sizeof( KeyUpInput ) );
	Sleep( 50 );
	KeyUpInput.ki.wVk = c;
	SendInput( 1, &KeyUpInput, sizeof( KeyUpInput ) );
}

int main( int argc, char *argv[] )

{
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

	for ( int line = 1; line < argc; line++ ) {
		for ( int letter = 0; letter < strlen( argv[line] ); letter++ ) {

			printf( "%c\n", toupper( argv[line][letter] ) );
			bool shift = false;
			char c = argv[line][letter];
			if ( c >= 0x41 && c <= 0x5A ) shift = true;
			c = toupper( c );	
			if ( c == ' ' ) c = VK_SPACE;
			if ( c == '.' ) c = VK_OEM_PERIOD, shift = true;
			if ( c == ',' ) c = VK_OEM_COMMA;
			if ( c == '/' ) c = VK_OEM_2;
			if ( c == ':' ) c = VK_OEM_1, shift = true;

			if ( shift ) {
				KeyDownInput.ki.wVk = VK_LSHIFT;
				SendInput( 1, &KeyDownInput, sizeof( KeyUpInput ) );
			}
			
			send_stroke( c );

			if ( shift ) {
				KeyUpInput.ki.wVk = VK_LSHIFT;
				SendInput( 1, &KeyUpInput, sizeof( KeyUpInput ) );
			}
		}
		send_stroke( VK_RETURN );
	}			
	getchar();
	return 0;
}

